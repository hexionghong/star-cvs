*
* $Id: gphini.F,v 1.1.1.2 2009/02/01 17:10:32 fisyak Exp $
*
* $Log: gphini.F,v $
* Revision 1.1.1.2  2009/02/01 17:10:32  fisyak
* Merge with .DEV2
*
* Revision 1.1.1.1  2008/12/10 20:48:41  fisyak
* Merge with macos version
*
* Revision 1.1.1.1  2005/05/25 22:37:55  fisyak
* Alice version of geant3 (-minicern)
*
* Revision 1.2  2003/11/28 11:23:56  brun
* New version of geant321 with all geant3 routines renamed from G to G3
*
* Revision 1.1.1.1  2002/07/24 15:56:26  rdm
* initial import into CVS
*
* Revision 1.1.1.1  2002/06/16 15:18:41  hristov
* Separate distribution  of Geant3
*
* Revision 1.1.1.1  1999/05/18 15:55:20  fca
* AliRoot sources
*
* Revision 1.1.1.1  1995/10/24 10:21:29  cernlib
* Geant
*
*
#include "geant321/pilot.h"
*CMZ :  3.21/02 29/03/94  15.41.22  by  S.Giani
*-- Author :
      SUBROUTINE G3PHINI
C.
C.    ******************************************************************
C.    *                                                                *
C     *       Initialize material constants for                        *
C.    *       the photoelectric effect                                 *
C.    *                                                                *
C.    *    ==>Called by : G3PHYSI                                      *
C.    *       Author    J. Chwastowski                                 *
C.    *                                                                *
C.    ******************************************************************
C.
      CALL G3PHRIN
      CALL G3SHLIN
      CALL G3PHXIN
      END
