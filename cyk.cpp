/*
THIS PROJECT WAS TESTED IN LINUX OS (UBUNTU).
TO RUN THE CODE FILES: 
	-STEP 1: open terminal and change directory to the folder of files
	-STEP 2: type: g++ -Wall *.cpp
	-STEP 3: type: ./a.out
	-STEP 4: enter a string to test. Example: baaba will output true and the steps of checking the string

**NOTE: IF IT SHOWS THE ERROR THAT YOUR LINUX DOES NOT HAVE G++ THEN INSTALL THEM BY: sudo apt-get install g++ USING TERMINAL 

This is the main method of the cyk algorthms, this file read the input from a txt file and send them to the header file.
*/

#include <fstream>
#include <iostream>
 
#include "grammar.hpp"
#include "cyk.hpp"
 
using namespace std;

int main()
{
	//reading the grammar from a txt file
    	string filename = "cyk.txt";
    	ifstream ifs(filename);
    
	//success of reading the file
    	if(ifs) 
    	{
        	AUTOMATA::grammar grammar(ifs);       
        
        	string sentences;
        	cout << "Enter a sentence: ";
        	cin >> sentences;
        
        	cout << endl;
       	 
       		int siz = sentences.size();
        
        	string sentence[siz];
        
        	for(int i = 0; i < siz; i++)
        	{
            		sentence[i] = sentences[i];
        	}
        
        	const size_t len = sizeof(sentence) / sizeof(sentence[0]);
        	bool success = AUTOMATA::cyk_parser(grammar).parse(sentence, sentence + len, cout);
        	cout << "Success: " << boolalpha << success << '\n';
    	}
    	else 
    	{
        	cerr << "Error opening the " << filename << " for working\n";
    	}
}
