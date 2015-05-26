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

	int start();
	void stop();
	virtual int svc(void); 

	ACE_Message_Queue<ACE_MT_SYNCH>* m_pQ;
	bool m_bRun;

protected:
	_u8* m_pRawBuffer;
	_u8* m_pResizeBuffer;
};

#endif
