//#if defined(WIN32)
// $Id: StConeJetFinder.cxx,v 1.29 2008/04/29 20:32:14 tai Exp $
#include "StConeJetFinder.h"

#include "TObject.h"

//std
#include <iostream>
#include <algorithm>
#include <time.h>
#include <map>
using std::multimap;
using std::for_each;
using std::sort;

#include "StMessMgr.h"

//StJetFinder
#include "StJetEtCell.h"
#include "StJetSpliterMerger.h"
#include "StProtoJet.h"

using namespace StSpinJet;

StConeJetFinder::StConeJetFinder(const StConePars& pars)
  : mPars(pars)
  , mSearchCounter(0)
  , mMerger(new StJetSpliterMerger())
  , _cellGrid(mPars)
{
    mMerger->setSplitFraction(mPars.splitFraction());
}

StConeJetFinder::~StConeJetFinder()
{

}

void StConeJetFinder::Init()
{
  _cellGrid.buildGrid(makeCellFactory());
}

StJetEtCellFactory* StConeJetFinder::makeCellFactory()
{
  return new StJetEtCellFactory;
}

void StConeJetFinder::findJets(JetList& protoJetList)
{
  mPreJets.clear();
  mSearchCounter = 0;

  _cellGrid.fillGridWith(protoJetList);

  if (mPars.debug()) {print();}

  CellList etSortedCellList = _cellGrid.EtSortedCellList();

  for (CellList::iterator etCell = etSortedCellList.begin(); etCell != etSortedCellList.end(); ++etCell) {

    if ((*etCell)->eT() <= mPars.seedEtMin()) break; //we're all done

    if (!acceptSeed((*etCell))) continue;

    //use a work object: mWorkCell
    initializeWorkCell((*etCell));

    findJets_sub1();
  }
    
  findJets_sub2();

  protoJetList.clear(); //clear 'em, add them back in as we find them
  
  for (ValueCellList::iterator realJetIt=mPreJets.begin(); realJetIt!=mPreJets.end(); ++realJetIt) {
    StJetEtCell* rj = &(*realJetIt);
    if ( rj->cellList().size()>0 ) { //at least one non-empty cell in cone
      protoJetList.push_back( collectCell(rj) );
    }
  }
}

void StConeJetFinder::findJets_sub1()
{
  if (mPars.performMinimization()) {
    doMinimization();
  } else {
    doSearch();
    addToPrejets(&mWorkCell);
  }
}

void StConeJetFinder::findJets_sub2()
{
  if (mPars.addMidpoints()) { 	//add seeds at midpoints
    addSeedsAtMidpoint(); //old style, add midpoints before split/merge
  }
    
  if (mPars.doSplitMerge()) {//split-merge
    mMerger->splitMerge(mPreJets);
  }
}

StConeJetFinder::SearchResult StConeJetFinder::doSearch()
{
    
  ++mSearchCounter;

  if (mPars.performMinimization() && mSearchCounter > 100) {
    return kTooManyTries;
  }
    
  //now search for clusters!  We walk in a square in eta-phi space, testing 
  //each cluster for it's distance from the centerCell
  //define the search window as phi +- deltaPhi, same for eta
  //The walk in eta is easy, strictly linear.
  //The walk in phi is just a little trickier, since phi is periodic

  //find this entry in map (this is a double check, not really necessary)
  StEtGridKey centerKey = findKey(mWorkCell.eta(), mWorkCell.phi());
	
  //begin walk in eta:

  int iEtaMin = centerKey.eta() - mPars.deltaEta();
  if (iEtaMin < 0) iEtaMin = 0 ; //avoid wasted searches in eta
	
  for (int iEta = iEtaMin; (iEta <= centerKey.eta() + mPars.deltaEta()) && (iEta < mPars.Neta()); ++iEta) {
		
    //begin walk in phi:
    for (int iPhi = centerKey.phi() - mPars.deltaPhi(); iPhi <= centerKey.phi() + mPars.deltaPhi(); ++iPhi) {
			
      int iModPhi = iPhi;
      if (iModPhi < 0) iModPhi = iModPhi + mPars.Nphi();
      if (iModPhi >= mPars.Nphi()) iModPhi = iModPhi - mPars.Nphi();
			
      StJetEtCell* otherCell = _cellGrid.CellI(iEta, iModPhi);
      if (!otherCell) {
	//	cout <<"otherCell doesn't exist.  key:\t"<<key<<endl;
      }
			
      if (acceptPair(&mWorkCell, otherCell) ) {//add it!
	//cout <<"Adding cell w/ key:\t"<<key<<endl;
	mWorkCell.add(otherCell);
      }
    }
  }
	
  //finished with walk, compare centroid to cone center
  const StProtoJet& centroid = mWorkCell.centroid();

  if (!inVolume(centroid.eta(), centroid.phi())) return kLeftVolume;

  StEtGridKey centroidKey = findKey(centroid.eta(), centroid.phi());
	
  if (centroidKey == centerKey) return kConverged;

  return kContinueSearch;
}

void StConeJetFinder::addToPrejets(StJetEtCell* cellp)
{
  //    PreJetInitializer initializer(*this);
  //    initializer(*cell); //test, try to initialize as we add
  //first find the pointer to the cell in the grid (not this local copy)

  StJetEtCell& cell = (*cellp);

  // from void PreJetInitializer::operator()(StJetEtCell& cell) 
  StJetEtCell* realCell = _cellGrid.CellD(cell.eta(), cell.phi());
  if (!realCell) {
    cout << "PreJetInitializer(). ERROR:\t"
	 << "real Cell doesn't exist." << endl;
    abort();
  }
	
  //now add this into the cone-cell collection, *if* it's not already there
  //this shouldn't happen, temp check to get rid of relic bug
  CellList& cells = cell.cellList();
  CellList::iterator where = std::find(cells.begin(), cells.end(), realCell);
  if (realCell->empty()==false) {
    if (where==cells.end()) {
      cout <<"\tADDING SELF IN CONE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
      cell.add(realCell);
    }
  }
	
  cell.setEt(0);
  for(CellList::iterator etCell = cell.cellList().begin(); etCell != cell.cellList().end(); ++etCell) {
    (*etCell)->update();
    //    cell.mEt += (*etCell)->eT();
    cell.setEt(cell.Et() + (*etCell)->eT());
  }
  /////////////////////////////////////////////////////////////////////////////


    mPreJets.push_back(cell);
}

void StConeJetFinder::initializeWorkCell(const StJetEtCell* other)
{
    mWorkCell.clear();
    mWorkCell = *other;
    mWorkCell.setEt(0.);
    if (mWorkCell.cellList().empty()==false) {
	cout <<"StConeJetFinder::initializeWorkCell(). ERROR:\t"
	     <<"workCell is not empty. abort()"<<endl;
	abort();
    }
}

void StConeJetFinder::doMinimization()
{
    mSearchCounter = 0;
	
    SearchResult res = kContinueSearch;
    while(res == kContinueSearch) {
		
      res = doSearch();
	
      if (res == kConverged) {
	addToPrejets(&mWorkCell);
	break;
      }			

      //find cell corresponding to centroid of cone
      StJetEtCell* newCenterCell = _cellGrid.CellD(mWorkCell.centroid().eta(), mWorkCell.centroid().phi());
      if (!newCenterCell) {
	cout << "newCenterCell doesn't exist.  key:\t" << endl;
	res = kLeftVolume;
      } else {
	initializeWorkCell(newCenterCell);
      }
    }

}

double midpoint(double v1, double v2)
{
    double high, low;
    if (v1>v2) {high=v1; low=v2;}
    else {high=v2; low=v1;}
    return (high-low)/2. + low;
}

//void StConeJetFinder::defineMidpoint(const StJetEtCell& pj1, const StJetEtCell& pj2, StJetEtCell& workCell) 
StJetEtCell* StConeJetFinder::defineMidpoint(const StJetEtCell& pj1, const StJetEtCell& pj2) 
{
    double etaMid = midpoint(pj1.eta(), pj2.eta() );
    double phiMid = midpoint(pj1.phi(), pj2.phi() );
    return _cellGrid.CellD(etaMid, phiMid);
}

//add a seed at the midpoint between any two jets separated by d<2.*coneRadius();
void StConeJetFinder::addSeedsAtMidpoint()
{
    //we have to first locate and remember pairs separated by d<2.*r.  
    //we then have to add midpoints.  You cannot add the midpoint when you locate the 
    //pair without invalidating the iterators that you are using to control your loop
    //That's why we need two loops.  Even an algorithm call to for_each will not advert this.
	
    //cout <<"--- StConeJetFinder::addSeedsAtMidpoint() ---"<<endl;
	
    mMidpointVec.clear();
	
    //cout <<"------------------ conents of PreJets: ---------------"<<endl;
    for (ValueCellList::iterator pj1=mPreJets.begin(); pj1!=mPreJets.end(); ++pj1) {
	//cout <<*pj1<<endl;
	for (ValueCellList::iterator pj2=pj1; pj2!=mPreJets.end(); ++pj2) {
	    //don't double count.  ignore same iterators and jets w/ same center location
	    if (pj1!=pj2 && (*pj1==*pj2)==false ) { 
		if ( (*pj1).distance(*pj2) <= 2.* mPars.coneRadius()) { 
		    //remember this combination
		    VCLItPairVec::value_type temp(pj1, pj2);
		    mMidpointVec.push_back(temp);
		}				
	    }
	}
    }
	
    //cout <<"StConeJetFinder::addSeedsAtMidpoint().  Adding:\t"<<mMidpointVec.size()<<"\tseeds"<<endl;
	
    for (VCLItPairVec::iterator it=mMidpointVec.begin(); it!=mMidpointVec.end(); ++it) {
	//clear the work cell
	mWorkCell.clear();
		
	//see if the midpoint is within the volume.  Should always be true, but double check
	StJetEtCell* mp = defineMidpoint(*(*it).first, *(*it).second );
	if (0) {
	    cout <<"--- new midpoint:"<<endl;
	    cout <<"midpoint j1:\t"<<*(*it).first<<endl;
	    cout <<"midpoint j2:\t"<<*(*it).second<<endl;
	}
	if (mp) {

	    //initialize the work cell to the midpoint
	    initializeWorkCell(mp);
	    
	    //add in the cone
	    mSearchCounter=0;
	    SearchResult res = doSearch();
	    if (mPars.requiredStableMidpoints()) {
		if (res == kConverged) {
		    addToPrejets(&mWorkCell);
		}	
	    }	
	    else {
		addToPrejets(&mWorkCell);
	    }
	}
	else {
	    LOG_ERROR <<"StConeJetFinder::addSeedsAtMidpoint(). ERROR:\t"
		 <<"midpoint is null.  This should never happen"<<endm;
	    abort();
	}
    }
}

bool StConeJetFinder::inVolume(double eta, double phi)
{
    int iEta = findEtaKey(eta);
    int iPhi = findPhiKey(phi);
    if (iEta < 0 || iPhi < 0) {
	return false;
    }
    return (_cellGrid.CellI(iEta, iPhi)) ? true : false;
}

StEtGridKey StConeJetFinder::findKey(double eta, double phi) const
{
  int iEta = findEtaKey(eta);
  int iPhi = findPhiKey(phi);
  if (iEta < 0 || iPhi < 0) {
    cout <<"StConeJetFinder::findKey(double, double). ERROR:\t"
	 <<"eta:\t"<<eta<<"\tphi:\t"<<phi<<"\t"
	 <<"iEta<0|| iPhi<0\tabort()"<<endl;
    //abort();
  }
  return StEtGridKey(iEta, iPhi);
}

int StConeJetFinder::findEtaKey(double eta) const
{
  return int((mPars.Neta()/(mPars.EtaMax() - mPars.EtaMin()))*(eta - mPars.EtaMin()));
}

int StConeJetFinder::findPhiKey(double phi) const
{
  while(phi > M_PI) phi -= 2*M_PI;
  while(phi < -M_PI) phi += 2*M_PI;
  return int( mPars.Nphi()*((phi - mPars.PhiMin())/(mPars.PhiMax() - mPars.PhiMin())));
}

bool StConeJetFinder::acceptSeed(const StJetEtCell* cell)
{
    return (cell->empty()==false);
}

bool StConeJetFinder::acceptPair(const StJetEtCell* centerCell, const StJetEtCell* otherCell) const
{
  if (!centerCell) return false;
  if (!otherCell) return false;
  if (otherCell->empty()) return false;
  if (otherCell->eT() <= mPars.assocEtMin()) return false; 
  if (centerCell->distance(*otherCell) >= mPars.coneRadius()) return false; 
  return true;
}


const StProtoJet& StConeJetFinder::collectCell(StJetEtCell* seed)
{
    //cout <<"StConeJetFinder::collectCell()"<<endl;
    if (seed->cellList().empty()==true) {
	StProtoJet& center = seed->protoJet();
	center.update();
	cout <<"\treturn w/o action.  empty cell"<<endl;
	return center;
    }
    
    else {
	
	//arbitrarily choose protojet from first cell in list
	CellList& cells = seed->cellList();
	StJetEtCell* centerCell = cells.front();
	StProtoJet& center = centerCell->protoJet();
	
	//cout <<"\tcombine protojets from other cells"<<endl;
	//int icell=0;
		
	//now combine protojets from other cells
	for (CellList::iterator it=cells.begin(); it!=cells.end(); ++it) {
	    if (it!=cells.begin()) { //don't double count first!
		StJetEtCell* cell = (*it);
		if (!cell) {
		    cout <<"\tStConeJetFinder::collectCell(). ERROR:\t"
			 <<"null cell.  skip"<<endl;
		}
		if (centerCell==*it) {
		    cout <<"\tStConeJetFinder::collectCell(). ERROR:\t"
			 <<"attempt to add self! skip"<<endl;
		}
		else {
		    //cout <<"\t\tadding cell:\t"<<icell++<<endl;
		    if (cell->empty()==false) {
			center.add( cell->protoJet() );
		    }
		}
	    }
	}
	//cout <<"\tupdate protojet"<<endl;
	center.update();
	//double pet = center.eT();
	//cout <<"\tcet:\t"<<cet<<"\tpet:\t"<<pet<<endl;
	//cout <<"\tfinished"<<endl;
	return center;
    }
}

void StConeJetFinder::print()
{
    cout <<"\nStConeJetFinder::print()"<<endl;
    cout <<"mNeta:\t"<<mPars.Neta()<<endl;
    cout <<"mNphi:\t"<<mPars.Nphi()<<endl;
    cout <<"mEtaMin:\t"<<mPars.EtaMin()<<endl;
    cout <<"mEtaMax:\t"<<mPars.EtaMax()<<endl;    
    cout <<"mPhiMin:\t"<<mPars.PhiMin()<<endl;
    cout <<"mPhiMax:\t"<<mPars.PhiMax()<<endl;
    cout <<"mR:\t"<<mPars.coneRadius()<<endl;
    cout <<"mAssocEtMin:\t"<<mPars.assocEtMin()<<endl;
    cout <<"mSeedEtMin:\t"<<mPars.seedEtMin()<<endl;
    cout <<"mphiWidth:\t"<<mPars.phiWidth()<<endl;
    cout <<"metaWidth:\t"<<mPars.etaWidth()<<endl;
    cout <<"mdeltaPhi:\t"<<mPars.deltaPhi()<<endl;
    cout <<"mdeltaEta:\t"<<mPars.deltaEta()<<endl;
    cout <<"mDoMinimization:\t"<<mPars.performMinimization()<<endl;
    cout <<"mAddMidpoints:\t"<<mPars.addMidpoints()<<endl;
    cout <<"mDoSplitMerge:\t"<<mPars.doSplitMerge()<<endl;
    cout <<"mSplitFraction:\t"<<mPars.splitFraction()<<endl;
    cout <<"splitFraction():\t"<<mMerger->splitFraction()<<endl;
    cout <<"mRequireStableMidpoints:\t"<<mPars.requiredStableMidpoints()<<endl;
    cout <<"mDebug:\t"<<mPars.debug()<<endl;
}


//non members ---

void PreJetLazyUpdater ::operator()(StJetEtCell& cell)
{
    sumEt += cell.eT();
}

void PreJetLazyUpdater ::operator()(StJetEtCell* cell)
{
    (*this)(*cell);
}

void PostMergeUpdater::operator()(StJetEtCell& cell)
{
    //now update each cell:
    PreJetLazyUpdater updater;
    updater = for_each( cell.cellList().begin(), cell.cellList().end(), updater );
	
    //now update jet-eT
    cell.mEt = updater.sumEt;
}

