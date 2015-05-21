#include "ace/SOCK_Connector.h" 
#include "ace/Log_Msg.h" 
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

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
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

	ACE_TCHAR pcTime[TIMESTAMP_SIZE];	//pc에서 스트림을 수신한 PC기준 시간
	ACE_TCHAR dvTime[TIMESTAMP_SIZE];	//단말에서 캡처하는 시점의 단말기준 시간
	_u8* pRawBuffer = new _u8[REF_AREA*RGBA_KIND * 2];
	while (1)
	{
		ACE_Time_Value tv = ACE_OS::gettimeofday();
		int nGet = client_stream.recv_n(pRawBuffer, HEADER_SIZE);
		ACE_ASSERT(nGet == HEADER_SIZE);

		int nWidth, nHeight, nLength;
		parseHeader(pRawBuffer, dvTime, TIMESTAMP_SIZE, &nWidth, &nHeight, &nLength);

		nGet = get_frame(pRawBuffer+HEADER_SIZE, nLength, client_stream);
		ACE_ASSERT(nGet == nLength);

		ACE_Time_Value nowTv = ACE_OS::gettimeofday();
		tv = nowTv - tv;

		ACE::timestamp(nowTv, pcTime, sizeof(pcTime));

		ACE_DEBUG((LM_DEBUG, "%s w(%d) h(%d) length(%d) %dms\n", dvTime, nWidth, nHeight, nLength, tv.msec()));
		ACE_DEBUG((LM_DEBUG, "%s\n",pcTime));
	}
	delete[] pRawBuffer;

	if (client_stream.close() == -1)
		ACE_ERROR_RETURN((LM_ERROR, "(%P|%t) %p \n", "close"), -1);

	return 0;
}