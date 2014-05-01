#include "RunStatus.h"
#include <rtsLog.h>

ClassImp(RunStatus) ;

RunStatus::RunStatus()
{
  run = 0;
  status = NULL; 
  setStatus("unknown");
}

RunStatus::~RunStatus()
{
  if(status) delete status;
}

void RunStatus::setStatus(const char *s)
{
  if(status) {
    if(strcmp(status, s) == 0) {
      return;
    }
  }

  if(status) delete status;
  status = new char[strlen(s)+1];
  strcpy(status, s);
  timeOfLastChange = time(NULL);
}

void RunStatus::dump()
{
  LOG("JEFF","Run Status----------------");
  LOG("JEFF", "run=%d",run);
  LOG("JEFF", "status=%s",status);
  LOG("JEFF", "--------------------------");
}

int RunStatus::running()
{
  if(status == NULL) return 0;

  if(strcmp(status, "running") == 0) return 1;
  return 0;
}

int RunStatus::getNumericStatus(char *str)
{
  static char *statname[5] = { (char *)"unknown", (char *)"starting", (char *)"running", (char *)"stopping", (char *)"stopped" };

  for(int i=0;i<5;i++) {
    if(strcmp(str, statname[i]) == 0) return i;
  }
  return 0;
}

