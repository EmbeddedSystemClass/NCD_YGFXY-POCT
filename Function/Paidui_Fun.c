/***************************************************************************************************
*FileName: Paidui_Fun
*Description: �Ŷӹ���
*Author: xsx_kair
*Data: 2016��12��13��11:47:20
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"Paidui_Fun.h"
#include	"OutModel_Fun.h"
#include	"System_Data.h"

#include	"CardLimit_Driver.h"

#include	"PlaySong_Task.h"

#include	"PaiDuiPage.h"
#include	"PreReadCardPage.h"
#include	"UI_Data.h"
#include	"MyTest_Data.h"
#include	"MyTools.h"
#include	"Timer_Data.h"

#include	<string.h>
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName: PaiDuiHandler
*Description: �Ŷ����̿���
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��13��11:58:58
***************************************************************************************************/
void PaiDuiHandler(void)
{
	unsigned char index = 0;
	PaiduiUnitData * temp = NULL;
	unsigned short tempvalue = 0;
	
	for(index=0; index<PaiDuiWeiNum; index++)
	{
		temp = GetTestItemByIndex(index);
		
		if((temp) && (temp->statues >= status_start) && (temp->statues <= status_timeup) && (Connect_Ok == getSystemRunTimeData()->paiduiModuleStatus))
		{
			//�����Ŷ�ģʽ
			if(temp->statues == status_start)
			{
				if(MaxLocation == getSystemRunTimeData()->motorData.location)
				{
					temp->statues = status_outcard;
				
					UpOneModelData(index, R_OFF_G_ON, R_OFF_G_OFF, 5);
					//20S��ʾһ�ν��������Ŷ�λ
					timer_set(&(temp->timer3), 10);
					AddNumOfSongToList(index+22, 0);
				}
			}
			
			//������ڵ���ʱ
			if(isInTimeOutStatus(temp) == false)
			{
				tempvalue = timer_surplus(&(temp->timer));
				
				if(0 == tempvalue)
				{
					timer_restart(&(temp->timer2));				//������ʱ��ʱ��
					
					//�ȴ������Ŷ�λ
					if(temp->statues == status_in_n)
					{
						temp->statues = status_in_o;
						UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 5);
						AddNumOfSongToList(index+38, 0);
						
						if(temp == GetCurrentTestItem())
							SetCurrentTestItem(NULL);
					}
					//�ȴ��γ��Ŷ�λ
					else if(temp->statues == status_out_n)
					{
						temp->statues = status_timeup;
						UpOneModelData(index, R_ON_G_OFF, R_OFF_G_OFF, 0);
						AddNumOfSongToList(index+38, 0);
						
						if(temp == GetCurrentTestItem())
							SetCurrentTestItem(NULL);
					}
					//�ȴ����뿨��
					else if(temp->statues == status_incard_n)
					{
						temp->statues = status_in_o;
						UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 5);
						AddNumOfSongToList(index+38, 0);
						
						if(temp == GetCurrentTestItem())
							SetCurrentTestItem(NULL);
					}
					//����ʱ��
					else if(temp->statues == status_timedown)
					{
						temp->statues = status_timeup;
						UpOneModelData(index, R_ON_G_OFF, R_OFF_G_OFF, 0);
						AddNumOfSongToList(index+38, 0);
						
						if(temp == GetCurrentTestItem())
							SetCurrentTestItem(NULL);
					}
					else if(temp->statues == status_prereadagain_n)
					{
						temp->statues = status_prereadagain_o;
						timer_restart(&(temp->timer2));				//������ʱ��ʱ��
						AddNumOfSongToList(index+38, 0);
					}
				}
				else if(tempvalue <= 30)
				{
					//�ȴ��ӿ��۰γ�
					if(temp->statues == status_outcard)
					{
						UpOneModelData(index, R_OFF_G_ON, R_OFF_G_OFF, 0);
						temp->statues = status_prereadagain_n;

						startActivity(createPreReadCardActivity, NULL, NULL);
					}
					//�ȴ������Ŷ�λ
					else if(temp->statues == status_in_n)
					{
						if(GetCurrentTestItem() == NULL)									//�������
						{
							if(CardPinIn == CardIN)									//��������п�����ʾ���
							{
								if(TimeOut == timer_expired(&(temp->timer3)))
								{
									timer_restart(&(temp->timer3));
									AddNumOfSongToList(46, 2);								//��ʾ��տ���
								}
							}
							else
							{
								SetCurrentTestItem(temp);
								UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 1);
								temp->statues = status_incard_n;
								timer_restart(&(temp->timer3));
								AddNumOfSongToList(index+30, 0);
							}
						}
						else
						{
							//��������Ŷ�λ���л�����ʱ���߳�ʱ״̬
							if(KEY_Pressed == GetKeyStatues(index))
							{
								temp->statues = status_timedown;
								UpOneModelData(index, R_ON_G_OFF, R_OFF_G_OFF, 0);
							}
							else if(TimeOut == timer_expired(&(temp->timer3)))
							{
								timer_restart(&(temp->timer3));
								AddNumOfSongToList(index+22, 0);
							}
						}
					}
					//�ȴ��γ��Ŷ�λ
					else if(temp->statues == status_out_n)
					{
						//��������п�����ʾ���
						if(CardPinIn == CardIN)
						{
							if(TimeOut == timer_expired(&(temp->timer3)))
							{
								timer_restart(&(temp->timer3));		
								AddNumOfSongToList(46, 2);					//��ʾ��տ���
							}
						}
						else
						{
							//����γ��Ŷ�λ
							if(KEY_NoPressed == GetKeyStatues(index))
							{
								UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 1);
								temp->statues = status_incard_n;
							}
							else if(TimeOut == timer_expired(&(temp->timer3)))
							{
								timer_restart(&(temp->timer3));			
								AddNumOfSongToList(index+30, 0);
							}
						}
					}
					//�ȴ����뿨��
					else if(temp->statues == status_incard_n)
					{
						if(CardPinIn == CardIN)
						{
							UpOneModelData(index, R_OFF_G_ON, R_OFF_G_OFF, 0);
							temp->statues = status_prereadagain_n;
							startActivity(createPreReadCardActivity, NULL, NULL);
						}
						else if(TimeOut == timer_expired(&(temp->timer3)))
						{
							UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 1);
							timer_restart(&(temp->timer3));
							AddNumOfSongToList(index+30, 5);
						}
					}
					//����ʱ��
					else if(temp->statues == status_timedown)
					{
						if(GetCurrentTestItem() == NULL)										//�������
						{
							if(CardPinIn == CardIN)										//��������п�����ʾ���
							{
								if(TimeOut == timer_expired(&(temp->timer3)))
								{
									timer_restart(&(temp->timer3));		
									AddNumOfSongToList(46, 2);					//��ʾ��տ���
								}
							}
							else
							{
								SetCurrentTestItem(temp);
								UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 1);
								temp->statues = status_out_n;
								timer_restart(&(temp->timer3));
								AddNumOfSongToList(index+30, 0);
							}
						}
						
						//����γ��Ŷ�λ
						if(KEY_NoPressed == GetKeyStatues(index))
						{
							temp->statues = status_in_n;
							UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 5);
							AddNumOfSongToList(index+22, 0);
							timer_restart(&(temp->timer3));
						}
					}
				}
				//ʱ��>30��
				else
				{
					//�ȴ��ӿ��۰γ�
					if(temp->statues == status_outcard)
					{
						//����ӿ��۰γ����򽫵�ǰ��������Ϊ�գ����Խ�����������
						if(CardPinIn == NoCard)
						{
							temp->statues = status_in_n;
							SetCurrentTestItem(NULL);
						}
						else if(TimeOut == timer_expired(&(temp->timer3)))
						{
							timer_restart(&(temp->timer3));
									
							AddNumOfSongToList(index+22, 0);
						}
					}
					//�ȴ������Ŷ�λ
					else if(temp->statues == status_in_n)
					{
						//��������Ŷ�λ���л�����ʱ���߳�ʱ״̬
						if(KEY_Pressed == GetKeyStatues(index))
						{
							//���뵹��ʱ״̬
							temp->statues = status_timedown;
								
							UpOneModelData(index, R_ON_G_OFF, R_OFF_G_OFF, 0);
						}
						else if(TimeOut == timer_expired(&(temp->timer3)))
						{
							timer_restart(&(temp->timer3));
									
							AddNumOfSongToList(index+22, 0);
						}
					}
					//�ȴ��γ��Ŷ�λ
					else if(temp->statues == status_out_n)
					{
						;
					}
					//�ȴ����뿨��
					else if(temp->statues == status_incard_n)
					{
						;
					}
					//����ʱ��
					else if(temp->statues == status_timedown)
					{
						//����γ��Ŷ�λ
						if(KEY_NoPressed == GetKeyStatues(index))
						{
							temp->statues = status_in_n;
							UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 5);
							AddNumOfSongToList(index+22, 0);
							timer_restart(&(temp->timer3));
						}
					}
				}
				
				if((tempvalue <= 40) && (GetCurrentTestItem() == NULL))
				{
					if(false == CheckStrIsSame(paiduiActivityName, getCurrentActivityName(), strlen(paiduiActivityName)))
					{
						backToActivity(lunchActivityName);
						startActivity(createPaiDuiActivity, NULL, NULL);
					}
				}
			}
			//��ʱ״̬
			else
			{
				tempvalue = GetMinWaitTime();
				
				//�ȴ������Ŷ�λ
				if(temp->statues == status_in_o)
				{
					//�п���������
					if((tempvalue > 40) && (NULL == GetCurrentTestItem()))
					{
						//��������п�����ʾ���
						if(CardPinIn == CardIN)
						{
							if(TimeOut == timer_expired(&(temp->timer3)))
							{
								timer_restart(&(temp->timer3));		
								AddNumOfSongToList(46, 2);					//��ʾ��տ���
							}
						}
						else
						{
							SetCurrentTestItem(temp);
							UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 1);
							temp->statues = status_incard_o;
							timer_restart(&(temp->timer3));
							AddNumOfSongToList(index+30, 0);
						}
					}
					else
					{
						//��������Ŷ�λ���л�����ʱ���߳�ʱ״̬
						if(KEY_Pressed == GetKeyStatues(index))
						{
							temp->statues = status_timeup;
							UpOneModelData(index, R_ON_G_OFF, R_OFF_G_OFF, 0);
						}
						else if(TimeOut == timer_expired(&(temp->timer3)))
						{
							timer_restart(&(temp->timer3));		
							AddNumOfSongToList(index+22, 0);
							UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 5);
						}
					}
				}
				//�ȴ��γ��Ŷ�λ
				else if(temp->statues == status_out_o)
				{
					//�п���������
					if(tempvalue <= 40)
					{
						if(temp == GetCurrentTestItem())
							SetCurrentTestItem(NULL);
						UpOneModelData(index, R_ON_G_OFF, R_OFF_G_OFF, 0);
						temp->statues = status_timeup;
					}
					else
					{
						//��������п�����ʾ���
						if(CardPinIn == CardIN)
						{
							if(TimeOut == timer_expired(&(temp->timer3)))
							{
								timer_restart(&(temp->timer3));		
								AddNumOfSongToList(46, 2);					//��ʾ��տ���
							}
						}
						else
						{
							//����γ��Ŷ�λ
							if(KEY_NoPressed == GetKeyStatues(index))
							{
								UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 1);
								temp->statues = status_incard_o;
							}
							else if(TimeOut == timer_expired(&(temp->timer3)))
							{
								timer_restart(&(temp->timer3));
								AddNumOfSongToList(index+30, 0);
							}
						}
					}
				}
				//�ȴ����뿨��
				else if(temp->statues == status_incard_o)
				{
					//�п���������
					if(tempvalue <= 40)
					{
						if(temp == GetCurrentTestItem())
							SetCurrentTestItem(NULL);

						UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 5);
						temp->statues = status_in_o;
						
						timer_restart(&(temp->timer3));		
						AddNumOfSongToList(index+22, 0);
					}
					else
					{
						if(CardPinIn == CardIN)
						{
							UpOneModelData(index, R_OFF_G_ON, R_OFF_G_OFF, 0);
							temp->statues = status_prereadagain_o;
							
							startActivity(createPreReadCardActivity, NULL, NULL);
						}
						else if(TimeOut == timer_expired(&(temp->timer3)))
						{
							timer_restart(&(temp->timer3));
							AddNumOfSongToList(index+30, 5);
						}
					}
				}
				//��ʱ��
				else if(temp->statues == status_timeup)
				{
					if((tempvalue > 40) && (NULL == GetCurrentTestItem()))
					{
						//��������п�����ʾ���
						if(CardPinIn == CardIN)
						{
							if(TimeOut == timer_expired(&(temp->timer3)))
							{
								timer_restart(&(temp->timer3));	
								AddNumOfSongToList(46, 2);					//��ʾ��տ���
							}
						}
						else
						{
							SetCurrentTestItem(temp);
							UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 1);
							temp->statues = status_out_o;
							timer_restart(&(temp->timer3));
							AddNumOfSongToList(index+30, 0);
						}
					}
					else
					{
						//����γ��Ŷ�λ
						if(KEY_NoPressed == GetKeyStatues(index))
						{
							UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 5);
							temp->statues = status_in_o;
							timer_restart(&(temp->timer3));		
							AddNumOfSongToList(index+22, 0);
						}
					}
				}
				
				if((tempvalue > 40) && (GetCurrentTestItem() == NULL))
				{
					if(false == CheckStrIsSame(paiduiActivityName, getCurrentActivityName(), strlen(paiduiActivityName)))
					{
						backToActivity(lunchActivityName);
						startActivity(createPaiDuiActivity, NULL, NULL);
					}
				}
			}
		}
	}
	
	if((IsPaiDuiTestting() == true) && (Connect_Error == getSystemRunTimeData()->paiduiModuleStatus))
	{
		//�����ǰ�����Ŷӽ���
		if(false == CheckStrIsSame(paiduiActivityName, getCurrentActivityName(), strlen(paiduiActivityName)))
		{
			//�����ǰû�ڲ���
			if(GetCurrentTestItem() == NULL)
			{
				backToActivity(lunchActivityName);
				startActivity(createPaiDuiActivity, NULL, NULL);
			}
		}
	}
}

/****************************************end of file************************************************/
