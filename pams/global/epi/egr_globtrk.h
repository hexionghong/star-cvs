/* egr_globtrk.h */
/* This file was made by the idl compiler "stic". Do not edit.
** Instead, edit the source idl file and then re-run the compiler.
** For help, type contact Craig Tull or Herb Ward. */
/* COMMENTS FROM IDL FILE:
   egr_globtrk.idl
   Table: egr_globtrk

 */
#ifndef EGR_GLOBTRK_H
#define EGR_GLOBTRK_H
#define EGR_GLOBTRK_SPEC \
"struct egr_globtrk { \
	long icharge; \
	long id; \
	long id_emc; \
	long id_global_pid; \
	long id_hypo_pid; \
	long id_part; \
	long id_pver; \
	long id_tof; \
	long last_row; \
	long ndegf; \
	long sflag; \
	float chisq; \
	float cov[15]; \
	float invpt; \
	float length; \
	float phi0; \
	float psi; \
	float r0; \
	float tanl; \
	float xlast[3]; \
	float z0; \
};"
typedef struct egr_globtrk_st {
	long icharge; /* charge of track */
	long id; /* global track primary key */
	long id_emc; /* f key to EMC table for track */
	long id_global_pid; /* foreign key to global_pid table */
	long id_hypo_pid; /* foreign key to hypo_pid table */
	long id_part; /* foreign key to particle table */
	long id_pver; /* f key to primary vertex for att. tracks */
	long id_tof; /* foreign key to ctf_cor table */
	long last_row; /* outermost TPC row included in track */
	long ndegf; /* no. deg. of freedom for track fit. */
	long sflag; /* Status of track reconstruction */
	float chisq; /* global fit chi squared */
	float cov[15]; /* Covariance for global track fit */
	float invpt; /* 1/pt at start of global track */
	float length; /* Track length - first to last point */
	float phi0; /* phi angle at start of global track */
	float psi; /* angle of p vector at start of gl. track */
	float r0; /* r coord at start of global track */
	float tanl; /* tan(dip) at start of global track */
	float xlast[3]; /* coordinates of outermost track point */
	float z0; /* z coord. at start of global track */
} EGR_GLOBTRK_ST;
#endif /* EGR_GLOBTRK_H */
