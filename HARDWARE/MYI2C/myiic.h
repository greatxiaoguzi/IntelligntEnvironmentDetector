#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"
#include "include.h" 	  

//GPIO����˵��:GPIOB->CRL�Ĵ�����������0-7��GPIOB->CRH��������8-15���Ĵ���ÿ��λ����һ�����ŵ����ã�ֻ������CNFy��MODEy����
//IO�������� ����ȫ���
#define SDA_IN()  {GPIOC->CRL&=0XFFFFFF0F;GPIOC->CRL|=8<<4;}  //8����4λ��0XFFFFFF0F | 0000 0000 1000 0000  �൱�ڰѼĴ���CRL��7λ��һ��Ϊ����
#define SDA_OUT() {GPIOC->CRL&=0XFFFFFF0F;GPIOC->CRL|=3<<4;}  //3����4λ��0XFFFFFF0F | 0000 0000 0011 0000  �൱�ڰѼĴ����ĵ�4��5λ��һ��Ϊ���

//IO��������	
#define IIC_SCL    PCout(0) //SCL
#define IIC_SDA    PCout(1) //SDA
#define READ_SDA   PCin(1)  //����SDA

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void  IIC_Start_bit(void);				        //����IIC��ʼ�ź�
void IIC_Stop(void);	  			      //����IICֹͣ�ź�
void IIC_Send_Byte(uint8_t txd);			    //IIC����һ���ֽ�
uint8_t IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź� 
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	  




#endif
















