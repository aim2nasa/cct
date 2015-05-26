#ifndef __CRESTORETASK_H__
#define __CRESTORETASK_H__

#include "ace/Task.h"
#include "fType.h"

class CRestoreTask : public ACE_Task < ACE_MT_SYNCH >
{
public:
	CRestoreTask();
	~CRestoreTask();

	virtual int svc(void);
	int restore(ACE_TCHAR* pDvTime,ACE_TCHAR* pPcTime,const _u8* compress_buffer, const _u32 compress_buffer_len, const _u16 width, const _u16 height, const _u32 bpp);
	static void downsideUp(const _u8* in_buffer, _u8* out_buffer, const _u16 width, const _u16 height, const _u32 bpp);
	static int write_bmp(ACE_TCHAR* pFileName, _u8* buffer, const _u16 width, const _u16 height, const _u32 bpp);

	bool	m_bWriteBmp;
protected:
	_u8*	m_pDecompBuffer;
	_u8*	m_pInvertBuffer;
};

#endif