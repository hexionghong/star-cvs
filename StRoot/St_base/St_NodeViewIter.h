//*-- Author :    Valery Fine(fine@bnl.gov)   25/01/99  
//  
// 

#ifndef STAR_St_NodeViewIter
#define STAR_St_NodeViewIter

#include "St_DataSetIter.h" 
#include "St_NodePosition.h"

class TClonesArray;
class St_NodeView;

class St_NodeViewIter : public St_DataSetIter {
private:
protected:
     TClonesArray  *m_Positions; // the array of the Node position in the absolute system 
public:
     St_NodeViewIter(St_NodeView *view, Int_t depth=1, Bool_t dir=kIterForward);
    ~St_NodeViewIter();
     virtual St_NodePosition *GetPosition(Int_t level=0);
     virtual void             Notify(St_DataSet *set);
     virtual void             Reset(St_DataSet *l=0,Int_t depth=0);
     St_NodePosition          operator[](Int_t level=0);
     St_NodePosition          *UpdateTempMatrix(St_NodePosition *curPosition);
     ClassDef(St_NodeViewIter,0)
};
#endif 

