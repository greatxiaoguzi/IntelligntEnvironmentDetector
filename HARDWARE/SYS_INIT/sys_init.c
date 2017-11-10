#ifndef __SYS_INIT_C
#define __SYS_INIT_C
#include "include.h"
//ϵͳʱ�����ã�����ʱ������
void sys_clock_init(uint8_t PLL)
{
	unsigned char temp=0;
	//MYRCC_DeInit();		  //��λ������������
 	RCC->CR|=0x00010000;  //�ⲿ����ʱ��ʹ��HSEON
	while(!(RCC->CR>>17));//�ȴ��ⲿʱ�Ӿ���
	RCC->CFGR=0X00000400; //APB1=DIV2;APB2=DIV1;AHB=DIV1;
	PLL-=2;//����2����λ
	RCC->CFGR|=PLL<<18;   //����PLLֵ 2~16
	RCC->CFGR|=1<<16;	  //PLLSRC ON
	FLASH->ACR|=0x32;	  //FLASH 2����ʱ����

	RCC->CR|=0x01000000;  //PLLON
	while(!(RCC->CR>>25));//�ȴ�PLL����
	RCC->CFGR|=0x00000002;//PLL��Ϊϵͳʱ��
	while(temp!=0x02)     //�ȴ�PLL��Ϊϵͳʱ�����óɹ�
	{
		temp=RCC->CFGR>>2;
		temp&=0x03;
	}
	//������ʱ�ӣ�APB2������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //��Ҫʹ�õ�����ʱ��,GPIOA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //��Ҫʹ�õ�����ʱ��,GPIOB
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  //��Ҫʹ�õ�����ʱ��,GPIOC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);   //��Ҫʹ�õ�����ʱ��,ADC1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	 //ʹ��PA,PD�˿�ʱ��
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);	 //ʹ��PB,PF�˿�ʱ��

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	RCC_ClockSecuritySystemCmd(ENABLE);                    //ϵͳʱ�Ӱ�ȫ���ã�
}
#endif


