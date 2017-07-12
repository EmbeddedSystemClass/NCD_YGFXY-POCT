/****************************************file start****************************************************/
#ifndef	_TEST_T_H
#define	_TEST_T_H

#include	"Define.h"
#include	"TestData.h"


char StartvTestTask(void);

MyRes StartTest(TestData * parm);

MyRes TakeTestResult(ResultState *testsult);

void clearTestResult(void);
#endif

/****************************************end of file************************************************/
