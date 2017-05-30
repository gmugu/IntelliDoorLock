/**
******************************************************************************
* @file   		udp_demo.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-02-14
* @brief   		UDP��ʾ����
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
*@brief		UDP���Գ���
*@param		��
*@return	��
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
	uint16 len=0;                                                      /*����һ��2KB�Ļ���*/	
	switch(getSn_SR(SOCK_UDPS))                                                /*��ȡsocket��״̬*/
	{
		case SOCK_CLOSED:                                                        /*socket���ڹر�״̬*/
			socket(SOCK_UDPS,Sn_MR_UDP,1111,0);                              /*��ʼ��socket*/
		  break;
		
		case SOCK_UDP:                                                           /*socket��ʼ�����*/
			if(isSend){
				delay_ms(10);

				printf("���͹㲥��%s\r\n",buffer);                                               /*��ӡ���ջ���*/ 
				sendto(SOCK_UDPS,(uint8*)buffer,strlen(buffer), remote_ip, remote_port);                /*W5500�ѽ��յ������ݷ��͸�Remote*/
				isSend=FALSE;
			}
			break;
	}

}

