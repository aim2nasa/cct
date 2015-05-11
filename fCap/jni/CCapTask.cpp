#include "CCapTask.h"
#include "cctTypeDef.h"
#include "fbinfo.h"

#define DDMS_RAWIMAGE_VERSION 1

CCapTask::CCapTask()
{
}

CCapTask::~CCapTask()
{
}

int CCapTask::svc(void)
{
  ACE_DEBUG((LM_DEBUG,"(%t) svc start\n"));

  FILE* fp;
  for(;;){
    fp = popen("screencap","r");
    if(!fp) ACE_ERROR_RETURN((LM_ERROR,"%p\n","popen(screencap)"),-1); 

    int w,h,f;
    if(fread(&w,1,sizeof(w),fp)!=sizeof(w)) ACE_ERROR_RETURN((LM_ERROR,"%p\n","fread(w)"),-1);
    if(fread(&h,1,sizeof(h),fp)!=sizeof(h)) ACE_ERROR_RETURN((LM_ERROR,"%p\n","fread(h)"),-1);
    if(fread(&f,1,sizeof(f),fp)!=sizeof(f)) ACE_ERROR_RETURN((LM_ERROR,"%p\n","fread(f)"),-1);
    ACE_DEBUG((LM_DEBUG,"(%t) w(%d) h(%d) f(%d)",w,h,f));

    struct fbinfo fbi;
    int nSurfInfo = get_surface_info(fbi,w,h,f);
    ACE_ASSERT(nSurfInfo!=-1);

    ACE_DEBUG((LM_DEBUG," bpp:%d size:(%d) w:%d h:%d",fbi.bpp,fbi.size,fbi.width,fbi.height));
    ACE_DEBUG((LM_DEBUG," R(%d,%d) G(%d,%d) B(%d,%d) A(%d,%d)\n",
              fbi.red_offset,fbi.red_length,
              fbi.green_offset,fbi.green_length,
              fbi.blue_offset,fbi.blue_length,
              fbi.alpha_offset,fbi.alpha_length));  

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
