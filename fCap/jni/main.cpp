#include "CCapTask.h"
#include "ace/OS_NS_unistd.h"

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
  ACE_DEBUG((LM_DEBUG,"(%t) start of main\n"));

  CCapTask cap;
  cap.activate(); 

  ACE_Message_Block* hangup;
  ACE_NEW_RETURN(hangup,ACE_Message_Block(0,ACE_Message_Block::MB_HANGUP),-1);

  ACE_OS::sleep(5); //for 5 seconds

  cap.putq(hangup);
  cap.wait();
  ACE_DEBUG((LM_DEBUG,"(%t) end of main\n"));
  return 0;
}
