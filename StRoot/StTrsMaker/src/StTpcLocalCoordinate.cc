/***********************************************************************
 *
 * $Id: StTpcLocalCoordinate.cc,v 1.1 1998/11/10 17:12:21 fisyak Exp $
 *
 * Author:  brian May 20, 1998
 *
 ************************************************************************
 *
 * Description:  Raw data information along with access functions
 *
 ************************************************************************
 *
 * $Log: StTpcLocalCoordinate.cc,v $
 * Revision 1.1  1998/11/10 17:12:21  fisyak
 * Put Brian trs versin into StRoot
 *
 * Revision 1.1  1998/11/10 17:12:21  fisyak
 * Put Brian trs versin into StRoot
 *
 * Revision 1.2  1998/11/01 16:20:35  lasiuk
 * remove 'St' from variable declarations
 *
 * Revision 1.1  1998/05/21 21:27:57  lasiuk
 * Initial revision
 *
 *
 ***********************************************************************/
#include "StTpcLocalCoordinate.hh"

StTpcLocalCoordinate::StTpcLocalCoordinate() {/**/}

StTpcLocalCoordinate::StTpcLocalCoordinate(const double& x, const double& y, const double& z)
{
    StTpcLocalCoordinate(StThreeVector<double>(x,y,z));
}

StTpcLocalCoordinate::StTpcLocalCoordinate(const StThreeVector<double>& pos)
    : mPos(pos) {/**/}

StTpcLocalCoordinate::~StTpcLocalCoordinate() {/**/}

// Non-member Functions
ostream& operator<<(ostream& os, const StTpcLocalCoordinate& a)
{
    return os << "TPC_Local ("
	      << a.pos().x() << ", "
	      << a.pos().y() << ", "
	      << a.pos().z() << ")";
}
