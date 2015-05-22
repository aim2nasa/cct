#include "CRestoreTask.h"
#include "utility.h"
#include "fMacro.h"
#include "zlib.h"
#include "ace/Lib_Find.h"

#define BUFFER_SIZE	(16*1024*1024)	//16MB

CRestoreTask::CRestoreTask()
	:m_bWriteBmp(false), m_pDecompBuffer(NULL), m_pInvertBuffer(NULL)
{
	m_pDecompBuffer = new _u8[BUFFER_SIZE];
	m_pInvertBuffer = new _u8[BUFFER_SIZE];
}

CRestoreTask::~CRestoreTask()
{
	delete [] m_pInvertBuffer;
	delete [] m_pDecompBuffer;
}

int CRestoreTask::svc(void)
{
	ACE_DEBUG((LM_DEBUG, "(%t) CRestoreTask::svc start\n"));

	ACE_TCHAR pcTime[TIMESTAMP_SIZE];	//pc에서 스트림을 수신한 PC기준 시간
	ACE_TCHAR dvTime[TIMESTAMP_SIZE];	//단말에서 캡처하는 시점의 단말기준 시간
	ACE_Message_Block *message;
	for (;;){
		if (this->getq(message) == -1) ACE_ERROR_RETURN((LM_ERROR, "%p\n", "getq"), -1);

		if (message->msg_type() == ACE_Message_Block::MB_HANGUP) {
			message->release();
			ACE_DEBUG((LM_DEBUG, "(%t) CRestoreTask::svc MB_HANGUP received\n"));
			break;
		}

		int nWidth, nHeight, nLength;
		int nHeaderSize = parseHeader(reinterpret_cast<const _u8*>(message->rd_ptr()), dvTime, pcTime, TIMESTAMP_SIZE, &nWidth, &nHeight, &nLength);
		ACE_ASSERT(nHeaderSize == (HEADER_SIZE+TIMESTAMP_SIZE));	//PC에서 수신후 붙인 타임스탬프로 인해 증가

		ACE_DEBUG((LM_DEBUG, "S:C:R:w:h:l(%d:%s:%s:%d:%d:%d)\n", message->size(), dvTime, pcTime, nWidth, nHeight, nLength));
		restore(dvTime,pcTime,reinterpret_cast<const _u8*>(message->rd_ptr() + nHeaderSize), nLength, nWidth, nHeight, 24);

		message->release();
	}
	ACE_DEBUG((LM_DEBUG, "(%t) CRestoreTask::svc end\n"));
	return 0;
}

int CRestoreTask::restore(ACE_TCHAR* pDvTime, ACE_TCHAR* pPcTime, const _u8* compress_buffer, const _u32 compress_buffer_len, const _u16 width, const _u16 height, const _u32 bpp)
{
	uLongf uDecompBufferLen = BUFFER_SIZE;
	uncompress((Bytef*)m_pDecompBuffer, &uDecompBufferLen, (Bytef*)compress_buffer, compress_buffer_len);
	ACE_ASSERT(uDecompBufferLen > 0);

	downsideUp(m_pDecompBuffer, m_pInvertBuffer, width, height, bpp);

	ACE_TCHAR fileName[512];
	ACE_OS::sprintf(fileName, "C%s_R%s_whls(%d_%d_%d_%d).bmp", pDvTime, pPcTime, width, height, compress_buffer_len);
	ACE::strrepl(fileName, ':', '_');

	if (m_bWriteBmp) write_bmp(fileName, m_pInvertBuffer, width, height, bpp);
	return 0;
}

void CRestoreTask::downsideUp(const _u8* in_buffer, _u8* out_buffer, const _u16 width, const _u16 height, const _u32 bpp)
{
	_u16 i = 0, j = height;
	_u32 pixel_per_bytes = bpp / 8;

	while(j--)
		memcpy(&out_buffer[width*pixel_per_bytes*i++], &in_buffer[width*pixel_per_bytes*j], width*pixel_per_bytes);
}

int CRestoreTask::write_bmp(ACE_TCHAR* pFileName, _u8* buffer, const _u16 width, const _u16 height, const _u32 bpp)
{
	FILE *f = ACE_OS::fopen(pFileName, ACE_TEXT("wb"));
	ACE_ASSERT(f!=NULL);

	BITMAPFILEHEADER bmf;
	BITMAPINFOHEADER bmi;
	memset(&bmf, 0, sizeof(bmf));
	memset(&bmi, 0, sizeof(bmi));

	bmi.biSize = sizeof(bmi);
	bmi.biSizeImage = width * height * (bpp / 8);
	bmi.biWidth = width;
	bmi.biHeight = height;
	bmi.biPlanes = 1;
	bmi.biBitCount = bpp;
	bmi.biCompression = BI_RGB;

	bmf.bfType = /*'B' + ('M' << 8)*/0x4D42;
	bmf.bfOffBits = sizeof(bmf) + bmi.biSize;
	bmf.bfSize = bmf.bfOffBits + bmi.biSizeImage;

	size_t nWrite;
	nWrite = ACE_OS::fwrite(&bmf,1,sizeof(bmf), f);
	ACE_ASSERT(nWrite == sizeof(bmf));

	nWrite = ACE_OS::fwrite(&bmi,1,sizeof(bmi), f);
	ACE_ASSERT(nWrite == sizeof(bmi));

	static unsigned char _zero = 0x00;
	int padding = (width * (bpp / 8)) % 4;

	if (padding) {
		for (int i = 0; i < height; ++i){
			nWrite = ACE_OS::fwrite(buffer + (width * (bpp / 8) * i), width * (bpp / 8), 1, f);

			for (int j = 0; j < padding; ++j)
				nWrite = ACE_OS::fwrite(&_zero, 1, 1, f);
		}
	}else {
		nWrite = ACE_OS::fwrite(buffer, bmi.biSizeImage, 1, f);
	}
	ACE_OS::fclose(f);
	return 0;
}