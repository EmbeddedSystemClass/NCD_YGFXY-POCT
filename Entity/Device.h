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
	char deviceid[DeviceIdLen];													//�豸id
	Operator operator;															//������
	char department[DeviceDepartmentLen];										//�豸����
	  char addr[DeviceAddrLen];													//�豸��ַ
	unsigned int modifyTimeStamp;												//��ǰ������豸��Ϣ��ʱ���������ȷ�����С��˵���������и��£����ȡ
}Device;
#pragma pack()

#define	DeviceStructSize	sizeof(Device)

#endif

/****************************************end of file************************************************/

