//
// Pibero Djawotho <pibero@comp.tamu.edu>
// Texas A&M University Cyclotron Institute
// 7 Jan 2009
//

#ifndef DSM_LAYER_B101_2009_HH
#define DSM_LAYER_B101_2009_HH

#include "DSMLayer.hh"
#ifdef __ROOT__
#include "RTS/trg/include/trgDataDefs.h"
#include "RTS/trg/include/trgConfNum.h"
#else
#include "trgDataDefs.h"
#include "trgConfNum.h"
#endif

struct DSMLayer_B101_2009 : public DSMLayer<TriggerDataBlk> {
  DSMLayer_B101_2009();
  void read(const TriggerDataBlk& event);
  void write(DSMLayer<TriggerDataBlk>& layer);
  void run();
};

#endif	// DSM_LAYER_B101_2009_HH
