#include "ace/Task.h"

class CCapTask : public ACE_Task<ACE_MT_SYNCH>
{
public:
  CCapTask();
  ~CCapTask();

  virtual int svc(void); 
};

CCapTask::CCapTask()
{
}

CCapTask::~CCapTask()
{
}

int CCapTask::svc(void)
{
  ACE_DEBUG((LM_DEBUG,"(%t) svc end\n"));
  return 0;
}

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
  CCapTask cap;
  cap.activate(); 
  
  cap.wait();
  ACE_DEBUG((LM_DEBUG,"(%t) end of main\n"));
  return 0;
}
