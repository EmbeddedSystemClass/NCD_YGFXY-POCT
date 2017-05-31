#ifndef	_UPLOAD_F_H_H
#define _UPLOAD_F_H_H

#include	"Define.h"
#include	"TestDataDao.h"
#include	"SystemSet_Data.h"
#include	"RemoteSoft_Data.h"
#include	"TestData.h"
#include	"HttpPostTool.h"


#pragma pack(1)
typedef struct HttpClientBuffer_Tag
{
	SystemSetData systemSetData;
	HttpPostBuffer httpPostBuffer;
	char tempBuffer[1024];
}HttpClientBuffer;
#pragma pack()

#define	httpClientBufferStructSize	sizeof(HttpClientBuffer)

void UpLoadFunction(void);

#endif
