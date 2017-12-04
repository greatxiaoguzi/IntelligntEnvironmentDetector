#include "sys.h"
#include "usart.h"	  
#include <stdio.h>                                                                                            
#include "cJSON.h"
#include "gizwits_protocol.h"
//////////////////////////////////////////////////////////////////////////////////
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "include.h"					//ucos 使用
#endif
//********************************************************************************
//V1.3修改说明
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
//////////////////////////////////////////////////////////////////////////////////
 
///加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//串口1支持printf函数
//重定义fputc函数 
int fputc(int ch, FILE *f)
{
	UsartSendDex(2,(uint8_t)ch);
	return ch;
}
#endif 

/*使用microLib的方法*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
#define HCHO_FORWARDLY_MODE  1
#define REC_DATA_LEN  50
//变量和缓存区
uint16_t USART1_RX_STA = 0;    
uint16_t Wifi_Rec_Outtime = 0;  //WIFI接受超时中断
uint8_t WifiRecBuf[USART1_REC_LEN];  //Wifi接收缓存区
//uint8_t WIFIRecDataFlag = 0;    	//接收数据标志
//uint8_t WifiSetFlag = 0;   		//开机起始的时候为零
uint8_t PMS_Rec_Buf[REC_DATA_LEN];       //甲醛传感器接收到的数据缓冲区
uint8_t HCHO_Rec_Buf[10];      //甲醛数据接收缓存区
uint8_t HCHO_RecFlag = 0;
uint8_t PMRecSta = 0;     //记录PM2.5传感器接收到的数据的实际位数
uint8_t HCHORecSta = 0;   //记录甲醛传感器接收到的数据实际位数

uint16_t HCHO_Orign_Data;
//发送数据读取指令
void PMS_Send_Cmd(SensorCmd_TypeDef Status)
{
	uint8_t i;
	uint8_t PMS_Read_Cmd[7];  //读取数据帧
	uint16_t Sum = 0;
	PMS_Read_Cmd[0] = 0X42;
	PMS_Read_Cmd[1] = 0X4D;
	switch(Status)
	{
		case PMS_PASSIVITY: 
			PMS_Read_Cmd[2] = 0XE1;
			PMS_Read_Cmd[3] = 0X00;
			PMS_Read_Cmd[4] = 0X00;
			break;
		case PMS_FORWARDLY: 
			PMS_Read_Cmd[2] = 0XE1;
			PMS_Read_Cmd[3] = 0X00;
			PMS_Read_Cmd[4] = 0X01;
			break;
		case PMS_STANDBY:	 
			PMS_Read_Cmd[2] = 0XE4;
			PMS_Read_Cmd[3] = 0X00;
			PMS_Read_Cmd[4] = 0X00;
			break;
		case PMS_NORMAL: 	
			PMS_Read_Cmd[2] = 0XE4;
			PMS_Read_Cmd[3] = 0X00;
			PMS_Read_Cmd[4] = 0X01;
			break;
		case PMS_READ_DATA: 
			PMS_Read_Cmd[2] = 0XE2;
			PMS_Read_Cmd[3] = 0X00;
			PMS_Read_Cmd[4] = 0X00;
			break;
		default:break;
	}
	Sum = PMS_Read_Cmd[0]+PMS_Read_Cmd[1]+PMS_Read_Cmd[2]+PMS_Read_Cmd[3]+PMS_Read_Cmd[4];
	PMS_Read_Cmd[5] = Sum>>8;
	PMS_Read_Cmd[6] = Sum&0XFF;
	for(i=0;i<7;i++)
	{
		UsartSendDex(4,PMS_Read_Cmd[i]);
	}
}
/*
//甲醛传感器工作模式及其数据读取设置
void HCHO_Sensor_Mode(SensorCmd_TypeDef Mode)
{
	uint8_t CmdBuf[9];
	uint8_t i;
	CmdBuf[0] = 0XFF;
	CmdBuf[1] = 0X01;
	switch(Mode)
	{
		case HCHO_FORWARDLY:    //主动式
		{
			CmdBuf[2] = 0X78;
			CmdBuf[3] = 0X40;
		}break;
		case HCHO_PASSIVITY:    //被动式
		{
			CmdBuf[2] = 0X78;
			CmdBuf[3] = 0X41;
		}break;
		case HCHO_READ_DATA:    //读数据
		{
			CmdBuf[2] = 0X86;
			CmdBuf[3] = 0X00;
		}break;
		default:break;
	}
	CmdBuf[4] = 0X00;
	CmdBuf[5] = 0X00;
	CmdBuf[6] = 0X00;
	CmdBuf[7] = 0X00;
	for(i=1;i<8;i++)
	{
		CmdBuf[8] += CmdBuf[i];
	}
	CmdBuf[8] = ~CmdBuf[8]+1;
	for(i=0;i<9;i++)
		UsartSendDex(2,CmdBuf[i]);
}*/
void HCHOSetPasty(SensorCmd_TypeDef Status)
{
	uint8_t i;
	uint8_t CmdBuf[9];
	CmdBuf[0] = 0XFF;
	CmdBuf[1] = 0X01;
	CmdBuf[2] = 0X78;
	if(Status == HCHO_PASSIVITY)
	{
		CmdBuf[3] = 0X41;
		CmdBuf[8] = 0X46;
	}
	else if(Status == HCHO_FORWARDLY)
	{
		CmdBuf[3] = 0X40;
		CmdBuf[8] = 0X47;
	}
	CmdBuf[4] = 0X00;
	CmdBuf[5] = 0X00;
	CmdBuf[6] = 0X00;
	CmdBuf[7] = 0X00;
	//CmdBuf[8] = 0X46;
	for(i=0;i<9;i++)
	{
		UsartSendDex(2,CmdBuf[i]);
	}
}
void HCHOReadData(void)
{
	uint8_t i;
	uint8_t CmdBuf[9];
	CmdBuf[0] = 0XFF;
	CmdBuf[1] = 0X01;
	CmdBuf[2] = 0X86;
	CmdBuf[3] = 0X00;
	CmdBuf[4] = 0X00;
	CmdBuf[5] = 0X00;
	CmdBuf[6] = 0X00;
	CmdBuf[7] = 0X00;
	CmdBuf[8] = 0X79;
	for(i=0;i<9;i++)
		UsartSendDex(2,CmdBuf[i]);
}
//初始化IO 串口1 串口3 串口4
//bound:波特率
//串口1： TX PA.9   RX	PA.10  串口3：TX PB10   RX	 PB11  串口4： TXPC10   RXPC11
void uart_init(u32 bound1,u32 bound2,u32 bound4)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//使能USART1，GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB1Periph_UART4, ENABLE);	//使能USART1，USART4时钟时钟
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
//    USART_DeInit(USART1);  //复位串口1
//    USART_DeInit(USART3);
//    USART_DeInit(UART4);
  
    //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9  
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10		
	//USART2_TX	  PA2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
	//USART2_RX   PA3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10	
    //USART4_TX PC10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化C10	
		//USART4_RX	  PC11d
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化PC11
      
     //USART1 初始化设置
    USART_InitStructure.USART_BaudRate = bound1;//一般设置为9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART1, &USART_InitStructure); //初始化串口1
    //USART3 初始化设置
    USART_InitStructure.USART_BaudRate = bound2;//一般设置为9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART2, &USART_InitStructure); //初始化串口3
     //USART4 初始化设置
    USART_InitStructure.USART_BaudRate = bound4;//一般设置为9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(UART4, &USART_InitStructure); //初始化串口4   
    
	//Usart1 NVIC 配置   WIFI数据传输
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器 
    //Usart2 NVIC 配置  甲醛
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器 
    //Usart4 NVIC 配置  PM2.5
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2 ;//抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器 
	
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断1
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断3
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启中断4
    USART_Cmd(USART1, ENABLE);                    //使能串口1
    USART_Cmd(USART2, ENABLE);
    USART_Cmd(UART4, ENABLE);
//使能串口3
}
//数据校验计算
uint8_t CheckSum(uint8_t *buf,uint8_t len)
{
    uint8_t i;
    uint8_t temp=0;
    for(i=0;i<len;i++)
    {
       temp^=*(buf+i);
    }      
    return temp;
}

//串口中断接收程序
//判断接收到的数据帧
//+IPD,0,7:数据区
//OK
#if EN_USART1_RX   //如果使能了接收
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	uint8_t Data;
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.进入中断保护
   OSIntEnter();    //进入OS中断保护
#endif
   if(USART1->SR&(1<<5))  //判断是否为串口1接收中断	
   {
			Data = USART1->DR;
			gizPutData(&Data, 1);
   }
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  			//退出中断保护									 
#endif
}
#endif
void HCHODataHandle(void)
{
	SensorData.HCHO = HCHO_Orign_Data;//Sum/10;
	HCHO_RecFlag = 1;
}
uint8_t HCHOFindPkg(uint8_t *buf)
{
	uint8_t i;
	for(i=0;i<HCHORecSta;i++)
	{
		#if HCHO_FORWARDLY_MODE==1  //主动式
		if((*(buf+i))==0XFF && (*(buf+i+1))==0x17)
		{
			if(*(buf+i+8) != 0)
				return i;
		}
		#else  //被动式
		if((*(buf+i))==0XFF && (*(buf+i+1))==0x86)
		{
			if(*(buf+i+8) != 0)
				return i;
		}
		#endif
	}
	return 0XFF;
} 
//帧完整性检查
uint8_t HCHOFrameCheck(uint8_t *buf)
{
	uint8_t i;
	uint8_t Sum=0;
	for(i=1;i<8;i++)
		Sum += *(buf+i);
	Sum = ~Sum + 1;
	if(*(buf+8) == Sum)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
static uint8_t Cnt = 0;
//帧数据处理
void HCHOFrameDataDeal(uint8_t *buf)
{
	#if HCHO_FORWARDLY_MODE==1
	HCHO_Orign_Data = ((*(buf+4)<<8) | *(buf+5))/10;
	#else
	HCHO_Orign_Data = ((*(buf+2)<<8) | *(buf+3))/10;
	#endif
	HCHORecSta = 0;
}
//串口3中断函数
void USART2_IRQHandler(void)                	//串口1中断服务程序
{
	uint8_t Data = 0;
	//uint8_t Index = 0XFF;
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.进入中断保护
OSIntEnter();    
#endif	
	if(USART2->SR&(1<<5))  //接收中断
	{
		Data = USART2->DR;
		if(HCHORecSta < REC_DATA_LEN)
		{
			HCHO_Rec_Buf[HCHORecSta] = Data;        //保存数据
			if(HCHO_Rec_Buf[0] == 0XFF)
			{
				HCHORecSta ++;   //用于字节接收
				if(HCHORecSta >= 9)
				{
					HCHORecSta = 0;
					if(!HCHOFrameCheck(&HCHO_Rec_Buf[0]))
					{
						if(HCHO_RecFlag)
						{
							HCHOFrameDataDeal(&HCHO_Rec_Buf[0]);
						}
						SensorOnline.Device.WZ_S = 1;
					}
				}
			}
		}
		/*Index = HCHOFindPkg(HCHO_Rec_Buf);      //帧判断,寻找帧头
		if(Index != 0XFF)
		{
			
			if(!HCHOFrameCheck(&HCHO_Rec_Buf[Index]))
			{
				
				HCHOFrameDataDeal(&HCHO_Rec_Buf[Index]);
				
			}
		}*/
		if(HCHORecSta >= REC_DATA_LEN)
		{
			HCHORecSta = 0; 
			memset((void *)&HCHO_Rec_Buf[0],0,REC_DATA_LEN);  //清空缓冲区
		}
	}
#ifdef OS_TICKS_PER_SEC	 	
	OSIntExit();  					//退出中断保护
#endif	
}
//寻找帧头
uint8_t FindPkgHead(uint8_t *buf)
{
	uint8_t i;
	for(i=0;i<PMRecSta;i++)
	{
		if((*(buf+i)==0X42) && (*(buf+i+1)==0X4D))
		{
			if(PMRecSta == ((*(buf+i+2)<<8|*(buf+i+3))+4+i))
			{
				return i;
			}
		}
	}
	return 0XFF;
}
//得到校验值
uint16_t Get_Sum_Val(uint8_t *buf)
{
	uint8_t i;
	uint16_t Sum = 0;
	for(i=0;i<30;i++)
	{
		Sum += *(buf+i);
	}
	return Sum;
}
//数据判检查
uint8_t Data_Check(uint8_t *buf)
{
	if(*(buf+0)!=0x42 || *(buf+1)!=0x4d)
	{
		return 1;
	}
    if((uint16_t)(*(buf+30)<<8|*(buf+31)) != Get_Sum_Val(buf))
		return 1;
	return 0;
}
//数据处理
void Data_Deal(uint8_t *buf)
{
	SensorData.PMData.PM1_0_S = (uint16_t)(*(buf)<<8|*(buf+1));
	SensorData.PMData.PM2_5_S = (uint16_t)(*(buf+2)<<8|*(buf+3));
	SensorData.PMData.PM10_S = (uint16_t)(*(buf+4)<<8|*(buf+5));
	SensorData.PMData.PM1_0_A = (uint16_t)(*(buf+6)<<8|*(buf+7));
	SensorData.PMData.PM2_5_A = (uint16_t)(*(buf+8)<<8|*(buf+9));
	SensorData.PMData.PM10_A = (uint16_t)(*(buf+10)<<8|*(buf+11));
	
	SensorData.PMData.Cnt_0_3 = (uint16_t)(*(buf+12)<<8|*(buf+13));
	SensorData.PMData.Cnt_0_5 = (uint16_t)(*(buf+14)<<8|*(buf+15));
	SensorData.PMData.Cnt_1_0 = (uint16_t)(*(buf+16)<<8|*(buf+17));
	SensorData.PMData.Cnt_2_5 = (uint16_t)(*(buf+18)<<8|*(buf+19));
	SensorData.PMData.Cnt_5_0 = (uint16_t)(*(buf+20)<<8|*(buf+21));
	SensorData.PMData.Cnt_10 = (uint16_t)(*(buf+22)<<8|*(buf+23));
	//SensorData.HCHO = (uint16_t)(*(buf+24)<<8|*(buf+25))+HCHO_MAKE_UP;
}
//串口4中断服务函数用于甲醛数据接收
//0XA5  0X5A 0X06 0X80 0Xxx  0Xxx 0Xxx  0Xxx 0XAA
//   起始    数量  指令   甲醛mg     甲醛ppm  结束
void UART4_IRQHandler(void)                	//串口1中断服务程序
{
    uint8_t Inchar =0XFF;
    uint8_t Index = 0;
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.进入中断保护
	 OSIntEnter();
#endif	
	if(UART4->SR&(1<<5))  //如果是接收到中断
	{
		Inchar = UART4->DR;
		if(PMRecSta < REC_DATA_LEN)
		{
			PMS_Rec_Buf[PMRecSta] = Inchar;        //保存数据
			PMRecSta++;   //用于字节接收
		}
		Index = FindPkgHead(PMS_Rec_Buf);      //帧判断,寻找帧头
		if(Index!=0XFF)
		{
			if(!Data_Check(&PMS_Rec_Buf[Index]))
			{
				Data_Deal(&PMS_Rec_Buf[Index+4]);
				SensorOnline.Device.PMS_5 = 1;
			}
			else
			{
				memset((void *)&PMS_Rec_Buf[0],0,REC_DATA_LEN);
			}
		}
		if(PMRecSta >= REC_DATA_LEN)
		{
			PMRecSta = 0; 
			memset((void *)&PMS_Rec_Buf[0],0,REC_DATA_LEN);  //清空缓冲区
		}
	}
#ifdef OS_TICKS_PER_SEC
	OSIntExit();  					//退出中断保护				 		 
#endif	
}
//串口发送十六进制数据
void UsartSendDex(uint8_t Uartnum,uint8_t data)
{
	switch(Uartnum)
	{
		case 1:
		{
			while((USART1->SR&0X40)==0);
			USART1->DR = data;
		}break;
		case 2:
		{
			while((USART2->SR&0X40)==0);
			USART2->DR = data;
		}break;
		case 4:
		{
			while((UART4->SR&0X40)==0);
			UART4->DR = data;
		}break;
		default:break;
	}
}
//串口发送字符串
void UsartSendString(uint8_t Uartnum,char * data)
{
	switch(Uartnum)
	{
		case 1:
		{
			while(*data)
			{
				while((USART1->SR&0X40)==0);
				USART1->DR = *data;
				data++;
			}
		}break;
		case 2:
		{
			
		}break;
		case 4:
		{
			
		}break;
		default:break;
	}
}
//串口缓冲区清零
void ClearBuf(void)
{
	uint16_t i;
	for(i=0;i<USART1_REC_LEN;i++)
		WifiRecBuf[i] = '\0';
	//UartRecBuf[USART_REC_LEN-1]= '\0';
	USART1_RX_STA = 0;
//	USART1_RX_Tout = 0;
}
//清除数据缓冲区
void ClearDataBuffer(uint8_t *Data)
{
   memset((void *)Data,sizeof(Data),0);
}
/********************************乐为服务器上传数据*****************************************/
//const uint8_t *LeWeiStr = "URL:http://www.lewei50.com/api/V1/gateway/UpdateSensors/02\r\nMethod:post\r\n--Header:userkey:f622fbf27ebf45408303618cc41d1060\r\nPOST Data:";
//网关号，用户秘钥
uint8_t PkgURL(uint8_t GateWay,uint8_t *UserKey)
{	
	uint8_t res;
	uint8_t buf[200];
	if(Esp8266_Send_Cmd("AT+CIPSEND","OK",200))
		return 1;
	ClearBuf();
	sprintf(buf,"{\"method\": \"update\",\"gatewayNo\": \"%02d\",\"userkey\": \"%s\"}&^!",GateWay,UserKey);
	if(!Esp8266_SendData(buf,"&^!",500))
	{
		if(!Sensor_Data_With_cJson())  //先上传一次数据
		{
			SetBeepFreq(AlarmFreqTab[6]);
			return 0;
		}
		else
			return 1;
	}
	else
		return 1;
}

//{"method": "upload","data":[{"Name":"Temp","Value":"33"},{"Name":"HUMI","Value":"96.2"}]}&^!
//创建cjson指针函数
//数据转换为cjson格式
uint8_t Sensor_Data_With_cJson(void)
{
	uint8_t i;
	uint8_t SendStat;
	char *pOut = 0;
	cJSON * pJsonRoot = NULL;
	cJSON * pJsonRoot0 = NULL;
	cJSON * pJsonSensor[7];
	pJsonRoot0 = cJSON_CreateObject();
	//pJsonRoot = cJSON_CreateArray();
	if(NULL == pJsonRoot0)
	{
		return 1;
	}
	cJSON_AddStringToObject(pJsonRoot0, "method", "upload");
	cJSON_AddItemToObject(pJsonRoot0,"data",pJsonRoot=cJSON_CreateArray());
	
	cJSON_AddItemToObject(pJsonRoot,"Sensor1",pJsonSensor[0]=cJSON_CreateObject());
	cJSON_AddStringToObject(pJsonSensor[0],"Name","Temp");
	cJSON_AddNumberToObject(pJsonSensor[0],"Value",SensorData.Temp);
	
	cJSON_AddItemToObject(pJsonRoot,"Sensor2",pJsonSensor[1]=cJSON_CreateObject());
	cJSON_AddStringToObject(pJsonSensor[1],"Name","HUMI");
	cJSON_AddNumberToObject(pJsonSensor[1],"Value",SensorData.Humi);
	if(Power_5V_IO_Status==0 && SampleCnt==2)   //如果传感器电源打开的话
	{
		cJSON_AddItemToObject(pJsonRoot,"Sensor3",pJsonSensor[2]=cJSON_CreateObject());
		cJSON_AddStringToObject(pJsonSensor[2],"Name","PM2.5");
		cJSON_AddNumberToObject(pJsonSensor[2],"Value",SensorData.PMData.PM2_5_S);
		
		cJSON_AddItemToObject(pJsonRoot,"Sensor5",pJsonSensor[4]=cJSON_CreateObject());
		if((SysParaSetInfo.SensorSwitch&0x02) == 0x02)
		{
			cJSON_AddStringToObject(pJsonSensor[4],"Name","HCHO");
			cJSON_AddNumberToObject(pJsonSensor[4],"Value",SensorData.HCHO);
		}
		else if((SysParaSetInfo.SensorSwitch&0x02)!=0x02)
		{
			cJSON_AddStringToObject(pJsonSensor[4],"Name","PM1.0");
			cJSON_AddNumberToObject(pJsonSensor[4],"Value",SensorData.PMData.PM1_0_S);
		}
		
		cJSON_AddItemToObject(pJsonRoot,"Sensor6",pJsonSensor[5]=cJSON_CreateObject());
		if((SysParaSetInfo.SensorSwitch&0x01) == 0x01)
		{
			cJSON_AddStringToObject(pJsonSensor[5],"Name","CO2");
			cJSON_AddNumberToObject(pJsonSensor[5],"Value",SensorData.Carbon);
		}
		else if((SysParaSetInfo.SensorSwitch&0x01) != 0x01)
		{
			cJSON_AddStringToObject(pJsonSensor[5],"Name","PM10");
			cJSON_AddNumberToObject(pJsonSensor[5],"Value",SensorData.PMData.PM10_S);
		}
	}
	cJSON_AddItemToObject(pJsonRoot,"Sensor7",pJsonSensor[6] = cJSON_CreateObject());
	cJSON_AddStringToObject(pJsonSensor[6],"Name","AirPress");
	cJSON_AddNumberToObject(pJsonSensor[6],"Value",SensorData.AirPres/1000);
	pOut = cJSON_Print(pJsonRoot0);
	if(NULL == pOut)
	{
		cJSON_Delete(pJsonRoot0);
		return 0XF1;
	}
	strcat(pOut,"&^!");
	cJSON_Delete(pJsonRoot0);
	if(Esp8266_SendData(pOut,"&^!",660))
	{
		SendStat = 0x01;
	}
	else
	{
		SendStat = 0x00;
	}
	myfree(pOut);
	return SendStat;
}
//CJSON数据解析函数
void parseJson(char * pMsg)
{
	cJSON * pJson;
	cJSON * pSub;
	cJSON * pSubSub;
    if(NULL == pMsg)
    {
        return;
    }
    pJson = cJSON_Parse(pMsg);
    if(NULL == pJson)                                                                                         
    {
        // parse faild, return
        return;
    }

    //get string from json
    pSub = cJSON_GetObjectItem(pJson, "hello");
    if(NULL == pSub)
    {
        //get object named "hello" faild
    }
    //printf("obj_1 : %s\n", pSub->valuestring);

    // get number from json
    pSub = cJSON_GetObjectItem(pJson, "number");
    if(NULL == pSub)
    {
        //get number from json faild
    }
    //printf("obj_2 : %d\n", pSub->valueint);

    // get bool from json
    pSub = cJSON_GetObjectItem(pJson, "bool");
    if(NULL == pSub)
    {
        // get bool from json faild
    }                                                                                                         
    //printf("obj_3 : %d\n", pSub->valueint);

 // get sub object
    pSub = cJSON_GetObjectItem(pJson, "subobj");
    if(NULL == pSub)
    {
        // get sub object faild
    }
    pSubSub = cJSON_GetObjectItem(pSub, "subjsonobj");
    if(NULL == pSubSub)
    {
        // get object from subject object faild
    }
    //printf("sub_obj_1 : %s\n", pSubSub->valuestring);
    cJSON_Delete(pJson);
}
/*若干说明
1. cJSON_CreateObject函数可创建一个根数据项，之后便可向该根数据项中添加string或int等内容
2. cJSON_AddNumberToObject向节点中添加子节点，例如此处添加value节点，节点值为123.4
3. cJSON_Print函数可以打印根数据项，加入制表符换行符等标识符使得JSON数据包更易阅读
4. 使用free函数释放被out占用的内存空间
*/
/*void CreatCjson(void)
{
    
    cJSON *root = cJSON_CreateObject(); // 创建JSON Object 
     
    cJSON_AddNumberToObject(root,"value",123.4);  // 加入节点（键值对），节点名称为value，节点值为123.4 
    
    char *out = cJSON_Print(root);  // 打印JSON数据包  
    printf("%s\n",out);  
   
    cJSON_Delete(root);   // 释放内存  
    free(out);
}*/
void FreecJSONSpace(char *p)
{
    free(p);             ///释放内存空间
}




