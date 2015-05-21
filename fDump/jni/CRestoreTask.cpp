#include "CRestoreTask.h"

int CRestoreTask::svc(void)
{
	ACE_DEBUG((LM_DEBUG, "(%t) CRestoreTask::svc start\n"));

	ACE_Message_Block *message;
	for (;;){
		if (this->getq(message) == -1) ACE_ERROR_RETURN((LM_ERROR, "%p\n", "getq"), -1);

		if (message->msg_type() == ACE_Message_Block::MB_HANGUP) {
			message->release();
			ACE_DEBUG((LM_DEBUG, "(%t) CRestoreTask::svc MB_HANGUP received\n"));
			break;
		}

		ACE_DEBUG((LM_DEBUG,"{%d:%d}\n",message->size(),this->msg_queue()->message_count()));

		message->release();
	}
	ACE_DEBUG((LM_DEBUG, "(%t) CRestoreTask::svc end\n"));
	return 0;
}