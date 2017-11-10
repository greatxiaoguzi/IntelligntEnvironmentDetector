#ifndef _APP_H
#define _APP_H
#include"include.h"
#include "ucos_ii.h"
//实现系统的任务调度，子任务的实现
//任务优先级得到设定
//堆栈的大小设定
//消息队列，信号量，邮箱，全局变量，数组
//串口初始化函数中，若不初始化，调用printf则有可能陷入死循环
//GUI界面初始化，
//总共创建了5个任务
extern OS_TMR   * tmr1; //外部调用
extern OS_TMR   * tmr2; //外部调用
extern OS_TMR   * tmr4;

extern OS_EVENT * sem_data_handle;
//START任务
//设置任务优先级
//开始任务                             任务优先级    任务堆栈大小
#define START_TASK_PRIO                   	10                     
#define START_STK_SIZE				        30
void start_task(void *pdata);

//主要显示任务
#define MainUI_TASK_PRIO			  		5
#define MainUI_STK_SIZE				  		512 
void MainUI_task(void *pdata);

//云端数据上传任务
#define DataCloud_TASK_PRIO                	3
#define DataCloud_STK_SIZE                  650    
void DataCloud_task(void *pdata);

//传感器数据及其一些状态判断任务
#define PRIOR_TASK_PRIO               		4
#define Prior_STK_SIZE           			300
void Prior_Task(void *pdata);

#endif



