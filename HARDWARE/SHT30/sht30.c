#include "include.h"

//sht30дָ��
//����ֵ��1ʧ�� 0�ɹ�
uint8_t Sht30_Write_Cmd(uint16_t Cmd)
{
	IIC_Start_bit();                  	//��ʼ�ź�
	IIC_Send_Byte(I2C_ADR_W);  			 //�����豸��ַ+д�ź�
	if(IIC_Wait_Ack())						//�ӻ�Ӧ��
		return 1;
	IIC_Send_Byte(Cmd>>8);    					//��������
	if(IIC_Wait_Ack())						//�ӻ�Ӧ��
		return 1;
	IIC_Send_Byte(Cmd&0xff);    					//��������
	if(IIC_Wait_Ack())						//�ӻ�Ӧ��
		return 1;
	IIC_Stop();                   //����ֹͣ�ź�
	delay_ms(5);
	return 0;
}
//SHT30�����λ
uint8_t SHT30_Reset(void)
{
	if(Sht30_Write_Cmd(CMD_SOFT_RESET))
		return 1;
	delay_ms(5);
	return 0;
}
//SHT30����ʱ��
//����ֵ��1ʧ�� 0�ɹ�
uint8_t SHT30_SetClock(void)
{
	if(Sht30_Write_Cmd(CMD_MEAS_CLOCKSTR_H))
		return 1;
	delay_ms(5);
	return 0;
}
//SHT30�������ڲ���Ƶ��
//����ֵ��1ʧ�� 0�ɹ�
uint8_t SHT30_SetPeriodicMeasurement(void)
{
	if(Sht30_Write_Cmd(CMD_MEAS_PERI_10_H))
		return 1;
	delay_ms(5);
	return 0;
}
//��ʼ��SHT30��ʪ�ȴ�����
//����ֵ��1ʧ�� 0�ɹ�
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
//��ȡ��ʪ�ȴ�����������ֵ
//����ֵ��1��ȡʧ�� 0��ȡ�ɹ�
uint8_t Read_sht30(float *Temp,uint8_t *Humi)
{   	
	uint8_t CrcByte;
	uint8_t TempBuf[2];
	uint8_t HumiBuf[2];
	Sht30_Write_Cmd(CMD_MEAS_PERI_1_H); //���ݲɼ�Ƶ��1 m/s
	IIC_Start_bit();   	//��ʼ�ź�
	IIC_Send_Byte(I2C_ADR_R);         //�����豸��ַ+���ź�
	if(IIC_Wait_Ack())
		return 1;
	TempBuf[0]=IIC_Read_Byte(1);  //����ACK
	TempBuf[1]=IIC_Read_Byte(1);  //����ACK
	CrcByte = IIC_Read_Byte(1);   //������Ҫ��һ��CRC�ֽڲ��ܶ�ʪ��ֵ
	HumiBuf[0]=IIC_Read_Byte(1);  //����ACK
	HumiBuf[1]=IIC_Read_Byte(1);  //����ACK

	IIC_Stop();                          //����ֹͣ�ź�
	*Temp = 175.72*(TempBuf[0]*256+TempBuf[1])/65535-46.85; //���϶�
	*Humi = 100*(HumiBuf[0]*256+HumiBuf[1])/65535;
	return 0;
}


















