/****************************************file start****************************************************/
#ifndef _DEVICEERRORDAO_D_H
#define	_DEVICEERRORDAO_D_H

#include	"Define.h"
#include	"DeviceError.h"

MyState_TypeDef writeDeviceErrorToFile(DeviceError * deviceError);
MyState_TypeDef readDeviceErrorFromFile(DeviceErrorReadPackge * deviceErrorReadPackge);
MyState_TypeDef plusDeviceErrorHeaderUpLoadIndexToFile(unsigned int index);
MyState_TypeDef deleteDeviceErrorFile(void);

#endif

/****************************************end of file************************************************/
