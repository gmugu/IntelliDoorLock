/**
******************************************************************************
* @file    			httputil.c
* @author  			WIZnet Software Team 
* @version 			V1.0
* @date    			2015-02-14
* @brief   	  	http中需要的实用函数
******************************************************************************
*/
#include "httputil.h"
#include "webpge.h"
#include "utility.h"
#include "api.h"
#include "ff.h"
#include "bsp_ov7725.h"
#define DEBUG_HTTP

extern CONFIG_MSG  ConfigMsg;
extern char tx_buf[MAX_URI_SIZE];
extern char rx_buf[MAX_URI_SIZE];
extern uint8 reboot_flag;

uint8 boundary[64];
uint8 tmp_buf[1460]={0xff,};
extern uint8 pub_buf[1024];

FIL fnew;													/* file objects */
FRESULT res_flash; 
UINT br, bw;            					/* File R/W count */
BYTE buffer[1024]={0};       		  /* file copy buffer */

/**
*@brief		将基本的配置信息设置到json_callback
*@param		无
*@return	无
*/
static void make_basic_config_setting_json_callback(int8* buf, CONFIG_MSG config_msg)
{
  sprintf(buf,"settingsCallback({\"ver\":\"%d.%d\",\
                \"mac\":\"%02X:%02X:%02X:%02X:%02X:%02X\",\
                \"ip\":\"%d.%d.%d.%d\",\
                \"gw\":\"%d.%d.%d.%d\",\
                \"sub\":\"%d.%d.%d.%d\",\
                });",config_msg.sw_ver[0],config_msg.sw_ver[1],
                config_msg.mac[0],config_msg.mac[1],config_msg.mac[2],config_msg.mac[3],config_msg.mac[4],config_msg.mac[5],
                config_msg.lip[0],config_msg.lip[1],config_msg.lip[2],config_msg.lip[3],
                config_msg.gw[0],config_msg.gw[1],config_msg.gw[2],config_msg.gw[3],
                config_msg.sub[0],config_msg.sub[1],config_msg.sub[2],config_msg.sub[3]
                );
}
/**
* url解析
*/
int parseUrl(char* url,char* path,char argkeys[][20],char argvalues[][200],int* arglen){
	char* tmp;
	char* tmp2;
	char* tmp3;
	int n;
	*arglen = 0;
	tmp = strstr(url,"/");
	tmp += strlen("/");
	tmp2 = strstr(tmp,"?");
	if(tmp2 == NULL){
		n = strlen(tmp);
		strncpy(path,tmp,n);
		path[n] = 0;
		return 1;
	}
	n = tmp2-tmp;
	strncpy(path,tmp,n);
	if(path[n-1]=='/'){
		path[n-1] = 0;
	}else{
		path[n] = 0;
	}
	tmp2 += strlen("?");

	while(tmp = strstr(tmp2,"&")){
		haha:
		tmp3 = strstr(tmp2,"=");
		if(tmp3 == NULL){
			goto hehe;
		}
		n = tmp3 - tmp2;
		strncpy(argkeys[*arglen],tmp2,n);
		argkeys[*arglen][n] = 0;
		tmp3 += strlen("=");
		n = tmp - tmp3;
		strncpy(argvalues[*arglen],tmp3,n);
		argvalues[*arglen][n] = 0;
		(*arglen)++;

		tmp2 = tmp + strlen("&");
	}
	tmp = url+strlen(url);
	goto haha;

	hehe:
	return 1;
}
/**
*@brief		完成http响应
*@param		无
*@return	无
*/
void do_https(void)
{
	uint8 ch=SOCK_HTTPS;																		/*定义一个socket*/
	uint16 len;
	
	st_http_request *http_request;													/*定义一个结构指针*/
	memset(rx_buf,0x00,MAX_URI_SIZE);
	http_request = (st_http_request*)rx_buf;			
	
	
	/* http service start */
	switch(getSn_SR(ch))																		/*获取socket状态*/
	{
		case SOCK_INIT:																				/*socket处于初始化状态*/
			listen(ch);
			break;
		
		case SOCK_LISTEN:																			/*socket处于监听状态*/
			break;
		
		case SOCK_ESTABLISHED:																/*socket处于连接状态*/
			if(getSn_IR(ch) & Sn_IR_CON)
			{
				setSn_IR(ch, Sn_IR_CON);													/*清除中断标志位*/
			}
			if ((len = getSn_RX_RSR(ch)) > 0)		
			{
				len = recv(ch, (uint8*)http_request, len); 				/*接收http请求*/
				*(((uint8*)http_request)+len) = 0;
				proc_http(ch, (uint8*)http_request);							/*接收http请求并发送http响应*/
				disconnect(ch);
			}
			break;
			
		case SOCK_CLOSE_WAIT:   															/*socket处于等待关闭状态*/
			if ((len = getSn_RX_RSR(ch)) > 0)
			{
				len = recv(ch, (uint8*)http_request, len);				/*接收http请求*/      
				*(((uint8*)http_request)+len) = 0;
				proc_http(ch, (uint8*)http_request);							/*接收http请求并发送http响应*/
			}
			disconnect(ch);
			break;
			
		case SOCK_CLOSED:                   									/*socket处于关闭状态*/
			socket(ch, Sn_MR_TCP, 80, 0x00);   									/*打开socket*/
			break;
		
		default:
			break;
	}
}

/**
*返回json数据
*/
void sendResponseJson(SOCKET s,char* json){
	uint16 send_len=0;
	uint8* http_response = (uint8*)rx_buf;
	unsigned long file_len=strlen(json);
	make_http_response_head((uint8*)http_response, PTYPE_TEXT,file_len);
	send(s,http_response,strlen((char const*)http_response));
	send_len=0;
	while(file_len){
		if(file_len>1024){
			if(getSn_SR(s)!=SOCK_ESTABLISHED){
				return;
			}
			send(s, (uint8 *)json+send_len, 1024);
			send_len+=1024;
			file_len-=1024;
		}else{
			send(s, (uint8 *)json+send_len, file_len);
			send_len+=file_len;
			file_len-=file_len;
		}
	}
}
/**
*返回html数据
*/
void sendResponseHtml(SOCKET s,char* path){
	uint16 send_len=0;
	uint8* http_response = (uint8*)rx_buf;
	unsigned long file_len;//strlen(path);
	res_flash = f_open(&fnew,path,FA_OPEN_EXISTING | FA_READ);
	if(res_flash != FR_OK){
		sendResponseJson(s,"404 not found!");
		return;
	}
	file_len = (&fnew)->fsize;
	make_http_response_head((uint8*)http_response, PTYPE_HTML,file_len);
	send(s,http_response,strlen((char const*)http_response));
	
	while(1){
		res_flash = f_read(&fnew,buffer,sizeof(buffer),&br);
		if(res_flash || br == 0){
			break;
		}
		send(s,buffer,br);
	}
	f_close(&fnew);
}

extern uint8_t Ov7725_vsync;
void sendImg(SOCKET s){
	int i,j;
	uint8 head[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\nContent-Length: 153600\r\n\r\n";
	uint8 data[640];
	
	uint16_t Camera_Data;
	while(Ov7725_vsync!=2);
	FIFO_PREPARE;
	for(i=0;i<240;i++){
		for(j=0;j<320;j++){
			READ_FIFO_PIXEL1(Camera_Data,Camera_Data);
		}
	}

	Ov7725_vsync = 0;

	send(s,head,strlen((char const*)head));

	while(Ov7725_vsync!=2);
	FIFO_PREPARE;

	for(i=0;i<240;i++){
		for(j=0;j<320;j++){
			READ_FIFO_PIXEL1(data[2*j],data[2*j+1]);
		}
		send(s,data,640);
	}
	Ov7725_vsync = 0;
}

/**
*@brief		接收http请求报文并发送http响应
*@param		s: http服务器socket
*@param		buf：解析报文内容
*@return	无
*/
void proc_http(SOCKET s, uint8 * buf)
{
	
	char path[30];
	char argkeys[20][20]={0};
	char argvalues[20][200]={0};
	int len;
	
	int8* name; 											
	int8 req_name[32]={0x00,};															/*定义一个http响应报文的指针*/
	unsigned long file_len=0;																/*定义http请求报文头的结构体指针*/
	uint16 send_len=0;
	uint8* http_response;
	st_http_request *http_request;
	memset(tx_buf,0x00,MAX_URI_SIZE);
	http_response = (uint8*)rx_buf;
	http_request = (st_http_request*)tx_buf;
	parse_http_request(http_request, buf);    							/*解析http请求报文头*/

	
	switch (http_request->METHOD)		
  {
		case METHOD_ERR :																			/*请求报文头错误*/
			memcpy(http_response, ERROR_REQUEST_PAGE, sizeof(ERROR_REQUEST_PAGE));
			send(s, (uint8 *)http_response, strlen((int8 const*)http_response));
			break;
		
		case METHOD_HEAD:																			/*HEAD请求方式*/
			
		case METHOD_GET:																			/*GET请求方式*/	
			
			parseUrl(http_request->URI,path,argkeys,argvalues,&len);

			printf("parseUrl-->URI:%s  path:%s  len:%d\n",http_request->URI,path,len);
			if(strcmp(path,"index.htm")==0 || strcmp(path,"")==0 || (strcmp(path,"index.html")==0)){
				sendResponseJson(s,INDEX_HTML);
			}else if(strcmp(path,"unlock.cgi")==0){
				api_unlock(s,argkeys,argvalues,len);
			}else if(strcmp(path,"getModel.cgi")==0){
				api_getModel(s);
			}else if(strcmp(path,"setModel.cgi")==0){
				api_setModel(s,argkeys,argvalues,len);
			}else if(strcmp(path,"addDevice.cgi")==0){
				api_addDevice(s,argkeys,argvalues,len);
			}else if(strcmp(path,"bindDevice.cgi")==0){
				api_bindDevice(s,argkeys,argvalues,len);
			}else if(strcmp(path,"visitor.img")==0){
				api_visitor_img(s,argkeys,argvalues,len);
			}else if(strcmp(path,"reboot.cgi")==0){
				api_reboot(s,argkeys,argvalues,len);
			}else if(strcmp(path,"debug.cgi")==0){
				api_debug(s,argkeys,argvalues,len);
			}else{
				sendResponseHtml(s,path);
			}
			break;
			
		case METHOD_POST:
			break;
			
		default :
			break;
	}
}

/**
*@brief		执行http响应
*@param		无  
*@return	无
*/
void make_cgi_response(uint16 delay, int8* url,int8* cgi_response_buf)
{
  sprintf(cgi_response_buf,"<html><head><title>iWeb - Configuration</title><script language=javascript>j=%d;function func(){document.getElementById('delay').innerText=' '+j + ' ';j--;setTimeout('func()',1000);if(j==0)location.href='http://%d.%d.%d.%d/';}</script></head><body onload='func()'>please wait for a while, the module will boot in<span style='color:red;' id='delay'></span> seconds.</body></html>",delay,url[0],url[1],url[2],url[3]);
  return;
}


