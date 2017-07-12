#ifndef JSONTOOBJECT_H_H
#define JSONTOOBJECT_H_H

#include	"Define.h"
#include	"DateTime.h"
#include	"Device.h"
#include	"Operator.h"
#include	"RemoteSoftInfo.h"
#include	"CJson.h"

MyRes ParseJsonToDateTime(const char * jsonStr, DateTime * dateTime);
MyRes ParseJsonToDevice(const char * jsonStr, Device * device);
MyRes ParseJsonToOperator(cJSON * json, Operator * opeartor);
MyRes ParseJsonToRemoteSoftInfo(const char * jsonStr, RemoteSoftInfo * remoteSoftInfo);

#endif

