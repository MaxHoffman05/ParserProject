/**
 * Computer Systems II - Project 3
 * Author: Max Hoffman
 * Date: 3/18/2024 - 3/24/2024
 * Purpose: Use the grammar class to look through an input file grabbing the different command names and executing them as necessary
 * Main.cpp gets the user input and calls the grammar program method
*/

#include "Grammar.hpp"
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) { 


    //get our inputs from the user first the size then the name of the input file
    int size;
    string fileN;
    cout << "Please enter the initial freelist (heap) size: " << endl;
    cin >> size;
    cout << "Please enter the name of the input file: " << endl;
    cin >> fileN;
   
    ifstream ifile(fileN);
    if (!ifile) {
        cerr << "ERROR: Could not open file:\"" << argv[1] << "\"" <<endl;
        return -1;
    }
    else {//call prog() which will call all the other grammar methods
        string validGrammar = prog(ifile, size);
        //I had all of my grammar methods return strings for debugging so I could print out the string here if needed
    }
    return 0;
}