#include "sys.h"
#define BRAND_NAME 		"Koala"

/**************************ESP8266���ò�����ַ***************************/
#define EEPROM_ADDR_ESP89266_CONFIG_FLAG 	0X0000
#define EEPROM_ADDR_ESP89266_SSID_SECRET 	0X0001
#define EEPROM_ADDR_ESP89266_USERKEY 		0X0097
#define EEPROM_ADDR_ESP89266_SVN 			0X00CA
//ϵͳ������ַ
#define EEPROM_ADDR_SYS_PARA_CONIFG_FLAG  	0X00E9
#define EEPROM_ADDR_SYS_PARA_BACKLIGHT 		0X00EA
#define EEPROM_ADDR_SYS_PARA_SETPARA 		0X00EC
#define EEPROM_ADDR_SYS_PARA_CITYINFO       0X010A  //������Ϣ
/************************************************************************/

#define PARA_SHOW_INTERFACE_BACKCOLOR			BLACK
#define PMCNT_SHOW_INTERFACE_BACKCOLOR			BLACK
#define WAVE_SHOW_INTERFACE_BACKCOLOR			BLACK
#define FillColor  								BLACK  //�ұ�����ĵ�ɫ
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

//��ص�ѹ��������ΪBAT_RES1����λ����һ��������һ��Ҫ�����ͬһ��λ
#define BAT_RES1 				30
#define BAT_RES2 				10
#define LI_PRO_MAX_VAL  		3.9   //﮵�����ŵ��ѹ
#define LI_PRO_MIN_VAL  		2.75  //﮵����С�ŵ��ѹ

#define Multiple  	  			-2  		//��ص�ѹ���汶��
#define HCHO_MAKE_UP  			3   		//��ȩ����ֵ

#define PI						3.1415

#define PM2_5_ID				0
#define HCHO_ID					1
#define CARBON_ID				2
#define TEMP_ID					3
#define HUMI_ID					4
#define AIRPRES_ID				5


//�������ʾ�����Ϣ
#define X0 						30          //ԭ������X
#define Y0 						200         //ԭ������Y
#define H_LEN 					280  	    //x��ĳ���
#define V_LEN 					170  	   //Y��ĳ���
#define HSCAL_UNIT 				24  //X�ᵥԪ����Ŀ
#define VSCAL_UNIT 				10  //Y�ᵥԪ�����Ŀ

#define CURSOR    				461  //��ʾ�����������α�

//��ʾ����UI
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
//����״̬����Ĭ���������
#define AUTOSTANDBY_STATUS 		1
#define AUTOSTANDBY_TIME 		1
#define INTERFACE_SWITCCH  		1
#define POWER_5V_STATUS   		1
#define SD_REM_TIME_UNIT     	5
#define SENSOR_SAMPLING_UNIT 	1
#define SENSOR_POWER_SWITCH     1  //CO2��HCHOȫ���ر�״̬
#define SENSOR_ALARM_STATUS     1  //��������ȫ���ر�״̬
#define THRLD_VAL             	10     //��ֵȫ������Ϊ0
#define BACK_LIGHT           	1   	//����
#define SPEECH_SW             	1
#define SPEECH_VOLUME         	8
//��ǰ����ʾ״̬����
typedef enum
{
	Curr_No_Show=0,
	Curr_Para_Show,
	Curr_PM2_5_Show,
	Curr_WaveForm_Show,
	Curr_Set_Show,
	
	Curr_SetPara_Show,  //��������ϵͳ����������
}Interface_TypeDef;
//���������ýṹ��
//һ����ʾԲ��Ļ�����Ϣ
__packed typedef struct  DISPCIRDEF
{
	uint16_t x0;
	uint16_t y0;	
	uint16_t width;			//�뾶
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
	uint8_t FontSize;//�����С
}DispCirDef;
//һ��Сʱ�����ݼ�¼,ÿ����Ӽ�¼һ��
__packed typedef struct
{
	uint8_t HCHO[14];//��ȩ
	uint16_t CO2[14];//������̼
	uint16_t PM2_5[14];
}DataRemember_TypeDef;

#define MENU_SIZE  43      //�˵����
//����һ���˵����ܵĽṹ��
typedef struct 
{
   uint8_t  EncoderStatusIndex;   //��ǰ��״̬������
   uint8_t  EncoderPressStatus;   	//����ȷ�ϼ�ʱ��״̬������
   uint8_t  EncoderForwordStatus;      //������˳ʱ��ת����״̬������     
   uint8_t  EncoderBackwordStatus;    //��������ʱ��ת����״̬������
   void (*CurrentOperate)(void);      //��ǰ״̬Ӧ��ִ�еĹ��ܲ���
}StatusTab;
//ϵͳ���в������ýṹ��
__packed typedef struct
{
	uint8_t AutoStandbyStatus;      //�Զ���״̬   MicroWaveStatus;		//΢����Ӧ����״̬
	uint8_t AutoStandbyTime;        //�Զ�������ʱ
	uint8_t InterfaceSwitch;        //�����л�����
	uint8_t Power_5V_Status;  		//5V���紫��������
	uint8_t SDRemTimeUnit;			//SD����¼ʱ����(��λ:����)
	uint8_t SensorSamplingTimeUnit;   //�������ϴ�ʱ����(��λ:����)
	uint8_t SensorSwitch;               //��������������  BIT 1:CO2 2:HCHO
	__packed struct      			//��ֵ���ÿ���
	{  
		uint8_t SensorAlarmStatus;  //����������״̬
		uint16_t ThrldVal[5];   	//Ҫ���ڵ���ֵ
	}ThresHoldStatus;
	__packed struct  //����ʶ�����
	{
		uint8_t Speech_Sw;
		uint8_t Speech_Volume;
	}SpeechRecognize;
}SysSetPara_TypeDef;
//�������Ƿ�����״̬��־
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
#define SW_RESERVE 0X00  //����λ
//���ƿ��صĵ�ǰ״̬
//���ù�����
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
extern int16_t BackLightBright;   	//�������ȣ���ֵ��
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



