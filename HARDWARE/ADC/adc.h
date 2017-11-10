#ifndef __ADC_H
#define __ADC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////
#define THRESHOLD   5  //传感器变化阈值

#define PMS_RESET       PCout(9)
#define PMS_SET         PCout(8)


#define PMS_RUNING      1  //开启状态
#define PMS_CLOSING     0  //关闭状态
extern uint8_t const ChalTab[2];  //要转换的ADC的通道
//extern uint8_t const JQCmd[5];
//extern uint8_t const JQOFFSET[5];
//传感器数据结构体
typedef struct
{		
	float Temp;				//温度数据
	uint16_t Altitude;      //海拔高度
	uint8_t Humi;				//湿度 
	uint16_t HCHO;   	 		//甲醛数据，真实浓度为本数值/1000,单位：mg/m3
	uint32_t AirPres;			//气压
	uint32_t Carbon;      	 	//CO2数据
    struct PM   				//PM2.5的所有数据
	{
		uint16_t PM1_0_S;  		//标准颗粒物浓度
		uint16_t PM2_5_S;		
		uint16_t PM10_S;
		uint16_t PM1_0_A;  		//大气环境下颗粒物浓度
		uint16_t PM2_5_A;		
		uint16_t PM10_A;
		uint16_t Cnt_0_3;       //0.1升空气中相对直径的颗粒物总数
		uint16_t Cnt_0_5;
		uint16_t Cnt_1_0;		
		uint16_t Cnt_2_5;		
		uint16_t Cnt_5_0;
		uint16_t Cnt_10;
	}PMData;
	uint16_t BatVol;
}SensorType;
//传感器在线状态
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


