/***************************************************************************
 *
 * $Id: StppLMVVertexFinder.h,v 1.1 2004/07/21 01:53:18 balewski Exp $
 *
 * Author: Thomas Ullrich, Feb 2002
 * Modified for pp by David Hardtke, Summer 2002
 ***************************************************************************/
#include <vector>
#include <StThreeVectorD.hh>
#include <StPhysicalHelixD.hh>
#include "StGenericVertexFinder.h"

class StEvent;
class StTrack;
class StMaker;
class StppLMVVertexFinder: public StGenericVertexFinder {
public:
    StppLMVVertexFinder();
    virtual ~StppLMVVertexFinder();

    bool            fit(StEvent*);       // fit the vertex
    StThreeVectorD  result() const;      // result of fit
    StThreeVectorD  error() const;       // error on fit result
    int             status() const;      // error and status flag

    void            setExternalSeed(const StThreeVectorD&);
    void            setPrintLevel(int = 0);
    void            printInfo(ostream& = cout) const;
    void            UseVertexConstraint(double x0, double y0, double dxdz, double dydz, double weight);
    void            NoVertexConstraint();
    void            CTBforSeed(){ mRequireCTB = true;}
    void            NoCTBforSeed(){ mRequireCTB = false;}
    int             NCtbMatches();
    void            SetFitPointsCut(int fitpoints);
    inline void DoUseITTF(){use_ITTF=kTRUE;};
    inline void DoNotUseITTF(){use_ITTF=kFALSE;};

private:
    bool                     requireCTB;

    double                   mWidthScale;
    double                   mX0  ; // starting point of beam parameterization
    double                   mY0  ; // starting point of beam parameterization
    double                   mdxdz; // beam slope
    double                   mdydz; // beam slope
    inline void setFlagBase(UInt_t base){mFlagBase=base;};
    

    StThreeVectorD           mFitResult;
    StThreeVectorD           mFitError;
    bool                     mExternalSeedPresent;
    StThreeVectorD           mExternalSeed;
    double                   mFmin;       // best function value found
    double                   mFedm;       // estimated vertical distance remaining to minimum
    double                   mErrdef;     // value of UP defining parameter uncertainty
    int                      mNpari;      // number of variable parameters
    int                      mNparx;      // highest parameter number defined
    int                      mStatus;     // status flag 
    bool                     mVertexConstrain; // Use vertex constraint from db
    bool                     mRequireCTB; // require CTB for seed
    bool            use_ITTF;    //Use only tracks with ITTF encoded method
    double                   mWeight ; // Weight in fit for vertex contraint
    StPhysicalHelixD*        mBeamHelix ; // Beam Line helix

    //jan--------------------
 private:
    struct ctbHit {
      float phi;
      float eta;
      float adc;
    };
    vector<ctbHit> mCtbHits;
    
    bool matchTrack2CTB (StTrack* rTrack, float & sigma);
    void collectCTBhitsData(StEvent* );
    bool collectCTBhitsMC();
    void ctb_get_slat_from_data(int slat, int tray, float & phiDeg, float &eta);
    bool ppLMV4();
    int mCtbThres_ch;// to reject slats below threshold
    float mCtbThres_mev;
    unsigned int             mMinNumberOfFitPointsOnTrack;
    double mMaxTrkDcaRxy;   //DCA to nominal beam line for each track
    double mMinTrkPt;  //~ pT=0.16(GeV/c) == R=2 (m )in 2001
    float  mMatchCtbMax_eta;
    float  mMatchCtbMax_phi;
    float  mDVtxMax;
    int    mBLequivNtr;
    int n1,n2,n3,n4,n5,n6;
    float mBfield;// magnetic field
    float  mCtbEtaSeg, mCtbPhiSeg;
    int mTotEve;
    int eveID;
    StMaker *mDumMaker;

    struct JHelix {StPhysicalHelixD helix; float sigma; };

    vector<JHelix> mPrimCand;

};






/*
ppLMV use new set of params
    INT:  CtbThres/ch=2   MinTrkPonits=10   i2=0   i3=0   i4=0   i5=0   i6=0   i7=0   i8=0   i9=9999
   FLOAT:  CtbThres/MeV=1.000000  MaxTrkDcaRxy=3.900000  MinTrkPt/GeV=0.200000  CtbEtaErr=0.020000  CtbPhiErr/deg=1.000000  MaxTrkDcaZ=180.000000  f6=0.000000  f7=0.000000  f8=0.000000  f9=8888.000000

 * Description:
 * StEvent based vertex fitter using a robust potential.
 * The actual fit is performed by MINUIT (TMinuit).
 * For documentation the following links and documents
 * are very useful:
 * http://wwwinfo.cern.ch/asdoc/minuit/minmain.html
 * http://root.cern.ch/root/html/TMinuit.html
 * http://www-glast.slac.stanford.edu/software/root/GRUG/docs/Feature/GRUGminuit.pdf
 *
 *
 * Member Functions:
 * -----------------
 * StppLMVVertexFinder::fit(StEvent* evt)
 * Find and fit the vertex for given event.
 *
 * StThreeVectorD StppLMVVertexFinder::result()
 * Returns the found vertex.
 *
 * int StppLMVVertexFinder::status()
 * The meaning of the return values of status() is as follows:
 *   -1 = not enough good tracks for fit
 *        in this case fit() returns false.
 *   All other values are related to the actual fit
 *   and reflect the status of the covariant matrix
 *   and thus the quality of the fit.
 *   (See also MNSTAT in Minuit documentation)
 *   0 = not calculated at all
 *   1 = diagonal approximation only
 *   2 = full matrix, but forced positive-definite
 *   3 = full accurate covariant matrix
 *
 * void StppLMVVertexFinder::setExternalSeed(const StThreeVectorD& seed);
 * If the seed is known, e.g. from pVPD, ZDC, or BBC, the estimated
 * position can be passed to the fitter. In this case the fit performs
 * faster, but not necessarily more accurate.
 * The seed has to be provided for every fit (fit()). It will only
 * be used for the next fit.
 *
 * void StppLMVVertexFinder::setPrintLevel(int level);
 * Set Minuit print level: 0-3
 * 0 means essentially no output
 * 3 prints a lot, for debugging only
 * 1 current default level
 *
 * void StppLMVVertexFinder::printInfo([ostream& os]);
 * Prints information of the last fit to output stream os.
 * If no argument is given the info is printed to cout.
 *
 * Example code:
 *
 * StEvent *event = ...;
 * StppLMVVertexFinder myfinder;
 * StThreeVectorD myvertex;
 * if (myfinder.fit(event)) {
 *     myvertex = myfinder.result();
 *     myfinder.printInfo();
 * }
 * else
 *     cout << "Error: vertex fit failed, no vertex." << endl;
 *
 * PP vertex finding:
 * For proton-proton (and presumable dAu) vertex finding, we only do a 
 * 1D fit and use the beamline constraint to get the x and y positions of the 
 * vertex.  To enable this mode, use:
 *
 *  myvertex.UseVertexConstraint(x0,y0,dzdy,dydz,weight)
 *
 * We also have the option of requiring that at least one track matches
 * the CTB during the first two scans for the vertex (these are coarse scans
 * to locate the probable z vertex
 *
 * myvertex.CTBforSeed();
 *
 * During the final fit (once the z position of the vertex has been constrained)
 * there is no CTB requirement.  To get the number of tracks with CTB match:
 *
 *  myvertex.NCtbMatches()
 *  
 ***************************************************************************
 *
 * $Log: StppLMVVertexFinder.h,v $
 * Revision 1.1  2004/07/21 01:53:18  balewski
 * first
 *
 * Revision 1.4  2004/04/06 02:43:43  lbarnby
 * Fixed identification of bad seeds (no z~0 problem now). Better flagging. Message manager used.
 *
 * Revision 1.3  2003/05/12 21:10:06  lbarnby
 * Made destructor virtual
 *
 * Revision 1.2  2003/05/09 22:19:51  lbarnby
 * Now also calculates and reports error on vertex. Corrected filter to use ITTF tracks. Some temporary protections against inf/Nan. Skip delete of TMinuit class since causing seg. fault.
 *
 * Revision 1.1  2002/12/05 23:42:46  hardtke
 * Initial Version for development and integration
 *
 **************************************************************************/

