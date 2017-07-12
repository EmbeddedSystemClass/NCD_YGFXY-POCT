/****************************************file start****************************************************/
#ifndef _RECORDDATA_D_H
#define	_RECORDDATA_D_H

#include	"Define.h"

MyRes writeRecordDataToFile(const char * fileName, void * recordData, unsigned int recordItemSize);
MyRes readRecordDataFromFile(const char * fileName, PageRequest * pageRequest, DeviceRecordHeader * deviceRecordHeader, 
	void * recordData1, void * recordData2, unsigned short recordItemSize);
MyRes plusRecordDataHeaderUpLoadIndexToFile(const char * fileName, unsigned char userIndexPlusNum, unsigned char ncdIndexPlusNum);
MyRes deleteRecordDataFile(const char * fileName);

#endif

/****************************************end of file************************************************/
