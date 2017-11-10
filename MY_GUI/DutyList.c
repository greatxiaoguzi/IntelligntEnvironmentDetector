#ifndef _DUTYLIST_C
#define _DUTYLIST_C
#include "include.h"
#include "DIALOG.h"
#include "LISTVIEW.h"
#include "BUTTON.h"
#include "TEXT.h"
#include "EDIT.h"
#include "FRAMEWIN.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_Duty     			(GUI_ID_USER + 0x4D)
#define ID_LISTVIEW_Duty     			(GUI_ID_USER + 0x4E)
#define ID_BUTTON_Quit     				(GUI_ID_USER + 0x4F)
#define ID_BUTTON_OK     					(GUI_ID_USER + 0x50)
#define ID_BUTTON_Clear     			(GUI_ID_USER + 0x56)    //清除值日数据
#define ID_EDIT_SoundCmd     			(GUI_ID_USER + 0x51)
#define ID_BUTTON_Change     			(GUI_ID_USER + 0x52)
#define ID_TEXT_SoundCmd     			(GUI_ID_USER + 0x53)
#define ID_EDIT_Week     					(GUI_ID_USER + 0x54)
#define ID_TEXT_Week     			    (GUI_ID_USER + 0x55)
/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO DutyListDialog[] = {
  { FRAMEWIN_CreateIndirect, "DutyList", ID_FRAMEWIN_Duty, 0, 0, 320, 240, 0, 0x0, 0 },
  { LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_Duty, 0, 0, 320, 80, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Quit", ID_BUTTON_Quit, 245, 185, 50, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Changed", ID_BUTTON_Change, 35, 185, 50, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_SoundCmd, 185, 110, 30, 20, 0, 0x64, 0 },
  { BUTTON_CreateIndirect, "OK", ID_BUTTON_OK, 175, 185, 50, 20, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "Clear", ID_BUTTON_Clear, 105, 185, 50, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "SoundCmd", ID_TEXT_SoundCmd, 181, 150, 50, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_Week, 59, 110, 40, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Week", ID_TEXT_Week, 61, 150, 38, 20, 0, 0x0, 0 },
};
static u8 DeleTask=0;
WM_HWIN hWin;               //主窗口句柄
static u8 SoundCmdBuf[7];   //语音命令接受缓冲区
static u8 Week=1;
static u8 DutyBuf[7];     //值日表储存数据，星期
static const char * _aTable_1[][7] = {
  { "", "", "","","","",""},
  { "", "", "","","","",""},
//  { "", "", "","","","",""},
//  { "", "", "","","","",""},
};
/*********************************************************************
*
*       _cbDialog
*/
static void DutyListCallBack(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
	WM_HWIN hWin;
  int     NCode;
  int     Id;
	u8 i,j;
	hWin=pMsg->hWin;
  switch (pMsg->MsgId) {
			case WM_INIT_DIALOG:
				hItem = pMsg->hWin;
				FRAMEWIN_SetTitleHeight(hItem, 20);
				FRAMEWIN_SetTextColor(hItem, GUI_LIGHTBLUE);
				FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
				// Initialization of 'Listview'
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_Duty);
				LISTVIEW_AddColumn(hItem, 45, "Mon", GUI_TA_HCENTER | GUI_TA_VCENTER);
				LISTVIEW_AddColumn(hItem, 45, "Tue", GUI_TA_HCENTER | GUI_TA_VCENTER);
				LISTVIEW_AddColumn(hItem, 45, "Wed", GUI_TA_HCENTER | GUI_TA_VCENTER);
				LISTVIEW_AddRow(hItem, NULL);
				LISTVIEW_SetGridVis(hItem, 1);
				LISTVIEW_AddColumn(hItem, 45, "Thr", GUI_TA_HCENTER | GUI_TA_VCENTER);
				LISTVIEW_AddColumn(hItem, 45, "Fri", GUI_TA_HCENTER | GUI_TA_VCENTER);
				LISTVIEW_AddColumn(hItem, 45, "Sat", GUI_TA_HCENTER | GUI_TA_VCENTER);
				LISTVIEW_AddColumn(hItem, 45, "Sun", GUI_TA_HCENTER | GUI_TA_VCENTER);
				LISTVIEW_SetRowHeight(hItem, 14);
				for(i=0;i<2;i++)
			  {
					 LISTVIEW_AddRow(hItem,_aTable_1[i]);//增加行
			  }
				for(i=0;i<7;i++)
					for(j=0;j<2;j++)
					{
						if(DutyBuf[i]!=0)
						{
								LISTVIEW_SetItemTextColor(WM_GetDialogItem(hWin, ID_LISTVIEW_Duty),i, j,0,GUI_RED);//设置单元格文本颜色
								LISTVIEW_SetItemText(WM_GetDialogItem(hWin, ID_LISTVIEW_Duty), i, j, "Duty");   //往对应的单元格里添加数据
						}
					}
				//LISTVIEW_SetItemTextColor()；                  //设置只是难过的单元格文本的颜色
				//void LISTVIEW_GetItemText(LISTVIEW_Handle hObj, unsigned Column,unsigned Row, char * pBuffer,unsigned MaxSize);//得到指定单元格的文本,pBuffer缓冲区指针  MaxSize缓冲区大小
				// Initialization of 'Edit'
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_SoundCmd);
				EDIT_SetTextAlign(hItem, GUI_TA_RIGHT);
	      EDIT_SetHexMode(hItem,0X00,0,0XFF);                 //启用十六进制编辑模式,最大值为0XFF
	      EDIT_SetValue(hItem,0XFD);                          //给编辑框送值
				// Initialization of 'Edit'
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_Week);
				EDIT_SetText(hItem, "");
				EDIT_SetDecMode(hItem,0,0,7,0,0);                 //启用十进制编辑模式
				EDIT_SetValue(hItem,Week);                       //编辑框设置值
				break;
			case WM_NOTIFY_PARENT:
			{
					Id    = WM_GetId(pMsg->hWinSrc);
					NCode = pMsg->Data.v;
					switch(NCode) {
							case WM_NOTIFICATION_RELEASED:
							{
									if(Id==ID_BUTTON_Quit)     //退出界面
									{
										  DeleTask=1;
											GUI_EndDialog(hWin, 1);		
									}
									else if(Id==ID_BUTTON_OK)
									{
										  //写入数据缓冲区										  
										  SoundCmdBuf[Week-1]=EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, ID_EDIT_SoundCmd));
										  for(i=0;i<2;i++)   //只针对行
											{
													LISTVIEW_SetItemTextColor(WM_GetDialogItem(hWin, ID_LISTVIEW_Duty),Week-1, i,0,GUI_RED);//设置单元格文本颜色
													LISTVIEW_SetItemText(WM_GetDialogItem(hWin, ID_LISTVIEW_Duty), Week-1, i, "Duty");   //往对应的单元格里添加数据
												  DutyBuf[Week-1]=Week;    //将值日星期写入数组
													//GUI_MessageBox("Finish","OK",GUI_MESSAGEBOX_CF_MOVEABLE);
											}
									}
									else if(Id==ID_BUTTON_Change)
									{
										  Week++;
										  if(Week==8)
												Week=1;
											EDIT_SetValue(WM_GetDialogItem(pMsg->hWin, ID_EDIT_Week), Week);//文本框设置定值
									}
									else if(Id==ID_BUTTON_Clear)
									{
											for(j=0;j<7;j++)
											{
													for(i=0;i<2;i++)   //只针对行
													{
															LISTVIEW_SetItemText(WM_GetDialogItem(hWin, ID_LISTVIEW_Duty), j, i, "   ");   //往对应的单元格里添加数据
													}
													DutyBuf[j]=0;        //清除星期值日表
											}
									}									
							}break;
					}
			}break;
     default:
        WM_DefaultProc(pMsg);
        break;
  }
}
void InsertData(void)
{
	     switch(Week)
			 {
				 case 1:							
						  EDIT_SetValue(WM_GetDialogItem(hWin, ID_EDIT_SoundCmd),0XF1);        //显示语音命令的值	
              break;				 
				 case 2:
						  EDIT_SetValue(WM_GetDialogItem(hWin, ID_EDIT_SoundCmd),0XF2);        //显示语音命令的值
				      break;
				 case 3:
						  EDIT_SetValue(WM_GetDialogItem(hWin, ID_EDIT_SoundCmd),0XF3);        //显示语音命令的值
				      break;
				 case 4:
						  EDIT_SetValue(WM_GetDialogItem(hWin, ID_EDIT_SoundCmd),0XF4);        //显示语音命令的值
				      break;
				 case 5:
						  EDIT_SetValue(WM_GetDialogItem(hWin, ID_EDIT_SoundCmd),0XF5);        //显示语音命令的值
				      break;
				 case 6:
						  EDIT_SetValue(WM_GetDialogItem(hWin, ID_EDIT_SoundCmd),0XF6);        //显示语音命令的值
				      break;
				 case 7:
						  EDIT_SetValue(WM_GetDialogItem(hWin, ID_EDIT_SoundCmd),0XF7);        //显示语音命令的值
				      break;
				 default :break;        
			 }
}
//SoundCmdBuf[]//语音命令缓冲区
//DutyBuf     //指定是星期几开始值日
//值日表数据监测并且提醒
void DustMonitor(void)
{
	switch(calendar.week)
	{
			case 1://星期
			{
				if(DutyBuf[0]!=0)
				{
					if(calendar.hour==12&&calendar.min==50)   //时间提醒一分钟
						beep(1,1);
					//.....此处发送语音命令
				}
			}break;
			case 2:
			{
				if(DutyBuf[1]!=0)
				{
					if(calendar.hour==12&&calendar.min==50)   //时间提醒一分钟
						beep(1,1);
					//.....此处发送语音命令
				}
			}break;
			case 3:
			{
				if(DutyBuf[2]!=0)
				{
					if(calendar.hour==12&&calendar.min==50)   //时间提醒一分钟
						beep(1,1);
					//.....此处发送语音命令
				}
			}break;
			case 4:
			{
				if(DutyBuf[3]!=0)
				{
					if(calendar.hour==12&&calendar.min==50)   //时间提醒一分钟
						beep(1,1);
					//.....此处发送语音命令
				}
			}break;
			case 5:
			{
				if(DutyBuf[4]!=0)
				{
					if(calendar.hour==12&&calendar.min==50)   //时间提醒一分钟
						beep(1,1);
					//.....此处发送语音命令
				}
			}break;		
			case 6:
			{
				if(DutyBuf[5]!=0)
				{
					if(calendar.hour==12&&calendar.min==50)   //时间提醒一分钟
						beep(1,1);
					//.....此处发送语音命令
				}
			}break;
			case 0:
			{
				if(DutyBuf[6]!=0)//若等于零则说明未被选择，不提习惯值日
				{			   
					if(calendar.hour==12&&calendar.min==50)   //时间提醒一分钟
						beep(1,1);
					//.....此处发送语音命令
				}
			}break;
			default:beep(0,0); break;
	}
}
//值日表任务创建
//运行顺序：进入任务管理，先发人名，再发语音控制命令
//选择星期-->等待串口接受数据-->等待确认存储，包括语音命令-->若存储则进行下一步接受
//转换通道
void DutyList_Task(void *pdata)
{
	  #if GUI_SUPPORT_MEMDEV             //所有的窗体自动的使用内存设备,不然图像会有闪动的现象
        WM_SetCreateFlags(WM_CF_MEMDEV);
    #endif
	  hWin = GUI_CreateDialogBox(DutyListDialog, GUI_COUNTOF(DutyListDialog), DutyListCallBack, WM_HBKWIN, 0, 0);
    while(1)
	 {
		  GUI_Exec();
			if(DeleTask)
			{
			    DeleTask=0;
				  Go_MainMenu();
				  OSTaskDel(OS_PRIO_SELF);    //删除自己	
			}
			InsertData();
	 }
}
#endif


