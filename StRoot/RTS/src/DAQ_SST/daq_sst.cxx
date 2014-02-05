#include <sys/types.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>

#include <rtsLog.h>
#include <rtsSystems.h>



#include <SFS/sfs_index.h>
#include <DAQ_READER/daqReader.h>
#include <DAQ_READER/daq_dta.h>


#include "daq_sst.h"


const char *daq_sst::help_string = "SST\n\
raw	returns raw data\n" ;

class daq_det_sst_factory : public daq_det_factory
{
public:
	daq_det_sst_factory() {
		daq_det_factory::det_factories[SST_ID] = this ;
	}

	daq_det *create() {
		return new daq_sst ;
	}
} ;

static daq_det_sst_factory sst_factory ;


daq_sst::daq_sst(daqReader *rts_caller) 
{
	rts_id = SST_ID ;
	name = rts2name(rts_id) ;
	sfs_name = "sst" ;
	caller = rts_caller ;
	
	if(caller) caller->insert(this, rts_id) ;

	raw = new daq_dta ;
	adc = new daq_dta ;

	LOG(DBG,"%s: constructor: caller %p",name,rts_caller) ;
	return ;
}

daq_sst::~daq_sst() 
{
	LOG(DBG,"%s: DEstructor",name) ;

	delete raw ;
	delete adc ;

	return ;
}



daq_dta *daq_sst::get(const char *bank, int sec, int rdo, int pad, void *p1, void *p2) 
{	
	Make() ;

	if(present == 0) return 0 ;


	if(strcasecmp(bank,"raw")==0) {
		return handle_raw(sec,rdo) ;
	}
	else if(strcasecmp(bank,"adc")==0) {
		return handle_adc(sec,rdo) ;
	}


	LOG(ERR,"%s: unknown bank type \"%s\"",name,bank) ;
	return 0 ;
}


daq_dta *daq_sst::handle_adc(int sec, int rdo, char *rdobuff, int words)
{
	int r_start, r_stop ;
	int s_start, s_stop ;
	daq_trg_word trg[1] ;

	assert(caller) ;	// sanity...

	if(!present) {
		return 0 ;
	}
	else {
		LOG(DBG,"%s: present %d",name,present) ;
	}


	if(sec <= 0) {
		s_start = 1 ;
		s_stop = 2 ;
	}
	else {
		s_start = s_stop = sec ;
	}

	if(rdo<=0) {
		r_start = 1 ;
		r_stop = 3 ;		// 1 sector has 3, 2nd has 2
	}
	else {
		r_start = r_stop = rdo ;
	}


	adc->create(128,"sst_adc",rts_id,DAQ_DTA_STRUCT(daq_sst_data_t)) ;


	for(int s=s_start;s<=s_stop;s++) {

	for(int r=r_start;r<=r_stop;r++) {
		u_int *dta ;
		int found ;	// helper

		if(rdobuff == 0) {
			daq_dta *raw_d = handle_raw(s,r) ;
			if(raw_d == 0) continue ;
			if(raw_d->iterate() == 0) continue ;

			dta = (u_int *) raw_d->Void ;
			words = raw_d->ncontent/4 ;

		}
		else {
			dta = (u_int *) rdobuff ;
		} ;

		//for ERRORs printounts
		get_l2((char *)dta,words,trg,r) ;

		u_int *d32 = dta ;
		u_int *d32_end = dta + words ;
		u_int *d32_start = dta ;

		//sanity!
		if(d32_end[-1] != 0xBBBBBBBB) {
			LOG(ERR,"S%2-%d: last word is 0x%08X, expect 0xBBBBBBBB -- data corrupt,skipping!",s,r,d32_end[-1]) ;
			continue ;
		}

	
		//we'll assume SST has bugs in the header so we don't do any checks but search for the data
		//immediatelly
		while(d32<d32_end) {
			u_int d = *d32++ ;

			if(d == 0xDDDDDDDD) {
				d32-- ;	// move back ;
				found = 1 ;
				break ;
			}
		}

		if(!found) {
			LOG(ERR,"S%d-%d: can't find 0xDDDDDDDD -- data corrupt, skipping!",s,r) ;
			continue ;
		}


		int fib = 0 ;
		while(d32<d32_end) {
			u_int d = *d32++ ;

			if(d != 0xDDDDDDDD) {
				u_int *d_here = d32 - 1 ;	// go back one
				LOG(ERR,"S%d-%d: fiber %d: can't find 0xDDDDDDDD at offset %d [0x%08X] -- data corrupt, skipping!",s,r,fib,
				    d_here-d32_start,*d_here) ;

				d_here -= 2 ;
				for(int i=0;i<5;i++) {
					LOG(ERR,"     %d: 0x%08X",d_here-d32_start,*d_here) ;
					d_here++ ;
				}
				goto err_ret ;
			}

			int words = d32[0] & 0x000FFFF0 ;
			words >>= 4 ;

			LOG(NOTE,"S%d-%d: fiber %d: ID 0x%08X, words %d",
			    s,r,fib,d32[0],words) ;
				
			if(d32[0] & 0xFFF00000) {
				if(d32[0] == 0x001000A0) {	// empty
					LOG(WARN,"S%d-%d: fiber %d: empty [0x%08X]",s,r,fib,d32[0]) ;					
				}
				else {
					LOG(ERR,"S%d-%d: fiber %d: odd data header 0x%08X",s,r,fib,d32[0]) ;
				}
			}

			words -= 1 ;	// for some reason...

			//first 9 words are some header
			for(int i=0;i<12;i++) {
				LOG(NOTE,"   %d: 0x%08X",i,d32[i]) ;
			}
			
			d32 += 9 ;	// skip this header
			words -= 9 ;

			int strip = 0 ;
			int hybrid = 0 ;


			daq_sst_data_t *sst = 0 ;
			daq_sst_data_t *sst_start  ;

			if(words) {
				sst = (daq_sst_data_t *)adc->request(3*words) ;
			}

			sst_start = sst ;

			//here is the ADC
			for(int i=0;i<words;i++) {
				d = *d32++ ;

				if(strip >= 768) {
					LOG(ERR,"S%d-%d: fiber %d, bad strip %d",s,r,fib,strip) ;
					goto err_ret ;
				}
				
				sst->strip = strip ;	
				sst->hybrid = hybrid++ ;
				sst->adc = d & 0x3FF ;
				sst++ ;

				if(hybrid==16) {
					hybrid = 0 ;
					strip++ ;
				}

				if(strip >= 768) {
					LOG(ERR,"S%d-%d: fiber %d, bad strip %d",s,r,fib,strip) ;
					goto err_ret ;
				}

				sst->strip = strip ;
				sst->hybrid = hybrid++ ;
				sst->adc = (d & 0xFFC00)  >> 10 ;
				sst++ ;
				
				if(hybrid==16) {
					hybrid = 0 ;
					strip++ ;
				}

				if(strip >= 768) {
					LOG(ERR,"S%d-%d: fiber %d, bad strip %d",s,r,fib,strip) ;
					goto err_ret ;
				}

				sst->strip = strip ;
				sst->hybrid = hybrid++ ;
				sst->adc = (d & 0x3FF00000) >> 20 ;
				sst++ ;

				if(hybrid==16) {
					hybrid = 0 ;
					strip++ ;
				}

			}

			
			//end of adc
			if(sst-sst_start) {
				LOG(NOTE,"Got %d structs, requested %d",sst-sst_start,3*words) ;
				adc->finalize(sst-sst_start,s,r,fib) ;
			}

			fib++ ;
			if(fib==8) break ;

			
		}

		if(*d32 != 0xCCCCCCCC) {
			LOG(ERR,"S%d-%d: can't find 0xCCCCCCCC at offset %d [0x%08X] -- data corrupt, skipping!",s,r,d32-d32_start,*d32) ;
			d32 -= 2 ;
			while(d32 < d32_end) {
				LOG(ERR,"    %d: 0x%08X",d32-d32_start,*d32) ;
				d32++ ;
			}
			goto err_ret ;
		}
		

		err_ret:;


	}	// end of loop over RDOs [1..3]

	}	// end of loop over Sectors [1..2]

	adc->rewind() ;

	return adc ;
	
}


daq_dta *daq_sst::handle_raw(int sec, int rdo)
{
	char *st ;
	int r_start, r_stop ;
	int s_start, s_stop ;
	int bytes ;
	char str[256] ;
	char *full_name ;


	assert(caller) ;	// sanity...

	if(!present) {
		return 0 ;
	}
	else {
		LOG(DBG,"%s: present %d",name,present) ;
	}



	if(sec <= 0) {
		s_start = 1 ;
		s_stop = 2 ;
	}
	else {
		s_start = s_stop = sec ;
	}

	if(rdo<=0) {
		r_start = 1 ;
		r_stop = 3 ;		// 1 sector has 3, 2nd has 2
	}
	else {
		r_start = r_stop = rdo ;
	}


	raw->create(8*1024,"sst_raw",rts_id,DAQ_DTA_STRUCT(char)) ;


	for(int s=s_start;s<=s_stop;s++) {

	for(int r=r_start;r<=r_stop;r++) {
		sprintf(str,"%s/sec%02d/rb%02d/raw",sfs_name, s, r) ;
		full_name = caller->get_sfs_name(str) ;
		
		if(!full_name) continue ;
		bytes = caller->sfs->fileSize(full_name) ;	// this is bytes


		st = (char *) raw->request(bytes) ;
		
		int ret = caller->sfs->read(str, st, bytes) ;
		if(ret != bytes) {
			LOG(ERR,"ret is %d") ;
		}

	
		raw->finalize(bytes,s,r,0) ;	;

	}	// end of loop over RDOs [1..3]

	}	// end of loop over Sectors [1..2]

	raw->rewind() ;

	return raw ;
	
}

	
int daq_sst::get_l2(char *buff, int words, struct daq_trg_word *trg, int rdo)
{
	// will look the same as PXL!
	int t_cou = 0 ;
	u_int *d32 = (u_int *)buff ;
	u_int err = 0 ;
	int last_ix = words - 1 ;
	int token, daq_cmd, trg_cmd ;
	u_int token_word ;

	// quick sanity checks...
	// errors in the lower 16 bits are critical in the sense that
	// I'm unsure about the token and events coherency.
	// Critical errors issue ERROR
	// Others issue WARN

	if(d32[0] != 0xAAAAAAAA) err |= 0x10000 ;			// header error
	if(d32[last_ix] != 0xBBBBBBBB) err |= 0x1	;	// trailer error

	token_word = d32[1] ;	// default

	//let's enumerate observed errors depending on location of 0xDDDDDDDD
	int got_dd = -1 ;
	for(int i=0;i<12;i++) {	// search, oh, 12 words only...
		if(d32[i] == 0xDDDDDDDD) {
			got_dd = i ;
			break ;
		}
	}

	switch(got_dd) {
	case 7 :		//no 0xAAAAAAAA at all
		token_word = d32[0] ;
		err |= 0x20000 ;
		break ;
	case 8 :		// this is normal
		break ;
	case 9 :		// double 0xAAAAAAAA
		token_word = d32[2] ;
		err |= 0x40000 ;	
		break ;
	default :
		err |= 0x2 ;	// unknown case so far...
		break ;
	}


#if 0
	// special TCD-only event check; is this implemented by SST?? Should be!
	if(token_word == 0x0000FFFF) {
		LOG(WARN,"RDO %d: trigger-only event...",rdo) ;
		token = 4097 ;
		daq_cmd = 0 ;
		trg_cmd = 4 ;
	}
	else {
#endif
	
	token = token_word & 0xFFF ;
	daq_cmd = (token_word & 0xF000) >> 12 ;
	trg_cmd = (token_word & 0xF0000) >> 16 ;

	// more sanity
	if(token == 0) {
		token = 4097 ;	// override with dummy token!
		err |= 4 ;
	}

	//check for USB trigger
	if(token_word & 0xFFF00000) {	// USB trigger?? Can't be
		err |= 4 ;
	}

	if(trg_cmd != 4) err |= 4 ;

	trg[t_cou].t = token ;
	trg[t_cou].daq = daq_cmd ;
	trg[t_cou].trg = trg_cmd ;
	trg[t_cou].rhic = d32[7] ;
	trg[t_cou].rhic_delta = 0 ;
	t_cou++ ;

#if 0	
	// get other trigger commands...
	int last_p = last_ix - 1 ;	// at CRC

	//u_int crc = d32[last_p] ;


	last_p-- ;	// at end of TCD info
	int first_trg = -1 ;		

	for(int i=last_p;i>=0;i--) {
		if(d32[i] == 0xCCCCCCCC) {	// trigger commands header...
			first_trg = i + 1 ;
			break ;
		}
	}

	if(first_trg > 0) {	// found other trigger commands...
		for(int i=first_trg;i<=last_p;i++) {
			trg[t_cou].t = d32[i] & 0xFFF ;
			trg[t_cou].daq = (d32[i] & 0xF000) >> 12 ;
			trg[t_cou].trg = (d32[i] & 0xF0000) >> 16 ;
			trg[t_cou].rhic = trg[0].rhic + 1 ;	// mock it up...
			trg[t_cou].rhic_delta = 0 ;

			switch(trg[t_cou].trg) {
			case 0xF :
			case 0xE :
			case 0xD :
				break ;
			default :
				continue ;
			}

			t_cou++ ;

			if(t_cou >= 120) {	// put a sanity limiter...
				err |= 8 ;
				break ;
			}
		}
	}
#endif

	//err = t_cou ;
	if(err & 0xFFFF) {
		LOG(ERR,"RDO %d: error 0x%X, t_cou %d",rdo,err,t_cou) ;

		for(int i=0;i<16;i++) {
			LOG(ERR,"  RDO %d: %2d/%2d: 0x%08X",rdo,i,words,d32[i]) ;
		}

		int s = last_ix - 10 ;
		if(s < 0) s = 0 ;

		for(int i=s;i<=last_ix;i++) {
			LOG(ERR,"  RDO %d: %2d/%2d: 0x%08X",rdo,i,words,d32[i]) ;
		}

		//HACK
		trg[0].t = 4097 ;	// kill this guy...
		if(t_cou) t_cou = 1 ;	// disregard anything else as well...
	}
	else if(err & 0xFFFF0000) {	//non critical, warnings

		LOG(WARN,"RDO %d: error 0x%X, t_cou %d",rdo,err,t_cou) ;

		for(int i=0;i<16;i++) {
			LOG(WARN,"  RDO %d: %2d/%2d: 0x%08X",rdo,i,words,d32[i]) ;
		}

		int s = last_ix - 10 ;
		if(s < 0) s = 0 ;

		for(int i=s;i<=last_ix;i++) {
			LOG(WARN,"  RDO %d: %2d/%2d: 0x%08X",rdo,i,words,d32[i]) ;
		}



	}

	return t_cou ;
}
