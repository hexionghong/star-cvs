/***************************************************************************
 * $Id: EventReader.hh,v 1.7 2000/01/04 20:54:47 levine Exp $
 * Author: M.J. LeVine
 ***************************************************************************
 * Description: common definitions for all detectors
 *      
 *
 *   change log
 * 06-June-99 MJL added EventInfo struct, changed method getEventInfo()
 * 06-June-99 MJL added printEventInfo()
 * 17-June-99 Herb Ward changed the dimension of errstr0 from 50 to 250
 * 23-Jun-99 MJL add verbose flag and setVerbose() method
 * 25-Jun-99 MJL added TPCV2P0_CPP_SR::getAsicParams(ASIC_params *);
 * 09-Jul-99 MJL added EventReader::findBank()
 * 20-Jul-99 MJL added EventReader::fprintError()
 * 20-Jul-99 MJL add alternate getEventReader with name of logfile
 * 20-Jul-99 MJL add overloaded printEventInfo(FILE *)
 * 28-Dec-99 MJL add alternate InitEventReaders, mapped and unmapped
 *
 ***************************************************************************
 * $Log: EventReader.hh,v $
 * Revision 1.7  2000/01/04 20:54:47  levine
 * Implemented memory-mapped file access in EventReader.cxx. Old method
 * (via seeks) is still possible by setting mmapp=0 in
 *
 * 	getEventReader(fd,offset,(const char *)logfile,mmapp);
 *
 *
 * but memory-mapped access is much more effective.
 *
 * Revision 1.6  1999/07/26 17:00:03  levine
 * changes to RICH file organization
 *
 * Revision 1.5  1999/07/21 21:33:08  levine
 *
 *
 * changes to include error logging to file.
 *
 * There are now 2 constructors for EventReader:
 *
 *  EventReader();
 *  EventReader(const char *logfilename);
 *
 * Constructed with no argument, there is no error logging. Supplying a file name
 * sends all diagnostic output to the named file (N.B. opens in append mode)
 *
 * See example in client.cxx for constructing a log file name based on the
 * datafile name.
 *
 * It is strongly advised to use the log file capability. You can grep it for
 * instances of "ERROR:" to trap anything noteworthy (i.e., corrupted data files).
 *
 * Revision 1.4  1999/07/10 21:31:17  levine
 * Detectors RICH, EMC, TRG now have their own (defined by each detector) interfaces.
 * Existing user code will not have to change any calls to TPC-like detector
 * readers.
 *
 * Revision 1.3  1999/07/02 04:37:41  levine
 * Many changes - see change logs in individual programs
 *
 *
 **************************************************************************/
#ifndef EVENTREADER_HH
#define EVENTREADER_HH




#include <sys/types.h>

#include <string>

#include "RecHeaderFormats.hh"
#include "Error.hh"

#define TRUE 1
#define FALSE 0



// Event Reader header files
// This file is included by Offline programs

class EventReader;

// Support Structures

// Information regarding the event
struct EventInfo // return from EventReader::getEventInfo()
{
  int EventLength;
  unsigned int UnixTime;
  unsigned int EventSeqNo;
  unsigned int TrigWord;
  unsigned int TrigInputWord;
  unsigned char TPCPresent;
  unsigned char SVTPresent;
  unsigned char TOFPresent;
  unsigned char EMCPresent;
  unsigned char SMDPresent;
  unsigned char FTPCPresent;
  unsigned char Reserved;
  unsigned char RICHPresent;
  unsigned char TRGDetectorsPresent;
  unsigned char L3Present;
};

// Each sequence contains one hit (zero suppressed data)
struct Sequence
{
  u_short startTimeBin;
  u_short Length;
  u_char *FirstAdc;
};

// Each pad contains an array of hits for that pad (zero suppressed data)
struct Pad
{
  u_char nseq;
  Sequence *seq;
};

// A pad row contains an array of pads (zero suppressed data)
struct PadRow
{
  u_short npads;
  Pad *pad;
};


// Gain structure
struct Gain
{
  int t0;          // t0 * 16
  int t0_rms;      // t0_rms * 16
  int rel_gain;    // rel_gain * 64
};

struct ASIC_Cluster
{
  short start_time_bin;
  short stop_time_bin;
};


struct Centroids {
  unsigned short x; // units: 1/64 pad 
  unsigned short t; // units: 1/64 timebin
}; 

struct SpacePt {
  Centroids centroids;
  unsigned short flags;
  unsigned short q;
};



// The sector reader virtual classes
class ZeroSuppressedReader
{
public:
  virtual int getPadList(int PadRow, u_char **padList)=0;
      // Fills (*padList[]) with the list of pad numbers containing hits
      // returns number of pads in (*padList)[]
      // or negative if call fails

  virtual int getSequences(int PadRow, int Pad, int *nSeq,
			   Sequence **SeqData)=0;
      //  Fills (*SeqData)[] along with the ADC
      // buffers pointed to by (*SeqData)[]
      // Set nSeq to the # of elements in the (*SeqData)[] array
      // returns 0 if OK.
      // or negative if call fails

// Read the clusters (space points) found in the mezzanine cluster-finder
  virtual int getSpacePts(int PadRow, int *nSpacePts, SpacePt **SpacePts)=0;
      // Fills (*SpacePts)[] along with the 
      // buffers pointed to by (*SpacePts)[]
      // Set nSpacePts to the # of elements in the (*SpacePts)[] array
      // returns 0 if OK.
      // or negative if call fails

  virtual int MemUsed()=0;
  virtual ~ZeroSuppressedReader() {};
};

// Reads Raw ADC values
class ADCRawReader
{
public:
  virtual int getPadList(int PadRow, unsigned char **padList)=0;
	// As for Zero suppressed data, this returns
 	// the list of pads for which data can be obtained
	// Therefore, the padList will always contain all of the
	// pads in the specified PadRow regardless of the data
	
  virtual int getSequences(int PadRow, int Pad, int *nArray, u_char **Array)=0;
	// Fills (*Array)[] with Raw data
	// Fills nArray with the # of elements in (*Array)[] (512 bytes / TPC)
	// returns 0 if OK.
	// returns negative if call fails

  virtual int MemUsed()=0;
  virtual ~ADCRawReader() {};
};

// Reads the Pedestal values
class PedestalReader
{
public:
  virtual int getPadList(int PadRow, unsigned char **padList)=0;
	// As for Zero suppressed data, this returns
 	// the list of pads for which data can be obtained
	// Therefore, the padList will always contain all of the
	// pads in the specified PadRow regardless of the data
	
  virtual int getSequences(int PadRow, int Pad, int *nArray, u_char **Array)=0;
	// Fills (*Array)[] with Pedestal data
	// Fills nArray with the # of elements in Array (512 bytes for TPC)
	// returns 0 if OK.
	// returns negative if call fails

  virtual int getNumberOfEvents()=0;
 	// returns the number of events the pedestal run based on

  virtual int MemUsed()=0;
  virtual ~PedestalReader() {};
};

// The RMS pedestal values
class PedestalRMSReader
{
public:
  virtual int getPadList(int PadRow, u_char **padList)=0;
	// As for Zero suppressed data, this returns
 	// the list of pads for which data can be obtained
	// Therefore, the (*padList)[] will always contain all of the
	// pads in the specified PadRow regardless of the data
	
  virtual int getSequences(int PadRow, int Pad, int *nArray, u_char **Array)=0;
	// Fills (*Array)[] with Pedestal RMS data * 16
	// Fills nArray with the # of elements in (*Array)[] (512 bytes / TPC)
	// returns 0 if OK.
	// returns negative if call fails

  virtual int getNumberOfEvents()=0;
 	// returns the number of events the pedestal run based on

  virtual int MemUsed()=0;
  virtual ~PedestalRMSReader() {};
};

// The gain reader
class GainReader
{
public:
  virtual int getGain(int PadRow, int Pad, struct Gain **gain)=0;
	// sets (*gain) to a valid gain structure pointer
	// returns 0 if OK
	// returns negative if call fails

  virtual int getMeanGain()=0;
      // returns mean gain

  virtual int getNumberOfEvents()=0;
	// returns the number of events the calculation is based upon

  virtual int MemUsed()=0;
  virtual ~GainReader() {};
};

// Reads Cluster Pointer Pairs from the ASIC
class CPPReader
{
public:
  virtual int getClusters(int PadRow, int Pad, int *nClusters, 
			  struct ASIC_Cluster **clusters)=0;
	// sets (*clusters) to beginning of array of clusters
	// sets nClusters to the length of the array
	// returns 0 if OK
	// returns negative if call fails

  virtual int getAsicParams(ASIC_params *)=0;

  virtual int MemUsed()=0;
  virtual ~CPPReader() {};
};

// Reads the bad channels
class BadChannelReader
{
public:
  virtual int IsBad(int PadRow, int Pad)=0;
	// returns true if the pad is bad.  
	// returns false if the pad is not bad.
	
  virtual int MemUsed()=0;
  virtual ~BadChannelReader() {};
};

// Read the front end electronics configuration
class ConfigReader
{
public:
  virtual int FEE_id(int PadRow, int Pad) = 0;
	// returns FEE_id

  virtual int MemUsed()=0;
  virtual ~ConfigReader() {};
};


// Detector Reader Virtual Class
class DetectorReader
{
  friend class EventReader;

public:
  virtual ZeroSuppressedReader *getZeroSuppressedReader(int sector)=0;
  virtual ADCRawReader *getADCRawReader(int sector)=0;
  virtual PedestalReader *getPedestalReader(int sector)=0;
  virtual PedestalRMSReader *getPedestalRMSReader(int sector)=0;
  virtual GainReader *getGainReader(int sector)=0;
  virtual CPPReader *getCPPReader(int sector)=0;
  virtual BadChannelReader *getBadChannelReader(int sector)=0;

  virtual ~DetectorReader() { };

  virtual int MemUsed()=0;

  int errorNo() { return errnum; };
  string errstr() { return string(errstr0); };

protected:

  // Buffer and index functions for the various readers.
  // Initially these will do nothing.  Add functionality 
  // to increase performance
  virtual int InformBuffers(ZeroSuppressedReader *, int sector)=0;
  virtual int InformBuffers(ADCRawReader *,int sector)=0;
  virtual int InformBuffers(PedestalReader *,int sector)=0;
  virtual int InformBuffers(PedestalRMSReader *,int sector)=0;
  virtual int InformBuffers(GainReader *,int sector)=0;
  virtual int InformBuffers(CPPReader *,int sector)=0;
  virtual int InformBuffers(BadChannelReader *,int sector)=0;
  virtual int InformBuffers(ConfigReader *,int sector)=0;

  virtual int AttachBuffers(ZeroSuppressedReader *, int sector)=0;
  virtual int AttachBuffers(ADCRawReader *, int sector)=0;
  virtual int AttachBuffers(PedestalReader *, int sector)=0;
  virtual int AttachBuffers(PedestalRMSReader *, int sector)=0;
  virtual int AttachBuffers(GainReader *, int sector)=0;
  virtual int AttachBuffers(CPPReader *, int sector)=0;
  virtual int AttachBuffers(BadChannelReader *, int sector)=0;
  virtual int AttachBuffers(ConfigReader *, int sector)=0;

  int errnum;
  char errstr0[250];

private:
  EventReader *er;
};

// Event Reader Class
class EventReader
{
public:
  EventReader();
  EventReader(const char *logfilename);

  void InitEventReader(int fd, long offset, int mmap);  
                             // takes open file descripter-offset
                             // works on MAPPED file
  void InitEventReader(int fd, long offset);  
                             // takes open file descripter-offset
                             // works on file
  void InitEventReader(void *event);           // pointer to the event
      //  There is an ambiguity here.  The specifier may point to
      //  A logical record header, or it may point to a DATAP Bank
      //  This ambiguity must be resolved by these functions before
      //  They store the DATAP pointer

  long NextEventOffset();
  void setVerbose(int); // 0 turns off all internal printout
  char * findBank(char *bankid); // navigates to pointer bnk below DATAP
  int verbose;

  ~EventReader();

  char *getDATAP() { return DATAP; };
  struct EventInfo getEventInfo();
  void printEventInfo();
  void printEventInfo(FILE *);
  void fprintError(int err, char *file, int line, char *userstring);

  int runno() { return runnum; }
  int errorNo() { return errnum; };
  string errstr() { return string(errstr0); };
  FILE *logfd; //file handle for log file
  char err_string[MX_MESSAGE][30];

  int MemUsed();              

protected:
  char *DATAP;             // Pointer to the memory mapped buffer
  int event_size;

  // Detector Buffering Functions
  int InformBuffers(DetectorReader *) { return FALSE; };
        // returns false.  
        // later will be used to give EventReader a detectors buffers
  int AttachBuffers(DetectorReader *) { return FALSE; };
        // returns false.
        // later will be used to give buffers back to DetectorReader

private:
  int fd;            // -1 if the event is in memory
  char *MMAPP;        // Begining of memory mapping
  
  long next_event_offset;

  int errnum;
  char errstr0[250];
  int runnum;
  // later storage for detector buffers
};

//#include "../RICH/RICH_Reader.hh"

// Declaration for the factories
DetectorReader *getDetectorReader(EventReader *, string);
EventReader *getEventReader(int fd, long offset, int MMap=1);
EventReader *getEventReader(int fd, long offset, const char *logfile, int MMap=1);
EventReader *getEventReader(char *event);
// declared in RICH_Reader.hh
// RICH_Reader *getRichReader(EventReader *er);



#endif
