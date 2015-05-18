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

#define SIZE_BUF (1024*4)
#define REF_WIDTH 1440
#define REF_HEIGHT 2560
#define REF_AREA (REF_WIDTH*REF_HEIGHT)
#define RGBA_KIND  4

typedef unsigned char   _u8;
typedef unsigned short  _u16;
typedef unsigned int    _u32;
typedef signed char     _s8;
typedef signed short    _s16;
typedef signed int      _s32;

static char* SERVER_HOST = "127.0.0.1";
static u_short SERVER_PORT = 19001;

int get_frame(_u8* p, _u32 fbiSize, ACE_SOCK_Stream& client_stream);

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

	int nRtn = 0, nIter=0;
	char buffer[SIZE_BUF];
	int nHeaderSize = (sizeof(ACE_Time_Value) + 3 * sizeof(int));
	ACE_DEBUG((LM_DEBUG, "header size:%d\n", nHeaderSize));

	_u8* pRawBuffer = new _u8[REF_AREA*RGBA_KIND * 2];
	while (true){
		if ((nRtn = client_stream.recv_n(buffer, nHeaderSize)) == -1) {
			ACE_ERROR((LM_ERROR, "(%P|%t) Error in reading header, recv_n(%d)\n", nRtn));
			break;
		}

		char* p = buffer;
		ACE_Time_Value tv = ACE_OS::gettimeofday();

		//ACE_OS::memcpy(&tv, p, sizeof(ACE_Time_Value)); p += sizeof(ACE_Time_Value);
		ACE_Date_Time dt;
		dt.update(tv);
		ACE_DEBUG((LM_DEBUG, "%02d-%02d %02d:%02d:%02d .%d\n",dt.month(),dt.day(),dt.hour(),dt.minute(),dt.second(),dt.microsec()));

		int nWidth = 576;
		int nHeight = 1024;
		int nLength = 57054;
		/*
		ACE_OS::memcpy(&nWidth, p, sizeof(int)); p += sizeof(int);
		nWidth = ntohs(nWidth);

		ACE_OS::memcpy(&nHeight, p, sizeof(int)); p += sizeof(int);
		ACE_OS::memcpy(&nLength, p, sizeof(int)); p += sizeof(int);
		ACE_DEBUG((LM_DEBUG, "width:%d height:%d length:%d\n", nWidth, nHeight, nLength));
		*/

		int nGet = get_frame(pRawBuffer, nLength, client_stream);
		ACE_ASSERT(nGet == nLength);
	}
	delete[] pRawBuffer;

	if (client_stream.close() == -1)
		ACE_ERROR_RETURN((LM_ERROR, "(%P|%t) %p \n", "close"), -1);

	return 0;
}

int get_frame(_u8* p, _u32 fbiSize, ACE_SOCK_Stream& client_stream)
{
	static const _u32 buff_size = 1 * 1024;
	int fb_size = fbiSize;
	int total_read = 0;
	int read_size = buff_size;
	int ret;
	_u8 buff[buff_size];

	while (0 < (ret = client_stream.recv_n(buff,read_size))) {
		total_read += ret;
		memcpy(p, buff, ret);
		p += ret;
		if (fb_size - total_read < buff_size) read_size = fb_size - total_read;
	}
	return total_read;
}
