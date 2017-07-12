/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"UserMPage.h"
#include	"LCD_Driver.h"
#include	"MyMem.h"
#include	"CRC16.h"
#include	"SystemSetPage.h"
#include	"DeviceDao.h"
#include	"MyTools.h"
#include	"SleepPage.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static UserMPageBuffer * page = NULL;

/******************************************************************************************/
/*****************************************局部函数声明*************************************/
static void activityStart(void);
static void activityInput(unsigned char *pbuf , unsigned short len);
static void activityFresh(void);
static void activityHide(void);
static void activityResume(void);
static void activityDestroy(void);
static MyRes activityBufferMalloc(void);
static void activityBufferFree(void);

static void showCurrentPageList(void);
static void clearPageList(void);
static void showCurrentOperatorInfo(void);
static void deleteCurrentOperator(void);
static MyRes addOrModifyOperator(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

/***************************************************************************************************
*FunctionName: createSelectUserActivity
*Description: 创建选择操作人界面
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:09
***************************************************************************************************/
MyRes createUserManagerActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "UserManagerActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
		return My_Pass;
	}
	
	return My_Fail;
}

/***************************************************************************************************
*FunctionName: activityStart
*Description: 显示主界面
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:32
***************************************************************************************************/
static void activityStart(void)
{
	page->myDeviceLock = getMyDeviceLock();
	page->isLocked = false;
	timer_set(&(page->timer), 1);
	
	clearPageList();
	
	SelectPage(106);
}

/***************************************************************************************************
*FunctionName: activityInput
*Description: 界面输入
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:59
***************************************************************************************************/
static void activityInput(unsigned char *pbuf , unsigned short len)
{
	/*命令*/
	page->lcdinput[0] = pbuf[4];
	page->lcdinput[0] = (page->lcdinput[0]<<8) + pbuf[5];
		
	/*返回*/
	if(page->lcdinput[0] == 0x1d00)
	{
		backToFatherActivity();
	}
		
	/*上翻也*/
	else if(page->lcdinput[0] == 0x1d03)
	{
		if(page->isLocked && page->pageindex > 0)
		{
			page->pageindex--;

			showCurrentPageList();
		}
	}
	/*下翻页*/
	else if(page->lcdinput[0] == 0x1d04)
	{
		if(page->isLocked && ((page->pageindex+1) < MaxOperatorPageSize))
		{
			page->tempOperator = &page->device.operators[(page->pageindex+1)*MaxPageShowOperatorSize];
			
			if(page->tempOperator->crc == CalModbusCRC16Fun(page->tempOperator, OneOperatorStructCrcSize, NULL))
			{
				page->pageindex++; 

				showCurrentPageList();
			}
		}
	}
	/*选择操作人*/
	else if((page->lcdinput[0] >= 0x1d07)&&(page->lcdinput[0] <= 0x1d0B))
	{
		page->tempOperator = &page->device.operators[(page->pageindex)*MaxPageShowOperatorSize + page->lcdinput[0] - 0x1d07];
		
		if(page->tempOperator->crc == CalModbusCRC16Fun(page->tempOperator, OneOperatorStructCrcSize, NULL))
		{
			page->tempV1 = page->lcdinput[0] - 0x1d07+1;
			BasicPic(0x1d40, 1, 137, 11, 10, 303, 79, 157, 135+(page->tempV1-1)*72);
			
			page->currentOperator = page->tempOperator;
			
			showCurrentOperatorInfo();
		}	
	}
	/*删除*/
	else if(page->lcdinput[0] == 0x1d01)
	{
		if(page->isLocked)
			deleteCurrentOperator();
	}
	//修改或者添加
	else if(page->lcdinput[0] == 0x1d02)
	{
		if(strlen(page->newOperator.name) > 0)
		{
			if(My_Pass == addOrModifyOperator())
				SendKeyCode(1);
			else
				SendKeyCode(2);
		}
		else
			SendKeyCode(2);
	}
	//姓名
	else if(page->lcdinput[0] == 0x1d50)
	{
		getLcdInputData(page->newOperator.name, &pbuf[7]);
	}
	//年龄
	else if(page->lcdinput[0] == 0x1d60)
	{
		getLcdInputData(page->newOperator.age, &pbuf[7]);
	}
	//性别
	else if(page->lcdinput[0] == 0x1d70)
	{
		getLcdInputData(page->newOperator.sex, &pbuf[7]);
	}
	//联系方式
	else if(page->lcdinput[0] == 0x1d80)
	{
		getLcdInputData(page->newOperator.phone, &pbuf[7]);
	}
	//职位
	else if(page->lcdinput[0] == 0x1d90)
	{
		getLcdInputData(page->newOperator.job, &pbuf[7]);
	}
	//备注
	else if(page->lcdinput[0] == 0x1da0)
	{
		getLcdInputData(page->newOperator.department, &pbuf[7]);
	}
}

/***************************************************************************************************
*FunctionName: activityFresh
*Description: 界面刷新
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:01:16
***************************************************************************************************/
static void activityFresh(void)
{
	if(TimeOut == timer_expired(&(page->timer)))
	{
		//先给设备信息文件上锁，防止其他线程修改数据
		if((page->isLocked == false) && (My_Pass == LockObject(page->myDeviceLock, &page, 1)))
		{
			/*读取所有操作人*/
			ReadDeviceFromFile(&(page->device));
			
			page->pageindex = 0;

			showCurrentPageList();
			
			page->isLocked = true;
		}
		
		timer_restart(&(page->timer));
	}
}

/***************************************************************************************************
*FunctionName: activityHide
*Description: 隐藏界面时要做的事
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:01:40
***************************************************************************************************/
static void activityHide(void)
{

}

/***************************************************************************************************
*FunctionName: activityResume
*Description: 界面恢复显示时要做的事
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:01:58
***************************************************************************************************/
static void activityResume(void)
{
	SelectPage(106);
}

/***************************************************************************************************
*FunctionName: activityDestroy
*Description: 界面销毁
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:02:15
***************************************************************************************************/
static void activityDestroy(void)
{
	UnLockObject(page->myDeviceLock, &page);
	
	activityBufferFree();
}

/***************************************************************************************************
*FunctionName: activityBufferMalloc
*Description: 界面数据内存申请
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 
***************************************************************************************************/
static MyRes activityBufferMalloc(void)
{
	if(NULL == page)
	{
		page = MyMalloc(sizeof(UserMPageBuffer));
		
		if(page)
		{
			memset(page, 0, sizeof(UserMPageBuffer));
	
			return My_Pass;
		}
		else
			return My_Fail;
	}
	else
		return My_Pass;
}

/***************************************************************************************************
*FunctionName: activityBufferFree
*Description: 界面内存释放
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:03:10
***************************************************************************************************/
static void activityBufferFree(void)
{
	MyFree(page);
	page = NULL;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************
*FunctionName:  showCurrentPageList
*Description:  显示当前页操作人列表
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年7月6日 14:16:50
***************************************************************************************************/
static void showCurrentPageList(void)
{
	page->tempV1 = page->pageindex*MaxPageShowOperatorSize;
	
	page->tempOperator = &(page->device.operators[page->tempV1]);
	
	/*显示列表数据*/
	for(page->tempV1=0; page->tempV1<MaxOperatorSize; page->tempV1++)
	{
		if(page->tempOperator->crc == CalModbusCRC16Fun(page->tempOperator, OneOperatorStructCrcSize, NULL))
			snprintf(page->buf, OperatorNameLen, "%s", page->tempOperator->name);
		else
			memset(page->buf, 0, 10);
		
		DisText(0x1d10+8*page->tempV1, page->buf, strlen(page->buf)+1);
		
		page->tempOperator++;
	}
	
	page->currentOperator = NULL;
	
	showCurrentOperatorInfo();
}

/***************************************************************************************************
*FunctionName:  clearPageList
*Description:  清除列表数据
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年7月7日 08:43:04
***************************************************************************************************/
static void clearPageList(void)
{
	/*显示列表数据*/
	for(page->tempV1=0; page->tempV1<MaxPageShowOperatorSize; page->tempV1++)
	{
		ClearText(0x1d10+8*page->tempV1);
	}
	
	page->currentOperator = NULL;
	
	showCurrentOperatorInfo();
}

/***************************************************************************************************
*FunctionName:  showCurrentOperatorInfo
*Description:  显示当前操作人信息
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年7月6日 14:19:13
***************************************************************************************************/
static void showCurrentOperatorInfo(void)
{
	if(page->currentOperator)
	{
		snprintf(page->buf, OperatorNameLen, "%s", page->currentOperator->name);
		DisText(0x1d50, page->buf, strlen(page->buf)+1);
			
		snprintf(page->buf, 10, "%s", page->currentOperator->age);
		DisText(0x1d60, page->buf, strlen(page->buf)+1);
			
		snprintf(page->buf, 10, "%s", page->currentOperator->sex);
		DisText(0x1d70, page->buf, strlen(page->buf)+1);
			
		snprintf(page->buf, 20, "%s", page->currentOperator->phone);
		DisText(0x1d80, page->buf, strlen(page->buf)+1);
			
		snprintf(page->buf, 30, "%s", page->currentOperator->job);
		DisText(0x1d90, page->buf, strlen(page->buf)+1);
			
		snprintf(page->buf, 20, "%s", page->currentOperator->department);
		DisText(0x1da0, page->buf, strlen(page->buf)+1);
		
		memcpy(&page->newOperator, page->currentOperator, OneOperatorStructSize);
	}
	else
	{
		ClearText(0x1d50);
			
		ClearText(0x1d60);
				
		ClearText(0x1d70);
				
		ClearText(0x1d80);
				
		ClearText(0x1d90);
				
		ClearText(0x1da0);
		
		BasicPic(0x1d40, 0, 137, 11, 10, 303, 79, 157, 135);
		
		memset(&page->newOperator, 0, OneOperatorStructSize);
	}
}

/***************************************************************************************************
*FunctionName:  deleteCurrentOperator
*Description:  删除当前操作人
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年7月6日 15:25:44
***************************************************************************************************/
static void deleteCurrentOperator(void)
{
	if(page->currentOperator && page->currentOperator != &(page->newOperator))
	{
		//删除当前操作人
		page->currentOperator->crc = 0;
		
		//重新排列操作人
		page->tempOperator = page->currentOperator;
		page->tempOperator++;
		memset(page->operatorCopy, 0, MaxOperatorSize*OneOperatorStructSize);
		page->tempV1 = OneOperatorStructSize * (&page->device.operators[MaxOperatorSize-1] - page->currentOperator);
		memcpy(page->operatorCopy, page->tempOperator, page->tempV1);
		
		memcpy(page->currentOperator, page->operatorCopy, page->tempV1);
		
		page->device.crc = CalModbusCRC16Fun(&(page->device), DeviceStructCrcSize, NULL);
		
		if(My_Pass != SaveDeviceToFile(&(page->device)))
			ReadDeviceFromFile(&page->device);
		
		page->pageindex = 0;

		showCurrentPageList();
	}
}

/***************************************************************************************************
*FunctionName:  addOrModifyOperator
*Description:  添加或者修改操作人
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年7月7日 16:07:17
***************************************************************************************************/
static MyRes addOrModifyOperator(void)
{
	page->newOperator.crc = CalModbusCRC16Fun(&page->newOperator, OneOperatorStructCrcSize, NULL);
	
	//查找是否存在名字一样的人
	page->tempV2 = MaxOperatorSize;
	page->tempOperator = page->device.operators;
	for(page->tempV1 = 0; page->tempV1 < MaxOperatorSize; page->tempV1++)
	{
		if(page->tempOperator->crc == CalModbusCRC16Fun(page->tempOperator, OneOperatorStructCrcSize, NULL))
		{
			if(CheckStrIsSame(page->newOperator.name, page->tempOperator->name, OperatorNameLen))
			{
				memcpy(page->tempOperator, &page->newOperator, OneOperatorStructSize);
				
				break;
			}
		}
		else if(page->tempV2 == MaxOperatorSize)
			page->tempV2 = page->tempV1;
		
		page->tempOperator++;
	}
	
	//如果不存在，说明要新建
	if(page->tempV1 >= MaxOperatorSize)
	{
		//如果数量已满，则无法添加
		if(page->tempV2 == MaxOperatorSize)
			return My_Fail;
		else
			memcpy(&page->device.operators[page->tempV2], &page->newOperator, OneOperatorStructSize);
	}
	
	page->device.crc = CalModbusCRC16Fun(&(page->device), DeviceStructCrcSize, NULL);
		
	if(My_Pass == SaveDeviceToFile(&(page->device)))
	{
		page->pageindex = 0;

		showCurrentPageList();
	
		return My_Pass;
	}
	else
	{
		return My_Fail;
	}
		
	
}

