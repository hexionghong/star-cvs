/***************************************************************************
 *
 * $Id: StRichSpectraMaker.cxx,v 1.5 2001/11/21 20:36:07 lasiuk Exp $
 *
 * Author:  bl
 ***************************************************************************
 *
 * Description: RICH offline software:
 *              StRichSpectraMaker.cxx - ROOT/STAR Maker for offline chain.
 *              Uses the information in the pidtraits to produce a spectrum
 ***************************************************************************
 *
 * $Log: StRichSpectraMaker.cxx,v $
 * Revision 1.5  2001/11/21 20:36:07  lasiuk
 * azimuth angle calculation, trace and retracing algorithms, rotation
 * matrices, clean up intersection calculation.  Addition of quick
 * rings for graphics, change definition of ntuples, and
 * removal of old PID method
 *
 * Revision 1.4  2001/08/22 19:33:35  lasiuk
 * remove trace of StPairD, and move some include files that
 * should ease parsing of CINT
 *
 * Revision 1.3  2001/08/21 17:58:34  lasiuk
 * for 2000 analysis
 *
 * Revision 1.2  2001/02/25 22:11:46  lasiuk
 * quality assessment
 *
 * Revision 1.1  2000/12/12 21:35:08  lasiuk
 * Initial Revision
 *
 **************************************************************************/
#include "StRichDisplayActivate.h"
#include "StRichSpectraMaker.h"

//#define WITH_GEANT_INFO 1
//#define P00hm 0

#include <iostream.h>
#include <fstream.h>
#include <assert.h>
#include <vector>

#include "StChain.h"
#include "St_DataSetIter.h"

#include "StGlobals.hh"
#include "StThreeVector.hh"
#include "StPhysicalHelixD.hh"
#include "SystemOfUnits.h"
#ifndef ST_NO_NAMESPACES
using namespace units;
using std::vector;
using std::unique;
#endif

// StEvent
#include "StEventTypes.h"
#include "StRichPidTraits.h"
#include "StRichPid.h"
#include "StRichPhotonInfo.h" // must be in StEventTypes.h

#ifdef RICH_WITH_PAD_MONITOR
#include "StRrsMaker/StRichSinglePixel.h"
#include "StRichDisplayMaker/StRichPadMonitor.h"
#endif

// Database
#include "StRrsMaker/StRichGeometryDb.h"
#include "StRrsMaker/StRichMomentumTransform.h"
#include "StRrsMaker/StRichCoordinateTransform.h"
#include "StRrsMaker/StRichCoordinates.h"

// Internal Rch
#include "StRchMaker/StRichSimpleHit.h"
#include "StRichPIDMaker/StRichTrack.h"

// SpectraMaker
#include "StRichRayTracer.h"
#include "StRichCerenkovHistogram.h"

// g2t tables
// $STAR/pams/sim/idl
#include "tables/St_g2t_tpc_hit_Table.h"
#include "tables/St_g2t_rch_hit_Table.h"
#include "tables/St_g2t_track_Table.h"
#include "StRichGeantCalculator.h"

ClassImp(StRichSpectraMaker) // macro
//-----------------------------------------------------------------

void dump(g2t_rch_hit_st *rch_hit, g2t_track_st *track)
{
    cout << "StRichSpectraMaker::dump()" << endl;
    cout << " de=        " << rch_hit->de << endl;
    cout << " id=        " << rch_hit->id << endl;
    cout << " volume_id= " << rch_hit->volume_id << endl;
    StThreeVectorD p(rch_hit->p[0], rch_hit->p[1], rch_hit->p[2]); 
    cout << " p=         " << p << endl;
    cout << " track_p=   " << rch_hit->track_p << endl;
    cout << "   track[" << (rch_hit->track_p) << "].ge_pid: " << track[(rch_hit->track_p)].ge_pid << endl;
    cout << "   track[" << (rch_hit->track_p) << "].next_parent_p: " << track[(rch_hit->track_p)].next_parent_p << endl;

    cout << "   track[" << (track[(rch_hit->track_p)].next_parent_p) << "].ge_pid: "
	 <<     track[(track[rch_hit->track_p].next_parent_p)].ge_pid << endl;

    cout << "   track[" << (rch_hit->track_p) << "].is_showershower?" << track[rch_hit->track_p].is_shower << endl;
    StThreeVectorD parentp(track[(rch_hit->track_p)].p[0],
			   track[(rch_hit->track_p)].p[1],
			   track[(rch_hit->track_p)].p[2]);
    cout << "   track[" << (rch_hit->track_p) << "].p: " << parentp << endl;
    StThreeVectorD nextParentp(track[(track[(rch_hit->track_p)].next_parent_p)].p[0],
			       track[(track[(rch_hit->track_p)].next_parent_p)].p[1],
			       track[(track[(rch_hit->track_p)].next_parent_p)].p[2]);
    cout << "   track[" << (track[(rch_hit->track_p)].next_parent_p) << "].p: " << nextParentp << endl;
    cout << "   track[" << (rch_hit->track_p) << "].ptot: " << track[(rch_hit->track_p)].ptot << endl;
 }

void dumpMinus1(g2t_rch_hit_st *rch_hit, g2t_track_st *track)
{
    cout << "StRichSpectraMaker::dumpMinus1()" << endl;
    cout << " de=        " << rch_hit->de << endl;
    cout << " id=        " << rch_hit->id << endl;
    cout << " volume_id= " << rch_hit->volume_id << endl;
    StThreeVectorD p(rch_hit->p[0], rch_hit->p[1], rch_hit->p[2]); 
    cout << " p=         " << p << endl;
    cout << " track_p=   " << rch_hit->track_p << endl;
    int index = (rch_hit->track_p-1);
    cout << "   track[" << index << "].ge_pid: " << track[index].ge_pid << endl;
    cout << "   track[" << index << "].is_shower: " << track[index].is_shower << endl;
    cout << "   track[" << index << "].next_parent_p: " << track[index].next_parent_p << endl;

    int index2 = track[index].next_parent_p-1;
    cout << "   track[" << index2 << "].ge_pid: "          << track[index2].ge_pid << endl;
    cout << "   track[" << index2 << "].is_showershower? " << track[index2].is_shower << endl;
    StThreeVectorD pp(track[index].p[0],
		      track[index].p[1],
		      track[index].p[2]);
    cout << "   track[" << index << "].p: " << pp << endl;
    StThreeVectorD nextParentp(track[index2].p[0],
			       track[index2].p[1],
			       track[index2].p[2]);
    cout << "   track[" << index2 << "].p: " << nextParentp << endl;
    cout << "   track[" << index2 << "].ptot: " << track[index2].ptot << endl;
 }

//-----------------------------------------------------------------

StRichSpectraMaker::StRichSpectraMaker(const char *name)
    : StMaker(name)
{
    //
    // Switches in the .h file
    //

    mNumberOfEvents = 0;
    mNumberOfGood2GevTracks = 0;
}

//-----------------------------------------------------------------

StRichSpectraMaker::~StRichSpectraMaker() {/* nopt */}

//-----------------------------------------------------------------

Int_t StRichSpectraMaker::Init() {
    cout << "StRichSpectraMaker::init()" << endl;
    
#ifdef RICH_SPECTRA_HISTOGRAM
    //mFile = new TFile("/star/rcf/scratch/lasiuk/theta/Spectra.root","RECREATE","Pid Ntuples");
    mFile = new TFile("./Spectra00hm.root","RECREATE","Pid Ntuples");
    mFile->SetFormat(1);

    
    //
    // this is at the event level
    //
    mEvt = new TNtuple("evt","Event Characteristics","vx:vy:vz:nPrim:nRich:ctb:zdc");
    
    mTrack = new TNtuple("track","Identified Tracks","zvtx:p:px:py:pz:pt:eta:sdca2:sdca3:x:y:dx:dy:theta:sig:mp:mass2:q:flag");
    //
    // this is at the Cerenkov photon level
    //
    mCerenkov = new TNtuple("cer","angle","p:px:py:pz:alpha:phx:phy:l3:phi:ce:stat:dx:dy:id:q:vz");

    //
    // geant
    // n-numberofhits, np-numberofPhotons
    mSimEvent = new TNtuple("sevt","evtii","n:np");
    // e-energy, l-lambda
    mSim = new TNtuple("sim","photon","e:l");
#endif

    this->initCutParameters();

    mGeometryDb = StRichGeometryDb::getDb();

    mAverageRadiationPlanePoint =
	StThreeVectorF(0.,
		       0.,
		       (mGeometryDb->proximityGap() +
			mGeometryDb->quartzDimension().z() +
			mGeometryDb->radiatorDimension().z()/2.));

    mGlobalRichNormal =
	StThreeVectorF(mGeometryDb->normalVectorToPadPlane().x(),
		       mGeometryDb->normalVectorToPadPlane().y(),
		       mGeometryDb->normalVectorToPadPlane().z());

    PR(mAverageRadiationPlanePoint);
    PR(mGlobalRichNormal);

    mTopRadiator = StThreeVectorF(0.,
				  0.,
				  (mGeometryDb->proximityGap() +
				   mGeometryDb->quartzDimension().z() +
				   mGeometryDb->radiatorDimension().z()));
    mBottomRadiator = StThreeVectorF(0.,
				     0.,
				     (mGeometryDb->proximityGap() +
				      mGeometryDb->quartzDimension().z()));

    cout << "*** Calculated Radiation Point: \nProximity: " <<
	(mGeometryDb->proximityGap()) << "\nQuartz: " <<
	(mGeometryDb->quartzDimension().z()) << "\nRadiator/2: " <<
	(mGeometryDb->radiatorDimension().z()/2.) << "\nTotal: " <<
	(mGeometryDb->proximityGap() +
	 mGeometryDb->quartzDimension().z() +
	 mGeometryDb->radiatorDimension().z()/2.) << endl;
    
    //
    // Coordinate and Momentum Transformation Routines
    //
    mTransform = StRichCoordinateTransform::getTransform(mGeometryDb);
    mMomTform  = StRichMomentumTransform::getTransform(mGeometryDb);  

    //
    // make the particles
    //
    mPion = StPionMinus::instance();
    mKaon = StKaonMinus::instance();
    mProton = StAntiProton::instance();

//     mMeanWavelength = 171.*nanometer;
//     mMeanWavelength = 183.*nanometer;
    mMeanWavelength = 176.*nanometer;
    PR(mMeanWavelength/nanometer);
    
    StRichMaterialsDb* materialsDb = StRichMaterialsDb::getDb();
    mIndex = materialsDb->indexOfRefractionOfC6F14At(mMeanWavelength);
    PR(mIndex);

    ////////////////////////////////////////////////////////////////////////
    double ii;
    for(ii=160.*nanometer; ii<220.*nanometer; ii+=1.*nanometer)
 	cout << (ii/nanometer) << " ";
    cout << "\n nc6f14" << endl;
    for(ii=160.*nanometer; ii<220.*nanometer; ii+=1.*nanometer)
 	cout << (materialsDb->indexOfRefractionOfC6F14At(ii)) << " ";
    cout << "\n lc6f14" << endl;
    for(ii=160.*nanometer; ii<220.*nanometer; ii+=1.*nanometer)
 	cout << (materialsDb->absorptionCoefficientOfC6F14At(ii)) << " ";
    cout << "\n nquartz" << endl;
    for(ii=160.*nanometer; ii<220.*nanometer; ii+=1.*nanometer)
 	cout << (materialsDb->indexOfRefractionOfQuartzAt(ii)) << " ";
    cout << "\n lquartz" << endl;
    for(ii=160.*nanometer; ii<220.*nanometer; ii+=1.*nanometer)
 	cout << (materialsDb->absorptionCoefficientOfQuartzAt(ii)) << " ";
    cout << "\n csiqe" << endl;
    for(ii=160.*nanometer; ii<220.*nanometer; ii+=1.*nanometer)
 	cout << (materialsDb->quantumEfficiencyOfCsIAt(ii)) << " ";
    cout << endl;
    
    this->printCutParameters();

    mTracer = new StRichRayTracer(mMeanWavelength);
    mHistogram = new StRichCerenkovHistogram();

    
    return StMaker::Init();
}

////////////////////////////////////////////////////////////////////////////////////////
void StRichSpectraMaker::initCutParameters() {
    //
    // Event Level
    //
    mVertexWindow = 30.*centimeter;
    
    //
    // Track Level
    //
    mPtCut = 0.*GeV; // GeV/c
    mEtaCut = 0.5; 
    mLastHitCut = 160.0*centimeter;
    mDcaCut = 3.0*centimeter;
    mFitPointsCut = 20;
    mPathCut = 500*centimeter;
    mPadPlaneCut = 2.0*centimeter;
    mRadiatorCut = 2.0*centimeter;

    mMomentumThreshold = 1.5*GeV;
    mMomentumLimit = 3.*GeV;
}

//-----------------------------------------------------------------

Int_t StRichSpectraMaker::Make() {
    cout << "StRichSpectraMaker::Make()" << endl;
    mNumberOfEvents++;
    //
    // ptr initialization for StEvent
    //
    mTheRichCollection = 0;

#ifdef WITH_GEANT_INFO
    StRichGeantCalculator calculator;
    ////////////////////////////////// <-------------------------
    if (!m_DataSet->GetList())  {
	St_DataSetIter geant(GetDataSet("geant"));
	St_g2t_rch_hit *g2t_rch_hit =
	    static_cast<St_g2t_rch_hit *>(geant("g2t_rch_hit"));

	if (!g2t_rch_hit) {
	    // For backwards compatibility look in dst branch
	    cout << "look in dst" << endl;
	    St_DataSetIter dstDstI(GetDataSet("dst"));
	    g2t_rch_hit = static_cast<St_g2t_rch_hit*>(dstDstI("g2t_rch_hit"));
	}
	if(!g2t_rch_hit){
	    cout << "StRichSpectraMaker::Make()";
	    cout << "\tNo g2t_rch_hit pointer";
	    cout << "\treturn from StRichSpectraMaker::Make()" << endl;
	    return kStWarn;
	}

	
	g2t_rch_hit_st *rch_hit     =  g2t_rch_hit->GetTable();
	int numberOfRichHits = g2t_rch_hit->GetNRows();
	PR(numberOfRichHits);

	St_g2t_track *g2t_track =
	    static_cast<St_g2t_track *>(geant("g2t_track"));

	
	if(!g2t_track){
	    cout << "StRichSpectraMaker::Make()\n";
	    cout << "\tNo g2t_track pointer\n";
	    cout << "\treturn from StRrsMaker::Make()" << endl;
	    return kStWarn;
	}
	
	int numberOfTracks          =  g2t_track->GetNRows();
	PR(numberOfTracks);

	g2t_track_st *track =  g2t_track->GetTable();

	 
	float simEventTuple[2];
	simEventTuple[0] = numberOfRichHits;

	
	float simTuple[2];
	int goodPhotons = 0;
 	vector<int> ptrackId;
 	vector<int> ptrackPtr;

	vector<int> ctrackId;
	vector<int> ctrackPtr;
	for(int ii=0; ii<numberOfRichHits; ii++) {

//  	    //dump(rch_hit, track);
//  	    dumpMinus1(rch_hit, track);

	    calculator.process(rch_hit, track);

	    if(rch_hit->de<0) {
// 		calculator.process(rch_hit, track);
		goodPhotons++;
		simTuple[0] = fabs(rch_hit->de);
		simTuple[1] = fabs(1240./rch_hit->de)/1.e9;
#ifdef RICH_SPECTRA_HISTOGRAM
		mSim->Fill(simTuple);
#endif
 		ptrackId.push_back(rch_hit->id);
 		ptrackPtr.push_back(rch_hit->track_p);
	    }
	    else {
		ctrackId.push_back(rch_hit->id);
		ctrackPtr.push_back(rch_hit->track_p);
	    }
	    rch_hit++;

	}
	PR(goodPhotons);
	simEventTuple[1] = goodPhotons;
#ifdef RICH_SPECTRA_HISTOGRAM
	mSimEvent->Fill(simEventTuple);
#endif
	calculator.status();
    }
#endif // WITH_GEANT_INFO
    ////////////////////////////////// <-------------------------
    //
    // Try get StEvent Structure
    //
    mEvent = (StEvent *) GetInputDS("StEvent");

    //
    // Interogate StEvent structure
    //    
    if (!mEvent) {
	cout << "StRichSpectraMaker::Make()\n";
	cout << "\tWARNING!!\n";
	cout << "\tCannot Get the StEvent*\n";
	cout << "\tReturn to chain" << endl;
	return kStWarn;
    }

    //
    // Check the RICH collection
    //
    if(!mEvent->richCollection()) {
	cout << "StRichSpectraMaker::Make()\n";
	cout << "\tWARNING!!\n";
	cout << "\tCannot Get the StRichCollection*\n";
 	cout << "\tReturn to chain" << endl;
 	return kStWarn;
    }
    
    
    mMagField    = .249117*tesla;    
    if (mEvent->summary()) {
	mMagField  = mEvent->summary()->magneticField()*kilogauss;
	PR(mMagField);
	cout << "  B field = " << (mMagField/tesla) << " T" << endl;
    } 
    else {
	cout << "StRichSpectraMaker::Make().\n";
	cout << "\tWARNING!\n";
	cout << "\tCannot get B field from mEvent->summary().\n";
	cout << "\tUse B= " << (mMagField/tesla) << " T" << endl;
    } 

    //
    // Vertex Position
    //
    if(!mEvent->primaryVertex()) {
    	cout << "StRichSpectraMaker::Make()\n";
	cout << "\tWARNING!!\n";
	cout << "\tEvent has no Primary Vertex\n";
	cout << "\tReturn to chain" << endl;
	return kStWarn;
    }
    mVertexPos = mEvent->primaryVertex()->position();
    if(abs(mVertexPos.z())>mVertexWindow) {
	cout << "Vertex out of range...(" << mVertexPos.z() << ")" << endl;
	return kStWarn;
    }
    //
    // Number of tracks to loop over
    //
    mNumberOfPrimaries = mEvent->primaryVertex()->numberOfDaughters();  
    PR(mNumberOfPrimaries);


    
    //
    // does the hit collection exist
    //
    if(mEvent->tpcHitCollection()) {
	cout << "StRichSpectraMaker::Make()\n";
	cout << "\tTpcHit collection exists" << endl;
    }
    else {
	cout << "StRichSpectraMaker::Make()\n";
	cout << "\tWARNING\n";
	cout << "\tTpcHit collection DOES NOT exist!!!!" << endl;
	cout << "\tContinuing..." << endl;
    }

#ifdef RICH_WITH_PAD_MONITOR
    cout << "StRichSpectraMaker::Next Event? <ret>: " << endl;
    char* dir = ".";
    char* name = "event";
    if(mPadMonitor)
	mPadMonitor->printCanvas("dir","name",mNumberOfEvents);
    do {
	if(getchar()) break;
    } while (true);
    
    mPadMonitor = StRichPadMonitor::getInstance(mGeometryDb);
    mPadMonitor->clearAll();
    
    this->drawRichPixels(mEvent->richCollection());
    this->drawRichHits(mEvent->richCollection());
#endif

    this->evaluateEvent();
    this->qualityAssessment();
     
    //
    //
    // The track loop
    //
    //
    float trackTuple[19] = {-999.};
    trackTuple[0] = mVertexPos.z();

    cout << "Looping over " << mNumberOfPrimaries << " primary Tracks" << endl;
    for(size_t ii=0; ii<mNumberOfPrimaries; ii++) { // primaries

//  	cout << "==> Track " << ii << "/" << (mNumberOfPrimaries-1);
	StTrack* track = mEvent->primaryVertex()->daughter(ii);
//  	cout << " p= " << track->geometry()->momentum().mag() << endl;

	//if( !this->checkMomentumWindow(track) ) continue;
// 	if (!this->checkTrack(track)) continue;
	
	//
	// Get the PID traits, if there is an StrichPIDTrait:
	//
	const StPtrVecTrackPidTraits&
	    theRichPidTraits = track->pidTraits(kRichId);

	if(!theRichPidTraits.size()) continue;
  	cout << " (" << theRichPidTraits.size() << ") Pid Traits.   p= ";

	StThreeVectorF trackMomentum = track->geometry()->momentum();
	cout << (track->geometry()->momentum().mag()) << endl;

	trackTuple[1] = abs(trackMomentum);
	trackTuple[2] = trackMomentum.x();
	trackTuple[3] = trackMomentum.y();
	trackTuple[4] = trackMomentum.z();
	trackTuple[5] = trackMomentum.perp();
	trackTuple[6] = trackMomentum.pseudoRapidity();

	//
	// info from the traits
	//
	
	StTrackPidTraits* theSelectedTrait =
	    theRichPidTraits[theRichPidTraits.size()-1];

	if(!theSelectedTrait) {
	    cout << "Error in the Selected Trait\nContinuing..." << endl;
	    continue;
	}
	
	StRichPidTraits *richPidTrait =
	    dynamic_cast<StRichPidTraits*>(theSelectedTrait);

	//
	// this should not be necessary for the
	// next round of production
	//

#ifdef P00hm
	trackTuple[7] = 0.;
	trackTuple[8] = 0.;

	//
	// set value for: "mMipResidual"
	//                "mAssociatedMip"
	//

	if(!this->assignMipResidual(track)) continue;
	
#else
	trackTuple[7] = richPidTrait->signedDca2d();
	trackTuple[8] = richPidTrait->signedDca3d();

	if(!richPidTrait->associatedMip()) continue;
	PR(richPidTrait->associatedMip()->local());
	PR(richPidTrait->mipResidual());
	
	mAssociatedMip = richPidTrait->associatedMip()->local();
	mMipResidual   = richPidTrait->mipResidual();
#endif

#ifdef RICH_WITH_PAD_MONITOR
	//
	// Put a box around the mip and
	// write its momentum and incidnet angle
	//
	
	// 25 = open box
	mPadMonitor->drawMarker(mAssociatedMip, 25, 3);
	mPadMonitor->update();
#endif

	// 	PR(residual);

	//
	//
	//
	trackTuple[9] = mAssociatedMip.x();
	trackTuple[10] = mAssociatedMip.y();
	trackTuple[11] = mMipResidual.x();
	trackTuple[12] = mMipResidual.y();

	this->doIdentification(track);

	unsigned short iflag;
	double cerenkovAngle = mHistogram->cerenkovAngle(&iflag);
	trackTuple[13] = (cerenkovAngle/degree);
	trackTuple[14] = (mHistogram->cerenkovSigma()/degree);
	trackTuple[15] = (mHistogram->cerenkovMostProbable()/degree);

	PR(mHistogram->bestAngle()/radian);
	double beta = 1./(mIndex*cos(cerenkovAngle));
	double gamma = 1./(sqrt(1-sqr(beta)));
	PR(gamma);
	PR(abs(trackMomentum));

	double mass2 = sqr(abs(trackMomentum))/(sqr(gamma)-1);

	trackTuple[16] =  mass2;
	trackTuple[17] = track->geometry()->charge();
	trackTuple[18] = iflag;
#ifdef RICH_SPECTRA_HISTOGRAM
	mTrack->Fill(trackTuple);
#endif
	cout << "okay tuple" << endl;
	
	
    } // loop over the tracks

    cout << "try clear data from histogram" << endl;
    mHistogram->clearData();
    
    return kStOk;
}


//-----------------------------------------------------------------
void StRichSpectraMaker::PrintInfo() 
{
    printf("**************************************************************\n");
    printf("* $Id: StRichSpectraMaker.cxx,v 1.5 2001/11/21 20:36:07 lasiuk Exp $\n");
    printf("**************************************************************\n");
    if (Debug()) StMaker::PrintInfo();
}

//-----------------------------------------------------------------


Int_t StRichSpectraMaker::Finish() {

    cout << "StRichSpectraMaker::Finish()" << endl;

    cout << mNumberOfGood2GevTracks << " Good 2.0-2.5 GeV tracks in" << endl;
    cout << mNumberOfEvents         << " Events." << endl;
	
    this->printCutParameters();

	
#ifdef RICH_SPECTRA_HISTOGRAM
    if(mFile) {
	cout << "StRichSpectraMaker::Finish()\n";
	cout << "\tClose the Histogram files!!!!!!" << endl;

	mFile->Write();
	mFile->Close();
	delete mFile;
	mFile = 0;
    }
#endif

    return StMaker::Finish();
}

// ----------------------------------------------------
bool StRichSpectraMaker::checkTrack(StTrack* track) const {

    //
    // track -- quality cuts
    //       -- momentum and parameter
    //
    bool status = true;
    if (!track) {
	cout << "StRichSpectraMaker::checkTrack()";
	cout << " --> !track" << endl;
	status = false;
    }
    if (track->flag()<0) {
	cout << "StRichSpectraMaker::checkTrack() --> ";
	cout << "track->flag()<0 (" << track->flag() << ")" << endl;
	status = false;
    }

    if (!track->geometry()) {
	cout << "StRichSpectraMaker::checkTrack() --> ";
	cout << "!track->geometry()" << endl;
	status = false;
    }

    if(track->geometry()->helix().distance(mVertexPos)>mDcaCut) {
	cout << "StRichSpectraMaker::checkTrack() --> ";
	cout << "mDcaCut ("
	     << track->geometry()->helix().distance(mVertexPos) << ")" << endl;
	status = false;
    }
	
    if(track->fitTraits().numberOfFitPoints(kTpcId) < mFitPointsCut) {
	cout << "StRichSpectraMaker::checkTrack() --> ";
	cout << "mFitPointsCut ("
	     << track->fitTraits().numberOfFitPoints(kTpcId) << endl;
	status = false;
    }

    if( fabs(track->geometry()->momentum().pseudoRapidity()) > mEtaCut ) {
	cout << "StRichSpectraMaker::checkTrack() --> ";
	cout << "mEtaCut ("
	     << track->geometry()->momentum().pseudoRapidity() << ")" << endl;
	status = false;
    }

    if (track->geometry()->momentum().perp() < mPtCut) {
  	cout << "StRichSpectraMaker::checkTrack() --> ";
	cout << "mPtCut ("
	     << track->geometry()->momentum().perp() << ")" << endl;
	status = false;
    }
    
    return status;
}

// ----------------------------------------------------
bool StRichSpectraMaker::checkMomentumThreshold(StTrack* track) const {

    if (track->geometry()->momentum().mag() > mMomentumThreshold) {
	return true;
    }

    return false;

}

// ----------------------------------------------------
bool StRichSpectraMaker::checkMomentumLimit(StTrack* track) const {

    if (track->geometry()->momentum().mag() < mMomentumLimit) {
	return true;
    }

    return false;

}

// ----------------------------------------------------
bool StRichSpectraMaker::checkMomentumWindow(StTrack* track) const {

    if ( this->checkMomentumThreshold(track) &&
	 this->checkMomentumLimit(track) ) {
	return true;
    }

    return false;

}

// ----------------------------------------------------
float StRichSpectraMaker::expectedNumberOfPhotons(float p, int pid) const
{
    float index2 = mIndex*mIndex;

    float mass;

    switch(pid) {
    case -211:
	mass = mPion->mass();
	break;

    case -321:
	mass = mKaon->mass();
	break;

    case -2212:
	mass = mProton->mass();
	break;

    default:
	cout << "StRichSpectraMaker::expectedNumberOfPhotons()\n";
	cout << "\tWARNING\n";
	cout << "\tBad Pid number (" << pid << ") " << endl;
	return -999;
    }

    float beta2 = p*p/(p*p + mass*mass);

    float fraction = (beta2*index2-1)/(beta2*(index2-1));
    return fraction;
}

// ----------------------------------------------------
bool StRichSpectraMaker::evaluateEvent() {

    float tuple[7];

    tuple[0] = mVertexPos.x();
    tuple[1] = mVertexPos.y();
    tuple[2] = mVertexPos.z();

    // primaries
    tuple[3] = mEvent->primaryVertex()->numberOfDaughters();
#ifdef P00hm
    tuple[4] = -999;
#else
    tuple[4] = mEvent->richCollection()->getTracks().size();
#endif

    tuple[5] = 0; //ctb
    tuple[6] = 0; //zdc ;

    mEvt->Fill(tuple);

    return true;
}

// ----------------------------------------------------
void StRichSpectraMaker::qualityAssessment() {

    cout << "StRichSpectraMaker::qualityAssessment()\n";

#ifndef P00hm 
    //vertex
    PR(mVertexPos);
    PR(mNumberOfPrimaries);
    
    StPtrVecTrack richTracks = mEvent->richCollection()->getTracks();
    PR(richTracks.size());

    for(size_t ii=0; ii<richTracks.size(); ii++) {
	cout << " ptr: " << richTracks[ii] << endl;
	cout << " p:   " << richTracks[ii]->geometry()->momentum().mag() << endl;
	
	const StPtrVecTrackPidTraits&
	    thePidTraits = richTracks[ii]->pidTraits(kRichId);

	PR(thePidTraits.size());

 	for(size_t jj=0; jj<thePidTraits.size(); jj++) {
	    // loop over traits
	    StRichPidTraits* theRichPidTraits =
		dynamic_cast<StRichPidTraits*>(thePidTraits[jj]);

	    if(!theRichPidTraits) {
		cout << "Bad pid traits" << endl;
		continue;
	    }
 	    PR(theRichPidTraits[jj].productionVersion());

 	    if(!theRichPidTraits[jj].associatedMip()) {
 		cout << "\tNo Associated MIP\n";
 		cout << "\tNo MIP Residual" << endl;
 	    }
 	    else {
 		PR(theRichPidTraits[jj].associatedMip()->local());

 		PR(theRichPidTraits[jj].mipResidual());
 		PR(theRichPidTraits[jj].refitResidual());
 	    }
 	    PR(theRichPidTraits[jj].signedDca2d());
 	    PR(theRichPidTraits[jj].signedDca3d());

	    cout << " *** Try get the pids" << endl;
 	    const StSPtrVecRichPid& theRichPids =
 		theRichPidTraits[jj].getAllPids();

	    PR(theRichPids.size());
 	    for(size_t kk=0; kk<theRichPids.size(); kk++) {
 		cout << "kk= " << kk << " ";
 		PR(theRichPids[kk]->getParticleNumber());
		PR(theRichPids[kk]->getMipResidual());
		
		const StSPtrVecRichPhotonInfo& photonInfo =
		    theRichPids[kk]->getPhotonInfo();
		PR(photonInfo.size());

		const StPtrVecRichHit& hits =
		    theRichPids[kk]->getAssociatedRichHits();
		PR(hits.size());
		
 	    }
	    
 	} // jj --> traits
	

	
    }
#endif
    //loop over these tracks
    cout << "========= END ::qualityAssessment =====" << endl;
}

// ----------------------------------------------------
void StRichSpectraMaker::doIdentification(StTrack* track) {

    cout << "\nStRichSpectraMaker::doIdentification()\n";
    mHistogram->clearData();
    
    const StSPtrVecRichHit& richHits = mEvent->richCollection()->getRichHits();
    PR(richHits.size());
    
    const StPtrVecTrackPidTraits&
	thePidTraits = track->pidTraits(kRichId);

    //
    // loop over the PID traits
    // and extract the PIDs
    //

    vector<StRichHit*> theRingHits;
    StThreeVectorF trackMip(-999.,-999.,-999);
    bool doAssociation = false;
    
    for(size_t jj=0; jj<thePidTraits.size(); jj++) {

	StRichPidTraits* theRichPidTraits =
	    dynamic_cast<StRichPidTraits*>(thePidTraits[jj]);

	if(!theRichPidTraits) {
	    cout << "StRichSpectraMaker::doIdentification()\n";
	    cout << "\tBad pid traits.  Continuing..." << endl;
	    continue;
	}
#ifndef P00hm
 	if(!theRichPidTraits[jj].associatedMip()) {
	    cout << "StRichSpectraMaker::doIdentification()\n";
	    cout << "\tNo Associated MIP\n";
	    cout << "\tNo MIP Residual" << endl;
	    if(richHits.size()) {
		cout << "try association" << endl;
		doAssociation = true;
	    }
	    else {
		trackMip =  theRichPidTraits[jj].associatedMip()->local();
	    }
	}
#endif
// 	PR(doAssociation);
	
	const StSPtrVecRichPid& theRichPids =
	    theRichPidTraits[jj].getAllPids();
	    
	for(size_t kk=0; kk<theRichPids.size(); kk++) {

// 	    const StSPtrVecRichPhotonInfo& photonInfo =
// 		theRichPids[kk]->getPhotonInfo();
// 	    PR(photonInfo.size());

// 	    PR(theRichPids[kk]->getConstantAreaCut()/degree);

	    
	    mHistogram->setPhi( max(static_cast<double>(theRichPids[kk]->getConstantAreaCut()),
				    mHistogram->phi()) );

	    cout << "Histogram Phi Cut:\nConstant Area Cut: " <<
		(theRichPids[kk]->getConstantAreaCut()/degree) << "\nPhi: " <<
		mHistogram->phi()/degree << endl;

	    //
	    // temporary only
	    //
	    mHistogram->setPhi(90.*degree);
	    //
	    //
	    //
	    
	    const StPtrVecRichHit& hits =
		theRichPids[kk]->getAssociatedRichHits();

	    if(!hits.size()) {
		cout << "StRichSpectraMaker:doIdentification()\n";
		cout << "\tNo hits in (" << kk << ")...next StRichPid" << endl;
		continue;
	    }

	    for(size_t ll=0; ll<hits.size(); ll++) {
		theRingHits.push_back(hits[ll]);
	    }
	    
// 	    PR(theRingHits.size());
	}
    }
    
    //
    // only the unique hits
    //
//     PR(theRingHits.size());

    sort(theRingHits.begin(),theRingHits.end());
    vector<StRichHit*> uniqueRingHits( theRingHits.begin(),
				       unique(theRingHits.begin(),
					      theRingHits.end()) );

//     PR(uniqueRingHits.size());
    
	    
    StRichTrack extrapolateTrack(track, mMagField/kilogauss);

    //
    // Argh this is wrong!
    //  but that is the way StRichTrack is coded
    //
    StThreeVectorF trackLocalMomentum = extrapolateTrack.getMomentumAtPadPlane()/GeV;
    StThreeVectorF impactPoint = extrapolateTrack.getImpactPoint();
    
    if(doAssociation) {
	extrapolateTrack.assignMIP(&richHits);
	if(!extrapolateTrack.getAssociatedMIP()) {
	    cout << "StRichSpectraMaker::doIdentification()\n";
	    cout << "\tCannot get an associated Track";
	}
	else {
	    trackMip = extrapolateTrack.getAssociatedMIP()->local();
// 	    PR(trackMip);
	}
    }

    StThreeVectorF calculatedRadiationPoint =
	this->calculateRadiationPoint(track, mAverageRadiationPlanePoint);
    
//     PR(trackLocalMomentum/GeV);
//     PR(impactPoint);
//     PR(calculatedRadiationPoint);
//     PR(trackResidual);
    
    float tuple[16];
    
    tuple[0] = abs(trackLocalMomentum);
    tuple[1] = trackLocalMomentum.x();
    tuple[2] = trackLocalMomentum.y();
    tuple[3] = trackLocalMomentum.z();

    PR(calculatedRadiationPoint);
    PR(mAverageRadiationPlanePoint);
    
    mTracer->setTrack(trackLocalMomentum, calculatedRadiationPoint, mAverageRadiationPlanePoint);
    tuple[4] = (mTracer->trackAngle()/degree);

#ifdef RICH_WITH_PAD_MONITOR
    //
    // draw a line that shows the direction of the momentum
    // in the xy plane
    //
    
    StThreeVectorF endLine = mAssociatedMip + 10.*(trackLocalMomentum.unit());
    PR(mAssociatedMip);
    PR(endLine);
    mPadMonitor->drawLine(mAssociatedMip,endLine);

    //
    // draw the rings
    //
    StThreeVectorF topPoint = this->calculateRadiationPoint(track, mTopRadiator);
    StThreeVectorF btmPoint = this->calculateRadiationPoint(track, mBottomRadiator);
    this->drawQuickRing(topPoint, btmPoint);

    //
    // draw the momentum and the inclination
    //
    char txt[50];
    sprintf(txt,"p=%f theta=%f\n",abs(trackLocalMomentum),mTracer->trackAngle()/degree);
    mPadMonitor->drawText(mAssociatedMip.x()+3, mAssociatedMip.y()+3, txt);


    mPadMonitor->update();
#endif

    for(size_t mm=0; mm<uniqueRingHits.size(); mm++) {
	
	StThreeVectorF photonPosition = uniqueRingHits[mm]->local();
// 	PR(photonPosition);
	
// 	double distance = abs(photonPosition - trackMip);
// 	PR(distance);
	
 	mTracer->setPhotonPosition(photonPosition);
#ifdef RICH_WITH_PAD_MONITOR
 	// 4 = open circle
 	mPadMonitor->drawMarker(photonPosition,4,2);
 	mPadMonitor->update();
#endif
	
 	tuple[5] = photonPosition.x();
 	tuple[6] = photonPosition.y();
	
 	float status;
 	double cerenkovAngle;
 	if(mTracer->processPhoton(&cerenkovAngle)) {
//  	    cout << "\n****True...(" << (cerenkovAngle/degree) << ")" << endl;
 	    status = 1;
 	}
 	else {
//  	    cout << "\n****False...(" << (cerenkovAngle/degree) << ")" << endl;
 	    status = 0;
 	}
	
//  	PR(mTracer->cerenkovAngle()/degree);
 	tuple[7] = (mTracer->epsilon());
 	tuple[8] = (mTracer->azimuth()/degree);
 	tuple[9] = (mTracer->cerenkovAngle()/degree);
 	tuple[10] = status;

 	tuple[11] = mMipResidual.x();
 	tuple[12] = mMipResidual.y();
 	tuple[13] = mEvent->id();
	tuple[14] = track->geometry()->charge();
	tuple[15] = mVertexPos.z();
	
 	mCerenkov->Fill(tuple);

	mHistogram->addEntry(StRichCerenkovPhoton(mTracer->cerenkovAngle(),
						  mTracer->azimuth(),
						  uniqueRingHits[mm]));
#ifdef RICH_WITH_PAD_MONITOR
 	// 4 = open circle
 	mPadMonitor->drawMarker(photonPosition,4,2);
 	mPadMonitor->update();
#endif

    } // loop over the hits

    mHistogram->status();
    unsigned short flag;
    PR(mHistogram->cerenkovAngle(&flag)/degree);
//     cout << "Next Event? <ret>: " << endl;
//     do {
// 	if(getchar()) break;
//     } while (true);
//     cout << "================== ID END ==================" << endl;
}

// ----------------------------------------------------
StThreeVectorF StRichSpectraMaker::calculateRadiationPoint(StTrack* track, StThreeVectorF& thePlane)
{

    cout << "StRichSpectraMaker::calculateRadiationPoint()" << endl;
    PR(thePlane);
    PR(mAverageRadiationPlanePoint);
    PR(thePlane);

    //
    // define the plane (in local coordinates) where the
    // track should extrapolate too.
    //
    
    StRichLocalCoordinate loc(thePlane.x(),
			      thePlane.y(),
			      thePlane.z());
     
    StGlobalCoordinate gNormalRadiationPoint;

    (*mTransform)(loc,gNormalRadiationPoint);
    StThreeVectorD globalNormalRadPoint(gNormalRadiationPoint.position().x(),
					gNormalRadiationPoint.position().y(),
					gNormalRadiationPoint.position().z());

    StPhysicalHelixD theHelix = track->geometry()->helix();
    double sP = theHelix.pathLength(globalNormalRadPoint,mGlobalRichNormal);
    StThreeVectorD globalRadPt = theHelix.at(sP);
//      PR(globalRadPt);
    StGlobalCoordinate g2(globalRadPt.x(),
			  globalRadPt.y(),
			  globalRadPt.z());
    StRichLocalCoordinate l2;
     
    (*mTransform)(g2,l2);
//      PR(l2);

    StThreeVectorF localRadPoint(l2.position().x(), l2.position().y(), l2.position().z());

    return localRadPoint;
}

// ----------------------------------------------------
bool StRichSpectraMaker::assignMipResidual(StTrack* track) {
	    
    StRichTrack* richTrack = new StRichTrack(track,mMagField);
    
    richTrack->assignMIP(&(mEvent->richCollection()->getRichHits()));
    
    StThreeVectorF projectedMIP  = richTrack->getProjectedMIP();
    if(!richTrack->getAssociatedMIP()) {
	cout << "StRichSpectraMaker::Make()\n";
	cout << "\tNo Associated MIP\n";
	cout << "\tSkip this track" << endl;
	delete richTrack;
	richTrack=0;
	return false;
    }
    
    mAssociatedMip = richTrack->getAssociatedMIP()->local();
    PR(projectedMIP);
    PR(mAssociatedMip);
    mMipResidual = (projectedMIP - mAssociatedMip);

    delete richTrack;
    richTrack = 0;
    return true;
}

// ----------------------------------------------------
void StRichSpectraMaker::printCutParameters(ostream& os) const
{
    os << "==============================================" << endl;
    os << "StRichSpectraMaker::printCutParameters()" << endl;
    os << "----------------------------------------------" << endl;
    os << "Event Level:" << endl;
    os << "\tVertexWindow =  " << (mVertexWindow/centimeter)  << " cm"    << endl;
    os << "\nTrack Level:" << endl;
    os << "\tPtCut =         " << (mPtCut/GeV)                << " GeV/c" << endl;
    os << "\tEtaCut =        " << mEtaCut                                 << endl;
    os << "\tLastHitCut =    " << (mLastHitCut/centimeter)    << " cm"    << endl;
    os << "\tDcaCut =        " << (mDcaCut/centimeter)        << " cm"    << endl;
    os << "\tFitPointsCut =  " << mFitPointsCut                           << endl;
    os << "\tPathCut =       " << (mPathCut/centimeter)       << " cm"    << endl;
    os << "\tPadPlaneCut =   " << (mPadPlaneCut/centimeter)   << " cm"    << endl;
    os << "\tRadiatorCut =   " << (mRadiatorCut/centimeter)   << " cm"    << endl;
    os << "\tLower Mom =     " << (mMomentumThreshold/GeV)    << " GeV/c" << endl;
    os << "\tUpper Mom =     " << (mMomentumLimit/GeV)    << " GeV/c" << endl;
    os << "----------------------------------------------" << endl;
    os << "----------------------------------------------" << endl;


}

////////////////////////
////////////////////////
// Drawing routines
//
void StRichSpectraMaker::drawRichPixels(StRichCollection* collection) const
{
#ifdef RICH_WITH_PAD_MONITOR
    const StSPtrVecRichPixel&  pixels = collection->getRichPixels();
    if(!pixels.size()) {
	cout << "StRichSpectraMaker::drawRichPixels()";
	cout << "\tNo Pixels in the Collection\n";
	cout << "\tReturning" << endl;
	return;
    }

    for(size_t ii=0; ii<pixels.size(); ii++) {
	mPadMonitor->drawPad(StRichSinglePixel(pixels[ii]->pad(),
					       pixels[ii]->row(),
					       pixels[ii]->adc()));
    }
    mPadMonitor->update();
    
#endif
}

void StRichSpectraMaker::drawRichHits(StRichCollection* collection) const
{
#ifdef RICH_WITH_PAD_MONITOR
    const StSPtrVecRichHit&  hits = collection->getRichHits();
    if(!hits.size()) {
	cout << "StRichSpectraMaker::drawRichHits()";
	cout << "\tNo Hits in the Collection\n";
	cout << "\tReturning" << endl;
	return;
    }

    for(size_t ii=0; ii<hits.size(); ii++) {
	//mPadMonitor->drawHit(hits[ii]);
	mPadMonitor->drawMarker(hits[ii]->local(),5);
    }
    mPadMonitor->update();
    

#endif
}

void StRichSpectraMaker::drawTracks() const
{
#ifdef RICH_WITH_PAD_MONITOR

    mPadMonitor->update();
#endif
}

void StRichSpectraMaker::drawQuickRing(StThreeVectorF& topPoint, StThreeVectorF& bottomPoint)
{
#ifdef RICH_WITH_PAD_MONITOR   
    //
    // generate the points
    //
    PR(mPion->mass());
    vector<StThreeVectorF> iPi = mTracer->calculatePoints(topPoint, mPion->mass());
    vector<StThreeVectorF> oPi = mTracer->calculatePoints(bottomPoint,mPion->mass());
    vector<StThreeVectorF> iKa = mTracer->calculatePoints(topPoint, mKaon->mass());
    vector<StThreeVectorF> oKa = mTracer->calculatePoints(bottomPoint,mKaon->mass());
    vector<StThreeVectorF> iPr = mTracer->calculatePoints(topPoint, mProton->mass());
    vector<StThreeVectorF> oPr = mTracer->calculatePoints(bottomPoint,mProton->mass());
    
    mPadMonitor->drawQuickRing(iPi, oPi, iKa, oKa, iPr, oPr);
    mPadMonitor->update();
#endif
}
