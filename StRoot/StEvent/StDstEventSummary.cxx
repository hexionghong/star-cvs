/***************************************************************************
 *
 * $Id: StDstEventSummary.cxx,v 1.1 1999/01/30 03:58:05 fisyak Exp $
 *
 * Author: Thomas Ullrich, Jan 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StDstEventSummary.cxx,v $
 * Revision 1.1  1999/01/30 03:58:05  fisyak
 * Root Version of StEvent
 *
 * Revision 1.4  1999/04/28 22:27:30  fisyak
 * New version with pointer instead referencies
 *
 * Revision 1.1  1999/01/15 22:53:33  wenaus
 * version with constructors for table-based loading
 *
 **************************************************************************/
#include "StDstEventSummary.h"
#ifdef __ROOT__

static const Char_t rcsid[] = "$Id: StDstEventSummary.cxx,v 1.1 1999/01/30 03:58:05 fisyak Exp $";
#endif

ClassImp(StDstEventSummary)

StDstEventSummary::StDstEventSummary() { /* noop */ }

StDstEventSummary::~StDstEventSummary() { /* noop */ }
    
