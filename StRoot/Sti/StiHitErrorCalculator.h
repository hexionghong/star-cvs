#ifndef StiHitErrorCalculator_HH
#define StiHitErrorCalculator_HH

/*!
  Class containing hit error parameterization and calculation
  methods. Correct use involves setting parameters relevant to
  a particular detector, either through construction or explicit
  assignment methods.
  
  \author Andrew Rose 01.21.2002
  
  Revision history
  ----------------
  01.25.2002   ver 1.0  Initial check in of code
  02.10.2002   ver 2.0  Encapsulated version
  02.17.2002   ver 2.1  IOBroker hooks added
*/

class HitError_st;


class StiHitErrorCalculator
{
 public:
  StiHitErrorCalculator(){/*noop*/};
  virtual ~StiHitErrorCalculator(){/*noop*/};
  virtual void calculateError(StiKalmanTrackNode *)const = 0; 
};

class StiDefaultHitErrorCalculator: public StiHitErrorCalculator
{
 public:
   StiDefaultHitErrorCalculator();
   StiDefaultHitErrorCalculator(const StiDefaultHitErrorCalculator & calc);
	 const StiDefaultHitErrorCalculator & operator=(const StiDefaultHitErrorCalculator & calc);
	 const StiDefaultHitErrorCalculator & operator=(const HitError_st & error);
   ~StiDefaultHitErrorCalculator();
   inline void calculateError(StiKalmanTrackNode *node) const;
   inline void set(double intrinsicZ, double driftZ,
									 double crossZ, double intrinsicX,
									 double driftX, double crossX);
   inline void set(ifstream& iFle);

   inline void report();

 private:
   double coeff[6];              //0:intrinsicZ  1: driftZ   2: crossZ
                                 //3:intrinsicX  4: driftX   5: crossX
};

#endif








