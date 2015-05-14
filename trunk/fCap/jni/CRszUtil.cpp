#include "CRszUtil.h"

void CRszUtil::method_0(int nThreshold,int nBpp,int nWidth,int nHeight,int* pResize_w,
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

void CRszUtil::method_1(int nBpp,int nWidth,int nHeight,int out_pixel_per_bytes,
                        _u8* resize_buffer,_u8* raw_buffer)
{
  ACE_ASSERT(resize_buffer);
  ACE_ASSERT(nBpp==3);
  conv_image_bpp(raw_buffer, resize_buffer, nWidth, nHeight, nBpp, out_pixel_per_bytes);
}

int CRszUtil::resize_raw_image(_u8* dest,_u8* raw_buffer,int nBpp,int nWidth,int nHeight,
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

int CRszUtil::conv_image_bpp(_u8* src, _u8* dest, const _u16 w, const _u16 h, _u32 src_bpp, _u32 dest_bpp) 
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
