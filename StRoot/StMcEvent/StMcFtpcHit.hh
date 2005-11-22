/***************************************************************************
 *
 * $Id: StMcFtpcHit.hh,v 2.11 2005/11/22 21:44:51 fisyak Exp $
 * $Log: StMcFtpcHit.hh,v $
 * Revision 2.11  2005/11/22 21:44:51  fisyak
 * Add compress Print for McEvent, add Ssd collections
 *
 * Revision 2.10  2005/09/28 21:30:14  fisyak
 * Persistent StMcEvent
 *
 * Revision 2.9  2005/07/06 20:05:28  calderon
 * Remove forward declaration of StThreeVectorF, use #include, and only in
 * StMcHit base class.  StThreeVectorF is not a class anymore, it is now
 * only a typedef, only template version of StThreeVector exists now.
 *
 * Revision 2.8  2005/01/27 23:40:47  calderon
 * Adding persistency to StMcEvent as a step for Virtual MonteCarlo.
 *
 * Revision 2.7  2003/10/08 20:17:55  calderon
 * -using <iostream>, std::cout, std::ostream.
 * -changes in FTPC volume Id.
 *   o Causes changes in decoding of plane().
 *   o sector() is added.
 *   o print volumeId and sector() in the operator<<.
 *
 * Revision 2.6  2000/06/06 02:58:41  calderon
 * Introduction of Calorimeter classes.  Modified several classes
 * accordingly.
 *
 * Revision 2.5  2000/05/05 15:25:43  calderon
 * Reduced dependencies and made constructors more efficient
 *
 * Revision 2.4  2000/01/18 20:52:31  calderon
 * Works with CC5
 *
 * Revision 2.3  1999/12/15 20:05:48  calderon
 * corrected the comment on the numbering of the plane
 *
 * Revision 2.2  1999/12/03 00:51:52  calderon
 * Tested with new StMcEventMaker.  Added messages for
 * diagnostics.
 *
 * Revision 2.1  1999/11/19 19:06:32  calderon
 * Recommit after redoing the files.
 *
 * Revision 2.0  1999/11/17 02:12:16  calderon
 * Completely revised for new StEvent
 *
 * Revision 1.4  1999/09/24 01:23:16  fisyak
 * Reduced Include Path
 *
 * Revision 1.3  1999/09/23 21:25:51  calderon
 * Added Log & Id
 * Modified includes according to Yuri
 *
 *
 **************************************************************************/
#ifndef StMcFtpcHit_hh
#define StMcFtpcHit_hh

#include "StMcHit.hh"
#include "StMemoryPool.hh"

class StMcTrack;
class g2t_ftp_hit_st;

#if !defined(ST_NO_NAMESPACES)
#endif

class StMcFtpcHit : public StMcHit {
public:
    StMcFtpcHit();
    StMcFtpcHit(const StThreeVectorF&,const StThreeVectorF&,
	     const float, const float, const long, const long, StMcTrack*);
    StMcFtpcHit(g2t_ftp_hit_st*);
    ~StMcFtpcHit();
#ifdef POOL
    void* operator new(size_t)     { return mPool.alloc(); }
    void  operator delete(void* p) { mPool.free(p); }
#endif
    unsigned long plane() const; // 1-20, where 1-10 = West and 11-20 = East
    unsigned long sector() const; // 1-6
  virtual void Print(Option_t *option="") const; // *MENU* 
   
private:
#ifdef POOL
    static StMemoryPool mPool; 
#endif
    ClassDef(StMcFtpcHit,1)
};

ostream&  operator<<(ostream& os, const StMcFtpcHit&);


#endif
