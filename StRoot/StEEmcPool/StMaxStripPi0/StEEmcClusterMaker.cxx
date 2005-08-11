#include "StEEmcClusterMaker.h"

#include <algorithm>
#include <iostream>
//#include "eeSinglePeak.h" 
#include "TMath.h"


/* StEvent stuff */
#include "StEvent/StEvent.h"
#include "StEvent/StEmcCollection.h"
#include "StEvent/StEmcDetector.h"
#include "StEvent/StEmcModule.h"
#include "StEvent/StEmcClusterCollection.h"
#include "StEvent/StEmcCluster.h"


#define FIXED_SMD_SEED
//#define DYNAMIC_SMD_SEED
//#define SKIP_STAT
//#define LOOSE_CUTS

#define DEBUG 0

ClassImp(StEEmcClusterMaker);

// ----------------------------------------------------------------------------
StEEmcClusterMaker::StEEmcClusterMaker(const Char_t *name):StMaker(name)
{

  /// by default, do not populate the StEmcClusterCollection
  mFillStEvent = 0;

  /// default, do not supress seeds adjacent to clusters 
  mSuppress = false; 

  /// each cluster (tower, pre/post, smd) are assigned a unique id in
  /// the order in which they are collected
  mClusterId = 0;

  /// Set default seed energies
  const Float_t eseeds[] = { 0.6, 1.0/1000, 1.0/1000, 1.0/1000., 0.3/1000., 0.3/1000. };
  for ( Int_t i = 0; i < 6; i++ ) seedEnergy( eseeds[i], i );

  mMaxExtent = 6; 
  mSeedFloor = 2.0; 

  ///
  /// Initialize storage banks for tower and SMD clusters.
  /// Both will be stored sector-wise and layer-wise.
  ///
  /// Tower clusters are stored according to the sector
  /// in which we find their seed tower.  
  ///
  /// Layers: T=0, P=1, Q=2, R=3   /  Planes: U=0, V=1
  ///
  /// mTowerClusters[sector][layer] returns vector of tower clusters
  /// mSmdClusters[sector][plane]   returns vector of SMD clusters
  ///

  StEEmcClusterVec_t t;
  std::vector< StEEmcClusterVec_t > layers;
  for ( Int_t i = 0; i < 4; i++ ) layers.push_back(t);
  for ( Int_t i = 0; i < 12; i++ ) mTowerClusters.push_back(layers);

  StEEmcSmdClusterVec_t s;
  std::vector< StEEmcSmdClusterVec_t > planes;
  planes.push_back(s);
  planes.push_back(s);
  for ( Int_t i = 0; i < 12; i++ ) mSmdClusters.push_back(planes);

  StEEmcTowerVec_t tow;
  std::vector< StEEmcTowerVec_t > lay;
  for ( Int_t i = 0; i < 4; i++ ) lay.push_back(tow);
  for ( Int_t i = 0; i < 12; i++ ) mSeedTowers.push_back(lay);

  mEEtow=new EEmcGeomSimple();
  mEEsmd=EEmcSmdGeom::instance();
  mEEmap=EEmcSmdMap::instance();
  
}

// ----------------------------------------------------------------------------
Int_t StEEmcClusterMaker::Init()
{
  mEEanalysis=(StEEmcA2EMaker*)GetMaker(mAnalysisName);
  assert(mEEanalysis);

  return StMaker::Init();
}

// ----------------------------------------------------------------------------
Int_t StEEmcClusterMaker::Make()
{

  /// Warn if we couldn't build tower clusters
  if ( !buildTowerClusters() ) return kStWarn;

  /// Warn if we couldn't build SMD clusters
  if ( !buildSmdClusters() ) return kStWarn; 

  /// If the option to fill StEvent is selected, do it
  if ( mFillStEvent ) fillStEvent();

  /// Verify that StEvent is a valid copy
  if ( mFillStEvent ) 
    if ( !verifyStEvent() ) Warning("Make","StEvent not properly copied");

  return kStOK;
}

// ----------------------------------------------------------------------------
void StEEmcClusterMaker::Clear( Option_t *opts )
{

  /// Clear cluster storage
  for ( Int_t sector=0; sector<12; sector++ ) {
    for ( Int_t layer=0; layer<4; layer++ )
      mTowerClusters[sector][layer].clear();    
    for ( Int_t plane=0; plane<2; plane++ )
      mSmdClusters[sector][plane].clear();
  }

  for ( Int_t i=0;i<6;i++ ) mNumberOfClusters[i]=0;

  mClusterId = 0;

  return;
}


// ----------------------------------------------------------------------------
Bool_t StEEmcClusterMaker::buildTowerClusters()
{

  /// Current algo only populates tower clusters, using Jan's 
  /// "minesweeper" algorithm (apologies to Bill Gates).
  //static const Int_t layer=0;


  /// Loop over layers to go here...  maybe above init of weights
  for ( Int_t layer=0;layer<4;layer++)
  {

    /// Weights for dividing energy of towers among 
    /// the tower-only clusters, like Jan's algorithm.
    Float_t weights[720]; for ( Int_t i=0;i<720;i++ ) weights[i]=0.;

    /// Temp storage for clusters
    StEEmcClusterVec_t myClusters;


    /// Get list of towers
    StEEmcTowerVec_t towers = mEEanalysis -> towers( layer );



    /// Order according to energy (STL sort is fast, N log(N))
    std::sort(towers.begin(),towers.end());
    /// Order descending in energy
    std::reverse(towers.begin(),towers.end());
   

    /// Find the last tower which exceeds see threshold (actually,
    /// "last" will be the first tower below the seed threshold).
    StEEmcTowerVec_t::iterator last = towers.begin();
    while ( last != towers.end() ) {

      if ( (*last).energy() < mSeedEnergy[layer] ) break;
      /// store seed towers by layer, sector <<< this breaks, 
      /// gives an "abort", which I assume means that we're 
      /// trying to push a tower into a nonexistant vector...
      /// did I maybe get the order wrong???
      //$$$      mSeedTowers[layer][ (*last).sector() ].push_back((*last));
#if DEBUG
      std::cout << "-- Seed tower ----------------------" << std::endl;
      (*last).print();
#endif
      last++;
    }
    
    StEEmcTowerVec_t::iterator iter = towers.begin();
    while ( iter != towers.end() ) {

      /// We have run out of seeds here
      if ( iter==last )	break;
      

      /// This seed tower is adjacent to another
      /// seed tower and will be clustered with
      /// that one.  No incrementation for now.
      /// (Future, we may allow this to be a seed
      /// if there's an SMD response beneath it,
      /// then use tower-sharing shape to divide...)
      if ( weights[ (*iter).index() ] > 0. ) {
	iter++;
	continue;
      }

      /// loop over neighboring towers and increment weight
      /// for this tower with the energy of the seed tower
      for ( Int_t in=0; in < (*iter).numberOfNeighbors(); in++ ) {
	StEEmcTower t=(*iter).neighbor(in);
	weights[ t.index() ] += (*iter).energy();
      }

     
      iter++;

    }// loop over towers to init weights


    /// Loop over towers again
    iter=towers.begin();
    while ( iter != towers.end() ) {

      /// We've run out of seeds
      if ( iter==last ) break;

      /// If the weight for this tower has been incremented,
      /// then it's adjacent to a more energetic tower and
      /// already is clustered with it.
      if ( weights[ (*iter).index() ] > 0. ) {
	iter++;
	continue;
      }

      StEEmcCluster cluster;
      StEEmcTower seed=(*iter);
#if DEBUG
      std::cout << "--- Clustering ----------------" << std::endl;
      seed.print();
#endif

      TVector3 momentum;
      cluster.add(seed,1.0);
      UInt_t sec,sub,eta;
      sec=(UInt_t)seed.sector(); 
      sub=(UInt_t)seed.subsector(); 
      eta=(UInt_t)seed.etabin();
      TVector3 d=mEEtow->getTowerCenter(sec,sub,eta).Unit();
      momentum += ( seed.energy() * d );

      for ( Int_t in=0; in<seed.numberOfNeighbors(); in++ ) {
	StEEmcTower t=seed.neighbor(in);
	sec=(UInt_t)t.sector(); 
	sub=(UInt_t)t.subsector(); 
	eta=(UInt_t)t.etabin();
	d=mEEtow->getTowerCenter(sec,sub,eta).Unit();
	Float_t weight = seed.energy() / weights[ t.index() ];
	momentum += ( t.energy() * d * weight );
	cluster.add(t, weight);
#if DEBUG
	std::cout << "adding " << t.name() << " E=" << t.energy() << " W=" << weight << std::endl;
#endif
      }


      /// set momentum and a unique key
      cluster.momentum( momentum );
      cluster.key( mClusterId++ );
#if DEBUG
      cluster.print();
#endif

      mTowerClusters[ seed.sector() ][ layer ].push_back( cluster );
      mNumberOfClusters[layer]++;
     
      iter++;

    }
          
  }// end of eventual loop over layers
  

  return true;

}

// ----------------------------------------------------------------------------
#ifdef FIXED_SMD_SEED
Bool_t StEEmcClusterMaker::buildSmdClusters()
{

  Int_t max_extent = mMaxExtent;


  /// Loop over all sectors
  for ( Int_t sector=0; sector<12; sector++ ) 
    /// Loop over all planes
    for ( Int_t plane=0; plane<2; plane++ ) {

        /// Try to cut down on noice around identified gammas
        Float_t floor[288]; for ( Int_t i=0; i<288; i++ ) floor[i]=0.;

	/// Energy of all strips in plane
	Float_t energy[288]; for ( Int_t i=0; i<288; i++ ) energy[i]=0.;

  	/// Get list of strips
    	StEEmcStripVec_t strips=mEEanalysis->strips(sector,plane);
	StEEmcStripVec_t seeds;
      	/// Sort by energy
	std::sort(strips.begin(),strips.end());
  	/// Order in descending energy
    	std::reverse(strips.begin(),strips.end());


	/// Copy energy of all hit strips into an array
	StEEmcStripVec_t::iterator istrip=strips.begin();
	while ( istrip != strips.end() ) {
	  if ( (*istrip).stat()||(*istrip).fail() ) {
	    istrip++;
	    continue;
	  }

	  energy[ (*istrip).index() ] = (*istrip).energy();
	  istrip++;
	}
	
	/// We'll flag each seed strip
	std::vector<Bool_t> seed_flags( strips.size(), false );

	/// Iterate over strips and find seeds
	Int_t nstrip=0;
	Int_t nseeds=0;
	istrip=strips.begin();
	while ( istrip!=strips.end() ) {

	  Int_t index=(*istrip).index();
	  Float_t eseed=(*istrip).energy();

	  /// We always ignore the corners of the planes
	  if ( index <= 3 || index >= 283 ) {
	    istrip++;
	    nstrip++;
	    continue;
	  }

	  /// Verify that this is a good strip
#ifdef SKIP_STAT
	  if ( (*istrip).fail() || (*istrip).stat() ) {
	    istrip++;
	    nstrip++;
	    continue;
	  }
#endif

	  /// Verify that this strip exceeds the floor by the
	  /// specified seed threshold.  If not, punt
	  if ( eseed < mSeedFloor*floor[ index ] + mSeedEnergy[4+plane] ) {
	    istrip++;
	    nstrip++;
	    continue;
	  }

	  /// Flag this as a seed strip
	  seeds.push_back( (*istrip) );
	  nstrip++;
	  nseeds++;

#ifndef LOOSE_CUTS	  
	  /// Set a "floor" around this strip	  
	  for ( Int_t i=0; i < 288; i++ ) {

	    Int_t dx=TMath::Abs(index-i);

	    /// Within +/- 2 strips, find no other seeds
	    if ( dx<3 ) 
	      if ( eseed > floor[i] ) floor[i]=eseed;
	    
	    /// Within +/- 4 strips, Floor is 20% of seed
	    if ( dx<5 ) 
	      if ( 0.20 * eseed > floor[i] ) floor[i] = 0.2 * eseed;

	    /// Within +/- 10 strips, Floor i 10% of seed
	    if ( dx<11 )
	      if ( 0.10 * eseed > floor[i] ) floor[i] = 0.1 * eseed;

	    /// Within +/- 20 strips, floor is 5% of seed
	    if ( dx<21 )
	      if ( 0.20*eseed > floor[i] ) floor[i] = 0.05 * eseed;
	    	     	    
	  }
#else
	  /// Set a "floor" around this strip	  
	  for ( Int_t i=0; i < 288; i++ ) {

	    Int_t dx=TMath::Abs(index-i);

	    /// Within +/- 6 strips, Floor is  5% of seed
	    if ( dx<7 ) 
	      if ( 0.05 * eseed > floor[i] ) floor[i] = 0.05 * eseed;

	  }

#endif
	  istrip++;

	}// found all seed strips


	/// Now for each seed strip, add all contiguous, 
	/// adjacent SMD strips out to +/- 3 strips on 
	/// either side.  Continuity is broken if strip
	/// falls below Nsigma threshold and is not marked
	/// as dead.
	Bool_t owned[288]; for (Int_t i=0;i<288;i++) owned[i]=false;
	Bool_t xseed[288]; for (Int_t i=0;i<288;i++) xseed[i]=false;

	StEEmcStripVec_t::iterator iseed=seeds.begin();
	while ( iseed != seeds.end() ) {
	  
	  /// Verify that this seed is not owned by another cluster
	  Int_t index=(*iseed).index();
	  if ( owned[index] || (mSuppress&&xseed[index]) ) {
	    iseed++;
	    continue;
	  }

	  /// This seed is now the property of a cluster
	  owned[index]=true;
	  xseed[index]=true;
	  /// Create our cluster
	  StEEmcSmdCluster cluster;
	  /// And give it the strip
	  cluster.add( (*iseed) );

	  /// Now go +/- 3 strips on either side and add them
	  /// to the cluster, respecting continuity etc...
	  for ( Int_t i=index+1; i<=index+max_extent; i++ ) {
	    /// Get the strip
	    StEEmcStrip strip=mEEanalysis->strip(sector,plane,i);
	    /// Break if not continuous (and not dead)
#ifdef SKIP_STAT
	    if (strip.energy()<=0.&&!strip.fail()&&!strip.stat()) break; 
#else
	    if ( strip.energy()<=0. ) break;
#endif
	    /// Mark this strip as owned
	    owned[ strip.index() ] = true;
	    xseed[ strip.index() ] = true;
	    /// Add to cluster
	    cluster.add(strip);	    	    
	  }
	  for ( Int_t i=index-1; i>=index-max_extent; i-- ) {
	    /// Get the strip
	    StEEmcStrip strip=mEEanalysis->strip(sector,plane,i);
	    /// Break if not continuous (and not dead)
#ifdef SKIP_STAT
	    if (strip.energy()<=0.&&!strip.fail()&&!strip.stat()) break;
#else
	    if (strip.energy()<=0.) break;
#endif
	    /// Mark this strip as owned
	    owned[ strip.index() ] = true;
	    xseed[ strip.index() ] = true;
	    /// Add to cluster
	    cluster.add(strip);	    	    
	  }
	  
	  /// Push the cluster into our storage vector
	  if ( cluster.size() >= 3 ) {
	    cluster.key( mClusterId++ );
	    mSmdClusters[ sector ][ plane ].push_back(cluster);
	    mNumberOfClusters[4+plane]++;

            // disallow strips on either side of the cluster
	    // from forming seeds
	    Int_t ns=cluster.numberOfStrips();
	    Int_t left=999,right=-999;
       	    for ( Int_t is=0;is<ns;is++ )
	    {
		StEEmcStrip s=cluster.strip(is);
		if ( s.index()<left ) left=s.index();
		if ( s.index()>right ) right=s.index();
	    }

	    xseed[left-1]=true;
	    xseed[left-2]=true;
	    xseed[right+1]=true;
	    xseed[right+2]=true;
	    
	  }
	  

	  iseed++;
	}	             	

    }// loop over planes/sectors

  return true;

}
#endif
#ifdef DYNAMIC_SMD_SEED

Bool_t StEEmcClusterMaker::buildSmdClusters()
{

  Int_t max_extent = mMaxExtent;


  /// Loop over all sectors
  for ( Int_t sector=0; sector<12; sector++ ) 
    {

      /// Get list of tower clusters in this sector
      StEEmcClusterVec_t clist = clusters(sector,0);

      /// We will build a list of seed strips in each sector
      StEEmcStripVec_t seeds;

      /// Loop over the smd planes
      for ( UInt_t iuv=0; iuv<2; iuv++ )
	{
	  
	  /// Generate a list of seed strips
	  StEEmcStripVec_t seeds;
	  
	  /// Array denoting whether strip has been used or not
	  std::vector<Int_t> used(288,0);
	  
	  /// Array of seed "floor" values
	  std::vector<Float_t> floor(288,0.);


	  /// Loop over all tower clusters
	  for ( UInt_t ic=0; ic<clist.size(); ic++ )
	    {
	      
	      /// Get the tower cluster
	      StEEmcCluster cluster=clist[ic];
	      Int_t sec=cluster.tower(0).sector();
	      Int_t sub=cluster.tower(0).subsector();
	      Int_t eta=cluster.tower(0).etabin();
	      
	      /// Estimiate energy deposit in smd for
	      /// a single gamma/e+/- with cluster energy
	      Float_t esmd = cluster.energy() * 0.0056;
	      
	      /// Establish an smd seed threshold based on 
	      /// this energy
	      Float_t ethresh = esmd * 0.1; /// old smd seed fraction
	      
	      /// Min and max strip ranges to search beneath 
	      /// the tower clusters, extend 10 strips beyond the
	      /// range of the seed tower
	      Int_t smin[2], smax[2];
	      mEEmap->getRangeU(sec,sub,eta, smin[0], smax[0] );
	      mEEmap->getRangeV(sec,sub,eta, smin[1], smax[1] );
	      smin[0]=TMath::Max(smin[0]-20,4);
	      smax[0]=TMath::Min(smax[0]+20,284);
	      smin[1]=TMath::Max(smin[1]-20,4);
	      smax[1]=TMath::Min(smax[1]+20,284);
	  

	      /// Loop over all smd strips to find seed strips
	      /// and form smd clusters
	      for ( Int_t istrip=smin[iuv]; istrip<smax[iuv]; istrip++ )
		{

		  /// Get the SMD strip.  If it exceeds the seed threshold,
		  /// then we add it to the list of seeds, and raise the
		  /// floor within +/- 20 smd strips
		  StEEmcStrip strip=mEEanalysis->strip(sector,iuv,istrip);

		  /// energy threshold for this smd strip is the larger
		  /// of the dynamic seed threshold or the "floor"
		  Float_t mythresh=TMath::Max(ethresh, floor[istrip]);

		  /// strip exceeds threshold and isn't in use by another
		  /// smd cluster
		  if ( strip.energy() > mythresh && !used[istrip] ) {

		    seeds.push_back( strip );
		    used[istrip]++;
		    floor[istrip] = strip.energy();
		    for ( Int_t i=0; i<20; i++ ) {
		      if ( istrip+i<288 ) floor[istrip+i]+=0.10*strip.energy();
		      if ( istrip-i>0   ) floor[istrip-i]+=0.10*strip.energy();
		    }

		    /// form a new smd cluster
		    StEEmcSmdCluster smdc;
		    smdc.key( mClusterId++ );
		    Bool_t goL=1, goR=1;
		    for ( Int_t i=0; i<max_extent; i++ ) {
		      goL&=(istrip-i>0);
		      goR&=(istrip+i<287);

		      if ( goL ) {
			StEEmcStrip s=mEEanalysis->strip(sector,iuv,istrip-i);
			if ( !s.stat() && !s.fail() ) {
			  goL&=s.energy()>0.;
			  if ( goL ) {
			    smdc.add(s);
			    used[istrip-i]=1;
			  }
			}
		      }

		      if ( goR ) {
			StEEmcStrip s=mEEanalysis->strip(sector,iuv,istrip+i);
			if ( !s.stat() && !s.fail() ) {
			  goR&=s.energy()>0.;
			  if ( goR ) {
			    smdc.add(s);
			    used[istrip+i]=1;
			  }
			}
		      }

		    }/// done forming cluster

		    if ( smdc.numberOfStrips() > 2 )
		      mSmdClusters[ sector ][ iuv ].push_back( smdc );

		  }
		    

		}


	    }

	}

    }

}

#endif

// ----------------------------------------------------------------------------

void StEEmcClusterMaker::fillStEvent()
{

  StEvent *stevent=(StEvent*)GetInputDS("StEvent");
  if ( !stevent ) {
    Warning("fillStEvent","called, but no StEvent to be found");
    return;
  }

  std::cout << "Adding tower clusters to StEvent at " << stevent << std::endl << std::flush;

  ///
  /// First the eemc tower clusters  
  ///
  StEmcDetector *detector=stevent->emcCollection()->detector(kEndcapEmcTowerId);
  if ( !detector )
    {
      Warning("fillStEvent","detector == NULL, MAJOR StEvent problem, continuing");
      return;
    }
  

  ///
  /// If we found clusters, create a cluster collection for
  /// this detector.  Otherwise, nullify the cluster collection,
  /// otherwise we run into an assert somewhere out in "STAR" 
  /// land... or maybe not,... no comments in barrel virtual 
  /// finder...
  ///
  
  if ( mNumberOfClusters[0] > 0 )
    {


      ///
      /// Obtain the cluster collection from the detector, and
      /// prepare to fill it.  If it doesn't exist, create it.
      ///
      StEmcClusterCollection *collect = detector -> cluster();
      if ( !collect ) 
	{
	  //Warning("fillStEvent","StEmcClusterCollection (towers) was NULL, so I'm creating one.");
	  collect = new StEmcClusterCollection();
	  detector->setCluster( collect );
	}

      assert(collect);
      collect->setDetector( kEndcapEmcTowerId );
      collect->setClusterFinderId( 123 );
      collect->setClusterFinderParamVersion( 123 );
  
      /// Loop over all EEMC sectors and fill collection
      for ( Int_t isector=0; isector<12; isector++ )
	{

	  /// Loop over all clusters in this sector
	  for ( UInt_t iclust=0; iclust<mTowerClusters[isector][0].size(); iclust++ ) 
	    {

	      StEEmcCluster cl=mTowerClusters[isector][0].at(iclust);
	      
	      /// for some reason, this code doesn't work when I
	      /// call StEEmcCluster::stemc(), but works just fine
	      /// here....
	      StEmcCluster *emccluster = new StEmcCluster();
	      emccluster->setEta( cl.momentum().Eta() );
	      emccluster->setPhi( cl.momentum().Phi() );
	      emccluster->setSigmaEta(-1.);
	      emccluster->setSigmaPhi(-1.);
	      emccluster->setEnergy( cl.energy() );
	      emccluster->SetUniqueID( cl.key() );
#if 1
	      for ( Int_t i=0; i< cl.numberOfTowers(); i++ ) 
		{
		  StEmcRawHit *hit=cl.tower(i).stemc();
		  assert( hit );         
		  emccluster->addHit( hit );
		}
#endif
	    
	      collect->addCluster( emccluster );

	      
	      /// association between StEmcCluster and my class
	      mEtoEE[ emccluster ] = cl;
	      cl.stemc( emccluster );

	    }
	  
	}

    }

  else // if ( mNumberOfClusters[0] == 0 )
    {

      detector->setCluster( NULL );

    }




  /// now set the collection in the detector
  //  detector->setCluster( collect );

  ///
  /// Next the pre and postshower clusters
  ///
  detector=stevent->emcCollection()->detector(kEndcapEmcPreShowerId);
  if ( !detector )
    {
      Warning("fillStEvent","detector == NULL for pre/post, no clusters for you");
    }
  else if ( mNumberOfClusters[1] > 0 ||
	    mNumberOfClusters[2] > 0 || 
	    mNumberOfClusters[3] > 0 )
    {

      StEmcClusterCollection *pqr = new StEmcClusterCollection();
      if ( !pqr )
	{
	  //Warning("fillStEvent","StEmcClusterCollection (pre/post) was NULL, so I'm creating one.");
	  pqr = new StEmcClusterCollection();
	  detector->setCluster( pqr );
	}
      assert(pqr); 
      pqr -> setDetector( kEndcapEmcPreShowerId );
      pqr -> setClusterFinderId( 123 );
      pqr -> setClusterFinderParamVersion( 321 );
  
      /// Loop over all EEMC sectors and fill collection
      for ( Int_t isector=0; isector<12; isector++ )
    
      /// Loop over PQR
      for ( Int_t ilayer=1; ilayer<4; ilayer++ ) 
    
      /// Loop over all clusters in this sector
      for ( UInt_t iclust=0; iclust<mTowerClusters[isector][ilayer].size(); iclust++ ) 
	{

	  StEEmcCluster cl=mTowerClusters[isector][ilayer].at(iclust);

	  /// for some reason, this code doesn't work when I
	  /// call StEEmcCluster::stemc(), but works just fine
	  /// here....
	  StEmcCluster *emccluster = new StEmcCluster();
	  emccluster->setEta( cl.momentum().Eta() );
	  emccluster->setPhi( cl.momentum().Phi() );
	  emccluster->setSigmaEta(-1.);
	  emccluster->setSigmaPhi(-1.);
	  emccluster->setEnergy( cl.energy() );
	  emccluster->SetUniqueID( cl.key() );
#if 1
	  for ( Int_t i=0; i< cl.numberOfTowers(); i++ ) 
	    {
	      StEmcRawHit *hit=cl.tower(i).stemc();
	      assert( hit );         
	      emccluster->addHit( hit );
	    }
#endif


	  pqr->addCluster( emccluster );

	  mEtoEE[ emccluster ] = cl;
	  cl.stemc( emccluster );

	}

    }
  else // if ( mNumberOfClusters[1, 2 OR 3] == 0 )
    {

      detector->setCluster( NULL );

    }

  
  ///
  /// Finally the U&V smd clusters
  ///
  StDetectorId ids[]={ kEndcapSmdUStripId, kEndcapSmdVStripId };


  for ( Int_t iplane=0; iplane<2; iplane++ ) 
    {

      detector=stevent->emcCollection()->detector(ids[iplane]);
      if ( !detector ) 
	{
	  Warning("fillStEvent","detector == NULL for smd plane, no clusters for you");
	}
      else if ( mNumberOfClusters[4+iplane] > 0 )
	{
	  
	  StEmcClusterCollection *smdc = detector -> cluster();
	  if ( !smdc ) 
	    {
	      //Warning("fillStEvent","StEmcClusterCollection (smd) was NULL, so I'm creating one.");
	      smdc = new StEmcClusterCollection();
	      detector->setCluster( smdc );
	    }
	  
	  smdc->setDetector( ids[iplane] );
	  smdc->setClusterFinderId( 123 );
	  smdc->setClusterFinderParamVersion( 321 );

	  
	  for ( Int_t isector=0; isector<12; isector++ ) 
	    {
	      
	      for ( UInt_t iclust=0; iclust<mSmdClusters[isector][iplane].size(); iclust++ ) 
		{
		  
		  StEEmcSmdCluster cl = mSmdClusters[isector][iplane].at(iclust);
		  

		  StEmcCluster *emccluster = new StEmcCluster();
		  emccluster->setEta( cl.mean() );
		  emccluster->setPhi( (Float_t)iplane );
		  emccluster->setSigmaEta(-1.);
		  emccluster->setSigmaPhi(-1.);
		  emccluster->setEnergy( cl.energy() );
		  emccluster->SetUniqueID( cl.key() );
		  for ( Int_t i=0; i< cl.numberOfStrips(); i++ ) 
		    {
		      StEmcRawHit *hit=cl.strip(i).stemc();
		      assert( hit );         
		      emccluster->addHit( hit );
		    }		  
		  smdc->addCluster( emccluster );

		  mEtoEEsmd[ emccluster ] = cl;
		  cl.stemc( emccluster );
		  
		}
	  
	      
	    }
	  
	}

      else
	{
	  detector -> setCluster( NULL );
	}



    }


}



// ----------------------------------------------------------------------------
Bool_t StEEmcClusterMaker::verifyStEvent()
  {

  /// verify tower clusters
  StEvent *stevent=(StEvent*)GetInputDS("StEvent");
  if ( !stevent ) {
    Warning("verifyStEvent","No StEvent found.");
    return true;
  }
  //StEmcCollection *emccollection=stevent->emcCollection();

  Bool_t go = true;
 
  StEmcDetector *detector=stevent->emcCollection()->detector(kEndcapEmcTowerId);
  if ( !detector )
    {
      Warning("verifyStEvent","detector == NULL for towers");
      return false;
    }

  
  StEmcClusterCollection *cc=detector->cluster();
  if ( cc )
    {


      /// ------------------------------------------------------------------------
      ///
      /// Tower-cluster checksum
      ///
      
      Float_t emc_sum_towers = 0.;
      Float_t eemc_sum_towers = 0.;
  
      StSPtrVecEmcCluster &emcClusters=cc->clusters();
      for ( UInt_t i=0; i<emcClusters.size(); i++ ) 
	{
	  StEmcCluster *cl=emcClusters[i];
	  assert(cl);
	  emc_sum_towers += cl->energy();
	}
      
      for ( Int_t sec=0; sec<12; sec++ ) 
	{
	  
	  for ( Int_t i=0; i<numberOfClusters(sec,0); i++ )
	    eemc_sum_towers += cluster(sec,0,i).energy();
	}
      
      std::cout << "StEEmcClusterMaker tower checksum: ";
      if ( emc_sum_towers == eemc_sum_towers ) {
	std::cout << "passed";
      }
      else {
	std::cout << "FAILED"; go=false;
      }
      std::cout << std::endl;

    }
  else {

    std::cout << "StEEmcClusterMaker tower checksum: NULL collection, nclust=" << mNumberOfClusters[0] << std::endl;
    go &= (mNumberOfClusters[0]==0);

  }

  /// ------------------------------------------------------------------------
  ///
  /// SMD-cluster checksum
  ///
  Float_t emc_sum_smdu = 0.;
  Float_t eemc_sum_smdu = 0.;

  detector=stevent->emcCollection()->detector(kEndcapSmdUStripId);
  if ( !detector )
    {
      Warning("verifyStEvent","detector == NULL for smdu");
      return false;
    }

  cc=detector->cluster();
  if ( cc ) 
    {

      StSPtrVecEmcCluster &smduClusters=cc->clusters();

      for ( UInt_t i=0; i<smduClusters.size(); i++ ) 
	{
	  StEmcCluster *cl=smduClusters[i];
	  assert(cl);
	  emc_sum_smdu += cl->energy();
	}
      
  
      for ( Int_t sec=0; sec<12; sec++ ) 
	{

	  for ( Int_t i=0; i<numberOfSmdClusters(sec,0); i++ )
	
	    {
	      eemc_sum_smdu += smdcluster(sec,0,i).energy();

	    }

	}

      std::cout << "StEEmcClusterMaker smdu checksum: ";
      if ( emc_sum_smdu == eemc_sum_smdu ) {
	std::cout << "passed";
      }
      else {
	std::cout << "FAILED"; go=false;
      }
      std::cout << std::endl;

    }  
  else 
    {
      std::cout << "StEEmcClusterMaker smdu checksum: NULL collection, nclust=" << mNumberOfClusters[4] << std::endl;
      go &= (mNumberOfClusters[4]==0);
    }


  /// -----------------------

  Float_t emc_sum_smdv = 0.;
  Float_t eemc_sum_smdv = 0.;

  detector=stevent->emcCollection()->detector(kEndcapSmdVStripId);
  if (!detector)
    {
      Warning("verifyStEvent","detector == NULL for smdv");
      return false;
    }

  cc=detector->cluster();

  if ( cc )
    {

      StSPtrVecEmcCluster &smdvClusters=cc->clusters();
      
      for ( UInt_t i=0; i<smdvClusters.size(); i++ ) 
	{
	  StEmcCluster *cl=smdvClusters[i];
	  assert(cl);
	  emc_sum_smdv += cl->energy();
	}
      
      
      for ( Int_t sec=0; sec<12; sec++ ) 
	{
	  
	  for ( Int_t i=0; i<numberOfSmdClusters(sec,1); i++ )
	    
	    {
	      eemc_sum_smdv += smdcluster(sec,1,i).energy();
	      
	    }
	  
	}
      
      std::cout << "StEEmcClusterMaker smdv checksum: ";
      if ( emc_sum_smdv == eemc_sum_smdv ) {
	std::cout << "passed";
      }
      else {
	std::cout << "FAILED"; go=false;
      }
      std::cout << std::endl;

    }
  else 
    {
      std::cout << "StEEmcClusterMaker smdv checksum: NULL collection, nclust=" << mNumberOfClusters[5] << std::endl;
      go &= (mNumberOfClusters[5]==0);
    }
  

  
  return go;
}



// ----------------------------------------------------------------------------
void StEEmcClusterMaker::print()
{

  std::cout << "StEEmcClusterMaker::print()" << std::endl;
  const Char_t *names[] = { "tower", "pre1", "pre2", "post", "smdu", "smdv" };
  for ( Int_t i=0;i<6;i++ )
    {

      std::cout << "Number of " << names[i] 
		<< " clusters = " << mNumberOfClusters[i] 
		<< std::endl;

    }

  std::cout << "printout of tower clusters follows:" << std::endl;
  for ( Int_t sec=0;sec<12;sec++)
  for ( Int_t i=0;i<numberOfClusters(sec,0);i++ )
    {
      StEEmcCluster clust=cluster(sec,0,i);
      clust.print();
      //      std::cout << "cluster.key()=" << clust.key() << std::endl;
      //      std::cout << "cluster.eta()=" << clust.momentum().Eta() << std::endl;
      //      std::cout << "cluster.phi()=" << clust.momentum().Phi() << std::endl;      
      //      std::cout << "cluster.energy()=" << clust.energy() << std::endl;
    }

  

}
