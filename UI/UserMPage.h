#ifndef __USERM_P_H__
#define __USERM_P_H__

#include	"Define.h"
#include	"UI_Data.h"
#include	"SystemSet_Data.h"
#include	"Operator.h"

typedef struct UserMPageBuffer_tag {
	unsigned char selectindex;										//ѡ�е�����
	unsigned char pageindex;										//ҳ������
	unsigned char filemaxitem;							//��ǰ�ļ�����������Ŀ
	unsigned char filemaxpagenum;						//��ǰ�ļ��������ݵ�ҳ��
	Operator user[MaxOperatorSize];									//ѡ�е���
	Operator * tempuser;
	Operator tempnewuser;
	unsigned short lcdinput[100];
	char buf[100];
}UserMPageBuffer;

MyState_TypeDef createUserManagerActivity(Activity * thizActivity, Intent * pram);

#endif

