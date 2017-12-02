#ifndef _ESP8266_C
#define _ESP8266_C	
#include "include.h"	
//STMflash读写部分申明定义
//uint8_t Flash_Buffer[50];      //flash缓冲区50个字节
//#define SIZE sizeof(Flash_Buffer)	 			  	//数组长度

//01004566
//乐为IP:121.40.22.15
//esp8266Wifi模块驱动程序
//配置网络通信协议，IP地址，端口号
//%SSID=HiWiFi_0F06D8&%sundayenjoy..1234&#
//%USERKEY=570b6146c6e242018da2ce6c9239627b&#
//乐为服务器密钥
uint8_t UserKey[60];
Esp8266 Esp8266Config={{"",""},{"TCP"},{""},8888,0,1,0,0};           //结构体实体化
uint8_t ReConfigFlag = 0XFF;  //是否重新配置WIFI

uint8_t Esp8266InitFinishFlag = 0;
uint8_t AcuireFinishFlag = 0;     //是否从网络获取过天气信息和时间信息
uint8_t WeatherAcquireStatus = 0;  //天气数据获取状态
uint8_t TimeAcquireStatus = 0;     //时间信息获取状态
//网络天气预报信息
NetWorkInfo_TypeDef NetWorkDataInfo = 
{
	{0},
	 0,
	{0},
	{0}
};  				//从互联网上获取到的信息数据结构
//读取SD卡参数到EEPROM中
uint8_t LoadParaToEeprom(void)
{
	UINT br;
	FIL *F_configdata;
	uint8_t *databuf;
	uint8_t res;
	char  *p1=0;
	uint8_t ConfigWriteFlag = 0XF0;
	databuf = (uint8_t *)mymalloc(sizeof(150)); 
	if(databuf==NULL)
		return 1;
	F_configdata = (FIL *)mymalloc(sizeof(FIL));
	if(F_configdata==NULL)
	{
		myfree(databuf);
		return 1;
	}
	Show_Str(100,45,200,24,"WIFI参数加载中",24,0,BLUE,BLACK);
	res=f_open(F_configdata,(const TCHAR*)"0:wificonfigfile.ini",FA_READ);
	if(res == 0)  	//打开成功
	{
		if(f_eof(F_configdata) == 1)
		{
			f_close(F_configdata);
			myfree(F_configdata);
			myfree(databuf);
			return 1;
		}
	}
	else
	{
		f_close(F_configdata);
		myfree(F_configdata);
		myfree(databuf);
		return 1;
	}
	if(f_gets((TCHAR*)databuf,150,F_configdata) != NULL)   //第一行
	{
		Show_Str(100,80,200,16,"配置SSID->",16,0,WHITE,BLACK);
		Show_Str(220,80,200,16,"写入中",16,0,WHITE,BLACK);
		p1 = strstr((const char*)databuf,(const char*)"%SSID");
		AT24CXX_Write(EEPROM_ADDR_ESP89266_SSID_SECRET,(uint8_t*)p1,150);  		//保存wifi的SSID和密码
		ConfigWriteFlag |= (1<<0);
		Show_Str(220,80,200,16,"已配置",16,0,GREEN,BLACK);
		Show_Str(100,100,200,16,"配置USERKEY->",16,0,WHITE,BLACK);
		memset(databuf,0,100);
	}
	else
	{
		f_close(F_configdata);
		myfree(F_configdata);
		myfree(databuf);
		return 1;
	}
	if(f_gets((TCHAR*)databuf,150,F_configdata) != NULL)		//第二行
	{
		Show_Str(220,100,200,16,"写入中",16,0,WHITE,BLACK);
		p1 = strstr((const char*)databuf,(const char*)"%USERKEY");
		AT24CXX_Write(EEPROM_ADDR_ESP89266_USERKEY/*151*/,(uint8_t*)p1,50);
		ConfigWriteFlag |= (1<<1);
		Show_Str(220,100,200,16,"已配置",16,0,GREEN,BLACK);
		Show_Str(100,120,200,16,"配置ServerIP->",16,0,WHITE,BLACK);
		memset(databuf,0,100);
	}
	else
	{
		f_close(F_configdata);
		myfree(F_configdata);
		myfree(databuf);
		return 1;
	}
	if(f_gets((TCHAR*)databuf,150,F_configdata) != NULL)		//第三行
	{
		Show_Str(220,120,200,16,"写入中",16,0,WHITE,BLACK);
		p1 = strstr((const char*)databuf,(const char*)"%SVN");
		AT24CXX_Write(EEPROM_ADDR_ESP89266_SVN/*202*/,(uint8_t*)p1,30);
		ConfigWriteFlag |= (1<<2);
		AT24CXX_WriteOneByte(0,ConfigWriteFlag);
		Show_Str(220,120,200,16,"已配置",16,0,GREEN,BLACK);
		Show_Str(160,140,200,16,"参数如下",16,0,GREEN,BLACK);
		memset(databuf,0,100);
	}
	else
	{
		f_close(F_configdata);
		myfree(F_configdata);
		myfree(databuf);
		return 1;
	}
	if(f_gets((TCHAR*)databuf,150,F_configdata) != NULL)		//第三行   %CITY=xian&#
	{
		p1 = strstr((const char*)databuf,(const char*)"%CITY");
		AT24CXX_Write(EEPROM_ADDR_SYS_PARA_CITYINFO,(uint8_t*)p1,30);
		memset(databuf,0,100);
	}
	else
	{
		f_close(F_configdata);
		myfree(F_configdata);
		myfree(databuf);
		return 1;
	}
	AT24CXX_Read(EEPROM_ADDR_ESP89266_SSID_SECRET,databuf,150);
	Show_Str(80,180,300,16,databuf,16,0,WHITE,BLACK);
	AT24CXX_Read(EEPROM_ADDR_ESP89266_USERKEY,databuf,50);
	Show_Str(80,205,300,16,databuf,16,0,WHITE,BLACK);
	AT24CXX_Read(EEPROM_ADDR_ESP89266_SVN,databuf,30);
	Show_Str(80,230,300,16,databuf,16,0,WHITE,BLACK);
	AT24CXX_Read(EEPROM_ADDR_SYS_PARA_CITYINFO,databuf,30);
	Show_Str(80,255,300,16,databuf,16,0,WHITE,BLACK);
	f_close(F_configdata);
	myfree(F_configdata);
	myfree(databuf);
}
//开机配置Esp8266模块，检测是否在位成功
//功能:开机先读取FLASH，得到值后开始配置连接，否则如果收到上位机发过来的数据，则开始连接
uint8_t Esp8266_Config(void)
{
	uint8_t i;
	uint8_t ErrorType;
    uint8_t SuccesFlag = 0;  		//连接成功标志
    uint8_t LinkTimes = 0;
	if(Current_Show_Interface == Curr_SetPara_Show && DispWifiInfoFlag)
		LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)==0)  //如果要重新配置的话执行
	{
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)==0)
		{
			delay_ms(2000);
			if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)==0)  //长按的话不再配置
				return 1;
		}
	}
	if(Current_Show_Interface == Curr_SetPara_Show && DispWifiInfoFlag)
		LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
    if(!Esp8266_ReStart())  //发送重新启动信号
    {
		if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
			Show_Str(260,70,200,16,"WIFI重启成功.",16,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
        Esp8266Config.WifiStartFlag = 1;
    }
	else
	{
		if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
		{
			Show_Str(260,70,200,16,"WIFI重启失败.",16,0,RED,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			Encoder_Type = ENCODER_PRESSED;
		}
        Esp8266Config.WifiStartFlag = 0;
		Esp8266InitFinishFlag = 1;
		return 1;
	}
	delay_ms(2000);
	delay_ms(1000);
    if(!Esp8266_SetMode(1))   //设置模式(AP和STA共存模式)
	{
		if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
			Show_Str(260,90,200,16,"TCP模式设置成功.",16,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	}
	else
	{
		if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
		{
			Show_Str(260,90,200,16,"TCP模式设置失败.",16,0,RED,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			Encoder_Type = ENCODER_PRESSED;
		}
		Esp8266InitFinishFlag = 1;
        return 1;
	}
	delay_ms(2000);
    //开始配置WIFI模块
	if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
		Show_Str(260,150,200,16,"WIFI设置中...",16,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(LinkTimes<2) //最多连3次
	{
		 LinkTimes ++;
		 if(!WifiNETSet()&&SuccesFlag==0)      //进入设置，如果设置成功的话，执行相应部分
		 {
             SuccesFlag = 1;
			 if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
				Show_Str(260,150,200,16,"WIFI设置成功",16,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		 }
		 else
		 {
			 if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
				Show_Str(260,150,200,16,"重新设置中.",16,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		 }
		 if(SuccesFlag==1)             //连接成功的话
		 {
			 if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
				Show_Str(260,170,200,16,"WIFI连接中...",16,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
				if(!Esp8266_ConnectWire())   //开始连接WIFI，连接成功的话
				{
					Esp8266Config.WifiLinkSuccFlag = 1;  //标志WIFI连接成功
					if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
						Show_Str(260,170,200,16,"WIFI连接成功",16,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
					for(i=0;i<2;i++)
					{
						 LED0 = !LED0;
						 delay_ms(500);
					}
					break;
				}
			 	else//否则连接未成功
				{
					SuccesFlag=0;
					Esp8266Config.WifiLinkSuccFlag = 0;
					if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
						Show_Str(260,170,200,16,"WIFI连接失败",16,0,RED,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
				}
		 }
	}
	if(LinkTimes>=3)
	{
		if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
			Show_Str(260,170,200,16,"WIFI连接失败",16,0,RED,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	}
////////////////////////////////////////////////////////////////////
	if(AcuireFinishFlag==0 && Current_Show_Interface != Curr_Set_Show)  			//在设置参数的情况下不进入
	{
		Esp8266_QuitPassThroughLinkServer();
		Esp8266_SetPassThrough(1);
		for(uint8_t i=0;i<4;i++)
		{
			//获取天气信息
			if(!Esp8266GetWeatherInfo(WEATHER_ADDRESS,APIKEY))  //获取网络天气预报
			{
				WeatherAcquireStatus = 1;
				break;
			}
			else
			{
				WeatherAcquireStatus = 0;
				continue;
			}
		}
		Esp8266_QuitPassThroughLinkServer();
		Esp8266_DisConectServer();
		Esp8266_SetPassThrough(1);
		//获取网络时间信息并且校准
		for(uint8_t i=0;i<4;i++)
		{
			if(!Esp8266GetBeijingInfo())
			{
				TimeAcquireStatus = 1;
				break;
			}
			else
			{
				TimeAcquireStatus = 0;
				continue;
			}
		}
		Esp8266_QuitPassThroughLinkServer();
		Esp8266_DisConectServer();
		Esp8266_SetPassThrough(1);
		AcuireFinishFlag = 1;
	}
	//LCD_Clear(BLACK);   //清屏
	if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
		Show_Str(260,210,200,16,"服务器连接中...",16,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	ErrorType = ReadServerPar();
	if(!ErrorType)   //从EEPROM读取服务器传参数
	{
		if(!Esp8266_ConectServer(UP_LOAD_SENSOR_DATA_MODE))
		{
			if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
				Show_Str(260,210,200,16,"服务器连接成功",16,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		}
		else
		{
			if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
			{
				Show_Str(260,210,200,16,"服务器连接失败",16,0,RED,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
				Encoder_Type = ENCODER_PRESSED;
			}
			delay_ms(500);
			Esp8266InitFinishFlag = 1;
			return 1;
		}
	}
	else
	{
		if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
			Show_Str(60,30,200,16,"参数读取错误  ",16,0,RED,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		//LCD_ShowNum(60,30,ErrorType,2,16,RED,BLACK);
	}
	if(PkgURL(2,(uint8_t*)UserKey))
	{
		if(Current_Show_Interface == Curr_SetPara_Show)
			Encoder_Type = ENCODER_PRESSED;
		Esp8266InitFinishFlag = 1;
		return 1;
	}
	Esp8266InitFinishFlag = 1;
	ClearBuf();
	if(Current_Show_Interface == Curr_SetPara_Show)
		Encoder_Type = ENCODER_PRESSED;
}
//对获取到的天气数据进行分析
//cJSON数据缓存的起始地址
void HandleWeathrerDataInfo(char *pMsg)
{
	uint8_t i;
	cJSON *root;
	cJSON *pSub;
	cJSON *arrayItem;
	cJSON *pItem;
	cJSON *pSubItem;
	cJSON *pChildItem;
	
	char *pr,*utf8str,*gbkstr;
	int len;
	uint8_t res;
	
	root = mymalloc(sizeof(cJSON));
	pSub = mymalloc(sizeof(cJSON));
	arrayItem = mymalloc(sizeof(cJSON));
	pItem = mymalloc(sizeof(cJSON));
	pSubItem = mymalloc(sizeof(cJSON));
	pChildItem = mymalloc(sizeof(cJSON));
	
	pr = mymalloc(1000);
	utf8str = mymalloc(50);
	gbkstr = mymalloc(50);
	
	memset(pr,0,1000);
	memset(gbkstr,0,50);
	memset(utf8str,0,50);
	
	if(pMsg == NULL)
	{
		return;
	}
	root = cJSON_Parse(pMsg);  //得到根节点
	if(NULL != root)                       //Show_Str(0,0,480,12,(uint8_t *)gbkstr,12,0,BLUE,BLACK);                                                                  
    {
		pSub = cJSON_GetObjectItem(root,"results");
		if(pSub != NULL)
		{
			arrayItem = cJSON_GetArrayItem(pSub,0);
			pr = cJSON_Print(arrayItem);
			pItem = cJSON_Parse(pr);
			if(pItem != NULL)
			{
				pSubItem = cJSON_GetObjectItem(pItem,"location");
				if(pSubItem != NULL)
				{
					pChildItem = cJSON_GetObjectItem(pSubItem,"name");  //城市名称
					if(pChildItem != NULL)
					{
						utf8str = pChildItem->valuestring;
						SwitchToGbk((const uint8_t*)utf8str,strlen(utf8str),(uint8_t *)gbkstr,&len);
						strcpy(NetWorkDataInfo.Weather_CityInfo,gbkstr);
						//Show_Str(0,0,480,16,NetWorkDataInfo.Weather_CityInfo,16,0,BLUE,BLACK);
					}
				}
				memset(utf8str,0,50);
				memset(gbkstr,0,50);
				pSubItem = cJSON_GetObjectItem(pItem,"now");
				if(pSubItem != NULL)
				{
					pChildItem = cJSON_GetObjectItem(pSubItem,"text");  //天气信息
					if(pChildItem != NULL)
					{
						utf8str = pChildItem->valuestring;
						SwitchToGbk((const uint8_t*)utf8str,strlen(utf8str),(uint8_t *)gbkstr,&len);
						strcpy(NetWorkDataInfo.Weather_Climate,gbkstr);
						//Show_Str(0,20,480,16,NetWorkDataInfo.Weather_Climate,16,0,BLUE,BLACK);
					}
					pChildItem = cJSON_GetObjectItem(pSubItem,"code");  //天气信息代码
					if(pChildItem != NULL)
					{
						gbkstr = pChildItem->valuestring;
						//Show_Str(0,40,480,16,(uint8_t *)gbkstr,16,0,BLUE,BLACK);
					}
					pChildItem = cJSON_GetObjectItem(pSubItem,"temperature");   //温度
					if(pChildItem != NULL)
					{
						gbkstr = pChildItem->valuestring;
						NetWorkDataInfo.Weather_TempInfo = strToint((uint8_t *)gbkstr);
						//LCD_ShowNum(0,60,NetWorkDataInfo.Weather_TempInfo,2,16,0,BLUE,BLACK);
					}
				}
			}
			cJSON_Delete(pItem);
		}
	}
	cJSON_Delete(root);
	myfree(root);
	myfree(pSub);
	myfree(arrayItem);
	myfree(pItem);
	myfree(pSubItem);
	myfree(pChildItem);
	myfree(pr);
	myfree(utf8str);
	myfree(gbkstr);
}
//解析时间数据
//格式     2017-10-22 22:38:37
void AdjustSysTime(uint8_t *Data)
{
	SysTimeData.Year = (Data[2] - 0x30)*10 + (Data[3]-0x30);
	SysTimeData.Month = (Data[5]-0x30)*10 + (Data[6]-0x30);
	SysTimeData.Day = (Data[8]-0x30)*10 + (Data[9]-0x30);
	SysTimeData.Hour = (Data[11]-0x30)*10 + (Data[12]-0x30);
	SysTimeData.Minute = (Data[14]-0x30)*10 + (Data[15]-0x30);
	SysTimeData.Second = (Data[17]-0x30)*10 + (Data[18]-0x30);
	Set_Rx8025t_Time(SysTimeData.Year,SysTimeData.Month,SysTimeData.Day,SysTimeData.Hour,SysTimeData.Minute,SysTimeData.Second);   //开始校准时间
}
void HandleTimeDataInfo(char *pMsg)
{
	cJSON *root;
	cJSON *pSub;
	cJSON *pChildItem;
	
	char *utf8str,*gbkstr;
	
	root = mymalloc(sizeof(cJSON));
	pSub = mymalloc(sizeof(cJSON));
	pChildItem = mymalloc(sizeof(cJSON));
	utf8str = mymalloc(50);
	gbkstr = mymalloc(50);
	int len;
	memset(gbkstr,0,50);
	memset(utf8str,0,50);
	if(pMsg == NULL)
	{
		return;
	}
	root = cJSON_Parse(pMsg);  //得到根节点
	if(root != NULL)
	{
		pSub = cJSON_GetObjectItem(root,"result");
		if(pSub != NULL)
		{
			pChildItem = cJSON_GetObjectItem(pSub,"datetime_1");
			if(pChildItem != NULL)
			{
				utf8str = pChildItem->valuestring;
				SwitchToGbk((const uint8_t*)utf8str,strlen(utf8str),(uint8_t *)gbkstr,&len);
				AdjustSysTime((uint8_t *)gbkstr);
				//Show_Str(0,100,480,16,(uint8_t *)gbkstr,16,0,BLUE,BLACK);
			}
			memset(gbkstr,0,50);
			pChildItem = cJSON_GetObjectItem(pSub,"week_3");
			if(pChildItem != NULL)
			{
				utf8str = pChildItem->valuestring;
				SwitchToGbk((const uint8_t*)utf8str,strlen(utf8str),(uint8_t *)gbkstr,&len);
				strcpy(NetWorkDataInfo.Time_Week,gbkstr);
				//Show_Str(0,120,480,16,(uint8_t *)gbkstr,16,0,BLUE,BLACK);
			}
		}
	}
	myfree(root);
	myfree(pSub);
	myfree(pChildItem);
	myfree(utf8str);
	myfree(gbkstr);
}
//ESP8266 获取网络天气信息
uint8_t Esp8266GetWeatherInfo(const uint8_t *host,const uint8_t *apikey)
{
	char buf[200];
	char bufcity[20];
	uint8_t len;
	uint8_t i;
	char *p1=0;
	ClearBuf();  //清楚缓存
	for(i=0;i<2;i++)
	{
		if(!Esp8266_ConectServer(GET_WEATHER_MODE))
			break;
		else
			continue;
	}
	AT24CXX_Read(EEPROM_ADDR_SYS_PARA_CITYINFO,buf,30);
	p1 = strstr((const char*)buf,(const char*)"%CITY") + 6;
	len = strstr((const char*)buf,(const char*)"&#") - (strstr((const char*)buf,(const char*)"%CITY")+6);
	for(uint8_t i=0;i<len;i++)
		bufcity[i] = *(p1+i);
	bufcity[len] = '\0';   //加入结束符
	if(Esp8266_Send_Cmd("AT+CIPSEND","OK",300))
	{
		return 1;
	}
	sprintf(buf,"GET https://%s/v3/weather/now.json?key=%s&location=%s&language=zh-Hans&unit=c",host,apikey,bufcity);
	if(!Esp8266_SendData(buf,"",500))  //成功得到天气数据
	{
		HandleWeathrerDataInfo(WifiRecBuf);
		return 0;
	}
	else
	{
		return 1;	
	}		
}
//ESP8266 获取网络天气信息
uint8_t Esp8266GetBeijingInfo(void)
{
	uint8_t i;
	char buf[200];
	ClearBuf();  //清楚缓存
	for(i=0;i<2;i++)
	{
		if(!Esp8266_ConectServer(GET_NETWORK_TIME_MODE))   //连接到时间服务器
			break;
		else
			continue;
	}
	if(Esp8266_Send_Cmd("AT+CIPSEND","OK",300))
		return 1;
	sprintf(buf,"GET http://api.k780.com:88/?app=life.time&appkey=10003&sign=b59bc3ef6191eb9f747dd4e83c99f2a4&format=json\r\n");
	if(!Esp8266_SendData(buf,"",500))  //成功得到天气数据
	{
		HandleTimeDataInfo(WifiRecBuf);
		return 0;
	}
	else
	{
		return 1;		
	}
}
//尝试重新连接WIFI
void TryReConectWifi(void)
{
	if(!Esp8266_ConnectWire())  //连接成功的话
	{
		Esp8266Config.WifiLinkSuccFlag = 1;  //标志连接成功
	}
	else
	{
		Esp8266Config.WifiLinkSuccFlag = 0;
	}
}
//获得ESP8266的当前连接状态
//返回值:当前的状态
//2:获得 ip
//3:建立连接
//4:失去连接
uint8_t Esp8266_GetServerLinkStat(void)
{
	char *p=0;
	uint8_t res=0;
	ClearBuf();
	res = Esp8266_Send_Cmd("AT+CIPSTATUS","OK",500);
	if(!res)             //如果成功
	{
		p = strstr((const char*)WifiRecBuf,"STATUS:")+7;
		res = *(uint8_t *)p-0x30;
		return res;
	}
   else
   {
	   while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
		USART1->DR = '+';
	   while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
		USART1->DR = '+';
	   while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
		USART1->DR = '+';
	   return 4;
   }
}
//模块重新启动 AT指令   AT+RST
//返回值：0成功  1失败
uint8_t Esp8266_ReStart(void)
{
	uint8_t res;
	ClearBuf();
	res = Esp8266_Send_Cmd("AT+RST","OK",500);
	return res;
}
//设置模块的工作方式
//ST模式
//AP模式
//ST+AP模式
//返回值： 0成功  1失败
uint8_t  Esp8266_SetMode(uint8_t Mode)
{
	uint8_t res;
    ClearBuf();		
	if(Mode)
		 res = Esp8266_Send_Cmd("AT+CWMODE=1","OK",500);
	else
		res = Esp8266_Send_Cmd("AT+CWMODE=3","OK",500);
 	return res;
}
//查看附近的无线网络
//返回值：0成功   1失败
uint8_t Esp8266_CheckNET(void)
{
	uint8_t res;
    ClearBuf();
	res = Esp8266_Send_Cmd("AT+CWLAP","OK",500);
    return res; 
}
//返回模块的IP地址说明已经连接路由器 返回OK
uint8_t Esp8266_BackIPAdd(void)
{
	uint8_t res;
    ClearBuf();
	res = Esp8266_Send_Cmd("AT+CIFSR","OK",500);
    return res; 
}
//"%SSID=" + splitstring1 + "&%" + splitstring2 + "&#";
//发送要连接的无线网络
//返回值： 0成功 1失败
uint8_t Esp8266_ConnectWire(void)
{
	uint8_t res;
	uint8_t buf[100];
    ClearBuf();
    sprintf(buf,"AT+CWJAP=\"%s\",\"%s\"\r\n",Esp8266Config.WireNet[0],Esp8266Config.WireNet[1]);  //发送无线网络ISSD和无线密码 
    res = Esp8266_Send_Cmd(buf,"OK",500);
	if(res)
		return 1;
	ClearBuf();
	res = Esp8266_Send_Cmd("AT+CIPSEND","OK",500);
	if(res)
		return 1;
    return res; 
}
//模块开启多连接
//返回值： 0成功 1失败
uint8_t Esp8266_OpenMoreConect(void)
{
	uint8_t res;
    ClearBuf();
	res = Esp8266_Send_Cmd("AT+CIPMUX=1","OK",500);
    return res;
}
//设置但链接
uint8_t  Esp8266_SetSingleLink(void)
{
	uint8_t res;
	ClearBuf();
	res = Esp8266_Send_Cmd("AT+CIPMUX=0","OK",500);
	return res;
}
//开始要连接的服务器的IP地址和端口号
//入口参数：Proto：通信协议  IP:服务器IP地址  Port 服务器端口号
//返回值; 0成功 1失败
uint8_t Esp8266_ConectServer(NetMode_TypeDef LinkMode)
{
	uint8_t res;
	uint8_t buf[100];
    ClearBuf();
	switch(LinkMode)
	{
		case UP_LOAD_SENSOR_DATA_MODE:
		{
			sprintf(buf,"AT+CIPSTART=\"%s\",\"%s\",%d\r\n",Esp8266Config.ComunitPro,Esp8266Config.ServerIP,Esp8266Config.ServerPort); //发送指令
			res = Esp8266_Send_Cmd(buf,"OK",500);
			return res;
		}break;
		case GET_NETWORK_TIME_MODE:
		{
			Esp8266WeatherConnectInfo_TypeDef ConnectionInfo;
			ConnectionInfo.ipAddress  = (uint8_t *)TIME_ADDRESS;
			ConnectionInfo.port = TIME_PORT;
			sprintf(buf,"AT+CIPSTART=\"%s\",\"%s\",%d\r\n","TCP",ConnectionInfo.ipAddress,ConnectionInfo.port); //发送指令
			res = Esp8266_Send_Cmd(buf,"OK",500);
			return res;
		}break;
		case GET_WEATHER_MODE:
		{
			Esp8266WeatherConnectInfo_TypeDef ConnectionInfo;
			ConnectionInfo.ipAddress  = (uint8_t *)WEATHER_ADDRESS;
			ConnectionInfo.port = WEATHER_PORT;
			sprintf(buf,"AT+CIPSTART=\"%s\",\"%s\",%d\r\n","TCP",ConnectionInfo.ipAddress,ConnectionInfo.port); //发送指令
			res = Esp8266_Send_Cmd(buf,"OK",500);
			return res;
		}break;
		default:break;
	}
}
//断开连接
//返回值： 0成功 1失败
uint8_t Esp8266_DisConectServer(void)
{
	uint8_t res;
	uint8_t buf[100];
    ClearBuf();
	sprintf(buf,"AT+CIPCLOSE=%d\r\n",Esp8266Config.SlrID);
	res = Esp8266_Send_Cmd(buf,"OK",500);
    return res;
}
//
//设置模块为Server模式
//
uint8_t Esp8266_SetServerMode(uint16_t Port)
{
	uint8_t res;
	ClearBuf();
	res = Esp8266_Send_Cmd("AT+CIPSERVER=1,333","OK",500);
	return res; 
}
//192.168.4.1
uint8_t GetIPAddr(void)
{
	uint8_t res;
	ClearBuf();
	res = Esp8266_Send_Cmd("AT+CIFSR","OK",500);
	return res;
}
//监听串口数据
//+IPD,0,10:1234567890
//UartRecBuf[50]
//返回值：模块的ID值
uint8_t ListenRec(void)
{
    if(USART1_RX_STA&0X8000)  //接收到了服务器数据
    {
		USART1_RX_STA = 0;
        if(strstr((const char*)WifiRecBuf,"+IPD"))  //接收到了数据
        {
             return 0;
        }
		ClearBuf();
    }
    else
        return 1; //返回模块的ID值
}
//设置透传模式
uint8_t  Esp8266_SetPassThrough(uint8_t mode)
{
	uint8_t res;
	ClearBuf();
	if(mode == 1)
		res = Esp8266_Send_Cmd("AT+CIPMODE=1","OK",500);
	else if(mode == 0)
		res = Esp8266_Send_Cmd("AT+CIPMODE=0","OK",500);
    return res;
}
//设置非透传模式
uint8_t Esp8266_SetNoPassThrough(void)
{
	uint8_t res;
	ClearBuf();
	res = Esp8266_Send_Cmd("AT+CIPMODE=0","OK",500);
    return res;
}
//退出透传并且重新连接服务器
uint8_t Esp8266_QuitPassThroughLinkServer(void)
{
	uint8_t i,res;
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = '+';
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = '+';
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = '+';
	delay_ms(500);
	return Esp8266_Send_Cmd("AT","OK",50);
}
//缓冲区	清零函数
//输入参数：缓冲区地址
void ClearBuffer(uint8_t *data)
{
    uint8_t i,len;
    len=strlen(data);
    for(i=0;i<len;i++)
    {
        data[i]=0;
    }
}
//查找特殊字符
uint8_t *Esp8266_Check_Cmd(const uint8_t *str)
{
	uint8_t *strx = 0;
	if(USART1_RX_STA&0X8000)		//接收到一次数据了
	{ 
		WifiRecBuf[USART1_RX_STA&0X7FFF] = 0;//添加结束符
		strx = strstr((const char*)WifiRecBuf,(const char*)str);
	}
	return (uint8_t*)strx;
}
//向Esp8266发送指令
uint8_t Esp8266_Send_Cmd(uint8_t *Cmd,const uint8_t *Ack,uint16_t waittime)
{
	uint8_t res=0; 
	USART1_RX_STA=0;
	Esp8266SendInfo("%s\r\n",Cmd);	//发送命令
	if(Ack&&WaitTime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART1_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(Esp8266_Check_Cmd(Ack))
					break;//得到有效数据 
				USART1_RX_STA=0;
			} 
		}
		if(WaitTime==0)
			res=1; 
	}
	return res;
}
//启动发送信息
//返回值; 0成功 1失败
uint8_t Esp8266_SendData(uint8_t *Data,const uint8_t *Ack,uint16_t waittime)
{
    uint8_t res = 0;
	ClearBuf();
	Esp8266SendInfo("%s\r\n",Data);	 
	
	if(Ack && waittime)  //确认数据发送成功
	{
		while(--waittime)
		{
			delay_ms(10);
			if(USART1_RX_STA&0X8000)
			{
				if(Esp8266_Check_Cmd(Ack))
				{
					break;
				}
				USART1_RX_STA = 0;
			}
		}
		if(waittime == 0)
		{
			res = 1;
		}
	}
	return res;
}
//读取服务器参数
uint8_t ReadServerPar(void)
{
	uint8_t i;
	uint8_t ConfigStat;		//配置状态
	//uint8_t buf[50];
	uint8_t *Start;   //字符串开始和结束的指针位置
	uint8_t StrLen;
	INTX_DISABLE();
	ConfigStat = AT24CXX_ReadOneByte(EEPROM_ADDR_ESP89266_CONFIG_FLAG);
	if((ConfigStat&0x02)==0x02)
	{
		ClearBuf();
		delay_ms(100);
		AT24CXX_Read(EEPROM_ADDR_ESP89266_USERKEY,WifiRecBuf,50);
		if(strstr((const char*)WifiRecBuf,(const char*)"%USER")==NULL)  //如果是没有发现指定字符串则返回
		{ 
			return 0X01;
		}
		else
		{
			StrLen = (strstr((const char*)WifiRecBuf,"&#")) - (strstr((const char*)WifiRecBuf,"%USER")+9);
			Start = strstr((const char*)WifiRecBuf,(const char*)"%USER") + 9;
			memcpy(UserKey,Start,StrLen);
		}
	}
	if((ConfigStat&0x04)==0x04)
	{
		ClearBuf();		
		delay_ms(100);
		AT24CXX_Read(EEPROM_ADDR_ESP89266_SVN,WifiRecBuf,30);
		INTX_ENABLE();
		if(strstr((const char*)WifiRecBuf,(const char*)"%SVN")==NULL)
		{
			ClearBuf();
			return 0X02;
		}
		else
		{
			StrLen = (strstr((const char*)WifiRecBuf,(const char*)"&%")) - (strstr((const char*)WifiRecBuf,(const char*)"%SVN")+5);
			Start = strstr((const char*)WifiRecBuf,(const char*)"%SVN") +5;
			for(i=0;i<StrLen;i++)
			{
				Esp8266Config.ServerIP[i] = *Start++;
			}
			StrLen = (strstr((const char*)WifiRecBuf,(const char*)"&#")) - (strstr((const char*)WifiRecBuf,(const char*)"&%")+2);
			Start = strstr((const char*)WifiRecBuf,(const char*)"&%") +2;
			Esp8266Config.ServerPort = (*Start-0x30)*1000+(*(Start+1)-0x30)*100+(*(Start+2)-0x30)*10+(*(Start+3)-0x30);
		}
	}
	return 0;
}
//Wifi连接设置处理
//数据格式：%SSID=guchenglong&%gcl19910621&#
//      7          18
//%SSID=TP-LINK_601&%qq18789463526qq&#
uint8_t WifiNETSet(void)
{
    uint8_t i;
	uint8_t *databuf;
	uint8_t ConfigStat;
    uint8_t *WifiSSID=0;                 //Wifi名称和密码指针分离
    uint8_t *Wifisecret=0;
    uint8_t Dist1;
    uint8_t Dist2;
	databuf = (uint8_t *)mymalloc(200); 
	if(databuf==NULL)
		return 1;
	ConfigStat = AT24CXX_ReadOneByte(EEPROM_ADDR_ESP89266_CONFIG_FLAG);  //读取是否配置标志
	if((ConfigStat&0X01)==0X01)   				  //已配置
	{
		ReConfigFlag = 0;
		delay_ms(100);
		AT24CXX_Read(EEPROM_ADDR_ESP89266_SSID_SECRET,databuf,150);  		//读出wifi的SSID和密码
	}
	else
		return 1; //错误状态
    if(strstr((const char*)databuf,(const char*)"%SSID")==NULL)  //如果是没有发现指定字符串则返回
    {
         return 1;
    }
    WifiSSID = strstr((const char*)databuf,(const char*)"%SSID")+6;       //指到SSID处
    Wifisecret = strstr((const char*)databuf,(const char*)"&%")+2;  //指到密码地址处
    Dist1 =  strstr((const char*)databuf,(const char*)"&%") - (strstr((const char*)databuf,(const char*)"%SSID")+6);    //这儿搞了好长时间终于好了，SSID部分的值
    Dist2 = strstr((const char*)databuf,(const char*)"&#") - (strstr((const char*)databuf,(const char*)"&%")+2);      //密码到结束符
//    memcpy(Esp8266Config.WireNet[0],WifiSSID,Dist1);
//	memcpy(Esp8266Config.WireNet[1],Wifisecret,Dist2);
//	Show_Str(260,200,180,12,Esp8266Config.WireNet[0],12,0,GREEN,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
//	Show_Str(260,240,180,12,Esp8266Config.WireNet[1],12,0,GREEN,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
//	return 0;
	if(*(WifiSSID-6)=='%')
    {
        for(i=0;i<Dist1;i++)
        {
            Esp8266Config.WireNet[0][i] = *WifiSSID; //保存SSID
            WifiSSID++;		
        }
        for(i=0;i<Dist2;i++)
        {
            Esp8266Config.WireNet[1][i] = *Wifisecret;  //保存密码
            Wifisecret++;
        }
        return 0;
    }
    else
    {
        return 1;
    }
}
//配置模块为服务器模式
void ConfigServerMode(void)
{
	char *pIP = 0;
	const char *ip = "APIP";
	LCD_Clear(BLACK);//清屏
	if(!Esp8266_SetMode(1))   		//设置为AP模式
		Show_Str(260,90,200,16,"AP模式设置成功",16,0,GREEN,BLACK);
	else
	{
		Show_Str(260,90,200,16,"AP模式设置失败",16,0,RED,BLACK);
		return;
	}
	if(!Esp8266_ReStart())  //发送重新启动信号
    {
		Show_Str(260,110,200,16,"WIFI重启成功",16,0,GREEN,BLACK);
	}
	else
	{
		Show_Str(260,110,200,16,"WIFI重启失败",16,0,RED,BLACK);
		return;
	}
	delay_ms(3000);
	if(!Esp8266_SetNoPassThrough())
		Show_Str(260,110,200,16,"非透传设置成功",16,0,GREEN,BLACK);
	else
		Show_Str(260,110,200,16,"非透传设置失败",16,0,GREEN,BLACK);
	if(!Esp8266_OpenMoreConect())	//打开多连接
		Show_Str(260,110,200,16,"多连接打开成功",16,0,GREEN,BLACK);
	else
	{
		Show_Str(260,110,200,16,"多连接打开失败",16,0,RED,BLACK);
		return;
	}
	delay_ms(1000);
	if(!Esp8266_SetServerMode(8080))  //设置服务器模式	
		Show_Str(260,130,200,16,"本地服务器开启成功",16,0,GREEN,BLACK);
	else
	{
		Show_Str(260,130,200,16,"本地服务器开启失败.",16,0,RED,BLACK);
		return;
	}
	delay_ms(1000);
	if(GetIPAddr())
		return;
	pIP = strstr((const char*)WifiRecBuf,ip)+6;
	if(pIP)
		Show_Str(324,150,200,16,(uint8_t *)pIP,16,0,GREEN,BLACK);
	else
		Show_Str(324,150,200,16,"无信息",16,0,GREEN,BLACK);
	Show_Str(260,150,100,16,"本地IP:",16,0,GREEN,BLACK);
	Show_Str(260,170,200,16,"端口:333.",16,0,GREEN,BLACK);
	//ClearBuf();
}
//网络数据处理
//输入参数：结构体  开关量  PWM值
//串口数据：+IPD,0,10:服务端数据
//信息数据格式: <数据内容格式>  分隔符  &         %SSID=" + splitstring1 + "&%" + splitstring2 + "&#
void NETDataDeal(void)
{
    uint8_t i;
    uint8_t *DataP;
    uint16_t num=0;
    if(strstr(WifiRecBuf,"S1ON")!=NULL)
    {		
		LCD_ShowString(128,190,200,16,16,"S1 ON",GREEN,BLACK);
    }
    if(strstr(WifiRecBuf,"S1OFF")!=NULL)
    {
		LCD_ShowString(128,190,200,16,16,"S1 OFF",GREEN,BLACK);
    }
    if(strstr(WifiRecBuf,"S2ON")!=NULL)
    {
		LCD_ShowString(128,190,200,16,16,"S2 ON",GREEN,BLACK);
    }
    if(strstr(WifiRecBuf,"S2OFF")!=NULL)
    {
		LCD_ShowString(128,190,200,16,16,"S2 OFF",GREEN,BLACK);
    }  
    if(strstr(WifiRecBuf,"Val1=")!=NULL)
    {
        DataP = strstr(WifiRecBuf,"Val1=") +5;    //指针移到数据处
        for(i=0;i<strlen(DataP)-6;i++)
        {
            if(*DataP!='&')
            {
                num=num*10+*DataP-'0';
                DataP++;
            }
            else
                break;
        }			 
//        Lcd12864PutNum(4,3,num);
        num=0;
    } 
    if(strstr(WifiRecBuf,"Val2=")!=NULL)
    {
        DataP = strstr(WifiRecBuf,"Val2=") +5;    //指针移到数据处
        for(i=0;i<strlen(DataP)-6;i++)
        {
            if(*DataP!='&')
            {
                num=num*10+*DataP-'0';
                DataP++;
            }
        else
            break;
        }
//        Lcd12864PutNum(6,3,num);
        num=0;
    } 
   // Esp8266_CIPSendData("Received Success",strlen("Received Success"));	 
    ClearBuf();
}
#endif

