*
* $Id: rnmaxf.F,v 1.1.1.2 2009/02/01 17:10:35 fisyak Exp $
*
* $Log: rnmaxf.F,v $
* Revision 1.1.1.2  2009/02/01 17:10:35  fisyak
* Merge with .DEV2
*
* Revision 1.1.1.1  2008/12/10 20:49:04  fisyak
* Merge with macos version
*
* Revision 1.1.1.1  2005/05/25 22:37:58  fisyak
* Alice version of geant3 (-minicern)
*
* Revision 1.1.1.1  2002/07/24 15:56:27  rdm
* initial import into CVS
*
* Revision 1.1.1.1  2002/06/16 15:18:43  hristov
* Separate distribution  of Geant3
*
* Revision 1.1.1.1  1999/05/18 15:55:21  fca
* AliRoot sources
*
* Revision 1.1.1.1  1995/10/24 10:21:58  cernlib
* Geant
*
*
#include "geant321/pilot.h"
*CMZ :  3.21/04 23/02/95  14.46.01  by  S.Giani
*-- Author :
C*********************************************************************
      FUNCTION RNMAXF(T)
C T := most probable value of distribution
C*********************************************************************
      DATA FF/0./
      SAVE FF,R1SQ,W,U
      U=EXPRNF(U)
      IF(FF) 30 ,10 ,30
   10 R1=FLTRNF(R1)
      R2=FLTRNF(R2)
      R1SQ=R1*R1
      R2SQ=R2*R2
      RSQ=R1SQ+R2SQ
      IF(RSQ-1.) 20 ,20 ,10
   20 W=EXPRNF(W)/RSQ
      FF=1.
      RNMAXF=(R2SQ*W+U)*T
      GO TO 40
   30 FF=0.
      RNMAXF=(R1SQ*W+U)*T
   40 RETURN
      END
