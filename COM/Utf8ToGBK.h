#ifndef _UTF8TOGBK_H_
#define _UTF8TOGBK_H_

#include	"Define.h"

MyState_TypeDef utf8ConvertToGBK(void * utf8Str, unsigned short utf8StrLen, void * gbkBuffer, unsigned short * gbkStrLen);

#endif


