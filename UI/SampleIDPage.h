#ifndef __SampleID_P_H__
#define __SampleID_P_H__

#include	"Define.h"
#include	"Timer_Data.h"
#include	"MyTest_Data.h"
#include	"UI_Data.h"

typedef struct SampleIDPage_tag {
	unsigned char tempbuf[150];
	unsigned int tempv1;
	PaiduiUnitData * currenttestdata;												//��ǰ��������
	unsigned short lcdinput[100];
}SampleIDPage;

MyRes createSampleActivity(Activity * thizActivity, Intent * pram);

#endif

