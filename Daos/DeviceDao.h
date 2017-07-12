/****************************************file start****************************************************/
#ifndef _DEVICEDAO_D_H
#define	_DEVICEDAO_D_H

#include	"Define.h"
#include	"Operator.h"
#include	"Device.h"
#include	"MyLock.h"

MyRes SaveDeviceToFile(Device * device);
MyRes ReadDeviceFromFile(Device * device);
MyRes deleteDeviceFile(void);
MyLock * getMyDeviceLock(void);

#endif

/****************************************end of file************************************************/
