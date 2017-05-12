#ifndef api__h
#define api__h
#include "types.h"

void api_getModel(SOCKET s);
void api_setModel(SOCKET s,char argkeys[][20],char argvalues[][200],int len);
void api_addDevice(SOCKET s,char argkeys[][20],char argvalues[][200],int len);
void api_bindDevice(SOCKET s,char argkeys[][20],char argvalues[][200],int len);
void api_visitor_img(SOCKET s,char argkeys[][20],char argvalues[][200],int len);
void api_unlock(SOCKET s,char argkeys[][20],char argvalues[][200],int len);
void api_reboot(SOCKET s,char argkeys[][20],char argvalues[][200],int len);
void api_debug(SOCKET s,char argkeys[][20],char argvalues[][200],int len);

#endif