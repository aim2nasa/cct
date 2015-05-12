#ifndef __CCAPTASK_H__
#define __CCAPTASK_H__

#include "ace/Task.h"
#include "cctTypeDef.h"
struct fbinfo;

class CCapTask : public ACE_Task<ACE_MT_SYNCH>
{
public:
  CCapTask();
  ~CCapTask();
  int get_surface_info(fbinfo& fbi,const int width, const int height, const int format);
  int get_raw_buffer(_u8* p,_u32 fbiSize,FILE* fp);

  void method_0(int nThreshold,int nBpp,int nWidth,int nHeight,
       int* pResize_w,int* pResize_h,int out_pixel_per_bytes,_u8* resize_buffer,_u8* raw_buffer);
  void method_1(int nBpp,int nWidth,int nHeight,int out_pixel_per_bytes,_u8* resize_buffer,_u8* raw_buffer);
  int resize_raw_image(_u8* dest,_u8* raw_buffer,int nBpp,int nWidth,int nHeight,
       int resize_w,int resize_h,_u32 out_pixel_per_bytes);
  int conv_image_bpp(_u8* src, _u8* dest, const _u16 w, const _u16 h, _u32 src_bpp, _u32 dest_bpp);

  virtual int svc(void); 

  ACE_Message_Queue<ACE_MT_SYNCH>* m_pQ;

protected:
  _u8* m_pRawBuffer;
};

#endif
