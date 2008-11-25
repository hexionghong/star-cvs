#include <sys/types.h>
#include <string.h>
#include <assert.h>

#include <rtsLog.h>
#include <rtsSystems.h>


#include <SFS/sfs_index.h>
#include <DAQ_READER/daqReader.h>
#include <DAQ_READER/daq_dta.h>



#include <DAQ_EMC/daq_emc.h>	// need this for the old style stuff...


#include "daq_etow.h"




const char *daq_etow::help_string = "ETOW tst\n" ;

class daq_det_etow_factory : public daq_det_factory
{
public:
        daq_det_etow_factory() {
                daq_det_factory::det_factories[ETOW_ID] = this ;
        }

        daq_det *create() {
                return new daq_etow ;
        }
} ;

static daq_det_etow_factory etow_factory ;



daq_etow::daq_etow(daqReader *rts_caller)
{

	LOG(DBG,"ETOW: rts_id %d, name %s",rts_id,name) ;

	// dname is ignored 
	rts_id  = ETOW_ID ;
	name = rts2name(rts_id) ;
	sfs_name = "etow" ;
	caller = rts_caller ;
	if(caller) caller->insert(this, rts_id) ;

	raw = new daq_dta ;
	adc = new daq_dta ;

	LOG(DBG,"%s: constructor: caller %p",name,caller) ;
}

daq_etow::~daq_etow()
{
	LOG(DBG,"%s: destructor",name) ;

	delete adc ;
	delete raw ;

	return ;
}

/* figure out the presence... */
int daq_etow::Make()
{
	int dummy ;

	present = 0 ;	// assume not...
	assert(caller) ;
	
	evt_num++ ;

	LOG(DBG,"%s: Make()",name) ;

	if(presence()) {	// in SFS
		present |= 2;
	}
	else if(legacyDetp(rts_id, caller->mem)) {	// directly in DATAP
		present |= 1 ;
	}
	else if(getEmcTrgData(caller->mem,2,&dummy)) {	// perhaps in the old TRG bank (FY08); ETOW has index 1!
		present |= 4 ;
	}

	switch(present) {
	case 1 :
		LOG(NOTE,"%s: %d: has DATAP",name,evt_num) ;
		break ;
	case 2 :
		LOG(NOTE,"%s: %d: has SFS(%s)",name,present,evt_num) ;
		break ;
	case 4 :
		LOG(NOTE,"%s: %d: has DATAP within Trigger",name,evt_num) ;
		break ;
	}

	return present ;

}
	
daq_dta *daq_etow::get(const char *bank, int c1, int c2, int c3, void *p1, void *p2)
{
	Make() ;
	if(!present) return 0 ;


	if(strcasecmp(bank,"raw") == 0) {
		return handle_raw() ;
	}
	else if(strcasecmp(bank,"adc") == 0) {
		return handle_adc() ;
	}
	else {
		LOG(ERR,"%s: unknown bank \"%s\"",name,bank) ;
		return 0 ;
	}

}


daq_dta *daq_etow::handle_raw()
{
	char *from, *st ;
	int bytes ;
	
	assert(caller) ;



	if(present & 1) {	// datap...		
		char *mem = (char *)legacyDetp(rts_id, caller->mem) ;
		from = emc_single_reader(mem, &bytes, rts_id) ;	
		if(from == 0) return 0 ;

	}
	else if(present & 4) {
		from = getEmcTrgData(caller->mem,2,&bytes) ;	// etow is index "0"
		if(from == 0) return 0 ;
	}
	else {
		char str[256] ;
		char *full_name ;

		sprintf(str,"%s/sec%02d/rb%02d/raw",sfs_name,1,1) ;
		full_name = caller->get_sfs_name(str) ;
		if(!full_name) return 0 ;

		bytes = caller->sfs->fileSize(full_name) ;

		raw->create(bytes,"raw",rts_id,DAQ_DTA_STRUCT(u_char)) ;

		st = (char *) raw->request(bytes) ;

		int ret = caller->sfs->read(str, st, bytes) ;
		if(ret != bytes) {
			LOG(ERR,"ret is %d") ;
		}
		goto done ;
	}

	raw->create(bytes,"raw",rts_id,DAQ_DTA_STRUCT(u_char)) ;
	st = (char *)raw->request(bytes) ;
	memcpy(st, from, bytes) ;

	done: ;

	raw->finalize(bytes,1,1,0) ;
	raw->rewind() ;

	LOG(DBG,"Returning raw bank...") ;
	return raw ;
}


daq_dta *daq_etow::handle_adc()
{
	u_short *raw_dta ;

	LOG(DBG,"Entering adc") ;

	daq_dta *dd = handle_raw() ;

	LOG(DBG,"raw bank %p",dd) ;

	if(dd && dd->iterate()) {
		raw_dta = (u_short *) dd->Byte ;
	}
	else {
		return 0 ;
	}


	LOG(DBG,"Got raw bank, on to adc...") ;

	adc->create(1,"adc", rts_id, DAQ_DTA_STRUCT(etow_t)) ;

	etow_t *etow_p = (etow_t *) adc->request(1) ;	// need 1 struct...


	// unpack 
	u_short *data = (u_short *)((u_int)raw_dta + 4 + 128) ;	// 4 byte dummy, 128 byte header
	
	for(int j=0;j<ETOW_PRESIZE;j++) {
		for(int i=0;i<ETOW_MAXFEE;i++) {
			etow_p->preamble[i][j] = l2h16(*data++) ;
		}
	}

	for(int j=0;j<ETOW_DATSIZE;j++) {
		for(int i=0;i<ETOW_MAXFEE;i++) {
			etow_p->adc[i][j] = l2h16(*data++) ;
		}
	}


	adc->finalize(1,1,1,0) ;
	adc->rewind() ;

	return adc ;
}
