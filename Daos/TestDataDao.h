/****************************************file start****************************************************/
#ifndef _TESTDATA_D_H
#define	_TESTDATA_D_H

#include	"Define.h"
#include	"SystemSet_Data.h"
#include	"TestData.h"



MyState_TypeDef writeTestDataToFile(TestData * testData);
MyState_TypeDef readTestDataFromFile(TestDataRecordReadPackage * testDataRecordReadPackage);
MyState_TypeDef plusTestDataHeaderUpLoadIndexToFile(unsigned int index);
MyState_TypeDef deleteTestDataFile(void);

#endif

/****************************************end of file************************************************/
