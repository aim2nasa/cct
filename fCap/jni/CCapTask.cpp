#include "CCapTask.h"

CCapTask::CCapTask()
{
}

CCapTask::~CCapTask()
{
}

int CCapTask::svc(void)
{
  ACE_DEBUG((LM_DEBUG,"(%t) svc start\n"));

  ACE_Message_Block* message;
  for(;;){
    if(this->getq(message)==-1) ACE_ERROR_RETURN((LM_ERROR,"%p\n","getq"),-1);

    if(message->msg_type()==ACE_Message_Block::MB_HANGUP) {
      message->release();
      break;
    }
  }
  ACE_DEBUG((LM_DEBUG,"(%t) svc end\n"));
  return 0;
}
