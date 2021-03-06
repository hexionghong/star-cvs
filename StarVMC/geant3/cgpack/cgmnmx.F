*
* $Id: cgmnmx.F,v 1.1.1.2 2009/02/01 17:10:06 fisyak Exp $
*
* $Log: cgmnmx.F,v $
* Revision 1.1.1.2  2009/02/01 17:10:06  fisyak
* Merge with .DEV2
*
* Revision 1.1.1.1  2008/12/10 20:46:02  fisyak
* Merge with macos version
*
* Revision 1.1.1.1  2005/05/25 22:36:39  fisyak
* Alice version of geant3 (-minicern)
*
* Revision 1.1.1.1  2002/07/24 15:56:27  rdm
* initial import into CVS
*
* Revision 1.1.1.1  2002/06/16 15:17:54  hristov
* Separate distribution  of Geant3
*
* Revision 1.1.1.1  1999/05/18 15:55:04  fca
* AliRoot sources
*
* Revision 1.1.1.1  1995/10/24 10:19:44  cernlib
* Geant
*
*
#include "geant321/pilot.h"
*CMZ :  3.21/02 29/03/94  15.41.31  by  S.Giani
*-- Author :
      SUBROUTINE CGMNMX(CG,RMIN,RMAX)
************************************************************************
*                                                                      *
*     Name: CGMNMX                                                     *
*     Author: E. Chernyaev                       Date:    19.08.88     *
*                                                Revised:              *
*                                                                      *
*     Function: Compute scope of CG-object                             *
*                                                                      *
*     References: CGTSTR                                               *
*                                                                      *
*     Input:  CG(*) - CG-object                                        *
*                                                                      *
*     Output: RMIN(3) - min coordinates                                 *
*             RMAX(3) - max coordinates                                 *
*                                                                      *
*     Errors: none                                                     *
*                                                                      *
************************************************************************
#include "geant321/cggpar.inc"
      REAL      CG(*),RMIN(3),RMAX(3)
*-
      DO 100 I=1,3
        RMIN(I) =+999999.
        RMAX(I) =-999999.
  100   CONTINUE
      CALL CGTSTR(CG,IREP)
      IF (IREP .LT. 0)          GOTO 999
*
      NFACE  = CG(KCGNF)
      IF (NFACE .EQ. 0)         GOTO 999
      JCG    = LCGHEA
      DO 300 NF=1,NFACE
        NEDGE  = CG(JCG+KCGNE)
        JCG    = JCG + LCGFAC
        DO 200 NE=1,NEDGE
          IF (CG(JCG+KCGX1) .LT. RMIN(1))       RMIN(1) = CG(JCG+KCGX1)
          IF (CG(JCG+KCGY1) .LT. RMIN(2))       RMIN(2) = CG(JCG+KCGY1)
          IF (CG(JCG+KCGZ1) .LT. RMIN(3))       RMIN(3) = CG(JCG+KCGZ1)
          IF (CG(JCG+KCGX2) .LT. RMIN(1))       RMIN(1) = CG(JCG+KCGX2)
          IF (CG(JCG+KCGY2) .LT. RMIN(2))       RMIN(2) = CG(JCG+KCGY2)
          IF (CG(JCG+KCGZ2) .LT. RMIN(3))       RMIN(3) = CG(JCG+KCGZ2)
*
          IF (CG(JCG+KCGX1) .GT. RMAX(1))       RMAX(1) = CG(JCG+KCGX1)
          IF (CG(JCG+KCGY1) .GT. RMAX(2))       RMAX(2) = CG(JCG+KCGY1)
          IF (CG(JCG+KCGZ1) .GT. RMAX(3))       RMAX(3) = CG(JCG+KCGZ1)
          IF (CG(JCG+KCGX2) .GT. RMAX(1))       RMAX(1) = CG(JCG+KCGX2)
          IF (CG(JCG+KCGY2) .GT. RMAX(2))       RMAX(2) = CG(JCG+KCGY2)
          IF (CG(JCG+KCGZ2) .GT. RMAX(3))       RMAX(3) = CG(JCG+KCGZ2)
          JCG    = JCG + LCGEDG
  200     CONTINUE
  300   CONTINUE
*
  999 RETURN
      END
