#include "include.h"
//实时时钟驱动部分
#define SCLDELAY  4
uint8_t const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; 	//月修正数据表
uint8_t MonthDayNum[2][12]={{31,29,31,30,31,30,31,31,30,31,30,31},{31,28,31,30,31,30,31,31,30,31,30,31}};
Rx8025StrucType SysTimeData;	//系统时间数据结构	
//RX8025T初始化
void RX8025t_Init(void)  //bug(这个在初始化的时候是必须的不然小时乱飞)
{
	IIC_Init();
	IIC_Start_bit();
	IIC_Send_Byte(RX8025_ADDR_WRITE);
	IIC_Stop();
	IIC_Send_Byte(0xe0);
	IIC_Stop();
	delay_ms(100);
	IIC_Send_Byte(0x20);
	IIC_Stop();
}
//rx80325t写入数据
uint8_t Rx8025tWriteReg(uint8_t Addr,uint8_t Len,uint8_t *pData)
{
	uint8_t i;
	uint8_t ErrorCode = 0;
	IIC_Start_bit();
	IIC_Send_Byte(RX8025_ADDR_WRITE);
	ErrorCode = IIC_Wait_Ack();
	IIC_Send_Byte(Addr);
	ErrorCode = IIC_Wait_Ack();
	for(i=0;i<Len;i++)
	{
		IIC_Send_Byte(*pData++);
		ErrorCode = IIC_Wait_Ack();
	}
	IIC_Stop();
	return  ErrorCode;
}
//rx80325t读出数据
uint8_t Rx8025tReadReg(uint8_t Addr,uint8_t Len,uint8_t *pData)
{
	uint8_t i;
	uint8_t ErrorCode;
	IIC_Start_bit();
	IIC_Send_Byte(RX8025_ADDR_WRITE);

	IIC_Send_Byte(Addr);

	IIC_Start_bit();
	IIC_Send_Byte(RX8025_ADDR_READ);

	ErrorCode =  IIC_Wait_Ack();
	for(i=0;i<Len-1;i++)
	{
		*pData++ = IIC_Read_Byte(1);
		//Rx8025tIIC_ACK();
	}
	*pData =  IIC_Read_Byte(0);
	//IIC_NAck();
	IIC_Stop();
	return ErrorCode;
}
//BCD码转BIN
uint16_t BCDToBIN(uint8_t Val)
{
	return ((Val&0x0f)+(Val >> 4)*10);
}
//BIN码转BCD码
uint8_t BINToBCD(uint8_t Val)
{
	return ((Val / 10) << 4) + Val % 10;
}
//判断是否为闰年
uint8_t JudgOrLeapYear(uint16_t Year)
{
	return (!(Year % 4) && (Year % 100)) || !(Year % 400);
}
//获得当前的时间赋值给时间结构体
uint8_t Rx8025tGetTime(Rx8025StrucType *GetTimeStr)
{
	uint8_t Date[7];
	uint8_t Err;
	Err = Rx8025tReadReg(RX8025_REG_SEC,7,Date);
	if(Err)
		return Err;
	GetTimeStr->Second = (uint8_t)BCDToBIN(Date[RX8025_REG_SEC]&0x7f);
	GetTimeStr->Minute = (uint8_t)BCDToBIN(Date[RX8025_REG_MIN]&0x7f);
	GetTimeStr->Hour = (uint8_t)BCDToBIN(Date[RX8025_REG_HOUR] &0x3f);
	GetTimeStr->Week = Date[RX8025_REG_WDAY]&0XFF;  //BCDToBIN(Date[RX8025_REG_WDAY] & 0x7f);
	GetTimeStr->Day = BCDToBIN(Date[RX8025_REG_MDAY] & 0x3f);	
	GetTimeStr->Month = BCDToBIN(Date[RX8025_REG_MONTH] & 0x1f);
	GetTimeStr->Year = BCDToBIN(Date[RX8025_REG_YEAR]);	
	return 0;
}
//设置时间
void Set_Rx8025t_Time(uint8_t year,uint8_t month,uint8_t day,uint8_t hour,uint8_t minute,uint8_t second)
{
	Rx8025StrucType TempData;
	//uint8_t week = 0;
	//week = GetWeek(2000+year,month,day);
	TempData.Year = year;
	TempData.Month = month;
	TempData.Day = day;
	//TempData.Week |= (1<<week);
	TempData.Hour = hour;
	TempData.Minute = minute;
	TempData.Second = second;
	TimeAdjust(&TempData);
}
//校正时间rx8035t
uint8_t TimeAdjust(Rx8025StrucType *At)
{
	uint8_t Date[7];
	Date[RX8025_REG_SEC] = BINToBCD(At->Second);
	Date[RX8025_REG_MIN] = BINToBCD(At->Minute);
	Date[RX8025_REG_HOUR] = BINToBCD(At->Hour);
	Date[RX8025_REG_WDAY] = BINToBCD(At->Week);
	Date[RX8025_REG_MDAY] = BINToBCD(At->Day);
	Date[RX8025_REG_MONTH] = BINToBCD(At->Month);
	Date[RX8025_REG_YEAR] = BINToBCD(At->Year% 100);
	
	return Rx8025tWriteReg(RX8025_REG_SEC,7,Date);		
}
//把时间转化为分钟
uint32_t TimeToMinute(Rx8025StrucType *Time_Set)
{
	uint8_t c;
	uint32_t	TimeMinute,i;

	Time_Set->Year &= 0xFF;
	
	TimeMinute=(uint32_t)525600*(uint32_t)Time_Set->Year;			//年//525600=(60*24*365)
	c=((Time_Set->Year-1)>>2)+1;
	TimeMinute+=(uint32_t)1440*(uint32_t)c;//1440=60*24
	if(Time_Set->Year&0x03) 
		c=1;					//闰年判断
	else
		c=0;

	if (Time_Set->Month==0) return 0;
	for (i=0;i<(Time_Set->Month-1);i++) 
	{			//月
		TimeMinute+=(uint32_t)1440*(uint32_t)MonthDayNum[c][i];////1440=60*24	
	}

	if (Time_Set->Day==0) 
		return 0;
	TimeMinute+=((uint32_t)1440*(uint32_t)(Time_Set->Day-1) );		//日
	TimeMinute+=((uint32_t)60*(uint32_t)Time_Set->Hour);					//小时
	TimeMinute+=(uint32_t)(Time_Set->Minute);					//分
	return TimeMinute;
}
//时间到秒转化
uint32_t TimeToSecond(Rx8025StrucType *Time_Set)
{	
	uint32_t TimeSecond; 
	TimeSecond = TimeToMinute(Time_Set)*60;
	TimeSecond += Time_Set->Second;						//秒
	return TimeSecond;
}
//分钟转化年月日时分秒
void MinuteToTime(uint32_t Minute,Rx8025StrucType *T)
{
	int iTemp1,iTemp2,i,HourT;
	T->Second=0;
	HourT=Minute/60;
	T->Minute=Minute%60;
	iTemp1=HourT/(365*24);
	iTemp2=HourT%(365*24);
	T->Year=iTemp1;
	iTemp2-=((iTemp1>>2)+1)*24;

	if(iTemp1&0x03) 
		iTemp1=1;							//闰年判断
	else		
		iTemp1=0;
	for ( i=0;i<12;i++ ) 
	{
		if (iTemp2<MonthDayNum[iTemp1][i]*24)
			break;
		iTemp2-=MonthDayNum[iTemp1][i]*24;
	}
	T->Month=i+1;
	T->Day=iTemp2/24;
	T->Hour=iTemp2%24;
	T->Day++;
}
//秒转化为时间
void SecondToTime(uint32_t SecondT,Rx8025StrucType *T)
{
	uint32_t iTemp1,iTemp2,i,sTemp,HourT;

	HourT=SecondT/3600;
	sTemp=SecondT%3600;
	T->Minute=sTemp/60;
	T->Second=sTemp%60;
	
	for ( iTemp1=0;iTemp1<100;iTemp1++ ) 
	{
		if(iTemp1&0x03) 
			iTemp2 = 365*24;
		else 
			iTemp2 = 366*24;
		if(HourT>=iTemp2) 
		{
			HourT -= iTemp2;
		}
		else 
		{
			break;
		}
	}
	T->Year=iTemp1;

	if(iTemp1&0x03)
		iTemp1=1;							//闰年判断
	else			
		iTemp1=0;
	for ( i=0;i<12;i++ )
	{
		if (HourT < MonthDayNum[iTemp1][i]*24)
			break;
		HourT -= MonthDayNum[iTemp1][i]*24;
	}
	T->Month = i+1;
	T->Day = HourT/24;
	T->Hour = HourT%24;
	T->Day++;

	iTemp2=SecondT%(7*24*3600);
	if (iTemp2>=1) 
		T->Week=iTemp2-1;
	else		  
		T->Week=6;
}
//根据年月日换算得到星期几
uint8_t GetWeek(uint16_t year,uint8_t month,uint8_t day)
{
	uint16_t temp2;
	uint8_t yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 
	// 如果为21世纪,年份数加100  
	if (yearH>19)yearL+=100;
	// 所过闰年数只算1900年之后的  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}
/*
//测试RX8025T是否正常
void TestRx8025t(void)
{
	uint8_t Tempbuf[20];
	if(TimeAdjust(&SysTimeSet))
		LED_RED = 0;
	else
		LED_RED = 1;
	OLED_Clear();
	while(1)
	{
		Rx8025tGetTime(&SysTimeSet);
		sprintf(Tempbuf,"20%2d/%d/%d" ,SysTimeSet.Year,SysTimeSet.Month,SysTimeSet.Day);
		OLED_ShowMixStr(0,35,128,16,Tempbuf,16,1);  //到时候这里用参数代替
		memset(Tempbuf,'\0',20);
		sprintf(Tempbuf,"%d:%d:%d",SysTimeSet.Hour,SysTimeSet.Minute,SysTimeSet.Second);
		OLED_ShowMixStr(0,50,128,16,Tempbuf,16,1);
		memset(Tempbuf,'\0',20);
		OLED_Refresh_Gram();
	}
}
*/




















