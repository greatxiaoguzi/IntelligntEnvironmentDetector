#ifndef _GSMSET_C
#define _GSMSET_C
#include "include.h"
#define ID_FRAMEWIN_2        (GUI_ID_USER + 0x61)
#define ID_BUTTON_Num0       (GUI_ID_USER + 0x62)
#define ID_BUTTON_Num1       (GUI_ID_USER + 0x63)
#define ID_BUTTON_Num2       (GUI_ID_USER + 0x64)
#define ID_BUTTON_Num3       (GUI_ID_USER + 0x65)
#define ID_BUTTON_Num4       (GUI_ID_USER + 0x66)
#define ID_BUTTON_Num5       (GUI_ID_USER + 0x67)
#define ID_BUTTON_Num6       (GUI_ID_USER + 0x68)
#define ID_BUTTON_Num7       (GUI_ID_USER + 0x69)
#define ID_BUTTON_Num8       (GUI_ID_USER + 0x6A)
#define ID_BUTTON_Num9       (GUI_ID_USER + 0x6B)
#define ID_BUTTON_Clear      (GUI_ID_USER + 0x6C)
#define ID_BUTTON_OK         (GUI_ID_USER + 0x6D)
#define ID_BUTTON_Quit       (GUI_ID_USER + 0x6E)
#define ID_TEXT_Number       (GUI_ID_USER + 0x6F)
#define ID_MULTIEDIT_0       (GUI_ID_USER + 0x70)
static u8 DeletTask=0;
static u8 order=0;
char  PhoneNumber[11]="10010";
extern  GUI_CONST_STORAGE GUI_FONT GUI_FontHZ12;
extern  GUI_CONST_STORAGE GUI_FONT GUI_FontHZ16;
extern  GUI_CONST_STORAGE GUI_FONT GUI_FontHZ24;
//资源列表
static const GUI_WIDGET_CREATE_INFO GSMSetDialog[] = {
  { FRAMEWIN_CreateIndirect, "GSM设置", ID_FRAMEWIN_2, 0, 0, 320, 240, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "1", ID_BUTTON_Num1, 15, 70, 35, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "2", ID_BUTTON_Num2, 60, 70, 35, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "3", ID_BUTTON_Num3, 105, 70, 35, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "4", ID_BUTTON_Num4, 15, 110, 35, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "5", ID_BUTTON_Num5, 60, 110, 35, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "6", ID_BUTTON_Num6, 105, 110, 35, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "7", ID_BUTTON_Num7, 15, 147, 35, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "8", ID_BUTTON_Num8, 60, 147, 35, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "9", ID_BUTTON_Num9, 105, 147, 35, 30, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "清除", ID_BUTTON_Clear, 15, 185, 35, 30, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "0", ID_BUTTON_Num0, 60, 185, 35, 30, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "确认", ID_BUTTON_OK, 105, 185, 35, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "退出", ID_BUTTON_Quit, 218, 185, 80, 20, 0, 0x0, 0 }, 
  { TEXT_CreateIndirect, "", ID_TEXT_Number, 10, 31, 151, 28, 0, 0x0, 0 },
  { MULTIEDIT_CreateIndirect, "Information", ID_MULTIEDIT_0, 160, 22, 120, 148, 0, 0x0, 0 },
};

static void GSMSetCallBack(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
	int     i;
  switch (pMsg->MsgId) {
				case WM_INIT_DIALOG:
				{
             hItem=pMsg->hWin; //获取句柄
					   FRAMEWIN_SetFont(hItem, &GUI_FontHZ16);
					   FRAMEWIN_SetTextColor(hItem,GUI_WHITE);
					   //初始化电话号码显示文本
					   hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_Number);
             TEXT_SetFont(hItem, GUI_FONT_24B_1);
					   TEXT_SetTextColor(hItem,GUI_RED);
					   //初始化按钮的字体显示  "清除"
					   hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_Clear);
             BUTTON_SetFont(hItem, &GUI_FontHZ16);
					   BUTTON_SetTextColor(hItem,0,GUI_BLUE);
					   //初始化按钮的字体显示  "确认"
					   hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OK);
             BUTTON_SetFont(hItem, &GUI_FontHZ16);
					   BUTTON_SetTextColor(hItem,0,GUI_BLUE);
					   //
					   //初始化按钮的字体显示  "退出"
					   hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_Quit);
             BUTTON_SetFont(hItem, &GUI_FontHZ16);
					   BUTTON_SetTextColor(hItem,0,GUI_BLUE);
					   //
					   hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_0);
					   MULTIEDIT_SetText( hItem,Uart2InfBuf);
				}break;
				case WM_NOTIFY_PARENT:{
				  Id    = WM_GetId(pMsg->hWinSrc);
          NCode = pMsg->Data.v;
					switch(NCode)
					{
						case WM_NOTIFICATION_RELEASED:{
							if(Id==ID_BUTTON_Num1)
							    PhoneNumber[order++]='1';	    
							else if(Id==ID_BUTTON_Num2)
							    PhoneNumber[order++]='2';
							else if(Id==ID_BUTTON_Num3)
								  PhoneNumber[order++]='3';
							else if(Id==ID_BUTTON_Num4)
							    PhoneNumber[order++]='4';
							else if(Id==ID_BUTTON_Num5)
							    PhoneNumber[order++]='5';
							else if(Id==ID_BUTTON_Num6)
							    PhoneNumber[order++]='6';
							else if(Id==ID_BUTTON_Num7)
							    PhoneNumber[order++]='7';
							else if(Id==ID_BUTTON_Num8)
							    PhoneNumber[order++]='8'; 
							else if(Id==ID_BUTTON_Num9)
							    PhoneNumber[order++]='9';
							else if(Id==ID_BUTTON_Num0)
							    PhoneNumber[order++]='0';
							else if(Id==ID_BUTTON_Clear)
							{
								  for(i=0;i<11;i++)
								  {
								     order=0;
							       PhoneNumber[i]=0;     //数组全部清零
								  }
					    }
							else if(Id==ID_BUTTON_OK)
							{							
								  //CallPhone();
								  SendMessage();
					    }
							else	if(Id==ID_BUTTON_Quit)
							{
								 DeletTask=1;	 
								 GUI_EndDialog(pMsg->hWin, 1);
							}
						}
					}
				}break;
		    default:
           WM_DefaultProc(pMsg);
           break;
	}
}
void GsmSet_Task(void *pdata)
{
	 u8 i;
	 WM_HWIN hWIN;                      //主窗口创建句柄
	 order=0;
   #if GUI_SUPPORT_MEMDEV             //所有的窗体自动的使用内存设备,不然图像会有闪动的现象
       WM_SetCreateFlags(WM_CF_MEMDEV);
   #endif
   hWIN = GUI_CreateDialogBox( GSMSetDialog, GUI_COUNTOF( GSMSetDialog), GSMSetCallBack, WM_HBKWIN, 0, 0); 
   while(1)
	 {		   
	     GUI_Exec();
		   if(DeletTask)
			 {
				  DeletTask=0;	
			    Go_MainMenu();
				  OSTaskDel(OS_PRIO_SELF);    //删除自己
			 }
			 if(PhoneNumber[0]=='0')
			 {
					 for(i=0;i<11;i++)
					 {
						 order=0;
						 PhoneNumber[i]=0;
					 }
			 }
			 else if(order<11)
			    TEXT_SetText(WM_GetDialogItem(hWIN, ID_TEXT_Number),PhoneNumber);			 
	     else if(order==11)
			    order=0;
	 }
}	
	
#endif


