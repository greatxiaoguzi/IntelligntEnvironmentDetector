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
extern GUI_CONST_STORAGE GUI_BITMAP bmonitor;   //�ⲿλͼ��״̬���
extern GUI_CONST_STORAGE GUI_BITMAP bmSysSet;   //ϵͳ����λͼ
extern GUI_CONST_STORAGE GUI_BITMAP bmInternet;
//static WM_CALLBACK * _pcbDialogGraph;
static GRAPH_DATA_Handle _ahData[4];   //�����������ߵľ��
static I16 _aValue[4];    //Ҫ��ʾ��������
static int TextDsp[]={ID_DTEXT_0,ID_DTEXT_1,ID_DTEXT_2,ID_DTEXT_3};    //��ʾ״̬��λ��̬����
static u8 DeletTask=0;
static u8 ChoseChanl=1;
BUTTON_Handle  hButton_mainmenu[4];   //������İ�ť��� 
TEXT_Handle  hText_MainMenu[5];       //������˵��ı����
///////////////////////////////////////////////////////////
//GUI_PID_STATE  TouchPoint[2];
u16 TouchSum = 0; //�ۼƴӴ������ͷŻ����ľ��룬�����ж��ǻ������ǵ㴥�����ǻ�����ִ�в�����
//u8 TouchCmd = 0;  //��־���ǵ㴥���ǻ���������
//u8 TouchFlag = 0; //������־��
//u8 Menu_ID  = 0;  //�˵������ID
//GUI_COLOR  ThemeColor = GUI_LIGHTBLUE;
GUI_COLOR  DesktopColor = GUI_LIGHTBLUE;
SingleList  MainMenu_pHead;     /* ������Ŀؼ�����ͷָ��          */
SingleList  OtherMenu_pHead;    /* ��������Ŀؼ������ͷָ��      */
MenuInfo    OtherMenu_Info;
u8 MenuDisnable = 0;
u8 SysTimeHour=1;
u8 SysTimeMinute=1;
u8 SysTimeDay=1;
u8 bufbmp[]="0:/PHOTOS/1.BMP";
////////////////////////////////////////////////////////////

/*********************************************************************
*
*       _aDialogCreateҪ�����ĶԻ�����Դ�б�
*/
static const GUI_WIDGET_CREATE_INFO MonitorDialog[] = {
  { FRAMEWIN_CreateIndirect, "״̬���", ID_FRAMEWIN_0, 0, 0, 320, 240, 0, 0x0, 0 },
  { GRAPH_CreateIndirect, "Graph", ID_GRAPH_0, 0, 15, 267, 160, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "�˳�", ID_BUTTON_0, 270, 195, 40, 20, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "ת��", ID_BUTTON_1, 225, 195, 40, 20, 0, 0x0, 0 },	
  { TEXT_CreateIndirect, "00��", ID_DTEXT_0, 0, 175, 50, 23, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "00��", ID_DTEXT_1, 58, 175, 50, 23, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "00CD", ID_DTEXT_2, 118, 175, 58, 23, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "00%", ID_DTEXT_3, 180, 175, 50, 23, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "1", ID_DTEXT_4, 50, 0, 20, 15, 0, 0x64, 0 },
  { PROGBAR_CreateIndirect, "Progbar", ID_PROGBAR_0, 269, 15, 20, 160, 1, 0x0, 0 },
  { PROGBAR_CreateIndirect, "Progbar", ID_PROGBAR_1, 292, 15, 20, 160, 1, 0x0, 0 },
  { TEXT_CreateIndirect, "����", ID_TEXT_0, 0, 0, 111, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "�¶�1", ID_TEXT_1, 3, 200, 45, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "�¶�2", ID_TEXT_2, 65, 200, 45, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "��ǿ", ID_TEXT_3, 124, 200, 45, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "ʪ��", ID_TEXT_4, 183, 200, 45, 20, 0, 0x0, 0 },
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
				 _aValue[i]=User_data[i];  //�ⲿ���鴫�ݸ���Ч����
			   if(_aValue[i]>135)
				   _aValue[i]=135;
			   else if(_aValue[i]<0)
				   _aValue[i]=0;
	   }
		 GRAPH_DATA_YT_AddValue(_ahData[i],_aValue[i]);  //ȷ������һ���������
	}
}
void MonitorInit(WM_MESSAGE *pMsg)
{
    int i;
	  WM_HWIN      hItem;                    //����
	  WM_HWIN      hDlg;
	  static GRAPH_SCALE_Handle _hScaleV;    //���崹ֱ�����������
    static GRAPH_SCALE_Handle _hScaleH;    //����ˮƽ�����������
	  static GUI_COLOR _aColor[]={GUI_RED,GUI_GREEN,GUI_BLUE,GUI_YELLOW};    //���������ɫ��������ɫ
    hItem=pMsg->hWin; //��ȡ���
	  hDlg=pMsg->hWin;//�����ڶԻ�����Ϣ����Ի�����
	  FRAMEWIN_SetSkin(hItem,FRAMEWIN_SKIN_FLEX);//���öԻ���Ƥ��
    FRAMEWIN_SetFont(hItem, &GUI_FontHZ16);
    FRAMEWIN_SetTextColor(hItem, 0x00400040);
    FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    //
    // Initialization of 'Graph'//��ʼ����ͼ����
	  //
    hItem=WM_GetDialogItem(hDlg,ID_GRAPH_0);//�õ���Ӧ�ľ��
    WIDGET_SetEffect(hItem,&WIDGET_Effect_3D);//���û�ͼ����Ϊ3Dģʽ
	  GRAPH_SetBorder(hItem,19,0,0,13); //����ͼ�ο�߽�Ŀ��
    for(i=0;i<GUI_COUNTOF(_aColor);i++)//Ϊ��ͼ�ؼ�������ݶ���һ������������
	  {
			  _ahData[i]=GRAPH_DATA_YT_Create(_aColor[i],500,0,0);     //��ʾ��������ɫ��������ʾ��������ݸ���������ָ��
			  GRAPH_DATA_YT_SetAlign(_ahData[i], GRAPH_ALIGN_LEFT);    //ɨ�跽ʽΪ��������
			  GRAPH_DATA_YT_MirrorX (_ahData[i], 1);
			  GRAPH_AttachData(hItem,_ahData[i]);   //Ϊ��ͼ�ؼ�������ݶ���
    }
    GRAPH_SetGridDistY(hItem,15);    //���ô�ֱդ��ĸ߶�
	  GRAPH_SetGridVis(hItem,1);    //դ���Ƿ�ɼ�
    GRAPH_SetGridDistX(hItem,10);   //����ˮƽդ��Ŀ��
	  GRAPH_SetGridVis(hItem,1);   //ˮƽդ���Ƿ�ɼ�
    _hScaleV=GRAPH_SCALE_Create(18,GUI_TA_RIGHT,GRAPH_SCALE_CF_VERTICAL,15);   //����ߵĳ߶�λ��
    GRAPH_SCALE_SetTextColor(_hScaleV,GUI_BLUE);   //���ñ�ǩ������ɫ
    GRAPH_AttachScale(hItem,_hScaleV);   //����ǩ��ӵ���ֱ����
    _hScaleH=GRAPH_SCALE_Create(150,GUI_TA_HCENTER,GRAPH_SCALE_CF_HORIZONTAL,20);   //���ϱߵĳ߶�λ��
	  GRAPH_SCALE_SetFactor(_hScaleH,0.1);//��ʹ������ʱ���̶ȶ���ĵ�λΪ �����ء�����˸�������Ӧ������ֵת��Ϊ����ĵ�λ
    GRAPH_SCALE_SetTextColor(_hScaleH,GUI_BLUE);   //���ñ�ǩ������ɫ
    GRAPH_AttachScale(hItem,_hScaleH);   //����ǩ��ӵ�ˮƽ����
	  GRAPH_SetColor(hItem,GUI_LIGHTGREEN,GRAPH_CI_BORDER);
	  //GRAPH_SetColor(hItem,GUI_WHITE,GRAPH_CI_GRID);
	  //
	  //Initialization of 'BUTTON0'
	  //
    hItem=WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
	  BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);   //���ð�ťƤ�� 
		BUTTON_SetFont(hItem,&GUI_FontHZ12);
	  BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
		//
	  //Initialization of 'BUTTON1'
	  //
    hItem=WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
	  BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);   //���ð�ťƤ�� 
		BUTTON_SetFont(hItem,&GUI_FontHZ12);
	  BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
    //
    // Initialization of 'DText0'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_DTEXT_0);//�õ��ı��༭����
		TEXT_SetFont(hItem, &GUI_FontHZ24);//�����ı�����Ĵ�С����ʽ
		TEXT_SetTextColor(hItem,GUI_RED);//����������ɫ��
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
    TEXT_SetTextColor(hItem, GUI_RED);//�¶�1����ɫ
    //
    // Initialization of 'Temp2'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
    TEXT_SetFont(hItem,&GUI_FontHZ16);
    TEXT_SetTextColor(hItem, GUI_GREEN);//�¶�2����ɫ
    //
    // Initialization of 'Light'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
    TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetTextColor(hItem, GUI_BLUE);//��ǿ��ɫ
    //
    // Initialization of 'Damp'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
    TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetTextColor(hItem, GUI_YELLOW);//ʪ����ɫ
    //
    // Initialization of 'AD1'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
    TEXT_SetFont(hItem, &GUI_FontHZ16);
    TEXT_SetTextColor(hItem, GUI_LIGHTBLUE);//AD1����ɫ
    //
    // Initialization of 'AD2'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
    TEXT_SetFont(hItem,&GUI_FontHZ16);
    TEXT_SetTextColor(hItem, GUI_LIGHTGREEN);//AD2����ɫ
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
		PROGBAR_SetSkin(hItem, PROGBAR_SKIN_FLEX);  //���ý�����1��Ƥ��
		PROGBAR_SetMinMax(hItem, 0, 255);
		//PROGBAR_SetValue(hItem, 50);
		//
			//Initialization of 'Progbar1'
		//
    hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_1);
		PROGBAR_SetSkin(hItem, PROGBAR_SKIN_FLEX);  //���ý�����2��Ƥ��
		PROGBAR_SetMinMax(hItem, 0, 255);
		//PROGBAR_SetValue(hItem, 50);
}
/*********************************************************************
*
*       MonitorCallBack�Ի���ص�����
*/
static void MonitorCallBack(WM_MESSAGE * pMsg) {            //���洰�ڻص�����
  int          NCode;
  int          Id;
	char buf[5];
	WM_HWIN   hItem;
  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:                                  //��ʼ��������Ϣ
		   MonitorInit(pMsg);
       break;
  case WM_NOTIFY_PARENT:
				Id    = WM_GetId(pMsg->hWinSrc);
				NCode = pMsg->Data.v;
				switch(NCode) {
							case WM_NOTIFICATION_RELEASED:           // Notifications sent by 'Button' ��ť�����Ӧ
							{
									if(Id==ID_BUTTON_0)
									{
											DeletTask=1;  //ɾ�������־��һ
											GUI_EndDialog(pMsg->hWin, 1);   //�����Ի���
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
	 u8 status;      //״̬����ֵ
	 char buf[10];
   WM_HWIN hGraph; //����Ի�����  
   WM_HWIN hWIN;   //�����ڴ������     
	 WM_HWIN hPrgbr;
	 WM_HWIN   hDlg;
	 WM_HWIN   hText;
   #if GUI_SUPPORT_MEMDEV  //���еĴ����Զ���ʹ���ڴ��豸,��Ȼͼ���������������
      WM_SetCreateFlags(WM_CF_MEMDEV);
   #endif
   hWIN = GUI_CreateDialogBox(MonitorDialog, GUI_COUNTOF(MonitorDialog), MonitorCallBack, WM_HBKWIN, 0, 0);
	 NRF24L01_RX_Mode();
   while(1)
   {
			 status=NRF24L01_Read_Buf(STATUS,Che_StaReg,1); //��ȡ״̬�Ĵ�����ֵ
			 if(!NRF24L01_RxPacket(User_data))              //�ж��Ƿ���ܵ�����		
			 {
					//NRF24L01_RxPacket(User_data);
					Chanl=(status&0x0e)>>1;                    //��ȡ���ĸ�ͨ�����ܵ������� 		
          if(Chanl==ChoseChanl)
				  {				 
						 hGraph=WM_GetDialogItem(hWIN,ID_GRAPH_0);      //�õ�GRAPH���
						 _AddValues(hGraph);                            //����ֵ�����ͼ����,Graph���
						 for(i=0;i<4;i++)
				     {
								hDlg=WM_GetClientWindow(hWIN);   //�õ����ھ��
								hText=WM_GetDialogItem(hDlg, TextDsp[i]);    //�õ��ı��༭��ľ��
								//EDIT_SetValue(hEdit,_aValue[i]);  //��ֵ���ı��༭��
							 switch(i)
							 {
								 case 0:
								 {
								     sprintf(buf,"%0.2d��",_aValue[0]);
							       TEXT_SetText(hText, buf);
								 }break;
								 case 1:
								 {
								     sprintf(buf,"%0.2d��",_aValue[1]);
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
						   GRAPH_DATA_YT_Clear(_ahData[i]);//�����ʾ������ͼ��������
						   _aValue[i]=0;
						   hDlg=WM_GetClientWindow(hWIN);   //�õ����ھ��
						   hEdit=WM_GetDialogItem(hDlg, EdtDsp[i]);    //�õ��ı��༭��ľ��
						   EDIT_SetValue(hEdit,_aValue[i]);  //��ֵ���ı��༭��
					  }	 
				 }*/
			 }
		 GUI_Exec();      //����ִ�л�ͼ
		 if(DeletTask)
		 {
			  DeletTask=0;	
				Go_MainMenu();              //����������
			  Chanl=0;
			  ChoseChanl=1;
			 //for(i=0;i<6;i++)
				  //User_data[i]=0;
			 for(i=0;i<4;i++)
				  _aValue[i]=0;
				OSTaskDel(OS_PRIO_SELF);    //ɾ���Լ�				 
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
		FatFs_Open("0:/PHOTOS");//��SD������ȡ��Ŀ¼�б�	
    BmpDecode((u8 *)bufbmp[0],0,0,320,240,1);
			*for(i=1;i<12;i++)
			{
			    BmpDecode((u8 *)bufbmp[i],0,0,320,240,1);
			    delay_ms(500);
			}*
}*/
void MainMenuInit(void)             //�������ʼ������
{
		u8 i;	
	  SingleList   pos   = NULL;
	  MainMenu_pHead = SingleListNodeCreate();  /* �������������ͷ�ڵ㣬���������Զ���ᱻ����    */
	  pos = MainMenu_pHead;
	  //Menu_ID = MainMenu_ID;
	  WM_SetDesktopColor(DesktopColor);
		GUI_CURSOR_Show();
		BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);    //���ð�ťƤ��
		hButton_mainmenu[0] = BUTTON_Create(30,55 ,56,58, GUI_ID_BUTTON0, WM_CF_SHOW);
		hButton_mainmenu[1] = BUTTON_Create(130,55 ,56,58, GUI_ID_BUTTON1, WM_CF_SHOW);
		hButton_mainmenu[2] = BUTTON_Create(230,55 ,56,58, GUI_ID_BUTTON2, WM_CF_SHOW);
		for (i = 0;i < 6;i++)
		{
//			BUTTON_SetTextAlign(hButton_mainmenu[i],GUI_TA_HCENTER | GUI_TA_VCENTER);
			pos = SingleListInsert(MainMenu_pHead,pos,hButton_mainmenu[i]);    //�����������水ť�����������
		}
		BUTTON_SetBitmap(hButton_mainmenu[0],0,&bmonitor);
		BUTTON_SetBitmap(hButton_mainmenu[1],0,&bmSysSet);;
		BUTTON_SetBitmap(hButton_mainmenu[2],0,&bmInternet);		
		
		hText_MainMenu[0]=TEXT_Create(5,5,190,30,GUI_ID_TEXT0,WM_CF_SHOW,"�������ϵͳ",TEXT_CF_LEFT);      //�������ı���ʾ
		TEXT_SetFont(hText_MainMenu[0],&GUI_FontHZ24);
		TEXT_SetTextColor(hText_MainMenu[0],GUI_RED);
		hText_MainMenu[1]=TEXT_Create(210,5,100,20,GUI_ID_TEXT1,WM_CF_SHOW,"Time",TEXT_CF_LEFT);
		//�����水ť�ı�
		hText_MainMenu[2]=TEXT_Create(30,120,70,20,GUI_ID_TEXT2,WM_CF_SHOW,"״̬���",TEXT_CF_LEFT);
		hText_MainMenu[3]=TEXT_Create(130,120,70,20,GUI_ID_TEXT4,WM_CF_SHOW,"ϵͳ����",TEXT_CF_LEFT);
		hText_MainMenu[4]=TEXT_Create(233,120,70,20,GUI_ID_TEXT6,WM_CF_SHOW,"����",TEXT_CF_LEFT);
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
//������ʱ����ʾ����
void DispTime(void)
{
		char buf[30];
    RTC_Get();
	  switch(calendar.week)
	  {
			case 0:sprintf(buf,"%0.2d:%0.2d ������", calendar.hour, calendar.min);break;
			case 1:sprintf(buf,"%0.2d:%0.2d ����һ", calendar.hour, calendar.min);break;
			case 2:sprintf(buf,"%0.2d:%0.2d ���ڶ�", calendar.hour, calendar.min);break;
			case 3:sprintf(buf,"%0.2d:%0.2d ������", calendar.hour, calendar.min);break;
			case 4:sprintf(buf,"%0.2d:%0.2d ������", calendar.hour, calendar.min);break;
			case 5:sprintf(buf,"%0.2d:%0.2d ������", calendar.hour, calendar.min);break;
			case 6:sprintf(buf,"%0.2d:%0.2d ������", calendar.hour, calendar.min);break;
			default: break;
	  }
	  
	  TEXT_SetText(hText_MainMenu[1], buf);
}
/********************************************************************************************************** 
** Function name:      Go_MainMenu
** Descriptions:       ����������˵�
** input parameters:   ��
** output parameters:  
** Returned value:     ��
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
	WM_Move(MainMenu_pHead,-320,0);    //ǡ�����Ƶ���������30��
	WM_Move(MainMenu_pHead,30 - WM_GetWindowOrgX(hButton_mainmenu[0]),0);
	//Menu_ID = MainMenu_ID;
}
/********************************************************************************************************** 
** Function name:      Out_MainMenu
** Descriptions:       �˳�������˵�
** input parameters:   ��
** output parameters:  
** Returned value:     ��
**********************************************************************************************************/
void Out_MainMenu(void)
{ 
	while(WM_GetWindowOrgX(hButton_mainmenu[0]) < 320) {
		WM_Move(MainMenu_pHead,320,0);   //��ֵ�������290��290+30=320��Ȼ�˵����治���Ƴ���������Һ������������
  }
}
/********************************************************************************************************** 
** Function name:      Out_OtherMenu
** Descriptions:       �˳������ķ�������˵���ɾ����Ҫ�˳�����������пؼ���
** input parameters:   ��
** output parameters:  
** Returned value:     ��
**********************************************************************************************************/
void Out_OtherMenu(void)
{
	//u8 i = 0;
	SingleList   pos = OtherMenu_pHead;
	//for(;i < 6;i++)
	WM_Move(OtherMenu_pHead,350,0);//�����˳�
	
	while(pos) {
	  WM_DeleteWindow(pos -> handle);  //ɾ�������еĿؼ�
		pos = pos->next;
	}
	SingleListFree(OtherMenu_pHead);     //�ͷ�����
}
/********************************************************************************************************** 
** Function name:      WM_Move
** Descriptions:       ���������ڵ����пؼ�
** input parameters:   SingleList pHead�������ͷָ��
                       int dx,int dy ��Ҫ�ƶ���λ����
** output parameters:  
** Returned value:     ��
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
	GUI_Exec();//�ػ�
}
/********************************************************************************************************** 
** Function name:      SingleListNodeCreate
** Descriptions:       �������������ͷ�ڵ�
** input parameters:   ��
** output parameters:  
** Returned value:     ����ͷָ��
**********************************************************************************************************/
SingleList SingleListNodeCreate ()
{
	SingleList pHead = (SingleList)malloc(sizeof(SingleListNode));
	pHead->next=NULL;
	return pHead;
}
/********************************************************************************************************* 
** Function name:      SingleListInsert
** Descriptions:       ���������������
** input parameters:   SingleList pList: Ҫ���������
                       SingleList pos: Ҫ����Ľڵ�
                       WM_HMEM x: Ҫ�������ֵ
** output parameters:  
** Returned value:     ����ڵ��ָ��
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
** Descriptions:       �ͷ�����ռ�õĿռ�
** input parameters:   CharIn pHead: ����ͷָ��
** output parameters:  
** Returned value:     ��
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
** Descriptions:       ���ṹ����0
** input parameters:   unsigned char *pta���ṹ�����ָ��
                       int size���ṹ��Ĵ�С
** output parameters:  
** Returned value:     ��
**********************************************************************************************************/
void ClearStruct(unsigned char *pta, int size )  
{  
  while(size){  
    *pta++ = 0;  
    size--;  
  }  
}
#endif



