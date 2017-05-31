/****************************************file start****************************************************/
#ifndef _TESTDATA_E_H
#define	_TESTDATA_E_H

#include	"Operator.h"
#include	"QRCode.h"
#include	"AdjustData.h"
#include	"DateTime.h"
#include	"Define.h"
#include	"Temperature.h"
#include	"TestSeries.h"

#pragma pack(1)
typedef struct TestData_tag {
	Operator 		operator;							//操作人
	char 			sampleid[MaxSampleIDLen];			//样本编号
	QRCode 			qrCode;								//二维码信息
	AdjustData 		adjustData;							//校准数据
	TestSeries 		testSeries;							//测试曲线
	DateTime 		testDateTime;						//测试时间
	Temperature 	temperature;						//测试温度
	unsigned short 	time;								//超时时间
	ResultState 	testResultDesc;						//测试结果状态
	unsigned short 	crc;
}TestData;
#pragma pack()

#endif

/****************************************end of file************************************************/
