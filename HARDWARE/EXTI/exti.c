#ifndef __EXTI__C
#define __EXTI__C
//�����жϲ���
#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "stm32f10x_exti.h"
#include "include.h"
//�ⲿIO�ӿ�GPIOC3��GPIOC5
//�ⲿ�жϳ�ʼ������
/*void EXTIX_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//�ⲿ�жϣ���Ҫʹ��AFIOʱ��
    //��ʼ���ж�PC3��PC5IO�ڵ�ʱ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPD;		//��������
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //GPIOA.10	  �ж����Լ��жϳ�ʼ������
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource2);

    EXTI_InitStructure.EXTI_Line=EXTI_Line2;            
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;   //�½��ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	  	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�2�� 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					//�����ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure); 
    //EXTI_InitStructure.EXTI_LineCmd = DISABLE;
}*/
/*void EXTI2_IRQHandler(void)
{
    //uint8_t status;
//    uint8_t sta;
    OS_CPU_SR  cpu_sr;
    OS_ENTER_CRITICAL();     //����ȫ���жϱ�־
    OSIntNesting++;
    OS_EXIT_CRITICAL();      //�ָ�ȫ���жϱ�־		  
    //SPI1_SetSpeed(SPI_BaudRatePrescaler_8);
    //status=NRF24L01_Read_Buf(STATUS,Che_StaReg,1);
    if(EXTI_GetITStatus(EXTI_Line2) != RESET)
    {
       if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)==0)
        {

        }
        EXTI_ClearITPendingBit(EXTI_Line2);  //���EXTI0��·����λ
    }
    OSIntExit();
}
*/

#endif


