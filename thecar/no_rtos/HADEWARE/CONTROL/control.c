#include "control.h"	
#include "stm32f10x_it.h" 
#include "tracking.h"
/**************************************************************************
函数功能：
		 所有的控制代码都在这里面
         5ms定时中断由MPU6050的INT引脚触发
         严格保证采样和数据处理的时间同步				 
**************************************************************************/
float Pitch,Roll,Yaw;						//角度
short gyrox,gyroy,gyroz;					//陀螺仪--角速度
short aacx,aacy,aacz;						//加速度
int Encoder_Left,Encoder_Right;             //左右编码器的脉冲计数
int Balance_Pwm,Velocity_Pwm,Turn_Pwm;		//角度环输出、速度环输出、转向环输出
int Moto1,Moto2;                            //电机PWM变量 应是Motor的 向Moto致敬		
int Target_Speed=0 ;
int Turn_Speed=0;		//期望速度（偏航）


//机械中值
float ZHONGZHI = -8;

//角度环系数
float 
	Balance_Kp=-500,//-500
	Balance_Kd=-2.5;//-2.5

//速度环系数
float 
	Velocity_Kp=380,//400
	Velocity_Ki=380/200;

float 
	Turn_Kd=-1.5,//转向环KP、KD -1
	Turn_Kp=-20;	//
	
#define SPEED_Y 50 //俯仰(前后)最大设定速度//50
#define SPEED_Z 150//偏航(左右)最大设定速度 //150

int EXTI15_10_IRQHandler(void)
{
	int PWM_out;
	
	if(EXTI_GetITStatus(EXTI_Line12)!=0)//一级判定
	{
		if(PAin(12)==0)//二级判定
		{
			EXTI_ClearITPendingBit(EXTI_Line12);//清除中断标志位
			
			//进入中断首先读取数据----编码器数据和角度数据
			Encoder_Left=Read_Encoder(2);               //===读取编码器的值
			Encoder_Right=Read_Encoder(4);              //===读取编码器的值
			
			mpu_dmp_get_data(&Pitch,&Roll,&Yaw);		//角度
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//陀螺仪
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//加速度
			
			
			if(Flag_Qian==0 && Flag_Hou==0&&Flag_Left==0&&Flag_Left==0) Target_Speed=0;//未接受到前进后退指令-->速度清零，稳在原地
			if(Flag_Qian==1)Target_Speed = 30;//前进1标志位拉高-->需要前进
			if(Flag_Hou==1)Target_Speed = -30;//
			Tracking_detection();
			Target_Speed=Target_Speed>SPEED_Y?SPEED_Y:(Target_Speed<-SPEED_Y?(-SPEED_Y):Target_Speed);
		
			//左右
			if(Flag_Left==2)Turn_Speed-=5;	//左转
			if(Flag_Right==2)Turn_Speed+=5;	//右转
			if((Flag_Left==0)&&(Flag_Right==0))Turn_Speed=0;
			if(Flag_Left==1)Turn_Speed-=30;	//左转
			if(Flag_Right==1)Turn_Speed+=30;	//右转
			Turn_Speed=Turn_Speed>SPEED_Z?SPEED_Z:(Turn_Speed<-SPEED_Z?(-SPEED_Z):Turn_Speed);//限幅( (20*100) * 100   )
			
			//转向约束
			if((Flag_Left==0)&&(Flag_Right==0))Turn_Kd=-1.4;//若无左右转向指令，则开启转向约束
			else if((Flag_Left==1)||(Flag_Right==1))Turn_Kd=0;//若左右转向指令接收到，则去掉转向约束
			
			Balance_Pwm =balance(Pitch,gyroy);          //===角度环
			Velocity_Pwm=velocity(Encoder_Left,Encoder_Right);                  //===速度环PID控制	 记住，速度反馈是正反馈，就是小车快的时候要慢下来就需要再跑快一点
			Turn_Pwm =turn(Encoder_Left,Encoder_Right,gyroz);         	//===转向环PID控制     
			
			
			PWM_out=Balance_Pwm +Velocity_Pwm;//最终输出
			Moto1=PWM_out-Turn_Pwm;                            //===计算左轮电机最终PWM
			Moto2=PWM_out+Turn_Pwm;                            //===计算右轮电机最终PWM
			Xianfu_Pwm();   //===PWM限幅
			
			Set_Pwm(Moto1,Moto2);     //===赋值给PWM寄存器  
		
		}
	}
	return 0;	
}


/**************************************************************************
函数功能：直立PD控制
入口参数：角度、角速度
返回  值：直立控制PWM
作    者：平衡小车之家
**************************************************************************/
int balance(float Angle,float Gyro)
{  
	float Bias;
	int balance;
	Bias=Angle-ZHONGZHI;                       //===求出平衡的角度中值 和机械相关
	balance=Balance_Kp*Bias+Gyro*Balance_Kd;   //===计算平衡控制的电机PWM  PD控制   kp是P系数 kd是D系数 
	return balance;
}

/**************************************************************************
函数功能：速度PI控制 修改前进后退速度，请修Target_Velocity，比如，改成60就比较慢了
入口参数：左轮编码器、右轮编码器
返回  值：速度控制PWM
作    者：平衡小车之家
**************************************************************************/
int velocity(int encoder_left,int encoder_right)
{  
     static float Velocity,Encoder_Least,Encoder,Movement;
	  static float Encoder_Integral,Target_Velocity;
   //=============速度PI控制器=======================//	
		Encoder_Least =(encoder_left+encoder_right)-Target_Speed;                    //===获取最新速度偏差==测量速度（左右编码器之和）-目标速度（此处为零） 
		Encoder *= 0.84;		                                                //===一阶低通滤波器       
		Encoder += Encoder_Least*0.16;	                                    //===一阶低通滤波器    
		Encoder_Integral +=Encoder;                                       //===积分出位移 积分时间：10ms
		Encoder_Integral=Encoder_Integral-Target_Speed;                       //===接收遥控器数据，控制前进后退
		if(Encoder_Integral>10000)  	Encoder_Integral=10000;             //===积分限幅
		if(Encoder_Integral<-10000)	Encoder_Integral=-10000;              //===积分限幅	
		Velocity=Encoder*Velocity_Kp+Encoder_Integral*Velocity_Ki;        //===速度控制	
	  return Velocity;
}

/**************************************************************************
函数功能：异常关闭电机
入口参数：倾角和电压
返回  值：1：异常  0：正常
**************************************************************************/
u8 Turn_Off(float angle, int voltage)
{
	    u8 temp;
			if(angle<-60||angle>60||1==Flag_Stop||voltage<1000)//电池电压低于11.1V关闭电机
			{	                                                 //===倾角大于40度关闭电机
      temp=1;                                            //===Flag_Stop置1关闭电机
			AIN1=0;                                            
			AIN2=0;
			BIN1=0;
			BIN2=0;
      }
			else
      temp=0;
      return temp;			
}

/**************************************************************************
函数功能：限制PWM赋值 
入口参数：无
返回  值：无
**************************************************************************/
void Xianfu_Pwm(void)
{	
	  int Amplitude=5200;    //===PWM满幅是7200 限制在6900
//		if(Flag_Qian==1)  Moto1+=DIFFERENCE;  //DIFFERENCE是一个衡量平衡小车电机和机械安装差异的一个变量。直接作用于输出，让小车具有更好的一致性。
//	  if(Flag_Hou==1)   Moto2-=DIFFERENCE;
    if(Moto1<-Amplitude) Moto1=-Amplitude;	
		if(Moto1>Amplitude)  Moto1=Amplitude;	
	  if(Moto2<-Amplitude) Moto2=-Amplitude;	
		if(Moto2>Amplitude)  Moto2=Amplitude;		
}

/**************************************************************************
函数功能：转向控制  修改转向速度，请修改Turn_Amplitude即可
入口参数：左轮编码器、右轮编码器、Z轴陀螺仪
返回  值：转向控制PWM
作    者：平衡小车之家
**************************************************************************/
int turn(int encoder_left,int encoder_right,float gyro)//转向控制
{
	int PWM_out;
	//这不是一个严格的PD控制器，Kd针对的是转向的约束，但Kp针对的是遥控的转向。
	PWM_out= Turn_Kd*gyro + Turn_Kp*Turn_Speed;
	return PWM_out;
}
/**************************************************************************
函数功能：绝对值函数
入口参数：int
返回  值：unsigned int
**************************************************************************/
int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}
/**************************************************************************
函数功能：赋值给PWM寄存器
入口参数：左轮PWM、右轮PWM
返回  值：无
**************************************************************************/
void Set_Pwm(int moto1,int moto2)
{
    	if(moto1<0)			AIN2=0,			AIN1=1;
			else 	          AIN2=1,			AIN1=0;
			PWMA=myabs(moto1);
		  if(moto2>0)	BIN1=1,			BIN2=0;
			else        BIN1=0,			BIN2=1;
			PWMB=myabs(moto2);	
}


