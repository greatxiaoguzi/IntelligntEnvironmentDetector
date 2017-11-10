#include "GUI.h"
#include "touch.h"
#include "24cxx.h"
//在使用触摸校准的时候可以用这个来读取AD的值
void Touch_MainTask(void)
{
	GUI_CURSOR_Show();
	GUI_CURSOR_Select(&GUI_CursorCrossL);
	GUI_SetBkColor(GUI_WHITE);
	GUI_SetColor(GUI_BLACK);
	GUI_Clear();
	GUI_DispString("Measurement of\nA/D converter values");
	
	while (1)
	{
		GUI_PID_STATE TouchState;
		int xPhys, yPhys;
		GUI_TOUCH_GetState(&TouchState);  /* Get the touch position in pixel */
		xPhys = GUI_TOUCH_X_MeasureX();     /* Get the A/D mesurement result in x */
		yPhys = GUI_TOUCH_X_MeasureY();     /* Get the A/D mesurement result in y */
		
		/* Display the measurement result */
		GUI_SetColor(GUI_BLUE);		
		GUI_DispStringAt("Analog input:\n", 0, 20);
		GUI_GotoY(GUI_GetDispPosY() + 2);
		GUI_DispString("x:");
		GUI_DispDec(xPhys, 4);
		GUI_DispString(", y:");
		GUI_DispDec(yPhys, 4);
		
		/* Display the according position */
		GUI_SetColor(GUI_RED);
		GUI_GotoY(GUI_GetDispPosY() + 4);
		GUI_DispString("\nPosition:\n");
		GUI_GotoY(GUI_GetDispPosY() + 2);
		GUI_DispString("x:");
		GUI_DispDec(TouchState.x,4);
		GUI_DispString(", y:");
		GUI_DispDec(TouchState.y,4);
		
		/* Wait a while */
		GUI_Delay(100);
	};
}


