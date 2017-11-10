#include "include.h"
//#include <locale.h>
//��������
const float USASt_Tab[7][4]={
	{0,15.4,0,50},
	{15.5,40.4,51,100},
	{40.5,65.4,101,150},
	{60.5,150.4,151,200},
	{150.5,250.4,201,300},
	{250.5,350.4,301,400},
	{350.5,500.4,401,500}
};
//״̬���ܽṹ��
const StatusTab EncoderTab[MENU_SIZE] =
{      //����           ����          ˳            ��                  ����ָ��
	{Para_Show,     Para_Show,    PM2_5_Show,   Set_Show,     (*Load_ParaShow_Interface_1)},  	//����1�����в�����ʾ����
	{PM2_5_Show,    PM2_5_Show,   WaveForm_Show,Para_Show,    (*Load_PMCnctrShow_Interface_2)},	//����2����������ʾ����
	{WaveForm_Show, WaveForm_Show,Set_Show,     PM2_5_Show,   (*Load_WaveformShow_Interface_3)}, //����3��������ʾ����
	{Set_Show,      Set_Prar_Show,Para_Show,    WaveForm_Show,(*Load_SetShow_Interface_4)},      //����4���������ý���
	
	{Set_Prar_Show,11,5,10,(*SetLay1UI_Time)},  			//����4�����ý���
	{5,16,6,4,(*SetLay1UI_AutoStandby)},
	{6,17,7,5,(*SetLay1UI_DeviceSW)},
	{7,18,8,6,(*SetLay1UI_DataRemUnit)},
	{8,19,9,7,(*SetLay1UI_Threshold)},
	{9,39,10,8,(*SetLay1UI_WIFIConfig)},
	{10,0,4,9,(*SetLay1UI_Quit)},
	//��������
	{11,12,0,0,(*SetLay2UI_Time_Year)}, 	//������
	{12,13,0,0,(*SetLay2UI_Time_Month)}, 	//������
	{13,14,0,0,(*SetLay2UI_Time_Day)},		//��������
	{14,15,0,0,(*SetLay2UI_Time_Hour)},		//����Сʱ
	{15,Set_Prar_Show,0,0,(*SetLay2UI_Time_Minute)},  //���÷��ӣ���ɺ������һ��˵���
	
	{16,34,0,0, (*SetLay2UI_AutoStandby)},  	//������󷵻ص���һ��ԭ����
	{17,40,0,0, (*SetLay2UI_DevicePower)},
	{18,35,0,0, (*SetLay2UI_SDRemUnit)},
	{19,26,0,0, (*SetLay2UI_ThresHold)},
	
	//��ֵ����
	{20,21,0,0,(*Set_PM2_5_Threshold)},
	{21,22,0,0,(*Set_HCHO_Threshold)},
	{22,23,0,0,(*Set_CO2_Threshold)},
	{23,24,0,0,(*Set_Temp_Threshold)},
	{24,27,0,0,(*Set_Humi_Threshold)},  				//����UI3������ֵ��Χ״̬��
	
	{25,9,0,0,(*LinkWifi_Enable)},		

	{26,20,27,36,(*SetLay3UI_ThresHold_Status)},  		//��ֵ״̬ģʽ
	{27,28,36,26,(*SetLay3UI_ThresHold_Adjust)},		//��ֵ��Χ����
	
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
//��������
uint8_t  EncoderFuncIndex = 0;    		//��ŵ�ǰ�Ĳ˵�����
void (*EncoderFuncPtr)(void);            //����Ҫ��ת�ĺ���ָ��
extern uint8_t CloseMachineFlag;
SysSetPara_TypeDef SysParaSetInfo;   		//ϵͳ���ò�����Ϣ
SensorOnlineStat_TypeDef SensorOnline;      //����������״̬
int16_t BackLightBright = 0;
DataRemember_TypeDef DataRemember_Hour;

static uint32_t SD_Write_Times = 0;  //дsd��ʱ�ı�����
uint8_t StandbyMode = 0;
uint8_t pname[50] = "0:/SensorMemory/SensorData.txt";   //�����洢�´������ļ���
uint8_t AutoJumpFlag = 0;  //�Զ���ת����������־
Interface_TypeDef Current_Show_Interface = Curr_No_Show;  //��ǰ��ʾ����
static uint8_t LoadStaticUIFlag = 0;
uint8_t DispWifiInfoFlag = 0;    //��ʾWIFI����Ϣ��־
uint16_t AQI_Value;

extern const unsigned char CloudUpload[106];
extern const unsigned char NoCloudUpload[106];
extern const unsigned char TempLogo[];
extern const unsigned char HumiLogo[];
extern const unsigned char AirPressLogo[];
const uint8_t Item_Id[7] = {HCHO_ID,PM2_5_ID,CARBON_ID,TEMP_ID,HUMI_ID,AIRPRES_ID};
const uint16_t QualityGradeColor[] = {C_LIME_GREEN,YELLOW,ORANGE,RED,PURPLE,MAROON};
uint8_t const *QualityGrade[]={"��","��","��","��","��","��","AQI"};
uint8_t const *SetUIL1_Text[] = {"1.ϵͳʱ������>","2.�Զ���������>","3.�豸��������>","4.ʱ���������>","5.״̬��������>","6.WIFI��������>","7.�˳�"};
uint8_t const *SetUIL2_Text[] = {"��","��","��","ʱ","��","��","�ر�","PM2_5","HCHO","CO2","�¶�","ʪ��","PM1.0","PM10"};                                                                                                      
uint8_t* const AirQuiyGrade[8][6] =
{
	{"PM2.5:��  ","PM2.5:��  ","�����Ⱦ","�ж���Ⱦ","�ض���Ⱦ","������Ⱦ"},
	{"��ȩ����","��ȩ�ϸ�","��ȩ����"},
	{"  ������  ","CO2����","CO2�ϸ�","CO2����"},
	{"��С����:��","��С����:��","��С����:��","��С����:��","��С����:��","��С����:��"},
	{"�ϴ����:��","�ϴ����:��","�ϴ����:��","�ϴ����:��","�ϴ����:��","�ϴ����:��"},
	{"�¶�����,","�¶�ƫ��,","����Ԥ��,"},
	{"ʪ������!","����ƫ��!","��������!"},
	{"������"}
};
static uint16_t CurrThemeColorFlag[3] = {0,0,0};  //�������ⱳ������ɫ
static uint16_t PreThemeColorFlag[3] = {0,0,0};
uint8_t AlarmCloseFlag = 0;   //���ӹرձ�־,�ֱ�رն�Ӧ���ݵ����ӣ�ֻ�رյ�ǰ�ģ����������
const uint16_t ValueTab[] = {0,35,75,115,150,250,500};//ɫ���ϵ���ֵ
uint8_t Draw_Wave_Flag = 0;  //����ÿ5���Ӽ�¼�Ĳ��η�ֹÿ�ζ�����ȫ��

const DispCirDef AllDispCirInfo[6]=
{      //���ﳤ�Ϳ�����Բ����Բ����
	{15, 64,103, 105,27,90,  {RED,DARKBLUE,   RED,  PARA_SHOW_INTERFACE_BACKCOLOR,   DARKBLUE,FillColor},	 "PM2.5","ug/m3",60},	
	{163,64,103, 105,27,90,  {RED,GRAYBLUE,   RED,  PARA_SHOW_INTERFACE_BACKCOLOR,   DARKBLUE,FillColor},    "��ȩ","mg/m3",60},
	{312,64, 103,105,32,90,  {RED,GRAYBLUE,   RED,  PARA_SHOW_INTERFACE_BACKCOLOR,   DARKBLUE,FillColor},    "CO2","ppm",60},	
	
	{65,235, 30,27,22,  23,  {WHITE,GRAYBLUE, C_LIME_GREEN,C_LIME_GREEN, PARA_SHOW_INTERFACE_BACKCOLOR,PARA_SHOW_INTERFACE_BACKCOLOR},  "�¶�",     "��",36},
	{215,235, 30,27,27,  23,  {WHITE,GRAYBLUE, C_LIME_GREEN,C_LIME_GREEN, PARA_SHOW_INTERFACE_BACKCOLOR,PARA_SHOW_INTERFACE_BACKCOLOR},  "ʪ��",     "%",36},	
	{340,235, 30,27,45, 23,  {WHITE,GRAYBLUE, C_LIME_GREEN,C_LIME_GREEN, PARA_SHOW_INTERFACE_BACKCOLOR,PARA_SHOW_INTERFACE_BACKCOLOR},  "   ��ѹ�뺣��",    "KPa",24},		
};
//﮵�ص�ѹ��ѹ��ADCֵ��ת��
const uint16_t Vol_To_ADC[13]=
{
	2332,3263,3425,3449,3474,
	3511,3524,3561,3598,3660,
	3722,3797,3908
};
//���Ѵ����Ļ����Ӵ�����
//void aWake_Standby(void)
//{
//	if(SysParaSetInfo.AutoStandbyStatus)  //�����ѿ����˵Ļ�
//	{
//		TIM2->CCR2 = BackLightBright;  	 	//������ʾ����ʾ
//		Set_PMS5003S_Status(PMS_RUNING);  //����PM2.5������
//		OSTmrStart(tmr2,&err);	   //���������ʱ��
//	}
//	Encoder_Type = ENCODER_VOID;
//}
//���ڱ�������
void Adjust_Back_Light(void)
{
	int16_t Temp;
	Show_Str(240,300,100,12,"���ȵ���",12,0,LIGHTBLUE,BLACK);
	Temp = TIM2->CCR2;
	delay_ms(300);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)  //����
		{
			Encoder_Type = ENCODER_VOID;
			Temp -=50;
			if(Temp<0)
				Temp = 0;
			TIM2->CCR2 = (uint16_t)Temp;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)//����
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
			break;  //����
		}
	}
}
//�����������е����ֵ
uint16_t MAX(uint16_t val1,uint16_t val2)
{
	return (val1>val2)?val1:val2;
}
//�����������е���Сֵ
uint16_t MIN(uint16_t val1,uint16_t val2)
{
	return (val1>val2)?val2:val1;	
}
//�������õĲ���
void WriteSetPara(void)
{
	uint16_t DataLen;
	uint8_t *p = &SysParaSetInfo.AutoStandbyStatus;
	DataLen = sizeof(SysSetPara_TypeDef);
//	OS_CPU_SR cpu_sr=0;
//	OS_ENTER_CRITICAL();	//�����ٽ���(�޷����жϴ��) 
	AT24CXX_Write(EEPROM_ADDR_SYS_PARA_SETPARA,p,DataLen);   //д�����
	AT24CXX_WriteOneByte(EEPROM_ADDR_SYS_PARA_CONIFG_FLAG,0XA5);
//	OS_EXIT_CRITICAL();		//�˳��ٽ���(���Ա��жϴ��)
}
//��ȡEEPROM��������õ�����
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
	ConfigFlag = AT24CXX_ReadOneByte(EEPROM_ADDR_SYS_PARA_CONIFG_FLAG);  //��ȡ�Ƿ����ñ�־
	if(ConfigFlag == 0XA5)   //���ǵ�һ������
	{
		TempData = AT24CXX_ReadOneByte(EEPROM_ADDR_SYS_PARA_BACKLIGHT/*250*/);   //��ȡ������ʾ��PWM��ֵ
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
		//��ϵͳ�������ý��濪ʼ��ȡ
	}
	else //�ǵ�һ������
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
		Handle_5V_POWER_Status(1);  		//��5V��������Դ
	else
		Handle_5V_POWER_Status(0);
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14) == 0)
	{
		LoadParaToEeprom();
		while(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14));
		Encoder_Type = ENCODER_VOID;
	}
	Rx8025tGetTime(&SysTimeData);  			//��ȡϵͳʱ��
	if(SysParaSetInfo.SpeechRecognize.Speech_Sw != 1)
	{
		DisableLD3320NVIC();
	}
	res = f_opendir(&dir,"0:/SensorMemory");
	if(res == FR_OK)
		return;
	else
		f_mkdir("0:/SensorMemory");  		//����Ŀ¼
}
//
//����һ��ɫ��
//
void LoadColorPat(void)
{ 
	uint16_t i=0;
	//uint16_t color;
	//uint16_t Ri=0,Gi=0,Bi=0;
	uint16_t ystep = (lcddev.height-53)/6;  				//�ı�ɫ�׵ĳ��ȸ�����
	Show_Str(CURSOR-21,270,18,16,(uint8_t*)QualityGrade[0],16,0,GREEN,FillColor);  //���
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
//���ƶ�̬�ĺ�ɫָʾ��
void DrawDynamicCir(uint16_t AQIValue,uint16_t color)
{
	uint8_t static PreStage = 6;
	uint16_t ystep = (lcddev.height-53)/6;
	if(AQIValue<=50 && AQIValue >=0 && PreStage!=1)  //�ȼ�1
	{
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep*(PreStage-1),CURSOR+2,lcddev.height-30-ystep*PreStage+1,FillColor);
		LCD_Fill(CURSOR-3,lcddev.height-30,CURSOR+2,lcddev.height-30-ystep+1,color);
		PreStage = 1;
	}
	else if(AQIValue<=100 && AQIValue>50 && PreStage!=2)//�ȼ�2
	{
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep*(PreStage-1),CURSOR+2,lcddev.height-30-ystep*PreStage+1,FillColor);
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep,CURSOR+2,lcddev.height-30-ystep*2+1,color);
		PreStage = 2;
	}
	else if(AQIValue<=150 && AQIValue>100 && PreStage!=3)//�ȼ�3
	{
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep*(PreStage-1),CURSOR+2,lcddev.height-30-ystep*PreStage+1,FillColor);
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep*2,CURSOR+2,lcddev.height-30-ystep*3+1,color);
		PreStage = 3;
	}
	else if(AQIValue<=200 && AQIValue>150 && PreStage!=4)//�ȼ�4
	{
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep*(PreStage-1),CURSOR+2,lcddev.height-30-ystep*PreStage+1,FillColor);
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep*3,CURSOR+2,lcddev.height-30-ystep*4+1,color);
		PreStage = 4;
	}
	else if(AQIValue<=300 && AQIValue>200 && PreStage!=5)//�ȼ�5
	{
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep*(PreStage-1),CURSOR+2,lcddev.height-30-ystep*PreStage+1,FillColor);
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep*4,CURSOR+2,lcddev.height-30-ystep*5+1,color);
		PreStage = 5;
	}
	else if(AQIValue>300 && PreStage!=6)//�ȼ�6
	{
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep*(PreStage-1),CURSOR+2,lcddev.height-30-ystep*PreStage+1,FillColor);
		LCD_Fill(CURSOR-3,lcddev.height-30-ystep*5,CURSOR+2,lcddev.height-30-ystep*6+1,color);
		PreStage = 6;
	}
}
//������ɫ
uint16_t CalColorValue(uint16_t Value,uint8_t IdItem)  //QualityGradeColor[]
{
	//uint16_t R,G,B;
	uint16_t RGB = 0x0000;
	switch(IdItem)
	{
		case HCHO_ID:  //��ȩ
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
	RGB |=(unsigned long) G*1000/4047<<5;	  //ת������Ļ��4Wɫ��ʾ
	RGB |=(unsigned long) B*1000/8225;*/
	return RGB;
}
//����AQI��ֵ
void Cal_USASt_Value(uint16_t Cvalue,uint16_t *AQI_Value,uint16_t *USAST_Value)
{
	uint16_t AQIValue;
	float Ihigh,Ilow,Chigh,Clow;
	//AQIValue = Cvalue/1000;  ��������
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
	//����AQI
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
//����״̬����
//�����ȼ�����
void AlarmStatusCheck(void)
{
	if(SysParaSetInfo.Power_5V_Status)
	{
		if(SensorData.HCHO>SysParaSetInfo.ThresHoldStatus.ThrldVal[HCHO_ID] && (SysParaSetInfo.SensorSwitch&0x02) == 0x02)
		{
			if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X08) == 0X08)
			{
				AlarmCloseFlag = 1;
				//SetBeepFreq(AlarmFreqTab[9],1);  //�����滻�ɶ�Ӧ�Ĳ���MP3���־Ϳ�����
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
//��������жϣ������������϶�����ʾ AQIȡֵ��Χ(0-500)
void EnvirQuaty(uint16_t x,uint16_t y)
{
	//uint16_t RGB;
	uint16_t USASt_Value;   //����ֵ
    Cal_USASt_Value(SensorData.PMData.PM2_5_S,&AQI_Value,&USASt_Value);
	Show_Str(80,3,300,24,"AQI:",24,0,AQI_COLOR,TOP_TITLE_BACK_COLOR);
	Show_Str(165,3,300,24,"����:",24,0,AQI_COLOR,TOP_TITLE_BACK_COLOR);
    LCD_ShowNum(128,3,AQI_Value,3,24,0,AQI_COLOR,TOP_TITLE_BACK_COLOR);
	LCD_ShowNum(223,3,USASt_Value,3,24,0,AQI_COLOR,TOP_TITLE_BACK_COLOR);
	if(SysParaSetInfo.Power_5V_Status)
		DrawDynamicCir(AQI_Value,WHITE);
	else
		DrawDynamicCir(AQI_Value,FillColor);
	AlarmStatusCheck();
	if(Current_Show_Interface == Curr_Para_Show)  //ֻ��������ʾ�����²���ʾ
	{
		if(SysParaSetInfo.Power_5V_Status)
		{
			//RGB = CalColorValue(SensorData.PMData.PM2_5_S,PM2_5_ID);
			if(SensorOnline.Device.PMS_5 == 1)
			{
				if(AQI_Value<=50 && AQI_Value>=0)  //�ȼ�1
				{
					Show_Str(AllDispCirInfo[PM2_5_ID].x0+20,y,200,16,AirQuiyGrade[PM2_5_ID][0],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X02)!=0X02)
						Show_Str(AllDispCirInfo[HCHO_ID].x0+10,y,200,16,AirQuiyGrade[3][0],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X01)!=0X01)
						Show_Str(AllDispCirInfo[CARBON_ID].x0+10,y,200,16,AirQuiyGrade[4][0],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
				}
				else if(AQI_Value>50 && AQI_Value<=100)//�ȼ�2
				{
					Show_Str(AllDispCirInfo[PM2_5_ID].x0+20,y,200,16,AirQuiyGrade[PM2_5_ID][1],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X02)!=0X02)
						Show_Str(AllDispCirInfo[HCHO_ID].x0+10,y,200,16,AirQuiyGrade[3][1],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X01)!=0X01)
						Show_Str(AllDispCirInfo[CARBON_ID].x0+10,y,200,16,AirQuiyGrade[4][1],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
				}
				else if(AQI_Value>=100 && AQI_Value<=150)//�ȼ�3
				{
					Show_Str(AllDispCirInfo[PM2_5_ID].x0+20,y,200,16,AirQuiyGrade[PM2_5_ID][2],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X02)!=0X02)
						Show_Str(AllDispCirInfo[HCHO_ID].x0+10,y,200,16,AirQuiyGrade[3][2],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X01)!=0X01)
						Show_Str(AllDispCirInfo[CARBON_ID].x0+10,y,200,16,AirQuiyGrade[4][2],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
				}
				else if(AQI_Value>=150 && AQI_Value<=200)//�ȼ�4
				{
					Show_Str(AllDispCirInfo[PM2_5_ID].x0+20,y,200,16,AirQuiyGrade[PM2_5_ID][3],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X02)!=0X02)
						Show_Str(AllDispCirInfo[HCHO_ID].x0+10,y,200,16,AirQuiyGrade[3][3],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X01)!=0X01)
						Show_Str(AllDispCirInfo[CARBON_ID].x0+10,y,200,16,AirQuiyGrade[4][3],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
				}
				else if(AQI_Value>=200 && AQI_Value<=300)//�ȼ�5
				{
					Show_Str(AllDispCirInfo[PM2_5_ID].x0+20,y,200,16,AirQuiyGrade[PM2_5_ID][4],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X02)!=0X02)
						Show_Str(AllDispCirInfo[HCHO_ID].x0+10,y,200,16,AirQuiyGrade[3][4],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
					if((SysParaSetInfo.SensorSwitch&0X01)!=0X01)
						Show_Str(AllDispCirInfo[CARBON_ID].x0+10,y,200,16,AirQuiyGrade[4][4],16,0,AQI_COLOR,PARA_SHOW_INTERFACE_BACKCOLOR);
				}
				else if(AQI_Value>300)//�ȼ�6
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
						if(SensorData.HCHO<8)  //��ȩ��CO2�ж�
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
			else if(SensorData.Temp > SysParaSetInfo.ThresHoldStatus.ThrldVal[TEMP_ID]+5)  //���������5��Ļ�
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
//����ɫ����
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
//			LCDDrawCir(DispCirInfo->x0,DispCirInfo->y0,DispCirInfo->r1,DispCirInfo->ColorInfo.Cir1color,0,1);  	 //��Բ
//			LCDDrawCir(DispCirInfo->x0,DispCirInfo->y0,DispCirInfo->r2,DispCirInfo->ColorInfo.Cir2color,0,1);      //��Բ
			if(SensorOnline.Device.BMP180 == 1)
			{
				Show_DrawBMP(DispCirInfo->x0-65,DispCirInfo->y0+10,40,40,(uint8_t*)AirPressLogo,DispCirInfo->ColorInfo.Textcolor,PARA_SHOW_INTERFACE_BACKCOLOR);
				Show_Str(DispCirInfo->x0+55,DispCirInfo->y0-10,150,16,DispCirInfo->Text_UnitMeas,16,0,DispCirInfo->ColorInfo.Textcolor,DispCirInfo->ColorInfo.TextBackcolor);
				Show_Str(DispCirInfo->x0+55,DispCirInfo->y0+15,150,16,"��",16,0,DispCirInfo->ColorInfo.Textcolor,DispCirInfo->ColorInfo.TextBackcolor);
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
//��ʾͼ���ڵĴ�������������
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
			if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_14)==0 && SensorOnline.Device.PMS_5==1)  //�����������Դ��ʹ�ܵĻ�ִ��
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
				Show_Str(DispCirInfo->x0+17,DispCirInfo->y0+45,150,24,"������",24,0,BLACK,RGB);//δʹ�ܵĻ��׵׻���
		}break;
		case HCHO_ID:
		{
			if((SysParaSetInfo.SensorSwitch&0x02) == 0X02)   //ʹ���˼�ȩ������
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
					Show_Str(DispCirInfo->x0+17,DispCirInfo->y0+45,150,24,"������",24,0,BLACK,RGB);  //δʹ�ܵĻ��׵׻���
			}
			else if((SysParaSetInfo.SensorSwitch&0x02) != 0X02)  //δʹ�ܼ�ȩ������
			{
				RGB = CalColorValue(SensorData.PMData.PM1_0_S,PM2_5_ID);  //����ʾ��ȩ��λ�ô�����ʾPM1.0
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
					Show_Str(DispCirInfo->x0+17,DispCirInfo->y0+45,150,24,"������",24,0,BLACK,RGB);  //δʹ�ܵĻ��׵׻���
			}
		}break;
		case CARBON_ID:
		{
			if((SysParaSetInfo.SensorSwitch&0X01) == 0X01)  //��ʾCO2����
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
			if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_14)==0)  //�����������Դ��ʹ�ܵĻ�ִ��
			{
				switch(NumBit)
				{
					case 0:Show_Str(DispCirInfo->x0+17,DispCirInfo->y0+45,150,24,"������",24,0,BLACK,RGB);break;
					case 1:
					{
						if(PreBitnum2 != 1)
							LCD_Fill(DispCirInfo->x0-13,DispCirInfo->y0+35,DispCirInfo->x0+113,DispCirInfo->y0+88,RGB);
						PreBitnum2 = 1;
						if((SysParaSetInfo.SensorSwitch&0X01)==0X01 && SensorOnline.Device.SENSAIR==1)
						{
							if(SensorData.Carbon==0)
								Show_Str(DispCirInfo->x0+8,DispCirInfo->y0+46,150,DispCirInfo->FontSize," ׼����",24,0,GRAY,RGB);
							else if(SensorData.Carbon>=2000)
								Show_Str(DispCirInfo->x0+25,DispCirInfo->y0+46,150,DispCirInfo->FontSize," ����",24,0,BLACK,RGB);
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
				Show_Str(DispCirInfo->x0+17,DispCirInfo->y0+45,150,24,"δʹ��",24,0,BLACK,RGB);
		}break;
		default:break;
	}
}
//���ش���������
void LoadSensorData(void)
{
	//����������ת��
	SensorData.BatVol = Get_Adc_Average(ChalTab[1],5);
	if(SensorData.BatVol < LOW_VOL_ALARM_THREHOLD)  //����ɼ�����ADֵС�ڱ�����ֵ�Ļ�
		AlarmCloseFlag = 5;
	Read_sht30(&SensorData.Temp,&SensorData.Humi);  //��ȡ�¶�ֵ
	if(SensorOnline.Device.BMP180 == 1)
		SensorData.AirPres = bmp180Convert();       		 //��ȡ��ѹֵ
	SensorData.Altitude = (44330.0*(1.0-pow((float)(SensorData.AirPres)/101325.0,1.0/5.255)));
	Rx8025tGetTime(&SysTimeData);
	if(SysParaSetInfo.Power_5V_Status == 1)
	{
		if((SysTimeData.Minute%SysParaSetInfo.SensorSamplingTimeUnit) == 0)
		{
			Handle_5V_POWER_Status(1);
			if(SensorOnline.Device.PMS_5 == 1)
				PMS_Send_Cmd(PMS_READ_DATA);  			//��PM2.5����
			if((SysParaSetInfo.SensorSwitch&0x02) == 0x02)
			{
				if(SensorOnline.Device.WZ_S == 1)
					HCHODataHandle();
			}
			if((SysParaSetInfo.SensorSwitch&0x01) == 0x01)
			{
				if(SensorOnline.Device.SENSAIR == 1)
				{
					if(TIM4CH2_CAPTURE_STA&0X80)//�ɹ�������һ��������
						TIM4CH2_CAPTURE_STA = 0;//������һ�β���
				}
			}
		}
		else
			Handle_5V_POWER_Status(0);
	}
	else
		Handle_5V_POWER_Status(0);
}
//����һСʱ�ļ�¼����
//һСʱ�ĵڼ�����
void Save_Hour_Data(uint8_t Order)
{
	if((SysParaSetInfo.SensorSwitch&0x01) == 0x01)
		DataRemember_Hour.CO2[Order] = SensorData.Carbon;
	if((SysParaSetInfo.SensorSwitch&0x01) == 0x01)
		DataRemember_Hour.HCHO[Order] = SensorData.HCHO;
	DataRemember_Hour.PM2_5[Order] = SensorData.PMData.PM2_5_S;
}

//��ʾ���ƿ��ص�״̬
//150  64
void Ctrl_Switch_Staus(uint16_t xoffset,uint16_t yoffset)
{
	Show_Str(5,yoffset+5,200,16,"��������״̬:",16,0,WHITE,BOTTOM_TITTLE_BACK_COLOR);
	LCD_DrawRectangle(lcddev.width-xoffset,yoffset+3,lcddev.width-xoffset+86,yoffset+20,WHITE); // 150  6
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X10) == 0X10)
	{
		//if(!CtrlSWStatus.bits.SW1)
		{
			LCD_Fill(lcddev.width-xoffset+2,yoffset+5,lcddev.width-xoffset+16,yoffset+18,GREEN);//��
			LCD_ShowNum(lcddev.width-xoffset+7,yoffset+6,1,1,12,1,BLACK,GREEN);
		}
		/*else
		{
			LCD_Fill(lcddev.width-xoffset+2,yoffset+5,lcddev.width-xoffset+16,yoffset+18,RED);//��ɫ��ʾ�Ѵ�
			LCD_ShowNum(lcddev.width-xoffset+7,yoffset+6,1,1,12,BLACK,RED);
		}*/
	}
	else 
	{
		LCD_Fill(lcddev.width-xoffset+2,yoffset+5,lcddev.width-xoffset+16,yoffset+18,GRAY);  //��
		LCD_ShowNum(lcddev.width-xoffset+7,yoffset+6,1,1,12,1,BLACK,GRAY);
	}
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X08)==0X08 && (SysParaSetInfo.SensorSwitch&0x02)==0x02)
	{
		//if(!CtrlSWStatus.bits.SW2)
		{
			LCD_Fill(lcddev.width-xoffset+18,yoffset+5,lcddev.width-xoffset+33,yoffset+18,GREEN);//��
			LCD_ShowNum(lcddev.width-xoffset+23,yoffset+6,2,1,12,1,BLACK,GREEN);
		}
		/*else
		{
			LCD_Fill(lcddev.width-xoffset+18,5,lcddev.width-xoffset+33,18,RED);//��
			LCD_ShowNum(lcddev.width-xoffset+23,yoffset+6,2,1,1,BLACK,RED);
		}*/
	}
	else 
	{
		LCD_Fill(lcddev.width-xoffset+18,yoffset+5,lcddev.width-xoffset+33,yoffset+18,GRAY);  //��
		LCD_ShowNum(lcddev.width-xoffset+23,yoffset+6,2,1,12,1,BLACK,GRAY);
	}
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X04)==0X04 && (SysParaSetInfo.SensorSwitch&0x01)==0x01)
	{
		//if(!CtrlSWStatus.bits.SW3)
		{
			LCD_Fill(lcddev.width-xoffset+35,yoffset+5,lcddev.width-xoffset+50,yoffset+18,GREEN);//��
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
		LCD_Fill(lcddev.width-xoffset+35,yoffset+5,lcddev.width-xoffset+50,yoffset+18,GRAY);  //��
		LCD_ShowNum(lcddev.width-xoffset+40,yoffset+6,3,1,12,1,BLACK,GRAY);
	}
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X02) == 0X02)
	{
		//if(!CtrlSWStatus.bits.SW4)
		{
			LCD_Fill(lcddev.width-xoffset+52,yoffset+5,lcddev.width-xoffset+67,yoffset+18,GREEN);//��
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
		LCD_Fill(lcddev.width-xoffset+52,yoffset+5,lcddev.width-xoffset+67,yoffset+18,GRAY);  //��
		LCD_ShowNum(lcddev.width-xoffset+57,yoffset+6,4,1,12,1,BLACK,GRAY);
	}
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X01) == 0X01)
	{
		//if(!CtrlSWStatus.bits.SW5)
		{
			LCD_Fill(lcddev.width-xoffset+69,yoffset+5,lcddev.width-xoffset+84,yoffset+18,GREEN);//��
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
		LCD_Fill(lcddev.width-xoffset+69,yoffset+5,lcddev.width-xoffset+84,yoffset+18,GRAY);  //��
		LCD_ShowNum(lcddev.width-xoffset+74,yoffset+6,5,1,12,1,BLACK,GRAY);
	}
}
//����ʱ��Ϳ���״̬��WIFI��ʾ״̬
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
//��ص�ѹ���㹫ʽ
//���������ϵ�����ѹ��Ӧ��ADCֵ ADC_MAX_VAL = 4.2*BAT_RES2/(BAT_RES1+BAT_RES2) * 4095/3.3
//���������ϵ���С��ѹ��Ӧ��ADCֵ ADC_MIN_VAL = 3.3*BAT_RES2/(BAT_RES1+BAT_RES2) * 4095/3.3
//ͼ�ο��ϼ���ȥ��������Ϊ WIDTH = (ADC_ABSLUTE-ADC_MIN_VAL) / ((ADC_MAX_VAL - ADC_MIN_VAL)/34)    36Ϊ���ͼ�ε��ĺ�������
//
//��ʾʱ�䵽������ͬʱ���洫�����ɼ���������
void DispTimeToUI(void)
{
	uint8_t i;
	uint8_t CurrentVol;
	uint16_t Process = 0;
	uint8_t buf[15];
	static uint8_t CreateNewFlag = 0;   //ÿ��һ�촴���µ��ļ�����־
	static uint8_t SaveFlag = 0;
	if((SysTimeData.Minute%5)==0 && SaveFlag!=SysTimeData.Minute)  //5����ִ��һ��
	{
		SaveFlag = SysTimeData.Minute;
		Save_Hour_Data(Draw_Wave_Flag++);		//�����ڲ��λ��ƺ��������ж���
		if(Draw_Wave_Flag == 15)
		{
			Draw_Wave_Flag = 0;
			memset(&DataRemember_Hour.HCHO[0],0,60); 		//����
		}
	}
	if(Esp8266InitFinishFlag == 1)
	{
		if((SysTimeData.Minute%SysParaSetInfo.SDRemTimeUnit)==0 && SysTimeData.Second<=1)
		{
			if(SysTimeData.Hour != 0)
				CreateNewFlag = 0;
			if(SysTimeData.Hour==0 && CreateNewFlag==0)  //�Ѿ����˵ڶ�����߸տ����Ļ�
			{
				CreateNewFlag = 1;
				Get_New_pname(pname);
			}
			Write_Data_To_SDCard(pname,1);  //�������ݵ�SD�������û���ļ��򴴽��ļ�������������
		}
	}
	if((SysTimeData.Second%10) == 0)				//10����ִ��һ��
	{
		if(Esp8266Config.ServerLinkStat && (SysParaSetInfo.SensorSwitch&0x04) == 0x04)  //����������״̬������ʾ��ص�LOGO��Ϣ
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
		for(i=0;i<13;i++)  //�ܹ�13����λ
		{
			if(SensorData.BatVol <= Vol_To_ADC[i])
			{
				CurrentVol = i;  //����1��
				break;
			}
		}
		if(CurrentVol > 1)
		{
			LCD_DrawRectangle(lcddev.width-40,8,lcddev.width-4,20,WHITE);  //��������������
			LCD_Fill(lcddev.width-4,11,lcddev.width,16,WHITE);			//���Ƶ�ض�����С��
			LCD_Fill(lcddev.width-39,9,lcddev.width-5,19,TOP_TITLE_BACK_COLOR);   		//������
			if(((uint16_t)CurrentVol*3)>35)  //��ֹ��Խ�߽�
				CurrentVol = 12;//35/(Multiple*3);
			LCD_Fill(lcddev.width-39,9,lcddev.width-39+(uint16_t)CurrentVol*3+Multiple,19,LGRAY);  //�����겻�ܴ���30����ĺ������ط�Χ
		}
		else
		{
			LCD_DrawRectangle(lcddev.width-40,8,lcddev.width-4,20,RED);  //��������������
			LCD_Fill(lcddev.width-4,11,lcddev.width,16,RED);			//���Ƶ�ض�����С��
			LCD_Fill(lcddev.width-39,9,lcddev.width-5,19,TOP_TITLE_BACK_COLOR);   		//������
			if(((uint16_t)CurrentVol*3)>35)  //��ֹ��Խ�߽�
				CurrentVol = 12;//35/(Multiple*3);
			LCD_Fill(lcddev.width-39,9,lcddev.width-39+(uint16_t)CurrentVol*3+Multiple,19,RED);  //�����겻�ܴ���30����ĺ������ط�Χ
		}
	}
}
//��ʾ������Ϣ
void DispWeatherInfo(void)
{
	//AcuireFinishFlag = 0;
	if(AcuireFinishFlag == 1)
	{
		AcuireFinishFlag = 0;
		uint8_t buf[20];
		sprintf(buf,"��ǰ����:%s",NetWorkDataInfo.Weather_CityInfo);
		Show_Str(271,0,150,16,buf,16,0,WEATHERINFO_COLOR,TOP_TITLE_BACK_COLOR);
		sprintf(buf,"%s %s %d��",NetWorkDataInfo.Time_Week,NetWorkDataInfo.Weather_Climate,NetWorkDataInfo.Weather_TempInfo);
		Show_Str(277,17,150,16,buf,16,0,WEATHERINFO_COLOR,TOP_TITLE_BACK_COLOR);
	}
}
//������ʾ����1
void Load_ParaShow_Interface_1(void)
{	
	uint8_t i;
	uint8_t err;
	int16_t Temp=0;
	uint16_t Cnt = 0;
	LCD_Fill(0,36,CURSOR-22,289,PARA_SHOW_INTERFACE_BACKCOLOR);  //�����ʾ����ʾ�Ĵ��������ݵ�����
	if(LoadStaticUIFlag == 0)
	{
		LoadStaticUIFlag = 1;
		//LCD_Fill(CURSOR-21,27,480,320,FillColor);  	//�ұߵĳ�����ɫ
		//LCD_Fill(0,0,480,25,TOP_TITLE_BACK_COLOR);			//�����ı��ⲿ��
		//LCD_Fill(0,290,480,320,BOTTOM_TITTLE_BACK_COLOR);		//�ײ��ı��ⱳ��ɫ
		LCD_DrawLine(0,35,CURSOR-22,35,LIGHTBLUE);	//�����ˮƽ��
		LCD_DrawLine(0,290,CURSOR-22,290,LIGHTBLUE);	//�����ˮƽ��
		LoadColorPat();   							//����ɫ�׺ͱ��
		Show_Str(0,3,100,24,BRAND_NAME,24,0,WHITE,TOP_TITLE_BACK_COLOR);  //��ʾ�̱�
		if(AcuireFinishFlag == 0)
			Show_Str(281,7,150,16,"��������Ϣ",16,0,GRAY,TOP_TITLE_BACK_COLOR);
		DispWeatherInfo();
	}
	Ctrl_Switch_Staus(360,296);	  //���ұ�����߱�������
	for(i=0;i<6;i++)
	{
		Load_ShowUI_1(&AllDispCirInfo[Item_Id[i]],Item_Id[i]);
	}
	LCD_DrawLine(0,216,CURSOR-22,216,LIGHTBLUE);  //�м�ĵ�һ��ˮƽ��	
	TIM2->CCR2 = BackLightBright;   //��ʱ����Һ����
	Current_Show_Interface = Curr_Para_Show;
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)
		{
			if(Encoder_Type==ENCODER_FORWORD || Encoder_Type==ENCODER_BACKWORD)
			{
				break;
			}
			else if(Encoder_Type == ENCODER_PRESSED)  //����ǳ����Ļ�
			{
				Encoder_Type = ENCODER_VOID;
				while((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0))
				{
					Cnt++;
					delay_ms(200); 
					if(Cnt>=10)
						break;
				}
				if(Cnt>=10)  //����ǳ����Ļ�
				{
					Cnt = 0;
					SetBeepFreq(AlarmFreqTab[8]);
					Show_Str(240,300,200,16,"���ȵ���    %",16,0,LIGHTBLUE,PARA_SHOW_INTERFACE_BACKCOLOR);
					LCD_ShowNum(305,300,100-BackLightBright/10,3,16,1,LIGHTBLUE,PARA_SHOW_INTERFACE_BACKCOLOR);
					while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);
					OSTmrStop(tmr2,OS_TMR_OPT_NONE,0,&err);
					while(1)
					{
						if(Cnt++ > 100)  			//4����û��Ӧ����������
							break;
						if(Encoder_Type == ENCODER_FORWORD)  //����
						{
							Encoder_Type = ENCODER_VOID;
							Cnt = 0;
							BackLightBright -= 50;
							if(BackLightBright <= 0)
								BackLightBright = 1;
							TIM2->CCR2 = (uint16_t)BackLightBright;
							LCD_ShowNum(305,300,100-BackLightBright/10,3,16,1,LIGHTBLUE,PARA_SHOW_INTERFACE_BACKCOLOR);
						}
						else if(Encoder_Type == ENCODER_BACKWORD)//����
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
							break;  //����
						}
						delay_ms(20);
					}
				}
			}
		}
		else
		{
			OSSemPend(sem_data_handle,0,&err);  //�����ź���
			//LoadSensorData();  //���ش���������
			for(i=0;i<7;i++)
			{
				if(CloseMachineFlag == 0)
					DispDataInfo(&AllDispCirInfo[Item_Id[i]],Item_Id[i]);
			}
			DispTimeToUI();
			if(CloseMachineFlag == 0)
				EnvirQuaty(0,192);  //��������ж�
		}
		//delay_ms(10);
		//EnvirQuaty(0,292);  //��������ж�
	}
}
//���ؽ���2Ҫ��ʾ�ľ�̬UI
void Load_UI2_Static(uint16_t x,uint16_t y,uint16_t unit)
{
	LCD_Fill(0,36,CURSOR-22,289,PMCNT_SHOW_INTERFACE_BACKCOLOR);  //�����ʾ����ʾ�Ĵ��������ݵ�����
	//LCD_DrawLine(0,27,CURSOR-22,27,BLUE);
	//��ʾŨ��ֵ��UI
	Show_Str(x+50,y,300,24,"Ũ��ֵ",24,0,C_THISTLE,BLACK);  
	Show_Str(280+x,y,300,24,"���������",24,0,C_THISTLE,BLACK); //���ﲹ����50
	
	Show_Str(x,y+unit,300,24,"PM2.5",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(x,y+unit*2,300,24,"PM1.0",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(x,y+unit*3,300,24,"PM10",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(x,y+unit*4,300,24,"HCHO",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(x,y+unit*5,300,24,"CO2",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(x+80,y+unit*4,100,24,".",24,0,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);  		//��ʾһ��
	
	Show_Str(x+135,y+unit,300,24,"ug/m3",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(x+135,y+unit*2,300,24,"ug/m3",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(x+135,y+unit*3,300,24,"ug/m3",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(x+135,y+unit*4,300,24,"mg/m3",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(x+135,y+unit*5,300,24,"ppm",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	//��ʾ�������UI
	
	Show_Str(240+x,y+unit,300,24,">0.3um:",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(240+x,y+unit*2,300,24,">0.5um:",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(240+x,y+unit*3,300,24,">1.0um:",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(240+x,y+unit*4,300,24,">2.5um:",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(240+x,y+unit*5,300,24,">5.0um:",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(240+x,y+unit*6,300,24,">10um:",24,0,C_ORANGE_RED,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	
	Show_Str(240+x+150,y+unit,300,24,"��",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(240+x+150,y+unit*2,300,24,"��",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(240+x+150,y+unit*3,300,24,"��",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(240+x+150,y+unit*4,300,24,"��",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(240+x+150,y+unit*5,300,24,"��",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(240+x+150,y+unit*6,300,24,"��",24,0,LIGHTBLUE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
}
//���ؽ���2Ҫ��̬��ʾ������ֵ
void Load_UI2_Dynamic(uint16_t x,uint16_t y,uint16_t unit)
{
	//uint8_t buf[10];
	//��ʾŨ��ֵ
	LCD_ShowNum(x,y+unit,SensorData.PMData.PM2_5_S,4,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);     	//PM2.5  ug/m3
	LCD_ShowNum(x,y+unit*2,SensorData.PMData.PM1_0_S,4,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);  	//PM1.0  ug/m3
	LCD_ShowNum(x,y+unit*3,SensorData.PMData.PM10_S,4,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR); 		//PM10  ug/m3
	
	//sprintf(buf,"%f",(float)SensorData.HCHO/100);
	LCD_ShowNum(x-5,y+unit*4,SensorData.HCHO/100,1,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);  				//HCHO  ug/m3
	LCD_ShowNum(x+24-5,y+unit*4,SensorData.HCHO/10%10,1,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	LCD_ShowNum(x+40-5,y+unit*4,SensorData.HCHO%10,1,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);
	LCD_ShowNum(x-5,y+unit*5,SensorData.Carbon,5,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);  			//CO2  ug/m3
	//��ʾ����������
	LCD_ShowNum(240+x+17,y+unit,SensorData.PMData.Cnt_0_3,5,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);  		//>0.3 um
	LCD_ShowNum(240+x+17,y+unit*2,SensorData.PMData.Cnt_0_5,5,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);  	//>0.5 um
	LCD_ShowNum(240+x+17,y+unit*3,SensorData.PMData.Cnt_1_0,5,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);  	//>1.0 um
	LCD_ShowNum(240+x+17,y+unit*4,SensorData.PMData.Cnt_2_5,5,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);  	//>2.5 um
	LCD_ShowNum(240+x+17,y+unit*5,SensorData.PMData.Cnt_5_0,5,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);  	//>5.0 um
	LCD_ShowNum(240+x+17,y+unit*6,SensorData.PMData.Cnt_10,5,24,1,WHITE,PMCNT_SHOW_INTERFACE_BACKCOLOR);  		//>10 um
}
//���ؿ�������Ũ����ʾ����2
void Load_PMCnctrShow_Interface_2(void)
{
	uint8_t err;
	Load_UI2_Static(20,42,35);//���x�����y�����
	Current_Show_Interface = Curr_PM2_5_Show;
	OSTmrStart(tmr1,&err);//���������ʱ��1
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
		OSSemPend(sem_data_handle,0,&err);  //�����ź���
		if(CloseMachineFlag == 0)
			Load_UI2_Dynamic(90,40,35);
		DispTimeToUI();		
		EnvirQuaty(0,192);
		if(AutoJumpFlag)
		{
			break;
		}
		delay_ms(10);
		//EnvirQuaty(0,292);  //��������ж�
	}
}
//����һСʱ���ݲ���
//��ȩ��PM3.5��CO2����ʪ�ȣ���ѹ         10         260
void Load_WaveformShow_Interface_3(void)
{
	uint8_t i;
	uint8_t err;
	static uint8_t RefreshFlag = 0;  //����ˢ�±�־ 
	//uint8_t HCHO;
	//uint16_t CO2,PM;
	LCD_Fill(0,36,CURSOR-22,289,WAVE_SHOW_INTERFACE_BACKCOLOR);  //�����ʾ����ʾ�Ĵ��������ݵ�����
	
	Show_Str(0,267,80,16,"PM2.5",16,0,LIGHTBLUE,WAVE_SHOW_INTERFACE_BACKCOLOR);    //���ڳ�ȫ����ʱ�����
	Show_Str(0,198,80,16,"HCHO",16,0,LIGHTBLUE,WAVE_SHOW_INTERFACE_BACKCOLOR);    //���ڳ�ȫ����ʱ�����
	Show_Str(0,121,80,16,"CO2",16,0,LIGHTBLUE,WAVE_SHOW_INTERFACE_BACKCOLOR);    //���ڳ�ȫ����ʱ�����
	//����������                                       �������
	LCD_DrawLine(40,275, CURSOR-40,275,WHITE);  //X��   PM2.5 Y����㿪ʼ����
	LCD_DrawLine(40,275, 40,36,WHITE);          //Y��
	LCD_DrawLine(40,206, CURSOR-40,206,WHITE);  //X��   HCHO
	LCD_DrawLine(40,129, CURSOR-40,129,WHITE);  //X��   CO2
	//���ƿ̶�
	for(i=1;i<32;i++)
	{
		LCD_DrawLine(40+12*i,274,40+12*i,271,GRAY);		//��ֱ���(436-10)/10=42.6
		if(i<15)
			LCD_ShowNum(40+29*(i-1),276,(i-1)*5,2,12,1,WHITE,WAVE_SHOW_INTERFACE_BACKCOLOR);		
		LCD_DrawLine(40+12*i,205,40+12*i,203,GRAY);		//��ֱ���(436-10)/10=42.6
		
		LCD_DrawLine(40+12*i,128,40+12*i,126,GRAY);		//��ֱ���(436-10)/10=42.6
		
		//LCD_DrawLine(30,290-23*i,CURSOR-30,290-23*i,GRAY);	//ˮƽ���
	}
	LCD_DrawLine(40,259,CURSOR-40,259,RED);//PM2.5   274-75/5������   �������
	LCD_DrawLine(40,165,CURSOR-40,165,RED);//HCHO
	LCD_DrawLine(40,90,CURSOR-40,90,RED);//CO2
	
	Show_Str(24,259,100,12,"75",12,0,RED,WAVE_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(14,155,100,12,"0.08",12,0,RED,WAVE_SHOW_INTERFACE_BACKCOLOR);
	Show_Str(14,85,100,12,"800",12,0,RED,WAVE_SHOW_INTERFACE_BACKCOLOR);
	for(i=0;i<Draw_Wave_Flag-1;i++)
	{
		LCD_DrawLine(40+i*29,274-DataRemember_Hour.PM2_5[i]/5,40+(i+1)*29,274-DataRemember_Hour.PM2_5[i+1]/5,GREEN);  //4Ϊ��ֵΪ300ʱ��
		if((SysParaSetInfo.SensorSwitch&0x01) == 0x01)
			LCD_DrawLine(40+i*29,128-DataRemember_Hour.CO2[i]/21,40+(i+1)*29,128-DataRemember_Hour.CO2[i+1]/21,GREEN);
		if((SysParaSetInfo.SensorSwitch&0x02) == 0x02)
			LCD_DrawLine(40+i*29,205-DataRemember_Hour.HCHO[i]*5.13,40+(i+1)*29,205-DataRemember_Hour.HCHO[i+1]*5.13,GREEN);
	}
	Current_Show_Interface = Curr_WaveForm_Show;
	OSTmrStart(tmr1,&err);//���������ʱ��1
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
		OSSemPend(sem_data_handle,0,&err);  //�����ź���
		DispTimeToUI();
		EnvirQuaty(0,192);
		if(AutoJumpFlag)
		{
			break;
		}
		delay_ms(10);
	}
	//������ӻ������ݵ�����
}
//���ò���UI����
void Load_SetShow_Interface_4(void)
{
	uint8_t Cnt = 0;
	uint8_t err;
	LCD_Fill(0,36,239,289,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	LCD_Fill(241,36,CURSOR-22,289,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(150,40,300,24,"ϵͳ��������",24,0,BLACK,BRRED);
	//LCD_DrawLine(0,60,CURSOR-25,60,LIGHTBLUE);  //�ָ���
	LCD_DrawLine(240,60,240,290,LIGHTBLUE);
	//LCD_DrawLine(0,290,CURSOR-25,290,LIGHTBLUE);
	Show_Str(25,65,300,24,(uint8_t *)SetUIL1_Text[0],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);    //���ڳ�ȫ����ʱ�����
	Show_Str(25,95,300,24,(uint8_t *)SetUIL1_Text[1],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,125,300,24,(uint8_t *)SetUIL1_Text[2],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,155,300,24,(uint8_t *)SetUIL1_Text[3],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,185,300,24,(uint8_t *)SetUIL1_Text[4],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,215,300,24,(uint8_t *)SetUIL1_Text[5],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,245,300,24,(uint8_t *)SetUIL1_Text[6],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Current_Show_Interface = Curr_Set_Show;
	OSTmrStart(tmr1,&err);//���������ʱ��1
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)
		{
			if(Encoder_Type == ENCODER_PRESSED)
			{
				Current_Show_Interface = Curr_SetPara_Show;
				OSTaskSuspend(PRIOR_TASK_PRIO);	//������������	
			}
			OSTmrStop(tmr1,OS_TMR_OPT_NONE,0,&err);
			break;
		}
		OSSemPend(sem_data_handle,0,&err);  //�����ź���
		DispTimeToUI();
		EnvirQuaty(0,192);
		if(AutoJumpFlag)
		{
			break;
		}
		delay_ms(10);   //ϵͳ��ʱ���������
	}
}
//��һ��˵�
//ʱ��
void SetLay1UI_Time(void)
{
	uint8_t err;
	Show_Str(25,65,300,24,(uint8_t *)SetUIL1_Text[0],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_TEXTBACKCOLOR);
	Show_Str(25,95,300,24,(uint8_t *)SetUIL1_Text[1],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,245,300,24,(uint8_t *)SetUIL1_Text[6],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR); //�˳��������ʾ
	//��ʾ���������
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
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //������ڰ��žͼ����ȴ�
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
//΢��
void SetLay1UI_AutoStandby(void)
{
	Show_Str(25,65,300,24,(uint8_t *)SetUIL1_Text[0],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,95,300,24,(uint8_t *)SetUIL1_Text[1],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_TEXTBACKCOLOR);
	Show_Str(25,125,300,24,(uint8_t *)SetUIL1_Text[2],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR); //�˳��������ʾ
	Show_Str(250,65,300,24,"�Զ�����:",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,89,300,24,"�����л�:",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,113,300,24,"������ʱ:  ��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if(SysParaSetInfo.AutoStandbyStatus)
	{
		Show_Str(360,65,300,24,"�ѿ�",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		LCD_ShowNum(360,113,SysParaSetInfo.AutoStandbyTime,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	}		
	else
	{
		Show_Str(360,65,300,24,"�ѹ�",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		LCD_ShowNum(360,113,SysParaSetInfo.AutoStandbyTime,2,24,1,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	}		
	if(SysParaSetInfo.InterfaceSwitch)
		Show_Str(360,89,300,24,"�ѿ�",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	else
		Show_Str(360,89,300,24,"�ѹ�",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)
			break;
		delay_ms(10);
	}
}
//����ʶ��
void SetLay1UI_DeviceSW(void)
{
	Show_Str(25,95,300,24,(uint8_t *)SetUIL1_Text[1],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,125,300,24,(uint8_t *)SetUIL1_Text[2],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_TEXTBACKCOLOR);
	Show_Str(25,155,300,24,(uint8_t *)SetUIL1_Text[3],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR); //�˳��������ʾ
	Show_Str(250,65,300,24,"�豸��Դ:",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,89,300,24,"����ʶ��:",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,113,300,24,"��������:",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if(SysParaSetInfo.Power_5V_Status)
		Show_Str(360,65,300,24,"�ѿ�",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	else
		Show_Str(360,65,300,24,"�ѹ�",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	if(SysParaSetInfo.SpeechRecognize.Speech_Sw)
	{
		Show_Str(360,89,300,24,"�ѿ�",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		LCD_ShowNum(360,113,SysParaSetInfo.SpeechRecognize.Speech_Volume,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	}
	else
	{
		Show_Str(360,89,300,24,"�ѹ�",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
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
	LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR); //�˳��������ʾ   
	Show_Str(250,65,300,24,"��¼���:    ��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,89,300,24,"�������:    ��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	LCD_ShowNum(365,65,SysParaSetInfo.SDRemTimeUnit,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(365,89,SysParaSetInfo.SensorSamplingTimeUnit,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)
			break;
		
		delay_ms(10);
	}
}
//��ֵ
void SetLay1UI_Threshold(void)
{
	uint8_t i =0;
	uint8_t buf[10];
	uint8_t *p = &SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus;  
	uint8_t *p1 = &SysParaSetInfo.SensorSwitch;
	Show_Str(25,155,300,24,(uint8_t *)SetUIL1_Text[3],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,185,300,24,(uint8_t *)SetUIL1_Text[4],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_TEXTBACKCOLOR);
	Show_Str(25,215,300,24,(uint8_t *)SetUIL1_Text[5],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR); //�˳��������ʾ 
	//Show_Str(250,170,300,24,"��������ֵ����",24,0,GREEN,BLACK);
	for(i=0;i<5;i++)
	{
		if(*p&(uint8_t)(1<<(4-i)))
		{
			if(i==1)
			{
				Show_Str(250,65+i*30,300,24,(uint8_t *)SetUIL2_Text[i+7],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
				Show_Str(330,65+i*30,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
				sprintf(buf,"0.%02d",SysParaSetInfo.ThresHoldStatus.ThrldVal[i]);
				Show_Str(360,65+i*30,100,24,buf,24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			}
			else
			{
				Show_Str(250,65+i*30,300,24,(uint8_t *)SetUIL2_Text[i+7],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
				Show_Str(330,65+i*30,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
				LCD_ShowNum(360,65+i*30,SysParaSetInfo.ThresHoldStatus.ThrldVal[i],4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			}
		}
		else
		{
			if(i==1)
			{
				Show_Str(250,65+i*30,300,24,(uint8_t *)SetUIL2_Text[i+7],24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
				Show_Str(330,65+i*30,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
				sprintf(buf,"0.%02d",SysParaSetInfo.ThresHoldStatus.ThrldVal[i]);
				Show_Str(360,65+i*30,100,24,buf,24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			}
			else
			{
				Show_Str(250,65+i*30,300,24,(uint8_t *)SetUIL2_Text[i+7],24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
				Show_Str(330,65+i*30,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
				LCD_ShowNum(360,65+i*30,SysParaSetInfo.ThresHoldStatus.ThrldVal[i],4,24,1,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			}
		}
	}
	if((*p1&0x02) == 0x02)
		Show_Str(250,65+5*30,300,24,"HCHOʹ��:��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	else if((*p1&0x02) != 0x02)
		Show_Str(250,65+5*30,300,24,"HCHOʹ��:��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if((*p1&0x01) == 0x01)
		Show_Str(250,65+6*30,300,24,"CO2ʹ��:��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	else if((*p1&0x01) != 0x01)
		Show_Str(250,65+6*30,300,24,"CO2ʹ��:��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)   //����
			break;
		delay_ms(10);
	}
}
//WIFI״̬����
void SetLay1UI_WIFIConfig(void)
{
	Show_Str(25,185,300,24,(uint8_t *)SetUIL1_Text[4],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,215,300,24,(uint8_t *)SetUIL1_Text[5],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_TEXTBACKCOLOR);
	Show_Str(25,245,300,24,(uint8_t *)SetUIL1_Text[6],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR); //�˳��������ʾ
	if(!Esp8266Config.WifiLinkSuccFlag)
		Show_Str(270,65,170,16,"���½���",16,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	else
		Show_Str(270,65,170,16,"��������",16,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
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
		Show_Str(250,175,170,24,"WIFIδʹ��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)   //����
		{
			if(Encoder_Type == ENCODER_PRESSED)
				LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR); //�˳��������ʾ
			break;
		}
		delay_ms(10);
	}
}
//�˳�
void SetLay1UI_Quit(void)
{
	Show_Str(25,65,300,24,(uint8_t *)SetUIL1_Text[0],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,215,300,24,(uint8_t *)SetUIL1_Text[5],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_BACKCOLOR);
	Show_Str(25,245,300,24,(uint8_t *)SetUIL1_Text[6],24,0,SET_SHOW_INTERFACE_LEFT_TEXTCOLOR,SET_SHOW_INTERFACE_LEFT_TEXTBACKCOLOR);
	LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR); //�˳��������ʾ
	Show_Str(260,170,300,24,"�˳�����",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)   //����
		{
			if(Encoder_Type == ENCODER_PRESSED)
			{
				WriteSetPara();			//д�����ò���
				DispWifiInfoFlag = 0;  	//�����ò���״̬����ʾWIFI��Ϣ�ı�־
				LCD_Fill(0,65,439,289,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
				OSTaskResume(PRIOR_TASK_PRIO);
			}
			break;
		}
		delay_ms(10);
	}
}
//�ڶ���˵�
//��
void SetLay2UI_Time_Year(void)
{
	Show_Str(290,65,300,24,(uint8_t *)SetUIL2_Text[0],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(290,95,300,24,(uint8_t *)SetUIL2_Text[1],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(290,205,300,24,(uint8_t *)SetUIL2_Text[4],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	
	LCD_ShowNum(290+50,65,2000+SysTimeData.Year,4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	LCD_ShowNum(290+50,95,SysTimeData.Month,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(290+50,205,SysTimeData.Minute,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //������ڰ��žͼ����ȴ�
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysTimeData.Year ++;
			if(SysTimeData.Year >= 89)
				SysTimeData.Year = 0;
			//������ʾ���õľ�����ֵ
			LCD_ShowNum(290+50,65,2000+SysTimeData.Year,4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysTimeData.Year --;
			if(SysTimeData.Year <= 0)
				SysTimeData.Year = 89;
			//������ʾ���õľ�����ֵ
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
//��
void SetLay2UI_Time_Month(void)
{
	Show_Str(290,65,300,24,(uint8_t *)SetUIL2_Text[0],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(290,95,300,24,(uint8_t *)SetUIL2_Text[1],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(290,135,300,24,(uint8_t *)SetUIL2_Text[2],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(290+50,65,2000+SysTimeData.Year,4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(290+50,95,SysTimeData.Month,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	LCD_ShowNum(290+50,135,SysTimeData.Day,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //������ڰ��žͼ����ȴ�
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysTimeData.Month ++;
			if(SysTimeData.Month>= 13)
				SysTimeData.Month = 1;
			//������ʾ���õľ�����ֵ
			LCD_ShowNum(290+50,95,SysTimeData.Month,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysTimeData.Month --;
			if(SysTimeData.Month <= 0)
				SysTimeData.Month = 12;
			//������ʾ���õľ�����ֵ
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
//��
void SetLay2UI_Time_Day(void)
{
	Show_Str(290,95,300,24,(uint8_t *)SetUIL2_Text[1],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(290,135,300,24,(uint8_t *)SetUIL2_Text[2],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(290,170,300,24,(uint8_t *)SetUIL2_Text[3],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	
	LCD_ShowNum(290+50,95,SysTimeData.Month,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(290+50,135,SysTimeData.Day,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	LCD_ShowNum(290+50,170,SysTimeData.Hour,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //������ڰ��žͼ����ȴ�
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysTimeData.Day ++;
			if(SysTimeData.Day >= 32)
				SysTimeData.Day = 1;
			//������ʾ���õľ�����ֵ
			LCD_ShowNum(290+50,135,SysTimeData.Day,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysTimeData.Day --;
			if(SysTimeData.Day <= 0)
				SysTimeData.Day = 31;
			//������ʾ���õľ�����ֵ
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
//ʱ
void SetLay2UI_Time_Hour(void)
{
	Show_Str(290,135,300,24,(uint8_t *)SetUIL2_Text[2],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(290,170,300,24,(uint8_t *)SetUIL2_Text[3],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(290,205,300,24,(uint8_t *)SetUIL2_Text[4],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	
	LCD_ShowNum(290+50,135,SysTimeData.Day,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(290+50,170,SysTimeData.Hour,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	LCD_ShowNum(290+50,205,SysTimeData.Minute,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //������ڰ��žͼ����ȴ�
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysTimeData.Hour ++;
			if(SysTimeData.Hour >= 24)
				SysTimeData.Hour = 0;
			//������ʾ���õľ�����ֵ
			LCD_ShowNum(290+50,170,SysTimeData.Hour,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysTimeData.Hour --;
			if(SysTimeData.Hour == 255)
				SysTimeData.Hour = 23;
			//������ʾ���õľ�����ֵ
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
//��
void SetLay2UI_Time_Minute(void)
{
	Show_Str(290,65,300,24,(uint8_t *)SetUIL2_Text[0],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(290,170,300,24,(uint8_t *)SetUIL2_Text[3],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(290,205,300,24,(uint8_t *)SetUIL2_Text[4],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	
	LCD_ShowNum(290+50,65,2000+SysTimeData.Year,4,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(290+50,170,SysTimeData.Hour,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	LCD_ShowNum(290+50,205,SysTimeData.Minute,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //������ڰ��žͼ����ȴ�
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysTimeData.Minute ++;
			if(SysTimeData.Minute >= 60)
				SysTimeData.Minute = 0;
			//������ʾ���õľ�����ֵ
			LCD_ShowNum(290+50,205,SysTimeData.Minute,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysTimeData.Minute --;
			if(SysTimeData.Minute <= 0)
				SysTimeData.Minute = 59;
			//������ʾ���õľ�����ֵ
			LCD_ShowNum(290+50,205,SysTimeData.Minute,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			LCD_Fill(290,65,430,250,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);//�˳��������ʾ		
			Set_Rx8025t_Time(SysTimeData.Year,SysTimeData.Month,SysTimeData.Day,SysTimeData.Hour,SysTimeData.Minute,0);  //����ʱ��	
			Update_Set_Info(1);	//ʱ�����		
			Show_Str(290,170,300,24,"������",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);			
			break;
		}
		delay_ms(10);
	}
}
//�˵�����3��������ֵ�Ƿ�����״̬ģʽ
void SetLay3UI_ThresHold_Status(void)
{
	LCD_Fill(249,169,430,185,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);//������ʾ��С��
	Show_Str(250,65,300,24,"��������",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(250,95,300,24,"������ֵ",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,125,300,24,"����ʹ��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)
			break;
		//delay_ms(100);
	}
}
//�˵�����3����ֵ����ģʽ
void SetLay3UI_ThresHold_Adjust(void)
{
	Show_Str(250,65,300,24,"��������",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,95,300,24,"������ֵ",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(250,125,300,24,"����ʹ��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)
			break;
		delay_ms(10);
	}
}
//��������������
void SetLay3UI_Sensor_Status(void)
{
	Show_Str(250,65,300,24,"��������",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,95,300,24,"������ֵ",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,125,300,24,"����ʹ��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //������ڰ��žͼ����ȴ�
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type != ENCODER_VOID)
			break;
		delay_ms(10);
	}
}
//���ü�ȩ����������״̬
void Set_HCHOSensor_Status(void)
{
	LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,65,300,24,(uint8_t *)SetUIL2_Text[8],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(250,95,300,24,(uint8_t *)SetUIL2_Text[9],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if((SysParaSetInfo.SensorSwitch&0X02) == 0X02)
		Show_Str(330,65,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
	else 
		Show_Str(330,65,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	if((SysParaSetInfo.SensorSwitch&0X01) == 0X01)
		Show_Str(330,95,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	else
		Show_Str(330,95,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SensorSwitch |= 1<<1;
			Show_Str(330,65,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SensorSwitch &= ~(1<<1);
			Show_Str(330,65,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			if((SysParaSetInfo.SensorSwitch&0X02) == 0X02)
				Show_Str(330,65,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			else
				Show_Str(330,65,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			break;
		}
	}
}
//���ö�����̼����������״̬
void Set_CO2Sensor_Status(void)
{
	Show_Str(250,65,300,24,(uint8_t *)SetUIL2_Text[8],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,95,300,24,(uint8_t *)SetUIL2_Text[9],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	if((SysParaSetInfo.SensorSwitch&0X02) == 0X02)
		Show_Str(330,65,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	else
		Show_Str(330,65,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if((SysParaSetInfo.SensorSwitch&0X01) == 0X01)
		Show_Str(330,95,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
	else
		Show_Str(330,95,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SensorSwitch |= 1<<0;
			Show_Str(330,95,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SensorSwitch &= ~(1<<0);
			Show_Str(330,95,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			if((SysParaSetInfo.SensorSwitch&0X01) == 0X01)
				Show_Str(330,95,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			else
				Show_Str(330,95,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			Show_Str(290,170,300,24,"������",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			break;
		}
	}
}
//PM2.5��ֵ����
void Set_PM2_5_Threshold(void)
{
	LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,65,300,24,(uint8_t *)SetUIL2_Text[7],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(250,95,300,24,(uint8_t *)SetUIL2_Text[8],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,135,300,24,(uint8_t *)SetUIL2_Text[9],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,170,300,24,(uint8_t *)SetUIL2_Text[10],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,205,300,24,(uint8_t *)SetUIL2_Text[11],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X10) == 0X10)
		Show_Str(330,65,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	else
		Show_Str(330,65,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //������ڰ��žͼ����ȴ�
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus |= 1<<4;
			Show_Str(330,65,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus &= ~(1<<4);
			Show_Str(330,65,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			Encoder_Type = ENCODER_VOID;
		}	
		else if(Encoder_Type == ENCODER_PRESSED)
			break;
		delay_ms(80);
	}
}
//HCHO��ֵ����
void Set_HCHO_Threshold(void)
{
	Show_Str(250,65,300,24,(uint8_t *)SetUIL2_Text[7],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,95,300,24,(uint8_t *)SetUIL2_Text[8],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(250,135,300,24,(uint8_t *)SetUIL2_Text[9],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X08) == 0X08)
		Show_Str(330,95,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	else
		Show_Str(330,95,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //������ڰ��žͼ����ȴ�
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus |= 1<<3;
			Show_Str(330,95,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus &= ~(1<<3);
			Show_Str(330,95,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_PRESSED)
			break;
		delay_ms(80);
	}
}
//CO2��ֵ����
void Set_CO2_Threshold(void)
{
	Show_Str(250,95,300,24,(uint8_t *)SetUIL2_Text[8],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,135,300,24,(uint8_t *)SetUIL2_Text[9],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(250,170,300,24,(uint8_t *)SetUIL2_Text[10],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X04) == 0X04)
		Show_Str(330,135,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	else
		Show_Str(330,135,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //������ڰ��žͼ����ȴ�
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus |= 1<<2;
			Show_Str(330,135,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus &= ~(1<<2);
			Show_Str(330,135,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_PRESSED)
			break;
		delay_ms(80);
	}
}

//�¶���ֵ����
void Set_Temp_Threshold(void)
{
	Show_Str(250,135,300,24,(uint8_t *)SetUIL2_Text[9],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,170,300,24,(uint8_t *)SetUIL2_Text[10],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(250,205,300,24,(uint8_t *)SetUIL2_Text[11],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X02) == 0X02)
		Show_Str(330,170,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	else
		Show_Str(330,170,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //������ڰ��žͼ����ȴ�
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus |= 1<<1;
			Show_Str(330,170,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus &= ~(1<<1);
			Show_Str(330,170,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_PRESSED)
			break;
		delay_ms(80);
	}
}
//ʪ����ֵ����
void Set_Humi_Threshold(void)
{
	Show_Str(250,65,300,24,(uint8_t *)SetUIL2_Text[7],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,170,300,24,(uint8_t *)SetUIL2_Text[10],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	Show_Str(250,205,300,24,(uint8_t *)SetUIL2_Text[11],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	if((SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus&0X01) == 0X01)
		Show_Str(330,205,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	else
		Show_Str(330,205,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //������ڰ��žͼ����ȴ�
	delay_ms(400);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus |= 1<<0;
			Show_Str(330,205,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus &= ~(1<<0);
			Show_Str(330,205,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			Encoder_Type = ENCODER_VOID;
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR); //�˳��������ʾ 
			Update_Set_Info(3);
			break;
		}
		delay_ms(80);
	}
}
//����PM2_5��ֵ
void Adjust_PM2_5_Val(void)
{
	uint8_t i;
	uint8_t buf[5];
	uint8_t *p = &SysParaSetInfo.ThresHoldStatus.SensorAlarmStatus;  	//�õ���һ����ַ
	LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	for(i=0;i<5;i++)
	{
		if(*p&(uint8_t)(1<<(4-i)))
		{
			if(i==0)
				Show_Str(250,65+i*35,300,24,(uint8_t *)SetUIL2_Text[7+i],24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);   //������ж�����ֻ�Ե�һ�����ж�����Ĳ���
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
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //������ڰ��žͼ����ȴ�
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
		else   //���û��ʹ�ܿ��صĻ�ֱ������
		{
			Encoder_Type = ENCODER_PRESSED;
			break;
		}
		delay_ms(80);
	}
}
//����HCHO��ֵ
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
			return;  //�������Ķ����б�Ҫ���ùʲ������Ļ����
		else
			LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		return;
	}
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //������ڰ��žͼ����ȴ�
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
//����CO2��ֵ
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
			return;  //�������Ķ����б�Ҫ���ùʲ������Ļ����
		else
			LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		return;
	}
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //������ڰ��žͼ����ȴ�
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
//�����¶���ֵ
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
			return;  //�������Ķ����б�Ҫ���ùʲ������Ļ����
		else
			LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		return;
	}
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //������ڰ��žͼ����ȴ�
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
//����ʪ����ֵ
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
			return;  //�������Ķ����б�Ҫ���ùʲ������Ļ����
		else
			LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		return;
	}
	while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==0);  //������ڰ��žͼ����ȴ�
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
			Update_Set_Info(3);	//����״̬���¸���			
			break;
		}
		delay_ms(80);
	}
}
//�Զ�������������
void SetLay2UI_AutoStandby(void)
{
	//LCD_Fill(290,65,400,250,BLACK);
	if(SysParaSetInfo.AutoStandbyStatus)
		Show_Str(360,65,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	else
		Show_Str(360,65,300,24,"�ر�",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.AutoStandbyStatus = TRUE;
			Show_Str(360,65,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.AutoStandbyStatus = FALSE;
			Show_Str(360,65,300,24,"�ر�",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			if(SysParaSetInfo.AutoStandbyStatus)
				Show_Str(360,65,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			else
				Show_Str(360,65,300,24,"�ر�",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			//Show_Str(290,170,300,24,"������",24,0,GREEN,BLACK);	
			break;
		}
		delay_ms(10);
	}
}
//�����л���������
void SetLay2UI_InterfaceSwitch(void)
{
	//LCD_Fill(290,65,400,250,BLACK);
	if(SysParaSetInfo.InterfaceSwitch)
		Show_Str(360,89,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	else
		Show_Str(360,89,300,24,"�ر�",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.InterfaceSwitch = TRUE;
			Show_Str(360,89,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.InterfaceSwitch = FALSE;
			Show_Str(360,89,300,24,"�ر�",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			//Show_Str(290,170,300,24,"������",24,0,GREEN,BLACK);	
			break;
		}
		delay_ms(10);
	}
}
//������ʱʱ������
void SetLay2UI_AutoStandbyTime(void)
{
	if(SysParaSetInfo.InterfaceSwitch)
		Show_Str(360,89,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	else
		Show_Str(360,89,300,24,"�ر�",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
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
			//����д����ʶ���Ҫ�Ĵ���
			break;
		}
		delay_ms(10);
	}
}
//�豸��������
void SetLay2UI_DevicePower(void)
{
	if(SysParaSetInfo.Power_5V_Status)
		Show_Str(360,65,300,24,"�ѿ�",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
	else
		Show_Str(360,65,300,24,"�ѹ�",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	//LCD_ShowNum(300,170,SysParaSetInfo.Power_5V_Status,2,24,1,WHITE,BLUE);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.Power_5V_Status =1;
			Show_Str(360,65,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);		
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.Power_5V_Status =0;
			Show_Str(360,65,300,24,"�ر�",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
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
//����ʶ�𿪹�����
void SetLay2UI_SpeechRecgSw(void)
{
	if(SysParaSetInfo.Power_5V_Status)
		Show_Str(360,65,300,24,"�ѿ�",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
	else
		Show_Str(360,65,300,24,"�ѹ�",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if(SysParaSetInfo.SpeechRecognize.Speech_Sw)
		Show_Str(360,89,300,24,"�ѿ�",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	else
		Show_Str(360,89,300,24,"�ѹ�",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SpeechRecognize.Speech_Sw = 1;
			Show_Str(360,89,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SpeechRecognize.Speech_Sw = 0;
			Show_Str(360,89,300,24,"�ر�",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			//����д����ʶ���Ҫ�Ĵ���
			break;
		}
		delay_ms(10);
	}
}
//������������
void SetLay2UI_SpeechRecgVol(void)
{
	if(SysParaSetInfo.SpeechRecognize.Speech_Sw)
		Show_Str(360,89,300,24,"�ѿ�",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	else
		Show_Str(360,89,300,24,"�ѹ�",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
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
			//����д����ʶ���Ҫ�Ĵ���
			break;
		}
		delay_ms(10);
	}
}
//SD��ʼ��¼ʱ��������
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
			//Show_Str(290,170,300,24,"��  ",24,0,WHITE,BLUE);	
			//LCD_ShowNum(360,205,SysParaSetInfo.SDRemTimeUnit,4,24,1,GRAY,BLACK);
			LCD_ShowNum(365,65,SysParaSetInfo.SDRemTimeUnit,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SDRemTimeUnit --;
			if(SysParaSetInfo.SDRemTimeUnit<=1 || SysParaSetInfo.SDRemTimeUnit>=59)
				SysParaSetInfo.SDRemTimeUnit = 1;
			//Show_Str(290,170,300,24,"�ر�  ",24,0,GRAY,BLUE);
			LCD_ShowNum(365,65,SysParaSetInfo.SDRemTimeUnit,2,24,1,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);		
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			//Show_Str(250,170,300,24,"������",24,0,GREEN,BLACK);	
			break;
		}
		delay_ms(10);
	}
}
//�������ϴ�ʱ��������
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
			//Show_Str(290,170,300,24,"�ر�  ",24,0,GRAY,BLUE);
			LCD_ShowNum(365,89,SysParaSetInfo.SensorSamplingTimeUnit,2,24,1,WHITE,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);		
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			Show_Str(250,170,300,24,"������",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			break;
		}
		delay_ms(10);
	}
}
//������ֵ
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
//����WIFI���Ƿ�ʹ������WIFI״̬
void LinkWifi_Enable(void)
{
	if((SysParaSetInfo.SensorSwitch&0X04) == 0X04)
	{
//		if(Esp8266Config.WifiLinkSuccFlag || !Esp8266InitFinishFlag)  //�Ѿ����ӹ�WIFI��
//		{
//			if(Esp8266Config.WifiLinkSuccFlag)
//				Show_Str(300,200,300,24,"������  ",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
//			else if(!Esp8266InitFinishFlag)
//				Show_Str(300,200,300,24,"��������",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
//			Encoder_Type = ENCODER_PRESSED;
//			return;
//		}
//		else
			Esp8266_Config();
	}
	else if((SysParaSetInfo.SensorSwitch&0X04) != 0X04)
	{
		Esp8266_DisConectServer();  //�Ͽ�TCP������
		Esp8266_ReStart();   //����
		delay_ms(3000);
		Esp8266Config.WifiLinkSuccFlag = 0;
		Encoder_Type = ENCODER_PRESSED;
	}
	return;
}
//����WIFIʹ��״̬
void SetLay2UI_WifiStatus(void)
{
	Show_Str(250,170,300,24,"WIFI״̬:",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if((SysParaSetInfo.SensorSwitch&0X04) == 0X04)
		Show_Str(360,170,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
	else 
		Show_Str(360,170,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
	Show_Str(250,70,190,16,"���ú󳤰����ö̰�����",16,0,GRAY,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	DispWifiInfoFlag = 1;
	while(1)
	{
		if(Encoder_Type == ENCODER_FORWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SensorSwitch |= 1<<2;  	 //��λ
			Show_Str(360,170,300,24,"��",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);	
		}
		else if(Encoder_Type == ENCODER_BACKWORD)
		{
			Encoder_Type = ENCODER_VOID;
			SysParaSetInfo.SensorSwitch &= ~(1<<2);  //���
			Show_Str(360,170,300,24,"��",24,0,GRAY,SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR);
		}
		else if(Encoder_Type == ENCODER_PRESSED)
		{
			LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR); //�˳��������ʾ 
			Show_Str(300,200,300,24,"������...",24,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);	
			DispWifiInfoFlag = 1;
			break;
		}
		delay_ms(10);
	}
}
//�������������в˵�
void MenuOperate(void)	
{
	if(!StandbyMode)
	{
		switch(Encoder_Type)
		{
			case  ENCODER_FORWORD: 	 //��ת
			{
				EncoderFuncIndex = EncoderTab[EncoderFuncIndex].EncoderForwordStatus;  	//ȷ�ϰ���
				Encoder_Type = ENCODER_VOID;
			}break;
			case  ENCODER_BACKWORD:   //��ת
			{
				EncoderFuncIndex = EncoderTab[EncoderFuncIndex].EncoderBackwordStatus;		//���ϰ���s2
				Encoder_Type = ENCODER_VOID;
			}break;
			case  ENCODER_PRESSED:    //����
			{
				EncoderFuncIndex = EncoderTab[EncoderFuncIndex].EncoderPressStatus; 	//���°���s3
				Encoder_Type = ENCODER_VOID;
			}break; 
			default : 
				break;
			//�˴���Ӱ����������
		}
		if(AutoJumpFlag&&(Current_Show_Interface==Curr_Para_Show||
			Current_Show_Interface==Curr_PM2_5_Show||
			Current_Show_Interface==Curr_WaveForm_Show||
			Current_Show_Interface==Curr_Set_Show))   //���������ص���������
		{
			AutoJumpFlag = 0;
			EncoderFuncIndex = Para_Show;  //��������ʾ����
		}
		EncoderFuncPtr = EncoderTab[EncoderFuncIndex].CurrentOperate; 
		(*EncoderFuncPtr)();//ִ�е�ǰ����
	}
	else
	{
		delay_ms(200);
	}
}
//����µ��ļ���
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
//���عػ�����
void LoadPowerOffUI(uint8_t Mode)
{
	static uint8_t LoadFlag = 0;
	if(!LoadFlag)
	{
		LoadFlag = 1;
		LCD_Fill(145,145,330,205,RED);
		if(Mode == 1)
			Show_Str(175,165,200,24,"�ػ���...",24,0,BLACK,RED);
		else if(Mode == 0)
			Show_Str(175,165,200,24,"������...",24,0,BLACK,RED);
	}
	
}
//���浱ǰ���ݵ�SD��
//��SD��дһ������Ҫ���ֽ���Ϊ13��Byte
void Write_Data_To_SDCard(const uint8_t *filename,uint8_t mode)
{
	UINT bw;
	FIL *F_Sensordata;
	uint8_t *databuf;
	uint8_t res;
	databuf = (uint8_t *)mymalloc(100);  //100���ֽڵĿռ�
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
		res = f_open(F_Sensordata,(const TCHAR*)filename,FA_READ|FA_WRITE);  	//��֮ǰ�е��ļ�
	if(mode==0||res==0x04)  						//�����ʧ���򴴽����ļ�
	{
		Get_New_pname(pname);
		res = f_open(F_Sensordata,(const TCHAR*)filename,FA_WRITE|FA_CREATE_NEW);
	}
	if((SysParaSetInfo.SensorSwitch&0x02) != 0x02)  	//��ȩδʹ��
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
	if(res==FR_OK)  //��������ɹ��Ļ���ʼд������
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
//SD����д����.��,˫�ֽ����Ѿ����Գɹ�
void SDWiriteDataTest(uint8_t *filename,uint8_t mode)
{
	UINT bw;
	FIL *F_testdata;
	uint16_t *databuf;
	uint8_t res;
	databuf = (uint16_t *)mymalloc(sizeof(Testbuf));  //����50���ֽڵĿռ�
	if(databuf==NULL)
	{
		Show_Str(0,100,300,16,"�ڴ�����ʧ��",16,0,GREEN,BLACK);
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
		res = f_open(F_testdata,(const TCHAR*)filename,FA_READ|FA_WRITE);  	//��֮ǰ�е��ļ�
	if(mode==0||res==0x04)  //�����ʧ���򴴽����ļ�
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
	databuf = (uint16_t *)mymalloc(sizeof(Testbuf));  //����50���ֽڵĿռ�
	if(databuf==NULL)
	{
		Show_Str(0,100,300,16,"�ڴ�����ʧ��",16,0,GREEN,BLACK);
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
	Show_Str(0,200,300,16,"���",16,0,GREEN,BLACK);
}
*/


