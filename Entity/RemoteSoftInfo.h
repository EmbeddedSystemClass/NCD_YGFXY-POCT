/****************************************file start****************************************************/
#ifndef _REMOTESOFTINFO_E_H
#define	_REMOTESOFTINFO_E_H

#include	"RemoteSoftInfo.h"
#include	"Define.h"

#pragma pack(1)
typedef struct
{
	unsigned short RemoteFirmwareVersion;
	char md5[32];
	char desc[200];
	bool isSuccessDownloadFirmware;
}RemoteSoftInfo;
#pragma pack()

#define	RemoteSoftInfoStructSize		sizeof(RemoteSoftInfo)							//最多保存的用户数目
#define	RemoteSoftInfoStructCrcSize		RemoteSoftInfoStructSize - 2						//最多保存的用户数目

#endif

/****************************************end of file************************************************/
