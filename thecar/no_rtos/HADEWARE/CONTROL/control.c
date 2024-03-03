#include "control.h"	
#include "stm32f10x_it.h" 
#include "tracking.h"
/**************************************************************************
�������ܣ�
		 ���еĿ��ƴ��붼��������
         5ms��ʱ�ж���MPU6050��INT���Ŵ���
         �ϸ�֤���������ݴ�����ʱ��ͬ��				 
**************************************************************************/
float Pitch,Roll,Yaw;						//�Ƕ�
short gyrox,gyroy,gyroz;					//������--���ٶ�
short aacx,aacy,aacz;						//���ٶ�
int Encoder_Left,Encoder_Right;             //���ұ��������������
int Balance_Pwm,Velocity_Pwm,Turn_Pwm;		//�ǶȻ�������ٶȻ������ת�����
int Moto1,Moto2;                            //���PWM���� Ӧ��Motor�� ��Moto�¾�		
int Target_Speed=0 ;
int Turn_Speed=0;		//�����ٶȣ�ƫ����


//��е��ֵ
float ZHONGZHI = -8;

//�ǶȻ�ϵ��
float 
	Balance_Kp=-500,//-500
	Balance_Kd=-2.5;//-2.5

//�ٶȻ�ϵ��
float 
	Velocity_Kp=380,//400
	Velocity_Ki=380/200;

float 
	Turn_Kd=-1.5,//ת��KP��KD -1
	Turn_Kp=-20;	//
	
#define SPEED_Y 50 //����(ǰ��)����趨�ٶ�//50
#define SPEED_Z 150//ƫ��(����)����趨�ٶ� //150

int EXTI15_10_IRQHandler(void)
{
	int PWM_out;
	
	if(EXTI_GetITStatus(EXTI_Line12)!=0)//һ���ж�
	{
		if(PAin(12)==0)//�����ж�
		{
			EXTI_ClearITPendingBit(EXTI_Line12);//����жϱ�־λ
			
			//�����ж����ȶ�ȡ����----���������ݺͽǶ�����
			Encoder_Left=Read_Encoder(2);               //===��ȡ��������ֵ
			Encoder_Right=Read_Encoder(4);              //===��ȡ��������ֵ
			
			mpu_dmp_get_data(&Pitch,&Roll,&Yaw);		//�Ƕ�
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//������
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//���ٶ�
			
			
			if(Flag_Qian==0 && Flag_Hou==0&&Flag_Left==0&&Flag_Left==0) Target_Speed=0;//δ���ܵ�ǰ������ָ��-->�ٶ����㣬����ԭ��
			if(Flag_Qian==1)Target_Speed = 30;//ǰ��1��־λ����-->��Ҫǰ��
			if(Flag_Hou==1)Target_Speed = -30;//
			Tracking_detection();
			Target_Speed=Target_Speed>SPEED_Y?SPEED_Y:(Target_Speed<-SPEED_Y?(-SPEED_Y):Target_Speed);
		
			//����
			if(Flag_Left==2)Turn_Speed-=5;	//��ת
			if(Flag_Right==2)Turn_Speed+=5;	//��ת
			if((Flag_Left==0)&&(Flag_Right==0))Turn_Speed=0;
			if(Flag_Left==1)Turn_Speed-=30;	//��ת
			if(Flag_Right==1)Turn_Speed+=30;	//��ת
			Turn_Speed=Turn_Speed>SPEED_Z?SPEED_Z:(Turn_Speed<-SPEED_Z?(-SPEED_Z):Turn_Speed);//�޷�( (20*100) * 100   )
			
			//ת��Լ��
			if((Flag_Left==0)&&(Flag_Right==0))Turn_Kd=-1.4;//��������ת��ָ�����ת��Լ��
			else if((Flag_Left==1)||(Flag_Right==1))Turn_Kd=0;//������ת��ָ����յ�����ȥ��ת��Լ��
			
			Balance_Pwm =balance(Pitch,gyroy);          //===�ǶȻ�
			Velocity_Pwm=velocity(Encoder_Left,Encoder_Right);                  //===�ٶȻ�PID����	 ��ס���ٶȷ�����������������С�����ʱ��Ҫ����������Ҫ���ܿ�һ��
			Turn_Pwm =turn(Encoder_Left,Encoder_Right,gyroz);         	//===ת��PID����     
			
			
			PWM_out=Balance_Pwm +Velocity_Pwm;//�������
			Moto1=PWM_out-Turn_Pwm;                            //===�������ֵ������PWM
			Moto2=PWM_out+Turn_Pwm;                            //===�������ֵ������PWM
			Xianfu_Pwm();   //===PWM�޷�
			
			Set_Pwm(Moto1,Moto2);     //===��ֵ��PWM�Ĵ���  
		
		}
	}
	return 0;	
}


/**************************************************************************
�������ܣ�ֱ��PD����
��ڲ������Ƕȡ����ٶ�
����  ֵ��ֱ������PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int balance(float Angle,float Gyro)
{  
	float Bias;
	int balance;
	Bias=Angle-ZHONGZHI;                       //===���ƽ��ĽǶ���ֵ �ͻ�е���
	balance=Balance_Kp*Bias+Gyro*Balance_Kd;   //===����ƽ����Ƶĵ��PWM  PD����   kp��Pϵ�� kd��Dϵ�� 
	return balance;
}

/**************************************************************************
�������ܣ��ٶ�PI���� �޸�ǰ�������ٶȣ�����Target_Velocity�����磬�ĳ�60�ͱȽ�����
��ڲ��������ֱ����������ֱ�����
����  ֵ���ٶȿ���PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int velocity(int encoder_left,int encoder_right)
{  
     static float Velocity,Encoder_Least,Encoder,Movement;
	  static float Encoder_Integral,Target_Velocity;
   //=============�ٶ�PI������=======================//	
		Encoder_Least =(encoder_left+encoder_right)-Target_Speed;                    //===��ȡ�����ٶ�ƫ��==�����ٶȣ����ұ�����֮�ͣ�-Ŀ���ٶȣ��˴�Ϊ�㣩 
		Encoder *= 0.84;		                                                //===һ�׵�ͨ�˲���       
		Encoder += Encoder_Least*0.16;	                                    //===һ�׵�ͨ�˲���    
		Encoder_Integral +=Encoder;                                       //===���ֳ�λ�� ����ʱ�䣺10ms
		Encoder_Integral=Encoder_Integral-Target_Speed;                       //===����ң�������ݣ�����ǰ������
		if(Encoder_Integral>10000)  	Encoder_Integral=10000;             //===�����޷�
		if(Encoder_Integral<-10000)	Encoder_Integral=-10000;              //===�����޷�	
		Velocity=Encoder*Velocity_Kp+Encoder_Integral*Velocity_Ki;        //===�ٶȿ���	
	  return Velocity;
}

/**************************************************************************
�������ܣ��쳣�رյ��
��ڲ�������Ǻ͵�ѹ
����  ֵ��1���쳣  0������
**************************************************************************/
u8 Turn_Off(float angle, int voltage)
{
	    u8 temp;
			if(angle<-60||angle>60||1==Flag_Stop||voltage<1000)//��ص�ѹ����11.1V�رյ��
			{	                                                 //===��Ǵ���40�ȹرյ��
      temp=1;                                            //===Flag_Stop��1�رյ��
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
�������ܣ�����PWM��ֵ 
��ڲ�������
����  ֵ����
**************************************************************************/
void Xianfu_Pwm(void)
{	
	  int Amplitude=5200;    //===PWM������7200 ������6900
//		if(Flag_Qian==1)  Moto1+=DIFFERENCE;  //DIFFERENCE��һ������ƽ��С������ͻ�е��װ�����һ��������ֱ���������������С�����и��õ�һ���ԡ�
//	  if(Flag_Hou==1)   Moto2-=DIFFERENCE;
    if(Moto1<-Amplitude) Moto1=-Amplitude;	
		if(Moto1>Amplitude)  Moto1=Amplitude;	
	  if(Moto2<-Amplitude) Moto2=-Amplitude;	
		if(Moto2>Amplitude)  Moto2=Amplitude;		
}

/**************************************************************************
�������ܣ�ת�����  �޸�ת���ٶȣ����޸�Turn_Amplitude����
��ڲ��������ֱ����������ֱ�������Z��������
����  ֵ��ת�����PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int turn(int encoder_left,int encoder_right,float gyro)//ת�����
{
	int PWM_out;
	//�ⲻ��һ���ϸ��PD��������Kd��Ե���ת���Լ������Kp��Ե���ң�ص�ת��
	PWM_out= Turn_Kd*gyro + Turn_Kp*Turn_Speed;
	return PWM_out;
}
/**************************************************************************
�������ܣ�����ֵ����
��ڲ�����int
����  ֵ��unsigned int
**************************************************************************/
int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}
/**************************************************************************
�������ܣ���ֵ��PWM�Ĵ���
��ڲ���������PWM������PWM
����  ֵ����
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

