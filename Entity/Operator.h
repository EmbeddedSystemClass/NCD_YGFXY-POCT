/****************************************file start****************************************************/
#ifndef _OPERATOR_E_H
#define	_OPERATOR_E_H

#define	OperatorNameLen	20

#pragma pack(1)
typedef struct
{
	unsigned short id;
	char name[OperatorNameLen];
	char age[10];
	char sex[10];
	char phone[20];
	char job[30];
	char department[20];
	unsigned short crc;
}Operator;
#pragma pack()

#define	MaxOperatorSize	20														//最多保存的用户数目
#define	MaxPageShowOperatorSize	5												//一页显示用户数目
#define OneOperatorStructSize	sizeof(Operator)								//一个操作人结构体大小
#define	OneOperatorStructSizeWithOutCrc	OneOperatorStructSize - 2					//一个操作人结构体大小,不包含crc
#define	AllOperatorStructSize	OneOperatorStructSize * MaxOperatorSize			//所有操作人大小

#endif

/****************************************end of file************************************************/
