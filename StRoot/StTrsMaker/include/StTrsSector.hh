/***************************************************************************
 *
 * $Id: StTrsSector.hh,v 1.5 2000/06/23 00:12:24 snelling Exp $
 *
 * Author: bl prelim
 ***************************************************************************
 *
 * Description: Keeps the analog (and later) digital information
 *              in the sector's time bins
 *
 ***************************************************************************
 *
 * $Log: StTrsSector.hh,v $
 * Revision 1.5  2000/06/23 00:12:24  snelling
 * Removed dependence on local files now pointed to StDbUtilities
 *
 * Revision 1.4  2000/01/10 23:11:32  lasiuk
 * Include MACROS for compatibility with SUN CC5.0
 *
 * Revision 1.3  1999/11/11 19:45:02  calderon
 * Made variables-> data members in analog signal generator to avoid
 * initialization time when member functions are called.
 * Inlined:
 *  StTrsParameterizedAnalogSignalGenerator::signalSampler()
 *  StTrsSector::addEntry()
 *  StTrsSector::assignTimeBins()
 *
 * Revision 1.2  1999/01/18 21:03:32  lasiuk
 * Jan 18,1999
 *
 * Revision 1.1  1998/11/10 17:12:12  fisyak
 * Put Brian trs versin into StRoot
 *
 * Revision 1.3  1998/11/08 17:06:33  lasiuk
 * use resize() for LINUX compatibiltiy
 * allocator specification for vector<>
 *
 * Revision 1.2  1998/11/04 18:49:26  lasiuk
 * modify constructors
 * macro ST_SECTOR_BOUND_CHECK
 * addEntry()
 * addTimeBins()
 *
 * Revision 1.1  1998/06/30 22:54:10  lasiuk
 * Initial Revision
 *
 **************************************************************************/
#ifndef ST_TRS_SECTOR_HH
#define ST_TRS_SECTOR_HH

#include <vector>
#if defined (__SUNPRO_CC) && __SUNPRO_CC >= 0x500
using std::vector;
#endif
#include "StTrsAnalogSignal.hh"
#include "StTpcGeometry.hh"

#include "StDbUtilities/StTpcPadCoordinate.hh"

#ifndef ST_NO_TEMPLATE_DEF_ARGS
typedef vector<StTrsAnalogSignal>  tpcTimeBins;
typedef vector<tpcTimeBins>        tpcPadRow;
typedef vector<tpcPadRow>          tpcSector;

typedef vector<StTrsAnalogSignal>::iterator timeBinIterator;
#else
typedef vector<StTrsAnalogSignal, allocator<StTrsAnalogSignal> > tpcTimeBins;
typedef vector<tpcTimeBins, allocator<tpcTimeBins> >             tpcPadRow;
typedef vector<tpcPadRow, allocator<tpcPadRow> >                 tpcSector;

typedef vector<StTrsAnalogSignal, allocator<StTrsAnalogSignal> >::iterator timeBinIterator;
#endif

typedef tpcPadRow::iterator                 padRowIterator;
typedef tpcSector::iterator                 rowIterator;

class StTrsSector {
public:
    StTrsSector(StTpcGeometry*);
    ~StTrsSector();
    
    //StTrsSector(const StTrsSector&);
    //StTrsSector& operator=(const StTrsSector&);

    // access functions
    tpcTimeBins&   timeBinsOfRowAndPad(int, int);
    tpcPadRow&     padsOfRow(int);
    tpcSector&     rows();

    int  size()                 const;
    int  numberOfRows()         const;
    int  numberOfPadsInRow(int) const;
    
    // Adding
    void clear();
    void addEntry(StTpcPadCoordinate&, StTrsAnalogSignal&);
    void addEntry(int, int, StTrsAnalogSignal&);            // row,pad

    void assignTimeBins(int, int, tpcTimeBins&);
    void assignTimeBins(StTpcPadCoordinate&, tpcTimeBins&);

private:
    tpcSector mSector;
};
inline tpcTimeBins& StTrsSector::timeBinsOfRowAndPad(int rowN, int padN) { return (mSector[(rowN-1)][(padN-1)]); }
inline tpcPadRow&   StTrsSector::padsOfRow(int rowN) { return (mSector[(rowN-1)]); }
inline tpcSector&   StTrsSector::rows() { return (mSector); }
inline int          StTrsSector::size() const { return mSector.size();}
inline int          StTrsSector::numberOfRows() const { return mSector.size();}
inline int          StTrsSector::numberOfPadsInRow(int rowN) const { return mSector[(rowN-1)].size();}
inline void StTrsSector::addEntry(int rowN, int padN, StTrsAnalogSignal& signl)
{
    
#ifdef ST_SECTOR_BOUNDS_CHECK
    if( (rowN > 0 && row <= mSector.size()) )
	if( (padN > 0 && pad <= mSector[(rowN-1)].size()) )
#endif
	    mSector[(rowN-1)][(padN-1)].push_back(signl);
}
inline void StTrsSector::assignTimeBins(int rowN, int padN, tpcTimeBins& tbins)
{
#ifdef ST_SECTOR_BOUNDS_CHECK
    if( (rowIndex > 0 && rowIndex <= mSector.size()) )
	if( (padIndex > 0 && padIndex <= mSector[rowIndex].size()) )
#endif
	    mSector[(rowN-1)][(padN-1)] = tbins;
}

#endif
