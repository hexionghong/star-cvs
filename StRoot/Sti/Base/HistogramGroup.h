#ifndef HistogramGroup_H_Included 
#define HistogramGroup_H_Included 
#include <vector>
#include <string>
using namespace std;
#include "Sti/Base/Named.h"
#include "Sti/Base/Described.h"
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TFile.h>
#include <TString.h>

class HistogramGroup : public Named, public Described, public vector<TH1*>
{
 public: 
  HistogramGroup();
  HistogramGroup(const string & name, const string & description);
  virtual ~HistogramGroup();

  TH1* hist(const TString  & nme);

  virtual void write(TFile * file);
  virtual void write(const string & fileName, const string &option="RECREATE");  
  virtual void write();
  virtual void reset();
  TH1 * add(TH1*histo);
  TH1D * book(const string &title, 
			      const string & description, 
			      int n, 
			      double xMin,
			      double xMax);
  TH2D * book(const string &title, 
			      const string & description, 
			      int nx, 
			      double xMin,
			      double xMax,
			      int ny, 
			      double yMin,
			      double yMax);

  TH3D * book(const string &title, 
			      const string & description, 
			      int nx, 
			      double xMin,
			      double xMax,
			      int ny, 
			      double yMin,
			      double yMax,
			      int nz, 
			      double zMin,
			      double zMax);

  TProfile * bookProfile(const string &title, 
			      const string & description, 
			      int n, 
			      double xMin,
			      double xMax);
  TProfile2D * bookProfile(const string &title, 
					   const string & description, 
					   int nx, 
					   double xMin,
					   double xMax,
					   int ny, 
					   double yMin,
					   double yMax);
  
};

inline TH1 * HistogramGroup::add(TH1*histo)
{
  push_back(histo);
  return histo;
}
#endif
