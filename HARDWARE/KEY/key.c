#include "key.h"
#include "delay.h"	 
//������ʼ������ 
//PA15��PC5 ���ó�����
volatile uint8_t Encoder_Type;   //��������ֵ
uint8_t MachineStartFlag = 0;    //����������ɱ�־
void EncoderInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//ʹ��PORTA,PORTCʱ��
		
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3|GPIO_Pin_14|GPIO_Pin_15;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOC, &GPIO_InitStructure);			
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource3);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource14);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource15);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;           
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;   //�½��ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line14;           
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;   //�����ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line15;           
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;   //�����ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�11
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�11
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 
}
void Micro_Wave_ReacT_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	
}
//��������ʼ��
void StartMachineInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PORTA,PORTCʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 //LED0-->PD2 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	POWER_CTRL = 0;
}
//��ʼ�رյ�Դ
void Close_FDN304P(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 //LED0-->PD2 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //�������
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
//��������������
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
//�ⲿ�жϺ���,��������ֵ�ж�
void EXTI15_10_IRQHandler()
{
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
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
	OSIntExit();  					//�˳��жϱ���				 		 
#endif
}
//�����ж�
void EXTI3_IRQHandler(void)
{
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
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
OSIntExit();  					//�˳��жϱ���				 		 
#endif
}











