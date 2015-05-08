#include "CCapTask.h"

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
  ACE_DEBUG((LM_DEBUG,"(%t) start of main\n"));

  CCapTask cap;
  cap.activate(); 

  cap.wait();
  ACE_DEBUG((LM_DEBUG,"(%t) end of main\n"));
  return 0;
}
