#ifndef StAngleDiff_hh
#define StAngleDiff_hh

template <class T1> class StThreeVector;

class StAngleDiff {

public:

  double phiDiff(StThreeVector<double> m1, StThreeVector<double> m2) ;
  double alphaDiff(StThreeVector<double> m1, StThreeVector<double> m2) ;
  double weightPhi(double phidiff) ;
  double weightAlpha(double alphadiff) ;
 
protected:
private:
 

};


#endif


