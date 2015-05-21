#include "utility.h"
#include "ace/OS.h"
#include "ace/SOCK_Stream.h"

int get_frame(_u8* p, _u32 fbiSize, ACE_SOCK_Stream& client_stream)
{
	static const _u32 buff_size = 1 * 1024;
	int fb_size = fbiSize;
	int total_read = 0;
	int read_size = buff_size;
	int ret;
	_u8 buff[buff_size];

	while (0 < (ret = client_stream.recv_n(buff, read_size))) {
		total_read += ret;
		ACE_OS::memcpy(p, buff, ret);
		p += ret;
		if (fb_size - total_read < buff_size) read_size = fb_size - total_read;
	}
	return total_read;
}

int parseHeader(const _u8* pBuffer, ACE_TCHAR* pTimeStamp, int nTimeStampSize, int* pWidth, int* pHeight, int* pLength)
{
	const _u8* p = pBuffer;

	ACE_OS::memcpy(pTimeStamp, p, nTimeStampSize); p += nTimeStampSize;
	ACE_OS::memcpy(pWidth, p, sizeof(int)); p += sizeof(int);
	ACE_OS::memcpy(pHeight, p, sizeof(int)); p += sizeof(int);
	ACE_OS::memcpy(pLength, p, sizeof(int)); p += sizeof(int);

	return nTimeStampSize + sizeof(int) * 3;
}