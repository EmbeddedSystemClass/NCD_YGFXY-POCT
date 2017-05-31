/***************************************************************************************************
*FileName：netconf
*Description：LWIP的初始化，网线状态检测任务
*Author：xsx
*Data：2016年3月9日15:11:31
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"lwip/mem.h"
#include 	"lwip/memp.h"
#include 	"lwip/dhcp.h"
#include 	"ethernetif.h"
#include 	"netconf.h"
#include 	"tcpip.h"
#include 	<stdio.h>
#include 	"stm32f4x7_eth.h"
#include 	"stm32f4x7_eth_bsp.h"
#include	"LwIPConfig.h"
#include	"Define.h"
#include	"SystemSet_Data.h"
#include	"System_Data.h"
#include	"string.h"

#include	"LwipCom_Task.h"
#include	"LwipDHCP_Task.h"
#include	"LwipServerForLab_Task.h"

/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
struct netif xnetif; /* network interface structure */
static void LwIP_Init(WireNetSet * wireNetSet);

 
/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
void ETH_link_callback(struct netif *netif);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/


 
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
static void LwIP_Init(WireNetSet * wireNetSet)
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
	
	ETH_BSP_Config();
	/* Create tcp_ip stack thread */
	tcpip_init( NULL, NULL );	
	
	/* IP address setting */
	if(wireNetSet->ipMode == Dynamic_IP)
	{
		ipaddr.addr = 0;
		netmask.addr = 0;
		gw.addr = 0;
	}
	else
	{
		IP4_ADDR(&ipaddr, wireNetSet->staticIP.ip_1, wireNetSet->staticIP.ip_2, wireNetSet->staticIP.ip_3, wireNetSet->staticIP.ip_4);
		IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
		IP4_ADDR(&gw, wireNetSet->staticIP.ip_1, wireNetSet->staticIP.ip_2, wireNetSet->staticIP.ip_3, 1);	
	}
	
	setSystemWireIP(ipaddr.addr);

	netif_add(&xnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

	//  Registers the default network interface.
	netif_set_default(&xnetif);
	
	if(LinkUp == ReadPHYLinkState())
	{
		netif_set_up(&xnetif);
		
		if(wireNetSet->ipMode == Dynamic_IP)
			SetGB_DHCPState(DHCP_START);
	}
	else
	{
		//  When the netif link is down this function must be called.
		netif_set_down(&xnetif);
		
		if(wireNetSet->ipMode == Dynamic_IP)
			SetGB_DHCPState(DHCP_LINK_DOWN);
	}
    
	// Set the link callback function, this function is called on change of link status
	netif_set_link_callback(&xnetif, ETH_link_callback);
}

void StartEthernet(void)
{
	WireNetSet myWireNetSet;
	
	memcpy(&myWireNetSet, &(getGBSystemSetData()->wireNetSet), sizeof(WireNetSet));
	
	//* 初始化LwIP
	LwIP_Init(&myWireNetSet);
	
	if(Dynamic_IP == myWireNetSet.ipMode)
		StartvLwipDHCPTask(&xnetif);
	
	StartvLwipComTask(&xnetif);
	
	StartLwipLabServerTask();

}




/***************************************************************************************************
*FunctionName：ETH_link_callback
*Description：网线连接状态改变回调函数
*Input：None
*Output：None
*Author：xsx
*Data：2016年3月9日17:02:01
***************************************************************************************************/
void ETH_link_callback(struct netif *netif)
{

	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
	
	WireNetSet myWireNetSet;
	
	memcpy(&myWireNetSet, &(getGBSystemSetData()->wireNetSet), sizeof(WireNetSet));
		
	if(netif_is_link_up(netif))
	{
		ETH_Start();

		if(myWireNetSet.ipMode == Dynamic_IP)
		{
			ipaddr.addr = 0;
			netmask.addr = 0;
			gw.addr = 0;
			
			SetGB_DHCPState(DHCP_START);
		}
		else
		{
			IP4_ADDR(&ipaddr, myWireNetSet.staticIP.ip_1, myWireNetSet.staticIP.ip_2, myWireNetSet.staticIP.ip_3, myWireNetSet.staticIP.ip_4);
			IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
			IP4_ADDR(&gw, myWireNetSet.staticIP.ip_1, myWireNetSet.staticIP.ip_2, myWireNetSet.staticIP.ip_3, 1);
		}

		setSystemWireIP(ipaddr.addr);
		
		netif_set_addr(&xnetif, &ipaddr , &netmask, &gw);
    
		/* When the netif is fully configured this function must be called.*/
		netif_set_up(&xnetif);    
	}
	else
	{
		ETH_Stop();
		
		if(myWireNetSet.ipMode == Dynamic_IP)
		{
			SetGB_DHCPState(DHCP_LINK_DOWN);
			dhcp_stop(netif);
		}
		
		ipaddr.addr = 0;
		setSystemWireIP(ipaddr.addr);

		/*  When the netif link is down this function must be called.*/
		netif_set_down(&xnetif);
	}
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
