/***************************************************************************************************
*FileName:
*Description:
*Author:xsx
*Data:
***************************************************************************************************/


/***************************************************************************************************/
/******************************************ͷ�ļ�***************************************************/
/***************************************************************************************************/

#include	"JsonToObject.h"
#include	"CRC16.h"

#include	<string.h>
#include	"stdio.h"
/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/



/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/


/***************************************************************************************************/
/***************************************************************************************************/
/***************************************����********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName:  ParseJsonToDateTime
*Description:  ����json��DateTime�ṹ��
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017��6��1�� 15:13:29
***************************************************************************************************/
MyRes ParseJsonToDateTime(const char * jsonStr, DateTime * dateTime)
{
	cJSON * json = NULL;
	cJSON * tempJson = NULL;
	MyRes status = My_Fail;
	
	if(jsonStr && dateTime)
	{
		json = cJSON_Parse(jsonStr);
		if(json)
		{
			//��
			tempJson = cJSON_GetObjectItem(json, "year");
			if((tempJson) && (tempJson->valueint > 2000))
				dateTime->year = tempJson->valueint - 2000;
			else
				goto END;
			
			//��
			tempJson = cJSON_GetObjectItem(json, "monthValue");
			if((tempJson) && (tempJson->valueint <= 12))
				dateTime->month = tempJson->valueint;
			else
				goto END;
			
			//��
			tempJson = cJSON_GetObjectItem(json, "dayOfMonth");
			if((tempJson) && (tempJson->valueint <= 31))
				dateTime->day = tempJson->valueint;
			else
				goto END;
			
			//ʱ
			tempJson = cJSON_GetObjectItem(json, "hour");
			if((tempJson) && (tempJson->valueint <= 24))
				dateTime->hour = tempJson->valueint;
			else
				goto END;
			
			//��
			tempJson = cJSON_GetObjectItem(json, "minute");
			if((tempJson) && (tempJson->valueint <= 59))
				dateTime->min = tempJson->valueint;
			else
				goto END;
			
			//��
			tempJson = cJSON_GetObjectItem(json, "second");
			if((tempJson) && (tempJson->valueint <= 59))
				dateTime->sec = tempJson->valueint;
			else
				goto END;
			
			status = My_Pass;
		}
	}

	END:
		cJSON_Delete(json);
	
		return status;
}

/***************************************************************************************************
*FunctionName:  ParseJsonToDeviceAndOperator
*Description:  ����json���豸��Ϣ�Ͳ�����
*Input:  jsonStr -- json�ַ���
*			device -- �����󱣴��豸���ݵ�λ��
*			operator -- �����󱣴�����˵�λ��
*Output:  
*Return:  
*Author:  xsx
*Date: 
***************************************************************************************************/
MyRes ParseJsonToDevice(const char * jsonStr, Device * device)
{
	cJSON * json = NULL;
	cJSON * tempJson1 = NULL;
	cJSON * tempJson2 = NULL;
	unsigned char size = 0, i;
	MyRes status = My_Fail;
	
	if(jsonStr && device)
	{
		json = cJSON_Parse(jsonStr);
		if(json)
		{
			//�豸��ַ
			tempJson1 = cJSON_GetObjectItem(json, "addr");
			if(tempJson1)
				memcpy(device->addr, tempJson1->valuestring, strlen(tempJson1->valuestring)+1);
			else
				goto END;
			
			//�豸����
			tempJson1 = cJSON_GetObjectItem(json, "department");
			if(tempJson1)
			{
				tempJson2 = cJSON_GetObjectItem(tempJson1, "name");
				memcpy(device->department, tempJson2->valuestring, strlen(tempJson2->valuestring)+1);
			}
			else
				goto END;
			
			//�豸�޸�ʱ���
			tempJson1 = cJSON_GetObjectItem(json, "modifyTimeStamp");
			if(tempJson1)
			{
				device->modifyTimeStamp = tempJson1->valueint;
			}
			else
				goto END;
			
			//������
			tempJson1 = cJSON_GetObjectItem(json, "operator");
			if(tempJson1)
			{
				if(My_Fail == ParseJsonToOperator(tempJson1, &(device->operator)))
					goto END;
			}
			else
				goto END;
			
			//������
			tempJson1 = cJSON_GetObjectItem(json, "operators");
			if(tempJson1)
			{
				size = cJSON_GetArraySize(tempJson1);
				for(i=0; i<size; i++)
				{
					tempJson2 = cJSON_GetArrayItem(tempJson1, i);
					
					if(My_Fail == ParseJsonToOperator(tempJson2, &device->operators[i]))
						goto END;
				}
			}
			else
				goto END;
			
			device->crc = CalModbusCRC16Fun(device, DeviceStructCrcSize, NULL);
			
			status = My_Pass;
		}
	}

	END:
		cJSON_Delete(json);
	
		return status;
}

MyRes ParseJsonToOperator(cJSON * json, Operator * opeartor)
{
	cJSON * tempJson1 = NULL;
	cJSON * tempJson2 = NULL;
	MyRes status = My_Fail;
	
	if(json && opeartor)
	{
		//������id
		tempJson1 = cJSON_GetObjectItem(json, "id");
		if(tempJson1)
			opeartor->id = tempJson1->valueint;
		else
			goto END;
		
		//����
		tempJson1 = cJSON_GetObjectItem(json, "name");
		if(tempJson1)
			memcpy(opeartor->name, tempJson1->valuestring, strlen(tempJson1->valuestring)+1);
		else
			goto END;
		
		//����
		tempJson1 = cJSON_GetObjectItem(json, "age");
		if(tempJson1)
			memcpy(opeartor->age, tempJson1->valuestring, strlen(tempJson1->valuestring)+1);
		else
			goto END;
		
		//�Ա�
		tempJson1 = cJSON_GetObjectItem(json, "sex");
		if(tempJson1)
			memcpy(opeartor->sex, tempJson1->valuestring, strlen(tempJson1->valuestring)+1);
		else
			goto END;
		
		//��ϵ��ʽ
		tempJson1 = cJSON_GetObjectItem(json, "phone");
		if(tempJson1)
			memcpy(opeartor->phone, tempJson1->valuestring, strlen(tempJson1->valuestring)+1);
		else
			goto END;
		
		//ְ��
		tempJson1 = cJSON_GetObjectItem(json, "job");
		if(tempJson1)
			memcpy(opeartor->job, tempJson1->valuestring, strlen(tempJson1->valuestring)+1);
		else
			goto END;
		
		//��ע
		tempJson1 = cJSON_GetObjectItem(json, "department");
		if(tempJson1)
		{
			tempJson2 = cJSON_GetObjectItem(tempJson1, "name");
			if(tempJson2)
				memcpy(opeartor->department, tempJson2->valuestring, strlen(tempJson2->valuestring)+1);
			else
				goto END;
		}
		else
			goto END;
		
		opeartor->crc = CalModbusCRC16Fun(opeartor, OneOperatorStructCrcSize, NULL);
		
		status = My_Pass;
	}

	END:
		return status;
}

MyRes ParseJsonToRemoteSoftInfo(const char * jsonStr, RemoteSoftInfo * remoteSoftInfo)
{
	cJSON * json = NULL;
	cJSON * tempJson1 = NULL;
	MyRes status = My_Fail;
	
	if(jsonStr && remoteSoftInfo)
	{
		json = cJSON_Parse(jsonStr);
		if(json)
		{
			//version
			tempJson1 = cJSON_GetObjectItem(json, "version");
			if(tempJson1)
			{
				remoteSoftInfo->RemoteFirmwareVersion = tempJson1->valueint;
			}
			else
				goto END;
			
			//md5
			tempJson1 = cJSON_GetObjectItem(json, "md5");
			if(tempJson1)
				memcpy(remoteSoftInfo->md5, tempJson1->valuestring, strlen(tempJson1->valuestring)+1);
			else
				goto END;

			status = My_Pass;
		}
	}

	END:
		return status;
}
