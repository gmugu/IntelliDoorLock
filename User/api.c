#include "api.h"
#include "httputil.h"
#include "stdlib.h"
#include "fatfs_flash_spi.h"

#include "w5500_conf.h"
#include "lock.h"
#include "udp.h"
#include "bsp_beep.h"

typedef struct {
	uint8 dk_switch;
	uint8 dk_m_passwd[6];
	uint8 dk_p1_s;
	uint8 dk_p1_passwd[6];
	uint8 dk_p1_time;
	uint8 dk_p2_s;
	uint8 dk_p2_passwd[6];
	uint8 dk_p2_time;
	uint8 dk_p3_s;
	uint8 dk_p3_passwd[6];
	uint8 dk_p3_time;
	
	uint8 device_list[10][30];		//绑定设备列表
	
	uint8 visitor_switch;   		//访客模式开关
	
	uint8 admin_passwd[20];			//最长20位管理密码
	
} Model;
int cookie=3456766;
void strToAry(const char* str,uint8* ary){
	int i,len;
	len=strlen(str);
	for(i=0;i<len;i++){
		ary[i]=str[i];
	}
	ary[len] = 0;
}
void aryToStr(const uint8* ary,int size,char* str){
	int i;
	for(i = 0; i < size; i++){
		str[i] = ary[i];
	}
	str[size] = 0;
}
char* getValueByKey(char* key,char argkeys[][20],char argvalues[][200],int len){
	int i;
	for(i=0;i<len;i++){
		if(strcmp(key,argkeys[i])==0){
			return argvalues[i];
		}
	}
}

void initFlash(){
	Model model;
	memset(&model,0,sizeof(model));
	model.dk_switch = 4;
	
	strToAry("admin",model.admin_passwd);
	SPI_FLASH_SectorErase(0);
	SPI_FLASH_BufferWrite(&model.dk_switch,0,sizeof(Model));
	delay_us(10);
}
void api_getModel(SOCKET s){

	Model model;
	char dk_m_passwd[7]={0};

	char result[500] = {0};
	char deviceList[700]={0};
	int i,len;

	SPI_FLASH_BufferRead(&model.dk_switch, 0, sizeof(Model));
	delay_us(10);

	aryToStr(model.dk_m_passwd,6,dk_m_passwd);

	sprintf(deviceList,"[");
	for(i=0;i<10;i++){
		char name[13],code[19];
		uint8* tmp=model.device_list[i];
		aryToStr(tmp,12,name);
		if(strlen(name)==0){
			continue;
		}
		aryToStr(tmp+12,18,code);
		sprintf(deviceList,"%s{\"id\":%d,\"name\":\"%s\",\"code\":\"%s\"},",deviceList,i,name,code);
	}
	len = strlen(deviceList);
	if(len!=1){
		deviceList[len-1] = ']';
	}else{
		deviceList[len] = ']';
	}

	sprintf(result,"{\
\"dk_m_passwd\":\"%s\",\n\
\"a\":\"%s\",\n\
\"device_list\":%s\n\
}",
	dk_m_passwd,
	model.admin_passwd,
	deviceList);
	
	sendResponseJson(s,result);
}

extern int reboot_flag;
void api_setModel(SOCKET s,char argkeys[][20],char argvalues[][200],int len){
	Model model;
	int i,isVaild=0,isLogin=0;
	char str[30];
	char *admin_new_passwd=NULL;
	SPI_FLASH_BufferRead(&model.dk_switch, 0, sizeof(Model));
	delay_us(10);

	for(i=0;i<len;i++){
		printf("%s %s\n",argkeys[i],argvalues[i]);
		if(strcmp(argkeys[i],"passwd")==0){
			aryToStr(model.admin_passwd,20,str);
			if(strcmp(str,argvalues[i])==0){
				isLogin=1;
			}else{
				isLogin=0;
				break;
			}
		}else if(strcmp(argkeys[i],"dk_switch")==0){
			model.dk_switch = atoi(argvalues[i]);
		}else if(strcmp(argkeys[i],"dk_m_passwd")==0){
			strToAry(argvalues[i],model.dk_m_passwd);
		}else if(strcmp(argkeys[i],"dk_p1_s")==0){
			model.dk_p1_s = atoi(argvalues[i]);
		}else if(strcmp(argkeys[i],"dk_p1_passwd")==0){
			strToAry(argvalues[i],model.dk_p1_passwd);
		}else if(strcmp(argkeys[i],"dk_p1_time")==0){
			model.dk_p1_time=atoi(argvalues[i]);
		}else if(strcmp(argkeys[i],"dk_p2_s")==0){
			model.dk_p2_s = atoi(argvalues[i]);
		}else if(strcmp(argkeys[i],"dk_p2_passwd")==0){
			strToAry(argvalues[i],model.dk_p2_passwd);
		}else if(strcmp(argkeys[i],"dk_p2_time")==0){
			model.dk_p2_time=atoi(argvalues[i]);
		}else if(strcmp(argkeys[i],"dk_p3_s")==0){
			model.dk_p3_s = atoi(argvalues[i]);
		}else if(strcmp(argkeys[i],"dk_p3_passwd")==0){
			strToAry(argvalues[i],model.dk_p3_passwd);
		}else if(strcmp(argkeys[i],"dk_p3_time")==0){
			model.dk_p3_time=atoi(argvalues[i]);
		}else if(strcmp(argkeys[i],"visitor_switch")==0){
			model.visitor_switch = atoi(argvalues[i]);
		}else if(strcmp(argkeys[i],"rename_device")==0){
			char* v = argvalues[i];
			strToAry(v+1,model.device_list[v[0]-48]);
		}else if(strcmp(argkeys[i],"delete_device")==0){
			char* v = argvalues[i];
			memset(model.device_list[atoi(v)],0,30);
		}else if(strcmp(argkeys[i],"admin_old_passwd")==0){
			aryToStr(model.admin_passwd,20,str);
			if(strcmp(str,argvalues[i])==0){
				if(admin_new_passwd!=NULL){
					strToAry(admin_new_passwd,model.admin_passwd);
				}else{
					isVaild = 1;
				}
			}else{
				sendResponseJson(s,"原密码错误!");
				close(s);
			}
		}else if(strcmp(argkeys[i],"admin_new_passwd")==0){
			if(isVaild == 1){
				strToAry(argvalues[i],model.admin_passwd);
			}else{
				admin_new_passwd=argvalues[i];
			}
		}
	}
//	if(isLogin){

		SPI_FLASH_SectorErase(0);
		SPI_FLASH_BufferWrite((uint8*)&model,0,sizeof(model));
		sendResponseJson(s,"保存成功!");
		close(s);
//	}else{
//		sendResponseJson(s,"密码错误!");
//	}
}

void api_addDevice(SOCKET s,char argkeys[][20],char argvalues[][200],int len){
	char re[100];
	Model model;
	int i;

	SPI_FLASH_BufferRead(&model.dk_switch, 0, sizeof(Model));
	for(i=0;i<10;i++){
		char name[13];
		uint8* tmp=model.device_list[i];
		aryToStr(tmp,12,name);
		if(strlen(name)==0){
			break;
		}
	}
	if(i==10){
		sprintf(re,"{\"msg\":\"最多绑定10台设备\"}");
	}else{
		char m[20]="";
		getMac(m);
		sprintf(re,"{\"index\":%d,\"lockMac\":\"%s\"}",i,m);
	}
	sendResponseJson(s,re);
	close(s);
}

void api_bindDevice(SOCKET s,char argkeys[][20],char argvalues[][200],int len){
	int i,index;
	Model model;
	char name[13];
	char code[19];

	SPI_FLASH_BufferRead(&model.dk_switch, 0, sizeof(Model));
	delay_us(10);
	
	for(i=0;i<len;i++){
		printf("%s %s\n",argkeys[i],argvalues[i]);
		if(strcmp(argkeys[i],"index")==0){
			index=atoi(argvalues[i]);
		}else if(strcmp(argkeys[i],"name")==0){
			strcpy(name,argvalues[i]);
		}else if(strcmp(argkeys[i],"code")==0){
			strcpy(code,argvalues[i]);
		}
	}
	strToAry(name,model.device_list[index]);
	strToAry(code,model.device_list[index]+12);
	SPI_FLASH_SectorErase(0);
	SPI_FLASH_BufferWrite((uint8*)&model,0,sizeof(model));
	sendResponseJson(s,"绑定成功!");
	close(s);
}

void api_unlock(SOCKET s,char argkeys[][20],char argvalues[][200],int len){
	int i,j;
	Model model;
	unsigned char* phoneMac;
	
	SPI_FLASH_BufferRead(&model.dk_switch, 0, sizeof(Model));
	delay_us(10);
	for(i=0;i<len;i++){
		printf("%s %s\n",argkeys[i],argvalues[i]);
		if(strcmp(argkeys[i],"phoneMac")==0){
			for(j=0;j<10;j++){
				phoneMac=model.device_list[j]+12;
				if(strcmp((char*)phoneMac,argvalues[i])==0){
					sendResponseJson(s,"{\"code\":0}");
					close(s);
					unlock(1);
					
					return;
				}
			}
			sendResponseJson(s,"{\"code\":-1,\"msg\":\"未绑定的设备\"}");
			close(s);
			
		}
	}
}

void api_visitor_img(SOCKET s,char argkeys[][20],char argvalues[][200],int len){
	sendImg(s);
	close(s);
}
#include "webpge.h"
void api_login(SOCKET s,char argkeys[][20],char argvalues[][200],int len){
	Model model;
	int i;
	
	for(i=0;i<len;i++){
		printf("%s %s\n",argkeys[i],argvalues[i]);
		if(strcmp(argkeys[i],"adminpasswd")==0){
			SPI_FLASH_BufferRead(&model.dk_switch, 0, sizeof(Model));
			delay_us(10);
			if(strcmp((char*)(model.admin_passwd),argvalues[i])==0){
				char redirect[50];
				cookie++;
				sprintf(redirect,"HTTP/1.1 302 Move temporarily\r\nLocation: index.html?cookie=%d\r\n\r\n",cookie);
				send(s,(uint8*)redirect,strlen(redirect));
				close(s);
			}else{
				sendResponseHtml(s,LOGIN_HTML);
				close(s);
			}
		}
		
	}
	
	
}

void api_reboot(SOCKET s,char argkeys[][20],char argvalues[][200],int len){
	sendResponseJson(s,"api_reboot");
}

void api_debug(SOCKET s,char argkeys[][20],char argvalues[][200],int len){
	int i;
	char* method = getValueByKey("method",argkeys,argvalues,len);
	if(strcmp("flash_write",method)==0){
		
	}else if(strcmp("flash_read",method)==0){
		
	}
}

static int keys[20]={0};
static int index=0;
void key_input(int key){
	int i;
	macBEEP_ON();
	if(key=='D'){//按门铃单独处理
		sendUDP("visitor comming!");
	}else	if(key=='*'){//密码起始符
		index=0;
		memset(keys,0,sizeof(keys));
	}else if(key=='#'){//密码结束符
		Model model;
		SPI_FLASH_BufferRead(&model.dk_switch, 0, sizeof(Model));
		delay_us(10);
		for(i=0;i<6;i++){
			if(model.dk_m_passwd[i]!=keys[i]){
				printf("密码错误!index:%d\n",i);
				goto L1;
			}
		}
		unlock(0);
		L1:
		index=0;
		memset(keys,0,sizeof(keys));
	}else{
		keys[index]=key;
		index++;
		index%=10;
	}
	delay_us(50000);
	macBEEP_OFF();
}