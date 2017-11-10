#include "key.h"
#include "delay.h"	 
//按键初始化函数 
//PA15和PC5 设置成输入
volatile uint8_t Encoder_Type;   //编码器键值
uint8_t MachineStartFlag = 0;    //机器启动完成标志
void EncoderInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//使能PORTA,PORTC时钟
		
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3|GPIO_Pin_14|GPIO_Pin_15;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOC, &GPIO_InitStructure);			
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource3);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource14);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource15);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;           
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;   //下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line14;           
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;   //上升沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line15;           
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;   //上升沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			//使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级11
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级11
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
}
void Micro_Wave_ReacT_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	
}
//开机键初始化
void StartMachineInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能PORTA,PORTC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 //LED0-->PD2 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	POWER_CTRL = 0;
}
//开始关闭电源
void Close_FDN304P(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 //LED0-->PD2 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
//编码器按键处理
void REncoderHandle(uint8_t Key_Type)
{
	switch(Key_Type)
	{
		case ENCODER_FORWORD:{}break;
		case ENCODER_BACKWORD:{}break;
		case ENCODER_PRESSED:{}break;
		default:break;
	}
}
//外部中断函数,编码器键值判断
void EXTI15_10_IRQHandler()
{
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntEnter();
#endif
	if(EXTI_GetITStatus(EXTI_Line14) != RESET)
    {
		delay_ms(10);
		EXTI->PR = EXTI_Line14;
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14) == 0)
		{
			if(!StandbyMode)
			{
				Encoder_Type = ENCODER_FORWORD;
				SetBeepFreq(AlarmFreqTab[9]);
			}
		}
	}
	else if(EXTI_GetITStatus(EXTI_Line15) != RESET)
	{
		delay_ms(10);
		EXTI->PR = EXTI_Line15;
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15) == 0)
		{
			if(!StandbyMode)
			{
				Encoder_Type = ENCODER_BACKWORD;
				SetBeepFreq(AlarmFreqTab[9]);
			}
		}
	}
#ifdef OS_TICKS_PER_SEC	 	
	OSIntExit();  					//退出中断保护				 		 
#endif
}
//按键中断
void EXTI3_IRQHandler(void)
{
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
OSIntEnter();    
#endif 	
	if(EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
		delay_ms(10);
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0)
		{
			if(!StandbyMode)
			{
				if(Current_Show_Interface != Curr_Set_Show)
				{
					Encoder_Type = ENCODER_PRESSED;
					if(AlarmCloseFlag != 0)
					{ 
						switch(AlarmCloseFlag)
						{
							case 1:SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus &= ~(1<<3); break;
							case 2:SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus &= ~(1<<4); break;
							case 3:SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus &= ~(1<<2); break;
							case 4:SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus &= ~(1<<1); break;
							case 5:SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus &= ~(1<<0); break;
							default:break;
						}
						AlarmCloseFlag = 0;
					}
					SetBeepFreq(AlarmFreqTab[4]);
				}
				else if(Current_Show_Interface == Curr_Set_Show)
				{
					if((SysParaSetInfo.SensorSwitch&0x04) == 0x04)
					{
						if(Esp8266InitFinishFlag == 1)
						{
							Encoder_Type = ENCODER_PRESSED;
							SetBeepFreq(AlarmFreqTab[4]);
						}
					}
					else
					{
						Encoder_Type = ENCODER_PRESSED;
						SetBeepFreq(AlarmFreqTab[4]);
					}
				}
			}
			//REncoderHandle(Encoder_Type);
		}
		EXTI_ClearITPendingBit(EXTI_Line3); 
	}
#ifdef OS_TICKS_PER_SEC	 	
OSIntExit();  					//退出中断保护				 		 
#endif
}











