//*-- Author :    Valery Fine(fine@bnl.gov)   27/10/2006
//
// $Id: StCheckQtEnv.h,v 1.2 2006/11/17 15:13:48 fine Exp $
// This class  sets the Qt/Root environment "on fly" and 
// generates the correct ROOT resource ".rootrc" file 
// also
#ifndef STAR_StCheckQtEnv
#define STAR_StCheckQtEnv

#include <stdio.h>
#include "TString.h"

class StCheckQtEnv {

public:
    static FILE *OpeFileName(const char *fileNamePrototype);
    static TString GetNewFileName(const char *fileNamePrototype);
    static Int_t SetRootResource(FILE *file, const char *plugin, 
                       const char *lib,
                       const char *full=0,Bool_t append=kFALSE); 

    static Long_t SetQtEnv();
};
#endif
