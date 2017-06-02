/****************************************file start****************************************************/
#ifndef _DEVICEADJUST_E_H
#define	_DEVICEADJUST_E_H

#include	"DateTime.h"
#include	"Operator.h"

#pragma pack(1)
typedef struct
{
	float normalv;
	float measurev;
	DateTime dateTime;
	Operator operator;
	char result[20];
	unsigned short crc;
}DeviceAdjust;
#pragma pack()

#define	DeviceAdjustStructSize		sizeof(DeviceAdjust)							//最多保存的用户数目
#define	DeviceAdjustStructCrcSize	DeviceAdjustStructSize - 2						//最多保存的用户数目

#endif

/****************************************end of file************************************************/
