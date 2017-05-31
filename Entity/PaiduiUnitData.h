/****************************************file start****************************************************/
#ifndef _PAIDUIUNITDATA_E_H
#define	_PAIDUIUNITDATA_E_H

#include	"TestData.h"
#include	"DateTime.h"
#include	"Define.h"

#pragma pack(1)
typedef struct PaiduiUnitData_tag {
	TestData testData;
	Timer timer;																	//常规倒计时计时器
	Timer timer2;																	//超时倒计时计时器
	Timer timer3;																	//超时倒计时计时器
	unsigned char errorcount;														//未操作次数
	MyPaiDuiStatues statues;														//测试阶段
	unsigned char jieduan;															//测试阶段
	unsigned char testlocation;
	unsigned short ledLight;														//测试时led的亮度，为系统设置中保存的校准led值
}PaiduiUnitData;
#pragma pack()

#endif

/****************************************end of file************************************************/
