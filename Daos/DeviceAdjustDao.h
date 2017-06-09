/****************************************file start****************************************************/
#ifndef _DEVICEADJUSTDAO_D_H
#define	_DEVICEADJUSTDAO_D_H

#include	"Define.h"
#include	"DeviceAdjust.h"

MyState_TypeDef writeDeviceAdjustToFile(DeviceAdjust * deviceAdjust);
MyState_TypeDef readDeviceAdjustFromFile(DeviceAdjustReadPackge * deviceAdjustReadPackge);
MyState_TypeDef plusDeviceAdjustHeaderUpLoadIndexToFile(unsigned int index);
MyState_TypeDef deleteDeviceAdjustFile(void);

#endif

/****************************************end of file************************************************/
