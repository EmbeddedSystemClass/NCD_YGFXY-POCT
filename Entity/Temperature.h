/****************************************file start****************************************************/
#ifndef _TEMPERATURE_E_H
#define	_TEMPERATURE_E_H

#pragma pack(1)
typedef struct Temperature_Tag
{
	float  E_Temperature;			//环境温度
	float	O_Temperature;			//被测物体温度
	float I_Temperature;			//内部温度
}Temperature;
#pragma pack()

#endif

/****************************************end of file************************************************/

