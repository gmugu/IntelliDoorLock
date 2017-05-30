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

FATFS fs;
uint8 reboot_flag = 0;
extern uint8_t Ov7725_vsync;

static UINT br, bw;
static FIL fnew;	
static FRESULT res_flash; 
static BYTE textFileBuffer[] = "欢迎使用野火STM32-V3 ISO开发板  今天是个好日子，新建文件系统测试文件";
static BYTE buffer[1024]={0};

int main(void)
{
	
	int i,ii,keyval;
	systick_init(72);										//初始化Systick工作时钟
	USART1_Config();										//初始化串口通信:115200@8-n-1
	LED_GPIO_Config();									//初始化led

	TM_FATFS_FLASH_SPI_disk_initialize();										//初始化8M串行flash
	{
		f_mount(&fs,"0:",1);			//挂载文件系统
		
		
		printf("\r\n f_mount res_flash=%d \r\n",res_flash);
	
	//如果没有文件系统就格式化创建创建文件系统
	if(res_flash ==FR_NO_FILESYSTEM)
	{
		res_flash=f_mkfs("0:",0,4096);							//格式化
		printf("\r\nmkfs res_flash=%d",res_flash);
		res_flash = f_mount(&fs,"0:",0);						//格式化后，先取消挂载
		res_flash = f_mount(&fs,"0:",1);						//重新挂载
	}
/**************************  flash   *****************************************/		
	//文件系统测试，写测试
	//打开文件，如果文件不参加则创建它
	res_flash = f_open(&fnew, "0:flashnewfile.txt", FA_CREATE_ALWAYS | FA_WRITE );
	 
	if ( res_flash == FR_OK )
	{
		res_flash = f_write(&fnew, textFileBuffer, sizeof(textFileBuffer), &bw);
		f_close(&fnew);      
	}
	//读测试
	res_flash = f_open(&fnew, "0:flashnewfile.txt", FA_OPEN_EXISTING | FA_READ); 	 
	res_flash = f_read(&fnew, buffer, sizeof(buffer), &br); 
	printf("\n s %s b \n", buffer);
	/* Close open files */
	res_flash = f_close(&fnew);	

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


