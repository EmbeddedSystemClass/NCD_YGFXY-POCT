/****************************************file start****************************************************/
#ifndef _DEVICEADJUSTDAO_D_H
#define	_DEVICEADJUSTDAO_D_H

#include	"Define.h"
#include	"DeviceAdjust.h"

MyRes writeDeviceAdjustToFile(DeviceAdjust * deviceAdjust);
MyRes readDeviceAdjustFromFile(DeviceAdjustReadPackge * deviceAdjustReadPackge);
MyRes plusDeviceAdjustHeaderuserUpLoadIndexToFile(unsigned int index);
MyRes deleteDeviceAdjustFile(void);

#endif

/****************************************end of file************************************************/
