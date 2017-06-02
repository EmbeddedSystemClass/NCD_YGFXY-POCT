/****************************************file start****************************************************/
#ifndef _DEVICEDAO_D_H
#define	_DEVICEDAO_D_H

#include	"Define.h"
#include	"Operator.h"
#include	"DeviceAdjust.h"

MyState_TypeDef SaveDeviceAdjustToFile(DeviceAdjust * deviceAdjust);
MyState_TypeDef ReadDeviceAdjustFromFile(DeviceAdjust * deviceAdjust);
MyState_TypeDef deleteDeviceAdjustFile(void);

#endif

/****************************************end of file************************************************/
