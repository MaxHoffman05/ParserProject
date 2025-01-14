

#ifndef _BLOCK_HPP_
#define _BLOCK_HPP_
#include <iostream>


class Block {
private:
    std::string _name;
    int _startIndex;
    int _endIndex;
    int _size = _endIndex - _startIndex;
    int _refCount;


public:
    Block(): _name(), _size(-1), _refCount(-1), _startIndex(-1), _endIndex(-1)  {} 
    Block(const std::string &name, int endIndex, int startIndex, int refCount)
    : _name(name), _refCount(refCount), _startIndex(startIndex), _endIndex(endIndex),_size(endIndex-startIndex) {}


    
    // accessors 
    const std::string& name() const {return _name;}
    int size() const {return _size;}
    int refCount() const {return _refCount;}
    int startIndex() const {return _startIndex;}
    int endIndex() const {return _endIndex;}

    // modifiers
    std::string& name() {return _name;}
    int& refCount() {return _refCount;}
    int& size() {return _size;}
    int& startIndex() {return _startIndex;}
    int& endIndex() {return _endIndex;}


    
  //   print - outputs the actual private field values
  std::ostream &print(std::ostream &os) const;

  // overloaded I/O operators
  friend std::ostream&  operator<< (std::ostream &os, const Block &block);


};
#endif