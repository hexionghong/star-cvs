/*:>--------------------------------------------------------------------
**: FILE:       tpt_residuals.c.template
**: HISTORY:
**:             00jan93-v000a-hpl- Created by stic Version
**:  Id: idl.y,v 1.14 1998/08/28 21:22:28 fisyak Exp  
**:<------------------------------------------------------------------*/
#include "tpt_residuals.h"
#include <stdlib.h>
#include <math.h>
#include "phys_constants.h"
#include "math_constants.h"

#define tls_index_sort_i_ F77_NAME(tls_index_sort_i,TLS_INDEX_SORT_I)
extern void type_of_call tls_index_sort_i_(long *, long *,long *,long *,long *);
#define gufld_ F77_NAME(gufld,GUFLD)
extern void type_of_call gufld(long *, long *);

long type_of_call tpt_residuals_(
  TABLE_HEAD_ST            *hit_h,      TCL_TPHIT_ST              *hit ,
  TABLE_HEAD_ST          *track_h,      TPT_TRACK_ST            *track ,
  TABLE_HEAD_ST            *res_h,        TPT_RES_ST              *res )
{
/*:>--------------------------------------------------------------------
**: ROUTINE:    tpt_residuals_
**: DESCRIPTION: Physics Analysis Module ANSI C template.
**:             This is an ANSI C Physics Analysis Module template
**:             automatically generated by stic from tpt_residuals.idl.
**:             Please edit comments and code.
**: AUTHOR:     hpl - H.P. Lovecraft, hplovecraft@cthulhu.void
**: ARGUMENTS:
**:       IN:
**:                hit    - PLEASE FILL IN DESCRIPTION HERE
**:               hit_h   - header Structure for hit
**:              track    - PLEASE FILL IN DESCRIPTION HERE
**:             track_h   - header Structure for track
**:    INOUT:
**:      OUT:
**:                res    - PLEASE FILL IN DESCRIPTION HERE
**:               res_h   - header Structure for res
**: RETURNS:    STAF Condition Value
**:>------------------------------------------------------------------*/
float xlocal[]={0,0,0}; /* scratch position */
float bfield[3]; /* magnetic field */
#define LEN 600000
long  loc_hit[LEN]; /* array to index the hit table sorted by track */
long  mylen=LEN; /* variable holding length of loc_hit array */
#define TRACK_LEN 20000
long loc_track[TRACK_LEN]; /* array to index the track table by track id */
long l; /* loop index */
long j; /* running track index */
long id_track;
long newmaxlen; /* new rowcount for the resolution table */
float x1,y1,z1;
float xc,yc,psic,radius;
float phi, phi0,phidif, tanl;
float arclen;
TCL_TPHIT_ST *jhit;
TPT_RES_ST *jres;

/* If no hits return */
if(hit_h->nok == 0) return STAFCV_OK;

/* If no tracks return */
if(track_h->nok ==0) return STAFCV_OK;

/* Get the magnetic field */
gufld_(xlocal,bfield);

/* Sort hits according to hit[i].track */
tls_index_sort_i_(&hit_h[0].nok, &hit[0].track,
		      &hit[1].track,&loc_hit[0],&mylen);

/* Correct for c indexing bu subtracting 1 */
    for(l=0;l<hit_h->nok;l++) {
      loc_hit[l]--;
      if(loc_hit[l]<0||loc_hit[l]>=hit_h->nok) { 
      return STAFCV_BAD;
      }     
    }

/* Establish pointers to get to a track by it's id */

for (l=0;l<track_h->nok; l++)
     loc_track[track[l].id]=l;

/* Loop over all the hits, until you run into track>0 */
for(l=0;l<hit_h->nok;l++)
if(hit[loc_hit[l]].track>0) break;

/* Set pointer to residuals to point to the first element */
jres = res;
/* Set size for the table of residuals */
res_h->nok = 0;

/* Start looping from the non-zero hit */
while (l<hit_h->nok && hit[loc_hit[l]].track>0)
  {
    /* get the track number */
    id_track=hit[loc_hit[l]].track/1000;
    j=loc_track[id_track];
    /* Make sure it's a valid track */
    if( track[j].flag>0){
    /* and calculate the track parameters */
    /* calculate the firstpoint on the track */
    x1 = track[j].r0*cos(track[j].phi0*C_RAD_PER_DEG);
    y1 = track[j].r0*sin(track[j].phi0*C_RAD_PER_DEG);
    z1 = track[j].z0;
    
    /* calculate radius */
    radius = 1.0/(track[j].invp*bfield[2]*C_D_CURVATURE);
    psic   = track[j].psi*C_RAD_PER_DEG + track[j].q/fabs(track[j].q)*C_PI*0.5;
    tanl   = track[j].tanl;

    /* and the position of the circle center */
    xc = x1 - radius*cos(psic);
    yc = y1 - radius*sin(psic);

    /* get the azimuthal angle (with respect to the center) for the first point */
    phi0=atan2(y1-yc,x1-xc);
    arclen=0;

    /* loop over all the points and calculate residuals */
    while (id_track == hit[loc_hit[l]].track/1000)
      {
	jhit=&(hit[loc_hit[l]]);
	jres->trk   = id_track;
	jres->hit   = jhit->id;
	/* Calculate residuum along the padrow */
	jres->resy  = (sqrt((jhit->x-xc)*(jhit->x-xc)+
		      (jhit->y-yc)*(jhit->y-yc))-radius)/
                       cos(jhit->alpha*C_RAD_PER_DEG);

	/* now calculate the track length, get the azimuthal angle 
	   (with respect to the center) for the current point */
        phi = atan2(jhit->y-yc,jhit->x-xc);
        phidif = fabs(phi-phi0);
        phidif = (phidif<C_2PI-phidif) ? phidif : C_2PI-phidif;
        arclen=radius*phidif;
	/*        phi0 = phi;*/
        jres->resz=jhit->z-z1-arclen*tanl;
	/* increase pointer to the residuals and increase the residual count */
	jres++;
        res_h->nok = res_h->nok+1;
        l++;
    	if (res_h->nok >= res_h->maxlen) { /* Increase table length*/
             newmaxlen = res_h->maxlen*1.3;
             ds2ReallocTable(&res_h,&res,newmaxlen);}
        if(l>=hit_h->nok) break;
      }
    }
    else
      {
	l++;
      }
    if(l>=hit_h->nok) break;
  }
   return STAFCV_OK;
}


