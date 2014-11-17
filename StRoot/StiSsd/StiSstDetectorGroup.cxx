#include "StiSsd/StiSstDetectorGroup.h"
#include "StiSsd/StiSsdHitLoader.h"
#include "StiSsd/StiSstDetectorBuilder.h"
#include "StEvent/StEvent.h"


StiSstDetectorGroup::StiSstDetectorGroup(bool active, bool buildIdealGeom)
  : StiDetectorGroup<StEvent>("SSD",
			      active?new StiSsdHitLoader():0,
			      new StiSstDetectorBuilder(active, buildIdealGeom))

{}


StiSstDetectorGroup::~StiSstDetectorGroup()
{}
