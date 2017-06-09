#ifndef __ADJUSTRECORD_P_H__
#define __ADJUSTRECORD_P_H__

#include	"DeviceAdjust.h"
#include	"DeviceAdjustDao.h"
#include	"UI_Data.h"


typedef struct AdjustRecordPageBuffer_tag {

	unsigned char selectindex;							//选中的索引
	unsigned int pageindex;							//页面索引
	unsigned int maxpagenum;
	
	DeviceAdjustReadPackge deviceAdjustReadPackge;		//读取数据
	
	char buf[100];										//临时缓冲区
	char tempBuf[100];										//临时缓冲区
	unsigned int tempvalue1;
	unsigned int tempvalue2;
	DeviceAdjust *tempDeviceAdjust;
	unsigned short lcdinput[100];
}AdjustRecordPageBuffer;

MyState_TypeDef createAdjustRecordActivity(Activity * thizActivity, Intent * pram);

#endif

