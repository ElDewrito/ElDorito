#include "Patch.h"

Patch::Patch(size_t Offset, std::initializer_list<uint8_t> Data, std::initializer_list<uint8_t> Reset /*= {}*/) 
  : Offset(Offset), 
    Data(Data), 
    Orig(Reset)
{
}

Patch::Patch() :
Offset(0)
{

}

Patch::Patch(size_t Offset, std::initializer_list<uint8_t> Data, Pointer base /*= Pointer::Base()*/)
  : Offset(Offset),
  Data(Data)
{
  Orig.resize(Data.size());
  base(Offset).Read(Orig.data(), Orig.size());
}
