//StiKTNVerticalIterator.h
//M.L. Miller (Yale Software)
//12/01

/*! \class StiKTNVerticalIterator
  This class is an STL compliant forward iterator that will traverse from
  the leaf of a tree upward to a root.

  \author M.L. Miller (Yale Software)
  \note We use the defualt copy/assignment generated by compiler.
  \note Singularity (i.e., 'end') is represented by setting mNode=0.
  \note StiKTNVertIterator is a non-virtual class.
*/

#ifndef StiKTNVerticalIterator_HH
#define StiKTNVerticalIterator_HH

#include <iterator>
using namespace std;

#include "StiKalmanTrackNode.h"
typedef StiKalmanTrackNode KTN_t;

//This is a temp hack to get around old gcc ansi-non-compliant STL implementation
class StiKTNVertIterator
#ifndef GNU_GCC
    : public iterator<forward_iterator_tag, KTN_t, ptrdiff_t, KTN_t*, KTN_t&>
#else
    : public forward_iterator<KTN_t, int>
#endif
{
public:
    
public:
    ///ctr-dstr
    StiKTNVertIterator() : mNode(0) {};
    StiKTNVertIterator(StiKalmanTrackNode* leaf) : mNode(leaf) {};
    StiKTNVertIterator(StiKalmanTrackNode& leaf) : mNode(&leaf) {};
    ~StiKTNVertIterator() {};

    ///equality:
    bool operator==(const StiKTNVertIterator& rhs);

    ///inequlity
    bool operator!=(const StiKTNVertIterator& rhs);

    ////Dereference
    StiKalmanTrackNode& operator*();
    
    ///prefix
    StiKTNVertIterator& operator++ ();
    
    ///postfix
    StiKTNVertIterator operator++(int);

    ///We demarcate the end of the traversal via  a singular iterator
    StiKTNVertIterator end();

private:
    StiKalmanTrackNode* mNode;
};

//inlines --

inline bool StiKTNVertIterator::operator==(const StiKTNVertIterator& rhs)
{
    return mNode==rhs.mNode;
}

inline bool StiKTNVertIterator::operator!=(const StiKTNVertIterator& rhs)
{
    return !(mNode==rhs.mNode);
}

inline StiKalmanTrackNode& StiKTNVertIterator::operator*()
{
    return *mNode;
}

//prefix
/*! In the case where the prefix operator increments beyond the root of the tree,
  the pointer to mNode is set to 0.   This demarcates the end of the traversal.
 */
StiKTNVertIterator& StiKTNVertIterator::operator++ ()
{
    if (mNode->isRoot() ) {
	mNode=0;
    }
    else {
	mNode = static_cast<StiKalmanTrackNode*>(mNode->getParent());
    }
    return *this;
}
    

//postfix
/*! In the case where the prefix operator increments beyond the root of the tree,
  the pointer to mNode is set to 0.   This demarcates the end of the traversal.
*/
StiKTNVertIterator StiKTNVertIterator::operator++(int)
{
    StiKTNVertIterator temp = *this;
    ++(*this);
    return temp;
}

StiKTNVertIterator StiKTNVertIterator::end()
{
    return StiKTNVertIterator(0);
}

#endif
