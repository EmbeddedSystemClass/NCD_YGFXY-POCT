/****************************************file start****************************************************/
#ifndef _DEVICEQUALITYDAO_D_H
#define	_DEVICEQUALITYDAO_D_H

#include	"Define.h"
#include	"DeviceQuality.h"

MyRes writeDeviceQualityToFile(DeviceQuality * deviceQuality);
MyRes readDeviceQualityFromFile(DeviceQualityReadPackge * readPackge);
MyRes plusDeviceQualityHeaderuserUpLoadIndexToFile(unsigned int index);
MyRes deleteDeviceQualityFile(void);

#endif

/****************************************end of file************************************************/
