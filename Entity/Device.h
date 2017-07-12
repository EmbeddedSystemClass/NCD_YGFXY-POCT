/****************************************file start****************************************************/
#ifndef _DEVICE_E_H
#define	_DEVICE_E_H

#include	"Operator.h"

#define	DeviceIdLen	20
#define	DeviceDepartmentLen	30
#define	DeviceAddrLen	50

#pragma pack(1)
typedef struct Device_Tag
{
	Operator operator;															//责任人
	Operator operators[MaxOperatorSize];										//操作人
	char department[DeviceDepartmentLen];										//设备部门
	char addr[DeviceAddrLen];													//设备地址
	unsigned int modifyTimeStamp;												//当前保存的设备信息的时间戳，如果比服务器小，说明服务器有更新，需读取
	unsigned short crc;
}Device;
#pragma pack()

#define	DeviceStructSize		sizeof(Device)
#define	DeviceStructCrcSize		DeviceStructSize - 2
	
#endif

/****************************************end of file************************************************/

