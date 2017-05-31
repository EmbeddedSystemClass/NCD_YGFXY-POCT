#ifndef __SUSER_P_H__
#define __SUSER_P_H__

#include	"Define.h"
#include	"MyTest_Data.h"
#include	"Timer_Data.h"
#include	"UI_Data.h"
#include	"Operator.h"

typedef struct UserPage_tag {
	unsigned char selectindex;										//选中的索引
	unsigned char pageindex;										//页面索引
	Operator user[MaxOperatorSize];										//所有操作人列表
	Operator * tempUser;											//临时指针
	Operator * tempUser2;											//当前选中的操作人，null表示未选择
	PaiduiUnitData * currenttestdata;										//当前测试数据
	unsigned short lcdinput[100];									//lcd输入解析
}UserPageBuffer;

MyState_TypeDef createSelectUserActivity(Activity * thizActivity, Intent * pram);

#endif

