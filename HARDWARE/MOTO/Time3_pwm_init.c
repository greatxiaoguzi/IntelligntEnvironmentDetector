#ifndef __Time3_pwm_init
#define __Time3_pwm_init
#include <stm32f10x.h>
#include "include.h"
//volatile uint16_t Loop50Hzcnt;//Loop100Hzcnt,Loop200Hzcnt;
//uint8_t Loop50HzFlag=0;//Loop100HzFlag=0,Loop200HzFlag=0;   //频率计时标志
//uint16_t Timecnt2=0;
uint8_t Beep=0;
const uint16_t AlarmFreqTab[10] = {10,40,70,100,200,300,400,1000,2000,5000};//报警频率表
void Timer2_Init(void)
{
    TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;     //定时时基
    NVIC_InitTypeDef NVIC_InitStructure;                 //中断配置
	/* Compute the prescaler value */
	/* Time base configuration */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);   //时钟使能
	TIM_TimeBaseStructure.TIM_Period = 5000;//1ms产生一次中断		       //计数上线	500,50ms产生一次中断
	TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;	           //pwm时钟分频,产生100KHz的频率
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//向上计数
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);        //初始化结构体		
	
	TIM_ARRPreloadConfig(TIM2, ENABLE);   //使能预装载寄存器
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); 						//TIM2_DIER[2]=1  允许更新中断
	TIM_Cmd(TIM2, ENABLE);                      //使能定时器3计时
	NVIC_InitStructure.NVIC_IRQChannel = 	TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器  
}
//定时器3中断函数
//1ms中断一次
//PulseCnt：脉冲计数
//定时器3任务：判断输出端口的状态以及为蜂鸣器提供定时时间
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)     //确认是定时器3产生中断
    {
        TIM2->SR = (uint16_t)~TIM_IT_Update;		       //清除中断标志位		
//        Loop50Hzcnt ++;
//        Timecnt2 ++;
//        LED0 = !LED0;
//        SendJQCmd(JQCmd);
//        if(++Loop50Hzcnt>=5000)   //空计数，备用
//        {
//            Loop50Hzcnt = 0;
//            Loop50HzFlag = 1;			
//        }
//        if(JQRecFlag)  //如果甲醛数据接收完成的话
//        {
//           JQCnt++;
//           if(JQCnt>220)
//             JQCnt = 0;
//        }
//        if(Bee.BeepEn)  //判断是否使能了让蜂鸣器的相关标识
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
                //TIM2->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));  //关闭定时器3结束计时
            
        }
//			  if(++Loop50Hzcnt>=20)        //50Hz判断
//				{
//				   Loop50Hzcnt = 0;
//					 Loop50HzFlag = 1;         //置位
//				}
//				if(++Loop100Hzcnt>=10)       //100Hz
//				{
//				   Loop100Hzcnt = 0;
//					 Loop100HzFlag = 1;        //置位
//				}
//			  if(++Loop200Hzcnt>=5)       //200Hz
//				{
//				   Loop200Hzcnt = 0;
//					 Loop200HzFlag = 1;        //置位
//				}							
		}
}
//定时器PWM初始化
void Tim3_init(uint16_t Period)
{
    TIM_OCInitTypeDef TIM_OCInitStructure;       //输出比较
    TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;     //定时时基
    GPIO_InitTypeDef GPIO_InitStructure;
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //PBO PB1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	uint16_t PrescalerValue = 0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);
	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;  //3-1=2;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = Period;		//定时器预装载值
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;	//pwm时钟分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//向上计数
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/* PWM1 Mode configuration: Channel */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = Period/2;//初始占空比为0
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
	//初始化PWM输出控制引脚
	TIM3->CCR2=Period/2;
}
//设置PWM的频率
//输入参数：要产生的频率值      范围:10 - 100
void SetBeepFreq(uint16_t Freq)
{
	uint8_t err;
	TIM3->ARR = 32000000/Freq;
	TIM3->CCR2 = (32000000/Freq)/10;
	TIM_Cmd(TIM3, ENABLE);
	Beep = 1;
}
//蜂鸣器端口初始化
void Beep_init(uint16_t Par)
{
   Tim3_init(Par);
}



#endif























