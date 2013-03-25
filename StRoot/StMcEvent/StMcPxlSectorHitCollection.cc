/***************************************************************************
 *
 * $Id: StMcPxlSectorHitCollection.cc,v 2.1 2013/03/25 23:50:36 perev Exp $
 * $Log: StMcPxlSectorHitCollection.cc,v $
 * Revision 2.1  2013/03/25 23:50:36  perev
 * Mustafa.Pxl add
 *
 *
 **************************************************************************/
#include "StMcPxlSectorHitCollection.hh"
#include "StMcPxlHit.hh"
static const char rcsid[] = "$Id: StMcPxlSectorHitCollection.cc,v 2.1 2013/03/25 23:50:36 perev Exp $";

ClassImp(StMcPxlSectorHitCollection)

//_____________________________________________________________________________
StMcPxlSectorHitCollection::StMcPxlSectorHitCollection()
{
   /* noop */
}
//_____________________________________________________________________________
StMcPxlSectorHitCollection::~StMcPxlSectorHitCollection()
{
   /* noop */
}

StMcPxlLadderHitCollection*
StMcPxlSectorHitCollection::ladder(unsigned int i)
{
   return (i < kNumberOfLadders) ? &(mLadders[i]) : 0;
}

const StMcPxlLadderHitCollection*
StMcPxlSectorHitCollection::ladder(unsigned int i) const
{
   return (i < kNumberOfLadders) ? &(mLadders[i]) : 0;
}

unsigned int StMcPxlSectorHitCollection::numberOfHits() const
{
   unsigned int sum = 0;
   for (int iLadder = 0; iLadder < kNumberOfLadders; iLadder++)
   {
      for (int iSensor = 0; iSensor < mLadders[iSensor].numberOfSensors(); iSensor++)
      {
         sum += mLadders[iLadder].sensor(iSensor)->hits().size();
      }
   }
   return sum;
}
