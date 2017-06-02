/****************************************file start****************************************************/
#ifndef _DEVICEDAO_D_H
#define	_DEVICEDAO_D_H

#include	"Define.h"
#include	"Operator.h"
#include	"Device.h"

MyState_TypeDef SaveDeviceToFile(Device * device);
MyState_TypeDef ReadDeviceFromFile(Device * device);
MyState_TypeDef deleteDeviceFile(void);

#endif

/****************************************end of file************************************************/
