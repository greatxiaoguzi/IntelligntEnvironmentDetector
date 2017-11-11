#include "sys.h"
#define BRAND_NAME 		"Koala"

/**************************ESP8266配置参数地址***************************/
#define EEPROM_ADDR_ESP89266_CONFIG_FLAG 	0X0000
#define EEPROM_ADDR_ESP89266_SSID_SECRET 	0X0001
#define EEPROM_ADDR_ESP89266_USERKEY 		0X0097
#define EEPROM_ADDR_ESP89266_SVN 			0X00CA
//系统参数地址
#define EEPROM_ADDR_SYS_PARA_CONIFG_FLAG  	0X00E9
#define EEPROM_ADDR_SYS_PARA_BACKLIGHT 		0X00EA
#define EEPROM_ADDR_SYS_PARA_SETPARA 		0X00EC
#define EEPROM_ADDR_SYS_PARA_CITYINFO       0X010A  //城市信息
/************************************************************************/

#define PARA_SHOW_INTERFACE_BACKCOLOR			BLACK
#define PMCNT_SHOW_INTERFACE_BACKCOLOR			BLACK
#define WAVE_SHOW_INTERFACE_BACKCOLOR			BLACK
#define FillColor  								BLACK  //右边区域的底色
#define TOP_TITLE_BACK_COLOR 					BLACK
#define AQI_COLOR  								C_VIOLET
#define WEATHERINFO_COLOR                       C_LIME_GREEN
#define BRAND_NAME_COLOR                        WHITE
#define TEMPHUMI_ALARM_COLOR                    C_VIOLET
#define BOTTOM_TITTLE_BACK_COLOR 				BLACK
#define SET_SHOW_INTERFACE_LEFT_TEXTBACKCOLOR	BLUE
#define SET_SHOW_INTERFACE_LEFT_TEXTCOLOR       BLACK
#define SET_SHOW_INTERFACE_RIGHT_TEXTBACKCOLOR	BLUE
#define SET_SHOW_INTERFACE_LEFT_BACKCOLOR		C_THISTLE
#define SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR      BLACK
#define SET_SHOW_INTERFACE_RIGHT_BACKCOLOR		C_THISTLE

#define LOW_VOL_ALARM_THREHOLD  2800

//电池电压采样电阻为BAT_RES1，单位必须一样，若不一样要换算成同一单位
#define BAT_RES1 				30
#define BAT_RES2 				10
#define LI_PRO_MAX_VAL  		3.9   //锂电池最大放电电压
#define LI_PRO_MIN_VAL  		2.75  //锂电池最小放电电压

#define Multiple  	  			-2  		//电池电压增益倍数
#define HCHO_MAKE_UP  			3   		//甲醛补偿值

#define PI						3.1415

#define PM2_5_ID				0
#define HCHO_ID					1
#define CARBON_ID				2
#define TEMP_ID					3
#define HUMI_ID					4
#define AIRPRES_ID				5


//左边轴显示相关信息
#define X0 						30          //原点坐标X
#define Y0 						200         //原点坐标Y
#define H_LEN 					280  	    //x轴的长度
#define V_LEN 					170  	   //Y轴的长度
#define HSCAL_UNIT 				24  //X轴单元格数目
#define VSCAL_UNIT 				10  //Y轴单元格的数目

#define CURSOR    				461  //显示空气质量的游标

//显示界面UI
#define Para_Show     			0
#define PM2_5_Show    			1
#define WaveForm_Show 			2
#define Set_Show      			3
#define Set_Prar_Show 			4
#define Set_Para_Year 			5
#define Set_Para_Month 			6
#define Set_Para_Day   			7
#define Set_Para_Hour  			8
#define Set_Para_Minute 		9
//开机状态参数默认配置情况
#define AUTOSTANDBY_STATUS 		1
#define AUTOSTANDBY_TIME 		1
#define INTERFACE_SWITCCH  		1
#define POWER_5V_STATUS   		1
#define SD_REM_TIME_UNIT     	5
#define SENSOR_SAMPLING_UNIT 	1
#define SENSOR_POWER_SWITCH     1  //CO2和HCHO全部关闭状态
#define SENSOR_ALARM_STATUS     1  //报警开关全部关闭状态
#define THRLD_VAL             	10     //阈值全部设置为0
#define BACK_LIGHT           	1   	//背光
#define SPEECH_SW             	1
#define SPEECH_VOLUME         	8
//当前的显示状态界面
typedef enum
{
	Curr_No_Show=0,
	Curr_Para_Show,
	Curr_PM2_5_Show,
	Curr_WaveForm_Show,
	Curr_Set_Show,
	
	Curr_SetPara_Show,  //正在设置系统参数界面了
}Interface_TypeDef;
//主界面设置结构体
//一个显示圆框的基本信息
__packed typedef struct  DISPCIRDEF
{
	uint16_t x0;
	uint16_t y0;	
	uint16_t width;			//半径
	uint16_t height;
	int8_t Xoff;
	int8_t Yoff;
	__packed struct ColorInfo
	{
		uint16_t Cir1color;
		uint16_t Cir2color;
		uint16_t Valuecolor;
		uint16_t Textcolor;	
		uint16_t TextBackcolor;
		uint16_t ValueBackcolor;
	}ColorInfo;
	uint8_t *Text_Name;
	uint8_t *Text_UnitMeas;
	uint8_t FontSize;//字体大小
}DispCirDef;
//一个小时的数据记录,每五分钟记录一次
__packed typedef struct
{
	uint8_t HCHO[14];//甲醛
	uint16_t CO2[14];//二氧化碳
	uint16_t PM2_5[14];
}DataRemember_TypeDef;

#define MENU_SIZE  43      //菜单深度
//定义一个菜单功能的结构体
typedef struct 
{
   uint8_t  EncoderStatusIndex;   //当前的状态索引号
   uint8_t  EncoderPressStatus;   	//按下确认键时的状态索引号
   uint8_t  EncoderForwordStatus;      //编码器顺时针转动的状态索引号     
   uint8_t  EncoderBackwordStatus;    //编码器逆时针转动的状态索引号
   void (*CurrentOperate)(void);      //当前状态应该执行的功能操作
}StatusTab;
//系统所有参数设置结构体
__packed typedef struct
{
	uint8_t AutoStandbyStatus;      //自动待状态   MicroWaveStatus;		//微波感应开关状态
	uint8_t AutoStandbyTime;        //自动待机延时
	uint8_t InterfaceSwitch;        //界面切换开关
	uint8_t Power_5V_Status;  		//5V供电传感器开关
	uint8_t SDRemTimeUnit;			//SD卡记录时间间隔(单位:分钟)
	uint8_t SensorSamplingTimeUnit;   //服务器上传时间间隔(单位:分钟)
	uint8_t SensorSwitch;               //传感器开关设置  BIT 1:CO2 2:HCHO
	__packed struct      			//阈值设置开关
	{  
		uint8_t SensorAlarmStatus;  //传感器报警状态
		uint16_t ThrldVal[5];   	//要调节的阈值
	}ThresHoldStatus;
	__packed struct  //语音识别参数
	{
		uint8_t Speech_Sw;
		uint8_t Speech_Volume;
	}SpeechRecognize;
}SysSetPara_TypeDef;
//传感器是否在线状态标志
typedef union
{
	uint8_t Status;
	struct
	{
		uint8_t PMS_5:		1;
		uint8_t WZ_S:		1;
		uint8_t SENSAIR:	1;
		uint8_t SHT30:		1;
		uint8_t BMP180:		1;
		uint8_t Reserve:	3;	
	}Device;
}SensorOnlineStat_TypeDef;
extern SensorOnlineStat_TypeDef SensorOnline; 
extern uint8_t AlarmCloseFlag;
extern uint8_t Power_5V_IO_Status;
/*
#define SW1_ON   1<<0
#define SW2_ON   1<<1
#define SW3_ON   1<<2
#define SW4_ON   1<<3
#define SW5_ON   1<<4
#define SW_RESERVE 0X00  //保留位
//控制开关的当前状态
//采用共用体
typedef union
{
	uint8_t All_SW_status;
	struct
	{
		uint8_t SW1:1;
		uint8_t SW2:1;
		uint8_t SW3:1;
		uint8_t SW4:1;
		uint8_t SW5:1;
		uint8_t reserve:3;
	}bits;
}CtrlSwBuf;
*/
extern Interface_TypeDef Current_Show_Interface;
extern int16_t BackLightBright;   	//背光亮度，数值型
extern uint8_t AutoJumpFlag;
extern SysSetPara_TypeDef SysParaSetInfo; 
extern uint8_t StandbyMode;
extern uint8_t DispWifiInfoFlag;

void ChangeUI(void);
void LoadColorPat(void);
void DispCirInfo(DispCirDef const *DispCirInfo,uint8_t Item);
void EnvirQuaty(uint16_t x,uint16_t y);
void MenuOperate(void);
uint16_t CalColorValue(uint16_t Value,uint8_t IdItem);
void FillThemeBackColor(DispCirDef const *DispCirInfo, uint8_t Item,uint16_t Color);


void Get_New_pname(uint8_t *pname);
//void aWake_Standby(void);
void Adjust_Back_Light(void);
void ReadSetPara(void);
void LoadSensorData(void);
void Save_Hour_Data(uint8_t Order);
void DispTimeToUI(void);
void TestSDCard(void);
void TestPm2_5(void);
void Write_Data_To_SDCard(const uint8_t *filename,uint8_t mode);
void LoadPowerOffUI(uint8_t Mode);
void DispWeatherInfo(void);

//void LoadInfoStruct(void);
void Load_ParaShow_Interface_1(void);
void Load_PMCnctrShow_Interface_2(void);
void Load_WaveformShow_Interface_3(void);
void Load_SetShow_Interface_4(void);
 
void SetLay1UI_Time(void);
void SetLay1UI_AutoStandby(void);
void SetLay1UI_DeviceSW(void);
void SetLay1UI_DataRemUnit(void);
void SetLay1UI_Threshold(void);
void SetLay1UI_Quit(void);
void SetLay1UI_WIFIConfig(void);


void SetLay2UI_Time_Year(void);
void SetLay2UI_Time_Month(void);
void SetLay2UI_Time_Day(void);
void SetLay2UI_Time_Hour(void);
void SetLay2UI_Time_Minute(void);

void SetLay3UI_ThresHold_Status(void);
void SetLay3UI_ThresHold_Adjust(void);
void SetLay3UI_Sensor_Status(void);

void Set_HCHOSensor_Status(void);
void Set_CO2Sensor_Status(void);

void Set_PM2_5_Threshold(void);
void Set_CO2_Threshold(void);
void Set_HCHO_Threshold(void);
void Set_Temp_Threshold(void);
void Set_Humi_Threshold(void);

void Adjust_Humi_Val(void);
void Adjust_Temp_Val(void);
void Adjust_CO2_Val(void);
void Adjust_HCHO_Val(void);
void Adjust_PM2_5_Val(void);
void LinkWifi_Enable(void);


void SetLay2UI_AutoStandby(void);
void SetLay2UI_InterfaceSwitch(void);
void SetLay2UI_AutoStandbyTime(void);
void SetLay2UI_DevicePower(void);
void SetLay2UI_SDRemUnit(void);
void SetLay2UI_ThresHold(void);
void SetLay2UI_SensoeSamplingUnit(void);
void SetLay2UI_WifiStatus(void);
void SetLay2UI_SpeechRecgSw(void);
void SetLay2UI_SpeechRecgVol(void);
void SetLay2UI_AutoStandbyTime(void);



