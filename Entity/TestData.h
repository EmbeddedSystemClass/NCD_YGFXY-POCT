/****************************************file start****************************************************/
#ifndef _TESTDATA_E_H
#define	_TESTDATA_E_H

#include	"Operator.h"
#include	"QRCode.h"
#include	"AdjustData.h"
#include	"DateTime.h"
#include	"Define.h"
#include	"Temperature.h"

#define	TestDataRecordPageShowNum		8							//一页8个数据

#pragma pack(1)
typedef struct TestSeries_tag {
	unsigned short TestPoint[MaxPointLen];
	unsigned short C_Point[2];
	unsigned short T_Point[2];
	unsigned short B_Point[2];
	float BasicBili;
	float BasicResult;
	float AdjustResult;
} TestSeries;
#pragma pack()

#pragma pack(1)
typedef struct TestData_tag {
	Operator 		operator;							//操作人
	char 			sampleid[MaxSampleIDLen];			//样本编号
	QRCode 			qrCode;								//二维码信息
	TestSeries 		testSeries;
	AdjustData 		adjustData;							//校准数据
	DateTime 		testDateTime;						//测试时间
	Temperature 	temperature;						//测试温度
	unsigned short 	time;								//超时时间
	ResultState 	testResultDesc;						//测试结果状态
	unsigned short 	crc;
}TestData;
#pragma pack()


#pragma pack(1)
typedef struct
{
	PageRequest pageRequest;
	DeviceRecordHeader deviceRecordHeader;
	TestData testData[TestDataRecordPageShowNum];
	unsigned char readTotalNum;							//读取到的有效数据数目
}TestDataRecordReadPackage;
#pragma pack()

#define	TestSeriesStructSize	sizeof(TestSeries)

#define	TestDataStructSize		sizeof(TestData)
#define	TestDataStructCrcSize	TestDataStructSize - 2

#define	TestDataRecordReadPackageStructSize		sizeof(TestDataRecordReadPackage)
#define	TestDataRecordReadPackageStructCrcSize	TestDataRecordReadPackageStructSize - 2
	
#endif

/****************************************end of file************************************************/
