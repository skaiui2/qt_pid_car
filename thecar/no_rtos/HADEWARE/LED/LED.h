#ifndef __LED_H
#define __LED_H 			   
//#include "sys.h" 
#include"stm32f10x.h"

#define LED1_GPIO_PORT GPIOB 
#define LED1_GPIO_PIN  GPIO_Pin_5

#define LED2_GPIO_PORT GPIOE 
#define LED2_GPIO_PIN  GPIO_Pin_5

void LED_Config(void);

#endif


