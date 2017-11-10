#ifndef _MY_GUI_C
#define _MY_GUI_C
#include "DIALOG.h"
#include "FRAMEWIN.h"
#include "GRAPH.h"
#include "BUTTON.h"
#include "EDIT.h"
#include "PROGBAR.h"
#include "TEXT.h"
#include "GUI.h"
#include <math.h>
#include "include.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0     (GUI_ID_USER + 0x00)
#define ID_BUTTON_1     (GUI_ID_USER + 0x06)
#define ID_GRAPH_0     (GUI_ID_USER + 0x07)
#define ID_BUTTON_0     (GUI_ID_USER + 0x08)
#define ID_DTEXT_0     (GUI_ID_USER + 0x09)
#define ID_DTEXT_1     (GUI_ID_USER + 0x0A)
#define ID_DTEXT_2     (GUI_ID_USER + 0x0B)
#define ID_DTEXT_3     (GUI_ID_USER + 0x0C)
#define ID_DTEXT_4      (GUI_ID_USER + 0x1B)
#define ID_PROGBAR_0     (GUI_ID_USER + 0x0D)
#define ID_PROGBAR_1     (GUI_ID_USER + 0x0E)
#define ID_TEXT_0     (GUI_ID_USER + 0x0F)
#define ID_TEXT_1     (GUI_ID_USER + 0x10)
#define ID_TEXT_2     (GUI_ID_USER + 0x11)
#define ID_TEXT_3     (GUI_ID_USER + 0x12)
#define ID_TEXT_4     (GUI_ID_USER + 0x13)
#define ID_TEXT_5     (GUI_ID_USER + 0x14)
#define ID_TEXT_6     (GUI_ID_USER + 0x15)
#define ID_TEXT_7     (GUI_ID_USER + 0x16)
#define ID_TEMPERATURE (GUI_ID_USER + 0)
extern  GUI_CONST_STORAGE GUI_FONT GUI_FontHZ12;
extern  GUI_CONST_STORAGE GUI_FONT GUI_FontHZ16;
extern  GUI_CONST_STORAGE GUI_FONT GUI_FontHZ24;
extern GUI_CONST_STORAGE GUI_BITMAP bmonitor;   //外部位图，状态监控
extern GUI_CONST_STORAGE GUI_BITMAP bmSysSet;   //系统设置位图
extern GUI_CONST_STORAGE GUI_BITMAP bmInternet;
//static WM_CALLBACK * _pcbDialogGraph;
static GRAPH_DATA_Handle _ahData[4];   //定义四种曲线的句柄
static I16 _aValue[4];    //要显示的数据数
static int TextDsp[]={ID_DTEXT_0,ID_DTEXT_1,ID_DTEXT_2,ID_DTEXT_3};    //显示状态单位静态数组
static u8 DeletTask=0;
static u8 ChoseChanl=1;
BUTTON_Handle  hButton_mainmenu[4];   //主界面的按钮句柄 
TEXT_Handle  hText_MainMenu[5];       //主界面菜单文本句柄
///////////////////////////////////////////////////////////
//GUI_PID_STATE  TouchPoint[2];
u16 TouchSum = 0; //累计从触摸到释放滑动的距离，用于判断是滑动还是点触。若是滑动则不执行操作。
//u8 TouchCmd = 0;  //标志着是点触还是滑动操作。
//u8 TouchFlag = 0; //触摸标志。
//u8 Menu_ID  = 0;  //菜单界面的ID
//GUI_COLOR  ThemeColor = GUI_LIGHTBLUE;
GUI_COLOR  DesktopColor = GUI_LIGHTBLUE;
SingleList  MainMenu_pHead;     /* 主界面的控件链表头指针          */
SingleList  OtherMenu_pHead;    /* 其他界面的控件链表表头指针      */
MenuInfo    OtherMenu_Info;
u8 MenuDisnable = 0;
u8 SysTimeHour=1;
u8 SysTimeMinute=1;
u8 SysTimeDay=1;
u8 bufbmp[]="0:/PHOTOS/1.BMP";
////////////////////////////////////////////////////////////

/*********************************************************************
*
*       _aDialogCreate要创建的对话框资源列表
*/
static const GUI_WIDGET_CREATE_INFO MonitorDialog[] = {
  { FRAMEWIN_CreateIndirect, "状态监控", ID_FRAMEWIN_0, 0, 0, 320, 240, 0, 0x0, 0 },
  { GRAPH_CreateIndirect, "Graph", ID_GRAPH_0, 0, 15, 267, 160, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "退出", ID_BUTTON_0, 270, 195, 40, 20, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "转换", ID_BUTTON_1, 225, 195, 40, 20, 0, 0x0, 0 },	
  { TEXT_CreateIndirect, "00度", ID_DTEXT_0, 0, 175, 50, 23, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "00度", ID_DTEXT_1, 58, 175, 50, 23, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "00CD", ID_DTEXT_2, 118, 175, 58, 23, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "00%", ID_DTEXT_3, 180, 175, 50, 23, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "1", ID_DTEXT_4, 50, 0, 20, 15, 0, 0x64, 0 },
  { PROGBAR_CreateIndirect, "Progbar", ID_PROGBAR_0, 269, 15, 20, 160, 1, 0x0, 0 },
  { PROGBAR_CreateIndirect, "Progbar", ID_PROGBAR_1, 292, 15, 20, 160, 1, 0x0, 0 },
  { TEXT_CreateIndirect, "监测点", ID_TEXT_0, 0, 0, 111, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "温度1", ID_TEXT_1, 3, 200, 45, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "温度2", ID_TEXT_2, 65, 200, 45, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "光强", ID_TEXT_3, 124, 200, 45, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "湿度", ID_TEXT_4, 183, 200, 45, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "AD1", ID_TEXT_5, 268, 175, 40, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "AD2", ID_TEXT_6, 292, 175, 22, 20, 0, 0x0, 0 },
  //USER START (Optionally insert additional widgets)
  { TEXT_CreateIndirect, "100", ID_TEXT_7, 280, 1, 40, 20, 0, 0x0, 0 },
  // USER END 
};
/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
static void _AddValues(WM_HWIN hGraph)
{
  int i;
	//int Add;
	//int Vz;
	for(i=0;i<GUI_COUNTOF(_aValue);i++)
	{
	   //Add=rand()%(2+i*i);
     //Vz=((rand()%2)<<1)-1;
	   //_aValue[i]+=Add*Vz;
		 if(Chanl==ChoseChanl)
		 {
				 _aValue[i]=User_data[i];  //外部数组传递给有效数组
			   if(_aValue[i]>135)
				   _aValue[i]=135;
			   else if(_aValue[i]<0)
				   _aValue[i]=0;
	   }
		 GRAPH_DATA_YT_AddValue(_ahData[i],_aValue[i]);  //确定是哪一个句柄的数
	}
}
void MonitorInit(WM_MESSAGE *pMsg)
{
    int i;
	  WM_HWIN      hItem;                    //定义
	  WM_HWIN      hDlg;
	  static GRAPH_SCALE_Handle _hScaleV;    //定义垂直方向的网格句柄
    static GRAPH_SCALE_Handle _hScaleH;    //定义水平方向的网格句柄
	  static GUI_COLOR _aColor[]={GUI_RED,GUI_GREEN,GUI_BLUE,GUI_YELLOW};    //描绘曲线颜色，四种颜色
    hItem=pMsg->hWin; //获取句柄
	  hDlg=pMsg->hWin;//将窗口对话框信息赋予对话框句柄
	  FRAMEWIN_SetSkin(hItem,FRAMEWIN_SKIN_FLEX);//设置对话框皮肤
    FRAMEWIN_SetFont(hItem, &GUI_FontHZ16);
    FRAMEWIN_SetTextColor(hItem, 0x00400040);
    FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    //
    // Initialization of 'Graph'//初始化绘图窗口
	  //
    hItem=WM_GetDialogItem(hDlg,ID_GRAPH_0);//得到相应的句柄
    WIDGET_SetEffect(hItem,&WIDGET_Effect_3D);//设置绘图窗口为3D模式
	  GRAPH_SetBorder(hItem,19,0,0,13); //设置图形框边界的宽度
    for(i=0;i<GUI_COUNTOF(_aColor);i++)//为绘图控件添加数据对象，一共有四项数据
	  {
			  _ahData[i]=GRAPH_DATA_YT_Create(_aColor[i],500,0,0);     //显示的曲线颜色，可以显示的最大数据个数，数据指针
			  GRAPH_DATA_YT_SetAlign(_ahData[i], GRAPH_ALIGN_LEFT);    //扫描方式为从左向右
			  GRAPH_DATA_YT_MirrorX (_ahData[i], 1);
			  GRAPH_AttachData(hItem,_ahData[i]);   //为绘图控件添加数据对象
    }
    GRAPH_SetGridDistY(hItem,15);    //设置垂直栅格的高度
	  GRAPH_SetGridVis(hItem,1);    //栅格是否可见
    GRAPH_SetGridDistX(hItem,10);   //设置水平栅格的宽度
	  GRAPH_SetGridVis(hItem,1);   //水平栅格是否可见
    _hScaleV=GRAPH_SCALE_Create(18,GUI_TA_RIGHT,GRAPH_SCALE_CF_VERTICAL,15);   //离左边的尺度位置
    GRAPH_SCALE_SetTextColor(_hScaleV,GUI_BLUE);   //设置标签字体颜色
    GRAPH_AttachScale(hItem,_hScaleV);   //将标签添加到垂直方向
    _hScaleH=GRAPH_SCALE_Create(150,GUI_TA_HCENTER,GRAPH_SCALE_CF_HORIZONTAL,20);   //离上边的尺度位置
	  GRAPH_SCALE_SetFactor(_hScaleH,0.1);//不使用因子时，刻度对象的单位为 “像素”。因此给定因子应将像素值转换为所需的单位
    GRAPH_SCALE_SetTextColor(_hScaleH,GUI_BLUE);   //设置标签字体颜色
    GRAPH_AttachScale(hItem,_hScaleH);   //将标签添加到水平方向
	  GRAPH_SetColor(hItem,GUI_LIGHTGREEN,GRAPH_CI_BORDER);
	  //GRAPH_SetColor(hItem,GUI_WHITE,GRAPH_CI_GRID);
	  //
	  //Initialization of 'BUTTON0'
	  //
    hItem=WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
	  BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);   //设置按钮皮肤 
		BUTTON_SetFont(hItem,&GUI_FontHZ12);
	  BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
		//
	  //Initialization of 'BUTTON1'
	  //
    hItem=WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
	  BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);   //设置按钮皮肤 
		BUTTON_SetFont(hItem,&GUI_FontHZ12);
	  BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
    //
    // Initialization of 'DText0'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_DTEXT_0);//得到文本编辑框句柄
		TEXT_SetFont(hItem, &GUI_FontHZ24);//设置文本字体的大小和样式
		TEXT_SetTextColor(hItem,GUI_RED);//设置字体颜色。
    //
    // Initialization of 'DText1'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_DTEXT_1);
		TEXT_SetFont(hItem,&GUI_FontHZ24);
		TEXT_SetTextColor(hItem,GUI_GREEN);
    //
    // Initialization of 'DText2'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_DTEXT_2);
		TEXT_SetFont(hItem,&GUI_FontHZ24);
		TEXT_SetTextColor(hItem,GUI_BLUE);
    //
    // Initialization of 'DText3'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_DTEXT_3);
		TEXT_SetFont(hItem, &GUI_FontHZ24);
		TEXT_SetTextColor(hItem,GUI_YELLOW);
    //
    // Initialization of 'DText4'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_DTEXT_4);
	  TEXT_SetFont(hItem,&GUI_FontHZ16);
	  TEXT_SetTextColor(hItem,GUI_RED);
    //
    // Initialization of 'Temperature disp'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetTextColor(hItem, GUI_LIGHTRED);
    //
    // Initialization of 'Temp1'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetTextColor(hItem, GUI_RED);//温度1的颜色
    //
    // Initialization of 'Temp2'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
    TEXT_SetFont(hItem,&GUI_FontHZ16);
    TEXT_SetTextColor(hItem, GUI_GREEN);//温度2的颜色
    //
    // Initialization of 'Light'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
    TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetTextColor(hItem, GUI_BLUE);//光强颜色
    //
    // Initialization of 'Damp'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
    TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetTextColor(hItem, GUI_YELLOW);//湿度颜色
    //
    // Initialization of 'AD1'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
    TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetTextColor(hItem, GUI_LIGHTBLUE);//AD1的颜色
    //
    // Initialization of 'AD2'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
    TEXT_SetFont(hItem,&GUI_FontHZ16);
    TEXT_SetTextColor(hItem, GUI_LIGHTGREEN);//AD2的颜色
    // USER START (Optionally insert additional code for further widget initialization)
	  //
		//Initialization of "100"
		//
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_7);
    TEXT_SetFont(hItem,&GUI_FontHZ16);
		TEXT_SetTextColor(hItem, GUI_RED);
		//
			//Initialization of 'Progbar0'
		//
    hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_0);
		PROGBAR_SetSkin(hItem, PROGBAR_SKIN_FLEX);  //设置进度条1的皮肤
		PROGBAR_SetMinMax(hItem, 0, 255);
		//PROGBAR_SetValue(hItem, 50);
		//
			//Initialization of 'Progbar1'
		//
    hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_1);
		PROGBAR_SetSkin(hItem, PROGBAR_SKIN_FLEX);  //设置进度条2的皮肤
		PROGBAR_SetMinMax(hItem, 0, 255);
		//PROGBAR_SetValue(hItem, 50);
}
/*********************************************************************
*
*       MonitorCallBack对话框回调函数
*/
static void MonitorCallBack(WM_MESSAGE * pMsg) {            //桌面窗口回掉函数
  int          NCode;
  int          Id;
	char buf[5];
	WM_HWIN   hItem;
  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:                                  //初始化窗口消息
		   MonitorInit(pMsg);
       break;
  case WM_NOTIFY_PARENT:
				Id    = WM_GetId(pMsg->hWinSrc);
				NCode = pMsg->Data.v;
				switch(NCode) {
							case WM_NOTIFICATION_RELEASED:           // Notifications sent by 'Button' 按钮点击响应
							{
									if(Id==ID_BUTTON_0)
									{
											DeletTask=1;  //删除任务标志置一
											GUI_EndDialog(pMsg->hWin, 1);   //结束对话框
									}
									else if(Id==ID_BUTTON_1)
									{
										  ChoseChanl++;
                      if(ChoseChanl==6)
                         ChoseChanl=1;
									    hItem = WM_GetDialogItem(pMsg->hWin, ID_DTEXT_4);
		                  sprintf(buf,"%d",ChoseChanl);
	                    TEXT_SetText(hItem, buf);
									}
							}break;
			        default:
				          WM_DefaultProc(pMsg);
				          break;
        }
	}
}
void Monitor_Task(void * pdata)
{
   u8 i;
	 u8 status;      //状态保存值
	 char buf[10];
   WM_HWIN hGraph; //定义对话框句柄  
   WM_HWIN hWIN;   //主窗口创建句柄     
	 WM_HWIN hPrgbr;
	 WM_HWIN   hDlg;
	 WM_HWIN   hText;
   #if GUI_SUPPORT_MEMDEV  //所有的窗体自动的使用内存设备,不然图像会有闪动的现象
      WM_SetCreateFlags(WM_CF_MEMDEV);
   #endif
   hWIN = GUI_CreateDialogBox(MonitorDialog, GUI_COUNTOF(MonitorDialog), MonitorCallBack, WM_HBKWIN, 0, 0);
	 NRF24L01_RX_Mode();
   while(1)
   {
			 status=NRF24L01_Read_Buf(STATUS,Che_StaReg,1); //读取状态寄存器的值
			 if(!NRF24L01_RxPacket(User_data))              //判断是佛接受到数据		
			 {
					//NRF24L01_RxPacket(User_data);
					Chanl=(status&0x0e)>>1;                    //读取是哪个通道接受道德数据 		
          if(Chanl==ChoseChanl)
				  {				 
						 hGraph=WM_GetDialogItem(hWIN,ID_GRAPH_0);      //得到GRAPH句柄
						 _AddValues(hGraph);                            //将数值赋予绘图窗口,Graph句柄
						 for(i=0;i<4;i++)
				     {
								hDlg=WM_GetClientWindow(hWIN);   //得到窗口句柄
								hText=WM_GetDialogItem(hDlg, TextDsp[i]);    //得到文本编辑框的句柄
								//EDIT_SetValue(hEdit,_aValue[i]);  //送值给文本编辑框
							 switch(i)
							 {
								 case 0:
								 {
								     sprintf(buf,"%0.2d度",_aValue[0]);
							       TEXT_SetText(hText, buf);
								 }break;
								 case 1:
								 {
								     sprintf(buf,"%0.2d度",_aValue[1]);
							       TEXT_SetText(hText, buf);
								 }break;
								 case 2:
								 {
								     sprintf(buf,"%0.2dCD",_aValue[2]);
							       TEXT_SetText(hText, buf);
								 }break;
								 case 3:
								 {
								     sprintf(buf,"%0.2d%%",_aValue[3]);
							       TEXT_SetText(hText, buf);
								 }break;
								 default: break;
							 }							 
				     }
						 hPrgbr=WM_GetDialogItem(hDlg,ID_PROGBAR_0); 
						 PROGBAR_SetValue(hPrgbr, User_data[4]);
						 hPrgbr=WM_GetDialogItem(hDlg,ID_PROGBAR_1); 
						 PROGBAR_SetValue(hPrgbr, User_data[5]);
				  }
				 /*else
				 {
				     for(i=0;i<4;i++)
					  {
						   GRAPH_DATA_YT_Clear(_ahData[i]);//清除显示的数据图形四条线
						   _aValue[i]=0;
						   hDlg=WM_GetClientWindow(hWIN);   //得到窗口句柄
						   hEdit=WM_GetDialogItem(hDlg, EdtDsp[i]);    //得到文本编辑框的句柄
						   EDIT_SetValue(hEdit,_aValue[i]);  //送值给文本编辑框
					  }	 
				 }*/
			 }
		 GUI_Exec();      //重新执行绘图
		 if(DeletTask)
		 {
			  DeletTask=0;	
				Go_MainMenu();              //进入主界面
			  Chanl=0;
			  ChoseChanl=1;
			 //for(i=0;i<6;i++)
				  //User_data[i]=0;
			 for(i=0;i<4;i++)
				  _aValue[i]=0;
				OSTaskDel(OS_PRIO_SELF);    //删除自己				 
		 }	 
   }
}
/*void BkBmpLoad(void)
{
	 u8 retry;
	 u8 i;
		for(retry = 0;retry < 5;retry ++)
	 {
 		   if(disk_initialize (0)==0)
			 {
 		      GUI_DispStringAt("SD card init success!",50,120);
			    f_mount(0,&fs);		
       }
			 else if(retry==4)
			{
 		        GUI_DispStringAt("SD card init failed!",50,120);
       }
 	 }
	 delay_ms(500);
	  GUI_Clear();
		FatFs_Open("0:/PHOTOS");//打开SD卡，获取根目录列表	
    BmpDecode((u8 *)bufbmp[0],0,0,320,240,1);
			*for(i=1;i<12;i++)
			{
			    BmpDecode((u8 *)bufbmp[i],0,0,320,240,1);
			    delay_ms(500);
			}*
}*/
void MainMenuInit(void)             //主界面初始化函数
{
		u8 i;	
	  SingleList   pos   = NULL;
	  MainMenu_pHead = SingleListNodeCreate();  /* 创建单向链表表头节点，这个链表将永远不会被销毁    */
	  pos = MainMenu_pHead;
	  //Menu_ID = MainMenu_ID;
	  WM_SetDesktopColor(DesktopColor);
		GUI_CURSOR_Show();
		BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);    //设置按钮皮肤
		hButton_mainmenu[0] = BUTTON_Create(30,55 ,56,58, GUI_ID_BUTTON0, WM_CF_SHOW);
		hButton_mainmenu[1] = BUTTON_Create(130,55 ,56,58, GUI_ID_BUTTON1, WM_CF_SHOW);
		hButton_mainmenu[2] = BUTTON_Create(230,55 ,56,58, GUI_ID_BUTTON2, WM_CF_SHOW);
		for (i = 0;i < 6;i++)
		{
//			BUTTON_SetTextAlign(hButton_mainmenu[i],GUI_TA_HCENTER | GUI_TA_VCENTER);
			pos = SingleListInsert(MainMenu_pHead,pos,hButton_mainmenu[i]);    //将所有主界面按钮句柄插入链表
		}
		BUTTON_SetBitmap(hButton_mainmenu[0],0,&bmonitor);
		BUTTON_SetBitmap(hButton_mainmenu[1],0,&bmSysSet);;
		BUTTON_SetBitmap(hButton_mainmenu[2],0,&bmInternet);		
		
		hText_MainMenu[0]=TEXT_Create(5,5,190,30,GUI_ID_TEXT0,WM_CF_SHOW,"机房监控系统",TEXT_CF_LEFT);      //主界面文本显示
		TEXT_SetFont(hText_MainMenu[0],&GUI_FontHZ24);
		TEXT_SetTextColor(hText_MainMenu[0],GUI_RED);
		hText_MainMenu[1]=TEXT_Create(210,5,100,20,GUI_ID_TEXT1,WM_CF_SHOW,"Time",TEXT_CF_LEFT);
		//主界面按钮文本
		hText_MainMenu[2]=TEXT_Create(30,120,70,20,GUI_ID_TEXT2,WM_CF_SHOW,"状态监控",TEXT_CF_LEFT);
		hText_MainMenu[3]=TEXT_Create(130,120,70,20,GUI_ID_TEXT4,WM_CF_SHOW,"系统设置",TEXT_CF_LEFT);
		hText_MainMenu[4]=TEXT_Create(233,120,70,20,GUI_ID_TEXT6,WM_CF_SHOW,"网络",TEXT_CF_LEFT);
		for(i=1;i<5;i++)
		   TEXT_SetFont(hText_MainMenu[i],&GUI_FontHZ16);
		//
		for(i=0;i<5;i++)
		{
		    pos=SingleListInsert(MainMenu_pHead,pos,hText_MainMenu[i]);
		}		
		User_data[2]=100;
		//GUI_Exec();	 
		/*while(WM_GetWindowOrgY(hButton_mainmenu[0]) > 30) {
		if(WM_GetWindowOrgY(hButton_mainmenu[0]) > 40) {
			WM_Move(MainMenu_pHead,0,-10);
		} else {
			WM_Move(MainMenu_pHead,0,-10);
		}
  }*/
}
//主界面时间显示函数
void DispTime(void)
{
		char buf[30];
    RTC_Get();
	  switch(calendar.week)
	  {
			case 0:sprintf(buf,"%0.2d:%0.2d 星期日", calendar.hour, calendar.min);break;
			case 1:sprintf(buf,"%0.2d:%0.2d 星期一", calendar.hour, calendar.min);break;
			case 2:sprintf(buf,"%0.2d:%0.2d 星期二", calendar.hour, calendar.min);break;
			case 3:sprintf(buf,"%0.2d:%0.2d 星期三", calendar.hour, calendar.min);break;
			case 4:sprintf(buf,"%0.2d:%0.2d 星期四", calendar.hour, calendar.min);break;
			case 5:sprintf(buf,"%0.2d:%0.2d 星期五", calendar.hour, calendar.min);break;
			case 6:sprintf(buf,"%0.2d:%0.2d 星期六", calendar.hour, calendar.min);break;
			default: break;
	  }
	  
	  TEXT_SetText(hText_MainMenu[1], buf);
}
/********************************************************************************************************** 
** Function name:      Go_MainMenu
** Descriptions:       进入主界面菜单
** input parameters:   无
** output parameters:  
** Returned value:     无
**********************************************************************************************************/
void Go_MainMenu(void)
{
//	WM_Move(MainMenu_pHead,-445,0);
//	while(WM_GetWindowOrgX(hButton_mainmenu[0]) < 30) {
//		if(WM_GetWindowOrgX(hButton_mainmenu[0]) < -30 ) {
//			WM_Move(MainMenu_pHead,30,0);
//		} else {
//			WM_Move(MainMenu_pHead,30 - WM_GetWindowOrgX(hButton_mainmenu[0]),0);
//		}
//  }
	WM_Move(MainMenu_pHead,-320,0);    //恰好能移到距左屏面30处
	WM_Move(MainMenu_pHead,30 - WM_GetWindowOrgX(hButton_mainmenu[0]),0);
	//Menu_ID = MainMenu_ID;
}
/********************************************************************************************************** 
** Function name:      Out_MainMenu
** Descriptions:       退出主界面菜单
** input parameters:   无
** output parameters:  
** Returned value:     无
**********************************************************************************************************/
void Out_MainMenu(void)
{ 
	while(WM_GetWindowOrgX(hButton_mainmenu[0]) < 320) {
		WM_Move(MainMenu_pHead,320,0);   //此值必须大于290，290+30=320不然菜单界面不能移除，可能是液晶驱动的问题
  }
}
/********************************************************************************************************** 
** Function name:      Out_OtherMenu
** Descriptions:       退出其他的非主界面菜单（删除所要退出界面里的所有控件）
** input parameters:   无
** output parameters:  
** Returned value:     无
**********************************************************************************************************/
void Out_OtherMenu(void)
{
	//u8 i = 0;
	SingleList   pos = OtherMenu_pHead;
	//for(;i < 6;i++)
	WM_Move(OtherMenu_pHead,350,0);//滑动退出
	
	while(pos) {
	  WM_DeleteWindow(pos -> handle);  //删除链表中的控件
		pos = pos->next;
	}
	SingleListFree(OtherMenu_pHead);     //释放链表
}
/********************************************************************************************************** 
** Function name:      WM_Move
** Descriptions:       滑动链表内的所有控件
** input parameters:   SingleList pHead：链表的头指针
                       int dx,int dy ：要移动的位移量
** output parameters:  
** Returned value:     无
**********************************************************************************************************/
void WM_Move(SingleList pHead ,int dx,int dy)
{
	SingleList   pos = pHead;
	if(dy < 0){
		TouchSum += -dy;
	} else {
		TouchSum +=  dy;
	}
	while(pos) {
	  WM_MoveWindow (pos -> handle, dx, dy);
		pos = pos->next;
	}
	GUI_Exec();//重绘
}
/********************************************************************************************************** 
** Function name:      SingleListNodeCreate
** Descriptions:       创建单向链表表头节点
** input parameters:   无
** output parameters:  
** Returned value:     链表头指针
**********************************************************************************************************/
SingleList SingleListNodeCreate ()
{
	SingleList pHead = (SingleList)malloc(sizeof(SingleListNode));
	pHead->next=NULL;
	return pHead;
}
/********************************************************************************************************* 
** Function name:      SingleListInsert
** Descriptions:       单向链表插入运算
** input parameters:   SingleList pList: 要插入的链表
                       SingleList pos: 要插入的节点
                       WM_HMEM x: 要插入的数值
** output parameters:  
** Returned value:     插入节点的指针
**********************************************************************************************************/
SingleList SingleListInsert (SingleList pList,SingleList pos, WM_HMEM x)
{
	SingleList ptr = (SingleList)malloc(sizeof(SingleListNode));
	ptr->handle    = x;
	ptr->next      = pos->next;
	pos->next      = ptr;
	return ptr;
}
/********************************************************************************************************** 
** Function name:      SingleListFree
** Descriptions:       释放链表占用的空间
** input parameters:   CharIn pHead: 链表头指针
** output parameters:  
** Returned value:     无
**********************************************************************************************************/
void SingleListFree (SingleList pHead)
{
	SingleList p = pHead;
	while(p) {
		pHead = pHead->next;
		free(p);
		p = pHead;
	}
}
/********************************************************************************************************** 
** Function name:      ClearStruct
** Descriptions:       将结构体清0
** input parameters:   unsigned char *pta：结构体的首指针
                       int size：结构体的大小
** output parameters:  
** Returned value:     无
**********************************************************************************************************/
void ClearStruct(unsigned char *pta, int size )  
{  
  while(size){  
    *pta++ = 0;  
    size--;  
  }  
}
#endif



