#ifndef OUTM_F_H_H
#define OUTM_F_H_H

#include	"TM1623_Driver.h"
#include 	"FreeRTOS.h"

/*����һ���Ŷ�λ�ĵƵ���˸״̬��Ŀǰֻ֧��һ����ɫ��˸*/
typedef struct
{ 
	TM1623_LED_State LED_Statues;	//led��ʾ����ɫ
	TM1623_LED_State LEN_OffStatus;	//led�����ɫ
	TM1623_KEY_State Key_Statues;	//����״̬
	unsigned char Time;				//led��˸���ʱ��. 0��ʾ����
}OneModelData_DefType;

typedef struct
{ 
	OneModelData_DefType OneModel_Data[9];		//��9���Ŷ�λ��
	unsigned int T_Count;						//��ʱ
}ModelData_DefType;

typedef struct
{ 
	TM1623_KEY_State keystatues;				//״̬�ı�Ϊ
	unsigned char index;						//�ı�״̬�İ�������
}KeyChange_Def;

void UpOneModelData(unsigned char modelindex, TM1623_LED_State ledstatues, TM1623_LED_State ledOffStatues, unsigned char time);
ModelData_DefType *GetGBModelData(void);
void OutModel_Init(void);
void TimeCountIncreme(void);
void ToggleLedStatues(unsigned char ledindex);

void ChangeOutModelStatues(void);

TM1623_KEY_State GetKeyStatues(unsigned char index);
#endif


