/**
******************************************************************************
* @file    			main.c
* @author  			WIZnet Software Team
* @version 			V1.0
* @date    			2015-02-14
* @brief   			��3.5.0�汾�⽨�Ĺ���ģ��
* @attention  	ʵ��ƽ̨������ iSO-V3 STM32 ������ + ����W5500���������
*
*               Ĭ��ʹ�ñ��𿪷����SPI2�ӿ�
*							
*               �������ԣ��뱣֤W5500��IP�����PC����ͬһ�����ڣ��Ҳ���ͻ
*               ����������߸�PC��ֱ����������PC���������ӵ�ַIPΪ��̬IP
*
* ʵ��ƽ̨:���� iSO-V3 STM32 ������ 
* ��̳    :http://www.chuxue123.com
* �Ա�    :http://firestm32.taobao.com
******************************************************************************
*/ 
#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_led.h"
#include "fatfs_flash_spi.h"
#include "ff.h"

#include "w5500.h"
#include "W5500_conf.h"
#include "socket.h"
#include "utility.h"

/*app����ͷ�ļ�*/
#include "http_server.h"
#include "httputil.h"
FATFS fs;
uint8 reboot_flag = 0;
int main(void)
{ 	
	int i,ii;
	systick_init(72);										/*��ʼ��Systick����ʱ��*/
	USART1_Config();										/*��ʼ������ͨ��:115200@8-n-1*/
	LED_GPIO_Config();									/*��ʼ��led*/
	TM_FATFS_FLASH_SPI_disk_initialize();										/*��ʼ��8M����flash*/
	{
		f_mount(&fs,"0:",1);			//�����ļ�ϵͳ
	}
	
	printf("  Ұ����������� �����ʼ�� Demo V1.0 \r\n");		
	
	gpio_for_w5500_config();						        /*��ʼ��MCU�������*/
	reset_w5500();											/*Ӳ��λW5500*/
	set_w5500_mac();										/*����MAC��ַ*/
	set_w5500_ip();											/*����IP��ַ*/
	
	socket_buf_init(txsize, rxsize);		/*��ʼ��8��Socket�ķ��ͽ��ջ����С*/
	
	printf(" Ӧ�ó���ִ���С��� \r\n"); 
	printf(" ��IE����������� W5500��IP�Ϳɷ���web Server \r\n");

	i=0;
	while(1) 														/*ѭ��ִ�еĺ���*/ 
	{
		i++;
		ii=i/1000;
		if(i%3000==0)
			LED_RGB111(ii%7);
    do_https();/*Web server���Գ���*/
    if(reboot_flag==1)
    reboot();
	}
} 

