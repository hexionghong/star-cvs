//:>----------------------------------------------------------------------
//: FILE:      FtfHit.h
//: HISTORY:
//:           18apr2000  ppy    add hardwareId
//:           19apr2000  cs ppy add short sector for dEdx
//:>----------------------------------------------------------------------

#ifndef FTFHIT
#define FTFHIT
#include "FtfBaseHit.h"

#ifdef SL3ROOT
#include "Rtypes.h"
#else
//#define ClassDef(a,b)
#endif

   class FtfPara  ;
   class FtfTrack ;
   

   class FtfHit: public FtfBaseHit {         
   public:
       void         printLinks ( ) ;
       void         printLinks ( int level ) ;
       void         setStatus ( FtfTrack* this_track ) ;

       long         id ;
       short        phiIndex ;        // Phi index    
       short        etaIndex ;        // Eta index    
       short        flags    ;        // various flags      
       short        sector   ;        // various flags      
       //
       void*        nextVolumeHit ;  // Next volume hit            
       void*        nextRowHit    ;  // Next row hit               
       float        r    ;            // radius                     
       float        phi  ;            // azimuthal angle            
       float        dphi ;            // Error in phi               
       float        eta  ;            // hit pseudorapidity         
       float        xp   ;            // x conformal coordinate 
       float        yp   ;            // y conformal coordinate 
       short        buffer1 ;          //
       short        buffer2 ;          
       unsigned short hardwareId ;
//ClassDef(FtfHit,1)
   } ;
#endif

