/***************************************************************************
 *
 * $Id: StTrackGeometry.cxx,v 2.1 1999/10/13 19:45:41 ullrich Exp $
 *
 * Author: Thomas Ullrich, Sep 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StTrackGeometry.cxx,v $
 * Revision 2.1  1999/10/13 19:45:41  ullrich
 * Initial Revision
 *
 * Revision 2.1  1999/10/13 19:45:41  ullrich
 * Initial Revision
 *
 **************************************************************************/
#include "StTrackGeometry.h"
#include "tables/dst_track.h"

ClassImp(StTrackGeometry)

static const char rcsid[] = "$Id: StTrackGeometry.cxx,v 2.1 1999/10/13 19:45:41 ullrich Exp $";

StTrackGeometry::StTrackGeometry() {/* noop */}

StTrackGeometry::StTrackGeometry(const dst_track_st&)   {/* noop */}

StTrackGeometry::~StTrackGeometry() { /* noop */ }
