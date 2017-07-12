/****************************************file start****************************************************/
#ifndef _DEVICEERRORDAO_D_H
#define	_DEVICEERRORDAO_D_H

#include	"Define.h"
#include	"DeviceError.h"

MyRes writeDeviceErrorToFile(DeviceError * deviceError);
MyRes readDeviceErrorFromFile(DeviceErrorReadPackge * deviceErrorReadPackge);
MyRes plusDeviceErrorHeaderuserUpLoadIndexToFile(unsigned int index);
MyRes deleteDeviceErrorFile(void);

#endif

/****************************************end of file************************************************/
