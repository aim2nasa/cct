#include "CRestoreTask.h"
#include "utility.h"
#include "fMacro.h"

int CRestoreTask::svc(void)
{
	ACE_DEBUG((LM_DEBUG, "(%t) CRestoreTask::svc start\n"));

	ACE_TCHAR pcTime[TIMESTAMP_SIZE];	//pc���� ��Ʈ���� ������ PC���� �ð�
	ACE_TCHAR dvTime[TIMESTAMP_SIZE];	//�ܸ����� ĸó�ϴ� ������ �ܸ����� �ð�
	ACE_Message_Block *message;
	for (;;){
		if (this->getq(message) == -1) ACE_ERROR_RETURN((LM_ERROR, "%p\n", "getq"), -1);

		if (message->msg_type() == ACE_Message_Block::MB_HANGUP) {
			message->release();
			ACE_DEBUG((LM_DEBUG, "(%t) CRestoreTask::svc MB_HANGUP received\n"));
			break;
		}

		int nWidth, nHeight, nLength;
		int nGet = parseHeader(reinterpret_cast<const _u8*>(message->rd_ptr()), dvTime, pcTime, TIMESTAMP_SIZE, &nWidth, &nHeight, &nLength);
		ACE_ASSERT(nGet == HEADER_SIZE+TIMESTAMP_SIZE);

		ACE_DEBUG((LM_DEBUG, "S:C:R:w:h:l(%d:%s:%s:%d:%d:%d)\n", message->size(), dvTime, pcTime, nWidth, nHeight, nLength));
		message->release();
	}
	ACE_DEBUG((LM_DEBUG, "(%t) CRestoreTask::svc end\n"));
	return 0;
}