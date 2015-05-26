#ifndef __CNULLTASK_H__
#define __CNULLTASK_H__

#include "ace/Task.h"

class CNullTask : public ACE_Task<ACE_MT_SYNCH>
{
public:
	CNullTask();
	~CNullTask();

	virtual int svc(void); 
};

#endif
