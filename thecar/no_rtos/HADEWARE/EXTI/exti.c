#include "exti.h"

/************************
外部中断，通过mpu6050的INT引脚，严格控制PID计算周期    PB5引脚
**************************/
void MPU6050_EXTI_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
		EXTI_InitTypeDef EXTI_InitStructure;
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);   //外部中断，需要使能AFIO时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能GPIO端口时钟
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	          //端口配置
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;         //上拉输入
		GPIO_Init(GPIOA, &GPIO_InitStructure);					      //根据设定参数初始化GPIO
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource12);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line12;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);
}

//优先级配置
void NVIC_Config(void)
{
	
	NVIC_InitTypeDef NVIC_InitStructure;
	

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//4级抢占,4级响应。

    //外部中断
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

		//Usart3 NVIC 配置
			NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
			NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}





