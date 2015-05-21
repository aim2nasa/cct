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
	int restore(const _u8* compress_buffer, const _u32 compress_buffer_len, const _u16 width, const _u16 height, const _u32 bpp);
	static void invert_data(const _u8* in_buffer, _u8* out_buffer, const _u16 width, const _u16 height, const _u32 bpp);

protected:
	_u8*	m_pDecompBuffer;
	_u8*	m_pInvertBuffer;
};

#endif