/**
************************************************************
* @file         gizwits_product.c
* @brief        Gizwits control protocol processing, and platform-related hardware initialization 
* @author       Gizwits
* @date         2017-07-19
* @version      V03030000
* @copyright    Gizwits
*
* @note         Gizwits is only for smart hardware
*               Gizwits Smart Cloud for Smart Products
*               Links | Value Added | Open | Neutral | Safety | Own | Free | Ecology
*               www.gizwits.com
*
***********************************************************/
#include <stdio.h>
#include <string.h>
#include "gizwits_product.h"
#include "stm32f10x.h"
extern void UsartSendDex(uint8_t num,uint8_t data);

static uint32_t timerMsCount;

/** Current datapoint */
dataPoint_t currentDataPoint;

/**@} */
/**@name Gizwits User Interface
* @{
*/

/**
* @brief Event handling interface

* Description:

* 1. Users can customize the changes in WiFi module status

* 2. Users can add data points in the function of event processing logic, such as calling the relevant hardware peripherals operating interface

* @param [in] info: event queue
* @param [in] data: protocol data
* @param [in] len: protocol data length
* @return NULL
* @ref gizwits_protocol.h
*/
extern char * WifiStateStr;
#define GRAY  			 0X8430
#define TOP_TITLE_BACK_COLOR 0x0000
extern void Show_Str(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t*str,uint8_t size,uint8_t mode,uint16_t Point_color,uint16_t Back_color);
int8_t gizwitsEventProcess(eventInfo_t *info, uint8_t *gizdata, uint32_t len)
{
  uint8_t i = 0;
  dataPoint_t *dataPointPtr = (dataPoint_t *)gizdata;
  moduleStatusInfo_t *wifiData = (moduleStatusInfo_t *)gizdata;
  protocolTime_t *ptime = (protocolTime_t *)gizdata;
  
#if MODULE_TYPE
  gprsInfo_t *gprsInfoData = (gprsInfo_t *)gizdata;
#else
  moduleInfo_t *ptModuleInfo = (moduleInfo_t *)gizdata;
#endif

  if((NULL == info) || (NULL == gizdata))
  {
    return -1;
  }

  for(i=0; i<info->num; i++)
  {
    switch(info->event[i])
    {
      case WIFI_SOFTAP:
				WifiStateStr="SoftAp模式";
				//Show_Str(281,17,150,16,(uint8_t *)WifiStateStr,16,0,GRAY,TOP_TITLE_BACK_COLOR);
        break;
      case WIFI_AIRLINK:
				WifiStateStr="AirLink模式";
				//Show_Str(281,17,150,16,(uint8_t *)WifiStateStr,16,0,GRAY,TOP_TITLE_BACK_COLOR);
        break;
      case WIFI_STATION:
				WifiStateStr="Station模式";
				//Show_Str(281,17,150,16,(uint8_t *)WifiStateStr,16,0,GRAY,TOP_TITLE_BACK_COLOR);
        break;
      case WIFI_CON_ROUTER:
				WifiStateStr="WiFi已连接路由";
				//Show_Str(281,17,150,16,(uint8_t *)WifiStateStr,16,0,GRAY,TOP_TITLE_BACK_COLOR);
        break;
      case WIFI_DISCON_ROUTER:
				WifiStateStr="WiFi断开路由";
				//Show_Str(281,17,150,16,(uint8_t *)WifiStateStr,16,0,GRAY,TOP_TITLE_BACK_COLOR);
        break;
      case WIFI_CON_M2M:
				WifiStateStr="WiFi连接到服务器";
				//Show_Str(281,17,150,16,(uint8_t *)WifiStateStr,16,0,GRAY,TOP_TITLE_BACK_COLOR);
        break;
      case WIFI_DISCON_M2M:
				WifiStateStr="WiFi断开服务器";
				//Show_Str(281,17,150,16,(uint8_t *)WifiStateStr,16,0,GRAY,TOP_TITLE_BACK_COLOR);
        break;
      case WIFI_RSSI:
        GIZWITS_LOG("RSSI %d\n", wifiData->rssi);
        break;
      case TRANSPARENT_DATA:
        GIZWITS_LOG("TRANSPARENT_DATA \n");
        //user handle , Fetch data from [data] , size is [len]
        break;
      case WIFI_NTP:
        GIZWITS_LOG("WIFI_NTP : [%d-%d-%d %02d:%02d:%02d][%d] \n",ptime->year,ptime->month,ptime->day,ptime->hour,ptime->minute,ptime->second,ptime->ntp);
        break;
      case MODULE_INFO:
            GIZWITS_LOG("MODULE INFO ...\n");
      #if MODULE_TYPE
            GIZWITS_LOG("GPRS MODULE ...\n");
            //Format By gprsInfo_t
      #else
            GIZWITS_LOG("WIF MODULE ...\n");
            //Format By moduleInfo_t
            GIZWITS_LOG("moduleType : [%d] \n",ptModuleInfo->moduleType);
      #endif
    break;
      default:
        break;
    }
  }

  return 0;
}
#include "adc.h"
extern SensorType SensorData;
/**
* User data acquisition

* Here users need to achieve in addition to data points other than the collection of data collection, can be self-defined acquisition frequency and design data filtering algorithm

* @param none
* @return none
*/
void userHandle(void)
{
		uint8_t index=0;
    currentDataPoint.valueWet = SensorData.Humi;//Add Sensor Data Collection
    currentDataPoint.valueTemp = SensorData.Temp;//Add Sensor Data Collection
    currentDataPoint.valueHCHO = SensorData.HCHO;//Add Sensor Data Collection
    currentDataPoint.valueCO2 = SensorData.Carbon;//Add Sensor Data Collection
		currentDataPoint.valuePM[0]=SensorData.PMData.PM1_0_A;
		currentDataPoint.valuePM[1]=SensorData.PMData.PM1_0_S;
		currentDataPoint.valuePM[2]=SensorData.PMData.PM2_5_A;
		currentDataPoint.valuePM[3]=SensorData.PMData.PM2_5_S;
		currentDataPoint.valuePM[4]=SensorData.PMData.PM10_A;
		currentDataPoint.valuePM[5]=SensorData.PMData.PM10_S;
	
		index=0;
		currentDataPoint.valueKLW[index++]=(uint8_t)((SensorData.PMData.Cnt_0_3>>0)&0xFF);
		currentDataPoint.valueKLW[index++]=(uint8_t)((SensorData.PMData.Cnt_0_3>>8)&0xFF);
		currentDataPoint.valueKLW[index++]=(uint8_t)((SensorData.PMData.Cnt_0_5>>0)&0xFF);
		currentDataPoint.valueKLW[index++]=(uint8_t)((SensorData.PMData.Cnt_0_5>>8)&0xFF);
		currentDataPoint.valueKLW[index++]=(uint8_t)((SensorData.PMData.Cnt_1_0>>0)&0xFF);
		currentDataPoint.valueKLW[index++]=(uint8_t)((SensorData.PMData.Cnt_1_0>>8)&0xFF);
		currentDataPoint.valueKLW[index++]=(uint8_t)((SensorData.PMData.Cnt_2_5>>0)&0xFF);
		currentDataPoint.valueKLW[index++]=(uint8_t)((SensorData.PMData.Cnt_2_5>>8)&0xFF);
		currentDataPoint.valueKLW[index++]=(uint8_t)((SensorData.PMData.Cnt_5_0>>0)&0xFF);
		currentDataPoint.valueKLW[index++]=(uint8_t)((SensorData.PMData.Cnt_5_0>>8)&0xFF);
		currentDataPoint.valueKLW[index++]=(uint8_t)((SensorData.PMData.Cnt_10>>0)&0xFF);
		currentDataPoint.valueKLW[index++]=(uint8_t)((SensorData.PMData.Cnt_10>>8)&0xFF);
		uint8_t Altitude_AirPres[6];
		memcpy(Altitude_AirPres,&SensorData.Altitude,2);
		memcpy(Altitude_AirPres+2,&SensorData.Altitude,4);
		gizwitsPassthroughData(Altitude_AirPres,6);
/*
    //XXX is Extend Datapoint Address ,User defined
    memcpy((uint8_t *)currentDataPoint.valuePM,XXX,sizeof(currentDataPoint.valuePM));
    //XXX is Extend Datapoint Address ,User defined
    memcpy((uint8_t *)currentDataPoint.valueKLW,XXX,sizeof(currentDataPoint.valueKLW));
   */ 
}

/**
* Data point initialization function

* In the function to complete the initial user-related data
* @param none
* @return none
* @note The developer can add a data point state initialization value within this function
*/
void userInit(void)
{
   memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));
    
    /** Warning !!! DataPoint Variables Init , Must Within The Data Range **/ 
    
   currentDataPoint.valueWet = 0;
   currentDataPoint.valueTemp = 0;
   currentDataPoint.valueHCHO = 0;
   currentDataPoint.valueCO2 = 0;
}


/**
* @brief  gizTimerMs

* millisecond timer maintenance function ,Millisecond increment , Overflow to zero

* @param none
* @return none
*/
void gizTimerMs(void)
{
    timerMsCount+=5;
}

/**
* @brief gizGetTimerCount

* Read system time, millisecond timer

* @param none
* @return System time millisecond
*/
uint32_t gizGetTimerCount(void)
{
    return timerMsCount;
}

/**
* @brief mcuRestart

* MCU Reset function

* @param none
* @return none
*/
void mcuRestart(void)
{
	__set_FAULTMASK(1);
	NVIC_SystemReset();
}
/**@} */

/**
* @brief TIMER_IRQ_FUN

* Timer Interrupt handler function

* @param none
* @return none
*/
void TIMER_IRQ_FUN(void)
{
  gizTimerMs();
}

/**
* @brief UART_IRQ_FUN

* UART Serial interrupt function ，For Module communication

* Used to receive serial port protocol data between WiFi module

* @param none
* @return none
*/
void UART_IRQ_FUN(void)
{
  uint8_t value = 0;
  //value = USART_ReceiveData(USART2);//STM32 test demo
  gizPutData(&value, 1);
}


/**
* @brief uartWrite

* Serial write operation, send data to the WiFi module

* @param buf      : Data address
* @param len       : Data length
*
* @return : Not 0,Serial send success;
*           -1，Input Param Illegal
*/
int32_t uartWrite(uint8_t *buf, uint32_t len)
{
    uint32_t i = 0;
    
    if(NULL == buf)
    {
        return -1;
    }
    
    #ifdef PROTOCOL_DEBUG
    GIZWITS_LOG("MCU2WiFi[%4d:%4d]: ", gizGetTimerCount(), len);
    for(i=0; i<len; i++)
    {
        GIZWITS_LOG("%02x ", buf[i]);
    }
    GIZWITS_LOG("\n");
    #endif

    for(i=0; i<len; i++)
    {
        //USART_SendData(UART, buf[i]);//STM32 test demo
				UsartSendDex(1,buf[i]);
        //Serial port to achieve the function, the buf[i] sent to the module
        if(i >=2 && buf[i] == 0xFF)
        {
          //Serial port to achieve the function, the 0x55 sent to the module
					UsartSendDex(1,0x55);
          //USART_SendData(UART, 0x55);//STM32 test demo
        }
    }


    
    return len;
}


