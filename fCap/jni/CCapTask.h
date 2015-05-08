#ifndef __CCAPTASK_H__
#define __CCAPTASK_H__

#include "ace/Task.h"
struct fbinfo;

class CCapTask : public ACE_Task<ACE_MT_SYNCH>
{
public:
  CCapTask();
  ~CCapTask();
   int get_surface_info(fbinfo& fbi,const int width, const int height, const int format);

  virtual int svc(void); 
};

#endif
