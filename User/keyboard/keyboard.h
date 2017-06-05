
#ifndef __KEY_H
#define __KEY_H


/************************** 4*4键盘连接引脚定义********************************/
#define      mac4x4_SCK_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define      mac4x4_GPIO_CLK                       RCC_APB2Periph_GPIOF
#define      mac4x4_GPIO_PORT                      GPIOF
#define      mac4x4_0_GPIO_PIN                     GPIO_Pin_0
#define      mac4x4_1_GPIO_PIN                     GPIO_Pin_1
#define      mac4x4_2_GPIO_PIN                     GPIO_Pin_2
#define      mac4x4_3_GPIO_PIN                     GPIO_Pin_3
#define      mac4x4_4_GPIO_PIN                     GPIO_Pin_4
#define      mac4x4_5_GPIO_PIN                     GPIO_Pin_5
#define      mac4x4_6_GPIO_PIN                     GPIO_Pin_6
#define      mac4x4_7_GPIO_PIN                     GPIO_Pin_7

// #define      mac4x4_0_GPIO_PIN                     GPIO_Pin_8
// #define      mac4x4_1_GPIO_PIN                     GPIO_Pin_9
// #define      mac4x4_2_GPIO_PIN                     GPIO_Pin_10
// #define      mac4x4_3_GPIO_PIN                     GPIO_Pin_11
// #define      mac4x4_4_GPIO_PIN                     GPIO_Pin_12
// #define      mac4x4_5_GPIO_PIN                     GPIO_Pin_13
// #define      mac4x4_6_GPIO_PIN                     GPIO_Pin_14
// #define      mac4x4_7_GPIO_PIN                     GPIO_Pin_15

#define HPin  mac4x4_3_GPIO_PIN|mac4x4_2_GPIO_PIN|mac4x4_1_GPIO_PIN|mac4x4_0_GPIO_PIN
#define LPin  mac4x4_7_GPIO_PIN|mac4x4_6_GPIO_PIN|mac4x4_5_GPIO_PIN|mac4x4_4_GPIO_PIN

typedef unsigned char u8;
typedef unsigned short u16;

static int KEY_TAB[4][4]={
{'A','B','C','D'},
{'3','6','9','#'},
{'2','5','8','0'},
{'1','4','7','*'}
};
int key_scan();
int intelli_key_scan();
void do_key();

#endif
