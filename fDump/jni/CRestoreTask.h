#ifndef __CRESTORETASK_H__
#define __CRESTORETASK_H__

#include "ace/Task.h"

class CRestoreTask : public ACE_Task < ACE_MT_SYNCH >
{
public:
	virtual int svc(void);

protected:
};

#endif