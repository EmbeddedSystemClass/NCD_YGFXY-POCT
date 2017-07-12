#ifndef __ABOUTUS_P_H__
#define __ABOUTUS_P_H__

#include	"UI_Data.h"
#include	"Timer_Data.h"

typedef struct AboutUsPageBuffer_tag {
	unsigned short lcdinput[20];
	char buf[100];
	unsigned short tempV;
	Timer timer;
}AboutUsPageBuffer;

MyRes createAboutUsActivity(Activity * thizActivity, Intent * pram);

#endif

