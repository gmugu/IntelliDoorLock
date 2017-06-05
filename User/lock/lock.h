
#ifndef _LOCK_H_
#define _LOCK_H_

#define LOCK_PIN                  GPIO_Pin_9
#define LOCK_GPIO_PORT            GPIOD
#define LOCK_GPIO_CLK             RCC_APB2Periph_GPIOD
#define LOCK_GPIO_FUNC            RCC_APB2PeriphClockCmd

#define LOCK(a)	if (a)	\
					GPIO_SetBits(LOCK_GPIO_PORT,LOCK_PIN);\
					else		\
					GPIO_ResetBits(LOCK_GPIO_PORT,LOCK_PIN)


void Lock_Config();

void unlock(int type);
void do_lock();


#endif