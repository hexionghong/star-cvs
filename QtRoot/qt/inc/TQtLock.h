// @(#)root/qt:$Name:  $:$Id: TQtLock.h,v 1.3.4.1 2016/05/23 18:32:45 jeromel Exp $
// Author: Giulio Eulisse  04/07/2005
#ifndef ROOT_TQtLock
#define ROOT_TQtLock

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TQtLock                                                              //
//                                                                      //
// Lock / unlock the critical section safely                            //
// To be replaced by TMutex class in future                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include <QApplication>
class TQtLock 
{
 public:
    TQtLock (void) { Lock();   }
   ~TQtLock (void) { UnLock(); }
    void Lock(Bool_t on=kTRUE) {
#ifdef NEEDLOCKING
       if (qApp) {
          if (on)  qApp->lock();
          else     qApp->unlock();
       }
#else
       if(on) {}
#endif
    }
    void UnLock(Bool_t on=kTRUE) { Lock(!on); }
};

#endif
