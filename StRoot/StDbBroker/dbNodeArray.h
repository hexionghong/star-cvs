/***************************************************************************
 *
 * $Id: dbNodeArray.h,v 1.1 2000/01/10 20:31:17 porter Exp $
 *
 * Author: R. Jeff Porter
 ***************************************************************************
 *
 * Description: Abstract class to hide stl of a simple list of nodes (objects)
 *
 ***************************************************************************
 *
 * $Log: dbNodeArray.h,v $
 * Revision 1.1  2000/01/10 20:31:17  porter
 * modified StDbBroker to be an interface to the DB-interface, StDbLib.
 *  - old functionality is retained for the short-term & modifications
 *    are extensions
 *
 *
 **************************************************************************/
#ifndef DBNODEARRAY_HH
#define DBNODEARRAY_HH

class StDbNode;

class dbNodeArray {

public:

  virtual ~dbNodeArray(){};

  virtual int       addNode(StDbNode* node)    = 0;
  virtual StDbNode* getNode(int index)         = 0;

  virtual void      setParentID(int index)     = 0;
  virtual int       getParentID(int index)     = 0;
  virtual StDbNode* getParent(int index)       = 0;

  virtual int       getNumNodes()              = 0;
  virtual void      reset()                    = 0;
  virtual StDbNode* next()                     = 0;

};

#endif





