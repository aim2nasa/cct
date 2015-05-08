#include "CCapTask.h"

CCapTask::CCapTask()
{
}

CCapTask::~CCapTask()
{
}

int CCapTask::svc(void)
{
  ACE_DEBUG((LM_DEBUG,"(%t) svc start\n"));

  FILE* fp = popen("screencap","r");
  if(!fp) ACE_ERROR_RETURN((LM_ERROR,"%p\n","popen(screencap)"),-1); 

  int w,h,f;
  if(fread(&w,1,sizeof(w),fp)!=sizeof(w)) ACE_ERROR_RETURN((LM_ERROR,"%p\n","fread(w)"),-1);
  if(fread(&h,1,sizeof(h),fp)!=sizeof(h)) ACE_ERROR_RETURN((LM_ERROR,"%p\n","fread(h)"),-1);
  if(fread(&f,1,sizeof(f),fp)!=sizeof(f)) ACE_ERROR_RETURN((LM_ERROR,"%p\n","fread(f)"),-1);
  ACE_DEBUG((LM_DEBUG,"(%t) w(%d) h(%d) f(%d)\n",w,h,f));

  ACE_Message_Block* message;
  for(;;){
    if(this->getq(message)==-1) ACE_ERROR_RETURN((LM_ERROR,"%p\n","getq"),-1);

    if(message->msg_type()==ACE_Message_Block::MB_HANGUP) {
      message->release();
      ACE_DEBUG((LM_DEBUG,"(%t) MB_HANGUP received\n"));
      break;
    }
  }
  pclose(fp);
  ACE_DEBUG((LM_DEBUG,"(%t) svc end\n"));
  return 0;
}
