/**
******************************************************************************
������
******************************************************************************
*/ 
#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_led.h"
#include "fatfs_flash_spi.h"
#include "ff.h"
#include "bsp_ov7725.h"

#include "w5500.h"
#include "W5500_conf.h"
#include "socket.h"
#include "utility.h"

#include "keyboard.h"

/*app����ͷ�ļ�*/
#include "http_server.h"
#include "httputil.h"
FATFS fs;
uint8 reboot_flag = 0;
extern uint8_t Ov7725_vsync;

int main(void)
{
	int i,ii,keyval;
	systick_init(72);										//��ʼ��Systick����ʱ��
	USART1_Config();										//��ʼ������ͨ��:115200@8-n-1
	LED_GPIO_Config();									//��ʼ��led
	
	TM_FATFS_FLASH_SPI_disk_initialize();										//��ʼ��8M����flash
	{
		f_mount(&fs,"0:",1);			//�����ļ�ϵͳ
	}

	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_AFIO, ENABLE );
	Ov7725_GPIO_Config();				// ov7725 gpio ��ʼ��
	while(Ov7725_Init() != SUCCESS);    // ov7725 �Ĵ�����ʼ��
	VSYNC_Init();	    // ov7725 ���ź��߳�ʼ��
	Ov7725_vsync = 0;
	
	printf("  Ұ����������� �����ʼ�� Demo V1.0 \r\n");		

	gpio_for_w5500_config();						        //��ʼ��MCU�������
	reset_w5500();											//Ӳ��λW5500
	set_w5500_mac();										//����MAC��ַ
	set_w5500_ip();											//����IP��ַ
			
	socket_buf_init(txsize, rxsize);		//��ʼ��8��Socket�ķ��ͽ��ջ����С
	
	printf(" Ӧ�ó���ִ���С��� \r\n"); 
//	printf(" ��IE����������� W5500��IP�Ϳɷ���web Server \r\n");

	i=0;
	while(1) 														/*ѭ��ִ�еĺ���*/ 
	{
		i++;
		if(i%1000==0){
			ii=i/1000;
			LED2(ii%2);
		}
    do_https();//Web server���Գ�
    if(reboot_flag==1){
			reboot();
		}
		
		keyval=intelli_key_scan();
		if(keyval!=-1&&keyval!=-2)
printf("key %c\n",keyval);


	}
} 


