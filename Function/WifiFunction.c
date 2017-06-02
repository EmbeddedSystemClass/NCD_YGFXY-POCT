/***************************************************************************************************
*FileName:WifiFunction
*Description:һЩWIFIģ��Ĳ�������,����ֻ�����߳���ʹ��
*Author:xsx
*Data:2016��4��30��16:06:36
***************************************************************************************************/


/***************************************************************************************************/
/******************************************ͷ�ļ�***************************************************/
/***************************************************************************************************/
#include	"WifiFunction.h"

#include 	"Usart4_Driver.h"
#include	"QueueUnits.h"

#include	"CRC16.h"
#include	"MyMem.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"

/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/
static xSemaphoreHandle xWifiMutex = NULL;									//WIFI������

/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/
static MyState_TypeDef ComWithWIFI(char * cmd, const char *strcmp, char *buf, unsigned short buflen, portTickType xBlockTime);
static void ProgressWifiListData(WIFI_Def *wifis, char *buf);
static MyState_TypeDef SetWifiServerInfo(SystemSetData * systemSetData);
static MyState_TypeDef SetWifiDefaultWorkMode(void);
static MyState_TypeDef SetWifiWorkInSTAMode(void);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************����********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName: takeWifiMutex, giveWifixMutex
*Description: 
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017��3��7��09:41:10
***************************************************************************************************/
MyState_TypeDef takeWifiMutex(portTickType xBlockTime)
{
	if(pdPASS == xSemaphoreTake(xWifiMutex, xBlockTime))
		return My_Pass;
	else
		return My_Fail;
}
void giveWifixMutex(void)
{
	xSemaphoreGive(xWifiMutex);
}

/***************************************************************************************************
*FunctionName: ComWithWIFI��wifiģ��ͨ��
*Description: 
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017��3��7��09:41:31
***************************************************************************************************/
static MyState_TypeDef ComWithWIFI(char * cmd, const char *strcmp, char *buf, unsigned short buflen, portTickType xBlockTime)
{
	MyState_TypeDef statues = My_Pass;
	unsigned char errorCnt = 0;
	
	//��ն�������
	while(pdPASS == ReceiveDataFromQueue(GetUsart4RXQueue(), NULL, buf, buflen, NULL, 1, 10 / portTICK_RATE_MS, 1 / portTICK_RATE_MS));
	
	for(errorCnt = 0; errorCnt < 3; errorCnt++)
	{
		if(pdPASS == SendDataToQueue(GetUsart4TXQueue(), NULL, cmd, strlen(cmd), 1, 500 * portTICK_RATE_MS, 0, EnableUsart4TXInterrupt))
		{
			if(buf)
			{
				memset(buf, 0, buflen);
					
				ReceiveDataFromQueue(GetUsart4RXQueue(), NULL, buf, buflen, NULL, 1, xBlockTime, 0);

				if(strcmp)
				{
					if(strstr(buf, strcmp) == NULL)
						statues = My_Fail;
				}
			}
		}
		else
			statues = My_Fail;
		
		if(statues == My_Pass)
			break;
	}

	return statues;
}


void WIFIInit(SystemSetData * systemSetData)
{
	vSemaphoreCreateBinary(xWifiMutex);
	
	SetWifiWorkInAT(AT_Mode);

	SetWifiServerInfo(systemSetData);

	SetWifiDefaultWorkMode();
	
	SetWifiWorkInSTAMode();
	
	RestartWifi();

}
/***************************************************************************************************
*FunctionName��SetWifiWorkInAT
*Description������wifiģ�鹤����ATģʽ
*Input��mode -- ����ģʽ
*Output��None
*Author��xsx
*Data��2016��3��14��10:43:46
***************************************************************************************************/
MyState_TypeDef SetWifiWorkInAT(WIFI_WorkMode_DefType mode)
{
	MyState_TypeDef statues = My_Fail;
	char *txbuf = NULL;
	
	if(mode == GetWifiWorkMode())
		return My_Pass;
	
	txbuf = MyMalloc(500);
	if(txbuf)
	{
		if(mode == AT_Mode)
		{
			if(My_Pass == ComWithWIFI("+++", "a", txbuf, 50, 500 * portTICK_RATE_MS))
			{
				if(My_Pass == ComWithWIFI("a", "+ok", txbuf, 50, 500 * portTICK_RATE_MS))
					statues = My_Pass;
			}
		}
		else
		{
			if(My_Pass == ComWithWIFI("AT+ENTM\r", "+ok", txbuf, 50, 50 * portTICK_RATE_MS))
				statues = My_Pass;
		}
		MyFree(txbuf);
	}
	
	return statues;
}

WIFI_WorkMode_DefType GetWifiWorkMode(void)
{
	char *txbuf = NULL;
	WIFI_WorkMode_DefType mode = None;
	
	txbuf = MyMalloc(500);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("+++\r", "+++", txbuf, 50, 50 * portTICK_RATE_MS))
			mode = AT_Mode;
		else
			mode = Normal_Mode;
	}
	
	MyFree(txbuf);
	
	return mode;
}

static MyState_TypeDef SetWifiDefaultWorkMode(void)
{
	char *txbuf = NULL;
	MyState_TypeDef statues = My_Fail;
	
	txbuf = MyMalloc(500);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("AT+TMODE=throughput\r", "+ok", txbuf, 50, 500 * portTICK_RATE_MS))
			statues = My_Pass;
	}
	
	MyFree(txbuf);
	
	return statues;
}

static MyState_TypeDef SetWifiWorkInSTAMode(void)
{
	char *txbuf = NULL;
	MyState_TypeDef statues = My_Fail;
	
	txbuf = MyMalloc(500);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("AT+WMODE=STA\r", "+ok", txbuf, 50, 500 * portTICK_RATE_MS))
			statues = My_Pass;
	}
	
	MyFree(txbuf);
	
	return statues;
}

MyState_TypeDef ScanApList(WIFI_Def *wifis)
{
	char *txbuf = NULL; 
	MyState_TypeDef statues = My_Fail;

	txbuf = MyMalloc(1000);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("AT+WSCAN\r", NULL, txbuf, 900, 5000 / portTICK_RATE_MS))
			ProgressWifiListData(wifis, txbuf);
		
		MyFree(txbuf);
	}
	
	return statues;
}

static void ProgressWifiListData(WIFI_Def *wifis, char *buf)
{
	static char *tempbuf, *tempbuf2, *tempbuf3;
	static char * (wifistr[MaxWifiListNum]);
	unsigned char i,j;
	
	tempbuf = strstr(buf, "\r\n");

	if(NULL == tempbuf)
		return;
	
	for(i=0; i<MaxWifiListNum; i++)
	{
		if(i == 0)
			wifistr[i] = strtok(tempbuf+2, "\n\r");
		else
			wifistr[i] = strtok(NULL, "\n\r");
		
		if(NULL == wifistr[i])
			break;
	}
	
	for(i=0; i<MaxWifiListNum; i++)
	{
		tempbuf3 = wifistr[i];
		if(tempbuf3)
		{
			memset(wifis, 0, sizeof(WIFI_Def));
			for(j=0; j<5; j++)
			{
				if(j == 0)
					tempbuf2 = strtok(tempbuf3, ",");
				else
					tempbuf2 = strtok(NULL, ",");
					
				if(tempbuf2)
				{
					if(j == 1)
						memcpy(wifis->ssid, tempbuf2, strlen(tempbuf2)) ;
					else if(j == 3)
					{
						if(strstr(tempbuf2, "OPEN"))
							memcpy(wifis->auth, "OPEN", 4);
						else if(strstr(tempbuf2, "SHARED"))
							memcpy(wifis->auth, "SHARED", 6);
						else if(strstr(tempbuf2, "WPAPSK"))
							memcpy(wifis->auth, "WPAPSK", 6);
						else if(strstr(tempbuf2, "WPA2PSK"))
							memcpy(wifis->auth, "WPA2PSK", 7);
							
						if(strstr(tempbuf2, "NONE"))
							memcpy(wifis->encry, "NONE", 4);
						else if(strstr(tempbuf2, "WEP-H"))
							memcpy(wifis->encry, "WEP-H", 5);
						else if(strstr(tempbuf2, "WEP-A"))
							memcpy(wifis->encry, "WEP-A", 5);
						else if(strstr(tempbuf2, "TKIP"))
							memcpy(wifis->encry, "TKIP", 4);
						else if(strstr(tempbuf2, "AES"))
							memcpy(wifis->encry, "AES", 3);
					}
					else if(j == 4)
					{
						wifis->indicator = strtol(tempbuf2 , NULL , 10);
						
						if(wifis->indicator > 10)
							wifis++;
					}
				}
			}				
		}
		else
			break;
	}
}

MyState_TypeDef ConnectWifi(WIFI_Def *wifis)
{
	char *txbuf = NULL;
	unsigned char i=0;
	MyState_TypeDef statues = My_Fail;
	
	/*�������ݻ�����*/
	txbuf = MyMalloc(500);
	if(txbuf)
	{
		/*����ssid*/
		memset(txbuf, 0, 100);
		sprintf(txbuf, (const char *)"AT+WSSSID=%s\r", wifis->ssid);
		if(My_Pass == ComWithWIFI(txbuf, "+ok", txbuf, 100, 1000 * portTICK_RATE_MS))
		{
			memset(txbuf, 0, 100);
			sprintf(txbuf, (const char *)"AT+WSKEY=%s,%s,%s\r", wifis->auth, wifis->encry, wifis->key);
			if(My_Pass == ComWithWIFI(txbuf, "+ok", txbuf, 100, 1000* portTICK_RATE_MS))
			{
				//����
				RestartWifi();
				
				vTaskDelay(8000 / portTICK_RATE_MS);
				
				for(i=0; i<3; i++)
				{
					if(My_Pass == SetWifiWorkInAT(AT_Mode))
					{
						memset(txbuf, 0, 100);
						if(My_Pass == WifiIsConnectted(txbuf))
						{
							if(strstr(txbuf, wifis->ssid) != NULL)
								statues = My_Pass;
						}
						
						break;
					}
					
					vTaskDelay(1000 / portTICK_RATE_MS);
				}
			}
		}
	}

	MyFree(txbuf);
	
	return statues;
}


MyState_TypeDef GetWifiStaIP(IP * ip)
{
	char *txbuf = NULL;
	char * tempp1 = NULL;
	MyState_TypeDef statues = My_Fail;
	
	/*�������ݻ�����*/
	txbuf = MyMalloc(500);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("AT+WANN\r", "+ok", txbuf, 100, 100 / portTICK_RATE_MS))
		{
			tempp1 = strtok(txbuf, ",");
			if(tempp1)
			{
				tempp1 = strtok(NULL, ",");
				
				if(tempp1)
				{
					ip->ip_1 = strtol(tempp1, &tempp1, 10);
					ip->ip_2 = strtol(tempp1+1, &tempp1, 10);
					ip->ip_3 = strtol(tempp1+1, &tempp1, 10);
					ip->ip_4 = strtol(tempp1+1, &tempp1, 10);
				}
			}
			
			statues = My_Pass;
		}
	}
	MyFree(txbuf);
	
	return statues;
}

MyState_TypeDef GetWifiStaMac(char *mac)
{
	char *txbuf = NULL;
	char * tempp1 = NULL;
	
	MyState_TypeDef statues = My_Fail;
	
	/*�������ݻ�����*/
	txbuf = MyMalloc(500);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("AT+WSMAC\r", "+ok", txbuf, 100, 100 / portTICK_RATE_MS))
		{
			tempp1 = strtok(txbuf, "=");
			if(tempp1)
			{
				tempp1 = strtok(NULL, "=");
				
				if(tempp1)
					memcpy(mac, tempp1, 12);
			}
			
			statues = My_Pass;
		}
	}
	MyFree(txbuf);
	
	return statues;
}

MyState_TypeDef WifiIsConnectted(char * ssid)
{
	char *txbuf = NULL; 
	char * tempp1 = NULL;
	char * tempp2 = NULL;
	unsigned char len = 0;
	MyState_TypeDef statues = My_Fail;
	
	/*�������ݻ�����*/
	txbuf = MyMalloc(500);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("AT+WSLK\r", "+ok", txbuf, 100, 50 / portTICK_RATE_MS))
		{
			if(!((strstr(txbuf, "Disconnected")) || (strstr(txbuf, "RF Off"))))
			{
				//�ܳ���
				len = strlen(txbuf);
				tempp1 = strtok(txbuf, "=");
				if(tempp1)
				{
					//��ȥͷ����
					len -= strlen(tempp1);
					//��ȥβ��mac����
					len -= 22;
					
					tempp1 = strtok(NULL, "=");
				
					if(tempp1)
					{
						tempp2 = strtok(tempp1, "(");
						
						if(tempp2)
							memcpy(ssid, tempp2, strlen(tempp2));
						
					}
				}
				statues = My_Pass;
			}
		}
	}
	MyFree(txbuf);
	
	return statues;
}

MyState_TypeDef RestartWifi(void)
{
	char *txbuf = NULL; 
	MyState_TypeDef statues = My_Fail;
	
	/*�������ݻ�����*/
	txbuf = MyMalloc(500);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("AT+Z\r", "+ok", txbuf, 50, 50 * portTICK_RATE_MS))
		{
			statues = My_Pass;	
		}
	}
	MyFree(txbuf);
	
	return statues;
}

MyState_TypeDef CheckWifiMID(void)
{
	char *txbuf = NULL; 
	MyState_TypeDef statues = My_Fail;
	
	/*�������ݻ�����*/
	txbuf = MyMalloc(500);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("AT+MID\r", "+ok", txbuf, 50, 50 * portTICK_RATE_MS))
		{
			if(strstr(txbuf, "USR-WIFI232-S"))
				statues = My_Pass;
		}
	}
	MyFree(txbuf);
	
	return statues;
}

unsigned char GetWifiIndicator(void)
{
	char *txbuf = NULL;
	unsigned char ind = 0;								//0����δ��wifi����������ź�ǿ��1
	char *s = NULL;
	
	/*�������ݻ�����*/
	txbuf = MyMalloc(500);
	if(txbuf)
	{
		s = txbuf;
		if(My_Pass == ComWithWIFI("AT+WSLQ\r", "+ok", txbuf, 50, 50 * portTICK_RATE_MS))
		{
			if(strstr(txbuf, "Disconnected"))
			{
				ind = 0;
			}
			else
			{
				s = strtok(txbuf, ",");
				if(s)
				{
					s = strtok(NULL, ",");
					if(s)
					{
						ind = strtol(s, NULL, 10);
						if(ind == 0)
							ind = 1;
					}
					else
						ind = 1;
				}
				else
					ind = 1;
			}
		}
		else
			ind = 0;
	}
	MyFree(txbuf);
	
	return ind;
}

static MyState_TypeDef SetWifiServerInfo(SystemSetData * systemSetData)
{
	char *txbuf = NULL; 
	MyState_TypeDef statues = My_Fail;
	
	/*�������ݻ�����*/
	txbuf = MyMalloc(500);
	if(txbuf)
	{
		sprintf(txbuf, "AT+NETP=TCP,CLIENT,%d,%d.%d.%d.%d\r\n\0", systemSetData->serverSet.serverPort, systemSetData->serverSet.serverIP.ip_1, 
			systemSetData->serverSet.serverIP.ip_2, systemSetData->serverSet.serverIP.ip_3, systemSetData->serverSet.serverIP.ip_4);
		if(My_Pass == ComWithWIFI(txbuf, "+ok", txbuf, 100, 1000 * portTICK_RATE_MS))
			statues = My_Pass;
	}
	MyFree(txbuf);
	
	return statues;
}
