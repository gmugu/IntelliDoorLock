#ifndef __HTTP_CLIENT_H
#define __HTTP_CLIENT_H
#include "stm32f10x.h"
#include "types.h"

void sendBuf(char* buf);

uint8 do_http_client(void);//TCP Clinet回环演示函数
#endif 

