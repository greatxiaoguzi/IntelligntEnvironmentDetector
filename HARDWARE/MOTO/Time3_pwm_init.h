#ifndef _Time3_pwm_init
#define _Time3_pwm_init
#include "sys.h"
extern uint8_t Beep;
void Timer2_Init(void);
void Tim3_init(uint16_t Period);
void SetPWMVal(uint16_t pwm1,uint16_t pwm2);
void SetBeepFreq(uint16_t Freq);
//·äÃùÆ÷¶Ë¿Ú³õÊ¼»¯
void Beep_init(uint16_t Par);

extern const uint16_t AlarmFreqTab[10];
#endif



