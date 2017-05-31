#ifndef __TESTF_H__
#define __TESTF_H__

#include	"Define.h"
#include 	"FreeRTOS.h"
#include	"MyTest_Data.h"

#define	DataMaxWinSize			20

typedef struct PointData_tag {
	unsigned short data;
	unsigned short index;
}PointData;


#pragma pack(1)
typedef struct TempCalData_tag{
	double tempvalue1;
	double tempvalue2;
	double average;														//ƽ��ֵ
	double stdev;														//��׼��
	float CV1;															//����ϵ��1
	float CV2;															//����ϵ��2
	unsigned short tempvalue3;
	unsigned short temptestline[MaxPointLen + FilterNum];
	double lastdata;														//��¼�ϴ��˲�����
	
	unsigned short maxdata;
	PaiduiUnitData * paiduiUnitData;												//��������ָ��
	ResultState resultstatues;											//���Խ��״̬
}TempCalData;
#pragma pack()

MyState_TypeDef InitTestFunData(void);
ResultState TestFunction(void * parm);

MyState_TypeDef TakeTestPointData(void * data);
MyState_TypeDef TakeTestResultData(void * data);

#endif

