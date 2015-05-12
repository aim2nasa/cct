#include "CCapTask.h"
#include "fbinfo.h"
#include "ace/OS_NS_time.h"

#define DDMS_RAWIMAGE_VERSION 1

CCapTask::CCapTask()
:m_pQ(NULL)
{
}

CCapTask::~CCapTask()
{
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

    int nAllocSize = fbi.width*fbi.height*(fbi.bpp/8);
    _u8* raw_buffer = new _u8[nAllocSize];
    int nRead = get_raw_buffer(raw_buffer,fbi.size,fp);
    ACE_ASSERT(nRead==fbi.size);
    ACE_DEBUG((LM_DEBUG," BUF",nRead));

    if(m_pQ) {
      ACE_NEW_RETURN(message,ACE_Message_Block(nAllocSize),-1);
      message->copy(reinterpret_cast<const char*>(raw_buffer),nAllocSize);
      int nPutQ = m_pQ->enqueue(message);
      ACE_ASSERT(nPutQ!=-1);
    }

    delete [] raw_buffer;

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

void CCapTask::method_0(int nThreshold,int nBpp,int nWidth,int nHeight,int* pResize_w,
                        int* pResize_h,int out_pixel_per_bytes,_u8* resize_buffer,_u8* raw_buffer)
{
  ACE_ASSERT(resize_buffer);
		
  if((nWidth * nHeight) > nThreshold) {
    float ratio = nWidth / (float)nHeight;
    while( (*pResize_w)*(*pResize_h) > nThreshold) {
      (*pResize_h)--;
      (*pResize_w) = (int)((float)(*pResize_h) * ratio);
    }
    resize_raw_image(resize_buffer,raw_buffer,nBpp,nWidth,nHeight,*pResize_w,*pResize_h,out_pixel_per_bytes);
  }
}

void CCapTask::method_1(int nBpp,int nWidth,int nHeight,int out_pixel_per_bytes,
                        _u8* resize_buffer,_u8* raw_buffer)
{
  ACE_ASSERT(resize_buffer);
  ACE_ASSERT(nBpp==3);
  conv_image_bpp(raw_buffer, resize_buffer, nWidth, nHeight, nBpp, out_pixel_per_bytes);
}

int CCapTask::resize_raw_image(_u8* dest,_u8* raw_buffer,int nBpp,int nWidth,int nHeight,
                               int resize_w,int resize_h,_u32 out_pixel_per_bytes)
{
  _u32 pixel_per_bytes = nBpp; 
  _u32 x_ratio = (_u32)((nWidth<<16)/resize_w) + 1;
  _u32 y_ratio = (_u32)((nHeight<<16)/resize_h) + 1;

  _u32 i, j;
  _u32 x2, y2;
  for(i = 0 ; i < resize_h ; ++i) {
    y2 = ((i * y_ratio)>>16) ;
    for(j = 0 ; j < resize_w ; ++j) {
      x2 = ((j * x_ratio)>>16) ;

      dest[(i*resize_w*out_pixel_per_bytes)+(j*out_pixel_per_bytes)+0] = 
        raw_buffer[(y2*nWidth*pixel_per_bytes)+(x2*pixel_per_bytes)+2];

      dest[(i*resize_w*out_pixel_per_bytes)+(j*out_pixel_per_bytes)+1] = 
        raw_buffer[(y2*nWidth*pixel_per_bytes)+(x2*pixel_per_bytes)+1];

      dest[(i*resize_w*out_pixel_per_bytes)+(j*out_pixel_per_bytes)+2] = 
        raw_buffer[(y2*nWidth*pixel_per_bytes)+(x2*pixel_per_bytes)+0];
    }
  }
  return 0;
}

int CCapTask::conv_image_bpp(_u8* src, _u8* dest, const _u16 w, const _u16 h, _u32 src_bpp, _u32 dest_bpp) 
{
  _u32 i, j;

  for(i = 0 ; i < h ; ++i) {
    for(j = 0 ; j < w ; ++j) {
      dest[(i * w * dest_bpp) + (j * dest_bpp) + 0] = src[(i * w * src_bpp) + (j * src_bpp) + 2];
      dest[(i * w * dest_bpp) + (j * dest_bpp) + 1] = src[(i * w * src_bpp) + (j * src_bpp) + 1];
      dest[(i * w * dest_bpp) + (j * dest_bpp) + 2] = src[(i * w * src_bpp) + (j * src_bpp) + 0];
    }
  }
  return 0;
}
