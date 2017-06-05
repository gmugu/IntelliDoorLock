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
#include "api.h"
#include "udp.h"

/*app����ͷ�ļ�*/
#include "http_server.h"
#include "httputil.h"

#include "http_client.h"

#include "bsp_TiMbase.h"
#include "lock.h"
#include "bsp_beep.h"
uint8 reboot_flag = 0;
extern uint8_t Ov7725_vsync;

static int count=0;
void do_lde(){
	int ii;
	count++;
	if(count%1000==0){
		ii=count/1000;
		LED2(ii%2);
	}
}

int main(void)
{
	
	int i,ii;
	systick_init(72);										//��ʼ��Systick����ʱ��
	USARTx_Config();										//��ʼ������ͨ��:115200@8-n-1
	LED_GPIO_Config();									//��ʼ��led

	TIMx_Configuration();																		// TIM2 ��ʱ����
	TIMx_NVIC_Configuration();															// ��ʱ�����ж����ȼ�
	macTIM_APBxClock_FUN (macTIM_CLK, ENABLE);							// TIM2 ���¿�ʱ�ӣ���ʼ��ʱ
	
	TM_FATFS_FLASH_SPI_disk_initialize();										//��ʼ��8M����flash
	
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_AFIO, ENABLE );
	Ov7725_GPIO_Config();				// ov7725 gpio ��ʼ��
	while(Ov7725_Init() != SUCCESS);    // ov7725 �Ĵ�����ʼ��
	VSYNC_Init();	    // ov7725 ���ź��߳�ʼ��
	Ov7725_vsync = 0;
	
	gpio_for_w5500_config();						//��ʼ��MCU�������
	reset_w5500();											//Ӳ��λW5500
	set_w5500_mac();										//����MAC��ַ
	set_w5500_ip();											//����IP��ַ
	socket_buf_init(txsize, rxsize);		//��ʼ��8��Socket�ķ��ͽ��ջ����С
	
	Lock_Config(); 	
	Beep_Init();
	
	printf(" Ӧ�ó���ִ���С��� \r\n"); 

	while(1) 														/*ѭ��ִ�еĺ���*/ 
	{
		do_lde();
    do_https();//Web server
		do_https1();
		do_http_client();//http client
		do_udp();//udp client
		do_key();//���̼���

    if(reboot_flag==1){
			reboot();
		}

	}
} 


