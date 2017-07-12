/****************************************file start****************************************************/
#ifndef _DEVICEMAINTENANCEDAO_D_H
#define	_DEVICEMAINTENANCEDAO_D_H

#include	"Define.h"
#include	"DeviceMaintenance.h"

MyRes writeDeviceMaintenanceToFile(DeviceMaintenance * deviceMaintenance);
MyRes readDeviceMaintenanceFromFile(DeviceMaintenanceReadPackge * deviceMaintenanceReadPackge);
MyRes plusDeviceMaintenanceHeaderuserUpLoadIndexToFile(unsigned int index);
MyRes deleteDeviceMaintenanceFile(void);

#endif

/****************************************end of file************************************************/
