/*
Computer Systems II - Project 2 - Beautified Parser
Author : Max Hoffman
Date : 1/28/2024 - 2/11/2024
Purpose : Create a parser to beautify an .txt file input and output the beautified file if it is correct syntax

Similar layout to our in-class examples of grammar
*/
#ifndef _GRAMMAR_HPP_
#define _GRAMMAR_HPP_

#include <fstream>
#include <iostream>


//used to check if we want to print out the code beautified or not
bool checkForErrors();


//all of the recursive methods for the grammar
std::string program(std::ifstream & is);
std::string compound(std::ifstream & is);
std::string stmtlist(std::ifstream & is);
std::string stmt(std::ifstream & is);
std::string exprlist(std::ifstream & is);
std::string expr(std::ifstream & is);
std::string declaration(std::ifstream & is);
std::string type(std::ifstream & is);
std::string simpexpr(std::ifstream & is);
std::string factor(std::ifstream & is);
std::string term(std::ifstream & is);
std::string idlist(std::ifstream & is);

#endif