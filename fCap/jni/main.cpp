#include "CCapTask.h"
#include "CZcTask.h"
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
	ACE_DEBUG((LM_DEBUG,"(%t) press any key and [enter] to exit\n"));

	CCapTask cap;
	CZcTask zc;
	CNullTask nt;

	cap.m_pQ = zc.msg_queue(); //captured blocks are send to zc task thread
	zc.m_pQ = nt.msg_queue();  //compressed blocks are send to null task thread which does nothing

	nt.activate();
	zc.activate();
	cap.start(); 

	ACE_thread_t tId;
	ACE_Thread_Manager::instance()->spawn(usrInp,(void*)&cap,THR_NEW_LWP|THR_JOINABLE,&tId);

	cap.wait();
	nt.wait();
	zc.wait();

	ACE_DEBUG((LM_DEBUG,"(%t) end of main\n"));
	return 0;
}
