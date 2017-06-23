/****************************************file start****************************************************/
#ifndef _DEVICEMAINTENANCEDAO_D_H
#define	_DEVICEADJUSTDAO_D_H

#include	"Define.h"
#include	"DeviceMaintenance.h"

MyState_TypeDef writeDeviceMaintenanceToFile(DeviceMaintenance * deviceMaintenance);
MyState_TypeDef readDeviceMaintenanceFromFile(DeviceMaintenanceReadPackge * deviceMaintenanceReadPackge);
MyState_TypeDef plusDeviceMaintenanceHeaderUpLoadIndexToFile(unsigned int index);
MyState_TypeDef deleteDeviceMaintenanceFile(void);

#endif

/****************************************end of file************************************************/
