#ifndef StHbtCutMonitor_hh
#define StHbtCutMonitor_hh

class StHbtEvent;
class StHbtTrack;
class StHbtV0;
class StHbtKink;

#include "StHbtMaker/Infrastructure/StHbtString.hh"

class StHbtCutMonitor{
  
private:
  
public:
  StHbtCutMonitor(){/* no-op */};
  virtual ~StHbtCutMonitor(){/* no-op */};
  virtual StHbtString Report(){ 
    string Stemp = "*** no user defined Fill(const StHbtEvent*), take from base class"; 
    StHbtString returnThis = Stemp;
    return returnThis; 
  }
  virtual void EventBegin(const StHbtEvent*) { /* no-op */ }
  virtual void EventEnd(const StHbtEvent*) { /* no-op */ }
  virtual void Fill(const StHbtEvent*) { 
#ifdef STHBTDEBUG
    cout << " *** no user defined Fill(const StHbtEvent*), take from base class" << endl;
#endif
  }
  virtual void Fill(const StHbtTrack*) { 
#ifdef STHBTDEBUG
    cout << " *** no user defined Fill(const StHbtTrack*), take from base class" << endl;
#endif
  }
  virtual void Fill(const StHbtV0*) { 
#ifdef STHBTDEBUG
    cout << " *** no user defined Fill(const StHbtV0Track*), take from base class" << endl;
#endif
  }
  virtual void Fill(const StHbtKink*) { 
#ifdef STHBTDEBUG
    cout << " *** no user defined Fill(const StHbtKink*), take from base class" << endl;
#endif
  }
  virtual void Finish() { 
#ifdef STHBTDEBUG
    cout << " *** no user defined Finish(), take from base class" << endl;
#endif
  }
  virtual void Init() { 
#ifdef STHBTDEBUG
    cout << " *** no user defined Init(), take from base class" << endl;
#endif
  }
};

#endif
