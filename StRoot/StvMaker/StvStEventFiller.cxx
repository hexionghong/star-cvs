#if 1
/***************************************************************************
 *
 * $Id: StvStEventFiller.cxx,v 1.2 2010/09/29 23:39:25 perev Exp $
 *
 * Author: Manuel Calderon de la Barca Sanchez, Mar 2002
 ***************************************************************************
 *
 * $Log: StvStEventFiller.cxx,v $
 * Revision 1.2  2010/09/29 23:39:25  perev
 * Intereface fillPulls(...) chamnged
 *
 * Revision 1.1  2010/07/06 20:27:53  perev
 * Alpha version of Stv (Star Tracker Virtual)
 *
 * Revision 1.2  2010/07/03 16:27:15  perev
 * Last time name Stv
 *
 * Revision 1.1  2010/06/22 19:34:28  perev
 * EventFiller added
 *
 * Revision 2.90  2010/01/27 21:43:49  perev
 * Add _nPrimTracks for case of fiterr
 *
 * Revision 2.89  2009/10/18 22:47:29  perev
 * assert instead of skip
 *
 * Revision 2.88  2009/10/16 14:56:02  fisyak
 * Add check that pHit exists
 *
 * Revision 2.87  2009/10/15 03:29:30  perev
 * Add primary vertex number and charge(GVB)
 *
 * Revision 2.86  2009/08/19 21:27:57  perev
 *  Account time of flight for StvPulls
 *
 * Revision 2.85  2009/03/16 13:50:14  fisyak
 * Move out all Stv Chairs into StDetectorDb
 *
 * Revision 2.84  2008/08/22 13:32:52  fisyak
 * add one more digit in trakc flag, mFlag=zxyy, where  z = 1 for pile up track in TPC (otherwise 0)
 *
 * Revision 2.83  2008/04/03 20:04:05  fisyak
 * Straighten out DB access via chairs
 *
 * Revision 2.82  2007/10/17 15:32:35  fisyak
 * rename Hft => Pxl
 *
 * Revision 2.81  2007/04/16 22:47:18  perev
 * aux.mPt is +ve
 *
 * Revision 2.80  2007/03/21 17:51:36  fisyak
 * adjust for ROOT 5.14
 *
 * Revision 2.79  2006/12/19 19:46:09  perev
 * Filling pull tracks added
 *
 * Revision 2.78  2006/12/18 01:30:39  perev
 * fillPulls reorganized
 *
 * Revision 2.77  2006/08/31 03:25:58  fisyak
 * Make cut for EEMC pointing track based on StTrackDetectorInfo instead of StTrackFitTraits
 *
 * Revision 2.76  2006/08/29 22:18:37  fisyak
 * move filling of StTrackDetectorInfo into fillTrack
 *
 * Revision 2.75  2006/08/28 17:02:23  fisyak
 * Add +x11 short tracks pointing to EEMC, clean up StvDedxCalculator
 *
 * Revision 2.74  2006/06/16 21:28:57  perev
 * FillStHitErr method added and called
 *
 * Revision 2.73  2006/05/31 03:59:04  fisyak
 * Add Victor's dca track parameters, clean up
 *
 * Revision 2.72  2006/04/07 18:00:30  perev
 * Back to the latest Stv
 *
 * Revision 2.69  2006/02/14 18:56:18  perev
 * setGlobalDca==>setDca
 *
 * Revision 2.68  2006/01/19 22:29:57  jeromel
 * kMaxId -> kMaxDetectorId
 *
 * Revision 2.67  2005/12/08 00:06:27  perev
 * BugFix, Instead of vertex, first hit was used
 *
 * Revision 2.66  2005/08/18 22:31:47  perev
 * More tests
 *
 * Revision 2.65  2005/08/17 22:04:36  perev
 * PoinCount cleanup
 *
 * Revision 2.64  2005/08/16 21:09:06  perev
 * remeve 5fit cut
 *
 * Revision 2.63  2005/08/16 20:37:23  perev
 * remove small pt cut
 *
 * Revision 2.62  2005/08/14 01:24:40  perev
 * test for nhits<5 removed
 *
 * Revision 2.61  2005/08/04 04:04:19  perev
 * Cleanup
 *
 * Revision 2.60  2005/07/21 21:50:24  perev
 * First/last point of track filled from node now
 *
 * Revision 2.59  2005/07/20 17:34:08  perev
 * MultiVertex
 *
 * Revision 2.58  2005/05/12 18:32:20  perev
 * Temprary hack, save residuals
 *
 * Revision 2.57  2005/04/11 17:42:39  perev
 * Temporary residuals saving added
 *
 * Revision 2.56  2005/03/24 17:51:16  perev
 * print error code added
 *
 * Revision 2.55  2005/03/17 06:33:20  perev
 * TPT like errors implemented
 *
 * Revision 2.54  2005/02/25 17:43:15  perev
 * StTrack::setKey(...StvTrack::getId()) now
 *
 * Revision 2.53  2005/02/17 23:19:03  perev
 * NormalRefangle + Error reseting
 *
 * Revision 2.52  2005/02/07 18:34:16  fisyak
 * Add VMC dead material
 *
 * Revision 2.51  2005/01/17 03:56:56  pruneau
 * change track container to vector
 *
 * Revision 2.50  2005/01/17 01:32:13  perev
 * parameters protected
 *
 * Revision 2.49  2004/12/21 20:46:00  perev
 * Cleanup. All known bugs fixed
 *
 * Revision 2.48  2004/12/02 22:14:53  calderon
 * Only fill the fitTraits.chi2[1] data member for primaries.
 * It holds node->getChi2() from the innerMostHitNode, which will be the
 * vertex for primaries.
 *
 * Revision 2.47  2004/12/02 04:18:06  pruneau
 * chi2[1] now set to incremental chi2 at inner most hit or vertex
 *
 * Revision 2.46  2004/12/01 15:35:46  pruneau
 * removed throw and replaced with continue
 *
 * Revision 2.45  2004/11/08 15:34:16  pruneau
 * fix of the chi2 calculation
 *
 * Revision 2.44  2004/10/27 03:25:54  perev
 * Version V3V
 *
 * Revision 2.43  2004/10/26 06:45:41  perev
 * version V2V
 *
 * Revision 2.42  2004/10/14 02:21:34  calderon
 * Updated code in StTrackDetectorInfo, now only increment the reference count
 * for globals, not for primaries.  So fillTrackDetectorInfo changed to reflect
 * this.
 *
 * Revision 2.41  2004/10/01 01:13:51  calderon
 * Added bug fix from Marco:
 * flag%100 -> flag/100.
 *
 * Revision 2.40  2004/08/17 20:04:28  perev
 * small leak fixed, delete physicalHelix,originD
 *
 * Revision 2.39  2004/08/17 04:53:05  calderon
 * When filling fit traits for primary tracks, set the new flag
 * mPrimaryVertexUsedInFit.
 *
 * Revision 2.38  2004/08/10 14:21:13  calderon
 * Use the firstHit from the dynamic_cast, to avoid a compiler warning
 * for an unused variable.
 *
 * Revision 2.37  2004/08/06 22:23:29  calderon
 * Modified the code to use the setNumberOfxxxPoints(unsigned char,StDetectorId)
 * methods of StTrack, StTrackDetectorInfo, StTrackFitTraits, and to use
 * the maxPointCount(unsigned int detId) method of StvTrack.
 *
 * Revision 2.36  2004/08/06 02:29:20  andrewar
 * Modifed call to getMaxPointCount
 *
 * Revision 2.35  2004/08/05 05:25:25  calderon
 * Fix the assignment of the first point for primaries.  Now,
 * the logic for both globals and primaries is that the first
 * point is the first element of the stHits() vector that
 * can actually be casted to an StHit (the vertex will fail this test,
 * all other hits coming from detectors will satisfy it).
 *
 * Revision 2.34  2004/07/30 18:49:18  calderon
 * For running in production, Yuri's dEdx Maker will fill the Pid Traits,
 * so the filling of Pid Traits in the filler is no longer needed:
 * it actually causes confusion because the V0 finders will loop over
 * the PID traits vector and find the first one, so they won't find
 * the trait created by the dEdx Maker.  It is best to just comment
 * out the filling of the Pid Traits here.
 *
 * Revision 2.33  2004/07/07 19:33:48  calderon
 * Added method fillFlags.  Flags tpc, tpc+svt (globals and primaries) and flags -x02 tracks with less than 5 total fit points
 *
 * Revision 2.32  2004/04/21 21:36:24  calderon
 * Correction in the comments about the encoded method.
 *
 * Revision 2.31  2004/03/31 00:27:29  calderon
 * Modifications for setting the fit points based on the chi2<chi2Max algorithm.
 * -Distinguish between points and fit points, so I added a function for each.
 * -Points is done as it was before, just counting the stHits for a given
 *  detector id.
 * -Fit points is done the same with the additional condition that each
 *  StvNode has to satisfy the chi2 criterion.
 *
 * Revision 2.30  2004/03/29 00:52:20  andrewar
 * Added key value to StTrack fill. Key is simply the size of the
 * StTrackNode container at the time the track is filled.
 *
 * Revision 2.29  2004/03/23 23:12:36  calderon
 * Added an "accept" function to filter unwanted tracks from Stv into StEvent.
 * The current method just looks for tracks with a negative length, since
 * these were causing problems for the vertex finder (length was nan).  The
 * nan's have been trapped (one hopes!) in StvTrack, and for these
 * cases the return value is negative, so we can filter them out with a
 * simple length>0 condition.
 *
 * Revision 2.28  2004/03/19 19:33:23  andrewar
 * Restored primary filling logic. Now taking parameters at the
 * vertex for Primary tracks.
 *
 * Revision 2.27  2004/01/27 23:40:46  calderon
 * The filling of the impactParameter() for global tracks is done now
 * only after finding the vertex.  The
 * StPhysicalHelix::distance(StThreeVectorD) method is used for both globals
 * and primaries, the only difference is where the helix is obtained:
 * - globals - helix from StTrack::geometry(), which was filled from the
 *             innermost hit node, which should be a hit at the time.
 * - primaries - helix from innermost hit node, which should be the vertex
 *             at the time it is called.
 *
 * Revision 2.26  2003/12/11 03:44:29  calderon
 * set the length right again, it had dissappeared from the code...
 *
 * Revision 2.25  2003/11/26 04:02:53  calderon
 * track->getChi2() returns the sum of chi2 for all sti nodes.  In StEvent,
 * chi2(0) should be chi2/dof, so we need to divide by
 * dof=track->getPointCount()-5;
 *
 * Revision 2.24  2003/09/07 03:49:10  perev
 * gcc 3.2 + WarnOff
 *
 * Revision 2.23  2003/09/02 17:59:59  perev
 * gcc 3.2 updates + WarnOff
 *
 * Revision 2.22  2003/08/21 21:21:56  andrewar
 * Added trap for non-finite dEdx. Added logic to fillGeometry so
 * info is for innerMostHitNode on a detector, not vertex (note:
 * Primaries only)
 *
 * Revision 2.21  2003/08/05 18:26:15  andrewar
 * DCA track update logic modified.
 *
 * Revision 2.20  2003/07/01 20:25:28  calderon
 * fillGeometry() - use node->getX(), as it should have been since the
 * beginning
 * impactParameter() - always use the innermos hit node, not just for globals
 * removed extra variables which are no longer used.
 *
 * Revision 2.19  2003/05/15 03:50:26  andrewar
 * Disabled call to filldEdxInfo for the SVT. Checks need to be
 * applied to make sure the detector is active before calculator
 * is called, but for the review filling this info is unnecessary.
 *
 * Revision 2.18  2003/05/14 00:04:35  calderon
 * The array of 15 floats containing the covariance matrix has a different
 * order in Stv than in StEvent.  In Stv the array is counted starting from
 * the first row, column go to next column until you hit the diagonal,
 * jump to next row starting from first column. In StEvent the array is
 * counted starting from the first row, column go to the next row until you
 * hit the end, jump to next column starting from diagonal.
 * The filling of the fitTraits was fixed to reflect this.
 *
 * Revision 2.17  2003/05/12 21:21:39  calderon
 * switch back to getting the chi2 from track->getChi2()
 * Covariance matrix is still obtained from node->get(), and the values
 * are not as expected in StEvent, so this will still need to change.
 *
 * Revision 2.16  2003/05/08 22:23:33  calderon
 * Adding a check for finiteness of node origin and node curvature.  If any
 * of the numbers is not finite, the code will abort().
 *local 
 * Revision 2.15  2003/04/29 18:48:52  pruneau
 * *** empty log message ***
 *
 * Revision 2.14  2003/04/29 15:28:10  andrewar
 * Removed hacks to get helicity right; switch now done at source
 * (StvNode).
 *
 * Revision 2.13  2003/04/25 21:42:47  andrewar
 * corrected DCA bug and added temp fix for helicity problem. This will
 * have to be modified when the helicity convention in StvStKalmanTrack
 * is updated.
 *
 * Revision 2.12  2003/04/04 14:48:34  pruneau
 * *** empty log message ***
 *
 * Revision 2.11  2003/03/14 19:02:55  pruneau
 * various updates - DCA is a bitch
 *
 * Revision 2.10  2003/03/13 21:20:10  pruneau
 * bug fix in filler fixed.
 *
 * Revision 2.9  2003/03/13 18:59:44  pruneau
 * various updates
 *
 * Revision 2.8  2003/03/13 16:01:48  pruneau
 * remove various cout
 *
 * Revision 2.7  2003/03/13 15:15:52  pruneau
 * various
 *
 * Revision 2.6  2003/03/12 17:58:05  pruneau
 * fixing stuff
 *
 * Revision 2.5  2003/02/25 16:56:20  pruneau
 * *** empty log message ***
 *
 * Revision 2.4  2003/02/25 14:21:10  pruneau
 * *** empty log message ***
 *
 * Revision 2.3  2003/01/24 06:12:28  pruneau
 * removing centralized io
 *
 * Revision 2.2  2003/01/23 05:26:02  pruneau
 * primaries rec reasonable now
 *
 * Revision 2.1  2003/01/22 21:12:15  calderon
 * Restored encoded method, uses enums but stores the value in constructor
 * as a data member so bit operations are only done once.
 * Fixed warnings.
 *
 * Revision 2.0  2002/12/04 16:50:59  pruneau
 * introducing version 2.0
 *
 * Revision 1.21  2002/09/20 02:19:32  calderon
 * Quick hack for getting code for review:
 * The filler now checks the global Dca for the tracks and only fills
 * primaries when dca<3 cm.
 * Also removed some comments so that the production log files are not swamped
 * with debug info.
 *
 * Revision 1.20  2002/09/12 22:27:15  andrewar
 * Fixed signed curvature -> StHelixModel conversion bug.
 *
 * Revision 1.19  2002/09/05 05:47:36  pruneau
 * Adding Editable Parameters and dynamic StvOptionFrame
 *
 * Revision 1.18  2002/08/29 21:09:22  andrewar
 * Fixed seg violation bug.
 *
 * Revision 1.17  2002/08/22 21:46:00  pruneau
 * Made a fix to StvStEventFiller to remove calls to StHelix and StPhysicalHelix.
 * Currently there is one instance of StHelix used a calculation broker to
 * get helix parameters such as the distance of closest approach to the main
 * vertex.
 *
 * Revision 1.16  2002/08/19 19:33:00  pruneau
 * eliminated cout when unnecessary, made helix member of the EventFStvStEventFilleriller
 *
 * Revision 1.15  2002/08/12 21:39:56  calderon
 * Introduced fillPidTraits, which uses the values obtained from
 * Andrews brand new dEdxCalculator to create two instances of an
 * StTrackPidTraits object and pass it to the track being filled.
 *
 * Revision 1.14  2002/08/12 15:29:21  andrewar
 * Added dedx calculators
 *
 * Revision 1.13  2002/06/28 23:30:56  calderon
 * Updated with changes debugging for number of primary tracks added.
 * Merged with Claude's latest changes, but restored the tabs, othewise
 * cvs diff will not give useful information: everything will be different.
 *
 * Revision 1.12  2002/06/26 23:05:31  pruneau
 * changed macro
 *
 * Revision 1.11  2002/06/25 15:09:16  pruneau
 * *** empty log message ***
 *
 * Revision 1.10  2002/06/18 18:08:34  pruneau
 * some cout statements removed/added
 *
 * Revision 1.9  2002/06/05 20:31:15  calderon
 * remove some redundant statements, the call to
 * StTrackNode::addTrack()
 * already calls
 * track->SetNode(this), so I don't need to do it again
 *
 * Revision 1.8  2002/05/29 19:14:45  calderon
 * Filling of primaries, in
 * StvStEventFiller::fillEventPrimaries()
 *
 * Revision 1.7  2002/04/16 19:46:44  pruneau
 * must catch exception
 *
 * Revision 1.6  2002/04/16 13:11:30  pruneau
 * *** empty log message ***
 *
 * Revision 1.5  2002/04/09 16:03:13  pruneau
 * Included explicit extension of tracks to the main vertex.
 *
 * Revision 1.4  2002/04/03 16:35:03  calderon
 * Check if primary vertex is available in StvStEventFiller::impactParameter(),
 * if not, return DBL_MAX;
 *
 * Revision 1.3  2002/03/28 04:29:49  calderon
 * First test version of Filler
 * Currently fills only global tracks with the following characteristics
 * -Flag is set to 101, as most current global tracks are.  This is not
 * strictly correct, as this flag is supposed to mean a tpc only track, so
 * really need to check if the track has svt hits and then set it to the
 * appropriate flag (501 or 601).
 * -Encoded method is set with bits 15 and 1 (starting from bit 0).  Bit 1
 * means Kalman fit.
 *  Bit 15 is an as-yet unused track-finding bit, which Thomas said ITTF
 * could grab.
 * -Impact Parameter calculation is done using StHelix and the primary vertex
 * from StEvent
 * -length is set using getTrackLength, which might still need tweaking
 * -possible points is currently set from getMaxPointCount which returns the
 *  total, and it is not
 *  what we need for StEvent, so this needs to be modified
 * -inner geometry (using the innermostHitNode -> Ben's transformer ->
 *  StPhysicalHelix -> StHelixModel)StvStEventFiller
 * -outer geometry, needs inside-out pass to obtain good parameters at
 *  outermostHitNode
 * -fit traits, still missing the probability of chi2
 * -topology map, filled from StuFixTopoMap once StDetectorInfo is properly set
 *
 * This version prints out lots of messages for debugging, should be more quiet
 * when we make progress.
 *
 **************************************************************************/
//ROOT
#include "RVersion.h"
#include "TCernLib.h"
#include "TVectorD.h"
#include "TMatrixD.h"
//std
#include "Stiostream.h"
#include <algorithm>
#include <stdexcept>

// SCL
#include "StPhysicalHelix.hh"
#include "StThreeVector.hh"
#include "StThreeVectorF.hh"
#include "PhysicalConstants.h"
#include "SystemOfUnits.h"
#include "StTrackDefinitions.h"
#include "StTrackMethod.h"
#include "StDedxMethod.h"

//StEvent
#include "StPrimaryVertex.h"
#include "StEventTypes.h"
#include "StDetectorId.h"
#include "StHelix.hh"
#include "StDcaGeometry.h"
#include "StHit.h"


#include "StEventUtilities/StEventHelper.h"
#include "StEventUtilities/StuFixTopoMap.cxx"
//Stv
#include "Stv/StvStl.h"
#include "Stv/StvHit.h"
#include "Stv/StvNode.h"
#include "Stv/StvTrack.h"
#include "StvUtil/StvPullEvent.h"
#include "StvUtil/StvHitErrCalculator.h"
#include "StarVMC/GeoTestMaker/StTGeoHelper.h"
//#include "StDetectorDbMaker/StvKalmanTrackFitterParameters.h"

//StvMaker
#include "StvMaker/StvStEventFiller.h"

#include "TMath.h"

#include <map>
std::map<const StvTrack*, StTrackNode*> gTrkNodeMap;
typedef std::map<const StvTrack*, StTrackNode*>::iterator TkMapIter;


//_____________________________________________________________________________
inline StThreeVectorF position(const StvNode *node)
{
  const double *d = node->GetFP().P;
  return StThreeVectorF(d[0],d[1],d[2]);
}
//_____________________________________________________________________________
inline StThreeVectorF position(const StvHit  *hit )
{
  const float *f = hit->x_g();
  return StThreeVectorF(f[0],f[1],f[2]);
}
//_____________________________________________________________________________
inline int getCharge(const StvNode *node)
{
 return node->GetFP().getCharge();
}
//_____________________________________________________________________________
inline double getCurvature(const StvNode *node)
{
 return node->GetFP()._curv;
}
//_____________________________________________________________________________
inline double getPhi(const StvNode *node)
{
 return node->GetFP()._psi;
}
//_____________________________________________________________________________
inline double getDip(const StvNode *node)
{
 return atan(node->GetFP()._tanl);
}
//_____________________________________________________________________________
inline StThreeVectorF getMom(const StvNode *node)
{
 double p[3];
 node->GetFP().getMom(p); return StThreeVectorF(p[0],p[1],p[2]);
}
//_____________________________________________________________________________
inline int getHelicity(const StvNode *node)
{
  double curv = node->GetFP()._curv;
  return (curv < 0) ? -1 : 1;
}

//_____________________________________________________________________________
inline double NICE(double ang)
{ return fmod(ang+M_PI*11,M_PI*2)-M_PI;}
//______________________________________________________________________________
/**
   returns the node information in TPT representation
   double x[6],  : state, for a definition, in radial implementation
                   rad  - radius at start (cm). See also comments
                   phi  - azimuthal angle  (in rad)      
                   z    - z-coord. (cm)                 
                   psi  - azimuthal angle of pT vector (in rads)     
                   tanl - tan(dip) =pz/pt               
                   q/pt -  
   double cc[15] : error matrix of the state "x" rad is fixed
                       code definition adopted here, where:

                                                 Units
                       ______|________________|____________
                       phi*R |  0  1  2  3  4 |  deg*cm
                        z0   |  1  5  6  7  8 |    cm
                       tanl  |  2  6  9 10 11 |    1         covar(i)
                        psi  |  3  7 10 12 13 |   deg
                       q/pt  |  4  8 11 13 14 | e*1/(GeV/c)
                       -----------------------------------

                       and where phi  = atan2(y0,x0)*(180 deg/pi)
                                 R    = sqrt(x0*x0 + y0*y0)
                                 q/pt = icharge*invpt; (This is what the 
                                        radius of curvature actually
                                        determines)
PhiPhi PhiZ PhiTan PhiPsi PhiPt
       ZZ   ZTan   ZPsi     ZPt
            TanTan TanPsi TanPt
                   PsiPsi PsiPt
		           PtPt


*/

//_____________________________________________________________________________
void getTpt(const StvNode *node,float  x[6],float  e[15])
{
  enum {jRad=0,jPhi,jZ,jTan,jPsi,jPti};
static const double DEG = 180./M_PI;
static       double fak[6] = {1,0,1,1,DEG,0};
static const int toUpp[15] = 	{0,
				 1,  5,
				 2,  6,  9,
				 3,  7, 10, 12,
				 4,  8, 11, 13, 14};

  double xx[6],ee[15];
  const StvNodePars &fp = node->GetFP();
  fp.GetRadial(xx,ee,&node->GetFE());
  fak[jPhi] = DEG*xx[jRad];
  fak[jPti] = fp.getCharge()*fabs(xx[jPti]);

  for (int i=0;i<6;i++) {x[i] = (float)(fak[i]*xx[i]);}
  if (!e) return;

  x[0] = (float)xx[0];
  for (int i=0,li=0;i< 5;li+=++i) {
    x[i+1]=(float)xx[i+1];
    for (int j=0;j<=i;j++) {
      e[toUpp[li+j]] = (float)(ee[li+j]*fak[i+1]*fak[j+1]);
  } }
}
//_____________________________________________________________________________
void getDcaLocal(const StvNode *node,float yz[2],float yzErr[2])
{
static int nCall=0; nCall++;
  const StvNodePars &fp = node->GetFP();
  double myTan = fp._tanl;
  double cos2L = 1./(1+myTan*myTan);
  double cosL = sqrt(cos2L);
  double sinL = myTan*cosL;
  double cosP = fp._cosCA;
  double sinP = fp._sinCA;
//		Track Frame
  TMatrixD dcaFrame(3,3);

  dcaFrame[0][0] =  cosL*cosP;
  dcaFrame[0][1] =  cosL*sinP;
  dcaFrame[0][2] =  sinL;

  dcaFrame[1][0] = -sinP;
  dcaFrame[1][1] =  cosP;
  dcaFrame[1][2] =  0;

  dcaFrame[2][0] = -sinL*cosP;
  dcaFrame[2][1] = -sinL*sinP;
  dcaFrame[2][2] =  cosL;

  StvHit *hit=node->GetHit();
  assert(hit);
  const double *hrr=node->GetHE();
  double d[3]={ hit->x_g()[0]-fp._x, hit->x_g()[1]-fp._y,hit->x_g()[2]-fp._z};
  TVectorD dif(3,d); 
  TVectorD loc = dcaFrame*dif;
  yz[0]= loc[1];   yz[1]= loc[2] ;
  
  yzErr[0] = (hrr[0] - node->GetFE().mHH);
  yzErr[1] = (hrr[2] - node->GetFE().mZZ);
  for (int j=0;j<2;j++){yzErr[j] = (yzErr[j]>1e-6)? sqrt(yzErr[j]):1e-3;}


}
  
//_____________________________________________________________________________
StvStEventFiller::StvStEventFiller()
{
   mGloPri = 0;
   mPullEvent=0;
  
  //mResMaker.setLimits(-1.5,1.5,-1.5,1.5,-10,10,-10,10);
  //mResMaker.setDetector(kSvtId);

  // encoded method = 16 bits = 12 finding and 4 fitting, Refer
  // to StTrackMethod.h and StTrackDefinitions.h in pams/global/inc/
  // and StEvent/StEnumerations.h
  // For the IT tracks use:
  // Fitting: kITKalmanFitId     (should be something like 7, but don't hardwire it)
  // Finding: tpcOther           (should be 9th LSB, or shift the "1" 8 places to the left, but also don't hardwire it) 
  // so need this bit pattern:
  // finding 000000010000     
  // fitting             0111 
  //               256  +   7 = 263;
  unsigned short bit = 1 << tpcOther;  // shifting the "1" exactly tpcOther places to the left
  mStvEncoded = kITKalmanFitId + bit; // adding that to the proper fitting Id

}

//_____________________________________________________________________________
StvStEventFiller::~StvStEventFiller()
{
   cout <<"StvStEventFiller::~StvStEventFiller()"<<endl;
}

//_____________________________________________________________________________
/*! 
  Algorithm:
  Loop over all tracks in the StvTrackContainer, doing for each track:
  - Create a new global track and associated information (see below)
    and set its data members according to the StvTrack,
    can be done in a StGlobalTrack constructor
  - Hang the new track to the StTrackNode container in StEvent, this creates a new entry
    in the container, the global track is now owned by it.
    <p>
  In addition to the StGlobalTrack, we need to create the following objects (owned by it):
  StTrackTopologyMap
  StTrackFitTraits
  StTrackGeometry (2 are needed, one at first point, one at last point)
  (note: StHelixModel is implementation of the StTrackGeometry abstract class)
  
  The track also owns a container of PidTraits, this algorithm will not fill this container.
  
  And set up links to:
  StTrackDetectorInfo (owned by StEvent, StSPtrVecTrackDetectorInfo)
  StTrackNode         (owned by StEvent, StSPtrVecTrackNode)
  These links are
  track  -> detector info
  track <-> track node

  Skeleton of the algorithm:
  <code> \n
  StSPtrVecTrackNode& trNodeVec = mEvent->trackNodes(); \n
  StSPtrVecTrackDetectorInfo& detInfoVec = mEvent->trackDetectorInfo(); \n
  for (trackIterator trackIt = mTracks->begin(); trackIt != mTracks->end(); ++trackIt) { \n
     StvTrack* kTrack = (*trackIt).second; // the container is a <map>, need second entry of <pair> \n
\n
     StTrackDetectorInfo* detInfo = new StTrackDetectorInfo();\n
     fillDetectorInfo(detInfo,kTrack);\n
     detInfoVec.push_back(detInfo);\n
     \n
     StTrackNode* trackNode = new StTrackNode;\n
     trNodeVec.push_back(trackNode);\n
     \n
     StGlobalTrack* gTrack = new StGlobalTrack();\n
     fillGlobalTrack(gTrack,kTrack);\n
     \n
     // set up relationships between objects\n
     gTrack->setDetectorInfo(detInfo);\n
     gTrack->setNode(trackNode);\n
     trackNode->AddTrack(gTrack);\n
  }\n
  </code>
  The creation of the various objects needed by StGlobalTrack are taken care of in the methods:
  fillTopologyMap(), fillGeometry(), fillFitTraits(), which are called within fillGlobalTrack().
  
*/
//_____________________________________________________________________________
void StvStEventFiller::fillEvent()
{
  //cout << "StvStEventFiller::fillEvent() -I- Started"<<endl;
  mGloPri=0;
  gTrkNodeMap.clear();  // need to reset for this event
  StSPtrVecTrackNode& trNodeVec = mEvent->trackNodes(); 
  StSPtrVecTrackDetectorInfo& detInfoVec = mEvent->trackDetectorInfo(); 
  int errorCount=0; 

  int fillTrackCount1=0;
  int fillTrackCount2=0;
  int fillTrackCountG=0;
  StErrorHelper errh;
  mTrackNumber=0;
  for (StvTrackConstIter trackIt = mTracks->begin(); trackIt!=mTracks->end();++trackIt) 
    {
      const StvTrack* kTrack = (*trackIt);
      if (!accept(kTrack)) continue; // get rid of riff-raff
      mTrackNumber++;
      StTrackDetectorInfo* detInfo = new StTrackDetectorInfo;
      fillDetectorInfo(detInfo,kTrack,true); //3d argument used to increase/not increase the refCount. MCBS oct 04.
      // track node where the new StTrack will reside
      StTrackNode* trackNode = new StTrackNode;
      // actual filling of StTrack from StvTrack
      StGlobalTrack* gTrack = new StGlobalTrack;
	{
	  fillTrackCount1++;
	  fillTrack(gTrack,kTrack,detInfo);
	  // filling successful, set up relationships between objects
	  detInfoVec.push_back(detInfo);
	  //cout <<"Setting key: "<<(unsigned short)(trNodeVec.size())<<endl;
	  gTrack->setKey((unsigned short)kTrack->GetId());
	  trackNode->addTrack(gTrack);
	  trNodeVec.push_back(trackNode);
	  // reuse the utility to fill the topology map
	  // this has to be done at the end as it relies on
	  // having the proper track->detectorInfo() relationship
	  // and a valid StDetectorInfo object.
	  //cout<<"Tester: Event Track Node Entries: "<<trackNode->entries()<<endl;
	  gTrkNodeMap.insert(map<const StvTrack*,StTrackNode*>::value_type (kTrack,trNodeVec.back()) );
	  if (trackNode->entries(global)<1)
	    cout << "StvStEventFiller::fillEvent() -E- Track Node has no entries!! -------------------------" << endl;  
          int ibad = gTrack->bad();
	  errh.Add(ibad);
          if (ibad) {
//VP	    printf("GTrack error: %s\n",errh.Say(ibad).Data());
//VP	    throw runtime_error("StvStEventFiller::fillEvent() StTrack::bad() non zero");
          }
	  fillTrackCount2++;
          fillPulls(kTrack,0);
          if (gTrack->numberOfPossiblePoints()<10) continue;
          if (gTrack->geometry()->momentum().mag()<0.1) continue;
	  fillTrackCountG++;
          
	}
    }
  if (errorCount>4)
    cout << "There were "<<errorCount<<"runtime_error while filling StEvent"<<endl;

  cout <<"StvStEventFiller::fillEvent() -I- Number of filled as global(1):"<< fillTrackCount1<<endl;
  cout <<"StvStEventFiller::fillEvent() -I- Number of filled as global(2):"<< fillTrackCount2<<endl;
  cout <<"StvStEventFiller::fillEvent() -I- Number of filled GOOD globals:"<< fillTrackCountG<<endl;
  errh.Print();

  return;
}
//_____________________________________________________________________________
void StvStEventFiller::fillEventPrimaries() 
{
  //cout <<"StvStEventFiller::fillEventPrimaries() -I- Started"<<endl;
  mGloPri=1;
  if (!gTrkNodeMap.size()) 
    {
      cout <<"StvStEventFiller::fillEventPrimaries(). ERROR:\t"
	   << "Mapping between the StTrackNodes and the StvKalmanTracks is empty.  Exit." << endl;
      return;
    }
  //Added residual maker...aar
  StPrimaryVertex* vertex = 0;
  StSPtrVecTrackDetectorInfo& detInfoVec = mEvent->trackDetectorInfo();
  cout << "StvStEventFiller::fillEventPrimaries() -I- Tracks in container:" << mTracks->size() << endl;
  int mVertN=0;
  int noPipe=0;
  int ifcOK=0;
  int fillTrackCount1=0;
  int fillTrackCount2=0;
  int fillTrackCountG=0;
  StErrorHelper errh;
  const StvTrack *kTrack = 0;
  StPrimaryTrack *pTrack = 0;
  StGlobalTrack  *gTrack = 0;
  StTrackNode    *nTRack = 0;
  mTrackNumber=0;
  for (StvTrackConstIter tkIter= mTracks->begin(); tkIter!=mTracks->end();++tkIter) {
    kTrack = *tkIter;
    if (!accept(kTrack)) 			continue;
    map<const StvTrack*, StTrackNode*>::iterator itKtrack = gTrkNodeMap.find(kTrack);
    if (itKtrack == gTrkNodeMap.end())  	continue;//Stv global was rejected
    mTrackNumber++;

    nTRack = (*itKtrack).second;
    assert(nTRack->entries()<=10);
    assert(nTRack->entries(global)); 

    //double globalDca = nTRack->track(global)->impactParameter();
    //Even though this is filling of primary tracks, there are certain
    // quantities that need to be filled for global tracks that are only known
    // after the vertex is found, such as dca.  Here we can fill them.
    // 
    gTrack = static_cast<StGlobalTrack*>(nTRack->track(global));
    assert(gTrack->key()==kTrack->GetId());
    float minDca = 1e10; //We do not know which primary. Use the smallest one
    
    pTrack = 0;
    for (mVertN=0; (vertex = mEvent->primaryVertex(mVertN));mVertN++) {
      StThreeVectorD vertexPosition = vertex->position();
      double zPrim = vertexPosition.z();
      // loop over StvKalmanTracks
      float globalDca = impactParameter(gTrack,vertexPosition);
      if (fabs(minDca) > fabs(globalDca)) minDca = globalDca;
 
      if (kTrack->IsPrimary()!=mVertN+1)	continue;
      const StvNode *lastNode = kTrack->GetNode(StvTrack::kPrimPoint);
      StvHit *pHit = lastNode->GetHit();
      assert (pHit);
      if (fabs(pHit->x_g()[2]-zPrim)>0.1)		continue;//not this primary

      fillTrackCount1++;
      // detector info
      StTrackDetectorInfo* detInfo = new StTrackDetectorInfo;
      fillDetectorInfo(detInfo,kTrack,false); //3d argument used to increase/not increase the refCount. MCBS oct 04.
      fillPulls(kTrack,1); 
      StPrimaryTrack* pTrack = new StPrimaryTrack;
      pTrack->setKey( gTrack->key());

      fillTrack(pTrack,kTrack, detInfo);
      // set up relationships between objects
      detInfoVec.push_back(detInfo);

      nTRack->addTrack(pTrack);  // StTrackNode::addTrack() calls track->setNode(this);
      vertex->addDaughter(pTrack);
      fillTrackCount2++;
      int ibad = pTrack->bad();
      errh.Add(ibad);
      if (ibad) {
//VP	        printf("PTrack error: %s\n",errh.Say(ibad).Data());
//VP	        throw runtime_error("StvStEventFiller::fillEventPrimaries() StTrack::bad() non zero");
      }
      if (pTrack->numberOfPossiblePoints()<10) 		break;
      if (pTrack->geometry()->momentum().mag()<0.1) 	break;
      fillTrackCountG++;
      break;
    } //end of verteces
//??      kTrack->setDca(minDca);
      gTrack->setImpactParameter(minDca);
      if (pTrack) pTrack->setImpactParameter(minDca);

  } // kalman track loop
  gTrkNodeMap.clear();  // need to reset for the next event
  cout <<"StvStEventFiller::fillEventPrimaries() -I- Primaries (1):"<< fillTrackCount1<< " (2):"<< fillTrackCount2<< " no pipe node:"<<noPipe<<" with IFC:"<< ifcOK<<endl;
  cout <<"StvStEventFiller::fillEventPrimaries() -I- GOOD:"<< fillTrackCountG <<endl;
  errh.Print();
  return;
}
//_____________________________________________________________________________
/// use the vector of StHits to fill the detector info
/// change: currently point and fit points are the same for StvKalmanTracks,
/// if this gets modified later in ITTF, this must be changed here
/// but maybe use track->getPointCount() later?
//_____________________________________________________________________________
void StvStEventFiller::fillDetectorInfo(StTrackDetectorInfo* detInfo, const StvTrack* track, bool refCountIncr) 
{
  //cout << "StvStEventFiller::fillDetectorInfo() -I- Started"<<endl;
  int dets[kMaxDetectorId][3];
  getAllPointCount(track,dets);
  const StvNode *node = 0;
  for (int i=1;i<kMaxDetectorId;i++) {
    if (!dets[i][1]) continue;
    detInfo->setNumberOfPoints(dets[i][1],static_cast<StDetectorId>(i));
  }
  for (StvNodeConstIter it = track->begin();it!=track->end();++it) 
  {
      node = (*it);
      if (node->GetType() != StvNode::kRegNode) continue;
      const StvHit *stiHit = node->GetHit();
      if (!stiHit)		continue;
      if (node->GetXi2()>1000)  continue;

// 	EXCEPTION Fill StHit errors for Gene
      StHit *hh = (StHit*)stiHit->stHit();
      FillStHitErr(hh,node);

      if (!hh) 			continue;
      
      detInfo->addHit(hh,refCountIncr);
      if (!refCountIncr) 	continue;
      hh->setFitFlag(1);
  }
  const double *d=0;
  node = track->GetNode(StvTrack::kLastPoint); d = node->GetFP().P;
  detInfo->setLastPoint (position(node));

  node = track->GetNode(StvTrack::kFirstPoint); d = node->GetFP().P;
  detInfo->setFirstPoint (position(node));

}
//_____________________________________________________________________________
void StvStEventFiller::fillGeometry(StTrack* gTrack, const StvTrack* track, bool outer)
{
  //cout << "StvStEventFiller::fillGeometry() -I- Started"<<endl;
  assert(gTrack);
  assert(track) ;
  const StvNode* node = track->GetNode((!outer)? StvTrack::kFirstPoint:StvTrack::kLastPoint);
  StvHit *ihit = node->GetHit();
  StThreeVectorF nodpos(position(node));
  StThreeVectorF hitpos(position(ihit));

  double dif = (hitpos-nodpos).mag();

  if (dif>3.) {
    dif = nodpos.z()-hitpos.z();
    printf("***Track(%d) DIFF TOO BIG %g \n",track->GetId(),dif);
    printf("H=%g %g %g N =%g %g %g\n",hitpos.x()   ,hitpos.y()   ,hitpos.z()
		                     ,nodpos.x(),nodpos.y(),nodpos.z());
     
    assert(fabs(dif)<50.);
  }

    // making some checks.  Seems the curvature is infinity sometimes and
  // the origin is sometimes filled with nan's...
  
  int ibad = nodpos.bad();
  if (ibad) {
      cout << "StvStEventFiller::fillGeometry() Encountered non-finite numbers!!!! Bail out completely!!! " << endl;
      cout << "StThreeVectorF::bad() = " << ibad << endl;
      cout << "Last node had:" << endl;
      cout << "nodpos        " << nodpos << endl;
      abort();
  }
  StTrackGeometry* geometry =new StHelixModel(short(getCharge(node)),
					      getPhi(node),
					      fabs(getCurvature(node)),
					      getDip(node),
					      nodpos, 
					      getMom(node), 
					      getHelicity(node));

  if (outer)
    gTrack->setOuterGeometry(geometry);
  else
    gTrack->setGeometry(geometry);


  return;
}

//_____________________________________________________________________________
// void StvStEventFiller::fillTopologyMap(StTrack* gTrack, const StvTrack* track){
// 	cout << "StvStEventFiller::fillTopologyMap()" << endl;
//     int map1,map2;
//     map1 = map2 = 0;
//     // change: add code to set the bits appropriately here

//     StTrackTopologyMap topomap(map1,map2);
//     gTrack->setTopologyMap(topomap);
//     return;
// }

//_____________________________________________________________________________
void StvStEventFiller::fillFitTraits(StTrack* gTrack, const StvTrack* track){
  // mass
  // this makes no sense right now... double massHyp = track->getMass();  // change: perhaps this mass is not set right?
  unsigned short geantIdPidHyp = 9999;
  //if (.13< massHyp<.14) 
  geantIdPidHyp = 9;
  // chi square and covariance matrix, plus other stuff from the
  // innermost track node
  const StvNode* node = track->GetNode(StvTrack::kFirstPoint);
  float x[6],covMFloat[15];
  getTpt(node,x,covMFloat);
  float chi2[2];
  //get chi2/dof
  chi2[0] = track->GetXi2();  
  chi2[1] = -999; // change: here goes an actual probability, need to calculate?
  // December 04: The second element of the array will now hold the incremental chi2 of adding
  // the vertex for primary tracks
  if (gTrack->type()==primary) {
    assert(!node->GetDetId());
    chi2[1]=node->GetXi2();
  }
    
  // setFitTraits uses assignment operator of StTrackFitTraits, which is the default one,
  // which does a memberwise copy.  Therefore, constructing a local instance of 
  // StTrackFitTraits is fine, as it will get properly copied.
  StTrackFitTraits fitTraits(geantIdPidHyp,0,chi2,covMFloat);
  // Now we have to use the new setters that take a detector ID to fix
  // a bug.  There is no encoding anymore.

  int dets[kMaxDetectorId][3]; 
  getAllPointCount(track,dets);

  for (int i=1;i<kMaxDetectorId;i++) {
    if (!dets[i][2]) continue;
    fitTraits.setNumberOfFitPoints((unsigned char)dets[i][2],(StDetectorId)i);
  }
  if (gTrack->type()==primary) {
     fitTraits.setPrimaryVertexUsedInFit(true);
  }
  gTrack->setFitTraits(fitTraits);
  return;
}

///_____________________________________________________________________________
/// data members from StEvent/StTrack.h
///  The track flag (mFlag accessed via flag() method) definitions with ITTF 
///(flag definition in EGR era can be found at  http://www.star.bnl.gov/STAR/html/all_l/html/dst_track_flags.html)
///
///  mFlag=zxyy, where  z = 1 for pile up track in TPC (otherwise 0) 
///                     x indicates the detectors included in the fit and 
///                    yy indicates the status of the fit. 
///  Positive mFlag values are good fits, negative values are bad fits. 
///
///  The first digit indicates which detectors were used in the refit: 
///
///      x=1 -> TPC only 
///      x=3 -> TPC       + primary vertex 
///      x=5 -> SVT + TPC 
///      x=6 -> SVT + TPC + primary vertex 
///      x=7 -> FTPC only 
///      x=8 -> FTPC      + primary 
///      x=9 -> TPC beam background tracks            
///
///  The last two digits indicate the status of the refit: 
///       = +x01 -> good track 
///
///       = -x01 -> Bad fit, outlier removal eliminated too many points 
///       = -x02 -> Bad fit, not enough points to fit 
///       = -x03 -> Bad fit, too many fit iterations 
///       = -x04 -> Bad Fit, too many outlier removal iterations 
///       = -x06 -> Bad fit, outlier could not be identified 
///       = -x10 -> Bad fit, not enough points to start 
///
///       = -x11 -> Short track pointing to EEMC

void StvStEventFiller::fillFlags(StTrack* gTrack) {
  if (gTrack->type()==global) {
    gTrack->setFlag(101); //change: make sure flag is ok
  }
  else if (gTrack->type()==primary) {
    gTrack->setFlag(301);
  }
  StTrackFitTraits& fitTrait = gTrack->fitTraits();
  //int tpcFitPoints = fitTrait.numberOfFitPoints(kTpcId);
  int svtFitPoints = fitTrait.numberOfFitPoints(kSvtId);
  int ssdFitPoints = fitTrait.numberOfFitPoints(kSsdId);
  int pxlFitPoints = fitTrait.numberOfFitPoints(kPxlId);
  int istFitPoints = fitTrait.numberOfFitPoints(kIstId);
  //  int totFitPoints = fitTrait.numberOfFitPoints();
  /// In the flagging scheme, I will put in the cases for
  /// TPC only, and TPC+SVT (plus their respective cases with vertex)
  /// Ftpc case has their own code and SSD doesn't have a flag...

  // first case is default above, tpc only = 101 and tpc+vertex = 301
  // next case is:
  // if the track has svt points, it will be an svt+tpc track
  // (we assume that the ittf tracks start from tpc, so we don't
  // use the "svt only" case.)
  if (svtFitPoints+ssdFitPoints+pxlFitPoints+istFitPoints>0) {
      if (gTrack->type()==global) {
	  gTrack->setFlag(501); //svt+tpc
      }
      else if (gTrack->type()==primary) {
	  gTrack->setFlag(601); //svt+tpc+primary
      }
  }
  const StTrackDetectorInfo *dinfo = gTrack->detectorInfo();
  if (dinfo) {
    Int_t NoTpcFitPoints = dinfo->numberOfPoints(kTpcId);
    Int_t NoFtpcWestId   = dinfo->numberOfPoints(kFtpcWestId);
    Int_t NoFtpcEastId   = dinfo->numberOfPoints(kFtpcEastId);
    // Check that it could be TPC pile-up track, i.e. in the same half TPC (West East) 
    // there are more than 2 hits with wrong Z -position
    Int_t flag = TMath::Abs(gTrack->flag());
    if (NoTpcFitPoints >= 11) {
      const StTrackDetectorInfo *dinfo = gTrack->detectorInfo();
      const StPtrVecHit& hits = dinfo->hits(kTpcId);
      Int_t Nhits = hits.size();
      Int_t NoWrongSignZ = 0;
      for (Int_t i = 0; i < Nhits; i++) {
	const StTpcHit *hit = (StTpcHit *) hits[i];
	if ((hit->position().z() < -1.0 && hit->sector() <= 12) ||
	    (hit->position().z() >  1.0 && hit->sector() >  12)) NoWrongSignZ++;
      }
      if (NoWrongSignZ >= 2) 
	gTrack->setFlag((flag%1000) + 1000); // +1000
    }
    if (NoTpcFitPoints < 11 && NoFtpcWestId < 5 && NoFtpcEastId < 5) { 
      // hadrcoded number correspondant to  __MIN_HITS_TPC__ 11 in StMuFilter.cxx
      //keep most sig. digit, set last digit to 2, and set negative sign
      gTrack->setFlag(-(((flag/100)*100)+2)); // -x02 
      if (gTrack->geometry()) {
	const StThreeVectorF &momentum = gTrack->geometry()->momentum();
	if (momentum.pseudoRapidity() > 0.5) {
	  const StTrackDetectorInfo *dinfo = gTrack->detectorInfo();
	  const StPtrVecHit& hits = dinfo->hits();
	  Int_t Nhits = hits.size();
	  for (Int_t i = 0; i < Nhits; i++) {
	    const StHit *hit = hits[i];
	    if (hit->position().z() > 150.0) {
	      gTrack->setFlag((((flag/100)*100)+11)); // +x11 
	      return;
	    }
	  }
	}
      }
    }
  }
}
//_____________________________________________________________________________
void StvStEventFiller::fillTrack(StTrack* gTrack, const StvTrack* track,StTrackDetectorInfo* detInfo )
{

  //cout << "StvStEventFiller::fillTrack()" << endl;
  // encoded method = 16 bits = 12 fitting and 4 finding, for the moment use:
  // kKalmanFitId
  // bit 15 for finding, (needs to be changed in StEvent).
  // change: make sure bits are ok, are the bits set up one in each position and nothing else?
  // this would mean that the encoded method is wasting space!
  // the problem is that in principle there might be combinations of finders for each tracking detector
  // but the integrated tracker will use only one for all detectors maybe
  // so need this bit pattern:
  // finding 100000000000     
  // fitting             0010 
  //            32768    +    2 = 32770;
  //
  // above is no longer used, instead use kITKalmanfitId as fitter and tpcOther as finding method

  gTrack->setEncodedMethod(mStvEncoded);
  double tlen = track->GetLength();
  assert(tlen >0.0 && tlen<1000.);
  gTrack->setLength(tlen);// someone removed this, grrrr!!!!
 
  // Follow the StDetectorId.h enumerations...
  // can't include them from here in order not to
  // create a package dependence...
  int dets[kMaxDetectorId][3];
  getAllPointCount(track,dets);
  for (int i=1;i<kMaxDetectorId;i++) {
    if(!dets[i][0]) continue;
    gTrack->setNumberOfPossiblePoints((unsigned char)dets[i][0],(StDetectorId)i);
  }

  fillGeometry(gTrack, track, false); // inner geometry
  fillGeometry(gTrack, track, true ); // outer geometry
  fillFitTraits(gTrack, track);
  gTrack->setDetectorInfo(detInfo);
  StuFixTopoMap(gTrack);
  fillFlags(gTrack);
  if (!track->GetNode(StvTrack::kPrimPoint)) fillDca(gTrack,track);
  return;
}
//_____________________________________________________________________________
bool StvStEventFiller::accept(const StvTrack* track)
{
//  int nPossiblePoints = track->getMaxPointCount(0);
//  int nMeasuredPoints = track->getPointCount   (0);
    int nFittedPoints   = track->GetNHits();
    if (nFittedPoints  <  5 )					return 0;
    if(track->GetLength()<=0) 				return 0; 
    // insert other filters for riff-raff we don't want in StEvent here.
    

    return 1;
}
//_____________________________________________________________________________
double StvStEventFiller::impactParameter(StTrack* track, StThreeVectorD &vertex) 
{
  StPhysicalHelixD helix = track->geometry()->helix();

  //cout <<"PHelix: "<<helix<<endl;
  return helix.distance(vertex);
}
//_____________________________________________________________________________
void StvStEventFiller::fillDca(StTrack* stTrack, const StvTrack* track)
{
  StGlobalTrack *gTrack = dynamic_cast<StGlobalTrack*>(stTrack);
  assert(gTrack);

  const StvNode *tNode = track->GetNode(StvTrack::kDcaPoint);
  if (!tNode) return;
  const StvNodePars &pars = tNode->GetFP(); 
  const StvFitErrs  &errs = tNode->GetFE();
  StvImpact myImp;
  pars.GetImpact(&myImp,&errs);

  StDcaGeometry *dca = new StDcaGeometry;
  gTrack->setDcaGeometry(dca);
  dca->set(&myImp.mImp,&myImp.mImpImp);

}
//_____________________________________________________________________________
void StvStEventFiller::FillStHitErr(StHit *hh,const StvNode *node)
{
#if 0
  double stiErr[6],stErr[6];
  memcpy(stiErr,node->hitErrs(),sizeof(stiErr));
  double alfa = node->getAlpha();
  double c = cos(alfa);
  double s = sin(alfa);
  double T[3][3]={{c,-s, 0}
                 ,{s, c, 0}
		 ,{0, 0, 1}};
  
  TCL::trasat(T[0],stiErr,stErr,3,3);
  StThreeVectorF f3(sqrt(stErr[0]),sqrt(stErr[2]),sqrt(stErr[5]));
  hh->setPositionError(f3);
#endif //0
}
//_____________________________________________________________________________
void StvStEventFiller::fillPulls(const StvTrack* track, int gloPri) 
{
  //cout << "StvStEventFiller::fillDetectorInfo() -I- Started"<<endl;
  if (!mPullEvent) return;
  if (gloPri && !track->IsPrimary()) return;
  int dets[kMaxDetectorId][3];
  getAllPointCount(track,dets);
  StvPullTrk aux;
  aux.mVertex = (unsigned char)track->IsPrimary();
  aux.mTrackNumber=mTrackNumber;
  aux.nAllHits = dets[0][2];
  aux.nTpcHits = dets[kTpcId][2];
  aux.nSsdHits = dets[kSsdId][2];
  aux.nFtpcHits = dets[kFtpcEastId][2]+dets[kFtpcWestId][2];
  aux.mL       = (unsigned char)track->GetLength();
  aux.mChi2    = track->GetXi2();
  StvTrack::EPointType pty =  ( !gloPri) ? StvTrack::kDcaPoint : StvTrack::kPrimPoint;
  const StvNode *node = track->GetNode(pty);
  assert(node);
  const StvNodePars &fp = node->GetFP();


  aux.mCurv    = fp._curv;
  aux.mPt      = fp.getPt();
  aux.mPsi     = fp._psi;
  aux.mDip     = atan(fp._tanl);
  StThreeVectorD v3(fp.P);
  aux.mRxy     = v3.perp();
  aux.mPhi     = v3.phi();
  aux.mZ       = v3.z();
  mPullEvent->Add(aux,gloPri);



  double len=0,preRho,preXy[2]; int myNode=0;
  for (StvNodeConstIter tNode=track->begin();tNode!=track->end();++tNode) 
  {
      const StvNode *node = (*tNode);
      StvHit *stiHit = node->GetHit();
      if (!stiHit)		continue;

      if (node->GetXi2()>1000)  continue;

      StHit *hh = (StHit*)stiHit->stHit();
      const StvNodePars &fp = node->GetFP();
      double dL = (stiHit->x_g()[0]-fp._x)*fp._cosCA 
	        + (stiHit->x_g()[1]-fp._y)*fp._sinCA;
      double myX = fp._x+dL*fp._cosCA;
      double myY = fp._y+dL*fp._sinCA;
      if (myNode) {
        dL = sqrt(pow(preXy[0]-myX,2)+pow(preXy[1]-myY,2));
        double rho = 0.5*(preRho+fabs(fp._curv));
        if (rho*dL>0.01) dL = 2*asin(0.5*dL*rho)/rho;
	len+=dL; 
      }
      myNode++;preXy[0]=myX; preXy[1]=myY; preRho = fabs(fp._curv);

      fillPulls(len,hh,stiHit,node,track,dets,gloPri);
      

      if (gloPri) continue;
      fillPulls(len,hh,stiHit,node,track,dets,2);
  }
}
//_____________________________________________________________________________
 void StvStEventFiller::fillPulls(double len
                                 ,StHit *stHit,const StvHit *stiHit
                                 ,const StvNode *node
				 ,const StvTrack     *track
                                 ,int dets[1][3],int gloPriRnd)
{
  double x,y,z,r;


//const StvFitErrs  &fe = node->GetFE();
  const StvNodePars &fp = node->GetFP(); 
  float yz[2],yzErr[2];
  getDcaLocal(node,yz,yzErr);

  StvPullHit aux;
// local frame
// local HIT
  aux.mVertex = (unsigned char)track->IsPrimary();
//   aux.nHitCand = node->getHitCand();
//   aux.iHitCand = node->getIHitCand();
//   if (!aux.nHitCand)  aux.nHitCand=1;
  aux.lYHit = yz[0];
  aux.lZHit = yz[1];
  aux.lYHitErr = sqrt(node->GetHE()[0]);
  aux.lZHitErr = sqrt(node->GetHE()[2]);
  aux.lYPulErr = yzErr[0];
  aux.lZPulErr = yzErr[1];
  aux.lYPul = aux.lYHit/aux.lYPulErr; if (fabs(aux.lYPul)>10) aux.lYPul=0;
  aux.lZPul = aux.lZHit/aux.lZPulErr; if (fabs(aux.lZPul)>10) aux.lZPul=0;
  aux.lLen = len;

// 		global frame

//		global Hit
  const StHitPlane *hitPlane = stiHit->detector();
  assert(hitPlane);
  const float *n = hitPlane->GetDir(stiHit->x_g())[0];
  aux.gPhiHP = atan2(n[1],n[0]);
  aux.gLamHP = asin(n[2]);

  x = stiHit->x_g()[0]; y = stiHit->x_g()[1]; z = stiHit->x_g()[2];
  r = sqrt(x*x+y*y);

  aux.gRHit = r;
  aux.gPHit = atan2(y,x);aux.gPHit = NICE(aux.gPHit);
  aux.gZHit = z;


//		global Fit
  x = fp._x; y = fp._y;z = fp._z;
  r = sqrt(x*x+y*y);
  aux.gRFit = r;
  aux.gPFit = (atan2(y,x));aux.gPFit=NICE(aux.gPFit);
  aux.gZFit = z;

  

  aux.gPsi  = fp._psi;
  aux.gDip  = atan(fp._tanl);

  // invariant
  aux.mCurv   = fp._curv;
  aux.mPt     = fabs(1./fp._ptin);
  aux.mCharge = stHit->charge();
  aux.mChi2   = node->GetXi2();
  aux.mDetector=node->GetDetId();
  aux.mTrackNumber=mTrackNumber;
  aux.nAllHits  = dets[0][2];
  aux.nTpcHits  = dets[kTpcId][2];
  aux.nFtpcHits = dets[kFtpcEastId][2]+dets[kFtpcWestId][2];
  aux.nSsdHits  = dets[kSsdId][2];
  mPullEvent->Add(aux,gloPriRnd);

}
//_____________________________________________________________________________
void StvStEventFiller::getAllPointCount(const StvTrack *track,int count[1][3])
{
//  output array actually is count[maxDetId+1][3] 
//  count[0] all detectors
//  count[detId] for particular detector
//  count[detId][0] == number of possible points
//  count[detId][1] == number of measured points
//  count[detId][2] == number of fitted   points
enum {kPP=0,kMP=1,kFP=2};

  memset(count[0],0,(kMaxDetectorId)*3*sizeof(int));
  StvNodeConstIter it;

  for (it=track->begin();it!=track->end();it++){
    const StvNode *node = (*it);
    if (node->GetType() != StvNode::kRegNode) continue;
    int detId= node->GetDetId();
    if (!detId)			continue;
    const StvHit* h = node->GetHit();

//fill possible points
    count[0][kPP]++; count[detId][kPP]++;
    
    if (!h ) 			continue;
//fill measured points
    count[0][kMP]++; count[detId][kMP]++;
    if (!node->GetXi2()>1000) 	continue;
    count[0][kFP]++; count[detId][kFP]++;
  }
}
#endif //0
