#include "CRestoreTask.h"
#include "utility.h"
#include "fMacro.h"
#include "zlib.h"

#define BUFFER_SIZE	(16*1024*1024)	//16MB

CRestoreTask::CRestoreTask()
	:m_pDecompBuffer(NULL), m_pInvertBuffer(NULL)
{
	m_pDecompBuffer = new _u8[BUFFER_SIZE];
	m_pInvertBuffer = new _u8[BUFFER_SIZE];
}

CRestoreTask::~CRestoreTask()
{
	delete [] m_pInvertBuffer;
	delete [] m_pDecompBuffer;
}

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
		int nHeaderSize = parseHeader(reinterpret_cast<const _u8*>(message->rd_ptr()), dvTime, pcTime, TIMESTAMP_SIZE, &nWidth, &nHeight, &nLength);
		ACE_ASSERT(nHeaderSize == (HEADER_SIZE+TIMESTAMP_SIZE));	//PC���� ������ ���� Ÿ�ӽ������� ���� ����

		ACE_DEBUG((LM_DEBUG, "S:C:R:w:h:l(%d:%s:%s:%d:%d:%d)\n", message->size(), dvTime, pcTime, nWidth, nHeight, nLength));
		restore(reinterpret_cast<const _u8*>(message->rd_ptr() + nHeaderSize), nLength, nWidth, nHeight, 24);

		message->release();
	}
	ACE_DEBUG((LM_DEBUG, "(%t) CRestoreTask::svc end\n"));
	return 0;
}

int CRestoreTask::restore(const _u8* compress_buffer, const _u32 compress_buffer_len, const _u16 width, const _u16 height, const _u32 bpp)
{
	uLongf uDecompBufferLen = BUFFER_SIZE;
	uncompress((Bytef*)m_pDecompBuffer, &uDecompBufferLen, (Bytef*)compress_buffer, compress_buffer_len);
	ACE_ASSERT(uDecompBufferLen > 0);

	return 0;
}