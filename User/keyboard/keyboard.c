#include "stdio.h"
#include "keyboard.h"
#include "stm32f10x.h"
#include "utility.h"




static int preKey=-1;

//抬起识别
int intelli_key_scan(){
	int value;
	value=key_scan();
	if(value==-1){
		if(preKey==-1){//没有按下
			return -1;
		}else{//按键抬起
			value=preKey;
			preKey=-1;
			return value;
		}
	}else{//按键按下
		preKey=value;
		return -2;
	}
}

int key_scan(){
	u8 i=5,j=5;  
	u16 temp1,temp2; 

	GPIO_InitTypeDef GPIO_InitStructure;
 	
	RCC_APB2PeriphClockCmd(mac4x4_GPIO_CLK, ENABLE);
		
	GPIO_InitStructure.GPIO_Pin = HPin;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(mac4x4_GPIO_PORT, &GPIO_InitStructure);					
	
	GPIO_InitStructure.GPIO_Pin =  LPin;              
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(mac4x4_GPIO_PORT, &GPIO_InitStructure);

	GPIO_Write(mac4x4_GPIO_PORT,0xfff0);
	if((GPIO_ReadInputData(mac4x4_GPIO_PORT)&0x00f0)==0x00f0) 
	    return -1;
	else if((GPIO_ReadInputData(mac4x4_GPIO_PORT)&0x00f0)<0x00f0)
	 	{
			delay_ms(2);
			if((GPIO_ReadInputData(mac4x4_GPIO_PORT)&0x00f0)<0x00f0)
				{
					temp1=GPIO_ReadInputData(mac4x4_GPIO_PORT)&0x00f0;
					switch(temp1)
				  	{
					  case 0xe0:j=0;break;
					  case 0xd0:j=1;break;
					  case 0xb0:j=2;break;
					  case 0x70:j=3;break;
					  default:break;
				  	}
				}
		}

	RCC_APB2PeriphClockCmd(mac4x4_GPIO_CLK, ENABLE);
  
	GPIO_InitStructure.GPIO_Pin = LPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(mac4x4_GPIO_PORT, &GPIO_InitStructure);
	  
	GPIO_InitStructure.GPIO_Pin = HPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(mac4x4_GPIO_PORT, &GPIO_InitStructure);
	  
	GPIO_Write(mac4x4_GPIO_PORT,0xff0f);
 	if((GPIO_ReadInputData(mac4x4_GPIO_PORT)&0x000f)==0x000f)
    	return -1;
 	else if((GPIO_ReadInputData(mac4x4_GPIO_PORT)&0x000f)<0x000f)
		{
			delay_ms(2);
			if((GPIO_ReadInputData(mac4x4_GPIO_PORT)&0x000f)<0x000f)
 				{
					temp2=GPIO_ReadInputData(mac4x4_GPIO_PORT)&0x000f;
				  	switch(temp2)
				  	{
					  case 0x0e:i=0;break;
					  case 0x0d:i=1;break;
					  case 0x0b:i=2;break;
					  case 0x07:i=3;break;
					  default:break;
				  	}
				}
		}
	
	if((i==5)||(j==5))
    	return -1;
 	else
    	return (KEY_TAB[i][j]); 
	
}

#include "api.h"
void do_key(){
	int keyval=intelli_key_scan();
	if(keyval!=-1&&keyval!=-2){
		printf("key %c press up\n",keyval);
		key_input(keyval);
	}
}





