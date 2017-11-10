#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	
//#define ENCODER_VOID     0   //�������ÿ�
//#define ENCODER_FORWORD   1  //˳ת
//#define ENCODER_BACKWORD  2  //��ת
//#define ENCODER_PRESSED   3  //��������

typedef enum
{
	ENCODER_VOID = 0,
	ENCODER_FORWORD =1,
	ENCODER_BACKWORD = 2,
	ENCODER_PRESSED = 3
}EncoderVal;
#define KEY0  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)//��ȡ����0

#define HumanReact_IO_Status PAin(13)  //΢���ӿ�

#define POWER_DETECC GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
#define POWER_CTRL    PCout(6)

extern volatile uint8_t Encoder_Type;
extern uint8_t MachineStartFlag;
void EncoderInit(void);
void Micro_Wave_ReacT_Init(void);
void StartMachineInit(void);
void Close_FDN304P(void);


#endif

















