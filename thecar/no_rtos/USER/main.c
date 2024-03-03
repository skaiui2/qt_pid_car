#include "stm32f10x.h"
#include "sys.h" 
#include "usart.h"
#include "stm32f10x_it.h" 
#include "encoder.h"
#include "tracking.h"
extern int  Target_Speed;
/************************************************
 ALIENTEK 战舰STM32F103开发板实验0
 工程模板
 注意，这是手册中的新建工程章节使用的main文件 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/


 void Delay(u32 count)
 {
   u32 i=0;
   for(;i<count;i++);
 }
 
 int main(void)
 {	
	 
	delay_init();
	NVIC_Config(); 
	OLED_Init();
	OLED_Clear();
	MPU_Init();
	
	GPIO_SetBits(GPIOA,GPIO_Pin_12);
	mpu_dmp_init();
	
	MPU6050_EXTI_Init();
	Motor_Init();
	PWM_Init_TIM1(7199,0);
	 uart_init(115200);
	uart3_init(9600);

	
	              
	Encoder_TIM2_Init();
	Encoder_TIM4_Init();	
	Tracking_Init();
	
	while(1)
	{
		//测试pwm与电机
		/*OLED_Float(1,10,Read_Encoder(2),3);
		OLED_Float(4,10,Read_Encoder(4),3);
		TIM_SetCompare1(TIM1,40);	
		TIM_SetCompare4(TIM1,40);	
		
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);GPIO_SetBits(GPIOB,GPIO_Pin_13);
		GPIO_SetBits(GPIOB,GPIO_Pin_14);GPIO_ResetBits(GPIOB,GPIO_Pin_15);*/		
		
	  OLED_Float(0,10,Pitch,3);
		OLED_Float(1,10,Roll,3);
		OLED_Float(2,10,Yaw,3);
		OLED_Float(3,10,Read_Encoder(4),3);
		OLED_Float(4,10,Read_Encoder(2),3);
		OLED_Float(5,10,Usart3_Receive,3);
			
		
		
		
	}
 }
