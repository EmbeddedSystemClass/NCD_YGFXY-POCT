#ifndef __ADJUSTLED_P_H__
#define __ADJUSTLED_P_H__

#include	"UI_Data.h"
#include	"MyTest_Data.h"
#include	"SystemSet_Data.h"

typedef struct AdjustLedPageBuffer_tag {
	unsigned short lcdinput[20];
	char buf[100];
	bool isTestting;									//�Ƿ����ڲ�����
	ResultState cardpretestresult;
	PaiduiUnitData PaiduiUnitData;
	unsigned short targetValue;							//Ŀ��ֵ
	unsigned short targetRange;							//���ֵ
	unsigned short maxPoint[2];							//0--���ֵ��1--���ֵλ��
	unsigned short i;
	unsigned char testCnt;								//���Ҵ���
	SystemSetData systemSetData;						//ϵͳ�������������ڱ���ʧ�ܻع�
}AdjustLedPageBuffer;


MyState_TypeDef createAdjustLedActivity(Activity * thizActivity, Intent * pram);

#endif

