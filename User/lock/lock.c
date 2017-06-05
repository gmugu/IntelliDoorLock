#include "lock.h"
#include "stdio.h"
#include "fatfs_flash_spi.h"
#include "http_client.h"
#include "w5500_conf.h"

volatile u32 time;
void Lock_Config(){
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����LOCK������ʱ��*/
	LOCK_GPIO_FUNC( LOCK_GPIO_CLK, ENABLE); 

	/*ѡ��Ҫ���Ƶ�����*/															   
	GPIO_InitStructure.GPIO_Pin = LOCK_PIN;	

	/*��������ģʽΪͨ�ÿ�©���*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;   

	/*������������Ϊ50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*���ÿ⺯������ʼ��*/
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