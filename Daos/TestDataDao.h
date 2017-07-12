/****************************************file start****************************************************/
#ifndef _TESTDATA_D_H
#define	_TESTDATA_D_H

#include	"Define.h"
#include	"SystemSet_Data.h"
#include	"TestData.h"



MyRes writeTestDataToFile(TestData * testData);
MyRes readTestDataFromFile(PageRequest * pageRequest, DeviceRecordHeader * deviceRecordHeader, 
	void * recordData1, void * recordData2);
MyRes plusTestDataHeaderuserUpLoadIndexToFile(unsigned char userIndexPlusNum, unsigned char ncdIndexPlusNum);
MyRes deleteTestDataFile(void);

#endif

/****************************************end of file************************************************/
