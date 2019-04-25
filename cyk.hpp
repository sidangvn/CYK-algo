/*
This is the cyk header file, this file will call the input from the cyk.cpp file and then use them to check wether or not the input string 
is in the grammar given in the txt file. The grammar algorithms for this project is in the grammar.hpp file.
When ever the algorithms catch a right input, it will store them in a table (cyk table). 
To know that the string is in the grammar or not, we will check the top cell of the table, if it has the variable S, then the input string is in the grammar and vice versa.
*/

#ifndef CYK_HPP
#define CYK_HPP
 
#include <vector>
#include <set>
#include <iostream>
#include <iterator>
 
#include "grammar.hpp"
 
using namespace std;

namespace AUTOMATA
{
   class cyk_parser
   {
      public:
      		cyk_parser(const AUTOMATA::grammar_CYK& grammar_CYK) : grammars(grammar_CYK){}
 
         	template <class IP> /*Template method*/
         	bool parse(IP bg, IP stop)
         	{
            		return parse(bg, stop, nullptr);
         	}
 
         	template <class IP> /*Template method*/
         	bool parse(IP bg, IP stop, ostream& cyk_out)
         	{
            		return parse(bg, stop, &cyk_out);
         	}
 
      private:
         	template <class IP> /*Template method*/
         	bool parse(IP bg, IP stop, ostream *cyk_out)
         	{
            		vector<string> str; //storage of working string
            		copy(bg, stop, back_inserter(str));
            		const size_t len = str.size();

            		vector<vector<set<string>>> table(len, vector<set<string>>(len));

            		unsigned int i = 0, j, k;
			string a = "____"; //making a table
			string aa = a;
            
            		for(const string& c : str) 
            		{
               			vector<AUTOMATA::cykAlgo::const_ptr> rules_cyk;
               			grammars.get_rules_cyk(c, back_inserter(rules_cyk));
               			for(AUTOMATA::cykAlgo::const_ptr flag : rules_cyk) 
               			{
                  			table[i][i].insert(flag -> left());
               			}
               			i++;
            		}
            
			//checking the string in the grammar
            		for(i = 1; i < len; i++) 
            		{
               			for(j = i; j < len; j++) 
               			{
                  			for(k = j - i; k < j; k++) 
                  			{
                     				vector<pair<string, string>> product;
                     				AUTOMATA::detail::cartesian_product(table[j - i][k].bg(), table[j - i][k].stop(), 
                            			table[k + 1][j].bg(), table[k + 1][j].stop(), back_inserter(product));
                     				vector<AUTOMATA::cykAlgo::const_ptr> rules_cyk;
                     				grammars.get_rules_cyk(product.bg(), product.stop(), back_inserter(rules_cyk));
                    
                     				for(AUTOMATA::cykAlgo::const_ptr flag : rules_cyk) 
                     				{
                        				table[j - i][j].insert(flag -> left());
                     				}
                  			}
               			}
            		}
            
			//output the work to the table
        		if(cyk_out)
        		{
            			for(i = 0; i < len; i++) 
            			{
                			k = 0;
                			for(j = len - i - 1; j < len; j++) 
                			{
						cout << "|";                    
						copy(table[k][j].bg(), table[k][j].stop(), ostream_iterator<string>(*cyk_out, " "));
                	    			++k;
			
                	    			*cyk_out << '\t';
						a += aa; //draw a table
		
                			}
                			*cyk_out << '\n';

					cout << a << endl; //output the table

            			}
	
            			for(const string& c : str) 
            			{
					cout << "   ";
                			*cyk_out << c << '\t';
            			}
            			*cyk_out << '\n' << '\n';
        		}
           
        		return table[0][len - 1].find(grammars.get_start_left()) != table[0][len - 1].stop();
    		}
 
   	private:
      		const AUTOMATA::grammar_CYK& grammars;
	};
 
}
 
#endif
