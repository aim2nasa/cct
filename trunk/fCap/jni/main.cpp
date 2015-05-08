#include "CCapTask.h"

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
  CCapTask cap;
  cap.activate(); 
  
  cap.wait();
  ACE_DEBUG((LM_DEBUG,"(%t) end of main\n"));
  return 0;
}
