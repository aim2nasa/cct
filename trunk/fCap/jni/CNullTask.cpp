#include "CNullTask.h"
#include "ace/OS_NS_time.h"

CNullTask::CNullTask()
{
}

CNullTask::~CNullTask()
{
}

int CNullTask::svc(void)
{
  ACE_DEBUG((LM_DEBUG,"(%t) svc start\n"));

  ACE_Message_Block *message;
  for(;;){
    if(this->getq(message)==-1) ACE_ERROR_RETURN((LM_ERROR,"%p\n","getq"),-1);

    if(message->msg_type()==ACE_Message_Block::MB_HANGUP) {
      message->release();
      ACE_DEBUG((LM_DEBUG,"(%t) MB_HANGUP received\n"));
      break;
    }

    ACE_DEBUG((LM_DEBUG,"\n[%d]\n",message->size()));

    message->release();
    //ACE_DEBUG((LM_DEBUG,"(%t) Q(%d)\n",this->msg_queue()->message_count()));
  }
  ACE_DEBUG((LM_DEBUG,"(%t) svc end\n"));
  return 0;
}
