/**
 * Computer Systems II - Project 3
 * Author: Max Hoffman
 * Date: 3/18/2024 - 3/24/2024
 * Purpose: Use the grammar to look through an input file grabbing the different command names and executing them as necessary
 * 
*/

#include "Grammar.hpp"
#include "Token.hpp"
#include "Block.hpp"
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
using namespace std;

//used for debugging to check if we had an error
bool isError = false;
bool checkPass() {
    return isError;
}

//global lists for all our variables and blocks
std::list<string> variableN;
std::list<Block*> variables;
std::list<Block*> fList;


//used for reference counts
list<Block*> ptrs;//list of block pointers
vector<vector<Block*>> listP;//list of block pointer lists



//merge sort method I made for a previous data structures project, has been changed to sort blocks instead of strings
// I also added a boolean as a parameter to be able to sort the variable and free list lists differently. The free 
//list is sorted by starting index while the variables list is sorted by name in alphabetical order
list<Block*> merge(list<Block*> list1, list<Block*> list2, bool flag) {
    list<Block*> toReturn;
    while(list1.size() != 0 && list2.size() != 0) {
        if(flag) {//we are sorting by name (variables list)
            if(list1.front()->name() < list2.front()->name()) {
                toReturn.push_back(list1.front());
                list1.pop_front();
            }
            else {
                toReturn.push_back(list2.front());
                list2.pop_front();
            } 
        }
        else {//sort by starting index instead (used for free list)
            if(list1.front()->startIndex() < list2.front()->startIndex()) {
                toReturn.push_back(list1.front());
                list1.pop_front();
            }
            else {
                toReturn.push_back(list2.front());
                list2.pop_front();
            } 
        }
    }
    while(list1.size() != 0) {
        toReturn.push_back(list1.front());
        list1.pop_front();
    }
    while(list2.size() != 0) {
        toReturn.push_back(list2.front());
        list2.pop_front();
    }
    return toReturn;
}


//merge sort recursive method
list<Block*> mergeSort(list<Block*> & toSort, bool flag) {
    if(toSort.size() <= 1) {
        return toSort;
    }
    else {
        //get our two sizes
        int size1 = toSort.size() / 2;
        int size2 = toSort.size() - size1;

        //create two vectors and load the values into them
        list<Block*> list1;
        list<Block*> list2;
        
        //how do we take the parts of the list?
        //take the first part of the original list and make a new list with size1
        //I dont know if there was a better way of deleting the first element of the list 
        //but I didnt want to use an iterator and this was the best that I could come up with
        for(int q = 0; q < size1; q++) {
            Block * newVal = toSort.front();
            toSort.pop_front();
            list1.push_back(newVal);
        }
        
        //do the same with another list this splits the original list in half
        for(int w = 0; w < size2; w++) {
            Block * newVal = toSort.front();
            toSort.pop_front();
            list2.push_back(newVal);
        }
        
        //recursively call, pass the boolean we passed in to be used in merge() to descide how we will sort the list
        mergeSort(list1, flag);
        mergeSort(list2, flag);
        toSort = merge(list1, list2, flag);//merge the lists together then return the final list
        return toSort;
    }  
}

//free method
void free(string varFree) {
    /*
    in order to free a variable we need to:
    go through variable list and delete it, also decrement any reference counts associated with the variable
    add a free block of the same size as long (as there is not another variable using the same info) to the free list
    sort both free list and variable list
    */
    Block * blockToFree;
    list<Block*> newList;
    for(Block * i : variables) {
        if(i->name() != varFree) {
            newList.push_back(i);
        }
        else {
            blockToFree = i;
        }
    }
    variables.swap(newList);

    //check if another variable in the variables list has the same starting and ending index
    bool isFound = false;
    for(Block * j : variables) {
        if(j->startIndex() == blockToFree->startIndex() && j->endIndex() == blockToFree->endIndex()) {
            isFound = true;
            break;
        }
    }

    if(!isFound) {//we need to add this block to free list to be available
        Block * newFreeBlock = new Block("freeB", blockToFree->endIndex(), blockToFree->startIndex(), 0);
        fList.push_back(newFreeBlock);
    }
    else {
        //we cant add the block to free list but we do need to take care of reference counts
        //search through reference list and decrement any blocks that share varFree
        for(vector<Block*> b : listP) {
            for(Block * y : b) {
                if(y->name() == varFree) {
                    y->refCount() -= 1;
                }
            }
        }

        //delete varFree from the reference list
        list<Block *> newList;
        for(Block * p : ptrs) {
            if(p->name() != varFree) {
                newList.push_back(p);
            }
        }
        ptrs.swap(newList);
    }
    //lastly sort the lists
    mergeSort(fList, false);
    mergeSort(variables, true);
}


//allocation method
void alloc(string varName, int size) {
    //first we must check if a variable already exists with that name
    bool alreadyExists = false;
    for(string i : variableN) {
        if(varName == i) {
            alreadyExists = true;
        }
    }
    
    if(alreadyExists) {//Case 1 : we are shrinking an existing variable or growing it
        

        //find the existing variable block
        Block * oldBlock;
        list<Block*> newList;
        for(Block * j : variables) {
            if(varName == j->name()) {
                oldBlock = j;
            }
            else {
                newList.push_back(j);
            }
        }
        //create our new block with the new start and end indexes
        Block  * newBlock = new Block(varName,oldBlock->startIndex() + size, oldBlock->startIndex(), oldBlock->refCount());
        list<Block> newFreeList;
        

        if(oldBlock->size() <= size) {//if we are growing, we must modify freelist
            //search through variables list and replace with newBlock
            list<Block*> tempL;
            for(Block * y : variables) {
                if(varName == y->name()) {
                    tempL.push_back(newBlock);
                }   
                else {
                    tempL.push_back(y);
                }
            }
            variables.swap(tempL);
            //then look through free list and modify the first fit block, take size - sizeOld for our new size of the free block
            bool doOnce = true;
            list<Block*> newFree;
            for(Block * g : fList) {
                if(g->size() >= newBlock->size() && doOnce) {
                    doOnce = false;
                    g->startIndex() = size; 
                    g->size() = g->endIndex() - g->startIndex();
                    newFree.push_back(g);
                }   
                else {
                    newFree.push_back(g);
                }
            }
            fList.swap(newFree);

        }
        else {//Or instead we are shrinking a block, that means an extra block will be added to free list
            //modify the block in variables first
            list<Block*> tempL;
            for(Block * i : variables) {
                if(i->name() == varName) {
                    tempL.push_back(newBlock);
                }
                else {
                    tempL.push_back(i);
                }
            }
            variables.swap(tempL);
            //add a new block to the free list, this was the remainder we arent using anymore
            Block * newFreeBlock = new Block("freeB",oldBlock->endIndex(),size,0);
            fList.push_back(newFreeBlock);
         
        }
        //sort both lists
        mergeSort(fList, false);
        mergeSort(variables, true);
    }
    else {//Case 2: We are creating a new variable
        for(Block *  j : fList) {
            if(j->size() >= size) {//allocate from first fit
                //first get rid of block that we found in the free list and section it if necessary
                Block * toDel = j;
                
                int newStartIndex = j->startIndex();
                int newEndIndex = j->startIndex() + size;
                variableN.push_back(varName);

                //create new block we will add to the variable list
                Block * newVar = new Block(varName, newEndIndex, newStartIndex, 1);
                
                list<Block*> newList;
                //go in and delete the block we just used up from the freeList, if the sizes didnt match that means we need to split the blocks
                if(j->size() == size) {
                    for(Block * k : fList) {
                        if(k->endIndex() != toDel->endIndex() && k->startIndex() != k->startIndex()) {
                            newList.push_back(k);
                        }
                    }
                    fList.swap(newList);

                    //we also need to add it to the variable list
                    variables.push_back(newVar);
                }
                else {
                    //if the sizes didnt match we need to chop the toDel in half
                    variables.push_back(newVar);

                    //now change the indexs of the block we found that this new variable fit in the free list
                    list<Block*> newList;
                    for(Block * q : fList) {
                        if(q->endIndex() != toDel->endIndex() && q->startIndex() != toDel->startIndex()) {
                            newList.push_back(q);
                        }
                        else {
                            //shrink the block then put that block back in the list
                            q->startIndex() = newEndIndex;
                            q->size() = q->size() - size;
                            newList.push_back(q);
                        }
                    }
                    fList.swap(newList);
                }
                //sort the list, using merge sort functions I made for a Data Structures & Algorithms project
                //we sort by starting address
                mergeSort(fList, false);
                mergeSort(variables, true);

                return;//get out since we added the variable
            }
        }
    }
    
}


//used to check all of the blocks in free list to see if we need to continue to compress
bool checkBlocks(vector<Block*> list) {
    for(int i = 0; i < list.size(); i++) {
        if(i+1 != list.size()) {
            if(list.at(i)->endIndex() == list.at(i+1)->startIndex()) {
                return false;
            }
        }
    }
    return true;
}

//compress method
void compress() {
    //look through the free list for any adjacent blocks
    list<Block*> compressedFreeList;
    vector<Block*> copy;
    for(Block * b : fList) {
        copy.push_back(b);
    }

    bool replace = false;
    bool flag = true;
    while(flag) {
        vector<Block*> copy2;
        
        for(int i = 0; i < copy.size(); i++) {//go through the vector and check the current spot block (at i) to the next block (at i+1), checks if we can combine them
            if(i+1 != copy.size()) {//prevents out of bounds error
                if(copy.at(i)->startIndex() + copy.at(i)->size() == copy.at(i+1)->startIndex() && copy.at(i)->name() != "DELETE") {//if the current blocks starting index plus its size is equal to the starting index of the next block that means we can combine them
                    //make a new block (this is our combined block from the current block and the next block)
                    Block * newB = new Block("freeB",copy.at(i+1)->endIndex(), copy.at(i)->startIndex(),0);
                    compressedFreeList.push_back(newB);
                    copy2.push_back(newB);
                    copy.at(i+1)->name() = "DELETE";//this is how I 'deleted' an item since with vectors I didnt want to deal with iterators to remove a block
                    
                }
                else {
                    if(copy.at(i)->name() != "DELETE") {//if the item has not been 'deleted' we can add it to the new free list, also means we cannot combine it
                        compressedFreeList.push_back(copy.at(i));
                        copy2.push_back(copy.at(i));
                    }
                }
            }
            else {
                if(copy.at(i)->name() != "DELETE") {//I realized that in very specific circumstances the last item would not get added and I realized I needed to add it if I did not go through the if statmenet that combines blocks
                    compressedFreeList.push_back(copy.at(i));//this took me a long time to figure out
                    copy2.push_back(copy.at(i));
                }
            }
        }

        if(checkBlocks(copy2)) {//call checkBlocks() which makes sure that we are done with combining all free blocks
            flag = false;
        }
        else {//if we return false that means we need to go back through the vector again
            compressedFreeList.clear();//clear out this list 
            copy.swap(copy2);//we make copy2 our new copy list that the above for loop uses
            copy2.clear();
        }
    }

    //at the very end we make sure to update the fList global that contains all the free blocks
    fList.swap(compressedFreeList);
    compressedFreeList.clear();
    mergeSort(fList, false);//also sort the new free list
}


//method for our assign operator when setting variables equal to each other
//varName = equalTo
//this method does not entirely work since it is in charge of updating the reference counts, I describe this in my read.me
void assignOP(string varName, string equalTo) {
    //first check if both strings are equal to since that is a valid test case
    //if they are we dont have to do anything
    if(varName != equalTo) {
        Block * equal;
        //create a new block variable with name of varName and the same stats as equalTo, to do this we must find equalTo in the variables list
        for(Block * j: variables) {
            if(j->name() == equalTo) {
                equal = j;
                break;
            }
        }
        //create our new block and add it to the 
        Block * newBlock = new Block(varName, equal->endIndex(), equal->startIndex(), equal->refCount());
        variables.push_back(newBlock);

        //look through each list in listP look for if any point to equalTo or varName
        bool flag = false;
        int index = 0;
        for(vector<Block*> b : listP) {
            for(Block * j : b) {
                if(j->name() == varName || j->name() == equalTo) {
                    flag = true;
                    break;
                }
            }
            index += 1;//we get our index to be used below to identify the correct list of pointers
        }
        
        if(flag) {//add to existing list
        
            index -= 1;
            listP.at(index).push_back(newBlock);

            //add ref counts for everything associated with varName or equalTo
            for(int i = 0; i < listP.at(index).size(); i++) {
                listP.at(index).at(i)->refCount() += 1;//iterate our reference count
            }
            variableN.push_back(varName);//add the variable to the variables list
            

        }
        else {//create a new ptr list
            //same code as above except we create a new list
            vector<Block*> newVec;
            newVec.push_back(newBlock);
            listP.push_back(newVec);
            variableN.push_back(varName);

            for(Block * q : variables) {
                if(q->name() == equalTo || q->name() == varName) {
                    q->refCount() += 1;
                }
            }

        }
        //sort the lists
        mergeSort(variables, true);
        mergeSort(fList, false);
    }
}
//dump method, prints out all the contents in each variable and free block lists
void dump() {
    cout << "=========================" << endl;
    cout << "Variables: " << endl;
    for(Block * j : variables) {
        cout << j->name() << ": " << j->startIndex() << "(" << j->size() << ") [" << j->refCount() << "]" << endl;
    }
    cout << "Free List: " << endl;
    for(Block * k : fList) {
        cout << k->startIndex() << "(" << k->size() << ") [" << k->refCount() << "]" << ", ";
        
    }
    cout << endl << "=========================" << endl;
}

//start of all the grammar methods, these grammar methods call the command methods above
//prog() grammar method
//prog -> <slist>
std::string prog(std::istream & is, int size) {
    //initialize free list with a free block as big as the size inputted by the user
    Block * freeB = new Block("freeB", size, 0, 0);
    fList.push_back(freeB);//add it to the free list
    
    string incomingVal = "";//set the incomingVal to nothing and pass it into slist
    string toRet = slist(is, incomingVal);
    return toRet;
}

//slist() grammar method
//slist -> <stmt> SEMICOLON <slist> | e (eplison)
std::string slist(std::istream & is, string incomingVal) {//I descided to use an incomingVal string parameter to handle the epsilon case easier, like the example we did in class of the left handed parser
    //check the first token to make sure it is a valid stmt (has to start with an id, if it doesnt that means we have nothing)
    Token id;
    int pos = is.tellg();
    id.get(is);

    if(id.type() == ID) {
        is.seekg(pos);
        string stmt1 = stmt(is);
        Token semi;
        semi.get(is);
        if(semi.type() == SEMICOLON) {
            string toPass = stmt1 + ";\n";
         
            if(is) {
                return stmt1 + ";\n" + slist(is, toPass);
            }
            else {
                return stmt1 + ";";
            }
        }
    }
    else {
        return incomingVal;
    }
}

//stmt grammar method
//stmt -> ID LPAREN ID RPAREN | ID LPAREN RPAREN | ID ASSIGNOP <rhs>
std::string stmt(std::istream & is) {
  
    int pos2 = is.tellg();
    Token id;
    int pos = is.tellg();
    id.get(is);
   

    if(id.type() == ID){
        string val = id.value();
        //check for LPAREN
        id.get(is);
        if(id.type() == LPAREN) {
            id.get(is);
            if(id.type() == RPAREN) {//second case
                //call dump
                if(val == "dump") {//we could have dump or compress needing to be called
                    dump();
                }
                else if(val == "compress") {
                    compress();
                }
                return val +"()";
            }
            else if(id.type() == ID) {//1st case
                string val2 = id.value();//second id in first case

                //check for RPAREN
                id.get(is);
                if(id.type() == RPAREN) {
                    if(val=="free") {//this is where we call free if it matches the val
                        free(val2);
                    }
                    return val + "(" + val2 + ")";
                }
            }
        }
        else if(id.type() == ASSIGNOP) {//3rd case
            string test = rhs(is);
            
            if(test.substr(0,5) == "alloc") {//check for alloc command
                string sizeVal;
                for(int i = 7; i < test.size() -1; i++) {
                    sizeVal += test[i];
                }
                alloc(val, atoi(sizeVal.c_str()));//call alloc with the value we grabbed from the string
                
            }
            else {//we know this is an assign op command
                assignOP(val, test);//call assign op
            }
            return val + " = " + test;
        }
    }
    else {
        //cerr << "Expected type ID instead got " << id << endl;
        isError = true;
    }

}

//rhs grammar method
//rhs -> ID LPAREN NUM_INT RPAREN | ID
std::string rhs(std::istream & is) {
    Token id;
    
    id.get(is);
    int pos2 = is.tellg();
    if(id.type() == ID) {//if we have an id
        //check next token to see if it is a LPAREN
        Token lparen;
        int pos = is.tellg();
        lparen.get(is);
        if(lparen.type() == LPAREN) {//if we have a left paren
            //check for a NUM_INT
            int pos2 = is.tellg();
            lparen.get(is);
            if(lparen.type() == NUM_INT) {//we have to have a NUM_INT here
                string val = lparen.value();
                lparen.get(is);
                if(lparen.type() == RPAREN) {//check for right paren
                    return id.value() + " (" + val + ")";
                }
                else {
                    //cerr << "ERROR in rhs expected RPAREN got " << lparen << endl;
                    isError = true;
                }
            }
            
            else {
                //cerr << "ERROR in rhs expected NUM_INT got " << lparen << endl;
                isError = true;
            }
        
        }
        else if(lparen.type() == SEMICOLON) {//check for semicolon
            is.seekg(pos2);//unget
            return id.value();//return what was before the semicolon
        }
        else {
            //unget the token and return the id
            is.seekg(pos);//unget
            return id.value();
        }
    }
    else {
        //cerr << "ERROR in RHS" << endl;
        isError = true;
    }
}