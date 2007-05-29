//class  StGammaPythiaMaker
//author R.Fatemi
//date   2007/May/24 
//This class allows access to the pythia record and the isolatation of prompt and decay photons. In addition, this code provides 
//the calculation of "asymmetries" in MC data by using partonic kinematic information from the pythia record along with polarized 
//and unpolarized pdfs and LO partonic asymmetries. Documentation on the Method of Asymmetry Weights (MAW) can be found in the 
//preprint on SPHINX (hep-0005320)

#ifndef STAR_StGammaPythiaMaker
#define STAR_StGammaPythiaMaker
                                                                  
#ifndef StMaker_H
#include "StMaker.h"
#include "TLorentzVector.h"
#include "StMessMgr.h"
#include <vector>

extern "C" void polar_(int*,double*,double*,double*,int*);
extern "C" Double_t ctq5pd_(int*,int*,double*,double*,int*);
extern "C" void num_(double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*);
extern "C" void denom_(double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*);
#endif

class StChain;
class StMuDstMaker;
class StMuEvent;
class StEventInfo;
class St_particle;
class St_g2t_event;
class St_g2t_pythia;

class StGammaPythiaMaker : public StMaker
{

 private:

 protected:

 public: 
  StGammaPythiaMaker(const char *name="GammaPythia");
  virtual  ~StGammaPythiaMaker();
  virtual Int_t Init();
  virtual Int_t  Make();
  void Zero();

  //pointers to makers
  StMuDstMaker *muDstMaker;
  StMuEvent *muEvent;
  St_particle *particleTabPtr;
  St_g2t_event *Pg2t_event;
  St_g2t_pythia *Pg2t_pythia;

  int geantPID;                        //sub process id from GEANT table
  int geantID;                         //event number from GEANT table
  int evtid;                           //event number from MuDst
  int pid;                             //subprocess id 
  Double_t parton1[11],parton2[11];    //pythia record entry of scattered partons
  int flavor1,flavor2,flavor3,flavor4; //flavor of scattered partons before/after hard interaction
  float s,t,u,hard_p,cos_th,x1,x2;     //partonic kinematics
  int pol_id_flag, unpol_id_flag;      //flags for *.F 
  double partonic_all,Q2;              //partonic a_LL from LO calculations and hard scale

  vector<TLorentzVector> Prompt4Mom;
  vector<TLorentzVector> Decay4Mom;
  TLorentzVector prompt;
  TLorentzVector decay;

  double df1_LO,df2_LO,f1_LO,f2_LO,weight_LO;         //Leading Order polarized pdf, unpolarized pdf and weight = df1*df2*partonic_all/f1/f2
  double df1_NLO,df2_NLO,f1_NLO,f2_NLO,weight_NLO;    //NLO STD polarized pdf, unpolarized pdf and weight = df1*df2*partonic_all/f1/f2
  double df1_NLO_g0,df2_NLO_g0,weight_NLO_g0;         //NLO G0  polarized pdf, unpolarized pdf and weight = df1*df2*partonic_all/f1/f2
  double df1_NLO_gmax,df2_NLO_gmax,weight_NLO_gmax;   //NLO GMAX polarized pdf, unpolarized pdf and weight = df1*df2*partonic_all/f1/f2
  double df1_NLO_gmin,df2_NLO_gmin,weight_NLO_gmin;   //NLO GMIN polarized pdf, unpolarized pdf and weight = df1*df2*partonic_all/f1/f2


  Double_t get_polPDF_LO(int x1, double d1, double d2);
  Double_t get_polPDF_NLO(int x1, double d1, double d2);
  Double_t get_polPDF_NLO_g0(int x1, double d1, double d2);
  Double_t get_polPDF_NLO_gmax(int x1, double d1, double d2);
  Double_t get_polPDF_NLO_gmin(int x1, double d1, double d2);
  
  Double_t get_unpolPDF_LO(int x1, double d1, double d2); 
  Double_t get_unpolPDF_NLO(int x1, double d1, double d2);

  Double_t getPartonicALL(double a, double b, double c, int d, int e, int f, int g, int h);
  
  virtual const char *GetCVS() const {
    static const char cvs[]="Tag $Name:  $ $Id: StGammaPythiaMaker.h,v 1.1 2007/05/29 14:22:48 rfatemi Exp $ built "__DATE__" "__TIME__ ; 
    return cvs;
  }  
  
  ClassDef(StGammaPythiaMaker,0)   //StAF chain virtual base class for Makers
};

#endif


