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

FATFS fs;
uint8 reboot_flag = 0;
extern uint8_t Ov7725_vsync;

static UINT br, bw;
static FIL fnew;	
static FRESULT res_flash; 
static BYTE textFileBuffer[] = "��ӭʹ��Ұ��STM32-V3 ISO������  �����Ǹ������ӣ��½��ļ�ϵͳ�����ļ�";
static BYTE buffer[1024]={0};

int main(void)
{
	
	int i,ii,keyval;
	systick_init(72);										//��ʼ��Systick����ʱ��
	USART1_Config();										//��ʼ������ͨ��:115200@8-n-1
	LED_GPIO_Config();									//��ʼ��led

	TM_FATFS_FLASH_SPI_disk_initialize();										//��ʼ��8M����flash
	{
		f_mount(&fs,"0:",1);			//�����ļ�ϵͳ
		
		
		printf("\r\n f_mount res_flash=%d \r\n",res_flash);
	
	//���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ
	if(res_flash ==FR_NO_FILESYSTEM)
	{
		res_flash=f_mkfs("0:",0,4096);							//��ʽ��
		printf("\r\nmkfs res_flash=%d",res_flash);
		res_flash = f_mount(&fs,"0:",0);						//��ʽ������ȡ������
		res_flash = f_mount(&fs,"0:",1);						//���¹���
	}
/**************************  flash   *****************************************/		
	//�ļ�ϵͳ���ԣ�д����
	//���ļ�������ļ����μ��򴴽���
	res_flash = f_open(&fnew, "0:flashnewfile.txt", FA_CREATE_ALWAYS | FA_WRITE );
	 
	if ( res_flash == FR_OK )
	{
		res_flash = f_write(&fnew, textFileBuffer, sizeof(textFileBuffer), &bw);
		f_close(&fnew);      
	}
	//������
	res_flash = f_open(&fnew, "0:flashnewfile.txt", FA_OPEN_EXISTING | FA_READ); 	 
	res_flash = f_read(&fnew, buffer, sizeof(buffer), &br); 
	printf("\n s %s b \n", buffer);
	/* Close open files */
	res_flash = f_close(&fnew);	

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
    do_https();//Web server
		do_http_client();
		do_udp();
    if(reboot_flag==1){
			reboot();
		}
		
		keyval=intelli_key_scan();
		if(keyval!=-1&&keyval!=-2){
			printf("key %c press up\n",keyval);
			key_input(keyval);
			
		}

	}
} 


