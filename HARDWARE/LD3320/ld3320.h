#ifndef __LD3320_H
#define __LD3320_H

#include "stm32f10x.h"


#define LD_CS  	PBout(6)
#define LD_WR 	PAout(12)
#define LD_RST 	PCout(5)

//	��������״̬����������¼������������ASRʶ����������MP3����
#define LD_MODE_IDLE		0x00
#define LD_MODE_ASR_RUN		0x08
#define LD_MODE_MP3		 	0x40


//	�������״̬����������¼������������ASRʶ������е��ĸ�״̬
#define LD_ASR_NONE				0x00	//	��ʾû������ASRʶ��
#define LD_ASR_RUNING			0x01	//	��ʾLD3320������ASRʶ����
#define LD_ASR_FOUNDOK			0x10	//	��ʾһ��ʶ�����̽�������һ��ʶ����
#define LD_ASR_FOUNDZERO 		0x11	//	��ʾһ��ʶ�����̽�����û��ʶ����
#define LD_ASR_ERROR	 		0x31	//	��ʾһ��ʶ��������LD3320оƬ�ڲ����ֲ���ȷ��״̬

#define CLK_IN   		 22.1184///* user need modify this value according to clock in */
#define LD_PLL_11			(uint8_t)((CLK_IN/2.0)-1)
#define LD_PLL_MP3_19		0x0f
#define LD_PLL_MP3_1B		0x18
#define LD_PLL_MP3_1D   	(uint8_t)(((90.0*((LD_PLL_11)+1))/(CLK_IN))-1)

#define LD_PLL_ASR_19 		(uint8_t)(CLK_IN*32.0/(LD_PLL_11+1) - 0.51)
#define LD_PLL_ASR_1B 		0x48
#define LD_PLL_ASR_1D 		0x1f

// LD chip fixed values.
#define        RESUM_OF_MUSIC               0x01
#define        CAUSE_MP3_SONG_END           0x20

#define        MASK_INT_SYNC				0x10
#define        MASK_INT_FIFO				0x04
#define    	   MASK_AFIFO_INT				0x01
#define        MASK_FIFO_STATUS_AFULL		0x08


extern uint8_t SpeechReactionFlag;
extern uint8_t bMp3Play;
extern uint8_t MP3FilePath[10][50];

extern uint8_t LD3320InitFlag;
extern uint8_t nAsrStatus;

void LD3320_Init(void);
void ProcessInt0(void);	  //���� ����ʶ���ж�
void LD_Reset(void);	  //оƬ��λ
void LD_TEST(void);
void SpeechRecgnizeTask(void);

unsigned char SPI2_NRF_SendByte(unsigned char byte);

unsigned char LD_Read(unsigned char a1);
void LD_Write(unsigned char data1,unsigned char data2);

void LD_ReloadMp3Data_Again(void);

void LD_TEST(void);

//================
uint8_t LD_GetResult(void);//ȡASRʶ����
void LD_AsrStart(void);//����ʶ���ʼ��
uint8_t LD_AsrAddFixed(void);//��ӹؼ����ﵽLD3320оƬ��
uint8_t LD_AsrRun(void);//��ʼʶ��
uint8_t LD_Check_ASRBusyFlag_b2(void);//оƬ���޳���
void LD_AsrAddFixed_ByIndex(uint8_t nIndex);//����
void LD_AsrAddFixed_ByString(char * pRecogString, uint8_t k);//��
uint8_t RunASR(void);//����һ��ʶ������
uint8_t PlayDemoSound_mp3(uint8_t *path,uint8_t Volume);
void DisableLD3320NVIC(void);



#endif
