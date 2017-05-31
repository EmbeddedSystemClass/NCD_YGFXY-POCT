/****************************************file start****************************************************/
#ifndef _RETESTDATA_E_H
#define	_RETESTDATA_E_H

#include	"PaiduiUnitData.h"
#include	"DateTime.h"
#include	"Define.h"

typedef struct ReTestData_tag{
	unsigned int retestcount;								//总次数
	unsigned short retestsurpluscount;						//剩余测试次数
	unsigned short retestedcount;							//已经测试次数
	unsigned char reteststatus;								//老化测试状态，0停止，非0测试
	unsigned char retestStep;								//测试步骤，1等待插卡，2读二维码，3读卡
	Timer retesttimer;										//老化测试计时器
	Timer oneretesttimer;									//一次老化测试计时器
	PaiduiUnitData paiduiUnitData;
	char result[30];										//当前测试的结论
	unsigned short playcount;								//老化播放计数器，记录播放次数
	Timer playtimer;										//老化音频总时间	
	Timer oneplaytimer;										//老化播放计时器，记录每次播放的时长
	DateTime startplayTime;								//当前音频起始时间
	DateTime endplayTime;									//当前音频结束时间
	double advalue1;										//DA值100时的AD值
	double advalue2;										//DA值200时的AD值
	double advalue3;										//DA值300时的AD值
	MyLEDCheck_TypeDef ledstatus;							//发光模块状态
	unsigned char playstatus;								//播放状态
}ReTestData;

#endif

/****************************************end of file************************************************/
