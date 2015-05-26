#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "fType.h"
#include "ace/SOCK_Stream.h"

int get_frame(_u8* p, _u32 fbiSize, ACE_SOCK_Stream& client_stream);
int parseHeader(const _u8* pBuffer, ACE_TCHAR* pTimeStamp, int nTimeStampSize, int* pWidth, int* pHeight, int* Length);
int parseHeader(const _u8* pBuffer, ACE_TCHAR* pDvTimeStamp, ACE_TCHAR* pPcTimeStamp, int nTimeStampSize, int* pWidth, int* pHeight, int* pLength);

#endif