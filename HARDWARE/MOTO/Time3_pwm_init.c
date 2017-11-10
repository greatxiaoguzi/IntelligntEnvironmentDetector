#ifndef __Time3_pwm_init
#define __Time3_pwm_init
#include <stm32f10x.h>
#include "include.h"
//volatile uint16_t Loop50Hzcnt;//Loop100Hzcnt,Loop200Hzcnt;
//uint8_t Loop50HzFlag=0;//Loop100HzFlag=0,Loop200HzFlag=0;   //Ƶ�ʼ�ʱ��־
//uint16_t Timecnt2=0;
uint8_t Beep=0;
const uint16_t AlarmFreqTab[10] = {10,40,70,100,200,300,400,1000,2000,5000};//����Ƶ�ʱ�
void Timer2_Init(void)
{
    TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;     //��ʱʱ��
    NVIC_InitTypeDef NVIC_InitStructure;                 //�ж�����
	/* Compute the prescaler value */
	/* Time base configuration */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);   //ʱ��ʹ��
	TIM_TimeBaseStructure.TIM_Period = 5000;//1ms����һ���ж�		       //��������	500,50ms����һ���ж�
	TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;	           //pwmʱ�ӷ�Ƶ,����100KHz��Ƶ��
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//���ϼ���
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);        //��ʼ���ṹ��		
	
	TIM_ARRPreloadConfig(TIM2, ENABLE);   //ʹ��Ԥװ�ؼĴ���
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); 						//TIM2_DIER[2]=1  ��������ж�
	TIM_Cmd(TIM2, ENABLE);                      //ʹ�ܶ�ʱ��3��ʱ
	NVIC_InitStructure.NVIC_IRQChannel = 	TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���  
}
//��ʱ��3�жϺ���
//1ms�ж�һ��
//PulseCnt���������
//��ʱ��3�����ж�����˿ڵ�״̬�Լ�Ϊ�������ṩ��ʱʱ��
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)     //ȷ���Ƕ�ʱ��3�����ж�
    {
        TIM2->SR = (uint16_t)~TIM_IT_Update;		       //����жϱ�־λ		
//        Loop50Hzcnt ++;
//        Timecnt2 ++;
//        LED0 = !LED0;
//        SendJQCmd(JQCmd);
//        if(++Loop50Hzcnt>=5000)   //�ռ���������
//        {
//            Loop50Hzcnt = 0;
//            Loop50HzFlag = 1;			
//        }
//        if(JQRecFlag)  //�����ȩ���ݽ�����ɵĻ�
//        {
//           JQCnt++;
//           if(JQCnt>220)
//             JQCnt = 0;
//        }
//        if(Bee.BeepEn)  //�ж��Ƿ�ʹ�����÷���������ر�ʶ
        {
//            if(Timecnt2<Bee.BeepTime)
//              Beep = 1;	
//            else					
//              Beep = 0;
        }
//        else
        {	
//              Beep = 0;
//              Bee.BeepEn = 0;						
//              Timecnt2=0;
                //TIM2->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));  //�رն�ʱ��3������ʱ
            
        }
//			  if(++Loop50Hzcnt>=20)        //50Hz�ж�
//				{
//				   Loop50Hzcnt = 0;
//					 Loop50HzFlag = 1;         //��λ
//				}
//				if(++Loop100Hzcnt>=10)       //100Hz
//				{
//				   Loop100Hzcnt = 0;
//					 Loop100HzFlag = 1;        //��λ
//				}
//			  if(++Loop200Hzcnt>=5)       //200Hz
//				{
//				   Loop200Hzcnt = 0;
//					 Loop200HzFlag = 1;        //��λ
//				}							
		}
}
//��ʱ��PWM��ʼ��
void Tim3_init(uint16_t Period)
{
    TIM_OCInitTypeDef TIM_OCInitStructure;       //����Ƚ�
    TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;     //��ʱʱ��
    GPIO_InitTypeDef GPIO_InitStructure;
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //PBO PB1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	uint16_t PrescalerValue = 0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);
	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;  //3-1=2;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = Period;		//��ʱ��Ԥװ��ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;	//pwmʱ�ӷ�Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//���ϼ���
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/* PWM1 Mode configuration: Channel */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = Period/2;//��ʼռ�ձ�Ϊ0
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
//	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
//	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
//	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
//	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
//	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
//	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3, DISABLE);
	//��ʼ��PWM�����������
	TIM3->CCR2=Period/2;
}
//����PWM��Ƶ��
//���������Ҫ������Ƶ��ֵ      ��Χ:10 - 100
void SetBeepFreq(uint16_t Freq)
{
	uint8_t err;
	TIM3->ARR = 32000000/Freq;
	TIM3->CCR2 = (32000000/Freq)/10;
	TIM_Cmd(TIM3, ENABLE);
	Beep = 1;
}
//�������˿ڳ�ʼ��
void Beep_init(uint16_t Par)
{
   Tim3_init(Par);
}



#endif























