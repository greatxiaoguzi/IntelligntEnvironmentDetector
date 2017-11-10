#include "include.h"
//#include <locale.h>
//美标计算表
const float USASt_Tab[7][4]={
	{0,15.4,0,50},
	{15.5,40.4,51,100},
	{40.5,65.4,101,150},
	{60.5,150.4,151,200},
	{150.5,250.4,201,300},
	{250.5,350.4,301,400},
	{350.5,500.4,401,500}
};
//状态功能结构体
const StatusTab EncoderTab[MENU_SIZE] =
{      //索引           按下          顺            逆                  函数指针
	{Para_Show,     Para_Show,    PM2_5_Show,   Set_Show,     (*Load_ParaShow_Interface_1)},  	//界面1，所有参数显示界面
	{PM2_5_Show,    PM2_5_Show,   WaveForm_Show,Para_Show,    (*Load_PMCnctrShow_Interface_2)},	//界面2，颗粒物显示界面
	{WaveForm_Show, WaveForm_Show,Set_Show,     PM2_5_Show,   (*Load_WaveformShow_Interface_3)}, //界面3，波形显示界面
	{Set_Show,      Set_Prar_Show,Para_Show,    WaveForm_Show,(*Load_SetShow_Interface_4)},      //界面4，参数设置界面
	
	{Set_Prar_Show,11,5,10,(*SetLay1UI_Time)},  			//界面4，设置界面
	{5,16,6,4,(*SetLay1UI_AutoStandby)},
	{6,17,7,5,(*SetLay1UI_DeviceSW)},
	{7,18,8,6,(*SetLay1UI_DataRemUnit)},
	{8,19,9,7,(*SetLay1UI_Threshold)},
	{9,39,10,8,(*SetLay1UI_WIFIConfig)},
	{10,0,4,9,(*SetLay1UI_Quit)},
	//日期设置
	{11,12,0,0,(*SetLay2UI_Time_Year)}, 	//设置年
	{12,13,0,0,(*SetLay2UI_Time_Month)}, 	//设置月
	{13,14,0,0,(*SetLay2UI_Time_Day)},		//设置日期
	{14,15,0,0,(*SetLay2UI_Time_Hour)},		//设置小时
	{15,Set_Prar_Show,0,0,(*SetLay2UI_Time_Minute)},  //设置分钟，完成后调到第一层菜单上
	
	{16,34,0,0, (*SetLay2UI_AutoStandby)},  	//设置完后返回到第一层原界面
	{17,40,0,0, (*SetLay2UI_DevicePower)},
	{18,35,0,0, (*SetLay2UI_SDRemUnit)},
	{19,26,0,0, (*SetLay2UI_ThresHold)},
	
	//阈值设置
	{20,21,0,0,(*Set_PM2_5_Threshold)},
	{21,22,0,0,(*Set_HCHO_Threshold)},
	{22,23,0,0,(*Set_CO2_Threshold)},
	{23,24,0,0,(*Set_Temp_Threshold)},
	{24,27,0,0,(*Set_Humi_Threshold)},  				//调到UI3设置阈值范围状态中
	
	{25,9,0,0,(*LinkWifi_Enable)},		

	{26,20,27,36,(*SetLay3UI_ThresHold_Status)},  		//阈值状态模式
	{27,28,36,26,(*SetLay3UI_ThresHold_Adjust)},		//阈值范围调节
	
	{28,29,0,0,(*Adjust_PM2_5_Val)},
	{29,30,0,0,(*Adjust_HCHO_Val)},
	{30,31,0,0,(*Adjust_CO2_Val)},
	{31,32,0,0,(*Adjust_Temp_Val)},
	{32,36,0,0,(*Adjust_Humi_Val)},
	{33,0,33,33,(*Adjust_Back_Light)},
	{34,42,0,0, (*SetLay2UI_InterfaceSwitch)},
	{35,7,0,0, (*SetLay2UI_SensoeSamplingUnit)},
	{36,37,26,27,(*SetLay3UI_Sensor_Status)}, 
	{37,38,0,0,(*Set_HCHOSensor_Status)},
	{38,8,0,0, (*Set_CO2Sensor_Status)},
	{39,25,0,0,(*SetLay2UI_WifiStatus)},
	{40,41,0,0,(*SetLay2UI_SpeechRecgSw)},
	{41,6,0,0,(*SetLay2UI_SpeechRecgVol)},
	{42,5,0,0,(*SetLay2UI_AutoStandbyTime)},
//	{43,},
	
};
//变量定义
uint8_t  EncoderFuncIndex = 0;    		//存放当前的菜单索引
void (*EncoderFuncPtr)(void);            //定义要跳转的函数指针
extern uint8_t CloseMachineFlag;
SysSetPara_TypeDef SysParaSetInfo;   		//系统设置参数信息
SensorOnlineStat_TypeDef SensorOnline;      //传感器在线状态
int16_t BackLightBright = 0;
DataRemember_TypeDef DataRemember_Hour;

static uint32_t SD_Write_Times = 0;  //写sd卡时的便宜量
uint8_t StandbyMode = 0;
uint8_t pname[50] = "0:/SensorMemory/SensorData.txt";   //用来存储新创建的文件名
uint8_t AutoJumpFlag = 0;  //自动跳转到主函数标志
Interface_TypeDef Current_Show_Interface = Curr_No_Show;  //当前显示界面
static uint8_t LoadStaticUIFlag = 0;
uint8_t DispWifiInfoFlag = 0;    //显示WIFI的信息标志
uint16_t AQI_Value;

extern const unsigned char CloudUpload[106];
extern const unsigned char NoCloudUpload[106];
extern const unsigned char TempLogo[];
extern const unsigned char HumiLogo[];
extern const unsigned char AirPressLogo[];
const uint8_t Item_Id[7] = {HCHO_ID,PM2_5_ID,CARBON_ID,TEMP_ID,HUMI_ID,AIRPRES_ID};
const uint16_t QualityGradeColor[] = {C_LIME_GREEN,YELLOW,ORANGE,RED,PURPLE,MAROON};
uint8_t const *QualityGrade[]={"优","良","轻","中","重","爆","AQI"};
uint8_t const *SetUIL1_Text[] = {"1.系统时间设置>","2.自动待机设置>","3.设备开关设置>","4.时间参数设置>","5.状态开关设置>","6.WIFI网络设置>","7.退出"};
uint8_t const *SetUIL2_Text[] = {"年","月","日","时","分","打开","关闭","PM2_5","HCHO","CO2","温度","湿度","PM1.0","PM10"};                                                                                                      
uint8_t* const AirQuiyGrade[8][6] =
{
	{"PM2.5:优  ","PM2.5:良  ","轻度污染","中度污染","重度污染","严重污染"},
	{"甲醛良好","甲醛较高","甲醛超标"},
	{"  无数据  ","CO2良好","CO2较高","CO2超标"},
	{"极小颗粒:优","极小颗粒:良","极小颗粒:轻","极小颗粒:中","极小颗粒:重","极小颗粒:爆"},
	{"较大颗粒:优","较大颗粒:良","较大颗粒:轻","较大颗粒:中","较大颗粒:重","较大颗粒:爆"},
	{"温度适宜,","温度偏高,","高温预警,"},
	{"湿度适宜!","空气偏干!","空气干燥!"},
	{"无数据"}
};
static uint16_t CurrThemeColorFlag[3] = {0,0,0};  //更行主题背景的颜色
static uint16_t PreThemeColorFlag[3] = {0,0,0};
uint8_t AlarmCloseFlag = 0;   //闹钟关闭标志,分别关闭对应数据的闹钟，只关闭当前的，不保存参数
const uint16_t ValueTab[] = {0,35,75,115,150,250,500};//色条上的数值
uint8_t Draw_Wave_Flag = 0;  //绘制每5分钟记录的波形防止每次都绘制全部

const DispCirDef AllDispCirInfo[6]=
{      //这里长和宽用外圆和内圆代替
	{15, 64,103, 105,27,90,  {RED,DARKBLUE,   RED,  PARA_SHOW_INTERFACE_BACKCOLOR,   DARKBLUE,FillColor},	 "PM2.5","ug/m3",60},	
	{163,64,103, 105,27,90,  {RED,GRAYBLUE,   RED,  PARA_SHOW_INTERFACE_BACKCOLOR,   DARKBLUE,FillColor},    "甲醛","mg/m3",60},
	{312,64, 103,105,32,90,  {RED,GRAYBLUE,   RED,  PARA_SHOW_INTERFACE_BACKCOLOR,   DARKBLUE,FillColor},    "CO2","ppm",60},	
	
	{65,235, 30,27,22,  23,  {WHITE,GRAYBLUE, C_LIME_GREEN,C_LIME_GREEN, PARA_SHOW_INTERFACE_BACKCOLOR,PARA_SHOW_INTERFACE_BACKCOLOR},  "温度",     "℃",36},
	{215,235, 30,27,27,  23,  {WHITE,GRAYBLUE, C_LIME_GREEN,C_LIME_GREEN, PARA_SHOW_INTERFACE_BACKCOLOR,PARA_SHOW_INTERFACE_BACKCOLOR},  "湿度",     "%",36},	
	{340,235, 30,27,45, 23,  {WHITE,GRAYBLUE, C_LIME_GREEN,C_LIME_GREEN, PARA_SHOW_INTERFACE_BACKCOLOR,PARA_SHOW_INTERFACE_BACKCOLOR},  "   气压与海拔",    "KPa",24},		
};
//锂电池电压电压向ADC值得转换
const uint16_t Vol_To_ADC[13]=
{
	2332,3263,3425,3449,3474,
	3511,3524,3561,3598,3660,
	3722,3797,3908
};
//唤醒待机的机器从待机中
//void aWake_Standby(void)
//{
//	if(SysParaSetInfo.AutoStandbyStatus)  //开关已开启了的话
//	{
//		TIM2->CCR2 = BackLightBright;  	 	//开启显示屏显示
//		Set_PMS5003S_Status(PMS_RUNING);  //开启PM2.5传感器
//		OSTmrStart(tmr2,&err);	   //开启软件定时器
//	}
//	Encoder_Type = ENCODER_VOID;
//}
//调节背光亮度
void Adjust_Back_Light(void)
{
	int16_t Temp;
	Show_Str(240,300,100,12,"亮度调节",12,0,LIGHTBLUE,BLACK);
	Temp = TIM2->CCR2;
	delay_ms(300);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)  //渐亮
		{
			Encoder_Type = ENCODER_VOID;
			Temp -=50;
			if(Temp<0)
				Temp = 0;
			TIM2->CCR2 = (uint16_t)Temp;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)//渐暗
		{
			Encoder_Type = ENCODER_VOID;
			Temp +=50;
			if(Temp>999)
				Temp = 999;
			TIM2->CCR2 = (uint16_t)Temp;
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			Encoder_Type = ENCODER_VOID;
			Show_Str(240,300,100,12,"        ",12,0,LIGHTBLUE,BLACK);
			break;  //返回
		}
	}
}
//返回两个数中的最大值
uint16_t MAX(uint16_t val1,uint16_t val2)
{
	return (val1>val2)?val1:val2;
}
//返回两个数中的最小值
uint16_t MIN(uint16_t val1,uint16_t val2)
{
	return (val1>val2)?val2:val1;	
}
//保存设置的参数
void WriteSetPara(void)
{
	uint16_t DataLen;
	uint8_t *p = &SysParaSetInfo.AutoStandbyStatus;
	DataLen = sizeof(SysSetPara_TypeDef);
//	OS_CPU_SR cpu_sr=0;
//	OS_ENTER_CRITICAL();	//进入临界区(无法被中断打断) 
	AT24CXX_Write(EEPROM_ADDR_SYS_PARA_SETPARA,p,DataLen);   //写入参数
	AT24CXX_WriteOneByte(EEPROM_ADDR_SYS_PARA_CONIFG_FLAG,0XA5);
//	OS_EXIT_CRITICAL();		//退出临界区(可以被中断打断)
}
//读取EEPROM里面的设置的数据
void ReadSetPara(void)
{
	DIR dir;
	uint8_t res;
	uint16_t BackPar;
	uint8_t TempData =0;
	uint8_t ConfigFlag = 0;
	SysSetPara_TypeDef *Temp;
	uint16_t Len = sizeof(SysSetPara_TypeDef);
	uint8_t buf[40];
	ConfigFlag = AT24CXX_ReadOneByte(EEPROM_ADDR_SYS_PARA_CONIFG_FLAG);  //读取是否配置标志
	if(ConfigFlag == 0XA5)   //不是第一次配置
	{
		TempData = AT24CXX_ReadOneByte(EEPROM_ADDR_SYS_PARA_BACKLIGHT/*250*/);   //读取背光显示的PWM数值
		delay_ms(1);
		BackPar = (uint16_t)(TempData<<8);
		TempData = AT24CXX_ReadOneByte(EEPROM_ADDR_SYS_PARA_BACKLIGHT+1/*251*/);
		BackPar |= TempData;
		BackLightBright = BackPar;
		AT24CXX_Read(EEPROM_ADDR_SYS_PARA_SETPARA/*233*/,buf,Len);
		Temp = (SysSetPara_TypeDef*)buf;
		SysParaSetInfo.AutoStandbyStatus = Temp->AutoStandbyStatus;
		SysParaSetInfo.AutoStandbyTime = Temp->AutoStandbyTime;
		SysParaSetInfo.InterfaceSwitch = Temp->InterfaceSwitch;
		SysParaSetInfo.Power_5V_Status = Temp->Power_5V_Status;
		SysParaSetInfo.SDRemTimeUnit = Temp->SDRemTimeUnit;
		SysParaSetInfo.SensorSamplingTimeUnit = Temp->SensorSamplingTimeUnit;
		SysParaSetInfo.SensorSwitch = Temp->SensorSwitch;
		SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus = Temp->ThresHoldStatus.SensorAlarmStatus;
		SysParaSetInfo.ThresHoldStatus.ThrldVal[PM2_5_ID] = Temp->ThresHoldStatus.ThrldVal[PM2_5_ID];
		SysParaSetInfo.ThresHoldStatus.ThrldVal[HCHO_ID] = Temp->ThresHoldStatus.ThrldVal[HCHO_ID];
		SysParaSetInfo.ThresHoldStatus.ThrldVal[CARBON_ID] = Temp->ThresHoldStatus.ThrldVal[CARBON_ID];
		SysParaSetInfo.ThresHoldStatus.ThrldVal[TEMP_ID] = Temp->ThresHoldStatus.ThrldVal[TEMP_ID];
		SysParaSetInfo.ThresHoldStatus.ThrldVal[HUMI_ID] = Temp->ThresHoldStatus.ThrldVal[HUMI_ID];
		SysParaSetInfo.SpeechRecognize.Speech_Sw = Temp->SpeechRecognize.Speech_Sw;
		SysParaSetInfo.SpeechRecognize.Speech_Volume = Temp->SpeechRecognize.Speech_Volume;
		//从系统参数设置界面开始读取
	}
	else //是第一次配置
	{
		TIM2->CCR2 = BACK_LIGHT;
		SysParaSetInfo.AutoStandbyStatus = AUTOSTANDBY_STATUS;
		SysParaSetInfo.AutoStandbyTime = AUTOSTANDBY_TIME;
		SysParaSetInfo.InterfaceSwitch = INTERFACE_SWITCCH;
		SysParaSetInfo.Power_5V_Status = POWER_5V_STATUS;
		SysParaSetInfo.SDRemTimeUnit = SD_REM_TIME_UNIT;
		SysParaSetInfo.SensorSamplingTimeUnit = SENSOR_SAMPLING_UNIT;
		SysParaSetInfo.SensorSwitch = SENSOR_POWER_SWITCH;
		SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus = SENSOR_ALARM_STATUS;
		SysParaSetInfo.ThresHoldStatus.ThrldVal[PM2_5_ID] = THRLD_VAL;
		SysParaSetInfo.ThresHoldStatus.ThrldVal[HCHO_ID] = THRLD_VAL;
		SysParaSetInfo.ThresHoldStatus.ThrldVal[CARBON_ID] = THRLD_VAL;
		SysParaSetInfo.ThresHoldStatus.ThrldVal[TEMP_ID] = THRLD_VAL;
		SysParaSetInfo.ThresHoldStatus.ThrldVal[HUMI_ID] = THRLD_VAL;
		SysParaSetInfo.SpeechRecognize.Speech_Sw = SPEECH_SW;
		SysParaSetInfo.SpeechRecognize.Speech_Volume = SPEECH_VOLUME;
	}
	if(SysParaSetInfo.Power_5V_Status)
		Handle_5V_POWER_Status(1);  		//打开5V传感器电源
	else
		Handle_5V_POWER_Status(0);
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14) == 0)
	{
		LoadParaToEeprom();
		while(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14));
		Encoder_Type = ENCODER_VOID;
	}
	Rx8025tGetTime(&SysTimeData);  			//获取系统时间
	if(SysParaSetInfo.SpeechRecognize.Speech_Sw != 1)
	{
		DisableLD3320NVIC();
	}
	res = f_opendir(&dir,"0:/SensorMemory");
	if(res == FR_OK)
		return;
	else
		f_mkdir("0:/SensorMemory");  		//创建目录
}
//
//加载一个色谱
//
void LoadColorPat(void)
{ 
	uint16_t i=0;
	//uint16_t color;
	//uint16_t Ri=0,Gi=0,Bi=0;
	uint16_t ystep = (lcddev.height-53)/6;  				//改变色谱的长度改这里
	Show_Str(CURSOR-21,270,18,16,(uint8_t*)QualityGrade[0],16,0,GREEN,FillColor);  //标度
	LCD_Fill(CURSOR+5,lcddev.height-30,CURSOR+19,lcddev.height-30-ystep,GREEN);

	Show_Str(CURSOR-21,300-30-ystep,18,16,(uint8_t*)QualityGrade[1],16,0,YELLOW,FillColor);
	LCD_Fill(CURSOR+5,lcddev.height-30-ystep,CURSOR+19,lcddev.height-30-ystep*2,YELLOW);

	Show_Str(CURSOR-21,300-30-ystep*2,18,16,(uint8_t*)QualityGrade[2],16,0,ORANGE,FillColor);
	LCD_Fill(CURSOR+5,lcddev.height-30-ystep*2,CURSOR+19,lcddev.height-30-ystep*3,ORANGE);

	Show_Str(CURSOR-21,300-30-ystep*3,18,16,(uint8_t*)QualityGrade[3],16,0,RED,FillColor);
	LCD_Fill(CURSOR+5,lcddev.height-30-ystep*3,CURSOR+19,lcddev.height-30-ystep*4,RED);

	Show_Str(CURSOR-21,300-30-ystep*4,18,16,(uint8_t*)QualityGrade[4],16,0,PURPLE,FillColor);
	LCD_Fill(CURSOR+5,lcddev.height-30-ystep*4,CURSOR+19,lcddev.height-30-ystep*5,PURPLE);

	Show_Str(CURSOR-21,300-30-ystep*5,18,16,(uint8_t*)QualityGrade[5],16,0,MAROON,FillColor);
	LCD_Fill(CURSOR+5,lcddev.height-30-ystep*5,CURSOR+19,lcddev.height-30-ystep*6,MAROON);
}
//绘制动态的红色指示点
void DrawDynamicCir(uint16_t AQIValue,uint16_t color)
{
	uint8_t static PreStage = 6;
	uint16_t ystep = (lcddev.height-53)/6;
	if(AQIValue<=50 && AQIValue >=0 && PreStage!=1)  //等级1
	{
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep*(PreStage-1),CURSOR+2,lcddev.height-30-ystep*PreStage+1,FillColor);
		LCD_Fill(CURSOR-3,lcddev.height-30,CURSOR+2,lcddev.height-30-ystep+1,color);
		PreStage = 1;
	}
	else if(AQIValue<=100 && AQIValue>50 && PreStage!=2)//等级2
	{
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep*(PreStage-1),CURSOR+2,lcddev.height-30-ystep*PreStage+1,FillColor);
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep,CURSOR+2,lcddev.height-30-ystep*2+1,color);
		PreStage = 2;
	}
	else if(AQIValue<=150 && AQIValue>100 && PreStage!=3)//等级3
	{
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep*(PreStage-1),CURSOR+2,lcddev.height-30-ystep*PreStage+1,FillColor);
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep*2,CURSOR+2,lcddev.height-30-ystep*3+1,color);
		PreStage = 3;
	}
	else if(AQIValue<=200 && AQIValue>150 && PreStage!=4)//等级4
	{
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep*(PreStage-1),CURSOR+2,lcddev.height-30-ystep*PreStage+1,FillColor);
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep*3,CURSOR+2,lcddev.height-30-ystep*4+1,color);
		PreStage = 4;
	}
	else if(AQIValue<=300 && AQIValue>200 && PreStage!=5)//等级5
	{
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep*(PreStage-1),CURSOR+2,lcddev.height-30-ystep*PreStage+1,FillColor);
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep*4,CURSOR+2,lcddev.height-30-ystep*5+1,color);
		PreStage = 5;
	}
	else if(AQIValue>300 && PreStage!=6)//等级6
	{
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep*(PreStage-1),CURSOR+2,lcddev.height-30-ystep*PreStage+1,FillColor);
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep*5,CURSOR+2,lcddev.height-30-ystep*6+1,color);
		PreStage = 6;
	}
}
//计算颜色
uint16_t CalColorValue(uint16_t Value,uint8_t IdItem)  //QualityGradeColor[]
{
	//uint16_t R,G,B;
	uint16_t RGB = 0x0000;
	switch(IdItem)
	{
		case HCHO_ID:  //甲醛
		{
			if(SysParaSetInfo.Power_5V_Status && SensorOnline.Device.WZ_S==1)
			{
				if(Value<=8)
					RGB = QualityGradeColor[0];
				else if(Value>8 && Value<=10)
					RGB = QualityGradeColor[1];
				else if((Value>10 && Value<=12))
					RGB = QualityGradeColor[2];
				else if((Value>12 && Value<=13))
					RGB = QualityGradeColor[3];
				else if((Value>13 && Value<=14))
					RGB = QualityGradeColor[4];
				else if(Value>14)
					RGB = QualityGradeColor[5];
			}
			else
				RGB = GRAY;
		}break;
		case PM2_5_ID:  //PM2.5
		{
			if(SysParaSetInfo.Power_5V_Status && SensorOnline.Device.PMS_5==1)
			{
				if(Value<=50)
					RGB = QualityGradeColor[0];
				else if(Value>50 && Value<=100)
					RGB = QualityGradeColor[1];
				else if(Value>100 && Value<=150)
					RGB = QualityGradeColor[2];
				else if(Value>150 && Value<=200)
					RGB = QualityGradeColor[3];
				else if(Value>200 && Value<=300)
					RGB = QualityGradeColor[4];
				else if(Value>300)
					RGB = QualityGradeColor[5];
			}
			else
				RGB = GRAY;
		}break;
		case CARBON_ID:  //CO2
		{
			if(SysParaSetInfo.Power_5V_Status && SensorOnline.Device.SENSAIR==1)
			{
				if(Value<=600)
					RGB = QualityGradeColor[0];
				else if(Value>600 && Value<=800)
					RGB = QualityGradeColor[1];
				else if(Value>800 && Value<=1000)
					RGB = QualityGradeColor[2];
				else if(Value>1000 && Value<=1200)
					RGB = QualityGradeColor[3];
				else if(Value>1200 && Value<=1500)
					RGB = QualityGradeColor[4];
				else if(Value>1500)
					RGB = QualityGradeColor[5];
			}
			else
				RGB = GRAY;
		}break;
	}
	/*R=(uint16_t) Value*240/R_init;if(R>240)R=240;
	G=(uint16_t) Value*240/G_init;if(G>240)G=240;
	B=(uint16_t) Value*240/B_init;if(B>240)B=240;		
	RGB |=(unsigned long) R*1000/8225<<11;			   
	RGB |=(unsigned long) G*1000/4047<<5;	  //转换成屏幕的4W色显示
	RGB |=(unsigned long) B*1000/8225;*/
	return RGB;
}
//计算AQI的值
void Cal_USASt_Value(uint16_t Cvalue,uint16_t *AQI_Value,uint16_t *USAST_Value)
{
	uint16_t AQIValue;
	float Ihigh,Ilow,Chigh,Clow;
	//AQIValue = Cvalue/1000;  计算美标
	if(Cvalue>USASt_Tab[0][0]&&Cvalue<USASt_Tab[0][1]){
		Clow = USASt_Tab[0][0];Chigh = USASt_Tab[0][1];Ilow =  USASt_Tab[0][2];Ihigh = USASt_Tab[0][3];}
	else if(Cvalue>USASt_Tab[1][0]&&Cvalue<USASt_Tab[1][1]){
		Clow = USASt_Tab[1][0];Chigh = USASt_Tab[1][1];Ilow =  USASt_Tab[1][2];Ihigh = USASt_Tab[1][3];}
	else if(Cvalue>USASt_Tab[2][0]&&Cvalue<USASt_Tab[2][1]){
		Clow = USASt_Tab[2][0];Chigh = USASt_Tab[2][1];Ilow =  USASt_Tab[2][2];Ihigh = USASt_Tab[2][3];}
	else if(Cvalue>USASt_Tab[3][0]&&Cvalue<USASt_Tab[3][1]){
		Clow = USASt_Tab[3][0];Chigh = USASt_Tab[3][1];Ilow =  USASt_Tab[3][2];Ihigh = USASt_Tab[3][3];}		
	else if(Cvalue>USASt_Tab[4][0]&&Cvalue<USASt_Tab[4][1]){
		Clow = USASt_Tab[4][0];Chigh = USASt_Tab[4][1];Ilow =  USASt_Tab[4][2];Ihigh = USASt_Tab[4][3];}			
	else if(Cvalue>USASt_Tab[5][0]&&Cvalue<USASt_Tab[5][1]){
		Clow = USASt_Tab[5][0];Chigh = USASt_Tab[5][1];Ilow =  USASt_Tab[5][2];Ihigh = USASt_Tab[5][3];}		
	else if(Cvalue>USASt_Tab[6][0]&&Cvalue<USASt_Tab[6][1]){
		Clow = USASt_Tab[6][0];Chigh = USASt_Tab[6][1];Ilow =  USASt_Tab[6][2];Ihigh = USASt_Tab[6][3];}	
	*USAST_Value = abs((uint16_t)((Ihigh - Ilow)*(Cvalue - Clow)/(Chigh - Clow) + Ilow));
		if(*USAST_Value>=500)
			*USAST_Value = 500;
	//计算AQI
	if(Cvalue>USASt_Tab[0][0]&&Cvalue<USASt_Tab[0][1]){
		Clow = USASt_Tab[0][0];Chigh = USASt_Tab[0][1];Ilow =  USASt_Tab[0][2];Ihigh = USASt_Tab[0][3];}
	else if(Cvalue>USASt_Tab[1][0]&&Cvalue<USASt_Tab[1][1]+19.6){
		Clow = USASt_Tab[1][0];Chigh = USASt_Tab[1][1]+19.6;Ilow =  USASt_Tab[1][2];Ihigh = USASt_Tab[1][3];}
	else if(Cvalue>USASt_Tab[2][0]&&Cvalue<USASt_Tab[2][1]+34.6){
		Clow = USASt_Tab[2][0];Chigh = USASt_Tab[2][1]+34.6;Ilow =  USASt_Tab[2][2];Ihigh = USASt_Tab[2][3];}
	else if(Cvalue>USASt_Tab[3][0]&&Cvalue<USASt_Tab[3][1]+49.6){
		Clow = USASt_Tab[3][0];Chigh = USASt_Tab[3][1]+49.6;Ilow =  USASt_Tab[3][2];Ihigh = USASt_Tab[3][3];}		
	else if(Cvalue>USASt_Tab[4][0]&&Cvalue<USASt_Tab[4][1]+0.4){
		Clow = USASt_Tab[4][0];Chigh = USASt_Tab[4][1]+0.4;Ilow =  USASt_Tab[4][2];Ihigh = USASt_Tab[4][3];}			
	else if(Cvalue>USASt_Tab[5][0]&&Cvalue<USASt_Tab[5][1]+0.4){
		Clow = USASt_Tab[5][0];Chigh = USASt_Tab[5][1]+0.4;Ilow =  USASt_Tab[5][2];Ihigh = USASt_Tab[5][3];}		
	else if(Cvalue>USASt_Tab[6][0]&&Cvalue<USASt_Tab[6][1]+0.4){
		Clow = USASt_Tab[6][0];Chigh = USASt_Tab[6][1]+0.4;Ilow =  USASt_Tab[6][2];Ihigh = USASt_Tab[6][3];}
	*AQI_Value = abs((uint16_t)((Ihigh - Ilow)*(Cvalue - Clow)/(Chigh - Clow) + Ilow));
		if(*AQI_Value>=500)
			*AQI_Value = 500;
}
extern uint8_t nAsrStatus;
//报警状态设置
//有优先级处理
void AlarmStatusCheck(void)
{
	if(SysParaSetInfo.Power_5V_Status)
	{
		if(SensorData.HCHO>SysParaSetInfo.ThresHoldStatus.ThrldVal[HCHO_ID] && (SysParaSetInfo.SensorSwitch&0x02) == 0x02)
		{
			if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X08) == 0X08)
			{
				AlarmCloseFlag = 1;
				//SetBeepFreq(AlarmFreqTab[9],1);  //这里替换成对应的播放MP3部分就可以了
			}
		}
		else if(SensorData.PMData.PM2_5_S>SysParaSetInfo.ThresHoldStatus.ThrldVal[PM2_5_ID] && (SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X10) == 0X10)
		{
			AlarmCloseFlag = 2;
			//SetBeepFreq(AlarmFreqTab[7],1);
		}
		else if(SensorData.Carbon>SysParaSetInfo.ThresHoldStatus.ThrldVal[CARBON_ID] && (SysParaSetInfo.SensorSwitch&0x01)==0x01)
		{
			if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X04) == 0X04)
			{
				AlarmCloseFlag = 3;
				//SetBeepFreq(AlarmFreqTab[5],1);
			}
		}
	}
	else if(SensorData.Temp>SysParaSetInfo.ThresHoldStatus.ThrldVal[TEMP_ID] && (SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X02)==0X02)
	{
		AlarmCloseFlag = 4;
		//SetBeepFreq(AlarmFreqTab[3],1);
	}
	else if(SensorData.Humi<SysParaSetInfo.ThresHoldStatus.ThrldVal[HUMI_ID] && (SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X01)==0X01)
	{
		AlarmCloseFlag = 5;
		//SetBeepFreq(AlarmFreqTab[1],1);
	}
}
//质量检测判断，在三个界面上都有显示 AQI取值范围(0-500)
void EnvirQuaty(uint16_t x,uint16_t y)
{
	//uint16_t RGB;
	uint16_t USASt_Value;   //美标值
    Cal_USASt_Value(SensorData.PMData.PM2_5_S,&AQI_Value,&USASt_Value);
	Show_Str(80,3,300,24,"AQI:",24,0,AQI_COLOR,TOP_TITLE_BACK_COLOR);
	Show_Str(165,3,300,24,"美标:",24,0,AQI_COLOR,TOP_TITLE_BACK_COLOR);
    LCD_ShowNum(128,3,AQI_Value,3,24,0,AQI_COLOR,TOP_TITLE_BACK_COLOR);
	LCD_ShowNum(223,3,USASt_Value,3,24,0,AQI_COLOR,TOP_TITLE_BACK_COLOR);
	if(SysParaSetInfo.Power_5V_Status)
		DrawDynamicCir(AQI_Value,WHITE);
	else
		DrawDynamicCir(AQI_Value,FillColor);
	AlarmStatusCheck();
	if(Current_Show_Interface == Curr_Para_Show)  //只有在主显示界面下才显示
	{
		if(SysParaSetInfo.Power_5V_Status)
		{
			//RGB = CalColorValue(SensorData.PMData.PM2_5_S,PM2_5_ID);
			if(SensorOnline.Device.PMS_5 == 1)
			{
				if(AQI_Value<=50 && AQI_Value>=0)  //等级1
				{
					Show_Str(AllDispCirInfo[PM2_5_ID].x0+20,y,200,16,AirQuiyGrade[PM2_5_ID][0],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X02)!=0X02)
						Show_Str(AllDispCirInfo[HCHO_ID].x0+10,y,200,16,AirQuiyGrade[3][0],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X01)!=0X01)
						Show_Str(AllDispCirInfo[CARBON_ID].x0+10,y,200,16,AirQuiyGrade[4][0],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
				}
				else if(AQI_Value>50 && AQI_Value<=100)//等级2
				{
					Show_Str(AllDispCirInfo[PM2_5_ID].x0+20,y,200,16,AirQuiyGrade[PM2_5_ID][1],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X02)!=0X02)
						Show_Str(AllDispCirInfo[HCHO_ID].x0+10,y,200,16,AirQuiyGrade[3][1],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X01)!=0X01)
						Show_Str(AllDispCirInfo[CARBON_ID].x0+10,y,200,16,AirQuiyGrade[4][1],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
				}
				else if(AQI_Value>=100 && AQI_Value<=150)//等级3
				{
					Show_Str(AllDispCirInfo[PM2_5_ID].x0+20,y,200,16,AirQuiyGrade[PM2_5_ID][2],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X02)!=0X02)
						Show_Str(AllDispCirInfo[HCHO_ID].x0+10,y,200,16,AirQuiyGrade[3][2],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X01)!=0X01)
						Show_Str(AllDispCirInfo[CARBON_ID].x0+10,y,200,16,AirQuiyGrade[4][2],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
				}
				else if(AQI_Value>=150 && AQI_Value<=200)//等级4
				{
					Show_Str(AllDispCirInfo[PM2_5_ID].x0+20,y,200,16,AirQuiyGrade[PM2_5_ID][3],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X02)!=0X02)
						Show_Str(AllDispCirInfo[HCHO_ID].x0+10,y,200,16,AirQuiyGrade[3][3],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X01)!=0X01)
						Show_Str(AllDispCirInfo[CARBON_ID].x0+10,y,200,16,AirQuiyGrade[4][3],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
				}
				else if(AQI_Value>=200 && AQI_Value<=300)//等级5
				{
					Show_Str(AllDispCirInfo[PM2_5_ID].x0+20,y,200,16,AirQuiyGrade[PM2_5_ID][4],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X02)!=0X02)
						Show_Str(AllDispCirInfo[HCHO_ID].x0+10,y,200,16,AirQuiyGrade[3][4],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X01)!=0X01)
						Show_Str(AllDispCirInfo[CARBON_ID].x0+10,y,200,16,AirQuiyGrade[4][4],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
				}
				else if(AQI_Value>300)//等级6
				{
					Show_Str(AllDispCirInfo[PM2_5_ID].x0+20,y,200,16,AirQuiyGrade[PM2_5_ID][5],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X02)!=0X02)
						Show_Str(AllDispCirInfo[HCHO_ID].x0+10,y,200,16,AirQuiyGrade[3][5],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X01)!=0X01)
						Show_Str(AllDispCirInfo[CARBON_ID].x0+10,y,200,16,AirQuiyGrade[4][5],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
				}
				if((SysParaSetInfo.SensorSwitch&0X02) == 0X02)
				{
					if(SensorOnline.Device.WZ_S == 1)
					{
						if(SensorData.HCHO<8)  //甲醛和CO2判断
							Show_Str(AllDispCirInfo[HCHO_ID].x0+20,y,200,16,AirQuiyGrade[HCHO_ID][0],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
						else if(SensorData.HCHO>8&&SensorData.HCHO<10)
							Show_Str(AllDispCirInfo[HCHO_ID].x0+20,y,200,16,AirQuiyGrade[HCHO_ID][1],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
						else
							Show_Str(AllDispCirInfo[HCHO_ID].x0+20,y,200,16,AirQuiyGrade[HCHO_ID][2],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					}
					else
						Show_Str(AllDispCirInfo[HCHO_ID].x0+20,y,200,16,AirQuiyGrade[7][0],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
				}
			}
			if(SensorOnline.Device.SENSAIR == 1)
			{
				if((SysParaSetInfo.SensorSwitch&0X01) == 0X01)
				{
					if(SensorOnline.Device.SENSAIR == 1)
					{
						if(SensorData.Carbon<1000)
						{
							if(SensorData.Carbon==0)
								Show_Str(AllDispCirInfo[CARBON_ID].x0+20,y,200,16,AirQuiyGrade[CARBON_ID][0],16,0,GRAY,PARA_SHOW_INTERFACE_BACKCOLOR);
							else
								Show_Str(AllDispCirInfo[CARBON_ID].x0+20,y,200,16,AirQuiyGrade[CARBON_ID][1],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
						}
						else if(SensorData.Carbon>=1000&&SensorData.Carbon<1500)
							Show_Str(AllDispCirInfo[CARBON_ID].x0+20,y,200,16,AirQuiyGrade[CARBON_ID][2],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
						else 
							Show_Str(AllDispCirInfo[CARBON_ID].x0+20,y,200,16,AirQuiyGrade[CARBON_ID][3],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					}
					else
						Show_Str(AllDispCirInfo[CARBON_ID].x0+20,y,200,16,AirQuiyGrade[7][0],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
				}
			}
		}
		else
		{
			Show_Str(AllDispCirInfo[PM2_5_ID].x0+28,y,200,16,AirQuiyGrade[7][0],16,0,GRAY,PARA_SHOW_INTERFACE_BACKCOLOR);
			Show_Str(AllDispCirInfo[HCHO_ID].x0+28,y,200,16,AirQuiyGrade[7][0],16,0,GRAY,PARA_SHOW_INTERFACE_BACKCOLOR);
			Show_Str(AllDispCirInfo[CARBON_ID].x0+28,y,200,16,AirQuiyGrade[7][0],16,0,GRAY,PARA_SHOW_INTERFACE_BACKCOLOR);
		}
		if(SensorOnline.Device.SHT30 == 1)
		{
			if(SensorData.Temp < SysParaSetInfo.ThresHoldStatus.ThrldVal[TEMP_ID])
				Show_Str(230, 300,150,16,AirQuiyGrade[5][0],16,0,TEMPHUMI_ALARM_COLOR,BOTTOM_TITTLE_BACK_COLOR);
			else if(SensorData.Temp > SysParaSetInfo.ThresHoldStatus.ThrldVal[TEMP_ID]+5)  //如果再增加5℃的话
				Show_Str(230,300,150,16,AirQuiyGrade[5][2],16,0,TEMPHUMI_ALARM_COLOR,BOTTOM_TITTLE_BACK_COLOR);
			else if(SensorData.Temp > SysParaSetInfo.ThresHoldStatus.ThrldVal[TEMP_ID])
				Show_Str(230,300,150,16,AirQuiyGrade[5][1],16,0,TEMPHUMI_ALARM_COLOR,BOTTOM_TITTLE_BACK_COLOR);
		
			if(SensorData.Humi > SysParaSetInfo.ThresHoldStatus.ThrldVal[HUMI_ID])
				Show_Str(302,300,150,16,AirQuiyGrade[6][0],16,0,TEMPHUMI_ALARM_COLOR,BOTTOM_TITTLE_BACK_COLOR);
			else if(SensorData.Humi < SysParaSetInfo.ThresHoldStatus.ThrldVal[HUMI_ID])
				Show_Str(302,300,150,16,AirQuiyGrade[6][1],16,0,TEMPHUMI_ALARM_COLOR,BOTTOM_TITTLE_BACK_COLOR);
			else if(SensorData.Humi < SysParaSetInfo.ThresHoldStatus.ThrldVal[HUMI_ID]-5)
				Show_Str(302,300,150,16,AirQuiyGrade[6][2],16,0,TEMPHUMI_ALARM_COLOR,BOTTOM_TITTLE_BACK_COLOR);
		}
	}
}
//填充白色方框
void FillThemeBackColor(DispCirDef const *DispCirInfo, uint8_t Item,uint16_t Color)
{
	LCD_Fill(DispCirInfo->x0,DispCirInfo->y0,DispCirInfo->x0+DispCirInfo->width,DispCirInfo->y0+DispCirInfo->height,Color);
	LCD_Fill(DispCirInfo->x0,DispCirInfo->y0,DispCirInfo->x0+DispCirInfo->width,DispCirInfo->y0-15,Color);
	LCD_Fill(DispCirInfo->x0,DispCirInfo->y0,DispCirInfo->x0-15,DispCirInfo->y0+DispCirInfo->height,Color);
	LCD_Fill(DispCirInfo->x0,DispCirInfo->y0+DispCirInfo->width,DispCirInfo->x0+DispCirInfo->width,DispCirInfo->y0+DispCirInfo->height+15,Color);
	LCD_Fill(DispCirInfo->x0+DispCirInfo->width,DispCirInfo->y0,DispCirInfo->x0+DispCirInfo->width+15,DispCirInfo->y0+DispCirInfo->height,Color);
	LCDDrawCir(DispCirInfo->x0,DispCirInfo->y0,15,Color,4,1);
	LCDDrawCir(DispCirInfo->x0,DispCirInfo->y0+DispCirInfo->height,15,Color,3,1);
	LCDDrawCir(DispCirInfo->x0+DispCirInfo->width,DispCirInfo->y0,15,Color,1,1);
	LCDDrawCir(DispCirInfo->x0+DispCirInfo->width,DispCirInfo->y0+DispCirInfo->height,15,Color,2,1);
	if(Item == HCHO_ID)
	{
		if((SysParaSetInfo.SensorSwitch&0x02) == 0x02)
		{
			Show_Str(DispCirInfo->x0+DispCirInfo->Xoff,DispCirInfo->y0-8,100,24,DispCirInfo->Text_Name,24,0,DispCirInfo->ColorInfo.Textcolor,Color);
			Show_Str(DispCirInfo->x0+DispCirInfo->Xoff,DispCirInfo->y0+DispCirInfo->Yoff,100,24,DispCirInfo->Text_UnitMeas,24,0,DispCirInfo->ColorInfo.Textcolor,Color);
		}
		else if((SysParaSetInfo.SensorSwitch&0x02) != 0x02)
		{
			Show_Str(DispCirInfo->x0+DispCirInfo->Xoff,DispCirInfo->y0-8,100,24,"PM1.0",24,0,DispCirInfo->ColorInfo.Textcolor,Color);
			Show_Str(DispCirInfo->x0+DispCirInfo->Xoff,DispCirInfo->y0+DispCirInfo->Yoff,100,24,"ug/m3",24,0,DispCirInfo->ColorInfo.Textcolor,Color);
		}
	}
	if(Item == CARBON_ID)
	{
		if((SysParaSetInfo.SensorSwitch&0x01) == 0x01)
		{
			Show_Str(DispCirInfo->x0+DispCirInfo->Xoff,DispCirInfo->y0-8,100,24,DispCirInfo->Text_Name,24,0,DispCirInfo->ColorInfo.Textcolor,Color);
			Show_Str(DispCirInfo->x0+DispCirInfo->Xoff,DispCirInfo->y0+DispCirInfo->Yoff,100,24,DispCirInfo->Text_UnitMeas,24,0,DispCirInfo->ColorInfo.Textcolor,Color);
		}
		else if((SysParaSetInfo.SensorSwitch&0x01) != 0x01)
		{
			Show_Str(DispCirInfo->x0+DispCirInfo->Xoff,DispCirInfo->y0-8,100,24,"PM10",24,0,DispCirInfo->ColorInfo.Textcolor,Color);
			Show_Str(DispCirInfo->x0+DispCirInfo->Xoff,DispCirInfo->y0+DispCirInfo->Yoff,100,24,"ug/m3",24,0,DispCirInfo->ColorInfo.Textcolor,Color);
		}
	}
	if(Item == PM2_5_ID)
	{
		Show_Str(DispCirInfo->x0+DispCirInfo->Xoff,DispCirInfo->y0-8,100,24,DispCirInfo->Text_Name,24,0,DispCirInfo->ColorInfo.Textcolor,Color);
		Show_Str(DispCirInfo->x0+DispCirInfo->Xoff,DispCirInfo->y0+DispCirInfo->Yoff,100,24,DispCirInfo->Text_UnitMeas,24,0,DispCirInfo->ColorInfo.Textcolor,Color);
	}
}
void Load_ShowUI_1(DispCirDef const *DispCirInfo,uint8_t Item)
{	
	switch(Item)
	{
		case TEMP_ID:
		{
			if(SensorOnline.Device.SHT30 == 1)
			{
				Show_DrawBMP(DispCirInfo->x0-60,DispCirInfo->y0+7,40,40,(uint8_t*)TempLogo,DispCirInfo->ColorInfo.Textcolor,PARA_SHOW_INTERFACE_BACKCOLOR);
				Show_Str(DispCirInfo->x0+55,DispCirInfo->y0-5,150,24,DispCirInfo->Text_UnitMeas,24,0,DispCirInfo->ColorInfo.Textcolor,DispCirInfo->ColorInfo.TextBackcolor);
				Show_Str(DispCirInfo->x0-DispCirInfo->Xoff,DispCirInfo->y0+DispCirInfo->Yoff,150,24,DispCirInfo->Text_Name,24,0,DispCirInfo->ColorInfo.Textcolor,DispCirInfo->ColorInfo.TextBackcolor);
			}
		}break;
		case HUMI_ID:
		{
			if(SensorOnline.Device.SHT30 == 1)
			{
				Show_DrawBMP(DispCirInfo->x0-70,DispCirInfo->y0+10,40,40,(uint8_t*)HumiLogo,DispCirInfo->ColorInfo.Textcolor,PARA_SHOW_INTERFACE_BACKCOLOR);
				Show_Str(DispCirInfo->x0+20,DispCirInfo->y0-5,150,24,DispCirInfo->Text_UnitMeas,24,0,DispCirInfo->ColorInfo.Textcolor,DispCirInfo->ColorInfo.TextBackcolor);
				Show_Str(DispCirInfo->x0-DispCirInfo->Xoff,DispCirInfo->y0+DispCirInfo->Yoff,150,24,DispCirInfo->Text_Name,24,0,DispCirInfo->ColorInfo.Textcolor,DispCirInfo->ColorInfo.TextBackcolor);
			}
		}break;
		case AIRPRES_ID:
//			LCDDrawCir(DispCirInfo->x0,DispCirInfo->y0,DispCirInfo->r1,DispCirInfo->ColorInfo.Cir1color,0,1);  	 //外圆
//			LCDDrawCir(DispCirInfo->x0,DispCirInfo->y0,DispCirInfo->r2,DispCirInfo->ColorInfo.Cir2color,0,1);      //内圆
			if(SensorOnline.Device.BMP180 == 1)
			{
				Show_DrawBMP(DispCirInfo->x0-65,DispCirInfo->y0+10,40,40,(uint8_t*)AirPressLogo,DispCirInfo->ColorInfo.Textcolor,PARA_SHOW_INTERFACE_BACKCOLOR);
				Show_Str(DispCirInfo->x0+55,DispCirInfo->y0-10,150,16,DispCirInfo->Text_UnitMeas,16,0,DispCirInfo->ColorInfo.Textcolor,DispCirInfo->ColorInfo.TextBackcolor);
				Show_Str(DispCirInfo->x0+55,DispCirInfo->y0+15,150,16,"米",16,0,DispCirInfo->ColorInfo.Textcolor,DispCirInfo->ColorInfo.TextBackcolor);
				Show_Str(DispCirInfo->x0-DispCirInfo->Xoff+15,DispCirInfo->y0+DispCirInfo->Yoff+13,150,16,DispCirInfo->Text_Name,16,0,DispCirInfo->ColorInfo.Textcolor,DispCirInfo->ColorInfo.TextBackcolor);
			
			}break;
		case HCHO_ID:
		case CARBON_ID:
		case PM2_5_ID:
			FillThemeBackColor(DispCirInfo,Item,CurrThemeColorFlag[Item]);
		break;
	}
}

uint8_t Get_Num_Bit(uint16_t Num)
{
	uint8_t Numbit = 0;
	if(Num == 0)
		return 0;
	if((Num/1000)!=0)
	{
		Numbit = 4;
		return Numbit;
	}
	else if((Num/100%10)!=0)
	{
		Numbit = 3;
		return Numbit;
	}
	else if((Num/10%10)!=0)
	{
		Numbit = 2;
		return Numbit;
	}
	else
		return 1;
}
//显示图形内的传感器数据内容
void DispDataInfo(DispCirDef const *DispCirInfo,uint8_t Item)
{
	char buf[20];			
	uint8_t Offset = 0;
	uint16_t RGB;
	uint8_t NumBit = 0;
	static uint8_t PreBitnum1 = 0;  //PM2.5
	static uint8_t PreBitnum2 = 0;  //CO2/PM1.0
	static uint8_t PreBitnum3 = 0;  //PM10
	switch(Item)
	{
		case TEMP_ID:
		{
//			if(SensorOnline.Device.SHT30 == 1)
			{
				sprintf(buf,"%d.%d",(int16_t)(SensorData.Temp*10)/10,(uint16_t)(SensorData.Temp*10)%10);
				if(SensorData.Temp < SysParaSetInfo.ThresHoldStatus.ThrldVal[TEMP_ID])
					Show_Str(DispCirInfo->x0-20,DispCirInfo->y0-17,150,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,QualityGradeColor[0]/*DispCirInfo->ColorInfo.Valuecolor*/,DispCirInfo->ColorInfo.ValueBackcolor);
				else
					Show_Str(DispCirInfo->x0-20,DispCirInfo->y0-17,150,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,QualityGradeColor[3]/*DispCirInfo->ColorInfo.Valuecolor*/,DispCirInfo->ColorInfo.ValueBackcolor);
			}
		}break;
		case HUMI_ID:
		{
			if(SensorOnline.Device.SHT30 == 1)
			{
				sprintf(buf,"%d",SensorData.Humi);
				if(SensorData.Humi > SysParaSetInfo.ThresHoldStatus.ThrldVal[HUMI_ID])
					Show_Str(DispCirInfo->x0-20,DispCirInfo->y0-17,150,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,QualityGradeColor[0]/*DispCirInfo->ColorInfo.Valuecolor*/,DispCirInfo->ColorInfo.ValueBackcolor);
				else
					Show_Str(DispCirInfo->x0-20,DispCirInfo->y0-17,150,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,QualityGradeColor[3]/*DispCirInfo->ColorInfo.Valuecolor*/,DispCirInfo->ColorInfo.ValueBackcolor);
			}
		}break;
		case AIRPRES_ID:
		{ 
			if(SensorOnline.Device.BMP180 == 1)
			{
				sprintf(buf,"%d.%d",SensorData.AirPres/1000,SensorData.AirPres/100%10);
				Show_Str(DispCirInfo->x0-10,DispCirInfo->y0-15,150,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,DispCirInfo->ColorInfo.Valuecolor,DispCirInfo->ColorInfo.ValueBackcolor);
				LCD_ShowNum(DispCirInfo->x0-20,DispCirInfo->y0+DispCirInfo->Yoff-13,SensorData.Altitude,5,24,1,DispCirInfo->ColorInfo.Valuecolor,DispCirInfo->ColorInfo.ValueBackcolor);
			}
		}break;
		case PM2_5_ID:
		{			
			
			RGB = CalColorValue(AQI_Value,PM2_5_ID);
			CurrThemeColorFlag[0] = RGB;
			if(CurrThemeColorFlag[0] != PreThemeColorFlag[0])
			{
				FillThemeBackColor(&AllDispCirInfo[Item_Id[1]],Item_Id[1],CurrThemeColorFlag[0]);
				PreThemeColorFlag[0] = CurrThemeColorFlag[0];
			}
			NumBit = Get_Num_Bit(SensorData.PMData.PM2_5_S);
			if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_14)==0 && SensorOnline.Device.PMS_5==1)  //如果传感器电源被使能的话执行
			{
				switch(NumBit)
				{
					case 1:
					{
						if(PreBitnum1 != 1)
							LCD_Fill(DispCirInfo->x0-13,DispCirInfo->y0+35,DispCirInfo->x0+113,DispCirInfo->y0+88,RGB);
						PreBitnum1 = 1;
						sprintf(buf,"%d",SensorData.PMData.PM2_5_S);
						Show_Str(DispCirInfo->x0+46,DispCirInfo->y0+20,150,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,BLACK,RGB);
					}break;
					case 2:
					{
						if(PreBitnum1 != 2)
							LCD_Fill(DispCirInfo->x0-13,DispCirInfo->y0+35,DispCirInfo->x0+113,DispCirInfo->y0+88,RGB);
						PreBitnum1 = 2;
						sprintf(buf,"%d",SensorData.PMData.PM2_5_S);
						Show_Str(DispCirInfo->x0+28,DispCirInfo->y0+20,150,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,BLACK,RGB);
					}break;
					case 3:
					{
						if(PreBitnum1 != 3)
							LCD_Fill(DispCirInfo->x0-13,DispCirInfo->y0+35,DispCirInfo->x0+113,DispCirInfo->y0+88,RGB);
						PreBitnum1 = 3;
						sprintf(buf,"%d",SensorData.PMData.PM2_5_S);
						Show_Str(DispCirInfo->x0+13,DispCirInfo->y0+20,150,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,BLACK,RGB);
					}break;
					case 4:
					{
						if(PreBitnum1 != 4)
							LCD_Fill(DispCirInfo->x0-13,DispCirInfo->y0+35,DispCirInfo->x0+113,DispCirInfo->y0+88,RGB);  
						PreBitnum1 = 4;
						sprintf(buf,"%d",SensorData.PMData.PM2_5_S);
						Show_Str(DispCirInfo->x0-5,DispCirInfo->y0+20,150,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,BLACK,RGB);
					}break;
				}
			}
			else
				Show_Str(DispCirInfo->x0+17,DispCirInfo->y0+45,150,24,"无数据",24,0,BLACK,RGB);//未使能的话白底灰字
		}break;
		case HCHO_ID:
		{
			if((SysParaSetInfo.SensorSwitch&0x02) == 0X02)   //使能了甲醛传感器
			{
				RGB = CalColorValue(SensorData.HCHO,HCHO_ID);
				CurrThemeColorFlag[1] = RGB;
				if(CurrThemeColorFlag[1] != PreThemeColorFlag[1])
				{					
					FillThemeBackColor(&AllDispCirInfo[Item_Id[0]],Item_Id[0],CurrThemeColorFlag[1]);
					PreThemeColorFlag[1] = CurrThemeColorFlag[1];
				}
				if(SysParaSetInfo.Power_5V_Status && SensorOnline.Device.WZ_S==1)
				{
					sprintf(buf,"%d.%d%d",SensorData.HCHO/100,SensorData.HCHO/10%10,SensorData.HCHO%10);
					Show_Str(DispCirInfo->x0-8,DispCirInfo->y0+20,150,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,BLACK,RGB);
				}
				else
					Show_Str(DispCirInfo->x0+17,DispCirInfo->y0+45,150,24,"无数据",24,0,BLACK,RGB);  //未使能的话白底灰字
			}
			else if((SysParaSetInfo.SensorSwitch&0x02) != 0X02)  //未使能甲醛传感器
			{
				RGB = CalColorValue(SensorData.PMData.PM1_0_S,PM2_5_ID);  //用显示甲醛的位置代替显示PM1.0
				CurrThemeColorFlag[1] = RGB;
				if(CurrThemeColorFlag[1] != PreThemeColorFlag[1])
				{					
					FillThemeBackColor(&AllDispCirInfo[Item_Id[0]],Item_Id[0],CurrThemeColorFlag[1]);
					PreThemeColorFlag[1] = CurrThemeColorFlag[1];
				}
				NumBit = Get_Num_Bit(SensorData.PMData.PM1_0_S);
				if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_14)==0 && SensorOnline.Device.PMS_5==1)
				{
					switch(NumBit)
					{
						case 1:
						{
							if(PreBitnum3 != 1)
								LCD_Fill(DispCirInfo->x0-13,DispCirInfo->y0+35,DispCirInfo->x0+113,DispCirInfo->y0+88,RGB);
							PreBitnum3 = 1;
							sprintf(buf,"%01d",SensorData.PMData.PM1_0_S);
							Show_Str(DispCirInfo->x0+46,DispCirInfo->y0+20,150,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,BLACK,RGB);
						}break;
						case 2:
						{
							if(PreBitnum3 != 2)
								LCD_Fill(DispCirInfo->x0-13,DispCirInfo->y0+35,DispCirInfo->x0+113,DispCirInfo->y0+88,RGB);
							PreBitnum3 = 2;
							sprintf(buf,"%02d",SensorData.PMData.PM1_0_S);
							Show_Str(DispCirInfo->x0+28,DispCirInfo->y0+20,150,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,BLACK,RGB);
						}break;
						case 3:
						{
							if(PreBitnum3 != 3)
								LCD_Fill(DispCirInfo->x0-13,DispCirInfo->y0+35,DispCirInfo->x0+113,DispCirInfo->y0+88,RGB);
							PreBitnum3 = 3;
							sprintf(buf,"%03d",SensorData.PMData.PM1_0_S);
							Show_Str(DispCirInfo->x0+13,DispCirInfo->y0+20,150,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,BLACK,RGB);
						}break;
						case 4:
						{
							if(PreBitnum3 != 4)
								LCD_Fill(DispCirInfo->x0-13,DispCirInfo->y0+35,DispCirInfo->x0+113,DispCirInfo->y0+88,RGB);
							PreBitnum3 = 4;
							sprintf(buf,"%04d",SensorData.PMData.PM1_0_S);
							Show_Str(DispCirInfo->x0-5,DispCirInfo->y0+20,150,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,BLACK,RGB);
						}break;
						default:break;
					}
				}
				else
					Show_Str(DispCirInfo->x0+17,DispCirInfo->y0+45,150,24,"无数据",24,0,BLACK,RGB);  //未使能的话白底灰字
			}
		}break;
		case CARBON_ID:
		{
			if((SysParaSetInfo.SensorSwitch&0X01) == 0X01)  //显示CO2数据
			{
				RGB = CalColorValue(SensorData.Carbon,CARBON_ID);
				CurrThemeColorFlag[2] = RGB;
				if(CurrThemeColorFlag[2] != PreThemeColorFlag[2])
				{					
					FillThemeBackColor(&AllDispCirInfo[Item_Id[2]],Item_Id[2],CurrThemeColorFlag[2]);
					PreThemeColorFlag[2] = CurrThemeColorFlag[2];
				}
				NumBit = Get_Num_Bit(SensorData.Carbon);
			}
			else if((SysParaSetInfo.SensorSwitch&0X01) != 0X01)
			{
				RGB = CalColorValue(SensorData.PMData.PM10_S,PM2_5_ID);
				CurrThemeColorFlag[2] = RGB;
				if(CurrThemeColorFlag[2] != PreThemeColorFlag[2])
				{					
					FillThemeBackColor(&AllDispCirInfo[Item_Id[2]],Item_Id[2],CurrThemeColorFlag[2]);
					PreThemeColorFlag[2] = CurrThemeColorFlag[2];
				}
				NumBit = Get_Num_Bit(SensorData.PMData.PM10_S);
			}
			if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_14)==0)  //如果传感器电源被使能的话执行
			{
				switch(NumBit)
				{
					case 0:Show_Str(DispCirInfo->x0+17,DispCirInfo->y0+45,150,24,"无数据",24,0,BLACK,RGB);break;
					case 1:
					{
						if(PreBitnum2 != 1)
							LCD_Fill(DispCirInfo->x0-13,DispCirInfo->y0+35,DispCirInfo->x0+113,DispCirInfo->y0+88,RGB);
						PreBitnum2 = 1;
						if((SysParaSetInfo.SensorSwitch&0X01)==0X01 && SensorOnline.Device.SENSAIR==1)
						{
							if(SensorData.Carbon==0)
								Show_Str(DispCirInfo->x0+8,DispCirInfo->y0+46,150,DispCirInfo->FontSize," 准备中",24,0,GRAY,RGB);
							else if(SensorData.Carbon>=2000)
								Show_Str(DispCirInfo->x0+25,DispCirInfo->y0+46,150,DispCirInfo->FontSize," 爆表",24,0,BLACK,RGB);
							else
							{
								sprintf(buf,"%01d",SensorData.Carbon);
								Show_Str(DispCirInfo->x0+46,DispCirInfo->y0+20,150,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,BLACK,RGB);
							}
						}
						else if((SysParaSetInfo.SensorSwitch&0X01)!=0X01 && SensorOnline.Device.PMS_5==1)
						{
							sprintf(buf,"%01d",SensorData.PMData.PM10_S);
							Show_Str(DispCirInfo->x0+46,DispCirInfo->y0+20,150,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,BLACK,RGB);
						}
					}break;
					case 2:
					{
						if(PreBitnum2 != 2)
							LCD_Fill(DispCirInfo->x0-13,DispCirInfo->y0+35,DispCirInfo->x0+113,DispCirInfo->y0+88,RGB);
						PreBitnum2 = 2;
						if((SysParaSetInfo.SensorSwitch&0X01)==0X01  && SensorOnline.Device.SENSAIR==1)
							sprintf(buf,"%02d",SensorData.Carbon);
						else if((SysParaSetInfo.SensorSwitch&0X01)!=0X01 && SensorOnline.Device.PMS_5==1)
							sprintf(buf,"%02d",SensorData.PMData.PM10_S);
						Show_Str(DispCirInfo->x0+28,DispCirInfo->y0+20,150,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,BLACK,RGB);
					}break;
					case 3:
					{
						if(PreBitnum2 != 3)
							LCD_Fill(DispCirInfo->x0-13,DispCirInfo->y0+35,DispCirInfo->x0+113,DispCirInfo->y0+88,RGB);
						PreBitnum2 = 3;
						if((SysParaSetInfo.SensorSwitch&0X01)==0X01 && SensorOnline.Device.SENSAIR==1)
							sprintf(buf,"%03d",SensorData.Carbon);
						else if((SysParaSetInfo.SensorSwitch&0X01)!=0X01 && SensorOnline.Device.PMS_5==1)
							sprintf(buf,"%03d",SensorData.PMData.PM10_S);
						Show_Str(DispCirInfo->x0+13,DispCirInfo->y0+20,150,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,BLACK,RGB);
					}break;
					case 4:
					{
						if(PreBitnum2 != 4)
							LCD_Fill(DispCirInfo->x0-13,DispCirInfo->y0+35,DispCirInfo->x0+113,DispCirInfo->y0+88,RGB);
						PreBitnum2 = 4;
						if((SysParaSetInfo.SensorSwitch&0X01)==0X01 && SensorOnline.Device.SENSAIR==1)
							sprintf(buf,"%04d",SensorData.Carbon);
						else if((SysParaSetInfo.SensorSwitch&0X01)!=0X01 && SensorOnline.Device.PMS_5==1)
							sprintf(buf,"%04d",SensorData.PMData.PM10_S);
						Show_Str(DispCirInfo->x0-5,DispCirInfo->y0+20,150,DispCirInfo->FontSize,buf,DispCirInfo->FontSize,0,BLACK,RGB);
					}break;
					default:break;
				}
			}
			else
				Show_Str(DispCirInfo->x0+17,DispCirInfo->y0+45,150,24,"未使能",24,0,BLACK,RGB);
		}break;
		default:break;
	}
}
//加载传感器数据
void LoadSensorData(void)
{
	//传感器数据转换
	SensorData.BatVol = Get_Adc_Average(ChalTab[1],5);
	if(SensorData.BatVol < LOW_VOL_ALARM_THREHOLD)  //如果采集到的AD值小于报警阈值的话
		AlarmCloseFlag = 5;
	Read_sht30(&SensorData.Temp,&SensorData.Humi);  //读取温度值
	if(SensorOnline.Device.BMP180 == 1)
		SensorData.AirPres = bmp180Convert();       		 //读取气压值
	SensorData.Altitude = (44330.0*(1.0-pow((float)(SensorData.AirPres)/101325.0,1.0/5.255)));
	Rx8025tGetTime(&SysTimeData);
	if(SysParaSetInfo.Power_5V_Status == 1)
	{
		if((SysTimeData.Minute%SysParaSetInfo.SensorSamplingTimeUnit) == 0)
		{
			Handle_5V_POWER_Status(1);
			if(SensorOnline.Device.PMS_5 == 1)
				PMS_Send_Cmd(PMS_READ_DATA);  			//读PM2.5数据
			if((SysParaSetInfo.SensorSwitch&0x02) == 0x02)
			{
				if(SensorOnline.Device.WZ_S == 1)
					HCHODataHandle();
			}
			if((SysParaSetInfo.SensorSwitch&0x01) == 0x01)
			{
				if(SensorOnline.Device.SENSAIR == 1)
				{
					if(TIM4CH2_CAPTURE_STA&0X80)//成功捕获到了一次上升沿
						TIM4CH2_CAPTURE_STA = 0;//开启下一次捕获
				}
			}
		}
		else
			Handle_5V_POWER_Status(0);
	}
	else
		Handle_5V_POWER_Status(0);
}
//保存一小时的记录数据
//一小时的第几分钟
void Save_Hour_Data(uint8_t Order)
{
	if((SysParaSetInfo.SensorSwitch&0x01) == 0x01)
		DataRemember_Hour.CO2[Order] = SensorData.Carbon;
	if((SysParaSetInfo.SensorSwitch&0x01) == 0x01)
		DataRemember_Hour.HCHO[Order] = SensorData.HCHO;
	DataRemember_Hour.PM2_5[Order] = SensorData.PMData.PM2_5_S;
}

//显示控制开关的状态
//150  64
void Ctrl_Switch_Staus(uint16_t xoffset,uint16_t yoffset)
{
	Show_Str(5,yoffset+5,200,16,"报警开关状态:",16,0,WHITE,BOTTOM_TITTLE_BACK_COLOR);
	LCD_DrawRectangle(lcddev.width-xoffset,yoffset+3,lcddev.width-xoffset+86,yoffset+20,WHITE); // 150  6
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X10) == 0X10)
	{
		//if(!CtrlSWStatus.bits.SW1)
		{
			LCD_Fill(lcddev.width-xoffset+2,yoffset+5,lcddev.width-xoffset+16,yoffset+18,GREEN);//白
			LCD_ShowNum(lcddev.width-xoffset+7,yoffset+6,1,1,12,1,BLACK,GREEN);
		}
		/*else
		{
			LCD_Fill(lcddev.width-xoffset+2,yoffset+5,lcddev.width-xoffset+16,yoffset+18,RED);//红色表示已打开
			LCD_ShowNum(lcddev.width-xoffset+7,yoffset+6,1,1,12,BLACK,RED);
		}*/
	}
	else 
	{
		LCD_Fill(lcddev.width-xoffset+2,yoffset+5,lcddev.width-xoffset+16,yoffset+18,GRAY);  //灰
		LCD_ShowNum(lcddev.width-xoffset+7,yoffset+6,1,1,12,1,BLACK,GRAY);
	}
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X08)==0X08 && (SysParaSetInfo.SensorSwitch&0x02)==0x02)
	{
		//if(!CtrlSWStatus.bits.SW2)
		{
			LCD_Fill(lcddev.width-xoffset+18,yoffset+5,lcddev.width-xoffset+33,yoffset+18,GREEN);//白
			LCD_ShowNum(lcddev.width-xoffset+23,yoffset+6,2,1,12,1,BLACK,GREEN);
		}
		/*else
		{
			LCD_Fill(lcddev.width-xoffset+18,5,lcddev.width-xoffset+33,18,RED);//白
			LCD_ShowNum(lcddev.width-xoffset+23,yoffset+6,2,1,1,BLACK,RED);
		}*/
	}
	else 
	{
		LCD_Fill(lcddev.width-xoffset+18,yoffset+5,lcddev.width-xoffset+33,yoffset+18,GRAY);  //灰
		LCD_ShowNum(lcddev.width-xoffset+23,yoffset+6,2,1,12,1,BLACK,GRAY);
	}
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X04)==0X04 && (SysParaSetInfo.SensorSwitch&0x01)==0x01)
	{
		//if(!CtrlSWStatus.bits.SW3)
		{
			LCD_Fill(lcddev.width-xoffset+35,yoffset+5,lcddev.width-xoffset+50,yoffset+18,GREEN);//白
			LCD_ShowNum(lcddev.width-xoffset+40,yoffset+6,3,1,12,1,BLACK,GREEN);
		}
		/*else
		{
			LCD_Fill(lcddev.width-xoffset+35,yoffset+5,lcddev.width-xoffset+50,yoffset+18,RED);
			LCD_ShowNum(lcddev.width-xoffset+40,yoffset+6,3,1,12,1,BLACK,RED);
		}*/
	}
	else 
	{
		LCD_Fill(lcddev.width-xoffset+35,yoffset+5,lcddev.width-xoffset+50,yoffset+18,GRAY);  //灰
		LCD_ShowNum(lcddev.width-xoffset+40,yoffset+6,3,1,12,1,BLACK,GRAY);
	}
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X02) == 0X02)
	{
		//if(!CtrlSWStatus.bits.SW4)
		{
			LCD_Fill(lcddev.width-xoffset+52,yoffset+5,lcddev.width-xoffset+67,yoffset+18,GREEN);//白
			LCD_ShowNum(lcddev.width-xoffset+57,yoffset+6,4,1,12,1,BLACK,GREEN);
		}
		/*else
		{
			LCD_Fill(lcddev.width-xoffset+52,yoffset+5,lcddev.width-xoffset+67,yoffset+18,RED);
			LCD_ShowNum(lcddev.width-xoffset+57,yoffset+6,4,1,12,1,BLACK,RED);
		}*/
	}
	else 
	{
		LCD_Fill(lcddev.width-xoffset+52,yoffset+5,lcddev.width-xoffset+67,yoffset+18,GRAY);  //灰
		LCD_ShowNum(lcddev.width-xoffset+57,yoffset+6,4,1,12,1,BLACK,GRAY);
	}
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X01) == 0X01)
	{
		//if(!CtrlSWStatus.bits.SW5)
		{
			LCD_Fill(lcddev.width-xoffset+69,yoffset+5,lcddev.width-xoffset+84,yoffset+18,GREEN);//白
			LCD_ShowNum(lcddev.width-xoffset+74,yoffset+6,5,1,12,1,BLACK,GREEN);
		}
		/*else
		{
			LCD_Fill(lcddev.width-xoffset+69,yoffset+5,lcddev.width-xoffset+84,yoffset+18,RED);
			LCD_ShowNum(lcddev.width-xoffset+74,yoffset+6,5,1,12,1,BLACK,RED);
		}*/
	}
	else
	{
		LCD_Fill(lcddev.width-xoffset+69,yoffset+5,lcddev.width-xoffset+84,yoffset+18,GRAY);  //灰
		LCD_ShowNum(lcddev.width-xoffset+74,yoffset+6,5,1,12,1,BLACK,GRAY);
	}
}
//更新时间和开关状态机WIFI显示状态
void Update_Set_Info(uint8_t Num)
{
	uint8_t buf[15];
	switch(Num)
	{
		case 1:
		{
			Rx8025tGetTime(&SysTimeData);
			sprintf(buf,"%02d:%02d:%02d",SysTimeData.Hour,SysTimeData.Minute,SysTimeData.Second);
			Show_Str(410,291,300,16,buf,16,0,WHITE,TOP_TITLE_BACK_COLOR);
			sprintf(buf,"20%02d/%02d/%02d",SysTimeData.Year,SysTimeData.Month,SysTimeData.Day);
			Show_Str(394,305,300,16,buf,16,0,WHITE,TOP_TITLE_BACK_COLOR);
		}break;
		case 3:
		{
			//if(Current_Show_Interface==Curr_Para_Show)
				//Ctrl_Switch_Staus(150,6);
			Ctrl_Switch_Staus(360,296);
		}break;
		default:break;
	}	
}
//电池电压计算公式
//采样电阻上的最大电压对应的ADC值 ADC_MAX_VAL = 4.2*BAT_RES2/(BAT_RES1+BAT_RES2) * 4095/3.3
//采样电阻上的最小电压对应的ADC值 ADC_MIN_VAL = 3.3*BAT_RES2/(BAT_RES1+BAT_RES2) * 4095/3.3
//图形框上加上去的坐标书为 WIDTH = (ADC_ABSLUTE-ADC_MIN_VAL) / ((ADC_MAX_VAL - ADC_MIN_VAL)/34)    36为电池图形到的横坐标宽度
//
//显示时间到主界面同时保存传感器采集到的数据
void DispTimeToUI(void)
{
	uint8_t i;
	uint8_t CurrentVol;
	uint16_t Process = 0;
	uint8_t buf[15];
	static uint8_t CreateNewFlag = 0;   //每隔一天创建新的文件名标志
	static uint8_t SaveFlag = 0;
	if((SysTimeData.Minute%5)==0 && SaveFlag!=SysTimeData.Minute)  //5分钟执行一次
	{
		SaveFlag = SysTimeData.Minute;
		Save_Hour_Data(Draw_Wave_Flag++);		//用于在波形绘制函数里面判断用
		if(Draw_Wave_Flag == 15)
		{
			Draw_Wave_Flag = 0;
			memset(&DataRemember_Hour.HCHO[0],0,60); 		//清零
		}
	}
	if(Esp8266InitFinishFlag == 1)
	{
		if((SysTimeData.Minute%SysParaSetInfo.SDRemTimeUnit)==0 && SysTimeData.Second<=1)
		{
			if(SysTimeData.Hour != 0)
				CreateNewFlag = 0;
			if(SysTimeData.Hour==0 && CreateNewFlag==0)  //已经到了第二天或者刚开机的话
			{
				CreateNewFlag = 1;
				Get_New_pname(pname);
			}
			Write_Data_To_SDCard(pname,1);  //保存数据到SD卡，如果没有文件则创建文件用来保存数据
		}
	}
	if((SysTimeData.Second%10) == 0)				//10秒钟执行一次
	{
		if(Esp8266Config.ServerLinkStat && (SysParaSetInfo.SensorSwitch&0x04) == 0x04)  //根绝服务器状态进行显示相关的LOGO信息
			Show_DrawBMP(lcddev.width-70,10,32,25,(uint8_t*)CloudUpload,GREEN,TOP_TITLE_BACK_COLOR);
		else
			Show_DrawBMP(lcddev.width-70,10,32,25,(uint8_t*)NoCloudUpload,GRAY,TOP_TITLE_BACK_COLOR);
	}
	DispWeatherInfo();
	sprintf(buf,"%02d:%02d:%02d",SysTimeData.Hour,SysTimeData.Minute,SysTimeData.Second);
	Show_Str(410,291,300,16,buf,16,0,WHITE,BOTTOM_TITTLE_BACK_COLOR);
	sprintf(buf,"20%02d/%02d/%02d",SysTimeData.Year,SysTimeData.Month,SysTimeData.Day);
	Show_Str(394,305,300,16,buf,16,0,WHITE,BOTTOM_TITTLE_BACK_COLOR);
	if((SysTimeData.Second%5)==0)
	{
		for(i=0;i<13;i++)  //总共13个档位
		{
			if(SensorData.BatVol <= Vol_To_ADC[i])
			{
				CurrentVol = i;  //补偿1个
				break;
			}
		}
		if(CurrentVol > 1)
		{
			LCD_DrawRectangle(lcddev.width-40,8,lcddev.width-4,20,WHITE);  //画出基本的轮廓
			LCD_Fill(lcddev.width-4,11,lcddev.width,16,WHITE);			//绘制电池顶部的小框
			LCD_Fill(lcddev.width-39,9,lcddev.width-5,19,TOP_TITLE_BACK_COLOR);   		//清除电池
			if(((uint16_t)CurrentVol*3)>35)  //防止超越边界
				CurrentVol = 12;//35/(Multiple*3);
			LCD_Fill(lcddev.width-39,9,lcddev.width-39+(uint16_t)CurrentVol*3+Multiple,19,LGRAY);  //横坐标不能大于30，框的横向像素范围
		}
		else
		{
			LCD_DrawRectangle(lcddev.width-40,8,lcddev.width-4,20,RED);  //画出基本的轮廓
			LCD_Fill(lcddev.width-4,11,lcddev.width,16,RED);			//绘制电池顶部的小框
			LCD_Fill(lcddev.width-39,9,lcddev.width-5,19,TOP_TITLE_BACK_COLOR);   		//清除电池
			if(((uint16_t)CurrentVol*3)>35)  //防止超越边界
				CurrentVol = 12;//35/(Multiple*3);
			LCD_Fill(lcddev.width-39,9,lcddev.width-39+(uint16_t)CurrentVol*3+Multiple,19,RED);  //横坐标不能大于30，框的横向像素范围
		}
	}
}
//显示天气信息
void DispWeatherInfo(void)
{
	//AcuireFinishFlag = 0;
	if(AcuireFinishFlag == 1)
	{
		AcuireFinishFlag = 0;
		uint8_t buf[20];
		sprintf(buf,"当前城市:%s",NetWorkDataInfo.Weather_CityInfo);
		Show_Str(271,0,150,16,buf,16,0,WEATHERINFO_COLOR,TOP_TITLE_BACK_COLOR);
		sprintf(buf,"%s %s %d℃",NetWorkDataInfo.Time_Week,NetWorkDataInfo.Weather_Climate,NetWorkDataInfo.Weather_TempInfo);
		Show_Str(277,17,150,16,buf,16,0,WEATHERINFO_COLOR,TOP_TITLE_BACK_COLOR);
	}
}
//加载显示界面1
void Load_ParaShow_Interface_1(void)
{	
	uint8_t i;
	uint8_t err;
	int16_t Temp=0;
	uint16_t Cnt = 0;
	LCD_Fill(0,36,CURSOR-22,289,PARA_SHOW_INTERFACE_BACKCOLOR);  //清楚显示板显示的传感器数据的内容
	if(LoadStaticUIFlag == 0)
	{
		LoadStaticUIFlag = 1;
		//LCD_Fill(CURSOR-21,27,480,320,FillColor);  	//右边的长条底色
		//LCD_Fill(0,0,480,25,TOP_TITLE_BACK_COLOR);			//顶部的标题部分
		//LCD_Fill(0,290,480,320,BOTTOM_TITTLE_BACK_COLOR);		//底部的标题背景色
		LCD_DrawLine(0,35,CURSOR-22,35,LIGHTBLUE);	//下面的水平线
		LCD_DrawLine(0,290,CURSOR-22,290,LIGHTBLUE);	//下面的水平线
		LoadColorPat();   							//加载色谱和标度
		Show_Str(0,3,100,24,BRAND_NAME,24,0,WHITE,TOP_TITLE_BACK_COLOR);  //显示商标
		if(AcuireFinishFlag == 0)
			Show_Str(281,7,150,16,"无天气信息",16,0,GRAY,TOP_TITLE_BACK_COLOR);
		DispWeatherInfo();
	}
	Ctrl_Switch_Staus(360,296);	  //从右边往左边便宜坐标
	for(i=0;i<6;i++)
	{
		Load_ShowUI_1(&AllDispCirInfo[Item_Id[i]],Item_Id[i]);
	}
	LCD_DrawLine(0,216,CURSOR-22,216,LIGHTBLUE);  //中间的第一个水平线	
	TIM2->CCR2 = BackLightBright;   //此时点亮液晶屏
	Current_Show_Interface = Curr_Para_Show;
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)
		{
			if(Encoder_Type==ENCODER_FORWORD || Encoder_Type==ENCODER_BACKWORD)
			{
				break;
			}
			else if(Encoder_Type == ENCODER_PRESSED)  //如果是长按的话
			{
				Encoder_Type = ENCODER_VOID;
				while((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0))
				{
					Cnt++;
					delay_ms(200); 
					if(Cnt>=10)
						break;
				}
				if(Cnt>=10)  //如果是长按的话
				{
					Cnt = 0;
					SetBeepFreq(AlarmFreqTab[8]);
					Show_Str(240,300,200,16,"亮度调节    %",16,0,LIGHTBLUE,PARA_SHOW_INTERFACE_BACKCOLOR);
					LCD_ShowNum(305,300,100-BackLightBright/10,3,16,1,LIGHTBLUE,PARA_SHOW_INTERFACE_BACKCOLOR);
					while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);
					OSTmrStop(tmr2,OS_TMR_OPT_NONE,0,&err);
					while(1)
					{
						if(Cnt++ > 100)  			//4秒钟没反应则跳出调节
							break;
						if(Encoder_Type == ENCODER_FORWORD)  //渐亮
						{
							Encoder_Type = ENCODER_VOID;
							Cnt = 0;
							BackLightBright -= 50;
							if(BackLightBright <= 0)
								BackLightBright = 1;
							TIM2->CCR2 = (uint16_t)BackLightBright;
							LCD_ShowNum(305,300,100-BackLightBright/10,3,16,1,LIGHTBLUE,PARA_SHOW_INTERFACE_BACKCOLOR);
						}
						else if(Encoder_Type == ENCODER_BACKWORD)//渐暗
						{
							Encoder_Type = ENCODER_VOID;
							Cnt = 0;
							BackLightBright += 50;
							if(BackLightBright >= 1000)
								BackLightBright = 999;
							TIM2->CCR2 = (uint16_t)BackLightBright;
							LCD_ShowNum(305,300,100-BackLightBright/10,3,16,1,LIGHTBLUE,PARA_SHOW_INTERFACE_BACKCOLOR);
						}
						else if(Encoder_Type == ENCODER_PRESSED)
						{
							Encoder_Type = ENCODER_VOID;
							Cnt = 0;
							Temp = (uint8_t)(BackLightBright>>8);
							AT24CXX_WriteOneByte(EEPROM_ADDR_SYS_PARA_BACKLIGHT,Temp);
							Temp = (uint8_t)(BackLightBright&0xff);
							AT24CXX_WriteOneByte(EEPROM_ADDR_SYS_PARA_BACKLIGHT+1,Temp);
							OSTmrStart(tmr2,&err);
							//Show_Str(240,300,100,16,"            ",16,0,LIGHTBLUE,BACK_COLOR);
							break;  //跳出
						}
						delay_ms(20);
					}
				}
			}
		}
		else
		{
			OSSemPend(sem_data_handle,0,&err);  //请求信号量
			//LoadSensorData();  //加载传感器数据
			for(i=0;i<7;i++)
			{
				if(CloseMachineFlag == 0)
					DispDataInfo(&AllDispCirInfo[Item_Id[i]],Item_Id[i]);
			}
			DispTimeToUI();
			if(CloseMachineFlag == 0)
				EnvirQuaty(0,192);  //质量检测判断
		}
		//delay_ms(10);
		//EnvirQuaty(0,292);  //质量检测判断
	}
}
//加载界面2要显示的静态UI
void Load_UI2_Static(uint16_t x,uint16_t y,uint16_t unit)
{
	LCD_Fill(0,36,CURSOR-22,289,PMCNT_SHOW_INTERFACE_BACKCOLOR);  //清楚显示板显示的传感器数据的内容
	//LCD_DrawLine(0,27,CURSOR-22,27,BLUE);
	//显示浓度值的UI
	Show_Str(x+50,y,300,24,"浓度值",24,0,C_THISTLE,BLACK);  
	Show_Str(280+x,y,300,24,"颗粒物个数",24,0,C_THISTLE,BLACK); //这里补偿了50
	
	Show_Str(x,y+unit,300,24,"PM2.5",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(x,y+unit*2,300,24,"PM1.0",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(x,y+unit*3,300,24,"PM10",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(x,y+unit*4,300,24,"HCHO",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(x,y+unit*5,300,24,"CO2",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(x+80,y+unit*4,100,24,".",24,0,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);  		//显示一点
	
	Show_Str(x+135,y+unit,300,24,"ug/m3",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(x+135,y+unit*2,300,24,"ug/m3",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(x+135,y+unit*3,300,24,"ug/m3",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(x+135,y+unit*4,300,24,"mg/m3",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(x+135,y+unit*5,300,24,"ppm",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	//显示颗粒物的UI
	
	Show_Str(240+x,y+unit,300,24,">0.3um:",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(240+x,y+unit*2,300,24,">0.5um:",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(240+x,y+unit*3,300,24,">1.0um:",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(240+x,y+unit*4,300,24,">2.5um:",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(240+x,y+unit*5,300,24,">5.0um:",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(240+x,y+unit*6,300,24,">10um:",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	
	Show_Str(240+x+150,y+unit,300,24,"个",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(240+x+150,y+unit*2,300,24,"个",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(240+x+150,y+unit*3,300,24,"个",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(240+x+150,y+unit*4,300,24,"个",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(240+x+150,y+unit*5,300,24,"个",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(240+x+150,y+unit*6,300,24,"个",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
}
//加载界面2要动态显示的数据值
void Load_UI2_Dynamic(uint16_t x,uint16_t y,uint16_t unit)
{
	//uint8_t buf[10];
	//显示浓度值
	LCD_ShowNum(x,y+unit,SensorData.PMData.PM2_5_S,4,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);     	//PM2.5  ug/m3
	LCD_ShowNum(x,y+unit*2,SensorData.PMData.PM1_0_S,4,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);  	//PM1.0  ug/m3
	LCD_ShowNum(x,y+unit*3,SensorData.PMData.PM10_S,4,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR); 		//PM10  ug/m3
	
	//sprintf(buf,"%f",(float)SensorData.HCHO/100);
	LCD_ShowNum(x-5,y+unit*4,SensorData.HCHO/100,1,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);  				//HCHO  ug/m3
	LCD_ShowNum(x+24-5,y+unit*4,SensorData.HCHO/10%10,1,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	LCD_ShowNum(x+40-5,y+unit*4,SensorData.HCHO%10,1,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	LCD_ShowNum(x-5,y+unit*5,SensorData.Carbon,5,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);  			//CO2  ug/m3
	//显示颗粒物数量
	LCD_ShowNum(240+x+17,y+unit,SensorData.PMData.Cnt_0_3,5,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);  		//>0.3 um
	LCD_ShowNum(240+x+17,y+unit*2,SensorData.PMData.Cnt_0_5,5,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);  	//>0.5 um
	LCD_ShowNum(240+x+17,y+unit*3,SensorData.PMData.Cnt_1_0,5,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);  	//>1.0 um
	LCD_ShowNum(240+x+17,y+unit*4,SensorData.PMData.Cnt_2_5,5,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);  	//>2.5 um
	LCD_ShowNum(240+x+17,y+unit*5,SensorData.PMData.Cnt_5_0,5,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);  	//>5.0 um
	LCD_ShowNum(240+x+17,y+unit*6,SensorData.PMData.Cnt_10,5,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);  		//>10 um
}
//加载颗粒物体浓度显示界面2
void Load_PMCnctrShow_Interface_2(void)
{
	uint8_t err;
	Load_UI2_Static(20,42,35);//起点x，起点y，间距
	Current_Show_Interface = Curr_PM2_5_Show;
	OSTmrStart(tmr1,&err);//启动软件定时器1
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)
		{
			if(Encoder_Type==ENCODER_FORWORD || Encoder_Type==ENCODER_BACKWORD)
			{
				OSTmrStop(tmr1,OS_TMR_OPT_NONE,0,&err);
				break;
			}
		}
		//LoadSensorData();
		OSSemPend(sem_data_handle,0,&err);  //请求信号量
		if(CloseMachineFlag == 0)
			Load_UI2_Dynamic(90,40,35);
		DispTimeToUI();		
		EnvirQuaty(0,192);
		if(AutoJumpFlag)
		{
			break;
		}
		delay_ms(10);
		//EnvirQuaty(0,292);  //质量检测判断
	}
}
//加载一小时数据波形
//甲醛，PM3.5，CO2，温湿度，气压         10         260
void Load_WaveformShow_Interface_3(void)
{
	uint8_t i;
	uint8_t err;
	static uint8_t RefreshFlag = 0;  //波形刷新标志 
	//uint8_t HCHO;
	//uint16_t CO2,PM;
	LCD_Fill(0,36,CURSOR-22,289,WAVE_SHOW_INTERFACE_BACKCOLOR);  //清楚显示板显示的传感器数据的内容
	
	Show_Str(0,267,80,16,"PM2.5",16,0,LIGHTBLUE,WAVE_SHOW_INTERFACE_BACKCOLOR);    //家在出全部的时间参数
	Show_Str(0,198,80,16,"HCHO",16,0,LIGHTBLUE,WAVE_SHOW_INTERFACE_BACKCOLOR);    //家在出全部的时间参数
	Show_Str(0,121,80,16,"CO2",16,0,LIGHTBLUE,WAVE_SHOW_INTERFACE_BACKCOLOR);    //家在出全部的时间参数
	//绘制坐标轴                                       起点坐标
	LCD_DrawLine(40,275, CURSOR-40,275,WHITE);  //X轴   PM2.5 Y轴起点开始坐标
	LCD_DrawLine(40,275, 40,36,WHITE);          //Y轴
	LCD_DrawLine(40,206, CURSOR-40,206,WHITE);  //X轴   HCHO
	LCD_DrawLine(40,129, CURSOR-40,129,WHITE);  //X轴   CO2
	//绘制刻度
	for(i=1;i<32;i++)
	{
		LCD_DrawLine(40+12*i,274,40+12*i,271,GRAY);		//垂直标度(436-10)/10=42.6
		if(i<15)
			LCD_ShowNum(40+29*(i-1),276,(i-1)*5,2,12,1,WHITE,WAVE_SHOW_INTERFACE_BACKCOLOR);		
		LCD_DrawLine(40+12*i,205,40+12*i,203,GRAY);		//垂直标度(436-10)/10=42.6
		
		LCD_DrawLine(40+12*i,128,40+12*i,126,GRAY);		//垂直标度(436-10)/10=42.6
		
		//LCD_DrawLine(30,290-23*i,CURSOR-30,290-23*i,GRAY);	//水平标度
	}
	LCD_DrawLine(40,259,CURSOR-40,259,RED);//PM2.5   274-75/5得来的   画标度线
	LCD_DrawLine(40,165,CURSOR-40,165,RED);//HCHO
	LCD_DrawLine(40,90,CURSOR-40,90,RED);//CO2
	
	Show_Str(24,259,100,12,"75",12,0,RED,WAVE_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(14,155,100,12,"0.08",12,0,RED,WAVE_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(14,85,100,12,"800",12,0,RED,WAVE_SHOW_INTERFACE_BACKCOLOR);
	for(i=0;i<Draw_Wave_Flag-1;i++)
	{
		LCD_DrawLine(40+i*29,274-DataRemember_Hour.PM2_5[i]/5,40+(i+1)*29,274-DataRemember_Hour.PM2_5[i+1]/5,GREEN);  //4为顶值为300时的
		if((SysParaSetInfo.SensorSwitch&0x01) == 0x01)
			LCD_DrawLine(40+i*29,128-DataRemember_Hour.CO2[i]/21,40+(i+1)*29,128-DataRemember_Hour.CO2[i+1]/21,GREEN);
		if((SysParaSetInfo.SensorSwitch&0x02) == 0x02)
			LCD_DrawLine(40+i*29,205-DataRemember_Hour.HCHO[i]*5.13,40+(i+1)*29,205-DataRemember_Hour.HCHO[i+1]*5.13,GREEN);
	}
	Current_Show_Interface = Curr_WaveForm_Show;
	OSTmrStart(tmr1,&err);//启动软件定时器1
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)
		{
			if(Encoder_Type==ENCODER_FORWORD || Encoder_Type==ENCODER_BACKWORD)
			{
				OSTmrStop(tmr1,OS_TMR_OPT_NONE,0,&err);
				break;
			}
		}
		OSSemPend(sem_data_handle,0,&err);  //请求信号量
		DispTimeToUI();
		EnvirQuaty(0,192);
		if(AutoJumpFlag)
		{
			break;
		}
		delay_ms(10);
	}
	//这里添加绘制数据的内容
}
//设置参数UI界面
void Load_SetShow_Interface_4(void)
{
	uint8_t Cnt = 0;
	uint8_t err;
	LCD_Fill(0,36,239,289,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	LCD_Fill(241,36,CURSOR-22,289,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(150,40,300,24,"系统参数设置",24,0,BLACK,BRRED);
	//LCD_DrawLine(0,60,CURSOR-25,60,LIGHTBLUE);  //分割线
	LCD_DrawLine(240,60,240,290,LIGHTBLUE);
	//LCD_DrawLine(0,290,CURSOR-25,290,LIGHTBLUE);
	Show_Str(25,65,300,24,(uint8_t *)SetUIL1_Text[0],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);    //家在出全部的时间参数
	Show_Str(25,95,300,24,(uint8_t *)SetUIL1_Text[1],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,125,300,24,(uint8_t *)SetUIL1_Text[2],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,155,300,24,(uint8_t *)SetUIL1_Text[3],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,185,300,24,(uint8_t *)SetUIL1_Text[4],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,215,300,24,(uint8_t *)SetUIL1_Text[5],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,245,300,24,(uint8_t *)SetUIL1_Text[6],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Current_Show_Interface = Curr_Set_Show;
	OSTmrStart(tmr1,&err);//启动软件定时器1
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)
		{
			if(Encoder_Type == ENCODER_PRESSED)
			{
				Current_Show_Interface = Curr_SetPara_Show;
				OSTaskSuspend(PRIOR_TASK_PRIO);	//挂起触摸屏任务	
			}
			OSTmrStop(tmr1,OS_TMR_OPT_NONE,0,&err);
			break;
		}
		OSSemPend(sem_data_handle,0,&err);  //请求信号量
		DispTimeToUI();
		EnvirQuaty(0,192);
		if(AutoJumpFlag)
		{
			break;
		}
		delay_ms(10);   //系统延时，任务调度
	}
}
//第一层菜单
//时间
void SetLay1UI_Time(void)
{
	uint8_t err;
	Show_Str(25,65,300,24,(uint8_t *)SetUIL1_Text[0],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_TEXTBACKCOLOR);
	Show_Str(25,95,300,24,(uint8_t *)SetUIL1_Text[1],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,245,300,24,(uint8_t *)SetUIL1_Text[6],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR); //退出后清楚显示
	//显示具体的内容
	Show_Str(290,65,300,24,(uint8_t *)SetUIL2_Text[0],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(290,95,300,24,(uint8_t *)SetUIL2_Text[1],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(290,135,300,24,(uint8_t *)SetUIL2_Text[2],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(290,170,300,24,(uint8_t *)SetUIL2_Text[3],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(290,205,300,24,(uint8_t *)SetUIL2_Text[4],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(290+50,65,2000+SysTimeData.Year,4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(290+50,95,SysTimeData.Month,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(290+50,135,SysTimeData.Day,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(290+50,170,SysTimeData.Hour,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(290+50,205,SysTimeData.Minute,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //如果还在按着就继续等待
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)
		{
			OSTmrStop(tmr1,OS_TMR_OPT_NONE,0,&err);
			break;
		}
		delay_ms(10);
	}
}
//微波
void SetLay1UI_AutoStandby(void)
{
	Show_Str(25,65,300,24,(uint8_t *)SetUIL1_Text[0],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,95,300,24,(uint8_t *)SetUIL1_Text[1],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_TEXTBACKCOLOR);
	Show_Str(25,125,300,24,(uint8_t *)SetUIL1_Text[2],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR); //退出后清楚显示
	Show_Str(250,65,300,24,"自动待机:",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,89,300,24,"界面切换:",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,113,300,24,"待机延时:  分",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if(SysParaSetInfo.AutoStandbyStatus)
	{
		Show_Str(360,65,300,24,"已开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		LCD_ShowNum(360,113,SysParaSetInfo.AutoStandbyTime,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	}		
	else
	{
		Show_Str(360,65,300,24,"已关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		LCD_ShowNum(360,113,SysParaSetInfo.AutoStandbyTime,2,24,1,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	}		
	if(SysParaSetInfo.InterfaceSwitch)
		Show_Str(360,89,300,24,"已开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	else
		Show_Str(360,89,300,24,"已关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)
			break;
		delay_ms(10);
	}
}
//语音识别
void SetLay1UI_DeviceSW(void)
{
	Show_Str(25,95,300,24,(uint8_t *)SetUIL1_Text[1],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,125,300,24,(uint8_t *)SetUIL1_Text[2],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_TEXTBACKCOLOR);
	Show_Str(25,155,300,24,(uint8_t *)SetUIL1_Text[3],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR); //退出后清楚显示
	Show_Str(250,65,300,24,"设备电源:",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,89,300,24,"语音识别:",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,113,300,24,"语音音量:",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if(SysParaSetInfo.Power_5V_Status)
		Show_Str(360,65,300,24,"已开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	else
		Show_Str(360,65,300,24,"已关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	if(SysParaSetInfo.SpeechRecognize.Speech_Sw)
	{
		Show_Str(360,89,300,24,"已开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		LCD_ShowNum(360,113,SysParaSetInfo.SpeechRecognize.Speech_Volume,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	}
	else
	{
		Show_Str(360,89,300,24,"已关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		LCD_ShowNum(360,113,SysParaSetInfo.SpeechRecognize.Speech_Volume,2,24,1,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	}
	//LCD_ShowNum(300,170,SysParaSetInfo.Power_5V_Status,2,24,1,WHITE,BLUE);
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)
			break;
		delay_ms(10);
	}
}
//CO2
void SetLay1UI_DataRemUnit(void)
{
	Show_Str(25,125,300,24,(uint8_t *)SetUIL1_Text[2],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,155,300,24,(uint8_t *)SetUIL1_Text[3],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_TEXTBACKCOLOR);
	Show_Str(25,185,300,24,(uint8_t *)SetUIL1_Text[4],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR); //退出后清楚显示   
	Show_Str(250,65,300,24,"记录间隔:    分",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,89,300,24,"采样间隔:    分",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	LCD_ShowNum(365,65,SysParaSetInfo.SDRemTimeUnit,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(365,89,SysParaSetInfo.SensorSamplingTimeUnit,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)
			break;
		
		delay_ms(10);
	}
}
//阈值
void SetLay1UI_Threshold(void)
{
	uint8_t i =0;
	uint8_t buf[10];
	uint8_t *p = &SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus;  
	uint8_t *p1 = &SysParaSetInfo.SensorSwitch;
	Show_Str(25,155,300,24,(uint8_t *)SetUIL1_Text[3],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,185,300,24,(uint8_t *)SetUIL1_Text[4],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_TEXTBACKCOLOR);
	Show_Str(25,215,300,24,(uint8_t *)SetUIL1_Text[5],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR); //退出后清楚显示 
	//Show_Str(250,170,300,24,"传感器阈值设置",24,0,GREEN,BLACK);
	for(i=0;i<5;i++)
	{
		if(*p&(uint8_t)(1<<(4-i)))
		{
			if(i==1)
			{
				Show_Str(250,65+i*30,300,24,(uint8_t *)SetUIL2_Text[i+7],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
				Show_Str(330,65+i*30,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
				sprintf(buf,"0.%02d",SysParaSetInfo.ThresHoldStatus.ThrldVal[i]);
				Show_Str(360,65+i*30,100,24,buf,24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			}
			else
			{
				Show_Str(250,65+i*30,300,24,(uint8_t *)SetUIL2_Text[i+7],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
				Show_Str(330,65+i*30,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
				LCD_ShowNum(360,65+i*30,SysParaSetInfo.ThresHoldStatus.ThrldVal[i],4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			}
		}
		else
		{
			if(i==1)
			{
				Show_Str(250,65+i*30,300,24,(uint8_t *)SetUIL2_Text[i+7],24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
				Show_Str(330,65+i*30,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
				sprintf(buf,"0.%02d",SysParaSetInfo.ThresHoldStatus.ThrldVal[i]);
				Show_Str(360,65+i*30,100,24,buf,24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			}
			else
			{
				Show_Str(250,65+i*30,300,24,(uint8_t *)SetUIL2_Text[i+7],24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
				Show_Str(330,65+i*30,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
				LCD_ShowNum(360,65+i*30,SysParaSetInfo.ThresHoldStatus.ThrldVal[i],4,24,1,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			}
		}
	}
	if((*p1&0x02) == 0x02)
		Show_Str(250,65+5*30,300,24,"HCHO使能:开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	else if((*p1&0x02) != 0x02)
		Show_Str(250,65+5*30,300,24,"HCHO使能:关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if((*p1&0x01) == 0x01)
		Show_Str(250,65+6*30,300,24,"CO2使能:开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	else if((*p1&0x01) != 0x01)
		Show_Str(250,65+6*30,300,24,"CO2使能:关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)   //跳出
			break;
		delay_ms(10);
	}
}
//WIFI状态配置
void SetLay1UI_WIFIConfig(void)
{
	Show_Str(25,185,300,24,(uint8_t *)SetUIL1_Text[4],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,215,300,24,(uint8_t *)SetUIL1_Text[5],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_TEXTBACKCOLOR);
	Show_Str(25,245,300,24,(uint8_t *)SetUIL1_Text[6],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR); //退出后清楚显示
	if(!Esp8266Config.WifiLinkSuccFlag)
		Show_Str(270,65,170,16,"按下进入",16,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	else
		Show_Str(270,65,170,16,"按下设置",16,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if((SysParaSetInfo.SensorSwitch&0x04) == 0x04)
	{
		if(Esp8266Config.WifiLinkSuccFlag)
		{
			Show_Str(250,140,170,16,Esp8266Config.WireNet[0],16,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			Show_Str(250,175,170,16,Esp8266Config.WireNet[1],16,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			Show_Str(250,210,170,16,Esp8266Config.ServerIP,16,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		}
	}
	else if((SysParaSetInfo.SensorSwitch&0x04) != 0x04)
		Show_Str(250,175,170,24,"WIFI未使能",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)   //跳出
		{
			if(Encoder_Type == ENCODER_PRESSED)
				LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR); //退出后清楚显示
			break;
		}
		delay_ms(10);
	}
}
//退出
void SetLay1UI_Quit(void)
{
	Show_Str(25,65,300,24,(uint8_t *)SetUIL1_Text[0],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,215,300,24,(uint8_t *)SetUIL1_Text[5],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,245,300,24,(uint8_t *)SetUIL1_Text[6],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_TEXTBACKCOLOR);
	LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR); //退出后清楚显示
	Show_Str(260,170,300,24,"退出设置",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)   //跳出
		{
			if(Encoder_Type == ENCODER_PRESSED)
			{
				WriteSetPara();			//写入配置参数
				DispWifiInfoFlag = 0;  	//在设置参数状态下显示WIFI信息的标志
				LCD_Fill(0,65,439,289,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
				OSTaskResume(PRIOR_TASK_PRIO);
			}
			break;
		}
		delay_ms(10);
	}
}
//第二层菜单
//年
void SetLay2UI_Time_Year(void)
{
	Show_Str(290,65,300,24,(uint8_t *)SetUIL2_Text[0],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(290,95,300,24,(uint8_t *)SetUIL2_Text[1],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(290,205,300,24,(uint8_t *)SetUIL2_Text[4],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	
	LCD_ShowNum(290+50,65,2000+SysTimeData.Year,4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	LCD_ShowNum(290+50,95,SysTimeData.Month,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(290+50,205,SysTimeData.Minute,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //如果还在按着就继续等待
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysTimeData.Year ++;
			if(SysTimeData.Year >= 89)
				SysTimeData.Year = 0;
			//这里显示设置的具体数值
			LCD_ShowNum(290+50,65,2000+SysTimeData.Year,4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysTimeData.Year --;
			if(SysTimeData.Year <= 0)
				SysTimeData.Year = 89;
			//这里显示设置的具体数值
			LCD_ShowNum(290+50,65,2000+SysTimeData.Year,4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			break;
		}
		delay_ms(10);
	}
}
//月
void SetLay2UI_Time_Month(void)
{
	Show_Str(290,65,300,24,(uint8_t *)SetUIL2_Text[0],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(290,95,300,24,(uint8_t *)SetUIL2_Text[1],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(290,135,300,24,(uint8_t *)SetUIL2_Text[2],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(290+50,65,2000+SysTimeData.Year,4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(290+50,95,SysTimeData.Month,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	LCD_ShowNum(290+50,135,SysTimeData.Day,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //如果还在按着就继续等待
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysTimeData.Month ++;
			if(SysTimeData.Month>= 13)
				SysTimeData.Month = 1;
			//这里显示设置的具体数值
			LCD_ShowNum(290+50,95,SysTimeData.Month,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysTimeData.Month --;
			if(SysTimeData.Month <= 0)
				SysTimeData.Month = 12;
			//这里显示设置的具体数值
			LCD_ShowNum(290+50,95,SysTimeData.Month,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			break;
		}
		delay_ms(10);
	}
}
//日
void SetLay2UI_Time_Day(void)
{
	Show_Str(290,95,300,24,(uint8_t *)SetUIL2_Text[1],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(290,135,300,24,(uint8_t *)SetUIL2_Text[2],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(290,170,300,24,(uint8_t *)SetUIL2_Text[3],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	
	LCD_ShowNum(290+50,95,SysTimeData.Month,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(290+50,135,SysTimeData.Day,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	LCD_ShowNum(290+50,170,SysTimeData.Hour,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //如果还在按着就继续等待
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysTimeData.Day ++;
			if(SysTimeData.Day >= 32)
				SysTimeData.Day = 1;
			//这里显示设置的具体数值
			LCD_ShowNum(290+50,135,SysTimeData.Day,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysTimeData.Day --;
			if(SysTimeData.Day <= 0)
				SysTimeData.Day = 31;
			//这里显示设置的具体数值
			LCD_ShowNum(290+50,135,SysTimeData.Day,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			break;
		}
		delay_ms(10);
	}
}
//时
void SetLay2UI_Time_Hour(void)
{
	Show_Str(290,135,300,24,(uint8_t *)SetUIL2_Text[2],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(290,170,300,24,(uint8_t *)SetUIL2_Text[3],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(290,205,300,24,(uint8_t *)SetUIL2_Text[4],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	
	LCD_ShowNum(290+50,135,SysTimeData.Day,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(290+50,170,SysTimeData.Hour,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	LCD_ShowNum(290+50,205,SysTimeData.Minute,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //如果还在按着就继续等待
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysTimeData.Hour ++;
			if(SysTimeData.Hour >= 24)
				SysTimeData.Hour = 0;
			//这里显示设置的具体数值
			LCD_ShowNum(290+50,170,SysTimeData.Hour,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysTimeData.Hour --;
			if(SysTimeData.Hour == 255)
				SysTimeData.Hour = 23;
			//这里显示设置的具体数值
			LCD_ShowNum(290+50,170,SysTimeData.Hour,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			break;
		}
		delay_ms(10);
	}
}
//分
void SetLay2UI_Time_Minute(void)
{
	Show_Str(290,65,300,24,(uint8_t *)SetUIL2_Text[0],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(290,170,300,24,(uint8_t *)SetUIL2_Text[3],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(290,205,300,24,(uint8_t *)SetUIL2_Text[4],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	
	LCD_ShowNum(290+50,65,2000+SysTimeData.Year,4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(290+50,170,SysTimeData.Hour,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(290+50,205,SysTimeData.Minute,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //如果还在按着就继续等待
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysTimeData.Minute ++;
			if(SysTimeData.Minute >= 60)
				SysTimeData.Minute = 0;
			//这里显示设置的具体数值
			LCD_ShowNum(290+50,205,SysTimeData.Minute,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysTimeData.Minute --;
			if(SysTimeData.Minute <= 0)
				SysTimeData.Minute = 59;
			//这里显示设置的具体数值
			LCD_ShowNum(290+50,205,SysTimeData.Minute,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			LCD_Fill(290,65,430,250,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);//退出后清楚显示		
			Set_Rx8025t_Time(SysTimeData.Year,SysTimeData.Month,SysTimeData.Day,SysTimeData.Hour,SysTimeData.Minute,0);  //设置时间	
			Update_Set_Info(1);	//时间更新		
			Show_Str(290,170,300,24,"已设置",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);			
			break;
		}
		delay_ms(10);
	}
}
//菜单界面3，设置阈值是否开启的状态模式
void SetLay3UI_ThresHold_Status(void)
{
	LCD_Fill(249,169,430,185,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);//下面显示的小字
	Show_Str(250,65,300,24,"报警开关",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(250,95,300,24,"报警阈值",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,125,300,24,"传感使能",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)
			break;
		//delay_ms(100);
	}
}
//菜单界面3，阈值调节模式
void SetLay3UI_ThresHold_Adjust(void)
{
	Show_Str(250,65,300,24,"报警开关",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,95,300,24,"报警阈值",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(250,125,300,24,"传感使能",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)
			break;
		delay_ms(10);
	}
}
//传感器开关设置
void SetLay3UI_Sensor_Status(void)
{
	Show_Str(250,65,300,24,"报警开关",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,95,300,24,"报警阈值",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,125,300,24,"传感使能",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //如果还在按着就继续等待
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)
			break;
		delay_ms(10);
	}
}
//设置甲醛传感器开关状态
void Set_HCHOSensor_Status(void)
{
	LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,65,300,24,(uint8_t *)SetUIL2_Text[8],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(250,95,300,24,(uint8_t *)SetUIL2_Text[9],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if((SysParaSetInfo.SensorSwitch&0X02) == 0X02)
		Show_Str(330,65,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
	else 
		Show_Str(330,65,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	if((SysParaSetInfo.SensorSwitch&0X01) == 0X01)
		Show_Str(330,95,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	else
		Show_Str(330,95,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SensorSwitch |= 1<<1;
			Show_Str(330,65,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SensorSwitch &= ~(1<<1);
			Show_Str(330,65,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			if((SysParaSetInfo.SensorSwitch&0X02) == 0X02)
				Show_Str(330,65,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			else
				Show_Str(330,65,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			break;
		}
	}
}
//设置二氧化碳传感器开关状态
void Set_CO2Sensor_Status(void)
{
	Show_Str(250,65,300,24,(uint8_t *)SetUIL2_Text[8],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,95,300,24,(uint8_t *)SetUIL2_Text[9],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	if((SysParaSetInfo.SensorSwitch&0X02) == 0X02)
		Show_Str(330,65,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	else
		Show_Str(330,65,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if((SysParaSetInfo.SensorSwitch&0X01) == 0X01)
		Show_Str(330,95,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
	else
		Show_Str(330,95,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SensorSwitch |= 1<<0;
			Show_Str(330,95,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SensorSwitch &= ~(1<<0);
			Show_Str(330,95,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			if((SysParaSetInfo.SensorSwitch&0X01) == 0X01)
				Show_Str(330,95,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			else
				Show_Str(330,95,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			Show_Str(290,170,300,24,"已设置",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			break;
		}
	}
}
//PM2.5阈值调节
void Set_PM2_5_Threshold(void)
{
	LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,65,300,24,(uint8_t *)SetUIL2_Text[7],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(250,95,300,24,(uint8_t *)SetUIL2_Text[8],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,135,300,24,(uint8_t *)SetUIL2_Text[9],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,170,300,24,(uint8_t *)SetUIL2_Text[10],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,205,300,24,(uint8_t *)SetUIL2_Text[11],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X10) == 0X10)
		Show_Str(330,65,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	else
		Show_Str(330,65,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //如果还在按着就继续等待
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus |= 1<<4;
			Show_Str(330,65,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus &= ~(1<<4);
			Show_Str(330,65,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			Encoder_Type = ENCODER_VOID;
		}	
		else if(Encoder_Type == ENCODER_PRESSED)
			break;
		delay_ms(80);
	}
}
//HCHO阈值调节
void Set_HCHO_Threshold(void)
{
	Show_Str(250,65,300,24,(uint8_t *)SetUIL2_Text[7],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,95,300,24,(uint8_t *)SetUIL2_Text[8],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(250,135,300,24,(uint8_t *)SetUIL2_Text[9],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X08) == 0X08)
		Show_Str(330,95,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	else
		Show_Str(330,95,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //如果还在按着就继续等待
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus |= 1<<3;
			Show_Str(330,95,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus &= ~(1<<3);
			Show_Str(330,95,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_PRESSED)
			break;
		delay_ms(80);
	}
}
//CO2阈值调节
void Set_CO2_Threshold(void)
{
	Show_Str(250,95,300,24,(uint8_t *)SetUIL2_Text[8],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,135,300,24,(uint8_t *)SetUIL2_Text[9],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(250,170,300,24,(uint8_t *)SetUIL2_Text[10],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X04) == 0X04)
		Show_Str(330,135,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	else
		Show_Str(330,135,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //如果还在按着就继续等待
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus |= 1<<2;
			Show_Str(330,135,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus &= ~(1<<2);
			Show_Str(330,135,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_PRESSED)
			break;
		delay_ms(80);
	}
}

//温度阈值调节
void Set_Temp_Threshold(void)
{
	Show_Str(250,135,300,24,(uint8_t *)SetUIL2_Text[9],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,170,300,24,(uint8_t *)SetUIL2_Text[10],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(250,205,300,24,(uint8_t *)SetUIL2_Text[11],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X02) == 0X02)
		Show_Str(330,170,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	else
		Show_Str(330,170,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //如果还在按着就继续等待
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus |= 1<<1;
			Show_Str(330,170,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus &= ~(1<<1);
			Show_Str(330,170,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_PRESSED)
			break;
		delay_ms(80);
	}
}
//湿度阈值调节
void Set_Humi_Threshold(void)
{
	Show_Str(250,65,300,24,(uint8_t *)SetUIL2_Text[7],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,170,300,24,(uint8_t *)SetUIL2_Text[10],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,205,300,24,(uint8_t *)SetUIL2_Text[11],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X01) == 0X01)
		Show_Str(330,205,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	else
		Show_Str(330,205,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //如果还在按着就继续等待
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus |= 1<<0;
			Show_Str(330,205,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus &= ~(1<<0);
			Show_Str(330,205,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR); //退出后清楚显示 
			Update_Set_Info(3);
			break;
		}
		delay_ms(80);
	}
}
//调节PM2_5阈值
void Adjust_PM2_5_Val(void)
{
	uint8_t i;
	uint8_t buf[5];
	uint8_t *p = &SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus;  	//得到第一个地址
	LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	for(i=0;i<5;i++)
	{
		if(*p&(uint8_t)(1<<(4-i)))
		{
			if(i==0)
				Show_Str(250,65+i*35,300,24,(uint8_t *)SetUIL2_Text[7+i],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);   //加这个判断意在只对第一个加判断其余的不管
			else
				Show_Str(250,65+i*35,300,24,(uint8_t *)SetUIL2_Text[7+i],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		}
		else
			Show_Str(250,65+i*35,300,24,(uint8_t *)SetUIL2_Text[7+i],24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	}
	p = &SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus;
	for(i=0;i<5;i++)
	{
		if(*p&(uint8_t)(1<<(4-i)))
		{
			if(i==1)
			{
				sprintf(buf,"0.%02d",SysParaSetInfo.ThresHoldStatus.ThrldVal[i]);
				Show_Str(360,65+i*35,100,24,buf,24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			}
			else
			{
				if(i == 0)
					LCD_ShowNum(360,65+i*35,SysParaSetInfo.ThresHoldStatus.ThrldVal[i],4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
				else
					LCD_ShowNum(360,65+i*35,SysParaSetInfo.ThresHoldStatus.ThrldVal[i],4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			}
		}
		else
		{
			if(i == 1)
			{
				sprintf(buf,"0.%02d",SysParaSetInfo.ThresHoldStatus.ThrldVal[i]);
				Show_Str(360,65+i*35,100,24,buf,24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			}
			else
				LCD_ShowNum(360,65+i*35,SysParaSetInfo.ThresHoldStatus.ThrldVal[i],4,24,1,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		}
	}
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //如果还在按着就继续等待
	delay_ms(400);
	while(1)
	{
		if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0x10) == 0x10)
		{
			if(Encoder_Type == ENCODER_FORWORD)
			{
				SysParaSetInfo.ThresHoldStatus.ThrldVal[PM2_5_ID] +=2;	
				if(SysParaSetInfo.ThresHoldStatus.ThrldVal[PM2_5_ID]>=500)
					SysParaSetInfo.ThresHoldStatus.ThrldVal[PM2_5_ID] = 2;
				LCD_ShowNum(360,65,SysParaSetInfo.ThresHoldStatus.ThrldVal[PM2_5_ID],4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
				Encoder_Type = ENCODER_VOID;
			}
			else if(Encoder_Type == ENCODER_BACKWORD)
			{
				SysParaSetInfo.ThresHoldStatus.ThrldVal[PM2_5_ID] -=2;
				if(SysParaSetInfo.ThresHoldStatus.ThrldVal[PM2_5_ID] <= 2)
					SysParaSetInfo.ThresHoldStatus.ThrldVal[PM2_5_ID] = 500;
				LCD_ShowNum(360,65,SysParaSetInfo.ThresHoldStatus.ThrldVal[PM2_5_ID],4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
				Encoder_Type = ENCODER_VOID;
			}
			else if(Encoder_Type == ENCODER_PRESSED)
			{
				break;
			}
		}
		else   //如果没有使能开关的话直接跳出
		{
			Encoder_Type = ENCODER_PRESSED;
			break;
		}
		delay_ms(80);
	}
}
//调节HCHO阈值
void Adjust_HCHO_Val(void)
{
	uint8_t buf[10];
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0x10) == 0x10)
	{
		Show_Str(250,65,300,24,(uint8_t *)SetUIL2_Text[7],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		LCD_ShowNum(360,65,SysParaSetInfo.ThresHoldStatus.ThrldVal[PM2_5_ID],4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	}
	else
	{
		Show_Str(250,65,300,24,(uint8_t *)SetUIL2_Text[7],24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		LCD_ShowNum(360,65,SysParaSetInfo.ThresHoldStatus.ThrldVal[PM2_5_ID],4,24,1,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	}
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0x08) == 0x08)
	{
		Show_Str(250,100,300,24,(uint8_t *)SetUIL2_Text[8],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		sprintf(buf,"0.%02d",SysParaSetInfo.ThresHoldStatus.ThrldVal[HCHO_ID]);
		Show_Str(360,100,100,24,buf,24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	}
	else
	{
		//Show_Str(250,100,300,24,(uint8_t *)SetUIL2_Text[8],24,0,GRAY,BLACK);
		//LCD_ShowNum(360,100,SysParaSetInfo.ThresHoldStatus.ThrldVal[HCHO_ID],4,24,1,GRAY,BLACK);
		Encoder_Type = ENCODER_PRESSED;
		if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0x04)==0x04||(SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0x02)==0x02||(SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0x01)==0x01)
			return;  //接下来的都还有必要设置故不清除屏幕内容
		else
			LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		return;
	}
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //如果还在按着就继续等待
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysParaSetInfo.ThresHoldStatus.ThrldVal[HCHO_ID] ++;	
			if(SysParaSetInfo.ThresHoldStatus.ThrldVal[HCHO_ID]>=20)
				SysParaSetInfo.ThresHoldStatus.ThrldVal[HCHO_ID] = 1;
			sprintf(buf,"0.%02d",SysParaSetInfo.ThresHoldStatus.ThrldVal[HCHO_ID]);
			Show_Str(360,100,100,24,buf,24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			//LCD_ShowNum(360,100,SysParaSetInfo.ThresHoldStatus.ThrldVal[HCHO_ID],4,24,1,GREEN,BLUE);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysParaSetInfo.ThresHoldStatus.ThrldVal[HCHO_ID] --;
			if(SysParaSetInfo.ThresHoldStatus.ThrldVal[HCHO_ID] <= 1)
				SysParaSetInfo.ThresHoldStatus.ThrldVal[HCHO_ID] = 20;
			sprintf(buf,"0.%02d",SysParaSetInfo.ThresHoldStatus.ThrldVal[HCHO_ID]);
			Show_Str(360,100,100,24,buf,24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{			
			
			break;
		}
		delay_ms(80);
	}
}
//调节CO2阈值
void Adjust_CO2_Val(void)
{
	uint8_t buf[10];
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0x08) == 0x08)
	{
		Show_Str(250,100,300,24,(uint8_t *)SetUIL2_Text[8],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		sprintf(buf,"0.%02d",SysParaSetInfo.ThresHoldStatus.ThrldVal[HCHO_ID]);
		Show_Str(360,100,100,24,buf,24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	}
	else
	{
		Show_Str(250,100,300,24,(uint8_t *)SetUIL2_Text[8],24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		sprintf(buf,"0.%02d",SysParaSetInfo.ThresHoldStatus.ThrldVal[HCHO_ID]);
		Show_Str(360,100,100,24,buf,24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	}
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0x04) == 0x04)
	{
		Show_Str(250,135,300,24,(uint8_t *)SetUIL2_Text[9],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		LCD_ShowNum(360,135,SysParaSetInfo.ThresHoldStatus.ThrldVal[CARBON_ID],4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	}
	else
	{
		//Show_Str(250,135,300,24,(uint8_t *)SetUIL2_Text[9],24,0,GRAY,BLACK);
		//LCD_ShowNum(360,135,SysParaSetInfo.ThresHoldStatus.ThrldVal[CARBON_ID],4,24,1,GRAY,BLACK);
		Encoder_Type = ENCODER_PRESSED;
		if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0x02)==0x02||(SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0x01)==0x01)
			return;  //接下来的都还有必要设置故不清除屏幕内容
		else
			LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		return;
	}
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //如果还在按着就继续等待
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysParaSetInfo.ThresHoldStatus.ThrldVal[CARBON_ID] +=10;	
			if(SysParaSetInfo.ThresHoldStatus.ThrldVal[CARBON_ID]>=2000)
				SysParaSetInfo.ThresHoldStatus.ThrldVal[CARBON_ID] = 2000;
			LCD_ShowNum(360,135,SysParaSetInfo.ThresHoldStatus.ThrldVal[CARBON_ID],4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysParaSetInfo.ThresHoldStatus.ThrldVal[CARBON_ID] -=10;
			if(SysParaSetInfo.ThresHoldStatus.ThrldVal[CARBON_ID] <= 10)
				SysParaSetInfo.ThresHoldStatus.ThrldVal[CARBON_ID] = 10;
			LCD_ShowNum(360,135,SysParaSetInfo.ThresHoldStatus.ThrldVal[CARBON_ID],4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{			
			break;
		}
		delay_ms(80);
	}
}
//调节温度阈值
void Adjust_Temp_Val(void)
{
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0x04) == 0x04)
	{
		Show_Str(250,135,300,24,(uint8_t *)SetUIL2_Text[9],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		LCD_ShowNum(360,135,SysParaSetInfo.ThresHoldStatus.ThrldVal[CARBON_ID],4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	}
	else
	{
		Show_Str(250,135,300,24,(uint8_t *)SetUIL2_Text[9],24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		LCD_ShowNum(360,135,SysParaSetInfo.ThresHoldStatus.ThrldVal[CARBON_ID],4,24,1,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	}
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0x02) == 0x02)
	{
		Show_Str(250,170,300,24,(uint8_t *)SetUIL2_Text[10],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		LCD_ShowNum(360,170,SysParaSetInfo.ThresHoldStatus.ThrldVal[TEMP_ID],4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	}
	else
	{
		//Show_Str(250,170,300,24,(uint8_t *)SetUIL2_Text[10],24,0,GRAY,BLACK);
		//LCD_ShowNum(360,170,SysParaSetInfo.ThresHoldStatus.ThrldVal[TEMP_ID],4,24,1,GRAY,BLACK);
		Encoder_Type = ENCODER_PRESSED;
		if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0x01) == 0x01)
			return;  //接下来的都还有必要设置故不清除屏幕内容
		else
			LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		return;
	}
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //如果还在按着就继续等待
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysParaSetInfo.ThresHoldStatus.ThrldVal[TEMP_ID] ++;	
			if(SysParaSetInfo.ThresHoldStatus.ThrldVal[TEMP_ID]>=100)
				SysParaSetInfo.ThresHoldStatus.ThrldVal[TEMP_ID] = 1;
			LCD_ShowNum(360,170,SysParaSetInfo.ThresHoldStatus.ThrldVal[TEMP_ID],4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysParaSetInfo.ThresHoldStatus.ThrldVal[TEMP_ID] --;
			if(SysParaSetInfo.ThresHoldStatus.ThrldVal[TEMP_ID] <= 1)
				SysParaSetInfo.ThresHoldStatus.ThrldVal[TEMP_ID] = 100;
			LCD_ShowNum(360,170,SysParaSetInfo.ThresHoldStatus.ThrldVal[TEMP_ID],4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{			
			break;
		}
		delay_ms(80);
	}
}
//调节湿度阈值
void Adjust_Humi_Val(void)
{
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0x02) == 0x02)
	{
		Show_Str(250,170,300,24,(uint8_t *)SetUIL2_Text[10],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		LCD_ShowNum(360,170,SysParaSetInfo.ThresHoldStatus.ThrldVal[TEMP_ID],4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	}
	else
	{
		Show_Str(250,170,300,24,(uint8_t *)SetUIL2_Text[10],24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		LCD_ShowNum(360,170,SysParaSetInfo.ThresHoldStatus.ThrldVal[TEMP_ID],4,24,1,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	}
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0x01) == 0x01)
	{
		Show_Str(250,205,300,24,(uint8_t *)SetUIL2_Text[11],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		LCD_ShowNum(360,205,SysParaSetInfo.ThresHoldStatus.ThrldVal[HUMI_ID],4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	}
	else
	{
		//Show_Str(250,205,300,24,(uint8_t *)SetUIL2_Text[11],24,0,GRAY,BLACK);
		//LCD_ShowNum(360,205,SysParaSetInfo.ThresHoldStatus.ThrldVal[HUMI_ID],4,24,1,GRAY,BLACK);
		Encoder_Type = ENCODER_PRESSED;
		if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0x01) == 0x01)
			return;  //接下来的都还有必要设置故不清除屏幕内容
		else
			LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		return;
	}
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //如果还在按着就继续等待
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysParaSetInfo.ThresHoldStatus.ThrldVal[HUMI_ID] ++;	
			if(SysParaSetInfo.ThresHoldStatus.ThrldVal[HUMI_ID]>=100)
				SysParaSetInfo.ThresHoldStatus.ThrldVal[HUMI_ID] = 1;
			LCD_ShowNum(360,205,SysParaSetInfo.ThresHoldStatus.ThrldVal[HUMI_ID],4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysParaSetInfo.ThresHoldStatus.ThrldVal[HUMI_ID] --;
			if(SysParaSetInfo.ThresHoldStatus.ThrldVal[HUMI_ID] <= 1)
				SysParaSetInfo.ThresHoldStatus.ThrldVal[HUMI_ID] = 100;
			LCD_ShowNum(360,205,SysParaSetInfo.ThresHoldStatus.ThrldVal[HUMI_ID],4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			Update_Set_Info(3);	//开关状态更新更新			
			break;
		}
		delay_ms(80);
	}
}
//自动待机开关设置
void SetLay2UI_AutoStandby(void)
{
	//LCD_Fill(290,65,400,250,BLACK);
	if(SysParaSetInfo.AutoStandbyStatus)
		Show_Str(360,65,300,24,"打开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	else
		Show_Str(360,65,300,24,"关闭",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.AutoStandbyStatus = TRUE;
			Show_Str(360,65,300,24,"打开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.AutoStandbyStatus = FALSE;
			Show_Str(360,65,300,24,"关闭",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			if(SysParaSetInfo.AutoStandbyStatus)
				Show_Str(360,65,300,24,"打开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			else
				Show_Str(360,65,300,24,"关闭",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			//Show_Str(290,170,300,24,"已设置",24,0,GREEN,BLACK);	
			break;
		}
		delay_ms(10);
	}
}
//界面切换开关设置
void SetLay2UI_InterfaceSwitch(void)
{
	//LCD_Fill(290,65,400,250,BLACK);
	if(SysParaSetInfo.InterfaceSwitch)
		Show_Str(360,89,300,24,"打开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	else
		Show_Str(360,89,300,24,"关闭",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.InterfaceSwitch = TRUE;
			Show_Str(360,89,300,24,"打开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.InterfaceSwitch = FALSE;
			Show_Str(360,89,300,24,"关闭",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			//Show_Str(290,170,300,24,"已设置",24,0,GREEN,BLACK);	
			break;
		}
		delay_ms(10);
	}
}
//待机延时时间设置
void SetLay2UI_AutoStandbyTime(void)
{
	if(SysParaSetInfo.InterfaceSwitch)
		Show_Str(360,89,300,24,"打开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	else
		Show_Str(360,89,300,24,"关闭",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if(SysParaSetInfo.AutoStandbyStatus)
		LCD_ShowNum(360,113,SysParaSetInfo.AutoStandbyTime,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	else
	{
		Encoder_Type = ENCODER_PRESSED;
		return;
	}
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.AutoStandbyTime ++;
			if(SysParaSetInfo.AutoStandbyTime >= 10)
				SysParaSetInfo.AutoStandbyTime = 10;
			LCD_ShowNum(360,113,SysParaSetInfo.AutoStandbyTime,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.AutoStandbyTime --;
			if(SysParaSetInfo.AutoStandbyTime <= 1)
				SysParaSetInfo.AutoStandbyTime = 1;
			LCD_ShowNum(360,113,SysParaSetInfo.AutoStandbyTime,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			//这里写语音识别必要的代码
			break;
		}
		delay_ms(10);
	}
}
//设备开关设置
void SetLay2UI_DevicePower(void)
{
	if(SysParaSetInfo.Power_5V_Status)
		Show_Str(360,65,300,24,"已开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
	else
		Show_Str(360,65,300,24,"已关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	//LCD_ShowNum(300,170,SysParaSetInfo.Power_5V_Status,2,24,1,WHITE,BLUE);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.Power_5V_Status =1;
			Show_Str(360,65,300,24,"打开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);		
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.Power_5V_Status =0;
			Show_Str(360,65,300,24,"关闭",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			if(SysParaSetInfo.Power_5V_Status)
				Handle_5V_POWER_Status(1);
			else
				Handle_5V_POWER_Status(0);
			break;
		}
		delay_ms(10);
	}
}
//语音识别开关设置
void SetLay2UI_SpeechRecgSw(void)
{
	if(SysParaSetInfo.Power_5V_Status)
		Show_Str(360,65,300,24,"已开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	else
		Show_Str(360,65,300,24,"已关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if(SysParaSetInfo.SpeechRecognize.Speech_Sw)
		Show_Str(360,89,300,24,"已开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	else
		Show_Str(360,89,300,24,"已关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SpeechRecognize.Speech_Sw = 1;
			Show_Str(360,89,300,24,"打开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SpeechRecognize.Speech_Sw = 0;
			Show_Str(360,89,300,24,"关闭",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			//这里写语音识别必要的代码
			break;
		}
		delay_ms(10);
	}
}
//语音音量设置
void SetLay2UI_SpeechRecgVol(void)
{
	if(SysParaSetInfo.SpeechRecognize.Speech_Sw)
		Show_Str(360,89,300,24,"已开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	else
		Show_Str(360,89,300,24,"已关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if(SysParaSetInfo.SpeechRecognize.Speech_Sw)
		LCD_ShowNum(360,113,SysParaSetInfo.SpeechRecognize.Speech_Volume,2,24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	else
	{
		Encoder_Type = ENCODER_PRESSED;
		return;
	}
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SpeechRecognize.Speech_Volume ++;
			if(SysParaSetInfo.SpeechRecognize.Speech_Volume >= 3)
				SysParaSetInfo.SpeechRecognize.Speech_Volume = 3;
			LCD_ShowNum(360,113,SysParaSetInfo.SpeechRecognize.Speech_Volume,2,24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SpeechRecognize.Speech_Volume --;
			if(SysParaSetInfo.SpeechRecognize.Speech_Volume <= 1)
				SysParaSetInfo.SpeechRecognize.Speech_Volume = 1;
			LCD_ShowNum(360,113,SysParaSetInfo.SpeechRecognize.Speech_Volume,2,24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);			
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			//这里写语音识别必要的代码
			break;
		}
		delay_ms(10);
	}
}
//SD开始记录时间间隔设置
void SetLay2UI_SDRemUnit(void)
{
	//LCD_Fill(290,65,400,250,BLACK);
	LCD_ShowNum(365,65,SysParaSetInfo.SDRemTimeUnit,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	LCD_ShowNum(365,89,SysParaSetInfo.SensorSamplingTimeUnit,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SDRemTimeUnit ++;
			if(SysParaSetInfo.SDRemTimeUnit>=59)
				SysParaSetInfo.SDRemTimeUnit = 59;
			//Show_Str(290,170,300,24,"打开  ",24,0,WHITE,BLUE);	
			//LCD_ShowNum(360,205,SysParaSetInfo.SDRemTimeUnit,4,24,1,GRAY,BLACK);
			LCD_ShowNum(365,65,SysParaSetInfo.SDRemTimeUnit,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SDRemTimeUnit --;
			if(SysParaSetInfo.SDRemTimeUnit<=1 || SysParaSetInfo.SDRemTimeUnit>=59)
				SysParaSetInfo.SDRemTimeUnit = 1;
			//Show_Str(290,170,300,24,"关闭  ",24,0,GRAY,BLUE);
			LCD_ShowNum(365,65,SysParaSetInfo.SDRemTimeUnit,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);		
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			//Show_Str(250,170,300,24,"已设置",24,0,GREEN,BLACK);	
			break;
		}
		delay_ms(10);
	}
}
//服务器上传时间间隔设置
void SetLay2UI_SensoeSamplingUnit(void)
{
	//LCD_Fill(290,65,400,250,BLACK);
	LCD_ShowNum(365,65,SysParaSetInfo.SDRemTimeUnit,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(365,89,SysParaSetInfo.SensorSamplingTimeUnit,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SensorSamplingTimeUnit ++;
			if(SysParaSetInfo.SensorSamplingTimeUnit>=59 || SysParaSetInfo.SensorSamplingTimeUnit<0)
				SysParaSetInfo.SensorSamplingTimeUnit = 0;
			LCD_ShowNum(365,89,SysParaSetInfo.SensorSamplingTimeUnit,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SensorSamplingTimeUnit --;
			if(SysParaSetInfo.SensorSamplingTimeUnit <= 1)
				SysParaSetInfo.SensorSamplingTimeUnit = 1;
			//Show_Str(290,170,300,24,"关闭  ",24,0,GRAY,BLUE);
			LCD_ShowNum(365,89,SysParaSetInfo.SensorSamplingTimeUnit,2,24,1,WHITE,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);		
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			Show_Str(250,170,300,24,"已设置",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			break;
		}
		delay_ms(10);
	}
}
//设置阈值
void SetLay2UI_ThresHold(void)
{
	LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Encoder_Type = ENCODER_PRESSED;
	return;
	while(1)
	{
		if(Encoder_Type == ENCODER_PRESSED)
			break;
		delay_ms(10);
	}
}
//设置WIFI后是否使能连接WIFI状态
void LinkWifi_Enable(void)
{
	if((SysParaSetInfo.SensorSwitch&0X04) == 0X04)
	{
//		if(Esp8266Config.WifiLinkSuccFlag || !Esp8266InitFinishFlag)  //已经连接过WIFI了
//		{
//			if(Esp8266Config.WifiLinkSuccFlag)
//				Show_Str(300,200,300,24,"已连接  ",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
//			else if(!Esp8266InitFinishFlag)
//				Show_Str(300,200,300,24,"正在连接",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
//			Encoder_Type = ENCODER_PRESSED;
//			return;
//		}
//		else
			Esp8266_Config();
	}
	else if((SysParaSetInfo.SensorSwitch&0X04) != 0X04)
	{
		Esp8266_DisConectServer();  //断开TCP的连接
		Esp8266_ReStart();   //重启
		delay_ms(3000);
		Esp8266Config.WifiLinkSuccFlag = 0;
		Encoder_Type = ENCODER_PRESSED;
	}
	return;
}
//设置WIFI使能状态
void SetLay2UI_WifiStatus(void)
{
	Show_Str(250,170,300,24,"WIFI状态:",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if((SysParaSetInfo.SensorSwitch&0X04) == 0X04)
		Show_Str(360,170,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
	else 
		Show_Str(360,170,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(250,70,190,16,"设置后长按配置短按连接",16,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	DispWifiInfoFlag = 1;
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SensorSwitch |= 1<<2;  	 //置位
			Show_Str(360,170,300,24,"开",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SensorSwitch &= ~(1<<2);  //清除
			Show_Str(360,170,300,24,"关",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR); //退出后清楚显示 
			Show_Str(300,200,300,24,"设置中...",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			DispWifiInfoFlag = 1;
			break;
		}
		delay_ms(10);
	}
}
//操作界面主运行菜单
void MenuOperate(void)	
{
	if(!StandbyMode)
	{
		switch(Encoder_Type)
		{
			case  ENCODER_FORWORD: 	 //正转
			{
				EncoderFuncIndex = EncoderTab[EncoderFuncIndex].EncoderForwordStatus;  	//确认按键
				Encoder_Type = ENCODER_VOID;
			}break;
			case  ENCODER_BACKWORD:   //反转
			{
				EncoderFuncIndex = EncoderTab[EncoderFuncIndex].EncoderBackwordStatus;		//向上按键s2
				Encoder_Type = ENCODER_VOID;
			}break;
			case  ENCODER_PRESSED:    //按下
			{
				EncoderFuncIndex = EncoderTab[EncoderFuncIndex].EncoderPressStatus; 	//向下按键s3
				Encoder_Type = ENCODER_VOID;
			}break; 
			default : 
				break;
			//此处添加按键错误代码
		}
		if(AutoJumpFlag&&(Current_Show_Interface==Curr_Para_Show||
			Current_Show_Interface==Curr_PM2_5_Show||
			Current_Show_Interface==Curr_WaveForm_Show||
			Current_Show_Interface==Curr_Set_Show))   //用于主动回到主界面用
		{
			AutoJumpFlag = 0;
			EncoderFuncIndex = Para_Show;  //跳回主显示界面
		}
		EncoderFuncPtr = EncoderTab[EncoderFuncIndex].CurrentOperate; 
		(*EncoderFuncPtr)();//执行当前操作
	}
	else
	{
		delay_ms(200);
	}
}
//获得新的文件名
void Get_New_pname(uint8_t *pname)
{
	Rx8025tGetTime(&SysTimeData);
	sprintf((char*)pname,"0:SensorMemory/SensorData20%02d.%02d.%02d.%02d.%02d.%02d.txt",
		SysTimeData.Year,
		SysTimeData.Month,
		SysTimeData.Day,
		SysTimeData.Hour,
		SysTimeData.Minute,
		SysTimeData.Second);
}
//加载关机界面
void LoadPowerOffUI(uint8_t Mode)
{
	static uint8_t LoadFlag = 0;
	if(!LoadFlag)
	{
		LoadFlag = 1;
		LCD_Fill(145,145,330,205,RED);
		if(Mode == 1)
			Show_Str(175,165,200,24,"关机中...",24,0,BLACK,RED);
		else if(Mode == 0)
			Show_Str(175,165,200,24,"电量低...",24,0,BLACK,RED);
	}
	
}
//保存当前数据道SD卡
//往SD卡写一次所需要的字节数为13个Byte
void Write_Data_To_SDCard(const uint8_t *filename,uint8_t mode)
{
	UINT bw;
	FIL *F_Sensordata;
	uint8_t *databuf;
	uint8_t res;
	databuf = (uint8_t *)mymalloc(100);  //100个字节的空间
	if(databuf==NULL)
	{
		return;
	}
	F_Sensordata = (FIL *)mymalloc(sizeof(FIL));
	if(F_Sensordata==NULL)
	{
		myfree(databuf);
		return;
	}
	if(mode==1)
		res = f_open(F_Sensordata,(const TCHAR*)filename,FA_READ|FA_WRITE);  	//打开之前有的文件
	if(mode==0||res==0x04)  						//如果打开失败则创建新文件
	{
		Get_New_pname(pname);
		res = f_open(F_Sensordata,(const TCHAR*)filename,FA_WRITE|FA_CREATE_NEW);
	}
	if((SysParaSetInfo.SensorSwitch&0x02) != 0x02)  	//甲醛未使能
	{
		if((SysParaSetInfo.SensorSwitch&0x01) != 0x01)
			sprintf(databuf,"%02d:%02d %2d %02d%% %02d %d %d\r\n",SysTimeData.Hour,SysTimeData.Minute,SensorData.Temp,SensorData.Humi,SensorData.PMData.PM1_0_S,SensorData.PMData.PM2_5_S,SensorData.PMData.PM10_S);
		else if((SysParaSetInfo.SensorSwitch&0x01) == 0x01)
			sprintf(databuf,"%02d:%02d %2d %02d%% %02d %d %d\r\n",SysTimeData.Hour,SysTimeData.Minute,SensorData.Temp,SensorData.Humi,SensorData.PMData.PM1_0_S,SensorData.PMData.PM2_5_S,SensorData.Carbon);
	}
	else if((SysParaSetInfo.SensorSwitch&0x02) == 0x02)
	{
		if((SysParaSetInfo.SensorSwitch&0x01) != 0x01)
			sprintf(databuf,"%02d:%02d %2d %02d%% %02d %d %d\r\n",SysTimeData.Hour,SysTimeData.Minute,SensorData.Temp,SensorData.Humi,SensorData.HCHO,SensorData.PMData.PM2_5_S,SensorData.PMData.PM10_S);
		else if((SysParaSetInfo.SensorSwitch&0x01) == 0x01)
			sprintf(databuf,"%02d:%02d %2d %02d%% %02d %d %d\r\n",SysTimeData.Hour,SysTimeData.Minute,SensorData.Temp,SensorData.Humi,SensorData.HCHO,SensorData.PMData.PM2_5_S,SensorData.Carbon);
	}
	if(res==FR_OK)  //如果创建成功的话则开始写入数据
	{
		f_lseek(F_Sensordata,SD_Write_Times*25);
		res = f_write(F_Sensordata,databuf,sizeof(SensorType),(UINT *)&bw); 
		SD_Write_Times++;
		f_close(F_Sensordata);
	}
	myfree(databuf);
	myfree(F_Sensordata);
}
/*
uint16_t Testbuf[10]={12345,56789,23546,65519,58754,25345,61257,25643,23453,45332};
//SD卡读写测试.单,双字节下已经测试成功
void SDWiriteDataTest(uint8_t *filename,uint8_t mode)
{
	UINT bw;
	FIL *F_testdata;
	uint16_t *databuf;
	uint8_t res;
	databuf = (uint16_t *)mymalloc(sizeof(Testbuf));  //申请50个字节的空间
	if(databuf==NULL)
	{
		Show_Str(0,100,300,16,"内存申请失败",16,0,GREEN,BLACK);
		return;
	}
	databuf = Testbuf;
	F_testdata = (FIL *)mymalloc(sizeof(FIL));
	if(F_testdata==NULL)
	{
		myfree(databuf);
		return;
	}
	if(mode==1)
		res = f_open(F_testdata,(const TCHAR*)filename,FA_READ|FA_WRITE);  	//打开之前有的文件
	if(mode==0||res==0x04)  //如果打开失败则创建新文件
		res = f_open(F_testdata,(const TCHAR*)filename,FA_WRITE|FA_CREATE_NEW);
	//f_sync(F_testdata);
	if(res==FR_OK)
	{
		res = f_write(F_testdata,databuf,sizeof(Testbuf),(UINT *)&bw); 
		f_close(F_testdata);
	}
	myfree(databuf);
	myfree(F_testdata);
}
void SDReadDataTest(uint8_t *filename)
{
	uint8_t i;
	UINT br;
	FIL *F_testdata;
	uint16_t *databuf;
	uint8_t res;
	databuf = (uint16_t *)mymalloc(sizeof(Testbuf));  //申请50个字节的空间
	if(databuf==NULL)
	{
		Show_Str(0,100,300,16,"内存申请失败",16,0,GREEN,BLACK);
		return;
	}
	F_testdata = (FIL *)mymalloc(sizeof(FIL));
	if(F_testdata==NULL)
	{
		myfree(databuf);
		return;
	}
	res=f_open(F_testdata,(const TCHAR*)filename,FA_READ);
	if(res==0)
	{
		f_read(F_testdata,databuf,sizeof(Testbuf),(UINT *)&br);
		for(i=0;i<10;i++)
		{
			LCD_ShowNum(0,i*18,databuf[i],6,16,1,GREEN,BLACK);//LCD_ShowNum(uint16_t x,uint16_t y,u32 num,uint8_t len,uint8_t size);
		}
		f_close(F_testdata);
	}
	myfree(databuf);
	myfree(F_testdata);	
}
void TestSDCard(void)
{
	SDWiriteDataTest("0:/guchenglong.txt",1);
	SDReadDataTest("0:/guchenglong.txt");
	Show_Str(0,200,300,16,"完成",16,0,GREEN,BLACK);
}
*/


