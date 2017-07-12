#ifndef __QUALITYRECORD_P_H__
#define __QUALITYRECORD_P_H__

#include	"DeviceQuality.h"
#include	"UI_Data.h"


typedef struct QualityRecordPageBuffer_tag {

	unsigned char selectindex;							//选中的索引
	unsigned int pageindex;							//页面索引
	unsigned int maxpagenum;
	
	DeviceQualityReadPackge deviceQualityReadPackge;		//读取数据
	
	char buf[100];										//临时缓冲区
	char tempBuf[100];										//临时缓冲区
	unsigned int tempvalue1;
	unsigned int tempvalue2;
	DeviceQuality *tempDeviceQuality;
	unsigned short lcdinput[100];
}QualityRecordPageBuffer;

MyRes createQualityRecordActivity(Activity * thizActivity, Intent * pram);

#endif

