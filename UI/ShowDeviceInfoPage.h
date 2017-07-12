#ifndef __SDEVICEINFO_P_H__
#define __SDEVICEINFO_P_H__

#include	"SystemSet_Data.h"
#include	"UI_Data.h"
#include	"Device.h"

typedef struct ShowDeviceInfoPageBuffer_Tag
{
	unsigned char presscount;
	unsigned char tempValue;
	Device device;
	SystemSetData systemSetData;
	unsigned short lcdinput[100];
	char tempBuf[100];
}ShowDeviceInfoPageBuffer;

MyRes createDeviceInfoActivity(Activity * thizActivity, Intent * pram);

#endif

