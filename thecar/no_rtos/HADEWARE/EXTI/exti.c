#include "exti.h"

/************************
�ⲿ�жϣ�ͨ��mpu6050��INT���ţ��ϸ����PID��������    PB5����
**************************/
void MPU6050_EXTI_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
		EXTI_InitTypeDef EXTI_InitStructure;
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);   //�ⲿ�жϣ���Ҫʹ��AFIOʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ��GPIO�˿�ʱ��
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	          //�˿�����
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;         //��������
		GPIO_Init(GPIOA, &GPIO_InitStructure);					      //�����趨������ʼ��GPIO
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource12);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line12;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);
}

//���ȼ�����
void NVIC_Config(void)
{
	
	NVIC_InitTypeDef NVIC_InitStructure;
	

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//4����ռ,4����Ӧ��

    //�ⲿ�ж�
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

		//Usart3 NVIC ����
			NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
			NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
}




