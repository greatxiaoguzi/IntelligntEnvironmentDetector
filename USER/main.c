/*
软件说明：新增芯片内部flash读写功能，用于参数保存
新增文件;eeprom.c   stm32f10x_flash.c

*/

//主函数：实现任务的创建以及子任务的实现
//任务堆栈的创建
#include "include.h"
extern const unsigned char WifiLogo[32];
//extern const unsigned char NoWifiLogo[32];
extern const unsigned char gImage_123[19494];
//任务堆栈HH
OS_STK START_TASK_STK[START_STK_SIZE];//开始任务堆栈
/*
//得到SD卡的状态
void GetSDCapcity(void)
{
	BYTE res;
	uint32_t total=0,free=0;
	uint32_t Process;
	BACK_COLOR = BLACK;
	res = exf_getfree("0",&total,&free);  //得到SD卡的容量大小
	if(res)
		Show_Str(1,10,100,20,"无SD卡存在",16,0,RED,LIGHTBLUE);
	else
	{
		total = total>>10;
		free = free>>10;
		Show_Str(200,10,200,16,"SD卡总容量:     MB",16,0,BLUE,LIGHTBLUE);
		LCD_ShowNum(290,10,total,5,16,BLACK,LIGHTBLUE);
		Show_Str(350,10,200,16,"SD卡可用容量:     MB",16,0,BLUE,LIGHTBLUE);
		LCD_ShowNum(355,10,free,5,16,BLACK,LIGHTBLUE);
		
		LCD_DrawRectangle(0,10, 180, 30,GRAY);  //画矩形
		LCD_Fill(1,11,179,29,LGRAY);
		Process = (uint32_t)(total-free)*248/total;
		LCD_Fill(1 ,11 ,1+Process+1,29,DARKBLUE);
	}
	delay_ms(2000);
	LCD_Clear(BLACK);
}
*/
//%SSID=AP_02C1E0&%01004566&#
//%USERKEY=af1c24b429134d6cbcb1c255f021277a&#
//%SVN=101.37.32.173&%9960&#
void InitHardware(void)
{
	SensorOnline.Status = 0x00;  		//刚开始没有设备的在线状态位全部为零
	LED_Init();
	Beep_init(7999);
	Micro_Wave_ReacT_Init();
	PMS5003S_Init();
	HCHOSetPasty(HCHO_FORWARDLY);   	//设置甲醛传感器为被动式接收数据模式
	EncoderInit();                  	//按键初始化
	Senseair8_Init(0XFFFF,72-1);  		//CO2传感器初始化，以1MHz频率计数,最大计数值为65535,溢出时间为：0.000001*65535=0.065S > 传感器工作频率周期为0.001S
	RX8025t_Init();
	AT24CXX_Init();							//字库flash初始化
    if(BMP1820Init())                    	 	//气压传感器初始化
	{
		SensorOnline.Device.BMP180 = 0;
	}
	if(SHT30_Init)  		//温湿度传感器初始化
	{
		SensorOnline.Device.SHT30 = 1;
	}
	else
		SensorOnline.Device.SHT30 = 0;
	LCD_Fill(0,0,480,320,PARA_SHOW_INTERFACE_BACKCOLOR);
	SetBeepFreq(AlarmFreqTab[8]);
	delay_ms(400);
	TIM_Cmd(TIM3,DISABLE);
	LD3320_Init();
}

//主函数
int main(void)
{
//	uint32_t Adcval;
    sys_clock_init(9);   		             	//系统时钟初始化
	SCB->VTOR = FLASH_BASE | 0xE000;  			//* APPP向量地址，APP程序起始基地址为：0x8010000 */	
    delay_init();      		                	//延时初始化   
	NVIC_Configuration();                    	//设置中断分组
	StartMachineInit();							//机器供电控制初始化
    uart_init(115200,9600,9600);     		  	//串口1,3，4初始化初始化()	
    TFTLCD_Init();      						//彩屏初始化  
    LED_Init();  								//LED初始化
	mem_init();									//内存池初始化	
	exfuns_init();								//文件系统初始化
	f_mount(fs[0],"0:",1); 						//挂载SD卡 
	f_mount(fs[1],"1:",1); 						//挂载FLASH.    
	Adc_Init(); 								//ADC转换初始化
//    ADC_DMAInit(DMA1_Channel1,(u32)&ADC1->DR,(u32)ADC_ChangeValue,4);  //DMA传输初始化
	if(Font_Init())							//字库初始化检查是否OK
		LCD_ShowString(100,100,100,16,16,"Font Init Fail!",RED,BLACK);
	if(SD_Initialize())							//SD卡初始化
		LCD_ShowString(100,120,100,16,16,"SD Init Fail!",RED,BLACK);
	InitHardware();   							//初始化传感器硬件部分及存储器部分
	ReadSetPara();    							//配置设置参数*/ 
//	while(1)
//	{
////		Adcval = Get_Adc_Average(ChalTab[1],5);
//		Adcval = bmp180Convert();
//		LCD_ShowNum(100,100,Adcval,6,24,0,BLUE,BLACK);
//		delay_ms(400);
//		LED0 =! LED0;
//	}
    //进入操作系统，创建任务
    OSInit();            						//UCOS初始化
    OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);     //创建开始任务
    OSStart();           						//开始任务
}

//end of file....




