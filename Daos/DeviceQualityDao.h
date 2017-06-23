/****************************************file start****************************************************/
#ifndef _DEVICEQUALITYDAO_D_H
#define	_DEVICEQUALITYDAO_D_H

#include	"Define.h"
#include	"DeviceQuality.h"

MyState_TypeDef writeDeviceQualityToFile(DeviceQuality * deviceQuality);
MyState_TypeDef readDeviceQualityFromFile(DeviceQualityReadPackge * readPackge);
MyState_TypeDef plusDeviceQualityHeaderUpLoadIndexToFile(unsigned int index);
MyState_TypeDef deleteDeviceQualityFile(void);

#endif

/****************************************end of file************************************************/
