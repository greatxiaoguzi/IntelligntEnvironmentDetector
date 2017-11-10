#ifndef _REMOTECTRL_C
#define _REMOTECTRL_C
#include "include.h"
//定义ID号
#define ID_FRAMEWIN_0          (GUI_ID_USER + 0x57)
#define ID_BUTTON_Quit         (GUI_ID_USER + 0x58)
#define ID_BUTTON_Forward      (GUI_ID_USER + 0x59)
#define ID_BUTTON_Backward     (GUI_ID_USER + 0x5A)
#define ID_SLIDER_Dir          (GUI_ID_USER + 0x5B)
#define ID_SLIDER_Acce         (GUI_ID_USER + 0x5C)   //油门
#define ID_TEXT_Dir            (GUI_ID_USER + 0x5D)
#define ID_TEXT_Acce           (GUI_ID_USER + 0x5E)
#define ID_TEXT_SerVal         (GUI_ID_USER + 0x5F)   //舵机的文本显示
#define ID_TEXT_AccVal         (GUI_ID_USER + 0x60)   //油门的文本显示

static u8 DeletTask=0;
//定义资源列表
static const GUI_WIDGET_CREATE_INFO RemoteCtrlDialog[] = {
  { FRAMEWIN_CreateIndirect, "Remote Control", ID_FRAMEWIN_0, 0, 0, 320, 240, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Forward", ID_BUTTON_Forward, 230, 106, 63, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Backward", ID_BUTTON_Backward, 230, 162, 63, 20, 0, 0x0, 0 },
  { SLIDER_CreateIndirect, "Slider", ID_SLIDER_Dir, 22, 168, 130, 20, 0, 0x0, 0 },
  { SLIDER_CreateIndirect, "Slider", ID_SLIDER_Acce, 22, 80, 130, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Dir", ID_TEXT_Dir, 66, 188, 36, 20, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "Acc", ID_TEXT_Acce, 66, 100, 36, 20, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "0", ID_TEXT_SerVal, 66, 14, 60, 40, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "0",  ID_TEXT_AccVal, 160, 14, 60, 40, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Quit", ID_BUTTON_Quit, 230, 14, 63, 20, 0, 0x0, 0 },
};
//窗口回调函数
static void RemoteCtrlCallBack(WM_MESSAGE * pMsg) {
	WM_HWIN hItem;
	WM_HWIN hSlider;
  int NCode;
  int Id;
	char  TextServer[3]={0,0,0};    //舵机字符存储
	char TextAcces[3]={0,0,0};     //油门值字符存储
  switch (pMsg->MsgId) {
		case WM_INIT_DIALOG:       //界面控件初始化
		{ 
			//初始化按钮:前进
        hItem=WM_GetDialogItem(pMsg->hWin, ID_BUTTON_Forward);
			  BUTTON_SetFont(hItem,GUI_FONT_16B_1);
	      BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);   //设置按钮皮肤 
	      BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
			  //后退
			  hItem=WM_GetDialogItem(pMsg->hWin, ID_BUTTON_Backward);
			  BUTTON_SetFont(hItem,GUI_FONT_16B_1);
	      BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);    
	      BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
			  //退出界面按钮
			  hItem=WM_GetDialogItem(pMsg->hWin, ID_BUTTON_Quit);
			  BUTTON_SetFont(hItem,GUI_FONT_16B_1);
	      BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);   
	      BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
			  //方向文本
			  hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_Dir);
        TEXT_SetFont(hItem,GUI_FONT_16B_1);
        TEXT_SetTextColor(hItem, GUI_RED);	
        //油门文本
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_Acce);
        TEXT_SetFont(hItem,GUI_FONT_16B_1);
        TEXT_SetTextColor(hItem, GUI_RED);	
        //舵机显示值
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SerVal);
        TEXT_SetFont(hItem,GUI_FONT_32B_1);
        TEXT_SetTextColor(hItem, GUI_BLUE);
        //油门显示值
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_AccVal);
        TEXT_SetFont(hItem,GUI_FONT_32B_1);
        TEXT_SetTextColor(hItem, GUI_BLUE);				
        //方向滑条
			  hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_Dir);
				SLIDER_SetSkin(hItem, SLIDER_SKIN_FLEX);
				SLIDER_SetRange(hItem,51,249); 
				SLIDER_SetNumTicks(hItem,10);
				SLIDER_SetValue(hItem,150);
				//油门滑条
				hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_Acce );
				SLIDER_SetSkin(hItem, SLIDER_SKIN_FLEX);
				SLIDER_SetRange(hItem,0,90);  //设置取值范围
				SLIDER_SetNumTicks(hItem,10);
				SLIDER_SetValue(hItem,0);//油门值
		}break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
     switch(NCode) {
			 case WM_NOTIFICATION_RELEASED:
			 {
					 if(Id==ID_BUTTON_Quit)
					 {
							DeletTask=1;	
							GUI_EndDialog(pMsg->hWin, 1);	           //退出
					 }
					 else if(Id==ID_BUTTON_Forward)
					 {
						  User_data[1]=1;					 
							if(NRF24L01_TxPacket(User_data)==TX_OK)      //发送数据包
						   ;
						 else NRF24L01_TxPacket(User_data);
					 }
					 else if(Id==ID_BUTTON_Backward)
					 {
							 User_data[1]=0;
							 if(NRF24L01_TxPacket(User_data)==TX_OK)
								 ;
							  else  NRF24L01_TxPacket(User_data);
					 }
			 }
			 case WM_NOTIFICATION_VALUE_CHANGED:
			 {
			     if(Id==ID_SLIDER_Dir)
					 {
					     hSlider=WM_GetDialogItem(pMsg->hWin,ID_SLIDER_Dir);
						   User_data[0]=SLIDER_GetValue(hSlider);//方向
						   if(NRF24L01_TxPacket(User_data)==TX_OK)
                ;						     
							 else
								   NRF24L01_TxPacket(User_data);
							 TextServer[0]=User_data[0]/100+0X30;
							 TextServer[1]=User_data[0]/10%10+0X30;
							 TextServer[2]=User_data[0]%10+0X30;
							 TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_SerVal),TextServer);
					 }
					 else if(Id==ID_SLIDER_Acce)
					 {
					     hSlider=WM_GetDialogItem(pMsg->hWin,ID_SLIDER_Acce);
						   User_data[2]=SLIDER_GetValue(hSlider);//油门
						   if(NRF24L01_TxPacket(User_data)==TX_OK)
								 ;						     
							 else
								   NRF24L01_TxPacket(User_data);
							 TextAcces[0]=User_data[2]/10+0X30;
							 TextAcces[1]=User_data[2]%10+0X30;
							 TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_AccVal),TextAcces);
					 }
			 }
			 default:
        WM_DefaultProc(pMsg);
        break;
		 }
  }
}
//任务函数
void RemoteCtrl_Task(void *pdata)
{
	 WM_HWIN hWIN;   //主窗口创建句柄
   #if GUI_SUPPORT_MEMDEV             //所有的窗体自动的使用内存设备,不然图像会有闪动的现象
       WM_SetCreateFlags(WM_CF_MEMDEV);
   #endif
   hWIN = GUI_CreateDialogBox(RemoteCtrlDialog, GUI_COUNTOF(RemoteCtrlDialog), RemoteCtrlCallBack, WM_HBKWIN, 0, 0);   //得到主窗口句柄
	  //NRF_Set_Mode(1);     //设置为发射模式
	 while(1)
	 {
			 GUI_Exec();
		   if(DeletTask)
			 {
				  DeletTask=0;
			    Go_MainMenu();
				  OSTaskDel(OS_PRIO_SELF);    //删除自己
			 }
			
	 }
    
}

#endif

