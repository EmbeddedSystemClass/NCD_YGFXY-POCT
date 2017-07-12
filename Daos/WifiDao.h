/****************************************file start****************************************************/
#ifndef _WIFI_D_H
#define	_WIFI_D_H

#include	"Define.h"

MyRes SaveWifiData(WIFI_Def * wifi);
MyRes ReadWifiData(WIFI_Def * wifi);
MyRes deleteWifi(WIFI_Def * wifi);
MyRes ClearWifi(void);
#endif

/****************************************end of file************************************************/
