/*******************************************
 * File: Token.cpp                         *
 * Author: S. Blythe                       *
 * Date: 12/2023                           *
 * PURPOSE: implementation for Token       *
 *******************************************/

#include "Token.hpp"

#include <fstream>
#include <iomanip>

using namespace std;

// the promised global for string equivalents of TokenType enumeration
string TokStr[]=
{ "ERROR", "EOF_TOK", "NUM_INT", "NUM_REAL", "ADDOP", "MULOP", "ID", "RELOP", "ASSIGNOP", "LPAREN", "RPAREN",  "SEMICOLON",  "LBRACK", "RBRACK", "COMMA", "AND", "OR", "INTEGER", "FLOAT", "WHILE", "IF", "THEN", "ELSE", "VOID", "BEGIN", "END"};

// This is a "list" of the keywords. Note that they are in the same order
//   as found in the TokenType enumaration. 
static string reserved[]={"int" , "float", "while", "if", "then", "else", "void", "begin", "end" };


static int **DFA_Table=nullptr;
int lineNum = 1;

/******************************************************
 *  just prints out the info describing this Token    *
 *    to specified stream                             *
 *                                                    *
 *   os  - the stream to add the Token to             *
 *                                                    *
 *   returns: the updated stream                      *
 ******************************************************/
ostream&
Token::print(ostream& os) const
{
  os
     << "{ Type:"   << left << setw(10) << TokStr[_type] 
     << " Value:"   << left << setw(10) << _value
     << " Line Number:" << _line_num
     << " }";
  return os;
}

/******************************************************
 *  Fills in information about this Token by reading  *
 *    it from specified input stream                  *
 *                                                    *
 *   is  - the stream to read the Token from          *
 *                                                    *
 *   returns: nothing                                 *
 *                                                    *
 *     **** YOU MUST CODE THIS !!!!!! ****            *
 ******************************************************/


//same method from the in class example we did used to switch between what is outputted in the DFA and the token types that correspond with that number
const TokenType stype(int num) {
  
  switch(num)
  {
    case 1:
      return ID;
    case 2:
      return NUM_INT;
    case 4:
      return NUM_REAL;
    case 5:
      return ADDOP;
    case 6:
      return MULOP;
    case 7:
      return RELOP; 
    case 8:
      return RELOP; 
    case 9:
      return ASSIGNOP; 
    case 10:
      return LPAREN; 
    case 11:
      return RPAREN; 
    case 13:
      return AND; 
    case 15:
      return OR; 
    case 16:
      return SEMICOLON; 
    case 17:
      return LBRACK; 
    case 18:
      return RBRACK; 
    case 19:
      return COMMA; 
    default: 
      return ERROR;
  }
}
//used to check if the token's value is a key word
enum TokenType keyWord(string num) {
  if(num == "int") {
    return INTEGER;
  }
  else if(num == "float") {
    return FLOAT;
  }
  else if(num == "while") {
    return WHILE;
  }
  else if(num == "if") {
    return IF;
  }
  else if(num == "then") {
    return THEN;
  }
  else if(num == "else") {
    return ELSE;
  }
  else if(num == "void") {
    return VOID;
  }
  else if(num == "begin") {
    return BEGIN;
  }
  else if(num == "end") {
    return END;
  }
  else {
    return ERROR;
  }
}

void Token::get(istream &is) {
  if(!is) {
    this->_type = EOF_TOK;
    return;
  }

  //build DFA
  if(!DFA_Table) {//table doesnt exist then make it

    DFA_Table = new int *[20];
    for(int state=0; state<=19; state++) {
      DFA_Table[state] = new int[256];

      for(int ch=0; ch < 256; ch++) {//set all spots to errors
        DFA_Table[state][ch] = -1;
      }
    }
    //fill in final states, these are all of the 'easier' ones
    DFA_Table[0][(int) '+'] = 5;
    DFA_Table[0][(int) '-'] = 5;
    DFA_Table[0][(int) '*'] = 6;
    DFA_Table[0][(int) '/'] = 6;
    DFA_Table[0][(int) '<'] = 7;
    DFA_Table[0][(int) '>'] = 7;
    DFA_Table[7][(int) '='] = 8;
    DFA_Table[9][(int) '='] = 8;
    DFA_Table[0][(int) '='] = 9;
    DFA_Table[0][(int) '('] = 10;
    DFA_Table[0][(int) ')'] = 11;
    DFA_Table[0][(int) '&'] = 12;
    DFA_Table[12][(int) '&'] = 13;
    DFA_Table[0][(int) '|'] = 14;
    DFA_Table[14][(int) '|'] = 15;
    DFA_Table[0][(int) ';'] = 16;
    DFA_Table[0][(int) '['] = 17;
    DFA_Table[0][(int) ']'] = 18;
    DFA_Table[0][(int) ','] = 19;
 
    //the states below require loops because they can have multiple values in the 2nd spot for the DFA Table
    //this is for IDs

    //these two for loops are for the first arrow from 0 to 1 where it can be an alpha so any upper or lowercase letter
    //we use the same strategy we used in class for iterating through the ascii table and putting a one in each spot of the table;
    for(char ch = 'a'; ch <= 'z'; ch++) {
      DFA_Table[0][(int) ch] = 1;
    }
    for(char ch='A'; ch <= 'Z'; ch++) {
      DFA_Table[0][(int) ch] = 1;
    }
    //now we deal with the two loops going out of state 1
    //the first two for loops are alpha loop, the second is a digit loop
    for(char ch = 'a'; ch <= 'z'; ch++) {
      DFA_Table[1][(int) ch] = 1;
    }
    for(char ch='A'; ch <= 'Z'; ch++) {
      DFA_Table[1][(int) ch] = 1;
    }
    //digit loop
    for(char ch='0'; ch <= '9'; ch++) {
      DFA_Table[1][(int) ch] = 1;
    }
    //last bit on the diagram is the digit/number states
    for(char ch='0'; ch <= '9'; ch++) {
      DFA_Table[0][(int) ch] = 2;
    }
    for(char ch='0'; ch <= '9'; ch++) {
      DFA_Table[2][(int) ch] = 2;
    }
    //float or double number part of the diagram now
    DFA_Table[2][(int) '.'] = 3;
    for(char ch='0'; ch <= '9'; ch++) {
      DFA_Table[3][(int) ch] = 4;
    }
    for(char ch='0'; ch <= '9'; ch++) {
      DFA_Table[4][(int) ch] = 4;
    }
  }
  //same code from in class example to skip spaces we just add to a global variable then set this tokens line number to that number
  char ch;
  ch = is.get();
  
  while(isspace(ch)) {
    if(ch == '\n') {
      this->_line_num = lineNum++;
    }
    ch = is.get();
  }
  
  if(!is) {//if this took me to the end of the file then return
    this->_line_num = lineNum++;
    this->_type = EOF_TOK;//remember to set type to end of file
    return;
  }
  
  is.putback(ch); 
  string val= ""; 

  //check for final state
  int curr = 0;
  int prev = -1;//# of state
  while(curr != -1) {
    
    //get char from stream
    ch = is.get();
    
    //skip over any comment code
    if(ch == '#') {
      repeat://I know this is really bad practice, first time ive used a goto label in c++ , I did not want to make a standalone method
      while(ch != '\n') {
        if(!is) {
          this->_type = EOF_TOK;
          this->_line_num = lineNum++;
          return;
        }
        ch = is.get();
      }

      this->_line_num = lineNum++;
      ch = is.get();
      
      if(ch == '#') {//if we are still getting comments
        goto repeat;
      }
      while(isspace(ch)) {//figured out I have to remember to skip over spaces here, this was causing a ton of issues where it would error 
      //after a comment and a new line, this also stops it from not recognizing a comment when there is a space after the hashtag
        if(ch == '\n') {
          this->_line_num = lineNum++;
        }
        ch = is.get();
      }
       if(ch == '#') {//if we are still getting comments after all those spaces lets go back to skipping comments
        goto repeat;
      }
      if(!is) {
        this->_type = EOF_TOK;
        return;
      }
    }
    prev = curr;
    curr = DFA_Table[curr][(int) ch];
    if(curr != -1) {//sets the value
      val += ch;
    }
    
    if(!is) {//if we are at the end of the file we need to get out
      this->_type = EOF_TOK; 
      break;//the reason we break here instead of returning is because we might have a token at the end of the file with no enter after it so if we were to return we would lose the last token
    }
  }
  bool isFound = false;
  if(curr == -1 && prev == 0) {//if curr is -1 and prev is equal to 0 that means we have an error since we didnt even go through the above while loop
    if(ch == '\n') {//if we had a new line we just need to skip all the code below, this should not be interpretted as an error
       goto skip;
    }
    this->_line_num = lineNum++;
    this->_type = ERROR;
    return;
    
  }

  //check to see if we have a keyword
  for(int i = 0; i < 9; i++) {
    if(reserved[i] == val) {
      isFound = true;
      break;
    }
  }

  //create tokentype and assign it depending on if it is a keyword or not
  TokenType type;
  if(isFound) {
    type = keyWord(val);
  }
  else {
     type = stype(prev);
  }

  //set all of the other values
  this->_value = val;
  this->_type = type;
  this->_line_num = lineNum;

  skip:
  if(is) {
    is.putback(ch);
  }
  
}
