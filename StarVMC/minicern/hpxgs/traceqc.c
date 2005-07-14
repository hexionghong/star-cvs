/*
 * $Id: traceqc.c,v 1.1.1.1 2005/05/25 20:36:46 fisyak Exp $
 *
 * $Log: traceqc.c,v $
 * Revision 1.1.1.1  2005/05/25 20:36:46  fisyak
 *
 * Revision 1.1.1.1  2002/07/24 15:56:28  rdm
 * initial import into CVS
 *
 * Revision 1.1.1.1  1999/05/18 15:55:33  fca
 * AliRoot sources
 *
 * Revision 1.1.1.1  1996/02/15 17:50:06  mclareni
 * Kernlib
 *
 */
#include "kerngen/pilot.h"
/*>    ROUTINE TRACEQC
  CERN PROGLIB# N105    TRACEQC         .VERSION KERNHPX  1.04  950928
  ORIG.  3/05/95  FR, JZ
  subsidiary to TRACEQ
*/
#if defined(CERNLIB_QX_SC)
      void traceqc_()
#endif
#if defined(CERNLIB_QXNO_SC)
      void traceqc()
#endif
{
      void U_STACK_TRACE();       /* somewhere in Fortran RTL */
      U_STACK_TRACE();
      return;
}
/*> END <----------------------------------------------------------*/
