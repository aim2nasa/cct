#include "CZcTask.h"
#include <zlib.h>

#define RAW_BYTE_PER_PIXEL 4
#define RAW_WIDTH 1440
#define RAW_HEIGHT 2560
#define COMPRESS_LEVEL 1

CZcTask::CZcTask()
:m_pGivenBuffer(NULL),m_pCompBuffer(NULL)
{
  m_pGivenBuffer = new _u8[RAW_WIDTH*RAW_HEIGHT*RAW_BYTE_PER_PIXEL];
  m_pCompBuffer = new _u8[RAW_WIDTH*RAW_HEIGHT*RAW_BYTE_PER_PIXEL*2]; //2 for redundancy
}

CZcTask::~CZcTask()
{
  delete [] m_pCompBuffer;
  delete [] m_pGivenBuffer;
}

int CZcTask::svc(void)
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

    _u8* raw_buffer = reinterpret_cast<_u8*>(message->rd_ptr());
    ACE_ASSERT(raw_buffer);

    _u32 cmp_buffer_size = sizeof(m_pCompBuffer);
    _u32 raw_buffer_size = sizeof(m_pGivenBuffer);
    int nRtn = compress2(m_pCompBuffer,(uLongf*)&cmp_buffer_size,raw_buffer,raw_buffer_size,COMPRESS_LEVEL);
    switch(nRtn){
	case Z_OK:
	  break;
	case Z_MEM_ERROR:
       	  ACE_DEBUG((LM_DEBUG,"(%t) Z_MEM_ERROR(%d),Insufficient memory\n",nRtn));
	  break;
	case Z_BUF_ERROR:
       	  ACE_DEBUG((LM_DEBUG,"(%t) Z_BUF_ERROR(%d),The buffer dest was not large enough to hold the compressed data\n",nRtn));
	  break;
	case Z_STREAM_ERROR:
       	  ACE_DEBUG((LM_DEBUG,"(%t) Z_STREAM_ERROR(%d),The level was not Z_DEFAULT_LEVEL, or was not between 0 and 9\n",nRtn));
	  break;
	default:
       	  ACE_DEBUG((LM_DEBUG,"(%t) Undefined error(%d)\n",nRtn));
	  break;
    }

    ACE_DEBUG((LM_DEBUG,"\n[%d->%d]\n",message->size(),cmp_buffer_size));

    message->release();
  }
  ACE_DEBUG((LM_DEBUG,"(%t) svc end\n"));
  return 0;
}
