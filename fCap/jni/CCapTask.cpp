#include "CCapTask.h"
#include "fbinfo.h"
#include "ace/OS_NS_time.h"

#define DDMS_RAWIMAGE_VERSION 1
#define REF_WIDTH 1440
#define REF_HEIGHT 2560
#define REF_AREA (REF_WIDTH*REF_HEIGHT)
#define RGBA_KIND  4
#define RGBA_BYTES (RGBA_KIND*8)
#define ENQUEUE_TIMEOUT	1 //time out 1 sec

CCapTask::CCapTask()
:m_pQ(NULL),m_pRawBuffer(NULL)
{
  m_pRawBuffer = new _u8[REF_AREA*RGBA_KIND*2]; //2 for redundancy
}

CCapTask::~CCapTask()
{
  delete [] m_pRawBuffer;
}

int CCapTask::svc(void)
{
  ACE_DEBUG((LM_DEBUG,"(%t) svc start\n"));

  time_t clock;
  FILE* fp;
  ACE_Message_Block* message;
  for(;;){
    ACE_OS::time(&clock);
    struct tm *tm = ACE_OS::localtime(&clock);
    ACE_Time_Value tv = ACE_OS::gettimeofday();
    fp = popen("screencap","r");
    if(!fp) ACE_ERROR_RETURN((LM_ERROR,"%p\n","popen(screencap)"),-1); 

    int w,h,f;
    if(fread(&w,1,sizeof(w),fp)!=sizeof(w)) ACE_ERROR_RETURN((LM_ERROR,"%p\n","fread(w)"),-1);
    if(fread(&h,1,sizeof(h),fp)!=sizeof(h)) ACE_ERROR_RETURN((LM_ERROR,"%p\n","fread(h)"),-1);
    if(fread(&f,1,sizeof(f),fp)!=sizeof(f)) ACE_ERROR_RETURN((LM_ERROR,"%p\n","fread(f)"),-1);
    ACE_DEBUG((LM_DEBUG,"[%d:%02d:%02d] w(%d) h(%d) f(%d)",tm->tm_hour,tm->tm_min,tm->tm_sec,w,h,f));

    struct fbinfo fbi;
    int nSurfInfo = get_surface_info(fbi,w,h,f);
    ACE_ASSERT(nSurfInfo!=-1);

    ACE_DEBUG((LM_DEBUG," bpp:%d sz:(%d) w:%d h:%d",fbi.bpp,fbi.size,fbi.width,fbi.height));
    ACE_DEBUG((LM_DEBUG," R(%d,%d) G(%d,%d) B(%d,%d) A(%d,%d)",
              fbi.red_offset,fbi.red_length,
              fbi.green_offset,fbi.green_length,
              fbi.blue_offset,fbi.blue_length,
              fbi.alpha_offset,fbi.alpha_length));  

    int nRead = get_raw_buffer(m_pRawBuffer,fbi.size,fp);
    ACE_ASSERT(nRead==fbi.size);
    ACE_DEBUG((LM_DEBUG," BUF",nRead));

    if(m_pQ) {
      int nAllocSize = fbi.width*fbi.height*(fbi.bpp/8);
      ACE_NEW_RETURN(message,ACE_Message_Block(nAllocSize),-1);
      message->copy(reinterpret_cast<const char*>(m_pRawBuffer),nAllocSize);
      ACE_Time_Value waitTime(ACE_OS::gettimeofday()+ACE_Time_Value(ENQUEUE_TIMEOUT,0));
      if(m_pQ->enqueue(message,&waitTime)<0){
        message->release();
        ACE_DEBUG((LM_DEBUG,"enqueue timeout(%dsec)\n",ENQUEUE_TIMEOUT));
      }
    }

    tv = ACE_OS::gettimeofday() - tv;
    ACE_DEBUG((LM_DEBUG," %dms\n",tv.msec()));
    pclose(fp);
  }
  ACE_DEBUG((LM_DEBUG,"(%t) svc end\n"));
  return 0;
}

int CCapTask::get_surface_info(fbinfo& fbi,const int width, const int height, const int format)
{
  /* see hardware/hardware.h */
  switch(format)
  {
	case 1: /* RGBA_8888 */
		fbi.bpp = 32;
		fbi.size = width * height * 4;
		fbi.red_offset = 0;		fbi.red_length = 8;
		fbi.green_offset = 8;	fbi.green_length = 8;
		fbi.blue_offset = 16;	fbi.blue_length = 8;
		fbi.alpha_offset = 24;	fbi.alpha_length = 8;
		break;
	case 2: /* RGBX_8888 */
		fbi.bpp = 32;
		fbi.size = width * height * 4;
		fbi.red_offset = 0;		fbi.red_length = 8;
		fbi.green_offset = 8;	fbi.green_length = 8;
		fbi.blue_offset = 16;	fbi.blue_length = 8;
		fbi.alpha_offset = 24;	fbi.alpha_length = 0;
		break;
	case 3: /* RGB_888 */
		fbi.bpp = 24;
		fbi.size = width * height * 3;
		fbi.red_offset = 0;		fbi.red_length = 8;
		fbi.green_offset = 8;	fbi.green_length = 8;
		fbi.blue_offset = 16;	fbi.blue_length = 8;
		fbi.alpha_offset = 24;	fbi.alpha_length = 0;
		break;
	case 4: /* RGB_565 */
		fbi.bpp = 16;
		fbi.size = width * height * 2;
		fbi.red_offset = 11;	fbi.red_length = 5;
		fbi.green_offset = 5;	fbi.green_length = 6;
		fbi.blue_offset = 0;	fbi.blue_length = 5;
		fbi.alpha_offset = 0;	fbi.alpha_length = 0;
		break;
	case 5:
		fbi.bpp = 32;
		fbi.size = width * height * 4;
		fbi.red_offset = 16;	fbi.red_length = 8;
		fbi.green_offset = 8;	fbi.green_length = 8;
		fbi.blue_offset = 0;	fbi.blue_length = 8;
		fbi.alpha_offset = 24;	fbi.alpha_length = 8;
		break;
	default: /* unknown type */
		return -1;
  }
  fbi.version = DDMS_RAWIMAGE_VERSION;
  fbi.width = width;		
  fbi.height = height;
  return 0;
}

int CCapTask::get_raw_buffer(_u8* p,_u32 fbiSize,FILE* fp)
{
  static const _u32 buff_size = 1 * 1024;
  int fb_size = fbiSize ;
  int total_read = 0;
  int read_size = buff_size;
  int ret;
  _u8 buff[buff_size];	

  while( 0 < (ret = fread(buff, 1, read_size, fp)) ) {
    total_read += ret;
    memcpy(p, buff, ret);
    p+=ret;
    if(fb_size - total_read < buff_size) read_size = fb_size-total_read;
  }
  return total_read;
}