#include <iostream>
#include "ace/SOCK_Connector.h" 
#include "ace/INET_Addr.h" 
#include "ace/Log_Msg.h" 
#include "ace/OS_NS_stdio.h" 
#include "ace/OS_NS_string.h" 
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_stdlib.h"
#include "ace/OS_NS_time.h"
#include "ace/Date_Time.h"
#include "ace/OS.h"
#include "utility.h"

#define SIZE_BUF (1024*4)
#define REF_WIDTH 1440
#define REF_HEIGHT 2560
#define REF_AREA (REF_WIDTH*REF_HEIGHT)
#define RGBA_KIND  4
#define TIMESTAMP_SIZE 27
#define HEADER_SIZE 39	//Timestamp(27)+width(4)+height(4)+length(4)

static char* SERVER_HOST = "127.0.0.1";
static u_short SERVER_PORT = 19001;

int main(int argc, char *argv[])
{
	const char *server_host = argc > 1 ? argv[1] : SERVER_HOST;
	u_short server_port = argc > 2 ? ACE_OS::atoi(argv[2]) : SERVER_PORT;
	ACE_DEBUG((LM_INFO, "(%P|%t) server info(addr:%s,port:%d)\n", server_host, server_port));

	ACE_SOCK_Stream client_stream;
	ACE_INET_Addr remote_addr(server_port, server_host);
	ACE_SOCK_Connector connector;

	ACE_DEBUG((LM_DEBUG, "(%P|%t) Starting connect to %s: %d \n", remote_addr.get_host_name(), remote_addr.get_port_number()));
	if (connector.connect(client_stream, remote_addr) == -1)
		ACE_ERROR_RETURN((LM_ERROR, "(%P|%t) %p \n", "connection failed"), -1);
	else
		ACE_DEBUG((LM_DEBUG, "(%P|%t) connected to %s \n", remote_addr.get_host_name()));

	ACE_TCHAR timeStamp[TIMESTAMP_SIZE];
	_u8* pRawBuffer = new _u8[REF_AREA*RGBA_KIND * 2];
	while (1)
	{
		ACE_Time_Value tv = ACE_OS::gettimeofday();
		int nGet = client_stream.recv_n(pRawBuffer, HEADER_SIZE);
		ACE_ASSERT(nGet == HEADER_SIZE);

		int nWidth, nHeight, nLength;
		parseHeader(pRawBuffer, timeStamp, TIMESTAMP_SIZE, &nWidth, &nHeight, &nLength);

		nGet = get_frame(pRawBuffer+HEADER_SIZE, nLength, client_stream);
		ACE_ASSERT(nGet == nLength);

		tv = ACE_OS::gettimeofday() - tv;

		ACE_Date_Time dt;
		dt.update(ACE_OS::gettimeofday());
		ACE_DEBUG((LM_DEBUG, "%s w(%d) h(%d) length(%d) %dms\n",timeStamp,nWidth,nHeight,nLength,tv.msec()));
		//ACE_DEBUG((LM_DEBUG, "Rcv %d-%02d-%02d %02d:%02d:%02d.%06d\n", dt.year(),dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second(), dt.microsec()));
	}
	delete[] pRawBuffer;

	if (client_stream.close() == -1)
		ACE_ERROR_RETURN((LM_ERROR, "(%P|%t) %p \n", "close"), -1);

	return 0;
}