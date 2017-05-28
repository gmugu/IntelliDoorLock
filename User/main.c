/**
******************************************************************************
主函数
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

/*app函数头文件*/
#include "http_server.h"
#include "httputil.h"
FATFS fs;
uint8 reboot_flag = 0;
extern uint8_t Ov7725_vsync;

int main(void)
{
	int i,ii,keyval;
	systick_init(72);										//初始化Systick工作时钟
	USART1_Config();										//初始化串口通信:115200@8-n-1
	LED_GPIO_Config();									//初始化led
	
	TM_FATFS_FLASH_SPI_disk_initialize();										//初始化8M串行flash
	{
		f_mount(&fs,"0:",1);			//挂载文件系统
	}

	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_AFIO, ENABLE );
	Ov7725_GPIO_Config();				// ov7725 gpio 初始化
	while(Ov7725_Init() != SUCCESS);    // ov7725 寄存器初始化
	VSYNC_Init();	    // ov7725 场信号线初始化
	Ov7725_vsync = 0;
	
	printf("  野火网络适配版 网络初始化 Demo V1.0 \r\n");		

	gpio_for_w5500_config();						        //初始化MCU相关引脚
	reset_w5500();											//硬复位W5500
	set_w5500_mac();										//配置MAC地址
	set_w5500_ip();											//配置IP地址
			
	socket_buf_init(txsize, rxsize);		//初始化8个Socket的发送接收缓存大小
	
	printf(" 应用程序执行中…… \r\n"); 
//	printf(" 在IE浏览器中输入 W5500的IP就可访问web Server \r\n");

	i=0;
	while(1) 														/*循环执行的函数*/ 
	{
		i++;
		if(i%1000==0){
			ii=i/1000;
			LED2(ii%2);
		}
    do_https();//Web server测试程
    if(reboot_flag==1){
			reboot();
		}
		
		keyval=intelli_key_scan();
		if(keyval!=-1&&keyval!=-2)
printf("key %c\n",keyval);


	}
} 


