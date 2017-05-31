/****************************************file start****************************************************/
#ifndef _MOTORDATA_E_H
#define	_MOTORDATA_E_H

typedef enum
{ 
	NotLimitted = 0,						//û��λ
	Limitted = 1							//��λ
}LimitStatus;

typedef enum
{ 
	OnLine = 0,										//�������
	OffLine	= 1										//�������
}DRVSleep;

typedef enum
{ 
	Forward = 1,									//�������
	Reverse	= 0										//�������
}DRVDir;

typedef enum
{ 
	LowPower = 1,									//����͹���
	NonamalPower = 0								//�����������
}DRVPower;

#pragma pack(1)
typedef struct MotorData_Tag
{
	unsigned short location;									//�����ǰλ��
	unsigned short targetLocation;								//Ŀ��λ��
	DRVDir motorDir;												//�������
}MotorData;
#pragma pack()

#endif

/****************************************end of file************************************************/
