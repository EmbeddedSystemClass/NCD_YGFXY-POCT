#ifndef __QUALITYDETAIL_P_H__
#define __QUALITYDETAIL_P_H__

#include	"UI_Data.h"
#include	"DeviceQuality.h"

typedef struct QualityDetailPageBuffer_tag {
	unsigned short lcdinput[100];
	char tempBuf[100];
	DeviceQuality deviceQuality;
	float tempValue1;
	double resultSum;										//有效测试结果和
	double tempValue2;
	unsigned char tempValue3;
}QualityDetailPageBuffer;

#define	QualityDetailPageBufferStructSize	sizeof(QualityDetailPageBuffer)

MyState_TypeDef createQualityDetailActivity(Activity * thizActivity, Intent * pram);

#endif

