#include "CCapTask.h"
#include "CNullTask.h"
#include "ace/OS_NS_unistd.h"
#include <iostream>

ACE_THR_FUNC_RETURN usrInp(void *p)
{
  ACE_DEBUG((LM_DEBUG,"(%t) start of usrInp\n"));
  CCapTask* pCapTask = static_cast<CCapTask*>(p);
  ACE_ASSERT(pCapTask);

  char buf[256];
  std::cin>>buf;
  ACE_DEBUG((LM_DEBUG,"(%t) stopping...\n"));

  pCapTask->stop();
  ACE_DEBUG((LM_DEBUG,"(%t) end of usrInp\n"));
  return 0;
}

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
  ACE_DEBUG((LM_DEBUG,"(%t) start of main\n"));

  CCapTask cap;
  CNullTask nt;

  cap.m_pQ = nt.msg_queue();

  cap.start(); 
  nt.activate();

  ACE_thread_t tId;
  ACE_Thread_Manager::instance()->spawn(usrInp,(void*)&cap,THR_NEW_LWP|THR_JOINABLE,&tId);

  cap.wait();

  ACE_DEBUG((LM_DEBUG,"(%t) end of main\n"));
  return 0;
}
