#ifndef __ERRORRECORD_P_H__
#define __ERRORRECORD_P_H__

#include	"DeviceError.h"
#include	"DeviceErrorDao.h"
#include	"UI_Data.h"


typedef struct ErrorRecordPageBuffer_tag {

	unsigned char selectindex;							//选中的索引
	unsigned int pageindex;							//页面索引
	unsigned int maxpagenum;
	
	DeviceErrorReadPackge deviceErrorReadPackge;		//读取数据
	
	char buf[100];										//临时缓冲区
	char tempBuf[100];										//临时缓冲区
	unsigned int tempvalue1;
	unsigned int tempvalue2;
	DeviceError *tempDeviceError;
	unsigned short lcdinput[100];
}ErrorRecordPageBuffer;

MyRes createErrorRecordActivity(Activity * thizActivity, Intent * pram);

#endif

