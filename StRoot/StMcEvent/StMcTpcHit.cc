/***************************************************************************
 *
 * $Id: StMcTpcHit.cc,v 2.1 1999/11/19 19:06:33 calderon Exp $
 * $Log: StMcTpcHit.cc,v $
 * Revision 2.1  1999/11/19 19:06:33  calderon
 * Recommit after redoing the files.
 *
 * Revision 2.0  1999/11/17 02:12:16  calderon
 * Completely revised for new StEvent
 *
 * Revision 1.3  1999/09/23 21:25:53  calderon
 * Added Log & Id
 * Modified includes according to Yuri
 *
 *
 **************************************************************************/
#include "StMcTpcHit.hh"
#include "StMcTrack.hh"
#include "tables/St_g2t_tpc_hit_Table.h"  

static const char rcsid[] = "$Id: StMcTpcHit.cc,v 2.1 1999/11/19 19:06:33 calderon Exp $";

StMemoryPool StMcTpcHit::mPool(sizeof(StMcTpcHit));

StMcTpcHit::StMcTpcHit() { /* noop */ }

StMcTpcHit::StMcTpcHit(const StThreeVectorF& p,
		       const float de, const float ds,
		       StMcTrack* parent)  : StMcHit(p, de, ds, parent)
{ /* noop */ }

StMcTpcHit::StMcTpcHit(g2t_tpc_hit_st* pt)
{
  mdE = pt->de;
  mdS = pt->ds;
  // Decode position.
  mPosition.setX(pt->x[0]); 
  mPosition.setY(pt->x[1]);
  mPosition.setZ(pt->x[2]);

  // The Local and Pad coordinates will be filled in the maker,
  // since they use the coordinate transforms.
  
}

StMcTpcHit::~StMcTpcHit() {/* noop */}




