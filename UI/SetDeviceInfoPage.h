#ifndef __SDI_P_H__
#define __SDI_P_H__

#include	"SystemSet_Data.h"
#include	"UI_Data.h"
#include	"Operator.h"
#include	"Device.h"

typedef struct SetDeviceInfoPageBuffer_tag {
	SystemSetData systemSetData;
	Operator user;														//��ʱ����
	char deviceunit[DeviceAddrLen];									//�豸ʹ�õ�λ
	unsigned char ismodify;
	unsigned short lcdinput[100];
	char tempBuf[100];
}SetDeviceInfoPageBuffer;


MyState_TypeDef createSetDeviceInfoActivity(Activity * thizActivity, Intent * pram);

#endif

