#include "sys.h"

//rx8025t设备地址
//设备读写地址
#define RX8025_ADDR_READ        0x65
#define RX8025_ADDR_WRITE       0x64
 /* r8025N寄存器定义 */
#define RX8025_REG_SEC          0x00
#define RX8025_REG_MIN          0x01
#define RX8025_REG_HOUR         0x02
#define RX8025_REG_WDAY         0x03
#define RX8025_REG_MDAY         0x04
#define RX8025_REG_MONTH        0x05
#define RX8025_REG_YEAR         0x06
#define RX8025_REG_RAM          0x07
#define RX8025_REG_ALWMIN       0x08
#define RX8025_REG_ALWHOUR      0x09
#define RX8025_REG_ALWWDAY      0x0a
#define RX8025_REG_TMCNT0       0x0b
#define RX8025_REG_TMCNT1       0x0c


 /* 0x0d is reserved */
#define RX8025_REG_FLAG         0x0e
#define RX8025_REG_CTRL         0x0f

#define RX8025_BIT_FLAG_VDET     (1<<0)//用于检测温度补偿的状态
#define RX8025_BIT_FLAG_VLF      (1<<1)//用于检测电压低的状态
//时间缓冲结构体
typedef struct
{
	uint8_t Year; 
	uint8_t Month;
	uint8_t Day;
	uint8_t Week;
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;	          
}Rx8025StrucType;

extern Rx8025StrucType SysTimeData;   //外部接口调用

void RX8025t_Init(void);
uint8_t Rx8025tWriteReg(uint8_t Addr,uint8_t Len,uint8_t *pData);
uint8_t Rx8025tReadReg(uint8_t Addr,uint8_t Len,uint8_t *pData);
uint16_t BCDToBIN(uint8_t Val);
uint8_t BINToBCD(uint8_t Val);
uint8_t JudgOrLeapYear(uint16_t Year);
uint8_t Rx8025tGetTime(Rx8025StrucType *GetTimeStr);
uint8_t TimeAdjust(Rx8025StrucType *At);
uint32_t TimeToMinute(Rx8025StrucType *Time_Set);
uint32_t TimeToSecond(Rx8025StrucType *Time_Set);
void MinuteToTime(uint32_t Minute,Rx8025StrucType *T);
void SecondToTime(uint32_t SecondT,Rx8025StrucType *T);
uint8_t GetWeek(uint16_t year,uint8_t month,uint8_t day);

void Set_Rx8025t_Time(uint8_t year,uint8_t month,uint8_t day,uint8_t hour,uint8_t minute,uint8_t second);

//void TestRx8025t(void);
































