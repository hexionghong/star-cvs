/***************************************************************************
 * $Id: CRC.hh,v 1.2 1999/07/02 04:37:41 levine Exp $
 * Author: M.W. Schulz
 ***************************************************************************
 * Description: definitions for CRC checking
 *      
 *
 ***************************************************************************
 * $Log: CRC.hh,v $
 * Revision 1.2  1999/07/02 04:37:41  levine
 * Many changes - see change logs in individual programs
 *
 *
 **************************************************************************/
#ifndef CRC_HH
#define CRC_HH

#include "RecHeaderFormats.hh"

#define CRC32DEF(a,b)  {(b) = (((b) & 0x80000000) ? (((b) << 1) | 1 ) : ((b) << 1))^(a) ;}

unsigned int compute_crc_bank(Bank* d,unsigned int lcrc = 0 ) ;
unsigned int compute_crc_block(int* d,unsigned int ByteOrdering,int length,unsigned int lcrc = 0 ) ;

#endif /* end of CRC_HH */
