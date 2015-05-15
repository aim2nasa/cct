#include "CZcTask.h"
#include <zlib.h>

#define RAW_BYTE_PER_PIXEL 4
#define RAW_WIDTH 1440
#define RAW_HEIGHT 2560
#define COMPRESS_LEVEL 1
#define ENQUEUE_TIMEOUT 1 //time out 1 sec

CZcTask::CZcTask()
:m_pQ(NULL),m_pGivenBuffer(NULL),m_pCompBuffer(NULL)
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
    ACE_DEBUG((LM_DEBUG,"(1)"));

    _u8* raw_buffer = reinterpret_cast<_u8*>(message->rd_ptr());
    ACE_ASSERT(raw_buffer);
    ACE_DEBUG((LM_DEBUG,"(2)"));

    _u32 raw_buffer_size = message->size();
    ACE_DEBUG((LM_DEBUG,"(2)Msg(%d)",raw_buffer_size));
    _u32 cmp_buffer_size = RAW_WIDTH*RAW_HEIGHT*RAW_BYTE_PER_PIXEL*2;
    ACE_DEBUG((LM_DEBUG,"(2)RAW(%d),CMP(%d)",raw_buffer_size,cmp_buffer_size));

    int nRtn = compress2(m_pCompBuffer,(uLongf*)&cmp_buffer_size,raw_buffer,raw_buffer_size,COMPRESS_LEVEL);

    ACE_DEBUG((LM_DEBUG,"(3):%d",cmp_buffer_size));
    switch(nRtn){
	case Z_OK:
          ACE_DEBUG((LM_DEBUG,"(4)ZOK"));
	  if(m_pQ){
	    ACE_Message_Block *cmp_message;	
            ACE_NEW_RETURN(cmp_message,ACE_Message_Block(cmp_buffer_size),-1);
            cmp_message->copy(reinterpret_cast<const char*>(m_pCompBuffer),cmp_buffer_size);
            ACE_Time_Value waitTime(ACE_OS::gettimeofday()+ACE_Time_Value(ENQUEUE_TIMEOUT,0));
            if(m_pQ->enqueue(cmp_message,&waitTime)<0){
              cmp_message->release();
              ACE_DEBUG((LM_DEBUG,"enqueue timeout(%dsec)\n",ENQUEUE_TIMEOUT));
            }
	  }
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

  if(m_pQ) {
    ACE_NEW_RETURN(message,ACE_Message_Block(0,ACE_Message_Block::MB_HANGUP),-1);
    m_pQ->enqueue(message);
  }
  ACE_DEBUG((LM_DEBUG,"(%t) svc end\n"));
  return 0;
}
