/**********************************************
 *
 * StMcAnalysisMaker.h
 *
 * Examples to use the structures from
 * StMcEvent and StAssociationMaker
 *
 **********************************************/

#ifndef StMcAnalysisMaker_HH
#define StMcAnalysisMaker_HH

#ifndef StMaker_H
#include "StMaker.h"
#endif

//  - if not using the methods of the class, then we can just put class TCanvas;
//   -  however, if we are using the methods of TCanvas, then #include "TCanvas.h"

class TH1F;
class TH2F;
class TCanvas;


class StMcAnalysisMaker : public StMaker {

 public:

    StMaker* currentChain;
    StMcAnalysisMaker(const char* name = "McAnalysis",
		       const char* title = "event/McAnalysis");
    virtual ~StMcAnalysisMaker();
    virtual Int_t Init();
    virtual Int_t Make();
    virtual Int_t Finish();

    // SetZones --> divide canvas into 2 x 2 zones
    virtual void   SetZones(Int_t columns=2, Int_t rows=2);


    TH2F*     mHitResolution;    //! Diff. between x and z coordinates of the hits.
    TH2F*     coordRec;          //! X and Y coord of rec. Track.
    TH2F*     coordMcPartner;    //! X and Y coord of  MC  Track.
    // Data-members to make up the output Canvases
    TCanvas*       mAssociationCanvas;    //!   
    Int_t          mPadColumns;     // Number of the columns (TPad's) on the single Canvas
    Int_t          mPadRows;        // Number of the rows (TPad's) on the single Canvas

private:

    Bool_t drawinit;
    
    //! Histograms booking constants
    static const Int_t mNumDeltaX;
    static const Int_t mNumDeltaZ;
    static const Float_t mMinDeltaX;
    static const Float_t mMaxDeltaX;
    static const Float_t mMinDeltaZ;
    static const Float_t mMaxDeltaZ;

    // the following is a ROOT macro  that is needed in all ROOT accessible code
    ClassDef(StMcAnalysisMaker, 1)

};

inline void StMcAnalysisMaker::SetZones(Int_t columns, Int_t rows){ mPadColumns =columns; mPadRows = rows;}

#endif
