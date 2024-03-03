#ifndef  _ENCODER_H
#define  _ENCODER_H

#include "sys.h" 



#define ENCODER_TIM_PERIOD (u16)(65535)
void TIM2_IRQHandler(void);
void TIM4_IRQHandler(void);
int Read_Encoder(u8 TIMX);
void Encoder_TIM2_Init(void);
void Encoder_TIM4_Init(void);
#endif


