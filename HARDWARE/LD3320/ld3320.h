#ifndef __LD3320_H
#define __LD3320_H

#include "stm32f10x.h"


#define LD_CS  	PBout(6)
#define LD_WR 	PAout(12)
#define LD_RST 	PCout(5)

//	以下三个状态定义用来记录程序是在运行ASR识别还是在运行MP3播放
#define LD_MODE_IDLE		0x00
#define LD_MODE_ASR_RUN		0x08
#define LD_MODE_MP3		 	0x40


//	以下五个状态定义用来记录程序是在运行ASR识别过程中的哪个状态
#define LD_ASR_NONE				0x00	//	表示没有在作ASR识别
#define LD_ASR_RUNING			0x01	//	表示LD3320正在作ASR识别中
#define LD_ASR_FOUNDOK			0x10	//	表示一次识别流程结束后，有一个识别结果
#define LD_ASR_FOUNDZERO 		0x11	//	表示一次识别流程结束后，没有识别结果
#define LD_ASR_ERROR	 		0x31	//	表示一次识别流程中LD3320芯片内部出现不正确的状态

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
void ProcessInt0(void);	  //播放 语音识别中断
void LD_Reset(void);	  //芯片复位
void LD_TEST(void);
void SpeechRecgnizeTask(void);

unsigned char SPI2_NRF_SendByte(unsigned char byte);

unsigned char LD_Read(unsigned char a1);
void LD_Write(unsigned char data1,unsigned char data2);

void LD_ReloadMp3Data_Again(void);

void LD_TEST(void);

//================
uint8_t LD_GetResult(void);//取ASR识别结果
void LD_AsrStart(void);//语音识别初始化
uint8_t LD_AsrAddFixed(void);//添加关键词语到LD3320芯片中
uint8_t LD_AsrRun(void);//开始识别
uint8_t LD_Check_ASRBusyFlag_b2(void);//芯片有无出错
void LD_AsrAddFixed_ByIndex(uint8_t nIndex);//索引
void LD_AsrAddFixed_ByString(char * pRecogString, uint8_t k);//↑
uint8_t RunASR(void);//启动一次识别流程
uint8_t PlayDemoSound_mp3(uint8_t *path,uint8_t Volume);
void DisableLD3320NVIC(void);



#endif
