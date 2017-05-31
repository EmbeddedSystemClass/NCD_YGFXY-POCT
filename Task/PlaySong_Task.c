/******************************************************************************************
*�ļ�����COM_Task.c
*�������������Ź�����
*˵�������ȼ�1����ͣ���1Sι��һ�Σ�
*���ߣ�xsx
******************************************************************************************/

#include	"PlaySong_Task.h"
#include	"PlaySong_Function.h"
#include	"QueueUnits.h"
#include	"System_Data.h"
#include	"SystemSet_Data.h"

#include 	"FreeRTOS.h"
#include 	"task.h"

#include	"string.h"
#include	"stdio.h"
/******************************************************************************************/
/*****************************************�ֲ���������*************************************/
const char wavfilename[59][20]=
{
	"0:/Audio/n1.wav\0",
	"0:/Audio/n2.wav\0",
	"0:/Audio/n3.wav\0",
	"0:/Audio/n4.wav\0",
	"0:/Audio/n5.wav\0",
	"0:/Audio/n6.wav\0",
	"0:/Audio/n7.wav\0",
	"0:/Audio/n8.wav\0",
	"0:/Audio/n9.wav\0",
	"0:/Audio/n10.wav\0",
	"0:/Audio/n11.wav\0",
	"0:/Audio/n12.wav\0",
	"0:/Audio/n13.wav\0",
	"0:/Audio/n14.wav\0",
	"0:/Audio/n15.wav\0",
	"0:/Audio/n16.wav\0",
	"0:/Audio/n17.wav\0",
	"0:/Audio/n18.wav\0",
	"0:/Audio/n19.wav\0",
	"0:/Audio/n20.wav\0",
	"0:/Audio/n21.wav\0",
	"0:/Audio/n22.wav\0",
	"0:/Audio/n23.wav\0",
	"0:/Audio/n24.wav\0",
	"0:/Audio/n25.wav\0",
	"0:/Audio/n26.wav\0",
	"0:/Audio/n27.wav\0",
	"0:/Audio/n28.wav\0",
	"0:/Audio/n29.wav\0",
	"0:/Audio/n30.wav\0",
	"0:/Audio/n31.wav\0",
	"0:/Audio/n32.wav\0",
	"0:/Audio/n33.wav\0",
	"0:/Audio/n34.wav\0",
	"0:/Audio/n35.wav\0",
	"0:/Audio/n36.wav\0",
	"0:/Audio/n37.wav\0",
	"0:/Audio/n38.wav\0",
	"0:/Audio/n39.wav\0",
	"0:/Audio/n40.wav\0",
	"0:/Audio/n41.wav\0",
	"0:/Audio/n42.wav\0",
	"0:/Audio/n43.wav\0",
	"0:/Audio/n44.wav\0",
	"0:/Audio/n45.wav\0",
	"0:/Audio/n46.wav\0",
	"0:/Audio/n47.wav\0",
	"0:/Audio/n48.wav\0",
	"0:/Audio/n49.wav\0",
	"0:/Audio/n50.wav\0",
	"0:/Audio/n51.wav\0",
	"0:/Audio/n52.wav\0",
	"0:/Audio/n53.wav\0",
	"0:/Audio/n54.wav\0",
	"0:/Audio/n55.wav\0",
	"0:/Audio/n56.wav\0",
	"0:/Audio/n57.wav\0",
	"0:/Audio/n58.wav\0",
	"0:/Audio/n59.wav\0"
};
#define vPlaySongTask_PRIORITY			( ( unsigned portBASE_TYPE ) 2U )

#define	SongListSize	20							//�����б�ɱ���100����Ƶ
static xQueueHandle SongListQueue = NULL;			//������Ƶ�Ķ���
static unsigned char S_PlayStatus = 0;
static DateTime tempTime;							//ϵͳʱ��
/******************************************************************************************/
/*****************************************�ֲ���������*************************************/

static void vPlaySongTask( void *pvParameters );

/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

/************************************************************************
** ������:StartvIwdgTask
** ��  ��:����������ι������
** ��  ��:��
** ��  ��:��
** ��  �أ���
** ��  ע����
** ʱ  ��: 2015��5��15�� 17:04:43 
** ��  �ߣ�xsx                                                 
************************************************************************/
char StartvPlaySongTask(void)
{
	if(SongListQueue == NULL)
		SongListQueue = xQueueCreate( SongListSize, ( unsigned portBASE_TYPE ) sizeof( unsigned char ) );
	
	return xTaskCreate( vPlaySongTask, ( const char * ) "vPlaySongTask ", configMINIMAL_STACK_SIZE*2, NULL, vPlaySongTask_PRIORITY, NULL );
}

/************************************************************************
** ������:vIwdgTask
** ��  ��:ι������
** ��  ��:��
** ��  ��:��
** ��  �أ���
** ��  ע����
** ʱ  ��: 2015��5��15�� 17:04:12  
** ��  �ߣ�xsx                                                 
************************************************************************/
static void vPlaySongTask( void *pvParameters )
{
	unsigned char tempdata;
	while(1)
	{
		if(pdPASS == xQueueReceive( SongListQueue, &tempdata, portMAX_DELAY ))
		{
			//������ʼ�����ź�
			S_PlayStatus = 1;
			
			AudioPlay(wavfilename[tempdata]);
			
			//����ֹͣ�����ź�
			S_PlayStatus = 0;
		}
	}
}

/***************************************************************************************************
*FunctionName��AddNumOfSongToList
*Description����Ӹ����������б������Ƶģ����У�����������
*Input��num -- ������ţ���������ļ��������Ӧ
*Output��������ӳɹ���ʧ��
*Author��xsx
*Data��2015��10��13��09:06:23
***************************************************************************************************/
unsigned char AddNumOfSongToList(unsigned char num, unsigned char mode)
{
	unsigned char songnum = num;
	
	memcpy(&tempTime, &(getSystemRunTimeData()->systemDateTime), sizeof(DateTime));
	
	if((tempTime.hour < 8) || (tempTime.hour >= 17))
		return pdPASS;
	
	if(getGBSystemSetData()->isMute)
		return pdPASS;
	
	if(SongListQueue == NULL)
		return pdFAIL;

	/*ģʽ1�� ��ֹ���ڲ��ţ���������Ƶ*/
	if(mode == 0)
	{
		StopMyPlay();
	}
	/*ģʽ2��������ڲ��ţ���ȡ������Ƶ*/
	else if(mode == 1)
	{
		return pdPASS;
	}
	/*ģʽ3�����ʣ��Ĳ���*/
	else if(mode == 2)
	{
		while(pdPASS == xQueueReceive( SongListQueue, &songnum, 0 ));
	}
	//ģʽ4������ʲô״̬�����������Ƶ���ȴ�����*/
	else
	{
		
	}
	
	songnum = num;
	if( xQueueSend( SongListQueue, &songnum, 0 ) != pdPASS )
	{
		return pdFAIL;
	}

	return pdPASS;
}

/***************************************************************************************************
*FunctionName: stopPlay
*Description: ֹͣ����
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017��1��20��14:58:57
***************************************************************************************************/
void stopPlay(void)
{
	StopMyPlay();
}

/***************************************************************************************************
*FunctionName: getPlayStatus
*Description: ��ȡ����״̬
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017��1��20��14:59:13
***************************************************************************************************/
unsigned char getPlayStatus(void)
{
	return S_PlayStatus;
}
