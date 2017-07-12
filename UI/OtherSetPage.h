#ifndef __OTHERSET_P_H__
#define __OTHERSET_P_H__

#include	"Define.h"
#include	"SystemSet_Data.h"
#include	"UI_Data.h"
#include	"DateTime.h"

typedef struct OtherSetPageBuffer_tag {
	DateTime temptime;
	SystemSetData systemSetData;
	SystemSetData	tempSystemSetData;
	bool isChange;
	char buf[50];
	unsigned short lcdinput[100];
	unsigned short tempvalue;
	Basic_ICO ico;
}OtherSetPageBuffer;

MyRes createOtherSetActivity(Activity * thizActivity, Intent * pram);

#endif

