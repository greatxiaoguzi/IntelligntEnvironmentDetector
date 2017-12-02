#include "include.h"
void IWDG_Init(uint8_t prer,uint16_t rlr) 
{	
 	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //使能对寄存器IWDG_PR和IWDG_RLR的写操作
	
	IWDG_SetPrescaler(prer);  //设置IWDG预分频值:设置IWDG预分频值为64
	
	IWDG_SetReload(rlr);  //设置IWDG重装载值
	
	IWDG_ReloadCounter();  //按照IWDG重装载寄存器的值重装载IWDG计数器
	
	IWDG_Enable();  //使能IWDG
}




































