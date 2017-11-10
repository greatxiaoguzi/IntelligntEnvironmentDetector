#include "include.h"

//sht30写指令
//返回值：1失败 0成功
uint8_t Sht30_Write_Cmd(uint16_t Cmd)
{
	IIC_Start_bit();                  	//起始信号
	IIC_Send_Byte(I2C_ADR_W);  			 //发送设备地址+写信号
	if(IIC_Wait_Ack())						//从机应答
		return 1;
	IIC_Send_Byte(Cmd>>8);    					//发送命令
	if(IIC_Wait_Ack())						//从机应答
		return 1;
	IIC_Send_Byte(Cmd&0xff);    					//发送命令
	if(IIC_Wait_Ack())						//从机应答
		return 1;
	IIC_Stop();                   //发送停止信号
	delay_ms(5);
	return 0;
}
//SHT30软件复位
uint8_t SHT30_Reset(void)
{
	if(Sht30_Write_Cmd(CMD_SOFT_RESET))
		return 1;
	delay_ms(5);
	return 0;
}
//SHT30设置时钟
//返回值：1失败 0成功
uint8_t SHT30_SetClock(void)
{
	if(Sht30_Write_Cmd(CMD_MEAS_CLOCKSTR_H))
		return 1;
	delay_ms(5);
	return 0;
}
//SHT30设置周期采样频率
//返回值：1失败 0成功
uint8_t SHT30_SetPeriodicMeasurement(void)
{
	if(Sht30_Write_Cmd(CMD_MEAS_PERI_10_H))
		return 1;
	delay_ms(5);
	return 0;
}
//初始化SHT30温湿度传感器
//返回值：1失败 0成功
uint8_t SHT30_Init(void)
{
	if(SHT30_Reset())
		return 1;
	if(SHT30_SetClock())
		return 1;
	if(SHT30_SetPeriodicMeasurement())
		return 1;
	return 0;
}
//读取温湿度传感器的数据值
//返回值：1读取失败 0读取成功
uint8_t Read_sht30(float *Temp,uint8_t *Humi)
{   	
	uint8_t CrcByte;
	uint8_t TempBuf[2];
	uint8_t HumiBuf[2];
	Sht30_Write_Cmd(CMD_MEAS_PERI_1_H); //数据采集频率1 m/s
	IIC_Start_bit();   	//起始信号
	IIC_Send_Byte(I2C_ADR_R);         //发送设备地址+读信号
	if(IIC_Wait_Ack())
		return 1;
	TempBuf[0]=IIC_Read_Byte(1);  //发送ACK
	TempBuf[1]=IIC_Read_Byte(1);  //发送ACK
	CrcByte = IIC_Read_Byte(1);   //这里先要读一个CRC字节才能度湿度值
	HumiBuf[0]=IIC_Read_Byte(1);  //发送ACK
	HumiBuf[1]=IIC_Read_Byte(1);  //发送ACK

	IIC_Stop();                          //发送停止信号
	*Temp = 175.72*(TempBuf[0]*256+TempBuf[1])/65535-46.85; //摄氏度
	*Humi = 100*(HumiBuf[0]*256+HumiBuf[1])/65535;
	return 0;
}


















