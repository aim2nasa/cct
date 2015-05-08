#include "CCapTask.h"

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
