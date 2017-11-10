#ifndef __ADC_H
#define __ADC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////
#define THRESHOLD   5  //�������仯��ֵ

#define PMS_RESET       PCout(9)
#define PMS_SET         PCout(8)


#define PMS_RUNING      1  //����״̬
#define PMS_CLOSING     0  //�ر�״̬
extern uint8_t const ChalTab[2];  //Ҫת����ADC��ͨ��
//extern uint8_t const JQCmd[5];
//extern uint8_t const JQOFFSET[5];
//���������ݽṹ��
typedef struct
{		
	float Temp;				//�¶�����
	uint16_t Altitude;      //���θ߶�
	uint8_t Humi;				//ʪ�� 
	uint16_t HCHO;   	 		//��ȩ���ݣ���ʵŨ��Ϊ����ֵ/1000,��λ��mg/m3
	uint32_t AirPres;			//��ѹ
	uint32_t Carbon;      	 	//CO2����
    struct PM   				//PM2.5����������
	{
		uint16_t PM1_0_S;  		//��׼������Ũ��
		uint16_t PM2_5_S;		
		uint16_t PM10_S;
		uint16_t PM1_0_A;  		//���������¿�����Ũ��
		uint16_t PM2_5_A;		
		uint16_t PM10_A;
		uint16_t Cnt_0_3;       //0.1�����������ֱ���Ŀ���������
		uint16_t Cnt_0_5;
		uint16_t Cnt_1_0;		
		uint16_t Cnt_2_5;		
		uint16_t Cnt_5_0;
		uint16_t Cnt_10;
	}PMData;
	uint16_t BatVol;
}SensorType;
//����������״̬
//typedef union
//{
//	uint8_t All_Device;
//	struct
//	{
//		uint8_t eeprom:1;
//		uint8_t rx8025t:1;
//		uint8_t sht30:1;
//		uint8_t esp8266:1;
//		uint8_t bmp180:1;
//		uint8_t pms5001:1;
//		uint8_t senseair8:1;
//		uint8_t reserve:1;
//	}bits;
//}SensorStat_Type;
//extern SensorStat_Type SensorOnlineStat;
extern SensorType SensorData;
//extern uint8_t const ThreVal[7];
void Adc_Init(void);
void ADC_DMAInit(DMA_Channel_TypeDef* DMA_CHx,u32 PeripAdd,u32 MemAdd,uint16_t BufLen);
void Set_PMS5003S_Status(uint8_t RunStatus);

uint16_t Get_Adc(uint8_t ch);
uint16_t Get_Adc_Average(uint8_t ch,uint8_t times);

void PMS5003S_Init(void);
#endif 


