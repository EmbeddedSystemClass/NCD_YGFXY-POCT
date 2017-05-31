#ifndef __PRERCARD_P_H__
#define __PRERCARD_P_H__

#include	"Define.h"
#include	"MyTest_Data.h"
#include	"Test_Fun.h"
#include	"UI_Data.h"

typedef struct PreReadPageBuffer_tag {
	PaiduiUnitData * currenttestdata;
	ResultState cardpretestresult;
	unsigned char preTestErrorCount;				//�����δ�����������ٴβ���ȷ�����û��������ǰ������ʾ�ز����
	ScanCodeResult scancode;
	Timer timer2;
	TestData temptestdata;							//��ʱ�������ݻ�����
	QRCode temperweima;						//��ʱ��ά�����ݻ�����
	char buf[100];
	unsigned short lcdinput[100];
}PreReadPageBuffer;

MyState_TypeDef createPreReadCardActivity(Activity * thizActivity, Intent * pram);

#endif
