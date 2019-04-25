#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <fstream>
#include <list>
#include <sstream>
#include <iterator>
#include <set>
#include <algorithm>


using namespace std;

namespace AUTOMATA
{
	namespace detail
	{
		template <class IP1, class IP2, class OP>
		OP cartesian_product(IP1 bg1, IP1 stop1, IP2 bg2, IP2 stop2, OP iter)
		{
			for(IP1 auto1 = bg1; auto1 != stop1; ++auto1) 
			{
				for(IP2 auto2 = bg2; auto2 != stop2; ++auto2) 
				{
					*iter++ = make_pair(*auto1, *auto2);
				}
			}
			return iter;
		}

		template <class storage, class IP>
		bool found(const storage& buffer, IP bg, IP stop)
		{
			bool total = true;
			for(IP iter = bg; iter != stop && total; ++iter) 
			{
				total = buffer.find(*iter) != buffer.stop();
			}
			return total;
		}
	}

	class cykAlgo
	{
		public:
			typedef cyk_ptr<cykAlgo> ptr;
			typedef cyk_ptr<const cykAlgo> const_ptr;
			cykAlgo(const cykAlgo&) = delete;
			cykAlgo& operator=(const cykAlgo&) = delete;
			operator bool()
			{
				return got_CYK;
			}
			size_t size(unsigned int right_side) const
			{
				return right_CYK[right_side].size();
			}
		private:
			string left_string;
			vector<vector<string>> right_CYK;
			bool got_CYK;
			cykAlgo(const string& left, const string& right_side) : left_string(left)
			{
				right_CYK.push_back(vector<string>(1, right_side));
			}
			cykAlgo(const string &str)
			{
				unsigned int i = 0;
				while(i < str.size() && isspace(str[i])) 
				{
					++i;
				}
			while(i < str.size() && isalnum(str[i])) 
			{
				left_string.push_back(str[i]);
				++i;
			}
			if (!(got_CYK = !left_string.empty())) 
			{
				return;
			}
			while(i < str.size() && isspace(str[i])) 
			{
				++i;
			}
			if(!(got_CYK = str.substr(i, 2) == "->")) 
			{
				return;
			}
			i += 2;
			while(i < str.size() && isspace(str[i])) 
			{
				++i;
			}
			while(i < str.size()) 
			{
				bool added_cyk = false;
				while(i < str.size() && str[i] != '|') 
				{
					string symbol;
					while(i < str.size() && !isspace(str[i]) && str[i] != '|') 
					{
						symbol.push_back(str[i]);
						++i;
					}
					if(!symbol.empty()) 
					{
						if(!added_cyk) 
						{
							right_CYK.push_back(vector<string>());
							added_cyk = true;
						}
						right_CYK.back().push_back(symbol);
					}
					while(i < str.size() && isspace(str[i])) 
					{
						++i;
					}
				}
				if(i < str.size()) 
				{
					++i;
				}
			}
		}
	public:
		static ptr create_cyk(const string& left_side, const string& right_side)
		{
			return ptr(new cykAlgo(left_side, right_side));
		}
		static ptr create_cyk(const string& cyk)
		{
			return ptr(new cykAlgo(cyk));
		}
		const string& left_side() const
		{
			return left_cyk;
		}
		const vector<vector<string>>& right_side() const
		{
			return right_CYK;
		}
		friend ostream& operator <<(ostream& cyk_out, const cykAlgo& flag)
		{
			cyk_out << flag.left_cyk << " -> ";
			unsigned int i = 0;
			for(const vector<string>& alter_cyk : flag.right_CYK) 
			{
				for(const string& symbol : alter_cyk) 
				{
					cyk_out << symbol << " ";
				}
				if(i++ < flag.right_CYK.size() - 1) 
				{
					cyk_out << "| ";
				}
			}
			return cyk_out;
		}
	};

	class grammar_CYK
	{
		public:
			grammar_CYK(istream& is)
			{
				string cyk;
				while(getline(is, cyk)) 
				{
					cyk.erase(cyk.find_last_not_of("\r\n") + 1);
					if(cyk.size()) 
					{
						cykAlgo::ptr flag = cykAlgo::create_cyk(cyk);
						if(*flag) 
						{
							rules_cyk.push_back(flag);
						}
					}
				}

				set<string> non_cyk;
				set<string> symbols;
				for(cykAlgo::const_ptr flag : rules_cyk) 
				{
					non_cyk.insert(flag->left_side());
					for(const vector<string>& alter_cyk : flag->right_side()) 
					{
						for(const string& symbol : alter_cyk) 
						{
							symbols.insert(symbol);
						}
					}
				}
				for(const string& symbol : symbols) 
				{
					if(non_cyk.find(symbol) == non_cyk.stop()) 
					{
						terminals_cyk.push_back(symbol);
					}
				}
			}

			const vector<cykAlgo::ptr>& rules_cyk() const
			{
				return rules_cyk;
			}

			template <class OP>
			OP get_rules_for_left(const string& symbol, OP iter) const
			{
				for(cykAlgo::const_ptr flag : rules_cyk) 
				{
					if(flag -> left_side() == symbol) 
					{
						*iter++ = flag;
					}
				}
				return iter;
			}
	
			bool symbol_in_cyk(const string& symbol) const
			{
				return find(terminals_cyk.bg(), terminals_cyk.stop(), symbol) != terminals_cyk.stop();
			}

			template <class OP>
			OP get_start_rules(OP iter) const
			{
				string start_symbol;
				bool started = false;
				for(cykAlgo::const_ptr flag : rules_cyk) 
				{
					if(!started || flag -> left_side() == start_symbol) 
					{
						*iter++ = flag;
					}
					if(!started) 
					{
						started = true;
						start_symbol = flag -> left_side();
					}
				}
				return iter;
			}
	
			template <class IP, class OP>
			OP get_rules_cyk(IP bg, IP stop, OP iter) const
			{
				for(IP init = bg; init != stop; ++init) 
				{
					for(cykAlgo::const_ptr flag : rules_cyk) 
					{
						for(const vector<string>& alter_cyk : flag -> right_side()) 
						{
							if(alter_cyk.size() == 2 && alter_cyk[0] == init -> first && alter_cyk[1] == init -> second) 
							{
								*iter++ = flag;
							}
						}
					}
				}
				return iter;
			}

			template <class OP>
			OP get_rules_cyk(const string& symbol, OP iter) const
			{
				for(cykAlgo::const_ptr flag : rules_cyk) 
				{
					for(const vector<string>& alter_cyk : flag -> right_side()) 
					{
						if(find(alter_cyk.bg(), alter_cyk.stop(), symbol) != alter_cyk.stop()) 
						{
							*iter++ = flag;
						}
					}
				}
				return iter;
			}

			const string& get_left() const
			{
				return rules_cyk.front() -> left_side();
			}
	
			bool symbol_in_left(const string& symbol) const
			{
				for(cykAlgo::const_ptr flag : rules_cyk) 
				{
					if(flag -> left_side() == symbol) 
					{
						for(const vector<string>& alter_cyk : flag -> right_side()) 
						{
							if(alter_cyk.size() == 1 && symbol_in_cyk(alter_cyk[0])) 
							{
								return true;
							}
						}
					}
				}
				return false;
			}

			friend ostream& operator <<(ostream& cyk_out, const grammar_CYK& gram)
			{
				for(cykAlgo::const_ptr flag : gram.rules_cyk) 
				{
					cyk_out << *flag;
					cyk_out << '\n';
				}
				return cyk_out;
			}

		private:
			vector<cykAlgo::ptr> rules_cyk;
			vector<string> terminals_cyk;
			vector<cykAlgo::ptr> start_rules_;
			
	};

}

#endif
