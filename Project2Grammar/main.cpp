/*
Computer Systems II - Project 2 - Beautified Parser
Author : Max Hoffman
Date : 1/28/2024 - 2/11/2024
Purpose : Create a parser to beautify an .txt file input and output the beautified file if it is correct syntax
Based off the provided code in Project 1, the reading in part of main was provided as well as part of the token code from the last project.

*/

#include <iostream>
#include <fstream>

#include "Grammar.hpp"
#include "Token.hpp"

using namespace std;

int main(int argc, char *argv[])
{
  //same code from provided main to set up our file stream
  // check to make sure an input file was specified. 
  if (argc!=2)
    {
      cerr << "USAGE: " << argv[0] << " <file>" << endl;
      return -1;
    }

  // open input file
  ifstream ifile(argv[1]);
  // if open was not successful, let user know. 
  if (!ifile) {
      cerr << "ERROR: Could not open file:\"" << argv[1] << "\"" <<endl;
      return -1;
  }

  string text = program(ifile);//this stores the string from the output of the grammar code

  if(checkForErrors()) {//we call this method to check if there were any errors
    //we had an error
    cout << "# UNsuccessful program ===========================" << endl;
  }
  else {//no errors in the input file
    cout << "# Successful program ===========================" << endl;
    cout << text << endl;//just print out the result of the program call!
    //I had a tough time desciding on which method to beautify the code. I went back and forth between using the 
    //token methods in main and parsing it that way or looping through the string index by index. But I realized 
    //I was making this harder than it had to be and doing it in the grammar made it easier
  }
  return 0;
}
