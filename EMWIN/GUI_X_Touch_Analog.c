/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2012  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.16 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  ARM LIMITED whose registered office
is situated at  110 Fulbourn Road,  Cambridge CB1 9NJ,  England solely
for  the  purposes  of  creating  libraries  for  ARM7, ARM9, Cortex-M
series,  and   Cortex-R4   processor-based  devices,  sublicensed  and
distributed as part of the  MDK-ARM  Professional  under the terms and
conditions  of  the   End  User  License  supplied  with  the  MDK-ARM
Professional. 
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUI_TOUCH_X.C
Purpose     : Config / System dependent externals for GUI
---------------------------END-OF-HEADER------------------------------
*/
#include "GUI.h"
#include "touch.h"
#include "24cxx.h"
#define Touch_Adjust_Base     0xf0
int TOUCH_AD_y0;  //X   == 221;
int TOUCH_AD_y1;  // == 3900;

int TOUCH_AD_x0;  //Y == 160;
int TOUCH_AD_x1;  // == 3883;



extern unsigned char touch_flag;


void GUI_TOUCH_X_ActivateX(void) 
{

}


void GUI_TOUCH_X_ActivateY(void)
{

}


int  GUI_TOUCH_X_MeasureX(void) 
{
  u16  x,y;
  TP_Read_XY2(&x,&y);
	//TP_Scan(0);
  return y;

}


int  GUI_TOUCH_X_MeasureY(void) 
{
  u16  x,y;
  TP_Read_XY2(&x,&y);
	//TP_Scan(0);
  return x;

}

