/***************************************************************************************************
*FileName: TestDataDao
*Description: ��������dao
*Author: xsx_kair
*Data: 2016��12��8��10:43:26
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"TestDataDao.h"

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

/***************************************************************************************************
*FunctionName: WriteTestData
*Description: д�������ݵ��ļ�
*Input: testdata -- �������ݵ�ַ
*		writeIndex -- д������
*Output: None
*Return: 	My_Pass -- ����ɹ�
*			My_Fail -- ����ʧ��
*Author: xsx
*Date: 2016��12��8��10:55:53
***************************************************************************************************/
MyState_TypeDef WriteTestData(TestData * testdata, unsigned int writeIndex)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	
	if(myfile && testdata)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));
		
		myfile->res = f_open(&(myfile->file), "0:/TD.NCD", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
			
		if(FR_OK == myfile->res)
		{
			myfile->res = f_lseek(&(myfile->file), writeIndex*sizeof(TestData));
			//д������
			testdata->crc = CalModbusCRC16Fun1(testdata, sizeof(TestData)-2);
			
			myfile->res = f_write(&(myfile->file), testdata, sizeof(TestData), &(myfile->bw));
			if(myfile->res == FR_OK)
				statues = My_Pass;
			
			f_close(&(myfile->file));
		}
	}
	
	MyFree(myfile);
	
	return statues;
}

/***************************************************************************************************
*FunctionName: ReadTestData
*Description: ��ȡ��������
*Input: readpackage -- ������ȡ��Ϣ�Լ���ȡ������
*Output: None
*Return: 	My_Pass -- ��ȡ�ɹ�
*			My_Fail -- ��ȡʧ��
*Author: xsx
*Date: 2016��12��8��11:25:18
***************************************************************************************************/
MyState_TypeDef ReadTestData(PageRequest * pageRequest, Page * page, SystemSetData * systemSetData)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef statues = My_Fail;
	unsigned char i=0;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	
	if(myfile && pageRequest && page)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));
		
		//��ն�ȡ���
		memset(page, sizeof(Page), 0);
		
		myfile->res = f_open(&(myfile->file), "0:/TD.NCD", FA_READ);
		
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));
			
			if(pageRequest->startElementIndex < systemSetData->testDataNum)
			{
				if(pageRequest->pageSize > (systemSetData->testDataNum - pageRequest->startElementIndex))
					pageRequest->pageSize = (systemSetData->testDataNum - pageRequest->startElementIndex);
				
				if(pageRequest->orderType == DESC)
					myfile->res = f_lseek(&(myfile->file), (pageRequest->startElementIndex)*sizeof(TestData));
				else
					myfile->res = f_lseek(&(myfile->file), ( systemSetData->testDataNum - (pageRequest->pageSize + pageRequest->startElementIndex))*sizeof(TestData));
				
				myfile->res = f_read(&(myfile->file), page->testData, pageRequest->pageSize * sizeof(TestData), &(myfile->br));
				
				for(i=0; i<pageRequest->pageSize; i++)
				{
					if(page->testData[i].crc == CalModbusCRC16Fun1(&(page->testData[i]), sizeof(TestData)-2))
						page->ElementsSize++;
				}

				statues = My_Pass;
			}
			
			f_close(&(myfile->file));
		}
	}
	
	MyFree(myfile);
	
	return statues;
}

/****************************************end of file************************************************/
