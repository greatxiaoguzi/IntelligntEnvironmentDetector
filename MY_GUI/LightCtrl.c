#ifndef _LIGHTCTRL_C
#define  _LIGHTCTRL_C
#include "DIALOG.h"
#include "LightCtrl.h"
#include "my_gui.h"
#include "rtc.h"  
/********************************************/

/********************************************/
//#ifndef GUI_FLASH
//#define GUI_FLASH
//#endif
//extern GUI_FLASH const GUI_FONT GUI_FontHZ_HZ_Font;     //�ⲿ�ֿ�����
extern  GUI_CONST_STORAGE GUI_FONT GUI_FontHZ12;
extern  GUI_CONST_STORAGE GUI_FONT GUI_FontHZ16;
extern  GUI_CONST_STORAGE GUI_FONT GUI_FontHZ24;
//�ƹ�Ԥ������
#define ID_FRAMEWIN_1   							(GUI_ID_USER + 0x1B)
#define ID_BUTTON_1     							(GUI_ID_USER + 0x1C)
#define ID_BUTTON_2     							(GUI_ID_USER + 0x1D)
#define ID_BUTTON_3     							(GUI_ID_USER + 0x1E)
#define ID_BUTTON_4     							(GUI_ID_USER + 0x1F)
#define ID_BUTTON_5     							(GUI_ID_USER + 0x20)
#define ID_BUTTON_6     							(GUI_ID_USER + 0x21)
#define ID_SLIDER_0     							(GUI_ID_USER + 0x22)
#define ID_SLIDER_1     							(GUI_ID_USER + 0x23)
#define ID_SLIDER_2     							(GUI_ID_USER + 0x24)
#define ID_SLIDER_3     							(GUI_ID_USER + 0x32)
#define ID_EDIT_RED       						(GUI_ID_USER + 0x25)
#define ID_EDIT_GREEN      						(GUI_ID_USER + 0x26)
#define ID_EDIT_BLUE       						(GUI_ID_USER + 0x27)
#define ID_TEXT_RED       						(GUI_ID_USER + 0x28)
#define ID_TEXT_GREEN       					(GUI_ID_USER + 0x29)
#define ID_TEXT_BLUE       						(GUI_ID_USER + 0x2A)
#define ID_TEXT_BCK         					(GUI_ID_USER + 0x31)
#define ID_CHECKBOX_0     						(GUI_ID_USER + 0x2B)
#define ID_CHECKBOX_1     						(GUI_ID_USER + 0x2C)
#define ID_CHECKBOX_2     						(GUI_ID_USER + 0x2D)
#define ID_CHECKBOX_3     						(GUI_ID_USER + 0x2E)
#define ID_BUTTON_OK      						(GUI_ID_USER + 0x2F)
#define ID_BUTTON_CANCEL     					(GUI_ID_USER + 0x30)
//����ɫ����
#define ID_BUTTON_BkClorApp     			(GUI_ID_USER + 0x33)//����ɫ����Ӧ��
#define ID_BUTTON_BkClorCancel  			(GUI_ID_USER + 0x34)//����ɫ�����˳�
#define ID_SLIDERBkClor_RED       		(GUI_ID_USER + 0x35)
#define ID_SLIDERBkClor_GREEN       	(GUI_ID_USER + 0x36)
#define ID_SLIDERBkClor_BLUE       		(GUI_ID_USER + 0x37)
#define ID_TEXTBkClor_RED       			(GUI_ID_USER + 0x38)//����ɫ�����ı��༭���ɫ
#define ID_TEXTBkClor_GREEN     			(GUI_ID_USER + 0x39)
#define ID_TEXTBkClor_BLUE      			(GUI_ID_USER + 0x3A)
//ʱ������
#define ID_BUTTONHourSetPlus    			(GUI_ID_USER + 0x3B)
#define ID_BUTTONHourSetMinus   			(GUI_ID_USER + 0x3C)
#define ID_BUTTONMinSetPlus     			(GUI_ID_USER + 0x3D)
#define ID_BUTTONMinSetMinus    			(GUI_ID_USER + 0x3E)
#define ID_TEXTHour             			(GUI_ID_USER + 0x3F)
#define ID_TEXTMin             				(GUI_ID_USER + 0x40)
#define ID_BUTTONTimeSetOK      			(GUI_ID_USER + 0x41)
#define ID_BUTTONTimeSetCancel   			(GUI_ID_USER + 0x42)
#define ID_BUTTONWeekSetPlus          (GUI_ID_USER + 0x4D)
#define ID_BUTTONWeekSetMinus         (GUI_ID_USER + 0x4E)
#define ID_TEXTDate             			(GUI_ID_USER + 0x4F)
//��ֵ���ú궨��
#define ID_BUTTONFireSetPlus          (GUI_ID_USER + 0x43)
#define ID_BUTTONFireSetMinus    			(GUI_ID_USER + 0x44)
#define ID_BUTTONTempraSetPlus				(GUI_ID_USER + 0x45)
#define ID_BUTTONTempraSetMinus				(GUI_ID_USER + 0x46)
#define ID_TEXTFire										(GUI_ID_USER + 0x47)
#define ID_TEXTTempra									(GUI_ID_USER + 0x48)
#define ID_TEXTNameFire								(GUI_ID_USER + 0x49)
#define ID_TEXTNameTmprea							(GUI_ID_USER + 0x4A)
#define ID_BUTTONThroeSetOK						(GUI_ID_USER + 0x4B)
#define ID_BUTTONThroeSetCancel				(GUI_ID_USER + 0x4C)
//�̵�����ʱ���ú궨��  
#define ID_FRAMEWIN_2                 (GUI_ID_USER + 0x71)
#define ID_BUTTONJDQHPlus							(GUI_ID_USER + 0x72)
#define ID_BUTTONJDQHMinus						(GUI_ID_USER + 0x73)
#define ID_BUTTONJDQSPlus						  (GUI_ID_USER + 0x74)
#define ID_BUTTONJDQSMinus						(GUI_ID_USER + 0x75)
#define ID_BUTTONJDQChage							(GUI_ID_USER + 0x76)
#define ID_BUTTONJDQClear							(GUI_ID_USER + 0x77)
#define ID_BUTTONJDQuit								(GUI_ID_USER + 0x78)
#define ID_TEXTJDQOrder               (GUI_ID_USER + 0x79)
#define ID_TEXTJDQHour                (GUI_ID_USER + 0x7A)
#define ID_TEXTJDQMin                 (GUI_ID_USER + 0x7B)
/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
//�ƹ���ƶԻ�����Դ�б�
//_aDialogCreate
static const GUI_WIDGET_CREATE_INFO LightCtrlDialog[] = {
  { FRAMEWIN_CreateIndirect, "��������", ID_FRAMEWIN_1, 0, 0, 320, 240, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "�̵���1", ID_BUTTON_1, 15, 11, 80, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "�̵���2", ID_BUTTON_2, 118, 11, 80, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "�̵���3", ID_BUTTON_3, 221, 11, 80, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "�̵���4", ID_BUTTON_4, 15, 47, 80, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "�̵���5", ID_BUTTON_5, 118, 46, 80, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "�̵���6", ID_BUTTON_6, 221, 45, 80, 20, 0, 0x0, 0 },
  { SLIDER_CreateIndirect, "Slider", ID_SLIDER_0, 35, 75, 130, 20, 0, 0x0, 0 },
  { SLIDER_CreateIndirect, "Slider", ID_SLIDER_1, 35, 100, 130, 20, 0, 0x0, 0 },
  { SLIDER_CreateIndirect, "Slider", ID_SLIDER_2, 35, 125, 130, 20, 0, 0x0, 0 },
	{ SLIDER_CreateIndirect, "Slider", ID_SLIDER_3, 35, 150, 130, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_RED, 166, 75, 34, 20, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_GREEN, 166, 100, 34, 20, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_BLUE, 166, 125, 36, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "��", ID_TEXT_RED, 0, 75, 32, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "��", ID_TEXT_GREEN, 0, 100, 34, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "��", ID_TEXT_BLUE, 0, 125, 41, 20, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "����", ID_TEXT_BCK, 0, 150, 41, 20, 0, 0x0, 0 }, 
  { CHECKBOX_CreateIndirect, "����Ԥ��", ID_CHECKBOX_0, 213, 75, 96, 20, 0, 0x0, 0 },
  { CHECKBOX_CreateIndirect, "�¶�Ԥ��", ID_CHECKBOX_1, 213, 100, 96, 20, 0, 0x0, 0 },
  { CHECKBOX_CreateIndirect, "����", ID_CHECKBOX_2, 213, 125, 96, 20, 0, 0x0, 0 },
  { CHECKBOX_CreateIndirect, "��ʱ����", ID_CHECKBOX_3, 213, 150, 96, 20, 0, 0x0, 0 },       
  { BUTTON_CreateIndirect, "ȡ��", ID_BUTTON_OK, 210, 186, 80, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "ȷ��", ID_BUTTON_CANCEL, 22, 185, 80, 20, 0, 0x0, 0 },
};
//����ɫ���öԻ�����Դ�б�
static const GUI_WIDGET_CREATE_INFO BkClorSetDialog[] = {
	{ FRAMEWIN_CreateIndirect,"BkSet",0,50,50,200,150,FRAMEWIN_CF_MOVEABLE},
	{ BUTTON_CreateIndirect,"ȷ��",ID_BUTTON_BkClorApp,6, 107,80, 20},
	{ BUTTON_CreateIndirect,"ȡ��",ID_BUTTON_BkClorCancel,102, 107,80, 20},	
	{ SLIDER_CreateIndirect, "Slider", ID_SLIDERBkClor_RED, 45, 8, 100, 20, 0, 0x0, 0 },
  { SLIDER_CreateIndirect, "Slider", ID_SLIDERBkClor_GREEN, 45, 40, 100, 20, 0, 0x0, 0 },
  { SLIDER_CreateIndirect, "Slider", ID_SLIDERBkClor_BLUE, 45, 69, 100, 20, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "��", ID_TEXTBkClor_RED, 5, 8, 30, 20, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "��", ID_TEXTBkClor_GREEN, 5, 40, 31, 20, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "��", ID_TEXTBkClor_BLUE, 5, 70, 28, 20, 0, 0x0, 0 },
};
//ʱ�����öԻ�����Դ�б�
static const GUI_WIDGET_CREATE_INFO TimeSetDialog[] = {
	{ FRAMEWIN_CreateIndirect, "TimeSet", 1, 150, 40, 136, 132, FRAMEWIN_CF_MOVEABLE},
  { BUTTON_CreateIndirect, "+", ID_BUTTONHourSetPlus, 5, 5, 35, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "-", ID_BUTTONHourSetMinus, 5, 50, 35, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "+", ID_BUTTONMinSetPlus, 50, 5, 35, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "-", ID_BUTTONMinSetMinus, 50, 50, 35, 20, 0, 0x0, 0 },	
	{ BUTTON_CreateIndirect, "+", ID_BUTTONWeekSetPlus, 95, 5, 35, 20, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "-", ID_BUTTONWeekSetMinus, 95, 50, 35, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "9", ID_TEXTHour, 17, 30, 25, 16, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "20", ID_TEXTMin, 55, 30, 25, 16, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "1", ID_TEXTDate, 100, 30, 25, 16, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "ȷ��", ID_BUTTONTimeSetOK, 4, 80, 50, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "ȡ��", ID_BUTTONTimeSetCancel, 72, 80, 50, 20, 0, 0x0, 0 },
};
//��ֵ���öԻ�����Դ�б�
static const GUI_WIDGET_CREATE_INFO ThroeSetDialog[] = {
	{ FRAMEWIN_CreateIndirect, "ThroeSet", 1, 50, 40, 136, 132, FRAMEWIN_CF_MOVEABLE},
  { BUTTON_CreateIndirect, "+", ID_BUTTONFireSetPlus, 15, 5, 35, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "-", ID_BUTTONFireSetMinus, 15, 50, 35, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "+", ID_BUTTONTempraSetPlus, 80, 5, 35, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "-", ID_BUTTONTempraSetMinus, 80, 50, 35, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "", ID_TEXTFire, 17, 30, 25, 16, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "", ID_TEXTTempra, 85, 30, 25, 16, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "�¶�", ID_TEXTNameFire, 80, 75, 37, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "����", ID_TEXTNameTmprea, 20, 75, 35, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "ȷ��", ID_BUTTONThroeSetOK, 4, 90, 50, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "ȡ��", ID_BUTTONThroeSetCancel, 71, 90, 50, 20, 0, 0x0, 0 },
};
//�̵�����ʱ������Դ�б�
static const GUI_WIDGET_CREATE_INFO JDQTimerDialog[] = {
  { FRAMEWIN_CreateIndirect, "JDQTimer", ID_FRAMEWIN_2, 100, 5, 185, 149, FRAMEWIN_CF_MOVEABLE},
  { BUTTON_CreateIndirect, "+", ID_BUTTONJDQHPlus, 70, 20, 32, 19, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "-", ID_BUTTONJDQHMinus, 70, 67, 32, 19, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "+", ID_BUTTONJDQSPlus, 125, 20, 32, 19, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "-", ID_BUTTONJDQSMinus, 125, 67, 32, 19, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "�ı�", ID_BUTTONJDQChage, 12, 104, 40, 25, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "���", ID_BUTTONJDQClear, 71, 105, 40, 25, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "ȷ��", ID_BUTTONJDQuit, 126, 104, 40, 25, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "1", ID_TEXTJDQOrder, 16, 44, 33, 30, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "0", ID_TEXTJDQHour, 73, 45, 38, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "0", ID_TEXTJDQMin, 131, 45, 32, 20, 0, 0x0, 0 },
};
/*********************************************************************
*
*       Static code
*
**********************************************************************/
static U8 _aColorSep[3] = {0, 0, 0};            // �ƹ�Ԥ��������ԭɫ���溯��Red, green and blue components
static U8 _aBkClorSetColorSep[3] = {0, 0, 0};   // ������ɫ���ý�����ԭɫ���溯��Red, green and blue components
static u8 DeletTask=0;
extern u8 SysTimeHour;
extern u8 SysTimeMinute;
extern u8 SysTimeDay;
static u8 ThroeFire=1;     //�ⲿ�ӿڱ���
static u8 ThroeTempra=1;   //�ⲿ�ӿڱ���
static u8 ThrFlag=0;
static u8 TimFlag=0;
static u8 BkFlag=0;
static u8 JDQFlag=0;
static u8 C1=0,C2=0,C3=0,C4=0;
u8 Fire=1;
u8 Temp=1;      //��ֵ������ʱ����
static u8 JDQOrder=1;
static u8 JDQHour=0;
static u8 JDQMin=0;
//static u8 Order=0;
static u8 JDQTimer[5][3];   //�̵�����ʱ���õ����黺����
static char JDQNum[5]="_____";     //��������ù��ļ̵�������Ŀ
//extern u8 MsgFlag;            //�ⲿ�������ñ���
static u8 MsageFlag=0;
void HomeCtrlInit(WM_MESSAGE *pMsg)       //�Ի����ʼ������
{
    WM_HWIN hItem;  //����
    hItem=pMsg->hWin; //��ȡ���
    FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    FRAMEWIN_SetFont(hItem, &GUI_FontHZ16);
    FRAMEWIN_SetTextColor(hItem, GUI_RED);
	  //Initialization of 'BUTTON0'��ʼ����ť�ؼ�0
    hItem=WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    BUTTON_SetFont(hItem,&GUI_FontHZ16);
	  BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);   //���ð�ťƤ�� 
	  BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
	  //Initialization of 'BUTTON0'��ʼ����ť�ؼ�0
    hItem=WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
    BUTTON_SetFont(hItem,&GUI_FontHZ16);
	  BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);   //���ð�ťƤ�� 
	  BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
	  //Initialization of 'BUTTON0'��ʼ����ť�ؼ�0	
    hItem=WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
    BUTTON_SetFont(hItem,&GUI_FontHZ16);
	  BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);   //���ð�ťƤ�� 
	  BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
	  //Initialization of 'BUTTON0'��ʼ����ť�ؼ�0	
    hItem=WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
    BUTTON_SetFont(hItem,&GUI_FontHZ16);
	  BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);   //���ð�ťƤ�� 
	  BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
	  //Initialization of 'BUTTON0'��ʼ����ť�ؼ�0	
    hItem=WM_GetDialogItem(pMsg->hWin, ID_BUTTON_5);
    BUTTON_SetFont(hItem,&GUI_FontHZ16);
	  BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);   //���ð�ťƤ�� 
	  BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
	  //Initialization of 'BUTTON0'��ʼ����ť�ؼ�0	
    hItem=WM_GetDialogItem(pMsg->hWin, ID_BUTTON_6);
    BUTTON_SetFont(hItem,&GUI_FontHZ16);
	  BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);   //���ð�ťƤ�� 
	  BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
	  //Initialization of 'BUTTON0'��ʼ����ť�ؼ�0	
    hItem=WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OK);
    BUTTON_SetFont(hItem,&GUI_FontHZ16);
	  BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);   //���ð�ťƤ�� 
	  BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
	  //Initialization of 'BUTTON0'��ʼ����ť�ؼ�0	
    hItem=WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CANCEL);
    BUTTON_SetFont(hItem,&GUI_FontHZ16);
	  BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);   //���ð�ťƤ�� 
	  BUTTON_SetTextColor(hItem, 0, GUI_BLUE);
    // Initialization of 'Edit0'
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_RED);
	  EDIT_SetMaxLen(hItem,3);
	  EDIT_SetDecMode(hItem,0,0,255,0,0);//����ʮ���Ʊ༭ģʽ��
	  //EDIT_SetValue(hItem,56);//�����ı���ǰֵ
    // Initialization of 'Edit1'
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_GREEN);
    EDIT_SetMaxLen(hItem,3);
	  EDIT_SetDecMode(hItem,0,0,255,0,0);//����ʮ���Ʊ༭ģʽ��
	  //EDIT_SetValue(hItem,57);//�����ı���ǰֵ
    // Initialization of 'Edit2'
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_BLUE);
    EDIT_SetMaxLen(hItem,3);
	  EDIT_SetDecMode(hItem,0,0,255,0,0);//����ʮ���Ʊ༭ģʽ��
	  //EDIT_SetValue(hItem,58);//�����ı���ǰֵ
    // Initialization of 'Text0'��ʼ���ı���0
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_RED);
    TEXT_SetFont(hItem,&GUI_FontHZ16);
    TEXT_SetTextColor(hItem, GUI_RED);//��ɫ��1����ɫ
    // Initialization of 'Text1'��ʼ���ı���1
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_GREEN);
    TEXT_SetFont(hItem,&GUI_FontHZ16);
    TEXT_SetTextColor(hItem, GUI_GREEN);//��ɫ��2����ɫ
    // Initialization of 'Text2'��ʼ���ı���2
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_BLUE);
    TEXT_SetFont(hItem,&GUI_FontHZ16);
    TEXT_SetTextColor(hItem, GUI_BLUE);//��ɫ��1����ɫ
		//
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_BCK);
    TEXT_SetFont(hItem,&GUI_FontHZ16);
    TEXT_SetTextColor(hItem, GUI_BLACK);//��ɫ��1����ɫ
    // Initialization of '�¶ȱ���'
    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
		CHECKBOX_SetState(hItem,C1);
    CHECKBOX_SetFont(hItem,&GUI_FontHZ16);
    CHECKBOX_SetText(hItem, "����Ԥ��");
    // Initialization of 'PM����'
    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
		CHECKBOX_SetState(hItem,C2);
    CHECKBOX_SetFont(hItem,&GUI_FontHZ16);
    CHECKBOX_SetText(hItem, "�¶�Ԥ��");
    // Initialization of '��������'
    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);
		CHECKBOX_SetState(hItem,C3);
    CHECKBOX_SetFont(hItem,&GUI_FontHZ16);
    CHECKBOX_SetText(hItem, "����");
    //Initialization of'ֵ������'
	  hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_3);
		CHECKBOX_SetState(hItem,C4);
    CHECKBOX_SetFont(hItem,&GUI_FontHZ16);
    CHECKBOX_SetText(hItem, "��ʱ����");
	  //'IDER0Iinitlization of"������ʼ��
	  hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0);
	  SLIDER_SetSkin(hItem, SLIDER_SKIN_FLEX);
    SLIDER_SetRange(hItem,0,255);
    SLIDER_SetNumTicks(hItem,10);
		SLIDER_SetValue(hItem,_aColorSep[0]);
	  //
	  hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_1);
	  SLIDER_SetSkin(hItem, SLIDER_SKIN_FLEX);
    SLIDER_SetRange(hItem,0,255);
    SLIDER_SetNumTicks(hItem,10);
    SLIDER_SetValue(hItem,_aColorSep[1]);
	  //
	  hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_2);
	  SLIDER_SetSkin(hItem, SLIDER_SKIN_FLEX);
    SLIDER_SetRange(hItem,0,255);
    SLIDER_SetNumTicks(hItem,10);
		SLIDER_SetValue(hItem,_aColorSep[2]);
		//
	  hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_3);
	  SLIDER_SetSkin(hItem, SLIDER_SKIN_FLEX);
    SLIDER_SetRange(hItem,0,255);
    SLIDER_SetNumTicks(hItem,10);
}
//�ƹ�Ԥ��������ɫ���ƺ��������༭��ֵ�������û��Լ���д
void  OnValueChange(WM_HWIN hDlg, int Id,u8 Chose)//�������������ݸ�ֵ��EDIT
{
  unsigned Index;
  unsigned v;
  WM_HWIN  hSlider;
  WM_HWIN  hEdit;
  Index=0;
  v=0;
	if(Chose)
	{
			if ((Id >= ID_SLIDER_0) && (Id <= ID_SLIDER_2)) {
					Index = Id - ID_SLIDER_0;
					//
					// SLIDER-widget has changed, update EDIT-widget
					//
					hSlider = WM_GetDialogItem(hDlg, ID_SLIDER_0 + Index);
					hEdit   = WM_GetDialogItem(hDlg, ID_EDIT_RED + Index);
					v = SLIDER_GetValue(hSlider);
					EDIT_SetValue(hEdit, v);  
				}
				 else if ((Id >= ID_EDIT_RED) && (Id <= ID_EDIT_GREEN)) {
					Index = Id - ID_EDIT_RED;
					//
					// If EDIT-widget has changed, update SLIDER-widget
					//
					hSlider = WM_GetDialogItem(hDlg, ID_SLIDER_0 + Index);
					hEdit   = WM_GetDialogItem(hDlg, ID_EDIT_RED + Index);
					v = EDIT_GetValue(hEdit);
					SLIDER_SetValue(hSlider, v);
			 }
				_aColorSep[Index] = v;   //��ֵ��������,
			 //
			 // At last invalidate dialog client window
			 WM_InvalidateWindow(WM_GetClientWindow(hDlg));
 }
	else
	{
	    if ((Id >= ID_SLIDERBkClor_RED) && (Id <= ID_SLIDERBkClor_BLUE)) {
					Index = Id - ID_SLIDERBkClor_RED;
					//
					// SLIDER-widget has changed, update EDIT-widget
					//
					hSlider = WM_GetDialogItem(hDlg, ID_SLIDERBkClor_RED + Index);
					v = SLIDER_GetValue(hSlider);
				}
				_aBkClorSetColorSep[Index] = v;
			 //
			 // At last invalidate dialog client window
			 WM_InvalidateWindow(WM_GetClientWindow(hDlg));
	}
}
static OnPaint(void)
{
    GUI_SetColor(_aColorSep[0] | (((U32)_aColorSep[1]) << 8) | (((U32)_aColorSep[2]) << 16));
	  GUI_FillCircle(155,195,20);
}
static OnBkClorPaint(void)
{
    GUI_SetColor(_aBkClorSetColorSep[0] | (((U32)_aBkClorSetColorSep[1]) << 8) | (((U32)_aBkClorSetColorSep[2]) << 16));
	  GUI_FillRect(165,20,190,80);
}
/*********************************************************************
*
*       _cbDialog
*/
static void cbWindow(WM_MESSAGE * pMsg){
  switch (pMsg->MsgId) {
  case WM_PAINT:
    GUI_SetBkColor(GUI_BLUE);
    GUI_Clear();
    GUI_SetColor(GUI_WHITE);
    GUI_SetFont(&GUI_FontHZ24);
    GUI_DispStringHCenterAt("ϵͳ����", 160, 5);
    //GUI_DrawBitmap(&bmSeggerLogoBlue, xPos, yPos );
  default:
    WM_DefaultProc(pMsg);
  }   
}
static void LightCtrlCallBack(WM_MESSAGE * pMsg) {
  WM_HWIN hWin;
  WM_HWIN hSlider;
  int     NCode;
  int     Id;
  hWin=pMsg->hWin;
  switch (pMsg->MsgId) {
  case WM_PAINT:
	    OnPaint();       //ָ�����������ɫ
	    return;
  case WM_INIT_DIALOG:{
      HomeCtrlInit(pMsg);   //��ʼ���Ի���
      break;
	  }
  case WM_NOTIFY_PARENT:
    Id= WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(NCode) {
      case WM_NOTIFICATION_RELEASED:
		  {
					if(Id==ID_BUTTON_OK){
						DeletTask=1;
						//TIM2->CCR1=SLIDER_GetValue(WM_GetDialogItem(hWin,ID_SLIDER_0));    //�Եõ��Ļ���ֵ���и�ֵ���ȽϼĴ���
						//TIM2->CCR2=SLIDER_GetValue(WM_GetDialogItem(hWin,ID_SLIDER_1)); 
						//TIM2->CCR3=SLIDER_GetValue(WM_GetDialogItem(hWin,ID_SLIDER_2));  
						GUI_EndDialog(hWin, 0);      //����					
					}
					else if(Id==ID_BUTTON_CANCEL){
						DeletTask=1;	
						GUI_EndDialog(hWin, 1);	   //�˳�			 						
					}
					if(Id==ID_BUTTON_6){
						//GUI_MessageBox("Dianshiji Opened!","State",GUI_MESSAGEBOX_CF_MOVEABLE);
					 //JDQ1=!JDQ1;
						UsartSendText(1,"No No");
					}
					else if(Id==ID_BUTTON_5){
						//GUI_MessageBox("Deng Opened!","State",GUI_MESSAGEBOX_CF_MOVEABLE);
					  JDQ5=!JDQ5;
					  UsartSendText(1,"JDQ2 Open");
					}
					else if(Id==ID_BUTTON_4){
						//GUI_MessageBox("Woshi Opened!","State",GUI_MESSAGEBOX_CF_MOVEABLE);
					  JDQ4=!JDQ4;
					  UsartSendText(1,"JDQ4 Open");
					}
					else if(Id==ID_BUTTON_3){
						//GUI_MessageBox("Ludeng Opened!","State",GUI_MESSAGEBOX_CF_MOVEABLE);
					  JDQ3=!JDQ3;
						UsartSendText(1,"JDQ3 Open");
					}
					else if(Id==ID_BUTTON_2){
						//GUI_MessageBox("Weishengjian Opened!","State",GUI_MESSAGEBOX_CF_MOVEABLE);
						JDQ2=!JDQ2;
						UsartSendText(1,"JDQ5 Open");
					}
					else if(Id==ID_BUTTON_1){
						//GUI_MessageBox("JDQ1 Open","State",GUI_MESSAGEBOX_CF_MOVEABLE);
						JDQ1=!JDQ1;
						UsartSendText(1,"JDQ1 Open");
					}
			 }break;
		}
	  case WM_NOTIFICATION_VALUE_CHANGED:
		{
			  if(Id==ID_SLIDER_0)
				{
						 OnValueChange(hWin,Id,1);       //��ֵ��ʾ	
						 hSlider=WM_GetDialogItem(hWin,ID_SLIDER_0);
						 TIM2->CCR1=SLIDER_GetValue(hSlider);
				}					
			  else if(Id==ID_SLIDER_1)
				{
						 OnValueChange(hWin,Id,1);
						 hSlider=WM_GetDialogItem(hWin,ID_SLIDER_1);
						 TIM2->CCR2=SLIDER_GetValue(hSlider);
				}
			  else if(Id==ID_SLIDER_2)
				{
						 OnValueChange(hWin,Id,1);
						 hSlider=WM_GetDialogItem(hWin,ID_SLIDER_2);
						 TIM2->CCR3=SLIDER_GetValue(hSlider);
				}
				else if(Id==ID_SLIDER_3)
			  {
				     hSlider=WM_GetDialogItem(hWin,ID_SLIDER_3);
				     TIM2->CCR4=SLIDER_GetValue(hSlider); 			 
				}
				else if(Id==ID_CHECKBOX_0){
						 //C1=CHECKBOX_GetState(WM_GetDialogItem(hWin,ID_CHECKBOX_0));	
						 C1=CHECKBOX_IsChecked(WM_GetDialogItem(hWin,ID_CHECKBOX_0));	//���ظ�ѡ��ĵ�ǰ״̬ ��ѡ�л�δѡ�У�				
				}
			  else if(Id==ID_CHECKBOX_1){
				      C2=CHECKBOX_GetState(WM_GetDialogItem(hWin,ID_CHECKBOX_1));					    
			   }
			  else if(Id==ID_CHECKBOX_2){
				     C3=CHECKBOX_GetState(WM_GetDialogItem(hWin,ID_CHECKBOX_2));					  
				}
				 else if(Id==ID_CHECKBOX_3){
					 C4=CHECKBOX_GetState(WM_GetDialogItem(hWin,ID_CHECKBOX_3));					 
				 }				
   }break;     
    default:
        WM_DefaultProc(pMsg);
        break;
  }
}
//����ɫ���ûص�����
static void BkClorSetCallBack(WM_MESSAGE * pMsg) {
	int Id,NCode;
	WM_HWIN  hItem;
	WM_HWIN hWin = pMsg->hWin;
	switch(pMsg -> MsgId) {
		case WM_PAINT:
	    OnBkClorPaint();       //ָ�����������ɫ
	    return;
		case WM_INIT_DIALOG:
			 BkFlag=1;
		   hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_BkClorApp);
			 BUTTON_SetFont(hItem,&GUI_FontHZ12);
			 BUTTON_SetTextColor(hItem, 0, GUI_RED);
		   //		
		   hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_BkClorCancel);
			 BUTTON_SetFont(hItem,&GUI_FontHZ12);
			 BUTTON_SetTextColor(hItem, 0, GUI_RED);
		   //
			 hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXTBkClor_RED);
			 TEXT_SetFont(hItem,&GUI_FontHZ16);
			 TEXT_SetTextColor(hItem, GUI_RED);//��ɫ��2����ɫ
		   //
		   hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXTBkClor_GREEN);
       TEXT_SetFont(hItem,&GUI_FontHZ16);
       TEXT_SetTextColor(hItem, GUI_GREEN);//��ɫ��2����ɫ
		   //
		   hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXTBkClor_BLUE);
       TEXT_SetFont(hItem,&GUI_FontHZ16);
       TEXT_SetTextColor(hItem, GUI_BLUE);//��ɫ��2����ɫ
		   //�������������ú�ɫ
		   hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDERBkClor_RED);
	     SLIDER_SetSkin(hItem, SLIDER_SKIN_FLEX);
       SLIDER_SetRange(hItem,0,254);
       SLIDER_SetNumTicks(hItem,10);
		   SLIDER_SetValue(hItem,(u8)(DesktopColor&0xff));
		   //
		   hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDERBkClor_GREEN);
	     SLIDER_SetSkin(hItem, SLIDER_SKIN_FLEX);
       SLIDER_SetRange(hItem,0,254);
       SLIDER_SetNumTicks(hItem,10);
			 SLIDER_SetValue(hItem,(u8)((DesktopColor>>8)&0xff));
			 //
			 hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDERBkClor_BLUE);
	     SLIDER_SetSkin(hItem, SLIDER_SKIN_FLEX);
       SLIDER_SetRange(hItem,0,254);
       SLIDER_SetNumTicks(hItem,10);
			 SLIDER_SetValue(hItem,(u8)((DesktopColor>>16)&0xff));
			 break;
		case WM_NOTIFY_PARENT:
				 Id = WM_GetId(pMsg ->hWinSrc);
		     NCode = pMsg ->Data.v;
		switch(NCode){
			case WM_NOTIFICATION_RELEASED:{
						if(Id==ID_BUTTON_BkClorApp)
						{
							BkFlag=0;
              DesktopColor=_aBkClorSetColorSep[0] | (((U32)_aBkClorSetColorSep[1]) << 8) | (((U32)_aBkClorSetColorSep[2]) << 16); //д�뱳��ɫ���ݣ�DeskTopColor��ֵ			
              WM_SetDesktopColor(DesktopColor);							
						  GUI_EndDialog(pMsg->hWin, 1);
						}
						else if(Id==ID_BUTTON_BkClorCancel)
						{
							  BkFlag=0;
								GUI_EndDialog(pMsg->hWin, 1);                        //�����Ի���
						}
		      }break;
			case WM_NOTIFICATION_VALUE_CHANGED:{
						 if(Id==ID_SLIDERBkClor_RED){
								OnValueChange(hWin,Id,0);
						 }
						 else if(Id==ID_SLIDERBkClor_GREEN){
								OnValueChange(hWin,Id,0);						 
						 }
						 else if(Id==ID_SLIDERBkClor_BLUE){
								OnValueChange(hWin,Id,0);		
						 }							 
					}break;				
		}break;		
    default:
       WM_DefaultProc(pMsg);
       break;
	}
}
//ʱ�����ûص�����
static void TimeSetCallBack(WM_MESSAGE * pMsg) {
  int NCode;
  int Id;
	WM_HWIN  hItem;
	char time_chars_Hour[3]={0,0,0};
	char time_chars_Min[3]={0,0,0};
	char time_chars_Day[3]={0,0,0};
	WM_HWIN hWin = pMsg->hWin;
  switch (pMsg->MsgId) {
	   case WM_INIT_DIALOG:
			 TimFlag=1;
		   //
		   hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTONTimeSetOK);
			 BUTTON_SetFont(hItem,&GUI_FontHZ12);
			 BUTTON_SetTextColor(hItem, 0, GUI_RED);
		   //		
		   hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTONTimeSetCancel);
			 BUTTON_SetFont(hItem,&GUI_FontHZ12);
			 BUTTON_SetTextColor(hItem, 0, GUI_RED);
		   //
			 RTC_Get();
		   SysTimeHour=calendar.hour;
		   SysTimeMinute=calendar.min;		   
		   time_chars_Hour[0]=SysTimeHour/10+0x30;
		   time_chars_Hour[1]=SysTimeHour%10+0x30;
		   time_chars_Min[0]=SysTimeMinute/10+0x30;
		   time_chars_Min[1]=SysTimeMinute%10+0x30;
		   time_chars_Day[0]=calendar.w_date/10+0x30;
		   time_chars_Day[1]=calendar.w_date%10+0x30;
			 hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXTHour);
       TEXT_SetFont(hItem,&GUI_FontHZ16);
       TEXT_SetTextColor(hItem, GUI_BLUE);
		   hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXTMin);
       TEXT_SetFont(hItem,&GUI_FontHZ16);
		   TEXT_SetTextColor(hItem, GUI_BLUE);
		   hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXTDate);
       TEXT_SetFont(hItem,&GUI_FontHZ16);
       TEXT_SetTextColor(hItem, GUI_BLUE);
		   TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXTHour),time_chars_Hour);
		   TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXTMin),time_chars_Min);
		   TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXTDate),time_chars_Day);
       
		    break;
		 case WM_NOTIFY_PARENT:
			   Id = WM_GetId(pMsg ->hWinSrc);
		     NCode = pMsg ->Data.v;
		     switch(NCode) {
		         case WM_NOTIFICATION_RELEASED:
				 				 if(Id==ID_BUTTONHourSetPlus)
								 {
										 SysTimeHour++;
										 if(SysTimeHour==24)
											 SysTimeHour=0;
										 time_chars_Hour[0]=SysTimeHour/10+0x30;
		                 time_chars_Hour[1]=SysTimeHour%10+0x30;
										 TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXTHour),time_chars_Hour);
								 }
								 else if(Id==ID_BUTTONHourSetMinus)
								 {
										SysTimeHour--;
										if(SysTimeHour==0)
											SysTimeHour=23;
										time_chars_Hour[0]=SysTimeHour/10+0x30;
		                time_chars_Hour[1]=SysTimeHour%10+0x30;
										TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXTHour),time_chars_Hour);
								 }
								 else if(Id==ID_BUTTONMinSetPlus)
								 {
										SysTimeMinute++;
										if(SysTimeMinute==60)
											SysTimeMinute=0;
										time_chars_Min[0]=SysTimeMinute/10+0x30;
		                time_chars_Min[1]=SysTimeMinute%10+0x30;
										TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXTMin),time_chars_Min);
								 }
								 else if(Id==ID_BUTTONMinSetMinus)
								 {
										SysTimeMinute--;
										if(SysTimeMinute==0)
											SysTimeMinute=59;
										time_chars_Min[0]=SysTimeMinute/10+0x30;
										time_chars_Min[1]=SysTimeMinute%10+0x30;
										TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXTMin),time_chars_Min);
								 }
								 else if(Id==ID_BUTTONWeekSetPlus)
								 {
									 SysTimeDay++;
									 if(SysTimeDay==32)
									    SysTimeDay=1;
									 time_chars_Day[0]=SysTimeDay/10+0x30;
									 time_chars_Day[1]=SysTimeDay%10+0x30;
									 TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXTDate),time_chars_Day);
									 
								 }
								 else if(Id==ID_BUTTONWeekSetMinus)
								 {
									 SysTimeDay--;
									 if(SysTimeDay==0)
									    SysTimeDay=31;					 
									 time_chars_Day[0]=SysTimeDay/10+0x30;
									 time_chars_Day[1]=SysTimeDay%10+0x30;
									 TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXTDate),time_chars_Day);
									 
								 }
								 else if(Id==ID_BUTTONTimeSetOK)
								 {
									  TimFlag=0;
										RTC_Set(calendar.w_year,calendar.w_month,SysTimeDay,SysTimeHour,SysTimeMinute,0);//д��ʱ��
									  //calendar.week=SysTimeDay;
										GUI_EndDialog(pMsg->hWin, 0);      //�رնԻ���,������ʱ��
								 }
								 else if(Id==ID_BUTTONTimeSetCancel)
								 {
									  TimFlag=0;
										GUI_EndDialog(pMsg->hWin, 1);      //�رնԻ�������ʱ��
								 }
						     break;	
						 default :break;								 
				}								 
		    default:
           WM_DefaultProc(pMsg);
           break;
	}
}
//��ֵ���ûص�����
static void ThroeSetCallBack(WM_MESSAGE * pMsg) {
  int NCode;
  int Id;
	WM_HWIN  hItem;
	char throe_chars_Fire[3]={0,0,0};
	char throe_chars_Tempra[3]={0,0,0};
	WM_HWIN hWin = pMsg->hWin;
  switch (pMsg->MsgId) {
	   case WM_INIT_DIALOG:
			 ThrFlag=1;
		   hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTONThroeSetOK);
			 BUTTON_SetFont(hItem,&GUI_FontHZ12);
			 BUTTON_SetTextColor(hItem, 0, GUI_RED);
		   //		
		   hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTONThroeSetCancel);
			 BUTTON_SetFont(hItem,&GUI_FontHZ12);
			 BUTTON_SetTextColor(hItem, 0, GUI_RED);
		   //		
		   hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXTNameTmprea);
			 TEXT_SetFont(hItem,&GUI_FontHZ12);
			 TEXT_SetTextColor(hItem,GUI_BLUE);
		   //		
		   hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXTNameFire);
			 TEXT_SetFont(hItem,&GUI_FontHZ12);
			 TEXT_SetTextColor(hItem,GUI_BLUE);
		   //
			 Fire=ThroeFire;//�����������¿�ʼ�Ӽ�
		   Temp=ThroeTempra;
		   throe_chars_Fire[0]=ThroeFire/10+0x30;
		   throe_chars_Fire[1]=ThroeFire%10+0x30;               //�������ӵ�ʮλ�͸�λ����������Ҵ�������
		   throe_chars_Tempra[0]=ThroeTempra/10+0x30;
		   throe_chars_Tempra[1]=ThroeTempra%10+0x30;
			 hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXTFire);
       TEXT_SetFont(hItem,&GUI_FontHZ16);
       TEXT_SetTextColor(hItem, GUI_BLUE);
		   hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXTTempra);
       TEXT_SetFont(hItem,&GUI_FontHZ16);
       TEXT_SetTextColor(hItem, GUI_BLUE);
		   TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXTFire),throe_chars_Fire);//�õ���ǰ����������ֵ��ֵ����ʾ
		   TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXTTempra),throe_chars_Tempra);
		    break;
		 case WM_NOTIFY_PARENT:
			   Id = WM_GetId(pMsg ->hWinSrc);
		     NCode = pMsg ->Data.v;
		     switch(NCode) {
		         case WM_NOTIFICATION_RELEASED:
				 				 if(Id==ID_BUTTONFireSetPlus)
								 {
										 Fire++;
										 if(Fire==100)
											 Fire=1;
										 throe_chars_Fire[0]=Fire/10+0x30;
		                 throe_chars_Fire[1]=Fire%10+0x30;
										 TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXTFire),throe_chars_Fire);
								 }
								 else if(Id==ID_BUTTONFireSetMinus)
								 {
										Fire--;
										if(Fire>100)
											Fire=100;										
										throe_chars_Fire[0]=Fire/10+0x30;
		                throe_chars_Fire[1]=Fire%10+0x30;
										TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXTFire),throe_chars_Fire);
								 }
								 else if(Id==ID_BUTTONTempraSetPlus)
								 {
										Temp++;
										if(Temp==100)
											Temp=1;
										throe_chars_Tempra[0]=Temp/10+0x30;
		                throe_chars_Tempra[1]=Temp%10+0x30;
										TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXTTempra),throe_chars_Tempra);
								 }
								 else if(Id==ID_BUTTONTempraSetMinus)
								 {
										Temp--;
										if(Temp>100)
											Temp=100;
										throe_chars_Tempra[0]=Temp/10+0x30;
										throe_chars_Tempra[1]=Temp%10+0x30;
										TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXTTempra),throe_chars_Tempra);
								 }
								 else if(Id==ID_BUTTONThroeSetOK)
								 {
									  ThrFlag=0;
									  ThroeTempra=Temp;
									  ThroeFire=Fire;
										GUI_EndDialog(pMsg->hWin, 0);      //�رնԻ���,������ʱ��
								 }
								 else if(Id==ID_BUTTONThroeSetCancel)
								 {
									  ThrFlag=0;
										GUI_EndDialog(pMsg->hWin, 1);      //�رնԻ�������ʱ��
								 }
						     break;
						 default :break;
				}
		    default:
           WM_DefaultProc(pMsg);
           break;
	}
}

//�̵�����ʱ���ƻص�����
static void JDQTimerCallBack(WM_MESSAGE * pMsg) {
	int NCode;
  int Id;
	WM_HWIN  hItem;
	u8 i,j;
	char Buf[2];
	char JDQ_char[1]={0};       //�̵�����Ŀ
	char JDQ_char_hour[2]={0,0};  //�̵���ʱ����
	char JDQ_char_min[2]={0,0};   //�̵�����������
	WM_HWIN hWin = pMsg->hWin;
  switch (pMsg->MsgId) {
	   case WM_INIT_DIALOG:
			 JDQFlag=1;
			 hItem = WM_GetDialogItem(pMsg->hWin,ID_TEXTJDQOrder);
		   TEXT_SetFont(hItem,&GUI_FontHZ24); 
		   TEXT_SetTextColor(hItem, GUI_BLUE);
		   Buf[0]=JDQOrder%10+0x30;
		   TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXTJDQOrder),Buf);
		   Buf[0]=JDQHour/10+0x30;
		   Buf[1]=JDQHour%10+0x30;
		   TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXTJDQHour),Buf);
		   Buf[0]=JDQMin/10+0x30;
		   Buf[1]=JDQMin%10+0x30;
		   TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXTJDQMin),Buf);
		   //
		   hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTONJDQChage);
			 BUTTON_SetFont(hItem,&GUI_FontHZ12);
			 BUTTON_SetTextColor(hItem, 0, GUI_RED);
		   //		
		   hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTONJDQClear);
			 BUTTON_SetFont(hItem,&GUI_FontHZ12);
			 BUTTON_SetTextColor(hItem, 0, GUI_RED);
		   //
		   hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTONJDQuit);
			 BUTTON_SetFont(hItem,&GUI_FontHZ12);
			 BUTTON_SetTextColor(hItem, 0, GUI_RED);
	     break;
	   case WM_NOTIFY_PARENT:
			   Id = WM_GetId(pMsg ->hWinSrc);
		     NCode = pMsg ->Data.v;
		     switch(NCode) {
		         case WM_NOTIFICATION_RELEASED: 
							 if(Id==ID_BUTTONJDQChage)
							 {
							     JDQOrder++;
								   if(JDQOrder==6)
									   JDQOrder=1;
									  JDQ_char[0]=JDQOrder%10+0x30;
										TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXTJDQOrder),JDQ_char);
							 }
	             else if(Id==ID_BUTTONJDQHPlus)
							 {
							      JDQHour++;
								    if(JDQHour>23)
											JDQHour=0;
										JDQ_char_hour[0]=JDQHour/10+0x30;
										JDQ_char_hour[1]=JDQHour%10+0x30;
										TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXTJDQHour),JDQ_char_hour);
							 }
							 else if(Id==ID_BUTTONJDQHMinus)
							 {
							      JDQHour--;
								    if(JDQHour>23)
											JDQHour=23;
										JDQ_char_hour[0]=JDQHour/10+0x30;
										JDQ_char_hour[1]=JDQHour%10+0x30;
										TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXTJDQHour),JDQ_char_hour);
							 }
							 else if(Id==ID_BUTTONJDQSPlus)
							 {
							      JDQMin++;
								    if(JDQMin>59)
									    JDQMin=0;										
										JDQ_char_min[0]=JDQMin/10+0x30;
										JDQ_char_min[1]=JDQMin%10+0x30;
										TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXTJDQMin),JDQ_char_min);
							 }
							 else if(Id==ID_BUTTONJDQSMinus)
							 {
							      JDQMin--;
								    if(JDQMin>59)
									    JDQMin=59;										
										JDQ_char_min[0]=JDQMin/10+0x30;
										JDQ_char_min[1]=JDQMin%10+0x30;
										TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXTJDQMin),JDQ_char_min);
							 }
							 else if(Id==ID_BUTTONJDQClear)
							 {
								      JDQOrder=0;										 
                      JDQHour=0;								 
								      JDQMin=0;
								      for(i=0;i<5;i++)
								      {
												 JDQNum[i]='_';
								         for(j=0;j<3;j++)
								            JDQTimer[i][j]=0;    //����������
								      }							           
							 }
							 else if(Id==ID_BUTTONJDQuit)
							 {
								    JDQFlag=0;
										JDQTimer[JDQOrder-1][0]=JDQOrder;
										JDQTimer[JDQOrder-1][1]=JDQHour;
										JDQTimer[JDQOrder-1][2]=JDQMin;	
								    JDQNum[JDQOrder-1]=JDQOrder%10+0x30;
								    /*if(JDQOrder==0)
											Order=0;
										else 
										{
								        Order++;
								        if(Order==5)
									         Order=0;
									  }*/
                    if(JDQOrder)
										  LED0=1;		
                    else				
                      LED0=0;
							      GUI_EndDialog(pMsg->hWin,1);
							 }
	             break;
						 default :break;
					}
				 default:
           WM_DefaultProc(pMsg);
           break;
	}
}
//���ܣ�Ԥ����⣬��ʱ����
void AlarmDete(void)    //�������
{
	  u8 i,j;
		for(i=0;i<5;i++)
		{
				if(C4&&JDQTimer[i][0]&&JDQTimer[i][1]==calendar.hour&&JDQTimer[i][2]==calendar.min&&calendar.sec<1)
				{
					  for(j=0;j<5;j++)
					  {
								switch(JDQTimer[j][0])
								{
										case 1:
										{											
											if(JDQTimer[j][1]==calendar.hour&&JDQTimer[j][2]==calendar.min)
											{
													LED0=0;
													if(GPIOC->ODR&GPIO_Pin_0)
														 JDQ1=0;
													else 
														 JDQ1=1;
													JDQTimer[0][0]=0;
													JDQNum[0]='_';
											}
											delay_ms(1000);
										}break;	
										case 2: 
										{
											if(JDQTimer[j][1]==calendar.hour&&JDQTimer[j][2]==calendar.min)
											{
													LED0=0;
													if(GPIOC->ODR&GPIO_Pin_1)
														 JDQ2=0;
													else 
														 JDQ2=1;
													JDQTimer[1][0]=0;
													JDQNum[1]='_';
											}
											delay_ms(1000);
										}break;	
										case 3: 
										{
											if(JDQTimer[j][1]==calendar.hour&&JDQTimer[j][2]==calendar.min)
											{
													LED0=0;
													if(GPIOC->ODR&GPIO_Pin_2)
														 JDQ3=0;
													else 
														 JDQ3=1;
													JDQTimer[2][0]=0;
													JDQNum[2]='_';
											}
											delay_ms(1000);
										}break;
										case 4: 
										{
											if(JDQTimer[j][1]==calendar.hour&&JDQTimer[j][2]==calendar.min)
											{
													LED0=0;
													if(GPIOC->ODR&GPIO_Pin_3)
														 JDQ4=0;
													else 
														 JDQ4=1;
													JDQTimer[3][0]=0;
													JDQNum[3]='_';
											}
											delay_ms(1000);
										}break;
										case 5: 
										{
											if(JDQTimer[j][1]==calendar.hour&&JDQTimer[j][2]==calendar.min)
											{
													LED0=0;
													if(GPIOC->ODR&GPIO_Pin_4)
														 JDQ5=0;
													else
														 JDQ5=1;
													JDQTimer[4][0]=0;
													JDQNum[4]='_';
											}
											delay_ms(1000);
										}break;										
										default:break;
								}
					  }
				}
		}
		NRF24L01_RxPacket(User_data);
		//if(MsgFlag||MsageFlag)          //����ⲿ�������ú��¶ȶ������趨ֵ��������GSMԤ������
		if(MsageFlag)
		{
			 MsageFlag=0;
		   //MsgFlag=0;
			 //SendMessage();
		}
    if(C1&&(User_data[2]>ThroeFire))
		{
			  //��������
			  C1=0;
			  GPIO_SetBits(GPIOA,GPIO_Pin_4);
		    //��λGSM��绰�����Ź���
			  MsageFlag=1;
		}
		else if(C2&&(User_data[0]>ThroeTempra))
		{
			  C2=0;
		    GPIO_SetBits(GPIOA,GPIO_Pin_4);
			  //��λGSM��绰�����Ź���
			  MsageFlag=1;
		}
		else
			GPIO_ResetBits(GPIOA,GPIO_Pin_4);   //�رշ�����	
}
void LightCtrl_Task(void *pdata)   //�ƹ��������
{
	 WM_HWIN hWIN;   //�����ڴ������
   #if GUI_SUPPORT_MEMDEV             //���еĴ����Զ���ʹ���ڴ��豸,��Ȼͼ���������������
       WM_SetCreateFlags(WM_CF_MEMDEV);
   #endif
   hWIN = GUI_CreateDialogBox(LightCtrlDialog, GUI_COUNTOF(LightCtrlDialog), LightCtrlCallBack, WM_HBKWIN, 0, 0);   //�õ������ھ��
   while(1)
   { 
		   GUI_Exec();
		   if(DeletTask)
			 {
				  DeletTask=0;	
			    Go_MainMenu();
				  OSTaskDel(OS_PRIO_SELF);    //ɾ���Լ�
			 }
		 }
}
void SysSet_Task(void *pdata)
{
  u8 i;
	WM_HWIN WM_Handle;            //�ӶԻ���ص����
	PROGBAR_Handle hProgBar[2];   //���������
	BUTTON_Handle hButton[5];     //��ť�������ʱ�䣬���ñ���ɫ
	TEXT_Handle hTextelse[4];     //����������ı���ʾ���
	SingleList   pos   = NULL;
	OtherMenu_pHead = SingleListNodeCreate();//��������
	pos = OtherMenu_pHead;
	hTextelse[0] = TEXT_Create(70,5,300, 30, 1, WM_CF_SHOW, "ϵͳ����",TEXT_CF_LEFT);
	TEXT_SetFont(hTextelse[0],&GUI_FontHZ24);
	hTextelse[1] = TEXT_Create(10,60,200, 20, 2, WM_CF_SHOW, "����",TEXT_CF_LEFT);               //����
	TEXT_SetFont(hTextelse[1],&GUI_FontHZ16);
	hTextelse[2] = TEXT_Create(10,100,200, 20, 3, WM_CF_SHOW, "ˮ��ͷ",TEXT_CF_LEFT);            //ˮ��ͷ
	TEXT_SetFont(hTextelse[2],&GUI_FontHZ16);
	hTextelse[3] = TEXT_Create(10,120,100, 30, 4, WM_CF_SHOW, "_____",TEXT_CF_LEFT);             //�̵�������Ŀ
	TEXT_SetFont(hTextelse[3],&GUI_FontHZ24);
	for(i=0;i<4;i++)
	{
		  TEXT_SetTextColor(hTextelse[i],GUI_RED);//�ı���ɫ����
	    pos = SingleListInsert(OtherMenu_pHead,pos,hTextelse[i]);       //���ı������������
	}
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);                            //���ð�ťƤ��
	hButton[0]=BUTTON_Create(250, 200 ,70,25, GUI_ID_BUTTON3, WM_CF_SHOW);
	hButton[1]=BUTTON_Create(160, 200 ,70,25, GUI_ID_BUTTON4, WM_CF_SHOW);
	hButton[2]=BUTTON_Create(86, 200 ,70,25, GUI_ID_BUTTON5, WM_CF_SHOW);
	hButton[3]=BUTTON_Create(10, 200 ,70,25, GUI_ID_BUTTON6, WM_CF_SHOW);
	hButton[4]=BUTTON_Create(10, 150 ,70,25, GUI_ID_BUTTON7, WM_CF_SHOW);
	for(i=0;i<5;i++)
	{
	    pos = SingleListInsert(OtherMenu_pHead,pos,hButton[i]);         //����ť�����������
		  BUTTON_SetFont(hButton[i],&GUI_FontHZ12);
		
	}
	
	BUTTON_SetText(hButton[0]," �˳�");                       //�˳�����
	BUTTON_SetText(hButton[1]," ��������");                      //ʱ������
	BUTTON_SetText(hButton[2]," ʱ������");                    //����ɫ����
	BUTTON_SetText(hButton[3]," ��ֵ����");                    //��ֵ����
	BUTTON_SetText(hButton[4]," �̵�����ʱ");
	hProgBar[0]=PROGBAR_Create(0,40,315,20,WM_CF_SHOW);                //��������������������ʾ
	hProgBar[1]=PROGBAR_Create(0,80,315,20,WM_CF_SHOW);
	for(i=0;i<2;i++)
	{
	    pos = SingleListInsert(OtherMenu_pHead,pos,hProgBar[i]);        //����ť�����������
	}
	PROGBAR_SetBarColor(hProgBar[0],0,GUI_GREEN);                       //��������ɽ�����ɫ
	PROGBAR_SetBarColor(hProgBar[0],1,GUI_LIGHTRED);                    //����δ��ɽ�����ɫ
	PROGBAR_SetBarColor(hProgBar[1],0,GUI_GREEN);
	PROGBAR_SetBarColor(hProgBar[1],1,GUI_LIGHTRED);
	TEXT_SetFont(hTextelse[3],&GUI_FontHZ24);
	while(1)
	{
		  NRF24L01_RxPacket(User_data);                   //���߽�������
			PROGBAR_SetValue(hProgBar[0],User_data[1]);     //��ʾ��������
			PROGBAR_SetValue(hProgBar[1],User_data[2]);	
		  if(C4&&JDQOrder==0)
			{
			   TEXT_SetText(hTextelse[3],"_____");
			}
		  /*else if(C4)     //�ж��Ƿ������˼̵������ҿ����Ƿ�������
			{
				 for(i=0;i<5;i++)
				 {
				    if(JDQTimer[i][0])
						   JDQNum[i]=(i+1)%10+0x30;
						else
							 JDQNum[i]='_';
				 }
	       TEXT_SetText(hTextelse[3],JDQNum);
			}*/
			else 
				TEXT_SetText(hTextelse[3],JDQNum);
			GUI_Exec();
			switch(GUI_GetKey())
			{
				case GUI_ID_BUTTON3:
					if(ThrFlag==0&&TimFlag==0&&BkFlag==0&&JDQFlag==0)                //Ϊ1���ӶԻ���Ϊ�˳���Ϊ0��˵�����˳�
					{
						 Out_OtherMenu();
						 Go_MainMenu();  					//����������
						 OSTaskDel(OS_PRIO_SELF);
					}
					else{;}
						 break;
				case GUI_ID_BUTTON4:
				{
				     WM_Handle = GUI_CreateDialogBox(BkClorSetDialog, GUI_COUNTOF(BkClorSetDialog), BkClorSetCallBack, 0, 0, 0);//����ɫ���ûص�����
				}break;				
			  case GUI_ID_BUTTON5:
				{
				     WM_Handle = GUI_CreateDialogBox(TimeSetDialog, GUI_COUNTOF(TimeSetDialog), TimeSetCallBack, 0, 0, 0);      //ϵͳʱ�����ûص�����
				}break;
				case GUI_ID_BUTTON6:
				{
				     WM_Handle = GUI_CreateDialogBox(ThroeSetDialog, GUI_COUNTOF(ThroeSetDialog), ThroeSetCallBack, 0, 0, 0);    //ϵͳ������ֵ���ûص�����
				}break;			
				case GUI_ID_BUTTON7:
				{
				     WM_Handle = GUI_CreateDialogBox(JDQTimerDialog, GUI_COUNTOF(JDQTimerDialog), JDQTimerCallBack, 0, 0, 0);    //ϵͳ������ֵ���ûص�����
				}break;	
				default :break;
			}	
	}
}
/*************************** End of file ****************************/
#endif


