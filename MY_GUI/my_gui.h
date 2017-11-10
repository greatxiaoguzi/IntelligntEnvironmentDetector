#ifndef _MY_GUI_H
#define _MY_GUI_H
#include "GUI.h"
#include "include.h"
#include "DIALOG.h"
#define TouchUnPress    0        //�״μ�⵽û�а���
#define TouchUnPressed  1        //û�а���
#define TouchPress      2        //�״μ�⵽����
#define TouchPressed    3        //����
#define MainMenu_ID     1
#define Monitor_ID      3 
#define LightCtrl_ID    5
#define DutyList_ID     7;
typedef struct SingleListNode {
	WM_HMEM      handle;
	struct SingleListNode   *next;

}SingleListNode;                                   /* ����SingleListNodeΪ�µ�������*/
typedef SingleListNode    *SingleList;             /* ����ָ������          */
typedef struct {
	WM_HMEM       FirstHandle;
	WM_HMEM       EndHandle;
}MenuInfo;    
extern GUI_COLOR  DesktopColor;
extern SingleList  OtherMenu_pHead;

extern u8 Menu_ID;                                 //�˵������ID
extern u8 TouchCmd;                                //��־���ǵ㴥���ǻ���������
void Monitor_Task(void *pdata);
void MainMenuInit(void);

SingleList SingleListNodeCreate ();
SingleList SingleListInsert (SingleList pList,SingleList pos, WM_HMEM x);
void SingleListFree (SingleList pHead);
void ClearStruct(unsigned char *pta, int size );
void WM_Move(SingleList pHead ,int dx,int dy);
void Out_OtherMenu(void);
void Out_MainMenu(void);
void Go_MainMenu(void);
//void BkBmpLoad(void);
void DispTime(void);
#endif



