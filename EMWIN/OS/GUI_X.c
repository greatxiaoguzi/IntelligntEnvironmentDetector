/*********************************************************************
*          Portions COPYRIGHT 2013 STMicroelectronics                *
*          Portions SEGGER Microcontroller GmbH & Co. KG             *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2013  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.22 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUI_X.C
Purpose     : Config / System dependent externals for GUI
---------------------------END-OF-HEADER------------------------------
*/

/**
  ******************************************************************************
  * @attention
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

#include "GUI.h"
#include "ucos_ii.h"
#include "sys.h"
/*********************************************************************
*
*       Global data
*/
//volatile GUI_TIMER_TIME OS_TimeMS;
static OS_EVENT *DispSem;  //信号量
static OS_EVENT *EventMbox;  //邮箱
static OS_EVENT *KeySem;  //键值

static int      KeyPressed;
static char     KeyIsInited;
/*********************************************************************
*
*      Timing:
*                 GUI_X_GetTime()
*                 GUI_X_Delay(int)

  Some timing dependent routines require a GetTime
  and delay function. Default time unit (tick), normally is
  1 ms.
*/

//GUI_TIMER_TIME GUI_X_GetTime(void) { 
//  return OS_TimeMS;
//}
int GUI_X_GetTime(void) 
{
	return ((int)OSTimeGet());  //获取OS时间
}
void GUI_X_Delay(int ms) { 
//  int tEnd = OS_TimeMS + ms;
//  while ((tEnd - OS_TimeMS) > 0);
	  INT32U ticks;
    ticks = (ms * 1000) / OS_TICKS_PER_SEC;  //根据操作系统时钟节拍来
    OSTimeDly((INT16U)ticks);

}

/*********************************************************************
*
*       GUI_X_Init()
*
* Note:
*     GUI_X_Init() is called from GUI_Init is a possibility to init
*     some hardware which needs to be up and running before the GUI.
*     If not required, leave this routine blank.
*/

//void GUI_X_Init(void) {}


/*********************************************************************
*
*       GUI_X_ExecIdle
*
* Note:
*  Called if WM is in idle state
*/

void GUI_X_ExecIdle(void) {
GUI_X_Delay(1);
}
void GUI_X_InitOS(void)
{ 
	DispSem   = OSSemCreate(1);  //创建一个互斥型变量
    EventMbox = OSMboxCreate((void *)0);  //创建一个邮箱
}

/**
  *****************************************************************************
  * @Name   : GUI解锁
  *
  * @Brief  : 主要是建立互斥相关函数
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void GUI_X_Unlock(void)
{ 
	OSSemPost(DispSem);
}

/**
  *****************************************************************************
  * @Name   : GUI上锁
  *
  * @Brief  : 主要是建立互斥相关函数
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void GUI_X_Lock(void)
{
	INT8U err;
	
    OSSemPend(DispSem, 0, &err);
}

/**
  *****************************************************************************
  * @Name   : GUI获取当前任务ID
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
U32 GUI_X_GetTaskId(void) 
{ 
	return ((U32)(OSTCBCur->OSTCBPrio));
}

/**
  *****************************************************************************
  * @Name   : GUI等待事件发生
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void GUI_X_WaitEvent(void) 
{
	INT8U err;

    (void)OSMboxPend(EventMbox, 0, &err);
}

/**
  *****************************************************************************
  * @Name   : GUI等待事件发生
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void GUI_X_SignalEvent(void) 
{
	(void)OSMboxPost(EventMbox, (void *)1);
}

/******************************************************************************
                               对于键盘支持函数
******************************************************************************/

/**
  *****************************************************************************
  * @Name   : GUI初始化检测
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
static  void  CheckInit (void) 
{
    if (KeyIsInited == FALSE) {
        KeyIsInited = TRUE;
        GUI_X_Init();
    }
}

/**
  *****************************************************************************
  * @Name   : GUI创建按键任务
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void GUI_X_Init (void) 
{
    KeySem = OSSemCreate(0);
}

/**
  *****************************************************************************
  * @Name   : GUI得到键值
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
int  GUI_X_GetKey (void) 
{
    int r;

    r = KeyPressed;
    CheckInit();
    KeyPressed = 0;
    return (r);
}

/**
  *****************************************************************************
  * @Name   : GUI等待按键
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
int  GUI_X_WaitKey (void) 
{
    int    r;
    INT8U  err;

    CheckInit();
    if (KeyPressed == 0) {
        OSSemPend(KeySem, 0, &err);
    }
    r          = KeyPressed;
    KeyPressed = 0;
    return (r);
}

/**
  *****************************************************************************
  * @Name   : GUI等待按键
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void  GUI_X_StoreKey (int k) 
{
    KeyPressed = k;
    OSSemPost(KeySem);
}

/******************************************************************************
                               OS dependent
******************************************************************************/

void GUI_X_Log (const char *s) { }
void GUI_X_Warn (const char *s) { }
void GUI_X_ErrorOut(const char *s) { }
/*********************************************************************
*
*      Logging: OS dependent

Note:
  Logging is used in higher debug levels only. The typical target
  build does not use logging and does therefor not require any of
  the logging routines below. For a release build without logging
  the routines below may be eliminated to save some space.
  (If the linker is not function aware and eliminates unreferenced
  functions automatically)

*/

//void GUI_X_Log     (const char *s) { GUI_USE_PARA(s); }
//void GUI_X_Warn    (const char *s) { GUI_USE_PARA(s); }
//void GUI_X_ErrorOut(const char *s) { GUI_USE_PARA(s); }

/*************************** End of file ****************************/
