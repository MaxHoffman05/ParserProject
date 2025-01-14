#include "Block.hpp"
#include <iostream>

using namespace std;

ostream &
Block::print(std::ostream &os) const
{
  os << _name
    << ":" << _startIndex
    << "(" << _size << ")"
    << "[" << _refCount << "]" << endl;
 

  return os;
}

std::ostream& operator<< (std::ostream &os, const Block &block)
{
  return block.print(os);
}