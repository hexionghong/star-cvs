/***************************************************************
 * $Id: StRichGHit.h,v 2.0 2000/08/09 16:17:00 gans Exp $
 *
 * Description:
 *   StRichGHit is a data type containing information
 *   passed by Geant.
 *   
 *   StRichGHit is a structure/class with:
 *     - a 3-vector position (doubles)
 *     - a string with the particleID
 *     - a representation of the volumeID
 *     - the energy loss (double)
 *     - a constructor with only a long initialization list
 *       
 ***************************************************************
 * $Log: StRichGHit.h,v $
 * Revision 2.0  2000/08/09 16:17:00  gans
 * Readded Files That were not added in last CVS. Cosmetic Changes, naming convention
 * for StRichDrawableT(foo)
 *
 * Revision 1.6  2000/04/05 15:59:26  lasiuk
 * short --> int
 *
 * Revision 1.5  2000/03/17 14:54:31  lasiuk
 * Large scale revisions after ROOT dependent memory leak
 *
 * Revision 1.4  2000/02/14 01:12:50  lasiuk
 * keep the track pointer info
 *
 * Revision 1.3  2000/02/08 16:23:44  lasiuk
 * change to class.  Augment constructors.
 * Incorporate system of units
 *
 * Revision 1.2  2000/01/27 17:05:37  lasiuk
 * add global information
 *
 * Revision 1.1  2000/01/18 21:32:01  lasiuk
 * Initial Revision
 *
 *   revision history:
 *     - 7/23/1999 created the struct,   Alexandre Nevski.
 *     - 7/27/1999 constructor added,    Alexandre Nevski.
 *     - 7/30/1999 fill added,           Alexandre Nevski.
 *     - 8/5/1999  problem with string,solved Alexandre Nevski. 
 ***************************************************************/
#ifndef ST_RICH_GHIT_H
#define ST_RICH_GHIT_H

#include <iostream.h>
#include <string>

#include "StThreeVector.hh"

#ifndef ST_NO_NAMESPACES
using std::string;
#endif

#ifndef ST_NO_NAMESPACES
//namespace StRichRawData {
#endif
    
class StRichGHit {
public:
    StRichGHit();

    StRichGHit(double x, double y, double z, double dE, int pID, string vID);
    StRichGHit(double x, double y, double z, int track_p, int pID);
    StRichGHit(double x, double y, double z, int pID);
    StRichGHit(double x, double y, double z, double dE, double ds, int pID, string vID,
	       double px, double py, double pz);
    ~StRichGHit();

    //StRichGHit(const StRichGHit&); { /* use default */}
    //StRichGHit& operator=(const StRichGHit&) {/* use default */}

    // Access
    const StThreeVector<double>& position()   const;
    StThreeVector<double>& position();
    const StThreeVector<double>& xGlobal()  const;
    const StThreeVector<double>& momentum() const;

    int     trackp()   const;
    double  cosX()     const;
    double  cosY()     const;
    double  cosZ()     const;
    double  ds()       const;
    double  dE()       const;
    int     id()       const;
    int     gid()      const;
    double  mass()     const;
    const string& volumeID() const;
    
    void fill(double x, double y, double z, int track_p,
	      double cosX, double coxY, double cosZ,
	      double step, double dE,
	      double px, double py, double pz,
	      int pID, string vID);

    void fill(StThreeVector<double>& x,
	      StThreeVector<double>& p,
	      int track_p,
	      double cosX, double coxY, double cosZ,
	      double step, double dE, double mass,
	      int id, int gid, string vID);

    void fill(double x, double y, double z, int track_p,
	      double cosX, double coxY, double cosZ, double step,
	      double dE, int pID, string vID);

    void addGlobal(double,double,double);

    void full(ostream&) const;

private:
    StThreeVector<double>    mXGlobal; //xx,yy,zz  NEVER USED!!!
    StThreeVector<double>    mXLocal;  //x,y,z
    StThreeVector<double>    mP;
    double                   mCosX, mCosY, mCosZ;
    int                      mTrackp;
    double                   mdS;
    int                      mId;
    int                      mGid;
    string                   mVolumeId;  //!
    double                   mdE;
    double                   mMass;
};    
ostream& operator<<(ostream&, const StRichGHit&);

inline const StThreeVector<double>& StRichGHit::position() const {return mXLocal;}
inline StThreeVector<double>& StRichGHit::position() {return mXLocal;}
inline const StThreeVector<double>& StRichGHit::xGlobal()  const {return mXGlobal;}
inline const StThreeVector<double>& StRichGHit::momentum() const {return mP;}
inline int     StRichGHit::trackp() const {return mTrackp;}
inline double  StRichGHit::cosX()     const {return mCosX;}
inline double  StRichGHit::cosY()     const {return mCosY;}
inline double  StRichGHit::cosZ()     const {return mCosZ;}
inline double  StRichGHit::ds()       const {return mdS;}
inline double  StRichGHit::dE()       const {return mdE;}
inline int   StRichGHit::id()       const {return mId ;}
inline int   StRichGHit::gid()       const {return mGid ;}
inline double  StRichGHit::mass()     const {return mMass;}
inline const string&  StRichGHit::volumeID() const {return mVolumeId ;}

#ifndef ST_NO_NAMESPACES
//} // namespace
#endif


#endif // StRichGHit_H
