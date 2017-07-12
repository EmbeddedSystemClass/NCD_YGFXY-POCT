/***************************************************************************************************
*FileName: TestDataDao
*Description: 测试数据dao
*Author: xsx_kair
*Data: 2016年12月8日10:43:26
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"RecordDataDao.h"

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
*FunctionName:  writeRecordDataToFile
*Description:  保存记录到sd卡，此处为通用写操作，只进行写数据和更新数据头操作， 具体数据的crc需要在此之前计算完成
*Input: 	fileName -- 写文件名
*			recordData -- 写入数据
*			recordItemSize -- 写入数据单元长度
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年7月3日 15:42:09
***************************************************************************************************/
MyRes writeRecordDataToFile(const char * fileName, void * recordData, unsigned int recordItemSize)
{
	FatfsFileInfo_Def * myfile = NULL;
	DeviceRecordHeader * deviceRecordHeader = NULL;
	MyRes statues = My_Fail;
	
	myfile = MyMalloc(MyFileStructSize);
	deviceRecordHeader = MyMalloc(DeviceRecordHeaderStructSize);
	
	if(myfile && recordData && deviceRecordHeader && fileName)
	{
		memset(myfile, 0, MyFileStructSize);
		memset(deviceRecordHeader, 0, DeviceRecordHeaderStructSize);
		
		myfile->res = f_open(&(myfile->file), fileName, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
			
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));
			f_lseek(&(myfile->file), 0);
			
			//读取数据头
			myfile->res = f_read(&(myfile->file), deviceRecordHeader, DeviceRecordHeaderStructSize, &(myfile->br));
			if(deviceRecordHeader->crc != CalModbusCRC16Fun(deviceRecordHeader, DeviceRecordHeaderStructCrcSize, NULL))
			{
				deviceRecordHeader->itemSize = 0;
				deviceRecordHeader->userUpLoadIndex = 0;
				deviceRecordHeader->ncdUpLoadIndex = 0;
				deviceRecordHeader->crc = CalModbusCRC16Fun(deviceRecordHeader, DeviceRecordHeaderStructCrcSize, NULL);
			}
			
			//先写一次数据头，防止没有数据头写数据异常
			if(0 == myfile->size)
			{
				f_lseek(&(myfile->file), 0);
				myfile->res = f_write(&(myfile->file), deviceRecordHeader, DeviceRecordHeaderStructSize, &(myfile->bw));
				if((FR_OK != myfile->res) || (myfile->bw != DeviceRecordHeaderStructSize))
					goto Finally;
			}
			
			//写数据
			f_lseek(&(myfile->file), deviceRecordHeader->itemSize * recordItemSize + DeviceRecordHeaderStructSize);
			myfile->res = f_write(&(myfile->file), recordData, recordItemSize, &(myfile->bw));

			if((FR_OK != myfile->res) || (myfile->bw != recordItemSize))
				goto Finally;
			
			//更新数据头
			deviceRecordHeader->itemSize += 1;
			deviceRecordHeader->crc = CalModbusCRC16Fun(deviceRecordHeader, DeviceRecordHeaderStructCrcSize, NULL);
			
			f_lseek(&(myfile->file), 0);
			myfile->res = f_write(&(myfile->file), deviceRecordHeader, DeviceRecordHeaderStructSize, &(myfile->bw));
			if((FR_OK == myfile->res) && (myfile->bw == DeviceRecordHeaderStructSize))
				statues = My_Pass;
			
			Finally:
				f_close(&(myfile->file));
		}
	}
	
	MyFree(myfile);
	MyFree(deviceRecordHeader);
	
	return statues;
}

/***************************************************************************************************
*FunctionName:  readRecordDataFromFile
*Description:  从sd读取记录数据，如果按照pageRequest读取，则读取数据保存在recordData1中
*	否则recordData1中保存根据用户上传索引读取的数据，recordData2保存根据纽康度上传索引读取的数据
*Input:  	fileName -- 文件名
*			pageRequest -- 分页请求参数
*			deviceRecordHeader -- 读取数据头地址
*			recordData1 -- 保存读取的数据地址1
*			recordData2 -- 保存读取的数据地址2
*			recordItemSize -- 读取数据条目大小
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年7月3日 16:14:55
***************************************************************************************************/
MyRes readRecordDataFromFile(const char * fileName, PageRequest * pageRequest, DeviceRecordHeader * deviceRecordHeader, 
	void * recordData1, void * recordData2, unsigned short recordItemSize)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyRes statues = My_Fail;
	
	myfile = MyMalloc(MyFileStructSize);

	if(myfile && fileName && deviceRecordHeader)
	{
		memset(myfile, 0, MyFileStructSize);
		memset(deviceRecordHeader, 0, DeviceRecordHeaderStructSize);
		
		myfile->res = f_open(&(myfile->file), fileName, FA_READ);
		
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));
			
			f_lseek(&(myfile->file), 0);
			
			//读取数据头
			myfile->res = f_read(&(myfile->file), deviceRecordHeader, DeviceRecordHeaderStructSize, &(myfile->br));
			if(deviceRecordHeader->crc != CalModbusCRC16Fun(deviceRecordHeader, DeviceRecordHeaderStructCrcSize, NULL))
				goto Finally;
			
			//如果pageRequest为空，则根据上传索引读取数据
			if(pageRequest == NULL)
			{
				//根据用户上传索引读取数据
				if(deviceRecordHeader->userUpLoadIndex < deviceRecordHeader->itemSize)
				{
					f_lseek(&(myfile->file), deviceRecordHeader->userUpLoadIndex * recordItemSize + DeviceRecordHeaderStructSize);
				
					f_read(&(myfile->file), recordData1, recordItemSize, &(myfile->br));
				}
				
				//根据纽康度上传索引读取数据
				if(deviceRecordHeader->ncdUpLoadIndex < deviceRecordHeader->itemSize)
				{
					f_lseek(&(myfile->file), deviceRecordHeader->ncdUpLoadIndex * recordItemSize + DeviceRecordHeaderStructSize);
				
					f_read(&(myfile->file), recordData2, recordItemSize, &(myfile->br));
				}
			}
			//如果pageRequest != NULL，表示是按照pageRequest的请求内容进行读取数据
			else if(pageRequest->startElementIndex < deviceRecordHeader->itemSize)
			{
				if(pageRequest->pageSize > (deviceRecordHeader->itemSize - pageRequest->startElementIndex))
					pageRequest->pageSize = (deviceRecordHeader->itemSize - pageRequest->startElementIndex);
					
				if(pageRequest->orderType == DESC)
					myfile->res = f_lseek(&(myfile->file), (pageRequest->startElementIndex) * recordItemSize + DeviceRecordHeaderStructSize);
				else
					myfile->res = f_lseek(&(myfile->file), (deviceRecordHeader->itemSize - (pageRequest->pageSize + pageRequest->startElementIndex)) * recordItemSize + DeviceRecordHeaderStructSize);
					
				f_read(&(myfile->file), recordData1, pageRequest->pageSize * recordItemSize, &(myfile->br));	
			}
			
			statues = My_Pass;
			
			Finally:
				f_close(&(myfile->file));
		}
	}
	MyFree(myfile);
	
	return statues;
}

/***************************************************************************************************
*FunctionName:  plusRecordDataHeaderUpLoadIndexToFile
*Description:  增加上传索引：在原数据上加上一个数字
*Input:  	fileName -- 文件名
*			userIndexPlusNum -- 用户上传索引加的大小
*			ncdIndexPlusNum -- 纽康度上传索引加的大小
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年7月3日 16:16:44
***************************************************************************************************/
MyRes plusRecordDataHeaderUpLoadIndexToFile(const char * fileName, unsigned char userIndexPlusNum, unsigned char ncdIndexPlusNum)
{
	FatfsFileInfo_Def * myfile = NULL;
	DeviceRecordHeader * deviceRecordHeader = NULL;
	MyRes statues = My_Fail;
	
	myfile = MyMalloc(MyFileStructSize);
	deviceRecordHeader = MyMalloc(DeviceRecordHeaderStructSize);

	if(myfile && deviceRecordHeader)
	{
		memset(myfile, 0, MyFileStructSize);

		myfile->res = f_open(&(myfile->file), fileName, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
		
		if(FR_OK == myfile->res)
		{
			f_lseek(&(myfile->file), 0);

			myfile->res = f_read(&(myfile->file), deviceRecordHeader, DeviceRecordHeaderStructSize, &(myfile->br));
			if((FR_OK == myfile->res) && (DeviceRecordHeaderStructSize == myfile->br) 
				&& (deviceRecordHeader->crc == CalModbusCRC16Fun(deviceRecordHeader, DeviceRecordHeaderStructCrcSize, NULL)))
			{
				deviceRecordHeader->userUpLoadIndex += userIndexPlusNum;
				deviceRecordHeader->ncdUpLoadIndex += ncdIndexPlusNum;
				deviceRecordHeader->crc = CalModbusCRC16Fun(deviceRecordHeader, DeviceRecordHeaderStructCrcSize, NULL);
				
				f_lseek(&(myfile->file), 0);
				myfile->res = f_write(&(myfile->file), deviceRecordHeader, DeviceRecordHeaderStructSize, &(myfile->bw));
				if((FR_OK == myfile->res)&&(myfile->bw == DeviceRecordHeaderStructSize))
					statues = My_Pass;
			}
			
			f_close(&(myfile->file));
		}
	}
	MyFree(myfile);
	MyFree(deviceRecordHeader);
	
	return statues;
}

/***************************************************************************************************
*FunctionName:  deleteRecordDataFile
*Description:  删除数据记录文件
*Input:  	fileName -- 删除的文件名
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年7月3日 16:18:06
***************************************************************************************************/
MyRes deleteRecordDataFile(const char * fileName)
{
	FRESULT res;
	
	res = f_unlink(fileName);
	
	if((FR_OK == res) || (FR_NO_FILE == res))
		return My_Pass;
	else
		return My_Fail;
}
/****************************************end of file************************************************/
