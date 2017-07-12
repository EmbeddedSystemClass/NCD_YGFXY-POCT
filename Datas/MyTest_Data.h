#ifndef _MYTEST_S_D_H__
#define _MYTEST_S_D_H__

#include	"Define.h"
#include	"PaiduiUnitData.h"
#include	"Operator.h"

typedef enum
{ 
	Error_OK = 0,																	//创建成功
	Error_Mem = 1,																	//内存错误
	Error_PaiDuiBusy = 2,															//排队测试忙，禁止再次创建批量测试
	Error_StopNormalTest = 3,														//批量测试中，禁止常规测试
	Error_PaiduiFull = 4,															//排队位满
	Error_PaiduiTesting = 5,														//测试中
	Error_PaiduiDisconnect = 6														//排队模块失联
}CreateTestErrorType;



typedef struct TestBuffer_tag{
	PaiduiUnitData * NormalTestDataBuffer;												//常规测试缓冲区
	PaiduiUnitData * (PaiduiTestDataBuffer[PaiDuiWeiNum]);								//8个排队位测试缓冲区
	Operator PaiduiUser;															//排队模式共用的操作人
	PaiduiUnitData * CurrentTestDataBuffer;												//当前测试
}TestBuffer;

bool IsPaiDuiTestting(void);
CreateTestErrorType CreateANewTest(TestType testtype);
MyRes DeleteCurrentTest(void);
PaiduiUnitData * GetTestItemByIndex(unsigned char index);
unsigned short GetMinWaitTime(void);
bool isSomePaiduiInOutTimeStatus(void);
bool isInTimeOutStatus(PaiduiUnitData * PaiduiUnitData);
void SetCurrentTestItem(PaiduiUnitData * s_PaiduiUnitData);
PaiduiUnitData * GetCurrentTestItem(void);

void SetPaiduiUser(Operator * user);
void GetPaiduiUser(Operator * user);

#endif

