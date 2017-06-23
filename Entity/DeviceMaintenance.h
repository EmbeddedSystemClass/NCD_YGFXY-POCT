/****************************************file start****************************************************/
#ifndef _DEVICEMAINTENANCE_E_H
#define	_DEVICEMAINTENANCE_E_H

#include	"DateTime.h"
#include	"Operator.h"
#include	"Define.h"

#define		DeviceMaintenanceRecordPageShowNum	8

#pragma pack(1)
typedef struct
{
	DateTime dateTime;
	Operator operator;
	bool isOk;
	char desc[50];
	unsigned short crc;
}DeviceMaintenance;
#pragma pack()

#pragma pack(1)
typedef struct
{
	PageRequest pageRequest;
	DeviceRecordHeader deviceRecordHeader;
	DeviceMaintenance deviceMaintenance[DeviceMaintenanceRecordPageShowNum];
	unsigned char readTotalNum;							//读取到的有效数据数目
}DeviceMaintenanceReadPackge;
#pragma pack()

#define	DeviceMaintenanceStructSize		sizeof(DeviceMaintenance)							//最多保存的用户数目
#define	DeviceMaintenanceStructCrcSize	DeviceMaintenanceStructSize - 2						//最多保存的用户数目



#define	DeviceMaintenanceReadPackgeStructSize		sizeof(DeviceMaintenanceReadPackge)								//最多保存的用户数目

#endif

/****************************************end of file************************************************/
