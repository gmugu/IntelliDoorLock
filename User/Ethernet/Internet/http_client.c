/**
******************************************************************************
* @file   		http_client.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-02-14
* @brief   		http_client ��ʾ���� 
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
*@brief		ִ��http client������
*@param		��
*@return	��
*/
uint8 do_http_client()
{	
	static uint8 i=0;
	uint8 ch=SOCK_HUMTEM;

	uint16 anyport=3000;
	uint8 sock_state=getSn_SR(ch);

	switch(sock_state)																							/*��ȡsocket SOCK_TCPS ��״̬*/
	{
		case SOCK_INIT:																									/*socket��ʼ�����*/
			if(isSend){
				printf("����http���� ip:%d.%d.%d.%d  %d\n",ip[0],ip[1],ip[2],ip[3],port);
				connect(ch, ip ,port);									/*��socket�˿�*/
			}
		break;	
		case SOCK_ESTABLISHED: 																					//socket���ӽ���
			if(getSn_IR(ch) & Sn_IR_CON)  
			{
				setSn_IR(ch, Sn_IR_CON);																		//��������жϱ�־
			}
			if(isFirst){
				send(ch,(const uint8 *)buffer,strlen(buffer));							/*W5500�� Yeelink��������������*/
				isFirst=FALSE;
				close(ch);
			}
			delay_ms(10);
			
		break;
		case SOCK_CLOSE_WAIT:  																					/*socket�ȴ��ر�״̬*/
			close(ch);  
		break;
		case SOCK_CLOSED: 																							/*socket�ر�*/
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

