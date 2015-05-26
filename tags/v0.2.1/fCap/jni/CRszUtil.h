#ifndef __CRSZUTIL_H__
#define __CRSZUTIL_H__

#include "cctTypeDef.h"

//Resize utility class

class CRszUtil 
{
public:
	static void method_0(int nThreshold, int nBpp, int nWidth, int nHeight,
		int* pResize_w, int* pResize_h, int out_pixel_per_bytes, _u8* resize_buffer, _u8* raw_buffer);
	static void method_1(int nBpp, int nWidth, int nHeight, int out_pixel_per_bytes,
		_u8* resize_buffer, _u8* raw_buffer);
	static int resize_raw_image(_u8* dest, _u8* raw_buffer, int nBpp, int nWidth, int nHeight,
		int resize_w, int resize_h, _u32 out_pixel_per_bytes);
	static int conv_image_bpp(_u8* src, _u8* dest, const _u16 w, const _u16 h, _u32 src_bpp, _u32 dest_bpp);
};

#endif
