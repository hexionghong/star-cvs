/***************************************************************************
 *
 * $Id: StDbTime.h,v 1.2 2000/01/10 20:37:55 porter Exp $
 *
 * Author: R. Jeff Porter
 ***************************************************************************
 *
 * Description: Time class for "unix & date-time" timestamp access to DB
 *
 ***************************************************************************
 *
 * $Log: StDbTime.h,v $
 * Revision 1.2  2000/01/10 20:37:55  porter
 * expanded functionality based on planned additions or feedback from Online work.
 * update includes:
 * 	1. basis for real transaction model with roll-back
 * 	2. limited SQL access via the manager for run-log & tagDb
 * 	3. balance obtained between enumerated & string access to databases
 * 	4. 3-levels of diagnostic output: Quiet, Normal, Verbose
 * 	5. restructured Node model for better XML support
 *
 * Revision 1.1  1999/09/30 02:06:11  porter
 * add StDbTime to better handle timestamps, modify SQL content (mysqlAccessor)
 * allow multiple rows (StDbTable), & Added the comment sections at top of
 * each header and src file
 *
 **************************************************************************/
#ifndef STDBTIME_H
#define STDBTIME_H

#include <string.h>


class StDbTime {

public:

unsigned int munixTime;
char* mdateTime;

  StDbTime(): munixTime(0), mdateTime(0) {};
  StDbTime(unsigned int utime): mdateTime(0) { munixTime = utime;};

  StDbTime(const char* dtime): munixTime(0) { 
                                         mdateTime=new char[strlen(dtime)+1]; 
                                         strcpy(mdateTime,dtime);};


  virtual ~StDbTime() { if(mdateTime) delete [] mdateTime; }

  void setUnixTime(unsigned int utime) { munixTime = utime;}
  virtual void setDateTime(const char* dtime) { 
                                  mdateTime=new char[strlen(dtime)+1]; 
                                  strcpy(mdateTime,dtime); };

  virtual void setTime(unsigned int utime, const char* dtime){ 
                                             setUnixTime(utime);
                                             setDateTime(dtime);}
  unsigned int getUnixTime() { return munixTime; }
  virtual char* getDateTime(){ return mdateTime; }

};


#endif







