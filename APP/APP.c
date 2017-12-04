#ifndef _APP_C
#define _APP_C
#include "include.h"
#define LOG_LEVEL 3
#define LOG_NAME "App"
#include "log.h"

//任务堆栈
__align(8) OS_STK MainUI_TASK_STK[MainUI_STK_SIZE];  			//主显示任务
__align(8) OS_STK DataCloud_TASK_STK[DataCloud_STK_SIZE];     	//云端数据任务
__align(8) OS_STK Prior_Task_STK[Prior_STK_SIZE];              //传感器数据及一些状态判断任务
//系统信号量
//OS_EVENT * msg_chanl;			  					//接受信息邮箱事件块指针
OS_EVENT * sem_data_handle;		      				//传感器数据处理完成后信号量释放
//系统定时器
OS_TMR   * tmr1;									//界面跳转服务
OS_TMR   * tmr2;									//自动待机服务
OS_TMR   * tmr3;									//电源键关机用服务

static uint8_t WifiOnlineDetect = 0;  //wifi是否掉线检测
static uint8_t AutostandbyCnt = 0;
//软件定时器回掉函数
//界面切换用
void tmr1_callback(OS_TMR *ptmr,void *p_arg) 
{
	uint8_t err;
	if(SysParaSetInfo.InterfaceSwitch)
	{
		AutoJumpFlag = 1;
	}
	OSTmrStop(tmr1,OS_TMR_OPT_NONE,0,&err);
}
//自动待机用
void tmr2_callback(OS_TMR *ptmr,void *p_arg) 
{
	uint8_t err;
	//先判断是否是显示界面界面如果不是则不熄屏
	if(Current_Show_Interface == Curr_Para_Show)
	{
		if(SysParaSetInfo.AutoStandbyStatus/* || HumanReact_IO_Status*/)  //如果是已经设置过了,高电平为感应到了人
		{
			AutostandbyCnt ++;
			if(AutostandbyCnt == SysParaSetInfo.AutoStandbyTime)
			{
				AutostandbyCnt = 0;
				StandbyMode = 1;         //待机标志置1
				TIM2->CCR2 = 999;  	 	//熄灭显示屏
				LCD_EnterSleep(1);
				OSTmrStop(tmr2,OS_TMR_OPT_NONE,0,&err);
			}
			//Set_PMS5003S_Status(PMS_CLOSING);  //关掉PM2.5传感器
		}
	}
}
extern const uint16_t Vol_To_ADC[13];
//熄屏状态下用于唤醒屏幕以及5V电源探测及关闭电源用到
uint16_t CloseMachineCnt = 0;
uint8_t CloseMachineFlag = 0;
void tmr3_callback(OS_TMR *ptmr,void *p_arg) 
{
	uint8_t err;
	if(Current_Show_Interface == Curr_Para_Show)   //待机延时用
	{
		if(CloseMachineFlag == 0)  //必须在没有关机状态下执行
		{
			if(POWER_DETECC==1 || SpeechReactionFlag==1)
			{
				SpeechReactionFlag = 0;
				if(SysParaSetInfo.AutoStandbyStatus/* || !HumanReact_IO_Status*/)  	//开关已开启了的话
				{
					StandbyMode = 0;
					LCD_EnterSleep(0);
					TIM2->CCR2 = BackLightBright;  	 	//开启显示屏显示
					//Set_PMS5003S_Status(PMS_RUNING);  	//开启PM2.5传感器
					OSTmrStart(tmr2,&err);	   		//开启软件定时器
				}
			}
		}
	}
	if(!StandbyMode || SensorData.BatVol<=Vol_To_ADC[3])  		//关机用,电压小于一定值的时候开始关机
	{
		if(POWER_DETECC || SensorData.BatVol<=Vol_To_ADC[3])  	//检测到有效电平
		{
			if(CloseMachineCnt++ >= 60)
			{
				CloseMachineFlag = 1;
				if(SensorData.BatVol <= Vol_To_ADC[3])
					LoadPowerOffUI(0);  //电量低关机操作
				else
					LoadPowerOffUI(1);  //正常关机操作
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
			if(CloseMachineCnt >= 60)  //用于在出现关机提示后按键松开继续关机
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
	IWDG_ReloadCounter();
	if(Beep++ ==3)
	{
		Beep = 0;
		TIM_Cmd(TIM3, DISABLE);
	}
}
//开始任务,创建各种任务
void start_task(void *pdata)
{
	uint8_t err;
    OS_CPU_SR cpu_sr=0;
    pdata = pdata;
    //msg_chanl=OSMboxCreate((void*)0);	     //创建消息邮箱
    sem_data_handle = OSSemCreate(0);				 //创建信号量	
	tmr1 = OSTmrCreate(2000,2000,OS_TMR_OPT_PERIODIC,(OS_TMR_CALLBACK)tmr1_callback,0,"tmr1",&err);  //界面切换定时
	tmr2 = OSTmrCreate(6000,6000,OS_TMR_OPT_PERIODIC,(OS_TMR_CALLBACK)tmr2_callback,0,"tmr2",&err); //自动待机定时15秒一次
	tmr3 = OSTmrCreate(5,5,OS_TMR_OPT_PERIODIC,(OS_TMR_CALLBACK)tmr3_callback,0,"tmr3",&err); //自动待机定时
	OSTmrStart(tmr3,&err);
    OSTmrStart(tmr2,&err);	//启动软件定时器2
	OSStatInit();                  //开启统计任务
    OS_ENTER_CRITICAL();           //进入临界区(关闭中断)
    OSTaskCreate(MainUI_task,(void*)0,(OS_STK*)&MainUI_TASK_STK[MainUI_STK_SIZE-1],MainUI_TASK_PRIO);       //触摸任务
    OSTaskCreate(DataCloud_task,(void*)0,(OS_STK*)&DataCloud_TASK_STK[DataCloud_STK_SIZE-1],DataCloud_TASK_PRIO);             //GUI主任务
    OSTaskCreate(Prior_Task,(void*)0,(OS_STK*)&Prior_Task_STK[Prior_STK_SIZE-1],PRIOR_TASK_PRIO);      
   
	//手机控制灯光任务  
    OSTaskSuspend(START_TASK_PRIO);      //挂起开始任务
    OS_EXIT_CRITICAL();                  //退出临界区(开中断)
}
//传感器数据及一些状态判断任务
void Prior_Task(void *pdata)
{
    while(1)
    {
			LoadSensorData();
			SpeechRecgnizeTask();
			if(nAsrStatus == LD_ASR_RUNING)
				OSSemPost(sem_data_handle); //发送数据处理完成信号量
			delay_ms(100);
    }
}
//PM2.5传感器的输出是一个模拟电压成正比的测量粉尘密度，具有0.5V/0.1mg/m3的灵敏度
//传感器数据采集任务
void MainUI_task(void *pdata)
{
	uint8_t err;
    while(1)
    {
		MenuOperate();   //处理按键响应函数 
		//IWDG_ReloadCounter();   //喂狗
    }
}
void Gizwits_Main_Init(void);
void GizwitsUpload(void);

//系统主任务包括液晶屏显示任务，网络数据传输等
void DataCloud_task(void *pdata)
{
	uint8_t err,i;
	if((SysParaSetInfo.SensorSwitch&0x04) == 0x04)  		//使能了ESP8266的话
	{
		Gizwits_Main_Init();//机智云初始化
	}
	while(1)
	{
		LOG_INFO("DataCloud_task Run\r\n");
		OSSemPend(sem_data_handle,0,&err);  //请求信号量
		//LoadSensorData();  //加载传感器数据
		GizwitsUpload();//机智云上传
		delay_ms(60000);   //任务切换
	}
}

#endif









