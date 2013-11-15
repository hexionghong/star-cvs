#ifndef _RTS_MONITOR_H_
#define _RTS_MONITOR_H_

/* LINUX issues */
#ifndef SHM_SHARE_MMU
#define SHM_SHARE_MMU  0
#endif



/* unused ! */
#define RTS_MON_SHM_KEY	0xffab0000 




/* 
	Default monitoring host.
	Default monitoring UDP port.
*/

#define RTS_MON_PORT		8100


#ifdef RTS_PROJECT_PP
	#define RTS_MON_HOST		"130.199.91.18"	/* old pp2pp host, deprecated */
#else
	#ifdef RTS_DAQMAN
		#define RTS_MON_HOST	RTS_DAQMAN
	#else
		#define RTS_MON_HOST	"boothost"	/* only really good for vxworks but I leave it in */
	#endif
#endif


#define RTS_MON_FILE		"/RTS/log/monServer.dta"		/* always! */


/*
	All the items are unsigned ints because I don't want to deal
	with allignment problems on var. compilers/platforms.
	All MUST be BigEndian!
	Correction...actually, the endianess is determined through the "task" i.e.
	task should be less than 255 and if it isn't, the we need to swap...

*/



#define RTS_MON_VERSION_OLD	0x000000003	/* in yyyy.xxxx BCD format */
#define RTS_MON_VERSION		0x000000003	/* in yyyy.xxxx BCD format */

#define RTS_MON_VERSION_X_FLAG	0x10000000 
#define RTS_MON_VERSION_X	(RTS_MON_VERSION_X_FLAG | RTS_MON_VERSION)	/* FY04 (Oct03) extended version */

/* total number of entities (i.e. node-task pairs we support) */
#define RTS_MON_MAX_NODES	256

#define RTS_MON_USER_WORDS	100	/* NX4bytes of storage for user defined entries */ 
#define RTS_MON_SYS_WORDS	17	/* DO NOT MODIFY UNLESS ONE CHANGES THE rtsMonStruct ! */

struct rtsMonRequired           /* this represents the minimum packet necessary */
{
  unsigned int size ;           /* size in bytes of this message */
  unsigned int node ;           /* daq Node Id of this sender */
  unsigned int task ;           /* DAQ TaskId of the sender */
  unsigned int version ;        /* version of this struct - see DAQ_MON_VERSION */
  unsigned int tim ;            /* current time in UNIX seconds */
  unsigned int state ;          /* 0 OFF, 1 ON, 2 AUX */
};


struct rtsMonStruct {
/* these MUST be present! */
	unsigned int size ;		/* size in bytes of this message */
	unsigned int node ;		/* RTS Node Id of this sender */
	unsigned int task ;		/* RTS TaskId of the sender */
	unsigned int version ;		/* version of this struct - see RTS_MON_VERSION */
	unsigned int tim ;		/* current time in UNIX seconds */
	unsigned int state ;		/* RC States */
/* these should be present although one can use 0xffffffff to signify Not Applicable */
	unsigned int tknIn ;		/* last announced token */
	unsigned int tknOut ;		/* last released token */
	unsigned int tknBad ;		/* last erroneus token */
	unsigned int couEvtsIn ;	/* tokens currently in the system */
	unsigned int couEvtsRun ;	/* events in this run */
	unsigned int couEvtsAll ;	/* events since boot */
	unsigned int couEvtsBad ;	/* rejected events because of errors in this run */
	unsigned int busy ;		/* % of time the system was BUSY i.e. incapable of new events */
	unsigned int evtsSec ;		/* input rate in events/second */
	unsigned int kbSecEvb ;		/* rate in kB/sec to the EVB */
	unsigned int kbSecAux ;		/* rate in kB/sec to the L3 system */
/* used for user defined entries --> see comment below */
	unsigned int user[RTS_MON_USER_WORDS] ;
} ;


// old! total size in shared memory 
#define RTS_MON_DATA_SIZE_V3	(sizeof(struct rtsMonStruct)*RTS_MON_MAX_NODES)

// new X-tended version...
#define RTS_MON_PACKET_SIZE	(10*1024)
#define RTS_MON_DATA_SIZE	(RTS_MON_PACKET_SIZE*RTS_MON_MAX_NODES)


/* 
	Maximum values of <name,value> strings in user space after which 
	the display program will chop the string off.
	Used to prevent crashes and to ease the display formatting.
*/
#define RTS_MON_NAME_MAX	12
#define RTS_MON_VAL_MAX		24


/* value which means "Not Applicable" and will be shown as "N.A." in the display */
#define RTS_MON_NA		0xFFFFFFFF
#define RTS_MON_NA_STRING	"N.A."

/*
	based upon the size ("size") the displaying program will determine the amount of
	user-added variables (if any) at the end of the structure.
	The "user" field is treated as a sequence of zero-terminated C strings which will
	be displayed in pairs <name,value> i.e.
	"File Name :","Myfile","Really bad tokens","123" etc.

*/


struct rtsMonGB {
	struct {
	  char name[32] ;	// if strlen() == NULL, unused..
	  u_int off_id ;		// offline id number i.e. 5001
	  u_int fired ;		// received from trigger
	  u_int rate ;		// rate of above
	  u_int l3_sent ;		// sent to L3
	  u_int aborted_gb ;	// released by GB
	  u_int aborted_l3 ;	// released by L3
	  u_int aborted_l25 ;	// released as L2.5
	  u_int err ;		// events with error
	  u_int built ;		// events built _successfully_
	  u_int xpress ;		// events sent to xpress stream
	} trgs[33] ;	// 0-31 are the usual, 32 is the sum
} ;


struct rtsMonTCD {
	u_int deadtime[32] ;
} ;


struct rtsMonSCA {
	struct {
		char name[32] ;
		u_int rate ;
	} trgs[33] ;	// 0-32 are normal, the rest is reserved

} ;

struct rtsMonL1Counters {
  struct {
    char name[32];
    u_int rate;
    u_int deadtime;
  } trgs[33];
  u_int detector_deadtime[16];
};

struct rtsMonEVB {
	u_int mb_run ;	// MB stored in this run 
	u_int gb_free ;	// still free on all disks
	u_int gb_all ;	// total capacity on all discs (for %-free calc...)
	// stuff pertaining to RCF goes here..

	u_int run_number ;	// hm, this is old, why do I have this here? *shrug*
	struct rtsMonGB gb ;	// new in FY09 -- EVB sends GB stuff as well!
} ;

struct rtsMonEVB_supertask {
  u_int mon[RTS_MON_SYS_WORDS];
  // General
  u_int mb_run;     // total data size for this run
  u_int gb_free;    // free disk space
  u_int gb_all;     // total disk space

  // RCF
  u_int files_sent;     // sent within last resets on the hour
  u_int files_waiting;  // waiting to be sent

  // By trigger Information
  struct {
    u_int off_id ;	   // offline id number i.e. 5001
  
    u_int fired ;	   // received from trigger
    u_int rate ;	   // rate of above
    u_int built ;	   // events built _successfully_

    u_int aborted_l25 ;	   // released as L2.5
    u_int err ;		   // events with error (det timeout, evb resources)
    u_int spurious;        // events with spurious (trg timeout, evt overrun)

    u_int stream;	   // which stream evts sent to

  } trgs[33] ;	           // 0-31 are the usual, 32 is the sum
};



// RCF writers structure
// the usual state is:
//	RC_PRESENT or RC_NONE	dormant
//	RC_READY	attempting to connect to RCF
//	RC_RUNNING	currently writing to RCF
//	RC_ERROR	an RCF error encountered
// Usual monioring words are:
//	kbSecEvb	is the last known rate
struct rtsMonRCF {
        char type[20];
        int curr_run;
        int curr_idx;
	struct {
		u_int run ;		// run number ; 0 means invalid/empty
		u_int mb_done ;		// MB already stored for this run
		u_int mb_all ;		// ...of MB total for this run
		u_int seq_done ;	// part number already stored
		u_int seq_all ;		//... of parts remaining
	} runs[11] ;

	// runs[0] is the _current_ run _if_ the state is either RC_RUNNING or RC_ERROR
	// runs[10] is the summary of remaining runs:
	//		run is the _number_ of runs remaining
	//		mb_all	is the MB remaining
	//		seq_all is the sequences remaining
};


// xtended structure used by RC/handler
// The "state" from the standard header is used as the general state
// The couEvtsRun from the standard header is the number of "scheduled"
// events.
// If not running the data should reflect the state of the last known
// run...
struct rtsMonRC {
	u_int run ;		// run number
	u_int type ;		// PEDESTAL etc.
	char config[128] ;	// Configuration name
	char dest[64] ;		// EVB destination name
	u_int t_start ;		// Start run time
	u_int t_end ;		// Stop run time (0 if not yet finished)
	u_int accepted ;	// bits: 1 operator rejected, 2 force stop by sombody, 4 unsuccessfull end
	u_int node_count ;	// the count of nodes following (all of which are in the run...)
        u_short err_node ;        // jml... the node that caused the last stop run
        u_short n_err_node;
	struct {
		u_short node ;
		u_short task ;
		u_int state ;
	} nodes[256] ;
	struct {
		char name[32] ;	// if strlen() == NULL, unused..
		u_int off_id ;		// offline id number i.e. 5001
	} trgs[32] ;
} ;


struct rtsMonL1 {		// used by GL3 as well!
	u_int late_events ;	// number of events above the time cutoff (i.e. 7 ms)
	u_int max_us ;		// the current maximum turaround time (in us)
	struct {
		u_int off_id ;		// offline id number
		u_int accepted ;	// fired in the run so far
		u_int rate ;		// rate/sec of "accepted"
		u_int aborted ;		// number of evebts aborted in this run
	} trgs[33] ;			// 0-31 are per trigger, 32 is the total sum...
} ;

struct rtsMonDET {
	u_int val_rbs ;	// valid RBs
	u_int cfg_rbs ;	// configured
	u_int buffs ;	// EVB/SL3 buffers free
	u_int evtsSpur ;	// spurious events; evtsBad are only the bad ones _with_ the Trigger
	u_char rb_status[12] ;	// USED in DDL dets!
} ;


#endif
