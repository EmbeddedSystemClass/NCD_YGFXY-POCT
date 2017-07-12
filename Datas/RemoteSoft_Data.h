#ifndef _REMOTESOFT_D_H__
#define _REMOTESOFT_D_H__

#include	"RemoteSoftInfo.h"
#include	"Define.h"

void setGbRemoteFirmwareVersion(unsigned short version);
unsigned short getGbRemoteFirmwareVersion(void);
void setIsSuccessDownloadFirmware(bool status);
bool getIsSuccessDownloadFirmware(void);
void setGbRemoteFirmwareMd5(char * md5);
bool checkMd5IsSame(char * sMd5, char * dMd5);

#endif

