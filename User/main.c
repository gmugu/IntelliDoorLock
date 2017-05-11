/**
******************************************************************************
* @file    			main.c
* @author  			WIZnet Software Team
* @version 			V1.0
* @date    			2015-02-14
* @brief   			用3.5.0版本库建的工程模板
* @attention  	实验平台：秉火 iSO-V3 STM32 开发板 + 秉火W5500网络适配板
*
*               默认使用秉火开发板的SPI2接口
*							
*               内网测试，请保证W5500的IP与测试PC机在同一网段内，且不冲突
*               如果是用网线跟PC机直连，请设置PC机本地连接地址IP为静态IP
*
* 实验平台:秉火 iSO-V3 STM32 开发板 
* 论坛    :http://www.chuxue123.com
* 淘宝    :http://firestm32.taobao.com
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

/*app函数头文件*/
#include "http_server.h"
#include "httputil.h"
FATFS fs;
uint8 reboot_flag = 0;
int main(void)
{ 	
	int i,ii;
	systick_init(72);										/*初始化Systick工作时钟*/
	USART1_Config();										/*初始化串口通信:115200@8-n-1*/
	LED_GPIO_Config();									/*初始化led*/
	TM_FATFS_FLASH_SPI_disk_initialize();										/*初始化8M串行flash*/
	{
		f_mount(&fs,"0:",1);			//挂载文件系统
	}
	
	printf("  野火网络适配版 网络初始化 Demo V1.0 \r\n");		
	
	gpio_for_w5500_config();						        /*初始化MCU相关引脚*/
	reset_w5500();											/*硬复位W5500*/
	set_w5500_mac();										/*配置MAC地址*/
	set_w5500_ip();											/*配置IP地址*/
	
	socket_buf_init(txsize, rxsize);		/*初始化8个Socket的发送接收缓存大小*/
	
	printf(" 应用程序执行中…… \r\n"); 
	printf(" 在IE浏览器中输入 W5500的IP就可访问web Server \r\n");

	i=0;
	while(1) 														/*循环执行的函数*/ 
	{
		i++;
		ii=i/1000;
		if(i%3000==0)
			LED_RGB111(ii%7);
    do_https();/*Web server测试程序*/
    if(reboot_flag==1)
    reboot();
	}
} 


