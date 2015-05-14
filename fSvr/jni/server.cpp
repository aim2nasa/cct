#include <ace/OS.h>
#include <ace/Log_Msg.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Acceptor.h>
#include "CStreamHandler.h"

#define SERVER_PORT 9191

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
	u_short server_port = argc > 1 ? ACE_OS::atoi(argv[1]) : SERVER_PORT;
	ACE_DEBUG((LM_INFO, "(%t) server start at port:%d\n", server_port));

	ACE_INET_Addr listen;
	listen.set(SERVER_PORT);

	ACE_Acceptor<CStreamHandler, ACE_SOCK_ACCEPTOR> acceptor;
	acceptor.open(listen);
	ACE_Reactor::instance()->run_reactor_event_loop();
	ACE_DEBUG((LM_INFO, "(%t) server end\n"));

	ACE_RETURN(0);
}