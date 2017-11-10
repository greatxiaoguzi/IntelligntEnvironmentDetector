#include "include.h"
//G8S������̼��������ʼ��
uint8_t TIM4CH2_CAPTURE_STA = 0;	//���벶��״̬		    				
volatile uint16_t TIM4CH2_CAPTURE_VAL = 0;	//���벶��ֵ
void Senseair8_Init(uint16_t arr,uint16_t psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM4_ICInitStructure;
   	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//ʹ��TIM4ʱ��
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7;  //PB7 ���֮ǰ����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PB7 ����  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_7);
	
	//��ʼ����ʱ��4 TIM4	 
	TIM_TimeBaseStructure.TIM_Period = arr; //�趨�������Զ���װֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 	//Ԥ��Ƶ��
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
  
	//��ʼ��TIM4���벶�����
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
  	TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
  	TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  	TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
  	TIM4_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
  	TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	
	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 
	
	TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC2,ENABLE);//��������ж� ,����CC1IE�����ж�	
	
	TIM_Cmd(TIM4,ENABLE); 	//ʹ�ܶ�ʱ��5
}
//5V��Դ�����ʼ��
void Handle_5V_POWER_Status(uint8_t Mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	if(Mode)  //��
	{		
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14;  
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_ResetBits(GPIOA,GPIO_Pin_14);
	}
	else   //�ر�
	{
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14;  
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
}
//��ʱ��5�жϷ������	 
void TIM4_IRQHandler(void)
{
	if((TIM4CH2_CAPTURE_STA&0X80)==0)	//��δ�ɹ�����	
	{
//		if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)		 
//		{
//			if(TIM4CH2_CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
//			{
//				if((TIM4CH2_CAPTURE_STA&0X3F)==0X3F)//�ߵ�ƽ̫����
//				{
//					TIM4CH2_CAPTURE_STA|=0X80;//��ǳɹ�������һ��
//					TIM4CH2_CAPTURE_VAL=0XFFFF;
//				}
//				else
//					TIM4CH2_CAPTURE_STA++;
//			}	 
//		}
		if(TIM_GetITStatus(TIM4, TIM_IT_CC2) != RESET)//����1���������¼�
		{
			if(TIM4CH2_CAPTURE_STA&0X40)		//����һ���½��� 		
			{
				TIM4CH2_CAPTURE_STA |= 0X80;		//��ǳɹ�����һ��������
				TIM4CH2_CAPTURE_VAL=TIM_GetCapture2(TIM4);
				SensorData.Carbon = 2*TIM4CH2_CAPTURE_VAL;
				if(SensorData.Carbon > 2000)
					SensorData.Carbon = 2000;
				SensorOnline.Device.SENSAIR = 1;
		   		TIM_OC2PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
			}
			else  								//��δ��ʼ,��һ�β���������
			{
				TIM4CH2_CAPTURE_STA=0;			//���
				TIM4CH2_CAPTURE_VAL=0;
	 			TIM_SetCounter(TIM4,0);
				TIM4CH2_CAPTURE_STA |= 0X40;		//��ǲ�����������
		   		TIM_OC2PolarityConfig(TIM4,TIM_ICPolarity_Falling);	 	//CC1P=1 ����Ϊ�½��ز���
			}
		}
 	}
    TIM_ClearITPendingBit(TIM4, TIM_IT_CC2|TIM_IT_Update); //����жϱ�־λ
}







































































