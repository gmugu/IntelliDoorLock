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
#include "api.h"
#include "udp.h"

/*app函数头文件*/
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
	systick_init(72);										//初始化Systick工作时钟
	USARTx_Config();										//初始化串口通信:115200@8-n-1
	LED_GPIO_Config();									//初始化led

	TIMx_Configuration();																		// TIM2 定时配置
	TIMx_NVIC_Configuration();															// 定时器的中断优先级
	macTIM_APBxClock_FUN (macTIM_CLK, ENABLE);							// TIM2 重新开时钟，开始计时
	
	TM_FATFS_FLASH_SPI_disk_initialize();										//初始化8M串行flash
	
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_AFIO, ENABLE );
	Ov7725_GPIO_Config();				// ov7725 gpio 初始化
	while(Ov7725_Init() != SUCCESS);    // ov7725 寄存器初始化
	VSYNC_Init();	    // ov7725 场信号线初始化
	Ov7725_vsync = 0;
	
	gpio_for_w5500_config();						//初始化MCU相关引脚
	reset_w5500();											//硬复位W5500
	set_w5500_mac();										//配置MAC地址
	set_w5500_ip();											//配置IP地址
	socket_buf_init(txsize, rxsize);		//初始化8个Socket的发送接收缓存大小
	
	Lock_Config(); 	
	Beep_Init();
	
	printf(" 应用程序执行中…… \r\n"); 

	while(1) 														/*循环执行的函数*/ 
	{
		do_lde();
    do_https();//Web server
		do_https1();
		do_http_client();//http client
		do_udp();//udp client
		do_key();//键盘监听

    if(reboot_flag==1){
			reboot();
		}

	}
} 


