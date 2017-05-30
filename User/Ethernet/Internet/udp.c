/**
******************************************************************************
* @file   		udp_demo.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-02-14
* @brief   		UDP演示函数
******************************************************************************
**/
#include <stdio.h>
#include <string.h>
#include "w5500_conf.h"
#include "w5500.h"
#include "socket.h"
#include "utility.h"
#include "udp.h"

/**
*@brief		UDP测试程序
*@param		无
*@return	无
*/
static uint8 remote_ip[]={255,255,255,255};
static uint16 remote_port=9406;
static char* buffer;
static bool isSend=FALSE;

void sendUDP(char* buf){
	buffer=buf;
	isSend=TRUE;
}

void do_udp(void)
{                                                              
	uint16 len=0;                                                      /*定义一个2KB的缓存*/	
	switch(getSn_SR(SOCK_UDPS))                                                /*获取socket的状态*/
	{
		case SOCK_CLOSED:                                                        /*socket处于关闭状态*/
			socket(SOCK_UDPS,Sn_MR_UDP,1111,0);                              /*初始化socket*/
		  break;
		
		case SOCK_UDP:                                                           /*socket初始化完成*/
			if(isSend){
				delay_ms(10);

				printf("发送广播：%s\r\n",buffer);                                               /*打印接收缓存*/ 
				sendto(SOCK_UDPS,(uint8*)buffer,strlen(buffer), remote_ip, remote_port);                /*W5500把接收到的数据发送给Remote*/
				isSend=FALSE;
			}
			break;
	}

}

