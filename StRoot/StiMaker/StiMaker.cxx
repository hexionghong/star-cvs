//  StiMaker.cxx
// M.L. Miller
// 5/00

#include <iostream.h>
#include <math.h>
#include <string>

//Root (Temp)
#include "TCanvas.h"
#include "TPolyMarker3D.h"
#include "TNode.h"
#include "TTUBE.h"

// StRoot
#include "StChain.h"
#include "St_DataSet.h"
#include "St_DataSetIter.h"
#include "StMessMgr.h"

// SCL
#include "SystemOfUnits.h"
#include "PhysicalConstants.h"

// StEvent
#include "StEventTypes.h"

//StMcEventMaker
#include "StMcEventMaker/StMcEventMaker.h"

// Sti
#include "Sti/StiHitContainer.h"
#include "Sti/StiHit.h"
#include "Sti/StiDetector.h"
#include "Sti/StiPlacement.h"
#include "Sti/StiHitFiller.h"
#include "Sti/StiDetectorContainer.h"
#include "Sti/StiTrackContainer.h"
#include "Sti/StiGeometryTransform.h"
#include "Sti/StiTrackSeedFinder.h"
#include "Sti/StiEvaluableTrackSeedFinder.h"
#include "Sti/StiDetectorFinder.h"
//#include "Sti/TrackNodeTest.h"
#include "Sti/StiCompositeSeedFinder.h"
#include "Sti/StiKalmanTrackFinder.h"

//StiGui
#include "StiGui/StiDrawableHits.h"
#include "StiGui/StiRootDrawableHits.h"
#include "StiGui/StiRootDrawableLine.h"
//#include "StiGui/StiRootDrawableHitContainer.h"
#include "StiGui/StiDisplayManager.h"

// StiMaker
#include "StiEvaluator.h"
#include "StiMaker.h"

StiMaker* StiMaker::sinstance = 0;

ostream& operator<<(ostream&, const StiHit&);


ClassImp(StiMaker)
  
StiMaker::StiMaker(const Char_t *name) : StMaker(name),
					 //Containers
					 mhitstore(0), mdetector(0), mtrackstore(0),
					 //Factories
					 mhitfactory(0), mtrackfactory(0), mktracknodefactory(0),
					 mdetectorfactory(0), mdatanodefactory(0), mkalmantrackfactory(0),
					 //Display
					 mdisplay(0),
					 //Utilities
					 mhitfiller(0),
					 //SeedFinders
					 mEvaluableSeedFinder(0), mKalmanSeedFinder(0), mcompseedfinder(0),
					 //Tracker
					 mtracker(0),
					 //Members
					 mevent(0), mMcEventMaker(0), mAssociationMaker(0)
{
    cout <<"StiMaker::StiMaker()"<<endl;
    sinstance = this;
}

StiMaker* StiMaker::instance()
{
    return (sinstance) ? sinstance : new StiMaker();
}

void StiMaker::kill()
{
    if (sinstance) {
	delete sinstance;
	sinstance = 0;
    }
    return;
}

StiMaker::~StiMaker() 
{
    StiHitContainer::kill();
    mhitstore = 0;
    
    delete mhitfactory;
    mhitfactory = 0;
    
    delete mhitfiller;
    mhitfiller = 0;
    
    StiDisplayManager::kill();
    mdisplay = 0;

    delete mtrackfactory;
    mtrackfactory = 0;

    delete mEvaluableSeedFinder;
    mEvaluableSeedFinder = 0;
    
    StiDetectorContainer::kill();
    mdetector = 0;
    
    StiTrackContainer::kill();
    mtrackstore = 0;

    delete mdetectorfactory;
    mdetectorfactory = 0;

    delete mdatanodefactory;
    mdatanodefactory = 0;

    delete mktracknodefactory;
    mktracknodefactory = 0;

    delete mkalmantrackfactory;
    mkalmantrackfactory=0;

    delete mKalmanSeedFinder;
    mKalmanSeedFinder=0;

    delete mcompseedfinder;
    mcompseedfinder=0;

    delete mtracker;
    mtracker = 0;
    
    StiGeometryTransform::kill();

    StiDetectorFinder::kill();

    StiEvaluator::kill();
}

void StiMaker::Clear(const char*)
{
    //Clear HitContainer
    mhitstore->clear();

    //Reset DetectorContainer
    StiDetectorContainer::instance()->reset();
    
    //Reset HitFactory
    mhitfactory->reset();

    //Reset EvaluableTrackFactory
    mtrackfactory->reset();
    mktracknodefactory->reset();

    //Reset KalmanTrackFactory
    mkalmantrackfactory->reset();
    
    //Reset DisplayManager
    mdisplay->reset();

    //Clear the track store
    mtrackstore->clear();
    
    StMaker::Clear();
}

Int_t StiMaker::Finish()
{
    return StMaker::Finish();
}

Int_t StiMaker::Init()
{
    //The Display
    //cout <<"Display"<<endl;
    mdisplay = StiDisplayManager::instance(); //Must come before anything that you want to be drawn
    mdisplay->cd();
    mdisplay->draw();
    mdisplay->update();
    //cout <<"\tdone"<<endl;

    //The track store
    mtrackstore = StiTrackContainer::instance();

    //The hit container
    //cout <<"Make HitContainer"<<endl;
    mhitstore = StiHitContainer::instance();
    //cout <<"\tdone"<<endl;

    //cout <<"Make Factories"<<endl;
    //The Hit Factory
    mhitfactory = new StiHitFactory("HitFactory");
    mhitfactory->setIncrementalSize(50000); //Allocate in chunks of 50k hits
    mhitfactory->setMaxIncrementCount(10);  //So, we can have 10 allocations at 50k a pop -> 500k hits max.

    //The Evalualbe Track Factory
    mtrackfactory = new StiEvaluableTrackFactory("EvaluableTrackFactory");
    mtrackfactory->setIncrementalSize(1000);
    mtrackfactory->setMaxIncrementCount(10);

    //The Track node factory
    mktracknodefactory = new StiKalmanTrackNodeFactory("StiKalmanTrackNodeFactory");
    mktracknodefactory->setIncrementalSize(1000);
    mktracknodefactory->setMaxIncrementCount(100);
    StiKalmanTrack::setKalmanTrackNodeFactory( mktracknodefactory );
    
    //The Kalman Track Factory
    mkalmantrackfactory = new StiKalmanTrackFactory("KalmanTrackFactory");
    mkalmantrackfactory->setIncrementalSize(1000);
    mkalmantrackfactory->setMaxIncrementCount(10);

    //cout <<"\tdone"<<endl;
    
    //EvaluableTrack SeedFinder
    //cout <<"StiEvaluableSeedFinder"<<endl;
    mEvaluableSeedFinder = new StiEvaluableTrackSeedFinder(mAssociationMaker);
    mEvaluableSeedFinder->setFactory(mtrackfactory);
    //cout <<"\tdone"<<endl;

    //cout <<"StiDetectorFactory"<<endl;
    //The StiDetector factory
    mdetectorfactory = new detector_factory("DrawableDetectorFactory");
    mdetectorfactory->setIncrementalSize(1000);
    mdetectorfactory->setMaxIncrementCount(10);
    mdetectorfactory->reset();
    //cout <<"\tdone"<<endl;

    //The DetectorNodeFactory
    //cout <<"DetectorNodeFactory"<<endl;
    mdatanodefactory = new data_node_factory("DataNodeFactory");
    mdatanodefactory->setIncrementalSize(1000);
    mdatanodefactory->setMaxIncrementCount(10);
    mdatanodefactory->reset();
    //cout <<"\tdone"<<endl;
    
    
    //The Detector Tree
    //cout <<"DetectorContainer"<<endl;
    mdetector = StiDetectorContainer::instance();
    mdetector->buildDetectors(mdatanodefactory, mdetectorfactory);
    mdetector->reset();
    //mdetector->print();
    //cout <<"\tdone"<<endl;
      
    mdisplay->draw();
    mdisplay->update();

    //cout <<"HitFiller"<<endl;
    //The Hit Filler
    mhitfiller = new StiHitFiller();
    mhitfiller->addDetector(kTpcId);
    mhitfiller->addDetector(kSvtId);
    cout <<"Hits used from detectors:\t"<<*mhitfiller<<endl;
    //cout <<"\tdone"<<endl;

    //    TrackNodeTest *pTest = new TrackNodeTest();
    //    pTest->doTest();

    //cout <<"StiCompositeSeedFinder"<<endl;
    //StiCompositeSeedFinder
    mKalmanSeedFinder = new StiTrackSeedFinder(mhitstore);
    mKalmanSeedFinder->setFactory(mkalmantrackfactory);
    mcompseedfinder =new StiCompositeSeedFinder();
    mcompseedfinder->buildOuterSeedFinder(mKalmanSeedFinder);
    mcompseedfinder->buildInnerSeedFinder(mKalmanSeedFinder);
    //cout <<"\tdone"<<endl;

    //cout <<"StiKalmanTrackFinder"<<endl;
    //The Tracker
    mtracker = new StiKalmanTrackFinder();
    mtracker->setTrackNodeFactory(mktracknodefactory);
    mtracker->setTrackSeedFinder(mEvaluableSeedFinder);
    //mtracker->setTrackSeedFinder(mcompseedfinder);
    mtracker->isValid(true);
    //cout <<"\tdone"<<endl;
    
    return StMaker::Init();
}

Int_t StiMaker::Make()
{
    StEvent* rEvent = 0;
    rEvent = (StEvent*) GetInputDS("StEvent");
    if (!mMcEventMaker) {
	cout <<"StiMaker::Make(). ERROR!\tmMcEventMaker==0"<<endl;
	return 0;
    }
    StMcEvent* mc = mMcEventMaker->currentMcEvent();
    if (!mc) {
	cout <<"StiMaker::Make(). ERROR!\tMcEvent==0"<<endl;
	return 0;
    }
    
    if (rEvent) {
	mevent = rEvent;
	
	cout <<"\n---------- StiMaker::Make() ------------\n"<<endl;
	cout <<"Number of Primary Vertices:\t"<<mevent->numberOfPrimaryVertices()<<endl;

	//Fill hits, organize the container
	mhitfiller->setEvent(mevent);
	mhitfiller->fillHits(mhitstore, mhitfactory);

	cout <<"StiMaker::Make()\tsortHits"<<endl;
	mhitstore->sortHits();
	cout <<"\tdone"<<endl;

	cout <<"StiMaker::Make()\tCall StiHitContainer::update()"<<endl;
	mhitstore->update();
	cout <<"\tdone"<<endl;
	    
	//Init seed finder for start
	mcompseedfinder->reset();

	//Initialize the SeedFinder, loop on tracks
	mEvaluableSeedFinder->setEvent(mc);

	//Test track finder
	
    }

    StiEvaluator::instance()->evaluateForEvent(mtrackstore);
    
    mdisplay->draw();
    mdisplay->update();
    return kStOK;
}

void StiMaker::printStatistics() const
{
    cout <<"HitFactory Size:\t"<<mhitfactory->getCurrentSize()<<endl;
    cout <<"HitContainer size:\t"<<mhitstore->size()<<endl;
    cout <<"Number of Primary Vertices:\t"<<mhitstore->numberOfVertices()<<endl;
}

void StiMaker::doNextAction()
{
    //Add call to next tracker action here
    mtracker->doNextAction();
    return;
}

