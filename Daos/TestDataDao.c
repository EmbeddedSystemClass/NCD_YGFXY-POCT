/***************************************************************************************************
*FileName: TestDataDao
*Description: 测试数据dao
*Author: xsx_kair
*Data: 2016年12月8日10:43:26
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

MyState_TypeDef writeTestDataToFile(TestData * testData)
{
	FatfsFileInfo_Def * myfile = NULL;
	DeviceRecordHeader * deviceRecordHeader = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(MyFileStructSize);
	deviceRecordHeader = MyMalloc(DeviceRecordHeaderStructSize);
	
	if(myfile && testData && deviceRecordHeader)
	{
		memset(myfile, 0, MyFileStructSize);
		memset(deviceRecordHeader, 0, DeviceRecordHeaderStructSize);
		
		myfile->res = f_open(&(myfile->file), TestDataFileName, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
			
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));
			f_lseek(&(myfile->file), 0);
			
			//读取数据头
			myfile->res = f_read(&(myfile->file), deviceRecordHeader, DeviceRecordHeaderStructSize, &(myfile->br));
			if(deviceRecordHeader->crc != CalModbusCRC16Fun1(deviceRecordHeader, DeviceRecordHeaderStructCrcSize))
			{
				deviceRecordHeader->itemSize = 0;
				deviceRecordHeader->uploadIndex = 0;
				deviceRecordHeader->crc = CalModbusCRC16Fun1(deviceRecordHeader, DeviceRecordHeaderStructCrcSize);
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
			testData->crc = CalModbusCRC16Fun1(testData, TestDataStructCrcSize);
			f_lseek(&(myfile->file), deviceRecordHeader->itemSize * TestDataStructSize + DeviceRecordHeaderStructSize);
			myfile->res = f_write(&(myfile->file), testData, TestDataStructSize, &(myfile->bw));

			if((FR_OK != myfile->res) || (myfile->bw != TestDataStructSize))
				goto Finally;
			
			//更新数据头
			deviceRecordHeader->itemSize += 1;
			deviceRecordHeader->crc = CalModbusCRC16Fun1(deviceRecordHeader, DeviceRecordHeaderStructCrcSize);
			
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

MyState_TypeDef readTestDataFromFile(TestDataRecordReadPackage * testDataRecordReadPackage)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef statues = My_Fail;
	unsigned char i=0;
	
	myfile = MyMalloc(MyFileStructSize);

	if(myfile && testDataRecordReadPackage)
	{
		memset(myfile, 0, MyFileStructSize);
		memset(&(testDataRecordReadPackage->deviceRecordHeader), 0, DeviceRecordHeaderStructSize);
		memset(&(testDataRecordReadPackage->testData), 0, TestDataStructSize * TestDataRecordPageShowNum);
		testDataRecordReadPackage->readTotalNum = 0;
		
		myfile->res = f_open(&(myfile->file), TestDataFileName, FA_READ);
		
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));
			
			f_lseek(&(myfile->file), 0);
			
			//读取数据头
			myfile->res = f_read(&(myfile->file), &(testDataRecordReadPackage->deviceRecordHeader), DeviceRecordHeaderStructSize, &(myfile->br));
			if(testDataRecordReadPackage->deviceRecordHeader.crc != CalModbusCRC16Fun1(&(testDataRecordReadPackage->deviceRecordHeader), DeviceRecordHeaderStructCrcSize))
				goto Finally;
			
			//如果pageRequest的crc错误，表示是按照上传索引读取数据进行上传
			if(testDataRecordReadPackage->pageRequest.crc != CalModbusCRC16Fun1(&(testDataRecordReadPackage->pageRequest), PageRequestStructCrcSize))
			{
				if(testDataRecordReadPackage->deviceRecordHeader.uploadIndex < testDataRecordReadPackage->deviceRecordHeader.itemSize)
				{
					f_lseek(&(myfile->file), testDataRecordReadPackage->deviceRecordHeader.uploadIndex * TestDataStructSize + DeviceRecordHeaderStructSize);
				
					if((testDataRecordReadPackage->deviceRecordHeader.itemSize - testDataRecordReadPackage->deviceRecordHeader.uploadIndex) >= TestDataRecordPageShowNum)
						f_read(&(myfile->file), testDataRecordReadPackage->testData, TestDataRecordPageShowNum * TestDataStructSize, &(myfile->br));
					else
						f_read(&(myfile->file), testDataRecordReadPackage->testData, (testDataRecordReadPackage->deviceRecordHeader.itemSize - testDataRecordReadPackage->deviceRecordHeader.uploadIndex) * TestDataStructSize, &(myfile->br));
				}
			}
			//如果pageRequest的crc正确，表示是按照pageRequest的请求内容进行读取数据
			else if(testDataRecordReadPackage->pageRequest.startElementIndex < testDataRecordReadPackage->deviceRecordHeader.itemSize)
			{
				if(testDataRecordReadPackage->pageRequest.pageSize > (testDataRecordReadPackage->deviceRecordHeader.itemSize - testDataRecordReadPackage->pageRequest.startElementIndex))
					testDataRecordReadPackage->pageRequest.pageSize = (testDataRecordReadPackage->deviceRecordHeader.itemSize - testDataRecordReadPackage->pageRequest.startElementIndex);
					
				if(testDataRecordReadPackage->pageRequest.orderType == DESC)
					myfile->res = f_lseek(&(myfile->file), (testDataRecordReadPackage->pageRequest.startElementIndex) * TestDataStructSize + DeviceRecordHeaderStructSize);
				else
					myfile->res = f_lseek(&(myfile->file), (testDataRecordReadPackage->deviceRecordHeader.itemSize - (testDataRecordReadPackage->pageRequest.pageSize + testDataRecordReadPackage->pageRequest.startElementIndex)) * TestDataStructSize + DeviceRecordHeaderStructSize);
					
				myfile->res = f_read(&(myfile->file), testDataRecordReadPackage->testData, testDataRecordReadPackage->pageRequest.pageSize * TestDataStructSize, &(myfile->br));	
				
				for(i=0; i<TestDataRecordPageShowNum; i++)
				{
					if(testDataRecordReadPackage->testData[i].crc == CalModbusCRC16Fun1(&testDataRecordReadPackage->testData[i], TestDataStructCrcSize))
						testDataRecordReadPackage->readTotalNum++;
				}
			}
			
			statues = My_Pass;
			
			Finally:
				f_close(&(myfile->file));
		}
	}
	MyFree(myfile);
	
	return statues;
}

MyState_TypeDef plusTestDataHeaderUpLoadIndexToFile(unsigned int index)
{
	FatfsFileInfo_Def * myfile = NULL;
	DeviceRecordHeader * deviceRecordHeader = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(MyFileStructSize);
	deviceRecordHeader = MyMalloc(DeviceRecordHeaderStructSize);

	if(myfile && deviceRecordHeader)
	{
		memset(myfile, 0, MyFileStructSize);

		myfile->res = f_open(&(myfile->file), TestDataFileName, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
		
		if(FR_OK == myfile->res)
		{
			f_lseek(&(myfile->file), 0);

			myfile->res = f_read(&(myfile->file), deviceRecordHeader, DeviceRecordHeaderStructSize, &(myfile->br));
			if((FR_OK == myfile->res) && (DeviceRecordHeaderStructSize == myfile->br) 
				&& (deviceRecordHeader->crc == CalModbusCRC16Fun1(deviceRecordHeader, DeviceRecordHeaderStructCrcSize)))
			{
				deviceRecordHeader->uploadIndex += index;
				deviceRecordHeader->crc = CalModbusCRC16Fun1(deviceRecordHeader, DeviceRecordHeaderStructCrcSize);
				
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

MyState_TypeDef deleteTestDataFile(void)
{
	FRESULT res;
	
	res = f_unlink(TestDataFileName);
	
	if((FR_OK == res) || (FR_NO_FILE == res))
		return My_Pass;
	else
		return My_Fail;
}
/****************************************end of file************************************************/
