#ifndef __WIFI_F_H__
#define __WIFI_F_H__

#include	"Define.h"
#include	"NetWork.h"
#include	"SystemSet_Data.h"

typedef enum
{ 
	None = 0,
	AT_Mode = 1,
	Normal_Mode = 2,
	PWM_Mode = 3,
}WIFI_WorkMode_DefType;

void WIFIInit(SystemSetData * systemSetData);

MyRes takeWifiMutex(portTickType xBlockTime);
void giveWifixMutex(void);
WIFI_WorkMode_DefType GetWifiWorkMode(void);
MyRes SetWifiWorkInAT(WIFI_WorkMode_DefType mode);
MyRes ScanApList(WIFI_Def *wifis);
MyRes ConnectWifi(WIFI_Def *wifis);
MyRes RestartWifi(void);
MyRes GetWifiStaMac(char *mac);
MyRes GetWifiStaIP(IP * ip);
MyRes CheckWifiMID(void);
unsigned char GetWifiIndicator(void);
MyRes WifiIsConnectted(char * ssid);
MyRes closeWifiServerA(void);
MyRes SetWifiServerInfo(const SystemSetData * systemSetData);
#endif

