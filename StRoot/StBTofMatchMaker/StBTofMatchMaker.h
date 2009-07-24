//! Barrel TOF Match Maker
/*!  \class StBTofMatchMaker
 *   \brief Match Maker for the BTOF detector
 *   \author Xin Dong, Frank Geurts
 *   \date June 2009
 *
 * The Barrel TOF MatchMaker matches STAR tracks to the BTOF cells.
 * 
 * $Id: StBTofMatchMaker.h,v 1.3 2009/07/24 18:52:53 dongx Exp $
 */
/*****************************************************************
 *
 * $Log: StBTofMatchMaker.h,v $
 * Revision 1.3  2009/07/24 18:52:53  dongx
 * - Local Z window restricted in the projection
 * - ToT selection is used firstly when more than one hits associated with a track
 * - matchFlag updated
 *    0:   no matching
 *    1:   1-1 matching
 *    2:   1-2 matching, pick up the one with higher ToT value (<25ns)
 *    3:   1-2 matching, pick up the one with closest projection posision along y
 *
 * Revision 1.2  2009/06/23 21:15:09  geurts
 * first set of doxygen tags
 *
 * Revision 1.1  2009/06/23 13:15:03  geurts
 * *** empty log message ***
 *
 *
 *******************************************************************/
#ifndef STBTOFMATCHMAKER_HH     
#define STBTOFMATCHMAKER_HH
#include "StMaker.h"
#include "StThreeVectorD.hh"

#include <string>
#include <vector>
#ifndef ST_NO_NAMESPACES
using std::string;
using std::vector;
#endif

class StEvent;
class StTrack;
class StGlobalTrack;
class StHelix;
#include "StThreeVectorF.hh"
class StTrackGeometry;
class StDcaGeometry;
class StBTofGeometry;
class StBTofCollection;
class StBTofRawHitCollection;
class StBTofHitCollection;
class StSPtrVecBTofRawHit;
class StSPtrVecBTofHit;
class TH1D;
class TH2D;
class TTree;

#if !defined(ST_NO_TEMPLATE_DEF_ARGS) || defined(__CINT__)
typedef vector<Int_t>  IntVec;
typedef vector<UInt_t>  UIntVec;
typedef vector<Double_t>  DoubleVec;
#else
typedef vector<Int_t, allocator<Int_t>>  IntVec;
typedef vector<UInt_t, allocator<UInt_t>>  UIntVec;
typedef vector<Double_t, allocator<Double_t>>  DoubleVec;
#endif

class StBTofMatchMaker : public StMaker {
public:
    /// Default constructor
    StBTofMatchMaker(const Char_t *name="btofMatch");
    ~StBTofMatchMaker();
    
    // void Clear(Option_t *option="");
    /// process start-up options
    Int_t  Init();
    /// initialize  DaqMap, Geometry, and INL
    Int_t  InitRun(Int_t);
    Int_t  FinishRun(Int_t); 
    /// Main match algorithm
    Int_t  Make();
    /// Print run summary, and write QA histograms
    Int_t  Finish();
    
    /// enable QA histogram filling
    void setCreateHistoFlag(Bool_t histos=kTRUE); 
    /// enable track-tree filling
    void setCreateTreeFlag(Bool_t tree=kTRUE);
    /// selection of inner or outer geometry. By default - outerGeometry
    void setOuterTrackGeometry();
    void setStandardTrackGeometry();
    /// set minimum hits per track
    void setMinHitsPerTrack(Int_t);
    /// set minimum fit points per track
    void setMinFitPointsPerTrack(Int_t);
    /// set minimum fit-points/max-points ratio
    void setMinFitPointsOverMax(Float_t);
    /// set maximum distance of closest approach
    void setMaxDCA(Float_t);
    /// set histogram output file name
    void setHistoFileName(Char_t*);
    /// set ntuple output file name
    void setNtupleFileName(Char_t*);
    /// save geometry if it will be used by following makers in the chain
    void setSaveGeometry(Bool_t geomSave=kFALSE);

private:
    StTrackGeometry* trackGeometry(StTrack*);//!

    /// book histograms
    void bookHistograms();
    /// write histograms
    void writeHistogramsToFile();

    /// event selection    
    Bool_t validEvent(StEvent *);
    /// track selection
    Bool_t validTrack(StTrack*);

public:
    Bool_t  doPrintMemoryInfo;     //! 
    Bool_t  doPrintCpuInfo;        //!

private:
    static const Int_t mDAQOVERFLOW = 255;

    /// number of trays (12)
    static const Int_t mNTray = 120;
    /// number of cells per tray (192)
    static const Int_t mNTOF = 192;
    /// number of modules per tray (32)
    static const Int_t mNModule = 32;
    /// number of cells per module (6)
    static const Int_t mNCell = 6;
    /// number of tubes per upVPD (19)
    static const Int_t mNVPD = 19;

    /// fixed tray ID for upVPD-east
    static const Int_t mEastVpdTrayId = 122;
    /// fixed tray ID for upVPD-west
    static const Int_t mWestVpdTrayId = 121;

    ///
    Float_t     mWidthPad;   //! cell pad width

    StEvent *mEvent;
    StBTofGeometry *mBTofGeom;         //! pointer to the TOF geometry utility class
    
    Bool_t mHisto;    //! create, fill and write out histograms
    Bool_t mSaveTree; //! create, fill and write out trees for tpc tracks
    
    Bool_t mOuterTrackGeometry; //! use outer track geometry (true) for extrapolation
    Bool_t mGeometrySave;

    string mHistoFileName; //! name of histogram file, if empty no write-out
    
    /// event counters
    Int_t  mEventCounter;          //! #processed events
    Int_t  mAcceptedEventCounter;  //! #events w/ valid prim.vertex
    Int_t  mTofEventCounter;       //! #events w/ Tof raw data
    Int_t  mAcceptAndBeam;         //! #(beam events) w/ prim.vertex
    
    /// various cut-offs and ranges
    unsigned int mMinHitsPerTrack; //! lower cut on #hits per track
    unsigned int mMinFitPointsPerTrack; //! lower cut on #fitpoints per track
    Float_t mMinFitPointsOverMax; //! lower cut on #fitpoints / #maxpoints
    Float_t mMaxDCA; //! upper cut (centimeters) on final (global) DCA
    
    //

    /// TOF histograms for matching QA
    TH2D* mADCTDCCorelation;
    
    TH1D* mEventCounterHisto;
    TH1D* mCellsMultInEvent;
    TH1D* mHitsMultInEvent;
    TH1D* mHitsPrimaryInEvent;   // ! primary tracks
    TH1D* mHitsMultPerTrack;
    TH1D* mDaqOccupancy[mNTray];
    TH1D* mDaqOccupancyProj[mNTray];
    TH2D* mHitsPosition;
        
    TH2D* mHitCorr[mNTray];
    TH2D* mHitCorrModule[mNTray];

    TH2D* mDeltaHitFinal[mNTray];

    TH2D* mTrackPtEta;
    TH2D* mTrackPtPhi;
    TH1D* mTrackNFitPts;
    TH2D* mTrackdEdxvsp;
    TH2D* mNSigmaPivsPt;

    TTree* mTrackTree;
    
    TH1D* mCellsPerEventMatch1;
    TH1D* mHitsPerEventMatch1;
    TH1D* mCellsPerTrackMatch1;
    TH1D* mTracksPerCellMatch1;
    TH1D* mDaqOccupancyMatch1;
    TH2D* mDeltaHitMatch1;
            
    TH1D* mCellsPerEventMatch2;
    TH1D* mHitsPerEventMatch2;
    TH1D* mCellsPerTrackMatch2;
    TH1D* mTracksPerCellMatch2;
    TH1D* mDaqOccupancyMatch2;
    TH2D* mDeltaHitMatch2;
            
    TH1D* mCellsPerEventMatch3;
    TH1D* mHitsPerEventMatch3;
    TH1D* mCellsPerTrackMatch3;
    TH1D* mTracksPerCellMatch3;
    TH1D* mDaqOccupancyMatch3;
    TH2D* mDeltaHitMatch3;
    
    TH1D* mCellsPrimaryPerEventMatch3;
            
#ifndef ST_NO_TEMPLATE_DEF_ARGS
    typedef vector<Int_t> idVector;
#else
    typedef vector<Int_t,allocator<Int_t>> idVector;
#endif
    typedef idVector::iterator idVectorIter;   

    struct StructCellHit{
      Int_t tray;
      Int_t module;
      Int_t cell;
      StThreeVectorF hitPosition;
      idVector trackIdVec;
      Int_t matchFlag;
      Float_t zhit;
      Float_t yhit;
      Double_t tot;
      Int_t index2BTofHit;
    };
    
    struct TRACKTREE{
      Float_t pt;
      Float_t eta;
      Float_t phi;
      Int_t   nfitpts;
      Float_t dEdx;
      Int_t   ndEdxpts;
      Int_t   charge;
      Int_t   projTrayId;
      Int_t   projCellChan;
      Float_t projY;
      Float_t projZ;
    };
    TRACKTREE trackTree;
    
#ifndef ST_NO_TEMPLATE_DEF_ARGS
    typedef vector<StructCellHit> tofCellHitVector;
#else
    typedef vector<StructCellHit,allocator<StructCellHit>> tofCellHitVector;
#endif
    typedef vector<StructCellHit>::iterator tofCellHitVectorIter;
    
    
    virtual const char *GetCVS() const 
      {static const char cvs[]="Tag $Name:  $ $Id: StBTofMatchMaker.h,v 1.3 2009/07/24 18:52:53 dongx Exp $ built "__DATE__" "__TIME__ ; return cvs;}
    
    ClassDef(StBTofMatchMaker,1)
};
      

inline void StBTofMatchMaker::setOuterTrackGeometry(){mOuterTrackGeometry=true;}

inline void StBTofMatchMaker::setStandardTrackGeometry(){mOuterTrackGeometry=false;}

inline void StBTofMatchMaker::setMinHitsPerTrack(Int_t nhits){mMinHitsPerTrack=nhits;}

inline void StBTofMatchMaker::setMinFitPointsPerTrack(Int_t nfitpnts){mMinFitPointsPerTrack=nfitpnts;}

inline void StBTofMatchMaker::setMinFitPointsOverMax(Float_t ratio) {mMinFitPointsOverMax=ratio;}

inline void StBTofMatchMaker::setMaxDCA(Float_t maxdca){mMaxDCA=maxdca;}

inline void StBTofMatchMaker::setHistoFileName(Char_t* filename){mHistoFileName=filename;}

inline void StBTofMatchMaker::setCreateHistoFlag(Bool_t histos){mHisto = histos;}

inline void StBTofMatchMaker::setCreateTreeFlag(Bool_t tree){mSaveTree = tree;}

inline void StBTofMatchMaker::setSaveGeometry(Bool_t geomSave){mGeometrySave = geomSave; }

#endif
