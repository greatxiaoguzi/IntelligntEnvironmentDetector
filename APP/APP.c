#ifndef _APP_C
#define _APP_C
#include "include.h"
//�����ջ
__align(8) OS_STK MainUI_TASK_STK[MainUI_STK_SIZE];  			//����ʾ����
__align(8) OS_STK DataCloud_TASK_STK[DataCloud_STK_SIZE];     	//�ƶ���������
__align(8) OS_STK Prior_Task_STK[Prior_STK_SIZE];              //���������ݼ�һЩ״̬�ж�����
//ϵͳ�ź���
//OS_EVENT * msg_chanl;			  					//������Ϣ�����¼���ָ��
OS_EVENT * sem_data_handle;		      				//���������ݴ�����ɺ��ź����ͷ�
//ϵͳ��ʱ��
OS_TMR   * tmr1;									//������ת����
OS_TMR   * tmr2;									//�Զ���������
OS_TMR   * tmr3;									//��Դ���ػ��÷���

static uint8_t WifiOnlineDetect = 0;  //wifi�Ƿ���߼��
static uint8_t AutostandbyCnt = 0;
//�����ʱ���ص�����
//�����л���
void tmr1_callback(OS_TMR *ptmr,void *p_arg) 
{
	uint8_t err;
	if(SysParaSetInfo.InterfaceSwitch)
	{
		AutoJumpFlag = 1;
	}
	OSTmrStop(tmr1,OS_TMR_OPT_NONE,0,&err);
}
//�Զ�������
void tmr2_callback(OS_TMR *ptmr,void *p_arg) 
{
	uint8_t err;
	//���ж��Ƿ�����ʾ����������������Ϩ��
	if(Current_Show_Interface == Curr_Para_Show)
	{
		if(SysParaSetInfo.AutoStandbyStatus/* || HumanReact_IO_Status*/)  //������Ѿ����ù���,�ߵ�ƽΪ��Ӧ������
		{
			AutostandbyCnt ++;
			if(AutostandbyCnt == SysParaSetInfo.AutoStandbyTime)
			{
				AutostandbyCnt = 0;
				StandbyMode = 1;         //������־��1
				TIM2->CCR2 = 999;  	 	//Ϩ����ʾ��
				LCD_EnterSleep(1);
				OSTmrStop(tmr2,OS_TMR_OPT_NONE,0,&err);
			}
			//Set_PMS5003S_Status(PMS_CLOSING);  //�ص�PM2.5������
		}
	}
}
extern const uint16_t Vol_To_ADC[13];
//Ϩ��״̬�����ڻ�����Ļ�Լ�5V��Դ̽�⼰�رյ�Դ�õ�
uint16_t CloseMachineCnt = 0;
uint8_t CloseMachineFlag = 0;
void tmr3_callback(OS_TMR *ptmr,void *p_arg) 
{
	uint8_t err;
	if(Current_Show_Interface == Curr_Para_Show)   //������ʱ��
	{
		if(CloseMachineFlag == 0)  //������û�йػ�״̬��ִ��
		{
			if(POWER_DETECC==1 || SpeechReactionFlag==1)
			{
				SpeechReactionFlag = 0;
				if(SysParaSetInfo.AutoStandbyStatus/* || !HumanReact_IO_Status*/)  	//�����ѿ����˵Ļ�
				{
					StandbyMode = 0;
					LCD_EnterSleep(0);
					TIM2->CCR2 = BackLightBright;  	 	//������ʾ����ʾ
					//Set_PMS5003S_Status(PMS_RUNING);  	//����PM2.5������
					OSTmrStart(tmr2,&err);	   		//���������ʱ��
				}
			}
		}
	}
	if(!StandbyMode || SensorData.BatVol<=Vol_To_ADC[3])  		//�ػ���,��ѹС��һ��ֵ��ʱ��ʼ�ػ�
	{
		if(POWER_DETECC || SensorData.BatVol<=Vol_To_ADC[3])  	//��⵽��Ч��ƽ
		{
			if(CloseMachineCnt++ >= 60)
			{
				CloseMachineFlag = 1;
				if(SensorData.BatVol <= Vol_To_ADC[3])
					LoadPowerOffUI(0);  //�����͹ػ�����
				else
					LoadPowerOffUI(1);  //�����ػ�����
				SetBeepFreq(AlarmFreqTab[6]);
				if(CloseMachineCnt > 70)
				{
					TIM2->CCR2 = 999;
					Close_FDN304P();
					CloseMachineCnt = 0;
				}
			}
		}
		else
		{
			if(CloseMachineCnt >= 60)  //�����ڳ��ֹػ���ʾ�󰴼��ɿ������ػ�
			{
				TIM2->CCR2 = 999;
				Close_FDN304P();
			}
			else
			{
				CloseMachineFlag = 0;
				CloseMachineCnt = 0;
			}
		}
	}
	if(Beep++ ==3)
	{
		Beep = 0;
		TIM_Cmd(TIM3, DISABLE);
	}
}
//��ʼ����,������������
void start_task(void *pdata)
{
	uint8_t err;
    OS_CPU_SR cpu_sr=0;
    pdata = pdata;
    //msg_chanl=OSMboxCreate((void*)0);	     //������Ϣ����
    sem_data_handle = OSSemCreate(0);				 //�����ź���	
	tmr1 = OSTmrCreate(2000,2000,OS_TMR_OPT_PERIODIC,(OS_TMR_CALLBACK)tmr1_callback,0,"tmr1",&err);  //�����л���ʱ
	tmr2 = OSTmrCreate(6000,6000,OS_TMR_OPT_PERIODIC,(OS_TMR_CALLBACK)tmr2_callback,0,"tmr2",&err); //�Զ�������ʱ15��һ��
	tmr3 = OSTmrCreate(5,5,OS_TMR_OPT_PERIODIC,(OS_TMR_CALLBACK)tmr3_callback,0,"tmr3",&err); //�Զ�������ʱ
	OSTmrStart(tmr3,&err);
    OSTmrStart(tmr2,&err);	//���������ʱ��2
	OSStatInit();                  //����ͳ������
    OS_ENTER_CRITICAL();           //�����ٽ���(�ر��ж�)
    OSTaskCreate(MainUI_task,(void*)0,(OS_STK*)&MainUI_TASK_STK[MainUI_STK_SIZE-1],MainUI_TASK_PRIO);       //��������
    OSTaskCreate(DataCloud_task,(void*)0,(OS_STK*)&DataCloud_TASK_STK[DataCloud_STK_SIZE-1],DataCloud_TASK_PRIO);             //GUI������
    OSTaskCreate(Prior_Task,(void*)0,(OS_STK*)&Prior_Task_STK[Prior_STK_SIZE-1],PRIOR_TASK_PRIO);      
   
	//�ֻ����Ƶƹ�����  
    OSTaskSuspend(START_TASK_PRIO);      //����ʼ����
    OS_EXIT_CRITICAL();                  //�˳��ٽ���(���ж�)
}
//���������ݼ�һЩ״̬�ж�����
void Prior_Task(void *pdata)
{
    while(1)
    {
		LoadSensorData();
		SpeechRecgnizeTask();
		if(nAsrStatus == LD_ASR_RUNING)
			OSSemPost(sem_data_handle); //�������ݴ�������ź���
		delay_ms(100);
    }
}
//PM2.5�������������һ��ģ���ѹ�����ȵĲ����۳��ܶȣ�����0.5V/0.1mg/m3��������
//���������ݲɼ�����
void MainUI_task(void *pdata)
{
	uint8_t err;
    while(1)
    {
		MenuOperate();   //��������Ӧ���� 
    }
}


//ϵͳ���������Һ������ʾ�����������ݴ����
void DataCloud_task(void *pdata)
{
	if((SysParaSetInfo.SensorSwitch&0x04) == 0x04)  		//ʹ����ESP8266�Ļ�
	{
		Esp8266_Config();
	}
	 while(1)
	 {
		if(!ListenRec())    	 //���ȼ������������Ƿ��н��յ�
		{
			//NETDataDeal();       //����������յ�������
		}
		else if(Current_Show_Interface != Curr_SetPara_Show)
		{
			if((SysParaSetInfo.SensorSwitch&0x04) == 0x04)
			{
				if(Esp8266InitFinishFlag)
				{
					if((SysTimeData.Second%20)!=0)   //�˴����ڽ�����������,�ʲ�ִ��
					{
						if(SysTimeData.Hour==0 && SysTimeData.Minute==0)  //������˵ڶ���Ļ��ڼ�������������ʱ����Ϣ
						{
							Esp8266_Config();
						}
						switch(Sensor_Data_With_cJson())
						{
							case 0XF1:break;
							case 0X00:
							{
								Esp8266Config.ServerLinkStat = 1;
								WifiOnlineDetect = 0;   //�������¿�ʼ�ж�
							}break;
							case 0X01:
							{
								Esp8266Config.ServerLinkStat = 0;
								SetBeepFreq(AlarmFreqTab[9]);
								WifiOnlineDetect ++;
								if(WifiOnlineDetect > 2)   //����˵��WIFI�Ѿ�ʧȥ���ӣ���Ҫ��������WIFI
								{
									WifiOnlineDetect = 0;
									Esp8266_QuitPassThroughLinkServer();
									Esp8266_DisConectServer();
//									Esp8266_ConectServer(UP_LOAD_SENSOR_DATA_MODE);
//									PkgURL(2,(uint8_t*)UserKey);
									Esp8266_Config();
								}
							}break;
							default:break;
						}
					}
				}
			}
			delay_ms(100);
		 }
		delay_ms(50);   //�����л�
	  }
}

#endif









