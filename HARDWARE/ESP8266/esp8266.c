#ifndef _ESP8266_C
#define _ESP8266_C	
#include "include.h"	
//STMflash��д������������
//uint8_t Flash_Buffer[50];      //flash������50���ֽ�
//#define SIZE sizeof(Flash_Buffer)	 			  	//���鳤��

//01004566
//��ΪIP:121.40.22.15
//esp8266Wifiģ����������
//��������ͨ��Э�飬IP��ַ���˿ں�
//%SSID=HiWiFi_0F06D8&%sundayenjoy..1234&#
//%USERKEY=570b6146c6e242018da2ce6c9239627b&#
//��Ϊ��������Կ
uint8_t UserKey[60];
Esp8266 Esp8266Config={{"",""},{"TCP"},{""},8888,0,1,0,0};           //�ṹ��ʵ�廯
uint8_t ReConfigFlag = 0XFF;  //�Ƿ���������WIFI

uint8_t Esp8266InitFinishFlag = 0;
uint8_t AcuireFinishFlag = 0;     //�Ƿ�������ȡ��������Ϣ��ʱ����Ϣ
uint8_t WeatherAcquireStatus = 0;  //�������ݻ�ȡ״̬
uint8_t TimeAcquireStatus = 0;     //ʱ����Ϣ��ȡ״̬
//��������Ԥ����Ϣ
NetWorkInfo_TypeDef NetWorkDataInfo = 
{
	{0},
	 0,
	{0},
	{0}
};  				//�ӻ������ϻ�ȡ������Ϣ���ݽṹ
//��ȡSD��������EEPROM��
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
	Show_Str(100,45,200,24,"WIFI����������",24,0,BLUE,BLACK);
	res=f_open(F_configdata,(const TCHAR*)"0:wificonfigfile.ini",FA_READ);
	if(res == 0)  	//�򿪳ɹ�
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
	if(f_gets((TCHAR*)databuf,150,F_configdata) != NULL)   //��һ��
	{
		Show_Str(100,80,200,16,"����SSID->",16,0,WHITE,BLACK);
		Show_Str(220,80,200,16,"д����",16,0,WHITE,BLACK);
		p1 = strstr((const char*)databuf,(const char*)"%SSID");
		AT24CXX_Write(EEPROM_ADDR_ESP89266_SSID_SECRET,(uint8_t*)p1,150);  		//����wifi��SSID������
		ConfigWriteFlag |= (1<<0);
		Show_Str(220,80,200,16,"������",16,0,GREEN,BLACK);
		Show_Str(100,100,200,16,"����USERKEY->",16,0,WHITE,BLACK);
		memset(databuf,0,100);
	}
	else
	{
		f_close(F_configdata);
		myfree(F_configdata);
		myfree(databuf);
		return 1;
	}
	if(f_gets((TCHAR*)databuf,150,F_configdata) != NULL)		//�ڶ���
	{
		Show_Str(220,100,200,16,"д����",16,0,WHITE,BLACK);
		p1 = strstr((const char*)databuf,(const char*)"%USERKEY");
		AT24CXX_Write(EEPROM_ADDR_ESP89266_USERKEY/*151*/,(uint8_t*)p1,50);
		ConfigWriteFlag |= (1<<1);
		Show_Str(220,100,200,16,"������",16,0,GREEN,BLACK);
		Show_Str(100,120,200,16,"����ServerIP->",16,0,WHITE,BLACK);
		memset(databuf,0,100);
	}
	else
	{
		f_close(F_configdata);
		myfree(F_configdata);
		myfree(databuf);
		return 1;
	}
	if(f_gets((TCHAR*)databuf,150,F_configdata) != NULL)		//������
	{
		Show_Str(220,120,200,16,"д����",16,0,WHITE,BLACK);
		p1 = strstr((const char*)databuf,(const char*)"%SVN");
		AT24CXX_Write(EEPROM_ADDR_ESP89266_SVN/*202*/,(uint8_t*)p1,30);
		ConfigWriteFlag |= (1<<2);
		AT24CXX_WriteOneByte(0,ConfigWriteFlag);
		Show_Str(220,120,200,16,"������",16,0,GREEN,BLACK);
		Show_Str(160,140,200,16,"��������",16,0,GREEN,BLACK);
		memset(databuf,0,100);
	}
	else
	{
		f_close(F_configdata);
		myfree(F_configdata);
		myfree(databuf);
		return 1;
	}
	if(f_gets((TCHAR*)databuf,150,F_configdata) != NULL)		//������   %CITY=xian&#
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
//��������Esp8266ģ�飬����Ƿ���λ�ɹ�
//����:�����ȶ�ȡFLASH���õ�ֵ��ʼ�������ӣ���������յ���λ�������������ݣ���ʼ����
uint8_t Esp8266_Config(void)
{
	uint8_t i;
	uint8_t ErrorType;
    uint8_t SuccesFlag = 0;  		//���ӳɹ���־
    uint8_t LinkTimes = 0;
	if(Current_Show_Interface == Curr_SetPara_Show && DispWifiInfoFlag)
		LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)==0)  //���Ҫ�������õĻ�ִ��
	{
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)==0)
		{
			delay_ms(2000);
			if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)==0)  //�����Ļ���������
				return 1;
		}
	}
	if(Current_Show_Interface == Curr_SetPara_Show && DispWifiInfoFlag)
		LCD_Fill(250,65,430,280,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
    if(!Esp8266_ReStart())  //�������������ź�
    {
		if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
			Show_Str(260,70,200,16,"WIFI�����ɹ�.",16,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
        Esp8266Config.WifiStartFlag = 1;
    }
	else
	{
		if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
		{
			Show_Str(260,70,200,16,"WIFI����ʧ��.",16,0,RED,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			Encoder_Type = ENCODER_PRESSED;
		}
        Esp8266Config.WifiStartFlag = 0;
		Esp8266InitFinishFlag = 1;
		return 1;
	}
	delay_ms(2000);
	delay_ms(1000);
    if(!Esp8266_SetMode(1))   //����ģʽ(AP��STA����ģʽ)
	{
		if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
			Show_Str(260,90,200,16,"TCPģʽ���óɹ�.",16,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	}
	else
	{
		if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
		{
			Show_Str(260,90,200,16,"TCPģʽ����ʧ��.",16,0,RED,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
			Encoder_Type = ENCODER_PRESSED;
		}
		Esp8266InitFinishFlag = 1;
        return 1;
	}
	delay_ms(2000);
    //��ʼ����WIFIģ��
	if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
		Show_Str(260,150,200,16,"WIFI������...",16,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	while(LinkTimes<2) //�����3��
	{
		 LinkTimes ++;
		 if(!WifiNETSet()&&SuccesFlag==0)      //�������ã�������óɹ��Ļ���ִ����Ӧ����
		 {
             SuccesFlag = 1;
			 if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
				Show_Str(260,150,200,16,"WIFI���óɹ�",16,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		 }
		 else
		 {
			 if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
				Show_Str(260,150,200,16,"����������.",16,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		 }
		 if(SuccesFlag==1)             //���ӳɹ��Ļ�
		 {
			 if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
				Show_Str(260,170,200,16,"WIFI������...",16,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
				if(!Esp8266_ConnectWire())   //��ʼ����WIFI�����ӳɹ��Ļ�
				{
					Esp8266Config.WifiLinkSuccFlag = 1;  //��־WIFI���ӳɹ�
					if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
						Show_Str(260,170,200,16,"WIFI���ӳɹ�",16,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
					for(i=0;i<2;i++)
					{
						 LED0 = !LED0;
						 delay_ms(500);
					}
					break;
				}
			 	else//��������δ�ɹ�
				{
					SuccesFlag=0;
					Esp8266Config.WifiLinkSuccFlag = 0;
					if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
						Show_Str(260,170,200,16,"WIFI����ʧ��",16,0,RED,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
				}
		 }
	}
	if(LinkTimes>=3)
	{
		if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
			Show_Str(260,170,200,16,"WIFI����ʧ��",16,0,RED,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	}
////////////////////////////////////////////////////////////////////
	if(AcuireFinishFlag==0 && Current_Show_Interface != Curr_Set_Show)  			//�����ò���������²�����
	{
		Esp8266_QuitPassThroughLinkServer();
		Esp8266_SetPassThrough(1);
		for(uint8_t i=0;i<4;i++)
		{
			//��ȡ������Ϣ
			if(!Esp8266GetWeatherInfo(WEATHER_ADDRESS,APIKEY))  //��ȡ��������Ԥ��
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
		//��ȡ����ʱ����Ϣ����У׼
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
	//LCD_Clear(BLACK);   //����
	if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
		Show_Str(260,210,200,16,"������������...",16,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
	ErrorType = ReadServerPar();
	if(!ErrorType)   //��EEPROM��ȡ������������
	{
		if(!Esp8266_ConectServer(UP_LOAD_SENSOR_DATA_MODE))
		{
			if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
				Show_Str(260,210,200,16,"���������ӳɹ�",16,0,SET_SHOW_INTERFACE_RIGHT_TEXTCOLOR,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
		}
		else
		{
			if(Current_Show_Interface==Curr_SetPara_Show && DispWifiInfoFlag)
			{
				Show_Str(260,210,200,16,"����������ʧ��",16,0,RED,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
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
			Show_Str(60,30,200,16,"������ȡ����  ",16,0,RED,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
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
//�Ի�ȡ�����������ݽ��з���
//cJSON���ݻ������ʼ��ַ
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
	root = cJSON_Parse(pMsg);  //�õ����ڵ�
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
					pChildItem = cJSON_GetObjectItem(pSubItem,"name");  //��������
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
					pChildItem = cJSON_GetObjectItem(pSubItem,"text");  //������Ϣ
					if(pChildItem != NULL)
					{
						utf8str = pChildItem->valuestring;
						SwitchToGbk((const uint8_t*)utf8str,strlen(utf8str),(uint8_t *)gbkstr,&len);
						strcpy(NetWorkDataInfo.Weather_Climate,gbkstr);
						//Show_Str(0,20,480,16,NetWorkDataInfo.Weather_Climate,16,0,BLUE,BLACK);
					}
					pChildItem = cJSON_GetObjectItem(pSubItem,"code");  //������Ϣ����
					if(pChildItem != NULL)
					{
						gbkstr = pChildItem->valuestring;
						//Show_Str(0,40,480,16,(uint8_t *)gbkstr,16,0,BLUE,BLACK);
					}
					pChildItem = cJSON_GetObjectItem(pSubItem,"temperature");   //�¶�
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
//����ʱ������
//��ʽ     2017-10-22 22:38:37
void AdjustSysTime(uint8_t *Data)
{
	SysTimeData.Year = (Data[2] - 0x30)*10 + (Data[3]-0x30);
	SysTimeData.Month = (Data[5]-0x30)*10 + (Data[6]-0x30);
	SysTimeData.Day = (Data[8]-0x30)*10 + (Data[9]-0x30);
	SysTimeData.Hour = (Data[11]-0x30)*10 + (Data[12]-0x30);
	SysTimeData.Minute = (Data[14]-0x30)*10 + (Data[15]-0x30);
	SysTimeData.Second = (Data[17]-0x30)*10 + (Data[18]-0x30);
	Set_Rx8025t_Time(SysTimeData.Year,SysTimeData.Month,SysTimeData.Day,SysTimeData.Hour,SysTimeData.Minute,SysTimeData.Second);   //��ʼУ׼ʱ��
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
	root = cJSON_Parse(pMsg);  //�õ����ڵ�
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
//ESP8266 ��ȡ����������Ϣ
uint8_t Esp8266GetWeatherInfo(const uint8_t *host,const uint8_t *apikey)
{
	char buf[200];
	char bufcity[20];
	uint8_t len;
	uint8_t i;
	char *p1=0;
	ClearBuf();  //�������
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
	bufcity[len] = '\0';   //���������
	if(Esp8266_Send_Cmd("AT+CIPSEND","OK",300))
	{
		return 1;
	}
	sprintf(buf,"GET https://%s/v3/weather/now.json?key=%s&location=%s&language=zh-Hans&unit=c",host,apikey,bufcity);
	if(!Esp8266_SendData(buf,"",500))  //�ɹ��õ���������
	{
		HandleWeathrerDataInfo(WifiRecBuf);
		return 0;
	}
	else
	{
		return 1;	
	}		
}
//ESP8266 ��ȡ����������Ϣ
uint8_t Esp8266GetBeijingInfo(void)
{
	uint8_t i;
	char buf[200];
	ClearBuf();  //�������
	for(i=0;i<2;i++)
	{
		if(!Esp8266_ConectServer(GET_NETWORK_TIME_MODE))   //���ӵ�ʱ�������
			break;
		else
			continue;
	}
	if(Esp8266_Send_Cmd("AT+CIPSEND","OK",300))
		return 1;
	sprintf(buf,"GET http://api.k780.com:88/?app=life.time&appkey=10003&sign=b59bc3ef6191eb9f747dd4e83c99f2a4&format=json\r\n");
	if(!Esp8266_SendData(buf,"",500))  //�ɹ��õ���������
	{
		HandleTimeDataInfo(WifiRecBuf);
		return 0;
	}
	else
	{
		return 1;		
	}
}
//������������WIFI
void TryReConectWifi(void)
{
	if(!Esp8266_ConnectWire())  //���ӳɹ��Ļ�
	{
		Esp8266Config.WifiLinkSuccFlag = 1;  //��־���ӳɹ�
	}
	else
	{
		Esp8266Config.WifiLinkSuccFlag = 0;
	}
}
//���ESP8266�ĵ�ǰ����״̬
//����ֵ:��ǰ��״̬
//2:��� ip
//3:��������
//4:ʧȥ����
uint8_t Esp8266_GetServerLinkStat(void)
{
	char *p=0;
	uint8_t res=0;
	ClearBuf();
	res = Esp8266_Send_Cmd("AT+CIPSTATUS","OK",500);
	if(!res)             //����ɹ�
	{
		p = strstr((const char*)WifiRecBuf,"STATUS:")+7;
		res = *(uint8_t *)p-0x30;
		return res;
	}
   else
   {
	   while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
		USART1->DR = '+';
	   while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
		USART1->DR = '+';
	   while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
		USART1->DR = '+';
	   return 4;
   }
}
//ģ���������� ATָ��   AT+RST
//����ֵ��0�ɹ�  1ʧ��
uint8_t Esp8266_ReStart(void)
{
	uint8_t res;
	ClearBuf();
	res = Esp8266_Send_Cmd("AT+RST","OK",500);
	return res;
}
//����ģ��Ĺ�����ʽ
//STģʽ
//APģʽ
//ST+APģʽ
//����ֵ�� 0�ɹ�  1ʧ��
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
//�鿴��������������
//����ֵ��0�ɹ�   1ʧ��
uint8_t Esp8266_CheckNET(void)
{
	uint8_t res;
    ClearBuf();
	res = Esp8266_Send_Cmd("AT+CWLAP","OK",500);
    return res; 
}
//����ģ���IP��ַ˵���Ѿ�����·���� ����OK
uint8_t Esp8266_BackIPAdd(void)
{
	uint8_t res;
    ClearBuf();
	res = Esp8266_Send_Cmd("AT+CIFSR","OK",500);
    return res; 
}
//"%SSID=" + splitstring1 + "&%" + splitstring2 + "&#";
//����Ҫ���ӵ���������
//����ֵ�� 0�ɹ� 1ʧ��
uint8_t Esp8266_ConnectWire(void)
{
	uint8_t res;
	uint8_t buf[100];
    ClearBuf();
    sprintf(buf,"AT+CWJAP=\"%s\",\"%s\"\r\n",Esp8266Config.WireNet[0],Esp8266Config.WireNet[1]);  //������������ISSD���������� 
    res = Esp8266_Send_Cmd(buf,"OK",500);
	if(res)
		return 1;
	ClearBuf();
	res = Esp8266_Send_Cmd("AT+CIPSEND","OK",500);
	if(res)
		return 1;
    return res; 
}
//ģ�鿪��������
//����ֵ�� 0�ɹ� 1ʧ��
uint8_t Esp8266_OpenMoreConect(void)
{
	uint8_t res;
    ClearBuf();
	res = Esp8266_Send_Cmd("AT+CIPMUX=1","OK",500);
    return res;
}
//���õ�����
uint8_t  Esp8266_SetSingleLink(void)
{
	uint8_t res;
	ClearBuf();
	res = Esp8266_Send_Cmd("AT+CIPMUX=0","OK",500);
	return res;
}
//��ʼҪ���ӵķ�������IP��ַ�Ͷ˿ں�
//��ڲ�����Proto��ͨ��Э��  IP:������IP��ַ  Port �������˿ں�
//����ֵ; 0�ɹ� 1ʧ��
uint8_t Esp8266_ConectServer(NetMode_TypeDef LinkMode)
{
	uint8_t res;
	uint8_t buf[100];
    ClearBuf();
	switch(LinkMode)
	{
		case UP_LOAD_SENSOR_DATA_MODE:
		{
			sprintf(buf,"AT+CIPSTART=\"%s\",\"%s\",%d\r\n",Esp8266Config.ComunitPro,Esp8266Config.ServerIP,Esp8266Config.ServerPort); //����ָ��
			res = Esp8266_Send_Cmd(buf,"OK",500);
			return res;
		}break;
		case GET_NETWORK_TIME_MODE:
		{
			Esp8266WeatherConnectInfo_TypeDef ConnectionInfo;
			ConnectionInfo.ipAddress  = (uint8_t *)TIME_ADDRESS;
			ConnectionInfo.port = TIME_PORT;
			sprintf(buf,"AT+CIPSTART=\"%s\",\"%s\",%d\r\n","TCP",ConnectionInfo.ipAddress,ConnectionInfo.port); //����ָ��
			res = Esp8266_Send_Cmd(buf,"OK",500);
			return res;
		}break;
		case GET_WEATHER_MODE:
		{
			Esp8266WeatherConnectInfo_TypeDef ConnectionInfo;
			ConnectionInfo.ipAddress  = (uint8_t *)WEATHER_ADDRESS;
			ConnectionInfo.port = WEATHER_PORT;
			sprintf(buf,"AT+CIPSTART=\"%s\",\"%s\",%d\r\n","TCP",ConnectionInfo.ipAddress,ConnectionInfo.port); //����ָ��
			res = Esp8266_Send_Cmd(buf,"OK",500);
			return res;
		}break;
		default:break;
	}
}
//�Ͽ�����
//����ֵ�� 0�ɹ� 1ʧ��
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
//����ģ��ΪServerģʽ
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
//������������
//+IPD,0,10:1234567890
//UartRecBuf[50]
//����ֵ��ģ���IDֵ
uint8_t ListenRec(void)
{
    if(USART1_RX_STA&0X8000)  //���յ��˷���������
    {
		USART1_RX_STA = 0;
        if(strstr((const char*)WifiRecBuf,"+IPD"))  //���յ�������
        {
             return 0;
        }
		ClearBuf();
    }
    else
        return 1; //����ģ���IDֵ
}
//����͸��ģʽ
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
//���÷�͸��ģʽ
uint8_t Esp8266_SetNoPassThrough(void)
{
	uint8_t res;
	ClearBuf();
	res = Esp8266_Send_Cmd("AT+CIPMODE=0","OK",500);
    return res;
}
//�˳�͸�������������ӷ�����
uint8_t Esp8266_QuitPassThroughLinkServer(void)
{
	uint8_t i,res;
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = '+';
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = '+';
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = '+';
	delay_ms(500);
	return Esp8266_Send_Cmd("AT","OK",50);
}
//������	���㺯��
//�����������������ַ
void ClearBuffer(uint8_t *data)
{
    uint8_t i,len;
    len=strlen(data);
    for(i=0;i<len;i++)
    {
        data[i]=0;
    }
}
//���������ַ�
uint8_t *Esp8266_Check_Cmd(const uint8_t *str)
{
	uint8_t *strx = 0;
	if(USART1_RX_STA&0X8000)		//���յ�һ��������
	{ 
		WifiRecBuf[USART1_RX_STA&0X7FFF] = 0;//��ӽ�����
		strx = strstr((const char*)WifiRecBuf,(const char*)str);
	}
	return (uint8_t*)strx;
}
//��Esp8266����ָ��
uint8_t Esp8266_Send_Cmd(uint8_t *Cmd,const uint8_t *Ack,uint16_t waittime)
{
	uint8_t res=0; 
	USART1_RX_STA=0;
	Esp8266SendInfo("%s\r\n",Cmd);	//��������
	if(Ack&&WaitTime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART1_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(Esp8266_Check_Cmd(Ack))
					break;//�õ���Ч���� 
				USART1_RX_STA=0;
			} 
		}
		if(WaitTime==0)
			res=1; 
	}
	return res;
}
//����������Ϣ
//����ֵ; 0�ɹ� 1ʧ��
uint8_t Esp8266_SendData(uint8_t *Data,const uint8_t *Ack,uint16_t waittime)
{
    uint8_t res = 0;
	ClearBuf();
	Esp8266SendInfo("%s\r\n",Data);	 
	
	if(Ack && waittime)  //ȷ�����ݷ��ͳɹ�
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
//��ȡ����������
uint8_t ReadServerPar(void)
{
	uint8_t i;
	uint8_t ConfigStat;		//����״̬
	//uint8_t buf[50];
	uint8_t *Start;   //�ַ�����ʼ�ͽ�����ָ��λ��
	uint8_t StrLen;
	INTX_DISABLE();
	ConfigStat = AT24CXX_ReadOneByte(EEPROM_ADDR_ESP89266_CONFIG_FLAG);
	if((ConfigStat&0x02)==0x02)
	{
		ClearBuf();
		delay_ms(100);
		AT24CXX_Read(EEPROM_ADDR_ESP89266_USERKEY,WifiRecBuf,50);
		if(strstr((const char*)WifiRecBuf,(const char*)"%USER")==NULL)  //�����û�з���ָ���ַ����򷵻�
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
//Wifi�������ô���
//���ݸ�ʽ��%SSID=guchenglong&%gcl19910621&#
//      7          18
//%SSID=TP-LINK_601&%qq18789463526qq&#
uint8_t WifiNETSet(void)
{
    uint8_t i;
	uint8_t *databuf;
	uint8_t ConfigStat;
    uint8_t *WifiSSID=0;                 //Wifi���ƺ�����ָ�����
    uint8_t *Wifisecret=0;
    uint8_t Dist1;
    uint8_t Dist2;
	databuf = (uint8_t *)mymalloc(200); 
	if(databuf==NULL)
		return 1;
	ConfigStat = AT24CXX_ReadOneByte(EEPROM_ADDR_ESP89266_CONFIG_FLAG);  //��ȡ�Ƿ����ñ�־
	if((ConfigStat&0X01)==0X01)   				  //������
	{
		ReConfigFlag = 0;
		delay_ms(100);
		AT24CXX_Read(EEPROM_ADDR_ESP89266_SSID_SECRET,databuf,150);  		//����wifi��SSID������
	}
	else
		return 1; //����״̬
    if(strstr((const char*)databuf,(const char*)"%SSID")==NULL)  //�����û�з���ָ���ַ����򷵻�
    {
         return 1;
    }
    WifiSSID = strstr((const char*)databuf,(const char*)"%SSID")+6;       //ָ��SSID��
    Wifisecret = strstr((const char*)databuf,(const char*)"&%")+2;  //ָ�������ַ��
    Dist1 =  strstr((const char*)databuf,(const char*)"&%") - (strstr((const char*)databuf,(const char*)"%SSID")+6);    //������˺ó�ʱ�����ں��ˣ�SSID���ֵ�ֵ
    Dist2 = strstr((const char*)databuf,(const char*)"&#") - (strstr((const char*)databuf,(const char*)"&%")+2);      //���뵽������
//    memcpy(Esp8266Config.WireNet[0],WifiSSID,Dist1);
//	memcpy(Esp8266Config.WireNet[1],Wifisecret,Dist2);
//	Show_Str(260,200,180,12,Esp8266Config.WireNet[0],12,0,GREEN,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
//	Show_Str(260,240,180,12,Esp8266Config.WireNet[1],12,0,GREEN,SET_SHOW_INTERFACE_RIGHT_BACKCOLOR);
//	return 0;
	if(*(WifiSSID-6)=='%')
    {
        for(i=0;i<Dist1;i++)
        {
            Esp8266Config.WireNet[0][i] = *WifiSSID; //����SSID
            WifiSSID++;		
        }
        for(i=0;i<Dist2;i++)
        {
            Esp8266Config.WireNet[1][i] = *Wifisecret;  //��������
            Wifisecret++;
        }
        return 0;
    }
    else
    {
        return 1;
    }
}
//����ģ��Ϊ������ģʽ
void ConfigServerMode(void)
{
	char *pIP = 0;
	const char *ip = "APIP";
	LCD_Clear(BLACK);//����
	if(!Esp8266_SetMode(1))   		//����ΪAPģʽ
		Show_Str(260,90,200,16,"APģʽ���óɹ�",16,0,GREEN,BLACK);
	else
	{
		Show_Str(260,90,200,16,"APģʽ����ʧ��",16,0,RED,BLACK);
		return;
	}
	if(!Esp8266_ReStart())  //�������������ź�
    {
		Show_Str(260,110,200,16,"WIFI�����ɹ�",16,0,GREEN,BLACK);
	}
	else
	{
		Show_Str(260,110,200,16,"WIFI����ʧ��",16,0,RED,BLACK);
		return;
	}
	delay_ms(3000);
	if(!Esp8266_SetNoPassThrough())
		Show_Str(260,110,200,16,"��͸�����óɹ�",16,0,GREEN,BLACK);
	else
		Show_Str(260,110,200,16,"��͸������ʧ��",16,0,GREEN,BLACK);
	if(!Esp8266_OpenMoreConect())	//�򿪶�����
		Show_Str(260,110,200,16,"�����Ӵ򿪳ɹ�",16,0,GREEN,BLACK);
	else
	{
		Show_Str(260,110,200,16,"�����Ӵ�ʧ��",16,0,RED,BLACK);
		return;
	}
	delay_ms(1000);
	if(!Esp8266_SetServerMode(8080))  //���÷�����ģʽ	
		Show_Str(260,130,200,16,"���ط����������ɹ�",16,0,GREEN,BLACK);
	else
	{
		Show_Str(260,130,200,16,"���ط���������ʧ��.",16,0,RED,BLACK);
		return;
	}
	delay_ms(1000);
	if(GetIPAddr())
		return;
	pIP = strstr((const char*)WifiRecBuf,ip)+6;
	if(pIP)
		Show_Str(324,150,200,16,(uint8_t *)pIP,16,0,GREEN,BLACK);
	else
		Show_Str(324,150,200,16,"����Ϣ",16,0,GREEN,BLACK);
	Show_Str(260,150,100,16,"����IP:",16,0,GREEN,BLACK);
	Show_Str(260,170,200,16,"�˿�:333.",16,0,GREEN,BLACK);
	//ClearBuf();
}
//�������ݴ���
//����������ṹ��  ������  PWMֵ
//�������ݣ�+IPD,0,10:���������
//��Ϣ���ݸ�ʽ: <�������ݸ�ʽ>  �ָ���  &         %SSID=" + splitstring1 + "&%" + splitstring2 + "&#
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
        DataP = strstr(WifiRecBuf,"Val1=") +5;    //ָ���Ƶ����ݴ�
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
        DataP = strstr(WifiRecBuf,"Val2=") +5;    //ָ���Ƶ����ݴ�
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

