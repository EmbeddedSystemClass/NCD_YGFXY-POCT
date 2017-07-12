#ifndef __SDI_P_H__
#define __SDI_P_H__

#include	"UI_Data.h"
#include	"Operator.h"
#include	"Device.h"
#include	"MyLock.h"
#include	"Timer_Data.h"

typedef struct SetDeviceInfoPageBuffer_tag {
	Device device;
	Operator operator;														//临时数据
	char deviceunit[DeviceAddrLen];									//设备使用单位
	bool ismodify;
	unsigned short lcdinput[100];
	char tempBuf[100];
	MyLock * myDeviceLock;
	bool isLocked;
	Timer timer;
}SetDeviceInfoPageBuffer;


MyRes createSetDeviceInfoActivity(Activity * thizActivity, Intent * pram);

#endif

