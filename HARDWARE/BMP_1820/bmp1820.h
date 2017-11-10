#ifndef BMP1820_H
#define BMP1820_H
#include "sys.h"
#define	BMP180_SlaveAddress     0xee	//��ѹ������������ַ
//�����Ĵ�����ַ
#define BMP180_REG_AC1 0XAA  //����11���Ĵ����ĵ�ַ,�Ǵ�����11���ֵĵ�ַ��
#define BMP180_REG_AC2 0XAC  //�⼸���Ĵ�����ֻ���ļĴ���,�����ݶ�ȡ֮ǰҪ�ȶ�ȡ��11���ֵ�����                      
#define BMP180_REG_AC3 0XAE
#define BMP180_REG_AC4 0XB0
#define BMP180_REG_AC5 0XB2
#define BMP180_REG_AC6 0XB4
#define BMP180_REG_B1  0XB6
#define BMP180_REG_B2  0XB8
#define BMP180_REG_MB  0XBA
#define BMP180_REG_MC  0XBC
#define BMP180_REG_MD  0XBE
#define BMP180_REG_OUT_XLSB   0XF8
#define BMP180_REG_OUT_LSB    0XF7
#define BMP180_REG_OUT_MSB    0XF6
#define BMP180_REG_CTRL_MEAS  0XF4
#define BMP180_REG_SOFT_RESET 0XE0
#define BMP180_REG_ID         0XD0

#define Standard_Pressure 101325   //��λpa
#define   OSS 0	 //	BMP085ʹ��
#define   REG_Pressure 0x34 // oss=0 4.5

uint8_t BMP1820Init(void);
uint32_t bmp180Convert(void);
































#endif



