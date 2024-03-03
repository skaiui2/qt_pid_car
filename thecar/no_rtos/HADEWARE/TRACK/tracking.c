#include "tracking.h"
#include "control.h"
#include "stm32f10x_it.h"
extern float Turn_Speed;
extern float Target_Speed;

/**************************************************************************
�������ܣ�ѭ��ģ��IO�ڳ�ʼ��
��ڲ�������
����  ֵ����
**************************************************************************/
void Tracking_Init(void)
{
	
		GPIO_InitTypeDef GPIO_InitStructure;

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��A\Bʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��A\Bʱ��
		
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1;//PB 0 1
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //���ó���������
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6|GPIO_Pin_7;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
} 



/**************************************************************************
�������ܣ�ѭ��ģ�飬�߼�����
��ڲ�������
����  ֵ����
**************************************************************************/	
extern float yaw;
void Tracking_detection(void)
{
	
	if((Left_Tracking==0 && EndRight_Tracking == 0 && Mid_Tracking==0 && Right_Tracking==0))
	{
		
	}
	else if(Mid_Tracking==1)  //�����ת
	{
		
		Flag_Left=2/5;
		
	}
	else if(Right_Tracking==1)  
	{
		Flag_Right=2/5;
		
	}
	
	else if(Left_Tracking==1)  //�����ת
	{
		
		Flag_Left=2;
		
	}
	else if(EndRight_Tracking==1)  
	{
		Flag_Right=2;
		
	}
	
	else
	{
		
	}
	
		
	
	
  
}