/**
 * Computer Systems II - Project 3
 * Author: Max Hoffman
 * Date: 3/18/2024 - 3/24/2024
 * Purpose: Use the grammar methods to look through an input file grabbing the different command names and executing them as necessary
*/

#ifndef _GRAMMAR_HPP_
#define _GRAMMAR_HPP_
#include <fstream>
#include <iostream>


bool checkPass();
void dump();
//all of our grammar methods
std::string prog(std::istream &is, int size);
std::string slist(std::istream &is, std::string incomingVal);
std::string stmt(std::istream &is);
std::string rhs(std::istream &is);


#endif