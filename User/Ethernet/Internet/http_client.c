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


char  postT[]={
"POST /v1.0/device/17530/sensor/38669/datapoints HTTP/1.1\r\n"
"Host:api.yeelink.net\r\n"
"Accept:*/*\r\n"
"U-ApiKey:07597cf93ee715d2fee144f25aa3ff65\r\n"
"Content-Length:12\r\n"
"Content-Type:application/x-www-form-urlencoded\r\n"
"Connection:close\r\n"
"\r\n"
"{\"value\":xx}\r\n"
};/*xx��������¶���ֵ*/

char  postH[]={//�ύʪ��
"POST /v1.0/device/17530/sensor/34928/datapoints HTTP/1.1\r\n"
"Host:api.yeelink.net\r\n"
"Accept:*/*\r\n"
"U-ApiKey:07597cf93ee715d2fee144f25aa3ff65\r\n"
"Content-Length:12\r\n"
"Content-Type:application/x-www-form-urlencoded\r\n"
"Connection:close\r\n"
"\r\n"
"{\"value\":xx}\r\n"
};/*xx��������¶���ֵ*/

uint8 ip[]={192,168,1,103};
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
				connect(ch, ip ,port);									/*��socket�˿�*/
			}
		break;	
		case SOCK_ESTABLISHED: 																					/*socket���ӽ���*/
			if(getSn_IR(ch) & Sn_IR_CON)  
			{
				setSn_IR(ch, Sn_IR_CON);																		/*��������жϱ�־*/
			}
			if(isFirst){
				send(ch,(const uint8 *)buffer,strlen(buffer));							/*W5500�� Yeelink��������������*/
				isFirst=FALSE;
			}
			delay_ms(20);
			
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

