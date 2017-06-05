/**
******************************************************************************
* @file   		http_client.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-02-14
* @brief   		http_client 演示函数 
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "http_client.h"
#include "W5500_conf.h"
#include "w5500.h"
#include "socket.h"
#include "utility.h"


uint8 ip[]={192,168,1,99};
uint16 port=8080;
static char* buffer;
bool isSend=FALSE;
bool isFirst=TRUE;

void sendBuf(char* buf){
	buffer=buf;
	isSend=TRUE;
}


/**
*@brief		执行http client主函数
*@param		无
*@return	无
*/
uint8 do_http_client()
{	
	static uint8 i=0;
	uint8 ch=SOCK_HUMTEM;

	uint16 anyport=3000;
	uint8 sock_state=getSn_SR(ch);

	switch(sock_state)																							/*获取socket SOCK_TCPS 的状态*/
	{
		case SOCK_INIT:																									/*socket初始化完成*/
			if(isSend){
				printf("发送http请求 ip:%d.%d.%d.%d  %d\n",ip[0],ip[1],ip[2],ip[3],port);
				connect(ch, ip ,port);									/*打开socket端口*/
			}
		break;	
		case SOCK_ESTABLISHED: 																					//socket连接建立
			if(getSn_IR(ch) & Sn_IR_CON)  
			{
				setSn_IR(ch, Sn_IR_CON);																		//清除接收中断标志
			}
			if(isFirst){
				send(ch,(const uint8 *)buffer,strlen(buffer));							/*W5500向 Yeelink服务器发送数据*/
				isFirst=FALSE;
				close(ch);
			}
			delay_ms(10);
			
		break;
		case SOCK_CLOSE_WAIT:  																					/*socket等待关闭状态*/
			close(ch);  
		break;
		case SOCK_CLOSED: 																							/*socket关闭*/
			isSend=FALSE;
			buffer=NULL;
			isFirst=TRUE;
			socket(ch, Sn_MR_TCP,anyport , 0x00);
		break;
		default:
		break;
	}  
	return sock_state;
}

