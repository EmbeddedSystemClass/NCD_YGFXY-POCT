#ifndef __NETINFO_P_H__
#define __NETINFO_P_H__

#include	"Define.h"
#include	"UI_Data.h"
#include	"NetWork.h"
#include	"SystemSet_Data.h"
#include	"System_Data.h"

typedef struct NetInfoPageBuffer_tag {
	unsigned short lcdinput[20];
	SystemData systemData;
	Timer timer;
	char tempbuffer1[100];
	Basic_ICO lineico;
	Basic_ICO wifiico;
	bool isGetWifiControl;								//是否已获取wifi控制权
	IP WifiIP;										//wifi的ip
	char WifiSSID[30];									//所链接wifi的名字
	char WifiMAC[13];									//wifi模块的mac
	unsigned char WifiIndicator;						//所连接wifi的信号
}NetInfoPageBuffer;


MyRes createNetInfoActivity(Activity * thizActivity, Intent * pram);

#endif

