/****************************************file start****************************************************/
#ifndef _TESTDATA_D_H
#define	_TESTDATA_D_H

#include	"Define.h"
#include	"SystemSet_Data.h"
#include	"TestData.h"

#define	DataShowNumInPage		8							//һҳ8������

typedef enum
{
	DESC = 0,												//����
	ASC = 1													//����
}OrderByEnum;

//��ȡ������Ϣ
#pragma pack(1)
typedef struct PageRequest_tag {
	unsigned int startElementIndex;											//��ʼ��ȡ������0Ϊ��һ��
	unsigned int pageSize;													//ÿҳ����Ŀ
	OrderByEnum orderType;													//����ʽ
	unsigned short crc;
}PageRequest;
#pragma pack()

//��ȡ��������
#pragma pack(1)
typedef struct Page_tag {
	unsigned short ElementsSize;											//��ȡ����������Ŀ
	TestData testData[DataShowNumInPage];									//��ȡ��������
	unsigned short crc;
}Page;
#pragma pack()


MyState_TypeDef WriteTestData(TestData * testdata, unsigned int writeIndex);
MyState_TypeDef ReadTestData(PageRequest * pageRequest, Page * page, SystemSetData * systemSetData);

#endif

/****************************************end of file************************************************/
