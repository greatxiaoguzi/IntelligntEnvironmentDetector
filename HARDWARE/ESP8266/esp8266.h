#ifndef _ESP8266_H
#define _ESP8266_H
#include "include.h"

#define Esp8266SendInfo  printf
#define WaitTime  2000  //等待超时时间
#define FLASH_SAVE_ADDR  0X08020000 				//设置FLASH 保存地址(必须为偶数)
__packed typedef struct
{
	uint8_t  WireNet[2][20];
	uint8_t  ComunitPro[4];
	uint8_t  ServerIP[15];
	uint16_t ServerPort;
//	uint8_t  ConectFlag;
	uint8_t  SlrID;
	uint8_t  WifiStartFlag;
	uint8_t  WifiLinkSuccFlag;  	//wifi连接成功标志
	uint8_t  ServerLinkStat;       	//服务器状态
}Esp8266;
//网络接受到的数据结构体
typedef struct
{
	uint8_t Time_Week[6];          //星期信息
	int8_t  Weather_TempInfo;  	   //温度信息
	uint8_t Weather_CityInfo[10];   //城市信息
	uint8_t Weather_Climate[8];   //天气信息
}NetWorkInfo_TypeDef;
//联网工作模式
typedef enum
{
	GET_WEATHER_MODE = 0,
	GET_NETWORK_TIME_MODE,
	UP_LOAD_SENSOR_DATA_MODE
}NetMode_TypeDef;
//天气信息
#define APIKEY 				 	"x5yd6spopsuraute"
#define WEATHER_ADDRESS    		"api.seniverse.com"
#define WEATHER_PORT       		80
//获取网络世间信息
#define TIME_ADDRESS			"api.k780.com"
#define TIME_PORT				88

//获取天气链接信息
typedef struct 
{
	uint8_t *ipAddress;
	uint32_t    port;
}Esp8266WeatherConnectInfo_TypeDef;
extern NetWorkInfo_TypeDef NetWorkDataInfo;

extern uint8_t Esp8266InitFinishFlag;
extern uint8_t AcuireFinishFlag;
extern uint8_t WeatherAcquireStatus;  //天气数据获取状态
extern uint8_t TimeAcquireStatus; 
//服务器密钥
extern uint8_t UserKey[60];
extern Esp8266 Esp8266Config;
extern uint8_t WifiSetFlag;
uint8_t LoadParaToEeprom(void);
uint8_t Esp8266_Config(void);
uint8_t Esp8266_ReStart(void);
uint8_t  Esp8266_SetMode(uint8_t Mode);
uint8_t Esp8266_CheckNET(void);
uint8_t Esp8266_BackIPAdd(void);
uint8_t Esp8266_ConnectWire(void);
uint8_t Esp8266_OpenMoreConect(void);
uint8_t Esp8266_ConectServer(NetMode_TypeDef LinkMode);
void TryReConectServer(void);
uint8_t Esp8266_DisConectServer(void);
void ConfigServerMode(void);
uint8_t Esp8266_GetServerLinkStat(void);
void TryReConectWifi(void);
uint8_t ReadServerPar(void);
uint8_t  Esp8266_SetSingleLink(void);
uint8_t  Esp8266_SetPassThrough(uint8_t mode);
uint8_t  Esp8266_QuitPassThroughLinkServer(void);
uint8_t Esp8266_SetNoPassThrough(void);
void UpdateWeatherInfo(void);
uint8_t Esp8266GetWeatherInfo(const uint8_t *host,const uint8_t *apikey);
uint8_t Esp8266GetBeijingInfo(void);
void AdjustSysTime(uint8_t *Data);
uint8_t PkgURL(uint8_t GateWay,uint8_t *UserKey);



uint8_t ListenRec(void);
void ClearBuffer(uint8_t *data);
uint8_t Hand(const char *Tars,uint16_t Time);
uint8_t SpecStrAdj(uint8_t *string);
uint8_t WifiNETSet(void);
void NETDataDeal(void);
uint8_t GetIPAddr(void);

uint8_t Esp8266_Send_Cmd(uint8_t *Cmd,const uint8_t *Ack,uint16_t waittime);
uint8_t Esp8266_SendData(uint8_t *Data,const uint8_t *Ack,uint16_t waittime);
uint8_t *Esp8266_Check_Cmd(const uint8_t *str);

#endif



