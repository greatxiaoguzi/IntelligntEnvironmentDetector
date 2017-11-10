#ifndef __USART_H
#define __USART_H
#include "sys.h" 
#include "adc.h" 
#define UartMode  1
#define TFTMode   0
//����1��ʼ��		   
//V1.3�޸�˵��
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//////////////////////////////////////////////////////////////////////////////////
//֡����
#define Frme_Head        0              //֡ͷ��ʼ
#define Frme_CmdIndex    2              //��������
#define Frme_ID          4              //�豸ID
#define Frme_DtaType     5              //�������ʹ�7��ʼ
#define Frme_DtaLen      6              //���ݳ���
#define Frme_DtaCoten    7              //��������
#define MAX_DTA_TYPE     0X07           //7���������� 
#define MAX_PKG_LEN       53
///////////////////////////////////////////////
//PM2.5��������ָ��
typedef enum
{
	PMS_PASSIVITY = 1,
	PMS_STANDBY = 2,
	PMS_FORWARDLY = 3,
	PMS_NORMAL = 4,
	PMS_READ_DATA = 5,
	
	HCHO_FORWARDLY = 6,
	HCHO_PASSIVITY = 7,
	HCHO_READ_DATA = 8
}SensorCmd_TypeDef;

#define USART1_REC_LEN  			400  	//�����������ֽ��� 10
//#define USART_TEC_LEN       7     //����������ֽ���Ϊ7
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1���� 

extern uint16_t USART1_RX_STA;    
//extern uint8_t WIFIRecDataFlag; //���ڽ��յ�����
extern uint8_t WifiRecBuf[USART1_REC_LEN];
//////////////////////////////////////////
//extern uint8_t JQRecFlag;
extern uint8_t JQDataDealRes;
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound1,u32 bound2,u32 bound4);

void UsartSendDex(uint8_t num,uint8_t data);
void ClearBuf(void);

uint8_t Sensor_Data_With_cJson(void);
void SendJQCmd(uint8_t const *buf);         //���ͼ�ȩ��ȡ����
uint8_t JQSensorTest(void);
void UartSendText(char *text,uint8_t uartNum);
void PMS_Send_Cmd(SensorCmd_TypeDef Status);
void HCHO_Sensor_Mode(SensorCmd_TypeDef Mode);
void HCHOSetPasty(SensorCmd_TypeDef Status);
void HCHOReadData(void);
void HCHODataHandle(void);

void FreecJSONSpace(char *p);
#endif


