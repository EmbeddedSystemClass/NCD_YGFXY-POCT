#ifndef __RECORD_P_H__
#define __RECORD_P_H__

#include	"MyTest_Data.h"
#include	"TestDataDao.h"
#include	"UI_Data.h"
#include	"SystemSet_Data.h"

typedef struct Record_tag {

	unsigned char selectindex;							//选中的索引
	unsigned int pageindex;								//页面索引
	unsigned int maxpagenum;
	
	PageRequest pageRequest;							//请求参数
	DeviceRecordHeader deviceRecordHeader;				//测试数据头信息
	TestData testData[TestDataRecordPageShowNum];		//读取到的测试数据
	unsigned char readTotalNum;							//读取到的有效数据数目
	
	char buf[300];										//临时缓冲区
	char tempBuf[100];										//临时缓冲区
	unsigned int tempvalue1;
	unsigned int tempvalue2;
	TestData *tempdata;
	unsigned short lcdinput[100];
	SystemSetData systemSetData;
}RecordPageBuffer;

MyRes createRecordActivity(Activity * thizActivity, Intent * pram);

#endif

