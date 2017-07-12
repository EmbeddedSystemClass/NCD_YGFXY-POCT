#ifndef __SETDEVICEID_P_H__
#define __SETDEVICEID_P_H__

#include	"SystemSet_Data.h"
#include	"UI_Data.h"
#include	"Device.h"

typedef struct SetDeviceIDPage_tag {
	SystemSetData systemSetData;
	char tempbuf[150];								//临时指针
	char deviceId[DeviceIdLen];							//新输入的设备id
	unsigned char ismodify;
	unsigned short lcdinput[100];
}SetDeviceIDPage;

MyRes createSetDeviceIDActivity(Activity * thizActivity, Intent * pram);

#endif

