// Named.cpp: implementation of the Named class.
//
//////////////////////////////////////////////////////////////////////

#include "Sti/Base/Named.h"

Named::Named(const string & aName)
{
  setName(aName);
}

Named::~Named()
{}

void Named::setName(const string & aName)
{
   int i = aName.size()-1;
   for(;i>=0 && aName[i]==' ';i--) {};
    _name = string(aName,0,i+1);
}

const string& Named::getName() const
{
    return _name;
}

bool Named::isNamed() const
{
   return (_name.size()>0 && _name!=" ");
}

bool Named::isName(const string &aName) const
{
   int i = aName.size()-1;
   for(;i>=0 && aName[i]==' ';i--) {}
   return _name==string(aName,0,i+1);
}

bool Named::isNamedAs(const Named &named) const
{
  return isName(named._name);
}

