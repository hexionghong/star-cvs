/* Copyright 1993, Lawrence Berkeley Laboratory */

/* dsxdr.h - definitions for xdr data structures */

/*
modification history
--------------------
24apr93,whg  written.
*/

/*
DESCRIPTION
TBS ...
*/

#ifndef DSXDR_H
#define DSXDR_H
#include "dstype.h"
#include <rpc/rpc.h>
#ifdef __cplusplus
extern "C" {
#endif
int dsCheckTruncation();
int dsEncodeBigEndian(XDR *xdrs, DS_DATASET_T *pDataset);
int dsEncodeLittleEndian(XDR *xdrs, DS_DATASET_T *pDataset);
int dsIgnoreTruncation();
int dsReadAll(XDR *xdrs);
int dsReadTest(XDR *xdrs, size_t count);
int dsWriteTest(XDR *xdrs, size_t count, int bigEndian);
bool_t xdr_dataset(XDR *xdrs, DS_DATASET_T **ppDataset);
bool_t xdr_dataset_data(XDR *xdrs, DS_DATASET_T *pDataset);
bool_t xdr_dataset_skip(XDR *xdrs);
bool_t xdr_dataset_type(XDR *xdrs, DS_DATASET_T **ppDataset);
#ifdef DS_PRIVATE
/******************************************************************************
*
*/
#define DS_SWAP_BUF_SIZE	512 	/* xdr_swap buffer size (multiple of 8) */
#define DS_XDR_HASH_LEN		1023	/* xdr_dataset hash table size(2^N - 1) */
/******************************************************************************
*
* Definitions for ANSI C
*
*/
#define XDR_NO_PROTO

#ifndef XDR_NO_PROTO
#undef XDR_GETBYTES
#define XDR_GETBYTES(xdrs, ptr, len)\
((bool_t (*)(XDR *, char *, unsigned))(xdrs)->x_ops->x_getbytes)(xdrs, ptr, len)

#undef XDR_PUTBYTES
#define XDR_PUTBYTES(xdrs, ptr, len)\
((bool_t (*)(XDR *, char *, unsigned))(xdrs)->x_ops->x_putbytes)(xdrs, ptr, len)

#undef XDR_GETLONG
#define XDR_GETLONG(xdrs, lp)\
((bool_t (*)(XDR *, long *))(xdrs)->x_ops->x_getlong)(xdrs, lp)

#undef XDR_PUTLONG
#define XDR_PUTLONG(xdrs, lp)\
((bool_t (*)(XDR *, long *))(xdrs)->x_ops->x_putlong)(xdrs, lp)

bool_t xdr_bytes(XDR *xdrs, char **sp, unsigned *sizep, unsigned maxsize);
bool_t xdr_double(XDR *xdrs, double *dp);
bool_t xdr_float(XDR *xdrs, float *fp);
#ifdef __sun
void xdr_free(xdrproc_t proc, char *objp);
#endif
bool_t xdr_int(XDR *xdrs, int *ip);
bool_t xdr_u_int(XDR *xdrs, unsigned *uip);
bool_t xdr_long(XDR *xdrs, long *lp);
bool_t xdr_u_long(XDR *xdrs, unsigned long *lp);
#ifdef __sun
bool_t xdr_opaque(XDR *xdrs, char *cp, unsigned cnt);
#endif
bool_t xdr_string(XDR *xdrs, char **cpp, unsigned maxsize);
#ifdef __sun
void xdrmem_create(XDR *xdrs, char *addr, unsigned size, enum xdr_op op);
#endif
void xdrstdio_create(XDR *xdrs, FILE *stream, enum xdr_op op);
#endif /* XDR_NO_PROTO */

#endif /* DS_PRIVATE */
#ifdef __cplusplus
}
#endif
#endif /* DSXDR_H */
