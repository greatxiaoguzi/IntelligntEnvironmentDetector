#ifndef BMP1820_C   
#define BMP1820_C 
//��ѹ�������������򲿷�
#include "include.h"
//���������ʼ��
struct
{
	short ac1;
	short ac2; 
	short ac3; 
	unsigned short ac4;
	unsigned short ac5;
	unsigned short ac6;
	short b1; 
	short b2;
	short mb;
	short mc;
	short md;
}Bmp180CalValue;
//��ȡBMP820����
uint16_t ReadBmp1820Data(uint8_t ST_Add)
{
    uint8_t msb, lsb;
    uint16_t data;
    IIC_Start_bit();                      //������ʼ�ź�
    IIC_Send_Byte(BMP180_SlaveAddress);    //�����豸��ַ+д�ź�
    if(IIC_Wait_Ack())
		return 1;
    IIC_Send_Byte(ST_Add);             //���ʹ洢��Ԫ��ַ
    if(IIC_Wait_Ack())
		return 1;                                 
    IIC_Start_bit();                          //��ʼ�ź�
    IIC_Send_Byte(BMP180_SlaveAddress+1);         //�����豸��ַ+���ź�
    if(IIC_Wait_Ack())
		return 1;                                        

    msb = IIC_Read_Byte(1);                 //BUF[0]�洢                
    lsb = IIC_Read_Byte(0);                //���һ��������Ҫ��NOACK

    IIC_Stop();                           //ֹͣ�ź�
    data = msb << 8;
    data |= lsb;	
    return data;
}
//��ȡ��ѹ��������ѹ��
uint16_t ReadBmp1820Pressure(void)
{
    IIC_Start_bit();                 //��ʼ�ź�
    IIC_Send_Byte(BMP180_SlaveAddress);   //�����豸��ַ+д�ź�
    if(IIC_Wait_Ack())
		return 1;
    IIC_Send_Byte(0xF4);	          // write register address
    if(IIC_Wait_Ack())
		return 1;
    IIC_Send_Byte(0x34);       	  // write register data for pressure
    if(IIC_Wait_Ack())
		return 1;
    IIC_Stop();                    //����ֹͣ�ź�
    delay_ms(5);    	                  // max time is 4.5ms
    //pressure = Multiple_read(0xF6);
    //pressure &= 0x0FFFF;

    return ReadBmp1820Data(BMP180_REG_OUT_MSB);    //pressure;	
}
//��ȡ�¶�����
uint16_t bmp180ReadTemp(void)
{
    IIC_Start_bit();                  //��ʼ�ź�
    IIC_Send_Byte(BMP180_SlaveAddress);   //�����豸��ַ+д�ź�   BMP180_SlaveAddress  0xee	��ѹ������������ַ
    if(IIC_Wait_Ack())
		return 1;
    IIC_Send_Byte(BMP180_REG_CTRL_MEAS);	          // write register address
    if(IIC_Wait_Ack())
		return 1;
    IIC_Send_Byte(0x2E);       	// write register data for temp
    if(IIC_Wait_Ack())
		return 1;
    IIC_Stop();                   //����ֹͣ�ź�
    delay_ms(5);	// max time is 4.5ms
    return ReadBmp1820Data(BMP180_REG_OUT_MSB);
}
//bmp1820��ʼ��
uint8_t BMP1820Init(void)
{
    //�⼸���Ĵ�����ֻ���ļĴ���,�����ݶ�ȡ֮ǰҪ�ȶ�ȡ��11���ֵ�����
    Bmp180CalValue.ac1 = ReadBmp1820Data(BMP180_REG_AC1);
    Bmp180CalValue.ac2 = ReadBmp1820Data(BMP180_REG_AC2);
    Bmp180CalValue.ac3 = ReadBmp1820Data(BMP180_REG_AC3);
    Bmp180CalValue.ac4 = ReadBmp1820Data(BMP180_REG_AC4);
    Bmp180CalValue.ac5 = ReadBmp1820Data(BMP180_REG_AC5);
    Bmp180CalValue.ac6 = ReadBmp1820Data(BMP180_REG_AC6);
    Bmp180CalValue.b1 =  ReadBmp1820Data(BMP180_REG_B1);
    Bmp180CalValue.b2 =  ReadBmp1820Data(BMP180_REG_B2);
    Bmp180CalValue.mb =  ReadBmp1820Data(BMP180_REG_MB);
    Bmp180CalValue.mc =  ReadBmp1820Data(BMP180_REG_MC);
    Bmp180CalValue.md =  ReadBmp1820Data(BMP180_REG_MD);
	delay_ms(100);
	if(bmp180Convert() == 0)
		return 1;
	else
		return 0;
}
//��ѹ����ת��
//���յ���ѹֵ
uint32_t bmp180Convert(void)
{
    //u32 *BMP_UT;
    uint32_t ut;
    uint16_t up;
    uint32_t x1, x2, b5, b6, x3, b3, p;
    uint32_t b4, b7;

    ut = bmp180ReadTemp();	   // ��ȡ�¶�
	if(ut==1)
		return 1;
    up = ReadBmp1820Pressure();  // ��ȡѹǿ    return pressure;	
	if(up==1)
		return 1;
    x1 = (((uint32_t)ut - (uint32_t)Bmp180CalValue.ac6)*(uint32_t)Bmp180CalValue.ac5) >> 15;
    x2 = ((uint32_t)Bmp180CalValue.mc << 11) / (x1 + Bmp180CalValue.md);
    //b5 = x1 + x2;
    //*BMP_UT = ((b5 + 8) >> 4);
    //*************
    b6 = b5 - 4000;
                             // Calculate B3
    x1 = (Bmp180CalValue.b2 * (b6 * b6)>>12)>>11;
    x2 = (Bmp180CalValue.ac2 * b6)>>11;
    x3 = x1 + x2;
    b3 = (((((uint32_t)Bmp180CalValue.ac1)*4 + x3)<<OSS) + 2)>>2;	
                             // Calculate B4
    x1 = (Bmp180CalValue.ac3 * b6)>>13;
    x2 = (Bmp180CalValue.b1 * ((b6 * b6)>>12))>>16;
    x3 = ((x1 + x2) + 2)>>2;
    b4 = (Bmp180CalValue.ac4 * (uint32_t)(x3 + 32768))>>15;

    b7 = ((uint32_t)(up - b3) * (50000>>OSS));
    if (b7 < 0x80000000)
		p = (b7<<1)/b4;
    else
		p = (b7/b4)<<1;

    x1 = (p>>8) * (p>>8);
    x1 = (x1 * 3038)>>16;
    x2 = (-7357 * p)>>16;
	if((uint32_t)(p+((x1 + x2 + 3791)>>4)) == 0)
		SensorOnline.Device.BMP180 = 0;
	else 
		SensorOnline.Device.BMP180 = 1;
    return (uint32_t)(p+((x1 + x2 + 3791)>>4));
}




































#endif



