#ifndef __WIFISET_P_H__
#define __WIFISET_P_H__

#include	"Define.h"
#include	"UI_Data.h"
#include	"SystemSet_Data.h"

typedef struct WifiPageBuffer_tag {
	unsigned char pageindex;							//ҳ������
	unsigned char selectindex;
	WIFI_Def wifilist[MaxWifiListNum];
	WIFI_Def * wifip;
	unsigned short wifinum;
	Basic_ICO myico[PageWifiNum];
	unsigned short lcdinput[100];
	bool isGetWifiControl;								//�Ƿ��ѻ�ȡwifi����Ȩ
	char buf[100];
}WifiPageBuffer;

MyState_TypeDef createWifiSetActivity(Activity * thizActivity, Intent * pram);

#endif

