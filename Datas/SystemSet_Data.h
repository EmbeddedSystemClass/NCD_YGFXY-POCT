/****************************************file start****************************************************/
#ifndef SYSTEMSET_D_H
#define	SYSTEMSET_D_H

#include	"Define.h"
#include	"Device.h"
#include	"AdjustData.h"
#include	"NetWork.h"

#pragma pack(1)
typedef struct SystemSetData_Tag
{		
	Device device;														//设备信息
	WireNetSet wireNetSet;												//有线网设置
	ServerSet serverSet;												//服务器设置
	bool isAutoPrint;													//是否自动打印
	bool isMute;														//是否静音
	unsigned short ledSleepTime;										//休眠时间 s
	unsigned char ledLightIntensity;									//屏幕亮度 0-100
	unsigned int testDataNum;											//测试数据已保存的数目
	unsigned int upLoadIndex;											//测试数据等待上传的索引
	unsigned short testLedLightIntensity;								//测试时采用的led亮度值，为校准后产生，默认为200
	AdjustData adjustData[MaxAdjustItemNum];
	unsigned char parm1[512];											//预留512字节的参数位置
	unsigned short crc;
}SystemSetData;
#pragma pack()

#define	SystemSetDataStructSize		sizeof(SystemSetData)
#define SystemSetDataStructCrcSize	SystemSetDataStructSize - 2

void setDefaultSystemSetData(SystemSetData * systemSetData);
const SystemSetData * getGBSystemSetData(void);
void upDateSystemSetData(SystemSetData * systemSetData);
MyState_TypeDef addAdjPram(SystemSetData * systemSetData, AdjustData * adjData);
void getAdjPram(const SystemSetData * systemSetData, AdjustData * adjData);
void setIsShowRealValue(bool isShow);
bool IsShowRealValue(void);

#endif

/****************************************end of file************************************************/
