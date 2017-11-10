#ifndef _APP_H
#define _APP_H
#include"include.h"
#include "ucos_ii.h"
//ʵ��ϵͳ��������ȣ��������ʵ��
//�������ȼ��õ��趨
//��ջ�Ĵ�С�趨
//��Ϣ���У��ź��������䣬ȫ�ֱ���������
//���ڳ�ʼ�������У�������ʼ��������printf���п���������ѭ��
//GUI�����ʼ����
//�ܹ�������5������
extern OS_TMR   * tmr1; //�ⲿ����
extern OS_TMR   * tmr2; //�ⲿ����
extern OS_TMR   * tmr4;

extern OS_EVENT * sem_data_handle;
//START����
//�����������ȼ�
//��ʼ����                             �������ȼ�    �����ջ��С
#define START_TASK_PRIO                   	10                     
#define START_STK_SIZE				        30
void start_task(void *pdata);

//��Ҫ��ʾ����
#define MainUI_TASK_PRIO			  		5
#define MainUI_STK_SIZE				  		512 
void MainUI_task(void *pdata);

//�ƶ������ϴ�����
#define DataCloud_TASK_PRIO                	3
#define DataCloud_STK_SIZE                  650    
void DataCloud_task(void *pdata);

//���������ݼ���һЩ״̬�ж�����
#define PRIOR_TASK_PRIO               		4
#define Prior_STK_SIZE           			300
void Prior_Task(void *pdata);

#endif



