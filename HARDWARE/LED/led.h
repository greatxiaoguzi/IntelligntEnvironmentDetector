#ifndef __LED_H
#define __LED_H	 
#include "include.h"								  
////////////////////////////////////////////////////////////////////////////////// 


#define LED0    PCout(4)	//     指示灯
void LED_Init(void);//初始化
void LEDAlarm(uint8_t Times);
#endif
