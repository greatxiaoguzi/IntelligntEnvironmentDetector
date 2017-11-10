#include "include.h"
#include "AT24cxx.h" 
#include "delay.h" 
//#include "delay.h"
//24CXX��������(�ʺ�24C01~24C16,24C32~256δ��������!�д���֤!)

//��ʼ��IIC�ӿ�
//����ֵ��0�ɹ� 1ʧ��
uint8_t AT24CXX_Init(void)
{
	if(AT24CXX_Check()==0)
		return 0;
	else
		return 1;
}
//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ
//����ֵ  :����������
uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr)
{
	uint8_t temp=0;		  	    																 
    IIC_Start_bit();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ	    
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	   
	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start_bit();  	 	   
	IIC_Send_Byte(0XA1);           //�������ģʽ			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}
//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void AT24CXX_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{				   	  	    																 
    IIC_Start_bit();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ	  
	}else IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);
}
//��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
//�ú�������д��16bit����32bit������.
//WriteAddr  :��ʼд��ĵ�ַ
//DataToWrite:���������׵�ַ
//Len        :Ҫд�����ݵĳ���2,4
void AT24CXX_WriteLenByte(uint16_t WriteAddr,uint32_t DataToWrite,uint8_t Len)
{
	uint8_t t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}
}

//��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
//�ú������ڶ���16bit����32bit������.
//ReadAddr   :��ʼ�����ĵ�ַ 
//����ֵ     :����
//Len        :Ҫ�������ݵĳ���2,4
uint32_t AT24CXX_ReadLenByte(uint16_t ReadAddr,uint8_t Len)
{  	
	uint8_t t;
	uint32_t temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 
		delay_us(500);		
	}
	return temp;
}
//���AT24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
uint8_t AT24CXX_Check(void)
{
	uint8_t temp;
	temp=AT24CXX_ReadOneByte(0X03FF);//����ÿ�ο�����дAT24CXX			   
	if(temp==0XA5)
		return 0;		   
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(0X03FF,0XA5);
	    temp=AT24CXX_ReadOneByte(0X03FF);	  
		if(temp==0XA5)
			return 0;
	}
	return 1;											  
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void AT24CXX_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{
	uint16_t i;
	for(i=0;i<NumToRead;i++)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		delay_ms(1);
	}
}  
//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
void AT24CXX_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{
	uint16_t i;
	for(i=0;i<NumToWrite;i++)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}
uint8_t WriteBuf[]= "wojiaoguchenglongsilasiladeganhuo!!!!!!";
uint8_t ReadBuf[50];
//�����Ƿ��ܹ�д��Ͷ���
void AT24CXXTest(void)
{
	 AT24CXX_Write(0,WriteBuf,strlen(WriteBuf));
	 AT24CXX_Read(0,ReadBuf,strlen(WriteBuf));
	
//	uint8_t Data[3]={0,0,0};
//	AT24CXX_WriteOneByte(0,111);
//	AT24CXX_WriteOneByte(1,222);
//	AT24CXX_WriteOneByte(2,123);
//	Data[0] = AT24CXX_ReadOneByte(0);
//	delay_us(500);
//	Data[1] = AT24CXX_ReadOneByte(1);
//	delay_us(500);
//	Data[2] = AT24CXX_ReadOneByte(2);
//	WriteBuffer(0,WriteBuf);
//	ReadBuffer(0,ReadBuf,sizeof(WriteBuf));
//	LCD_ShowNum(20,150,Data[0],5,16);
//	LCD_ShowNum(20,170,Data[1],5,16);
//	LCD_ShowNum(20,190,Data[2],5,16);
	LCD_ShowString(0,150,320,16,16,ReadBuf,GREEN,BLACK);
//	LCD_ShowString(20,200,200,16,16,"Finish",GREEN,BLACK);
}











