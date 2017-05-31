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
	Operator 		operator;							//������
	char 			sampleid[MaxSampleIDLen];			//�������
	QRCode 			qrCode;								//��ά����Ϣ
	AdjustData 		adjustData;							//У׼����
	TestSeries 		testSeries;							//��������
	DateTime 		testDateTime;						//����ʱ��
	Temperature 	temperature;						//�����¶�
	unsigned short 	time;								//��ʱʱ��
	ResultState 	testResultDesc;						//���Խ��״̬
	unsigned short 	crc;
}TestData;
#pragma pack()

#endif

/****************************************end of file************************************************/
