#ifndef __QUALITY_P_H__
#define __QUALITY_P_H__

#include	"UI_Data.h"
#include	"DeviceQuality.h"

typedef struct QualityPageBuffer_tag {
	unsigned short lcdinput[100];
	char tempBuf[50];
	DeviceQuality * deviceQuality;
	bool canTestNow;										//现在能否进行测试
	bool isTestting;										//是否正在进行一次测试
	unsigned char testIndex;								//测试次数索引
	ResultState cardTestResult;								//测试结果
	ScanCodeResult cardScanResult;							//扫码结果
	TestData  testData;										//测试数据缓存
	float tempValue1;
	double resultSum;										//有效测试结果和
	double tempValue2;
}QualityPageBuffer;

#define	QualityPageBufferStructSize	sizeof(QualityPageBuffer)

MyState_TypeDef createQualityActivity(Activity * thizActivity, Intent * pram);

#endif

