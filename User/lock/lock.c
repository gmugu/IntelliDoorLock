#include "lock.h"
#include "stdio.h"
#include "fatfs_flash_spi.h"
#include "http_client.h"
#include "w5500_conf.h"

volatile u32 time;
void Lock_Config(){
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启LOCK的外设时钟*/
	LOCK_GPIO_FUNC( LOCK_GPIO_CLK, ENABLE); 

	/*选择要控制的引脚*/															   
	GPIO_InitStructure.GPIO_Pin = LOCK_PIN;	

	/*设置引脚模式为通用开漏输出*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;   

	/*设置引脚速率为50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*调用库函数，初始化*/
	GPIO_Init(LOCK_GPIO_PORT, &GPIO_InitStructure);	

	LOCK(1);
	 
	
}
static bool isOpen=FALSE;
void unlock(int type){
	char buf[100];
	char mac[30];
	printf("unlocking.... \n");
	
	LOCK(0);
	isOpen=TRUE;
	time=0;
	
	getMac(mac);
	sprintf(buf,"GET /uploadlog?lockMac=%s&event=%d HTTP/1.1\r\nHost: www.gmugu.com\r\n\r\n",mac,type);
	sendBuf(buf);
}

void do_lock(){

	if(isOpen==TRUE&&(4000<=time&&time<=6000)){
		printf("LOCK OFF\n");
		LOCK(1);
		isOpen=FALSE;
		time=0;
	}
}