*
* $Id: seclf9.F,v 1.1.1.2 2009/02/01 17:10:35 fisyak Exp $
*
* $Log: seclf9.F,v $
* Revision 1.1.1.2  2009/02/01 17:10:35  fisyak
* Merge with .DEV2
*
* Revision 1.1.1.1  2008/12/10 20:49:05  fisyak
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
* Revision 1.1.1.1  1995/10/24 10:21:59  cernlib
* Geant
*
*
#include "geant321/pilot.h"
*CMZ :  3.21/04 23/02/95  14.46.01  by  S.Giani
*-- Author :
      SUBROUTINE SECLF9(FSE,IFSE,EX,U,E)
C       THIS ROUTINE SAMPLES AN EXIT ENERGY FROM
C       AN EVAPORATION SPECTRUM
#include "geant321/minput.inc"
      DIMENSION FSE(*),IFSE(*)
      SAVE
C       DETERMINE THETA
      IP=1
      EMAX=E-U
      NR=IFSE(IP)
      NE=IFSE(IP+1)
      IP=2*NR+1
      DO 10 I=1,NE
         IP=IP+2
         IF(E.LE.FSE(IP))GO TO 20
   10 CONTINUE
      GO TO 30
   20 IF(I.EQ.1)GO TO 40
C       DETERMINE THE INTERPOLATING SCHEME
      CALL INTSCH(IFSE,I,IS,NR)
      E1=FSE(IP-2)
      E2=FSE(IP)
      CALL INTERP(E,THETA,E1,FSE(IP-1),E2,FSE(IP+1),IS)
      GO TO 50
C       INCIDENT ENERGY IS ABOVE THE LAST INCIDENT ENERGY GIVEN
C       USE THE LAST DISTRIBUTION
   30 THETA=FSE(IP+1)
      GO TO 50
C       INCIDENT ENERGY IS BELOW THE FIRST INCIDENT ENERGY GIVEN
C       USE THE FIRST DISTRIBUTION
   40 THETA=FSE(IP+1)
C       SELECT THE EXIT ENERGY FROM THE EVAPORATION SPECTRUM
   50 R1=FLTRNF(0)
      R2=FLTRNF(0)
      W=-ALOG(R1*R2)
      EX=THETA*W
      IF(EX.LE.EMAX)RETURN
#if defined(CERNLIB_MDEBUG)
      IF(EX.GT.5.*EMAX) WRITE(IOUT,10000)EX,EMAX
10000 FORMAT(' MICAP: WARNING-EX,EMAX=',1P2E13.5,' IN SECLF9')
#endif
      EX=EMAX
      RETURN
      END
