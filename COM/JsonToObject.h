#ifndef JSONTOOBJECT_H_H
#define JSONTOOBJECT_H_H

#include	"Define.h"
#include	"DateTime.h"
#include	"Device.h"
#include	"Operator.h"
#include	"CJson.h"

MyState_TypeDef ParseJsonToDateTime(const char * jsonStr, DateTime * dateTime);
MyState_TypeDef ParseJsonToDevice(const char * jsonStr, Device * device);
MyState_TypeDef ParseJsonToOperator(cJSON * json, Operator * opeartor);

#endif

