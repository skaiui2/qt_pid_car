#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"

int EXTI15_10_IRQHandler(void);
int balance(float angle,float gyro);
int velocity(int encoder_left,int encoder_right);
int turn(int encoder_left,int encoder_right,float gyro);
u8 Turn_Off(float angle, int voltage);
void Xianfu_Pwm(void);
int turn(int encoder_left,int encoder_right,float gyro);//×ªÏò¿ØÖÆ
void Set_Pwm(int moto1,int moto2);
int myabs(int a);


#endif






