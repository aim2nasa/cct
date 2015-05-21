#include "ace/SOCK_Connector.h" 
#include "ace/Log_Msg.h" 
#include "ace/OS.h"
#include "utility.h"
#include "CRestoreTask.h"
#include "fMacro.h"

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

	CRestoreTask rt;
	ACE_Message_Queue<ACE_MT_SYNCH>* m_pQ = rt.msg_queue();
	rt.activate();

	ACE_Message_Block *message;
	ACE_TCHAR pcTime[TIMESTAMP_SIZE];	//pc에서 스트림을 수신한 PC기준 시간
	ACE_TCHAR dvTime[TIMESTAMP_SIZE];	//단말에서 캡처하는 시점의 단말기준 시간
	_u8* pRawBuffer = new _u8[REF_AREA*RGBA_KIND * 2];
	while (1)
	{
		ACE_Time_Value tv = ACE_OS::gettimeofday();
		int nGet = client_stream.recv_n(pRawBuffer, HEADER_SIZE);
		ACE_ASSERT(nGet == HEADER_SIZE);

		int nWidth, nHeight, nLength;
		nGet = parseHeader(pRawBuffer, dvTime, TIMESTAMP_SIZE, &nWidth, &nHeight, &nLength);
		ACE_ASSERT(nGet == HEADER_SIZE);

		nGet = get_frame(pRawBuffer+HEADER_SIZE, nLength, client_stream);
		ACE_ASSERT(nGet == nLength);

		ACE_Time_Value nowTv = ACE_OS::gettimeofday();
		tv = nowTv - tv;

		ACE::timestamp(nowTv, pcTime, sizeof(pcTime));
		ACE_DEBUG((LM_DEBUG, "CapDv(%s) RcvPc(%s) w(%d) h(%d) length(%d) %dms\n", dvTime, pcTime, nWidth, nHeight, nLength, tv.msec()));

		ACE_NEW_RETURN(message, ACE_Message_Block(HEADER_SIZE + nLength + TIMESTAMP_SIZE), -1);	//PC time stamp를 추가
		ACE_OS::memcpy(message->wr_ptr(), dvTime, TIMESTAMP_SIZE);
		message->wr_ptr(TIMESTAMP_SIZE);
		ACE_OS::memcpy(message->wr_ptr(), pcTime, TIMESTAMP_SIZE);
		message->wr_ptr(TIMESTAMP_SIZE);
		ACE_OS::memcpy(message->wr_ptr(), pRawBuffer+TIMESTAMP_SIZE, nLength+HEADER_SIZE-TIMESTAMP_SIZE);
		message->wr_ptr(nLength+HEADER_SIZE-TIMESTAMP_SIZE);

		ACE_ASSERT(m_pQ);
		m_pQ->enqueue(message);
	}
	delete[] pRawBuffer;

	if (client_stream.close() == -1)
		ACE_ERROR_RETURN((LM_ERROR, "(%P|%t) %p \n", "close"), -1);

	return 0;
}