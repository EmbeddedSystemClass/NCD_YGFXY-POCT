/***************************************************************************************************
*FileName: WifiDao
*Description: ����wifi����
*Author: xsx_kair
*Data: 2017��2��16��11:37:59
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"WifiDao.h"
#include	"MyTools.h"
#include	"CRC16.h"
#include	"MyMem.h"

#include	"ff.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"
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

MyState_TypeDef SaveWifiData(WIFI_Def * wifi)
{
	FatfsFileInfo_Def * myfile = NULL;
	WIFI_Def * wifiBuf = NULL;
	MyState_TypeDef statues = My_Fail;
	unsigned char i=0;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	wifiBuf = MyMalloc(sizeof(WIFI_Def) * MaxSaveWifiNum);
	if(myfile && wifiBuf && wifi)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));
		memset(wifiBuf, 0, sizeof(WIFI_Def) * MaxSaveWifiNum);

		myfile->res = f_open(&(myfile->file), "0:/WifiSet.ncd", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
			
		if(FR_OK == myfile->res)
		{
			f_lseek(&(myfile->file), 0);
			
			//�������б����wifi
			myfile->res = f_read(&(myfile->file), wifiBuf, sizeof(WIFI_Def) * MaxSaveWifiNum, &(myfile->br));
			if(FR_OK == myfile->res)
			{
				//�����Ƿ���ڴ������wifi
				for(i=0; i<MaxSaveWifiNum; i++)
				{
					if(wifiBuf[i].crc == CalModbusCRC16Fun1(&(wifiBuf[i]), sizeof(WIFI_Def)-2))
					{
						//����ҵ��Ա�������ݣ����滻
						if(pdPASS == CheckStrIsSame(wifi->ssid, wifiBuf[i].ssid, MaxSSIDLen))
						{
							wifi->crc = CalModbusCRC16Fun1(wifi, sizeof(WIFI_Def)-2);
							memcpy(&(wifiBuf[i]), wifi, sizeof(WIFI_Def));
							
							f_lseek(&(myfile->file), 0);
							myfile->res = f_write(&(myfile->file), wifiBuf, sizeof(WIFI_Def) * MaxSaveWifiNum, &(myfile->bw));
							if(FR_OK == myfile->res)
								statues = My_Pass;
							
							break;
						}
					}
				}
				
				//�����ǰ������wifi�����ڣ����½�
				if(i >= MaxSaveWifiNum)
				{
					//���ҿ�λ�ã����浱ǰwifi
					for(i=0; i<MaxSaveWifiNum; i++)
					{
						if(wifiBuf[i].crc != CalModbusCRC16Fun1(&(wifiBuf[i]), sizeof(WIFI_Def)-2))
						{
							wifi->crc = CalModbusCRC16Fun1(wifi, sizeof(WIFI_Def)-2);
							memcpy(&(wifiBuf[i]), wifi, sizeof(WIFI_Def));
							
							f_lseek(&(myfile->file), 0);
							myfile->res = f_write(&(myfile->file), wifiBuf, sizeof(WIFI_Def) * MaxSaveWifiNum, &(myfile->bw));
							if(FR_OK == myfile->res)
								statues = My_Pass;
							
							break;
						}
					}
				}
			}
			f_close(&(myfile->file));
		}
	}
	
	MyFree(wifiBuf);
	MyFree(myfile);
	
	return statues;
}

MyState_TypeDef ReadWifiData(WIFI_Def * wifi)
{
	FatfsFileInfo_Def * myfile = NULL;
	WIFI_Def * wifiBuf = NULL;
	MyState_TypeDef statues = My_Fail;
	unsigned char i=0;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	wifiBuf = MyMalloc(sizeof(WIFI_Def) * MaxSaveWifiNum);
	
	if(myfile && wifiBuf && wifi)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));
		memset(wifiBuf, 0, sizeof(WIFI_Def) * MaxSaveWifiNum);
		
		myfile->res = f_open(&(myfile->file), "0:/WifiSet.ncd", FA_READ);
		
		if(FR_OK == myfile->res)
		{
			//�������б����wifi
			f_lseek(&(myfile->file), 0);
			myfile->res = f_read(&(myfile->file), wifiBuf, sizeof(WIFI_Def) * MaxSaveWifiNum, &(myfile->br));
			if(FR_OK == myfile->res)
			{
				//�����Ƿ���ڴ������wifi
				for(i=0; i<MaxSaveWifiNum; i++)
				{
					if(wifiBuf[i].crc == CalModbusCRC16Fun1(&(wifiBuf[i]), sizeof(WIFI_Def)-2))
					{
						//����ҵ��Ա�������ݣ����ȡ
						if(pdPASS == CheckStrIsSame(wifi->ssid, wifiBuf[i].ssid, MaxSSIDLen))
						{
							memcpy(wifi->key, wifiBuf[i].key, MaxKEYLen);
							statues = My_Pass;
							break;
						}
					}
				}
			}

			f_close(&(myfile->file));
		}
	}
	
	MyFree(wifiBuf);
	MyFree(myfile);
	
	return statues;
}

MyState_TypeDef deleteWifi(WIFI_Def * wifi)
{
	FatfsFileInfo_Def * myfile = NULL;
	WIFI_Def * wifiBuf = NULL;
	MyState_TypeDef statues = My_Fail;
	unsigned char i=0;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	wifiBuf = MyMalloc(sizeof(WIFI_Def) * MaxSaveWifiNum);
	
	if(myfile && wifiBuf && wifi)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));
		memset(wifiBuf, 0, sizeof(WIFI_Def) * MaxSaveWifiNum);
		
		myfile->res = f_open(&(myfile->file), "0:/WifiSet.ncd", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
		
		if(FR_OK == myfile->res)
		{
			//�������б����wifi
			f_lseek(&(myfile->file), 0);
			myfile->res = f_read(&(myfile->file), wifiBuf, sizeof(WIFI_Def) * MaxSaveWifiNum, &(myfile->br));
			if(FR_OK == myfile->res)
			{
				//�����Ƿ���ڴ������wifi
				for(i=0; i<MaxSaveWifiNum; i++)
				{
					if(wifiBuf[i].crc == CalModbusCRC16Fun1(&(wifiBuf[i]), sizeof(WIFI_Def)-2))
					{
						//����ҵ��Ա�������ݣ���ɾ��
						if(pdPASS == CheckStrIsSame(wifi->ssid, wifiBuf[i].ssid, MaxSSIDLen))
						{
							memset(&(wifiBuf[i]), 0, sizeof(WIFI_Def));
							
							f_lseek(&(myfile->file), 0);
							myfile->res = f_write(&(myfile->file), wifiBuf, sizeof(WIFI_Def) * MaxSaveWifiNum, &(myfile->bw));
							if(FR_OK == myfile->res)
								statues = My_Pass;
							
							break;
						}
					}
				}
				
				//û�ҵ����򷵻�ɾ���ɹ�
				if(i >= MaxSaveWifiNum)
					statues = My_Pass;
			}

			f_close(&(myfile->file));
		}
	}
	
	MyFree(wifiBuf);
	MyFree(myfile);
	
	return statues;
}

/***************************************************************************************************
*FunctionName: ClearWifi
*Description: ɾ��wifi����
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017��3��1��09:59:32
***************************************************************************************************/
MyState_TypeDef ClearWifi(void)
{
	FRESULT res;
	
	res = f_unlink("0:/WifiSet.ncd");
	
	if((FR_OK == res) || (FR_NO_FILE == res))
		return My_Pass;
	else
		return My_Fail;
}

/****************************************end of file************************************************/
