#ifndef __CZCTASK_H__
#define __CZCTASK_H__

#include "ace/Task.h"
#include "cctTypeDef.h"

class CZcTask : public ACE_Task<ACE_MT_SYNCH>
{
public:
  CZcTask();
  ~CZcTask();

  virtual int svc(void); 

  ACE_Message_Queue<ACE_MT_SYNCH>* m_pQ;

protected:
  _u8* m_pGivenBuffer;
  _u8* m_pCompBuffer;
};

#endif
