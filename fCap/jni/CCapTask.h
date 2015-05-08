#ifndef __CCAPTASK_H__
#define __CCAPTASK_H__

#include "ace/Task.h"

class CCapTask : public ACE_Task<ACE_MT_SYNCH>
{
public:
  CCapTask();
  ~CCapTask();

  virtual int svc(void); 
};

#endif
