/*********************************************************************
 *
 * $Id: StTpcLocalCoordinate.hh,v 1.1 1998/11/10 17:12:06 fisyak Exp $
 *
 * Author: brian May 20, 1998
 *
 **********************************************************************
 *
 * Description:  Raw data information along with access functions
 *
 **********************************************************************
 *
 * $Log: StTpcLocalCoordinate.hh,v $
 * Revision 1.1  1998/11/10 17:12:06  fisyak
 * Put Brian trs versin into StRoot
 *
 * Revision 1.1  1998/11/10 17:12:06  fisyak
 * Put Brian trs versin into StRoot
 *
 * Revision 1.2  1998/11/01 16:21:03  lasiuk
 * remove 'St' from variable declarations
 * add set functions in local Coordinates
 *
 * Revision 1.1  1998/05/21 21:27:38  lasiuk
 * Initial revision
 *
 *
 **********************************************************************/
#ifndef ST_TPC_LOCAL_COORDINATE_HH
#define ST_TPC_LOCAL_COORDINATE_HH

#include <iostream.h>

#include "StGlobals.hh"
#include "StThreeVector.hh"

class StTpcLocalCoordinate {
public:
    StTpcLocalCoordinate();
    StTpcLocalCoordinate(const double&, const double&, const double&);
    StTpcLocalCoordinate(const StThreeVector<double>&);

    virtual ~StTpcLocalCoordinate();
    //StTpcLocalCoordinate(const StTpcLocalCoordinate&);
    //StTpcLocalCoordinate& operator=(const StTpcLocalCoordinate&);
    
    // To Modify Coordinates
    StThreeVector<double>& pos();

private:
    StThreeVector<double> mPos;

inline const StThreeVector<double>& StTpcLocalCoordinate::pos() const { return(mPos); }
inline StThreeVector<double>& StTpcLocalCoordinate::pos() { return(mPos); }
// Non-member
ostream& operator<<(ostream&, const StTpcLocalCoordinate&);
#endif
