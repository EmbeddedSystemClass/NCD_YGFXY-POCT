#ifndef __LUNCH_P_H__
#define __LUNCH_P_H__

#include	"Timer_Data.h"
#include	"MyTest_Data.h"
#include	"UI_Data.h"
#include	"SystemSet_Data.h"

typedef struct LunchPageBuffer_tag {
	Timer timer;
	unsigned char presscount;
	char buf[100];
	unsigned short lcdinput[100];
	CreateTestErrorType error;
}LunchPageBuffer;

MyState_TypeDef createLunchActivity(Activity * thizActivity, Intent * pram);

#endif

