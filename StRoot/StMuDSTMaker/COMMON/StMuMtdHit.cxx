#include "StMuMtdHit.h"
#include "StEvent/StEvent.h"
#include "StEvent/StTrack.h"
#include "StEvent/StMtdHit.h"
#include "StEvent/StMtdRawHit.h"

ClassImp(StMuMtdHit)

StMuMtdHit::StMuMtdHit(const StMtdHit *hit){

	mBackLeg = hit->backleg();
	mModule = hit->module();
	mCell  = hit->cell();
    mLeadingEdgeTime = hit->leadingEdgeTime();
    mTrailingEdgeTime = hit->trailingEdgeTime();
	
    mIdTruth  = hit->idTruth();  
	mQuality  = hit->qaTruth(); 
	if(hit->associatedTrack()!=0) mTrackKey = hit->associatedTrack()->key();	
	else mTrackKey = 0;
	
	// rongrong
	mIndex2Primary = -1;
	mIndex2Global = -1;
}

int
StMuMtdHit::backleg() const { return mBackLeg; }

int
StMuMtdHit::module() const { return mModule; }

int
StMuMtdHit::cell() const { return mCell; }

pair<double,double>
StMuMtdHit::leadingEdgeTime() const { return mLeadingEdgeTime; }

pair<double,double>
StMuMtdHit::trailingEdgeTime() const { return mTrailingEdgeTime; }

pair<double,double>
StMuMtdHit::tot() const { return pair<double,double>(mTrailingEdgeTime.first - mLeadingEdgeTime.first, mTrailingEdgeTime.second - mLeadingEdgeTime.second); }

short 
StMuMtdHit::associatedTrackKey() const { return mTrackKey; }

int
StMuMtdHit::idTruth() const { return mIdTruth; }

int
StMuMtdHit::qaTruth() const { return mQuality; }


int
StMuMtdHit::index2Primary() const { return mIndex2Primary; }

int
StMuMtdHit::index2Global() const {return mIndex2Global; }
