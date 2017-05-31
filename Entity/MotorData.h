/****************************************file start****************************************************/
#ifndef _MOTORDATA_E_H
#define	_MOTORDATA_E_H

typedef enum
{ 
	NotLimitted = 0,						//没限位
	Limitted = 1							//限位
}LimitStatus;

typedef enum
{ 
	OnLine = 0,										//电机在线
	OffLine	= 1										//电机离线
}DRVSleep;

typedef enum
{ 
	Forward = 1,									//电机正向
	Reverse	= 0										//电机反向
}DRVDir;

typedef enum
{ 
	LowPower = 1,									//电机低功耗
	NonamalPower = 0								//电机正常功耗
}DRVPower;

#pragma pack(1)
typedef struct MotorData_Tag
{
	unsigned short location;									//电机当前位置
	unsigned short targetLocation;								//目标位置
	DRVDir motorDir;												//电机方向
}MotorData;
#pragma pack()

#endif

/****************************************end of file************************************************/
