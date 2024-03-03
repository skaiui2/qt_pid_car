#ifndef  _PWM_H
#define  _PWM_H

#include "sys.h" 

#define PWMA   TIM1->CCR1  //PA8
#define AIN1   PBout(14)
#define AIN2   PBout(15)

#define BIN1   PBout(13)
#define BIN2   PBout(12)
#define PWMB   TIM1->CCR4  //PA11


void PWM_Init_TIM1(u16 Per,u16 Psc);
void Motor_Init(void);
#endif
