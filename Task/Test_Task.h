/****************************************file start****************************************************/
#ifndef	_TEST_T_H
#define	_TEST_T_H

#include	"Define.h"
#include	"TestData.h"


char StartvTestTask(void);

MyState_TypeDef StartTest(TestData * parm);

MyState_TypeDef TakeTestResult(ResultState *testsult);

void clearTestResult(void);
#endif

/****************************************end of file************************************************/
