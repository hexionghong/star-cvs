// $Id: StarStack.h,v 1.1 2004/07/12 20:35:59 potekhin Exp $
//

#ifndef STARSTACK_H
#define STARSTACK_H


// Warning this material comes from Ex02 and is not adapted from Ali

#include "StarParticle.h"
#include <TVirtualMCStack.h>

#include <stack>

class StarStack : public TVirtualMCStack
{
  public:
    StarStack(Int_t size);
    StarStack();
    virtual ~StarStack();     

    // methods
    virtual void  PushTrack(Int_t toBeDone, Int_t parent, Int_t pdg,
  	              Double_t px, Double_t py, Double_t pz, Double_t e,
  		      Double_t vx, Double_t vy, Double_t vz, Double_t tof,
		      Double_t polx, Double_t poly, Double_t polz,
		      TMCProcess mech, Int_t& ntr, Double_t weight,
		      Int_t is) ;
    virtual TParticle* PopNextTrack(Int_t& track);
    virtual TParticle* PopPrimaryForTracking(Int_t i); 
    void Print() const;   
    void Reset();   
   
    // set methods
    virtual void  SetCurrentTrack(Int_t track);                           

    // get methods
    virtual Int_t  GetNtrack() const;
    virtual Int_t  GetNprimary() const;
    virtual TParticle* GetCurrentTrack() const;    
    virtual Int_t  GetCurrentTrackNumber() const;
    virtual Int_t  GetCurrentParentTrackNumber() const;
    StarParticle*  GetParticle(Int_t id) const;
    
  private:
    // data members
    std::stack<StarParticle*>  fStack;    //!
    TObjArray*                 fParticles;
    Int_t                      fCurrentTrack;
    Int_t                      fNPrimary;
    
    ClassDef(StarStack,1) // StarStack
};

#endif //STARSTACK_H   
   

