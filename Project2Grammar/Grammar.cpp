/*
Computer Systems II - Project 2 - Beautified Parser
Author : Max Hoffman
Date : 1/28/2024 - 2/11/2024
Purpose : Create a parser to beautify an .txt file input and output the beautified file if it is correct syntax

*/
#include "Grammar.hpp"
#include "Token.hpp"
using namespace std;

bool isError = false;//global variable to check if we have had any errors, wherever there were cerr statements now we just set this variable to false to indicate an error
int spaceCount = 0;//used to keep track of indentation

//this is how we print out the code if there were no errors or if there was errors we dont even print it out
bool checkForErrors() {
    return isError;
}

//simpexpr method: term | term ADDOP simpexpr
string simpexpr(ifstream & is) {
    //we know we first have a term
    string val = term(is);
    int pos = is.tellg();
    
    //check next token for an addop
    Token tok;
    tok.get(is);
    
    //these are for if exprlist() or expr() call this method, I dont know if this is the intended solution but it worked for me
    //all of these keywords or tokens belong in a higher up method in the grammar structure not here so we need to use the unget to go backwards
    if(tok.type() == END || tok.type() == THEN || tok.type() == ELSE || tok.type() == IF || tok.type() == EOF_TOK || tok.type() == RELOP || tok.type() == COMMA || tok.type() == LPAREN || tok.type() == RPAREN || tok.type() == SEMICOLON) {
        is.seekg(pos);
        return val;
    }
    else if(tok.type() == ADDOP) {//recursive case
        return val + " " + tok.value() + " " + simpexpr(is);
    }
    else if(tok.type() == EOF_TOK){//if we reached the end of the file return what we have
        return val;
    }
    else {
        //cerr << "ERROR in simpexpr() : " << tok << endl;
        isError = true;
    } 
}


//exprlist method: expr | expr COMMA exprlist
string exprlist(ifstream & is) {
    string val = expr(is);//both start with an expr()
    
    int pos = is.tellg();
    Token tok;
    tok.get(is);
  

    //check if there is a comma next
    if(tok.type() == COMMA) {
        return val + ", " + exprlist(is);
    }
    else if(tok.type() != ERROR) {
        is.seekg(pos);
        return val;
    }
    else {
        //cerr << "Error in exprlist() token : " << tok << endl;
        isError = true;
    }
}


//expr method: simpexpr | simpexpr RELOP simpexpr 
string expr(ifstream & is) {
    //we know expr() starts with a simpexpr() and could be followed by a relop
    string val = simpexpr(is);
    int pos = is.tellg();

    //get the next token to check if it is a relop
    Token tok;
    tok.get(is);
    
    if(tok.type() == RELOP) {
        return val + " " + tok.value() + " " + simpexpr(is);
    }
    else {
        //base case, we only have one expr() left or this was after a relop, we need to unget the last token
        is.seekg(pos);
        return val;
    }
}


//term method: factor | factor MULOP term
string term(ifstream & is) {
    //they each start with a factor
    string val = factor(is);

    int pos = is.tellg();
    Token tok;
    tok.get(is);//check to see if we have a multiply at next token
 
    if(tok.type() == MULOP) {
        return val + " * " + term(is);//if we do then we call term since there will be another factor
    }
    else {
        is.seekg(pos);
        return val;//base case is that we will have one factor
    }
}


//type method: INTEGER | FLOAT | VOID
string type(ifstream & is) {
    Token typeTok;
    typeTok.get(is);

    //no recursion here, just return the appropriate value
    if(typeTok.type() == INTEGER) {
        return typeTok.value();
    }
    else if(typeTok.type() == FLOAT) {
        return typeTok.value();
    }
    else if(typeTok.type() == VOID) {
        return typeTok.value();
    }

    //if we got here there was an error
    //cerr << "This was not a type called by type() " << typeTok << endl;
    isError = true;
}

//idlist method: ID | ID COMMA idlist
string idlist(ifstream & is) {
    int pos = is.tellg();//save our position, moved this
    Token id;
    id.get(is);//we know this must be a id first
    
    int pos2 = is.tellg();

    //set up the indent
    string toReturn;
    for(int i = 0; i < spaceCount; i++) {
        toReturn += " ";
    }
    if(id.type() == COMMA) {//case 1 is that we have a comma
        return toReturn + id.value() + " " + idlist(is);
    }
    else if(id.type() == ID) {//we just have an id is the 'base case'
        //check to see if there is a comma next
        Token id2;
        id2.get(is);
       
        if(id2.type() == COMMA) {//could be a comma
           
            is.seekg(pos2);
            return id.value() + idlist(is);//if there is a comma next we will do a recursive call but 'unget' the last token which was a comma so the above if statement can complete
        }
        else if(id2.type() == SEMICOLON) {//could be a semicolon
            is.seekg(pos2);
            return toReturn + id.value();
            
        }
        is.seekg(pos);//we need to unget the last get since that was probably a semicolon
        return toReturn + id.value();
    }
    else {
        //cerr << "Error in idlist() expected Id or Comma instead : " << id << endl;
        isError = true;
    }

}

//compound method: BEGIN stmtlist END
string compound(std::ifstream & is) {
    //check for begin
    Token check;
    check.get(is);
   
    
    if(check.value() == "begin") {//we know a compound starts with a begin, in order to make sure we only take begin as a valid token we check its value because without that, the user could pass BEGIN or BeGiN and it would be valid, but it needs to be case sensitive
        string toReturn;
        
        spaceCount += 1;//indent by 1

        string val = stmtlist(is);
        
        int pos = is.tellg();
        check.get(is);
        if(check.value() == "end" ) {//check for our last end statement
           
           //get if we are at the last end statement
            int pos2 = is.tellg();
            check.get(is);
            if(check.type() == EOF_TOK) {
              spaceCount -= 1;  
            }
            else {
                is.seekg(pos2);
            }

            for(int i = 0; i < spaceCount; i++) {//setup our indent
                toReturn += " ";
            }
            spaceCount -= 1;

            
           return toReturn + "begin\n" + toReturn +  val + "\n" + toReturn + "end";
        }
        else {
            //cerr << "No end error instead got: " << val << endl;
            isError = true;
        }   
    }
    else {
        //cerr << "Error in compound() no begin! : instead got " << check.value() << endl;
        isError = true;
    }

}


//stmtlist method: stmt | stmt SEMICOLON stmtlist, DONE
string stmtlist(std::ifstream & is) {
    int pos2 = is.tellg();
    //a stmtlist() starts with a stmt
    string val = stmt(is);
    
    int pos = is.tellg();
    Token semi;
    semi.get(is);//get the next token to check for a semicolon

    if(semi.type() == EOF_TOK) {
        is.seekg(pos);
        return val;
    }
    if(semi.type() == SEMICOLON) {//if we see a semicolon
        string toReturn;//setup indent
        for(int i = 0; i < spaceCount; i++) {
            toReturn + " ";
        }
        return toReturn + val + "; \n" + stmtlist(is);//recursive call
    }
    else {
        is.seekg(pos);
        return val;
    }
}

//stmt method: ID | ID LPAREN exprlist RPAREN | ID ASSIGNOP expr | IF expr THEN compound ELSE compound | WHILE LPAREN expr RPAREN compound | compound
string stmt(std::ifstream & is) {
    //check if the first token is a id
    int pos = is.tellg();
    Token id;
    Token paren;
    id.get(is);
    
    if(id.type() == ID) {
        //check to see if the next token is LPAREN for the second case
        int pos2 = is.tellg();
        paren.get(is);
        if(paren.type() == LPAREN) {//2nd case
            string exp = exprlist(is);
            paren.get(is);

            if(paren.type() == RPAREN) {//check if we have both parentheses
                string toReturn;//setup indent
                for(int i = 0; i < spaceCount; i++) {
                    toReturn += " ";
                }

                return toReturn + id.value() + " (" + exp + ") ";  //return our string
            }
            else {
                //cerr << "Error in stmt() expected RPAREN got: " << paren;
                isError = true;
            }
        }
        else if(paren.type() == ASSIGNOP) {//3rd case
            string val = expr(is);
            string toReturn;
         
            for(int i = 0; i < spaceCount; i++) {
                toReturn += " ";//setup indent
            }
            return toReturn + id.value() + " " + paren.value() + " " + val;
        }
        else {//1st case 
        
            is.seekg(pos2);//go back to before the second get
            string toReturn;
            
            for(int i = 0 ; i < spaceCount; i++) {
                toReturn += " ";//setup indent
            }
            return toReturn + id.value();//1st case we just have an ID
        }
        
    }
    else if(id.value() == "if") {//4th case
            
            string val = expr(is);//it starts with an expr()
            Token then;
            then.get(is);//we should then have a then
            if(then.value() == "then") { 
                string com = compound(is);//then a compound so we call the method
               
                then.get(is);
                if(then.value() == "else") {//lastly we need to check for an else 
                    string com2 = compound(is);//this else is then followed by another compound
                    string toReturn;
                    for(int i = 0; i < spaceCount; i++) {
                        toReturn += " ";
                    }
                    return toReturn + id.value() +" "+ val + " then \n" + com  + "\n" + toReturn + then.value() + "\n" + com2 ;//return the whole string
                }
            }
           
    }
    else if(id.value() == "while") {//5th case
        Token paren;
        paren.get(is);
        if(paren.type() == LPAREN) {//similar structure to the if case, just check for the tokens and build our string
            string exp = expr(is);
            paren.get(is);
            if(paren.type() == RPAREN) {
                string comp = compound(is);
                string toReturn;
              
                for(int i = 0; i < spaceCount; i++) {
                    toReturn += " ";
                }
                return toReturn + "while (" + exp + ") \n" + toReturn + comp;//return the whole string
            }
            else {
                //cerr << "Missing RPAREN in while statment instead got: " << paren << endl;
                isError = true;
            }
        }
        else {
            //cerr << "Missing LPAREN in while statment instead got: " << paren << endl;
            isError = true;
        }
    }
    else if(id.type() == FLOAT || id.type() == INTEGER || id.type() == VOID) {//last case is a compound() call, if it starts with a type we know it is a compound call
        if(id.value() == "float" || id.value() == "int" || id.value() == "void") {
            string com = compound(is);
            return com;//call and return compound()
        }
        else {
            //cerr << "Need to be case sensitive in declaring your types!" << endl;
            isError = true;
        }
    }
    else {
        //cerr << "ERROR IN STMT()";
        isError = true;
    }
}

//declaration method: type idlist SEMICOLON
string declaration(ifstream & is) {
    //we start with a type then idlist
    string typeStr = type(is);
    string ids = idlist(is);

    //we know this will be a semicolon to end the idlist line
    Token semi;
    semi.get(is);
    if(semi == SEMICOLON) {//check for semicolon
        int pos = is.tellg();
        semi.get(is);
        if(semi.type() == BEGIN) {//this if statement is how we get the extra newline after the last declaration
            is.seekg(pos);
            return typeStr + " " + ids + ";\n\n";
        }
        else {
            is.seekg(pos);
            return typeStr + " " + ids + ";\n";
        }
    }
    else {
        //cerr << "error in declaration() got : " << semi << endl;
        isError = true;
    }


}


//program method: delcaration program | compound
string program(ifstream & is) {
    int pos = is.tellg();
    //check to see if we have a declaration or compound first, each of these start with a different token type
    Token tok;
    tok.get(is);

    if(tok.type() == INTEGER || tok.type() == FLOAT || tok.type() == VOID) {//check if we have a type first
        //unget the token and call the functions
        is.seekg(pos);
        string dec = declaration(is);
        return dec  + program(is);

    }     
    else if(tok.type() == BEGIN) {//if there was a begin we know that there must only be one compound statement
        is.seekg(pos);
        return compound(is);
    }
    else {
        //cerr << "Invalid program, needs to start with a type or begin statement, instead got: " << tok << endl;
        isError = true;
    }
}

//factor method: ID | ID LPAREN exprlist RPAREN | NUM_REAL | NUM_INT | LPAREN expr RPAREN
string factor(ifstream & is) {
    Token fac;
    fac.get(is);
    if(fac.type() == ID) {//case 1 
        //check for the second case
        int pos = is.tellg();
        Token fac2;
        fac2.get(is);
        if(fac2.type() == LPAREN) {//check for left paren
            string val = exprlist(is);
            //then check for the right paren
            fac2.get(is);
            if(fac2.type() == RPAREN) {
                string toReturn;
                for(int i = 0; i < spaceCount; i++) {
                    toReturn += " ";
                }
                return toReturn + fac.value() + " (" + val + ") ";
            }
            else {
                //cerr << "Error in factor() missing RPAREN, instead got: " << fac2 << endl;
                isError = true;
            }
        }
        else {//if we did not have the second case then we only had one id
            is.seekg(pos);
            string toReturn;
            for(int i = 0; i < spaceCount; i++) {
                toReturn += " ";
            }
            return fac.value();
        }
    }
    else if(fac.type() == NUM_REAL) {//case 3
        return fac.value();
    }
    else if(fac.type() == NUM_INT) {//case 4
        return fac.value();
    }
    else if(fac.type() == LPAREN) {//case 5
        string exp = expr(is);//call expr()
        Token paren;
        paren.get(is);
        
        if(paren.type() == RPAREN) {//check for right paren
            string toReturn;
            for(int i = 0; i < spaceCount; i++) {
                toReturn += " ";
            }

            return "(" + exp + ")";
        }
        else {
            //cerr << "Error in factor() expected RPAREN got: " << paren << endl;
            isError = true;
        }
    }
    else {
       //cerr << "ERROR unexpected token " << fac << endl;
       isError = true;
    }
}