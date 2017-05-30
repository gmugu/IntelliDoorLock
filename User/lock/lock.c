#include "lock.h"
#include "stdio.h"
#include "fatfs_flash_spi.h"
#include "http_client.h"
#include "w5500_conf.h"
void unlock(){
	char buf[100];
	char mac[30];
	printf("unlocking.... \n");
	
	getMac(mac);
	sprintf(buf,"GET /uploadlog?lockMac=%s HTTP/1.1\r\nHost: www.gmugu.com\r\n\r\n",mac);
	sendBuf(buf);
}
