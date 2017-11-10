#ifndef _MY_GUI_H
#define _MY_GUI_H
#include "GUI.h"
#include "include.h"
#include "DIALOG.h"
#define TouchUnPress    0        //首次检测到没有按下
#define TouchUnPressed  1        //没有按下
#define TouchPress      2        //首次检测到按下
#define TouchPressed    3        //按下
#define MainMenu_ID     1
#define Monitor_ID      3 
#define LightCtrl_ID    5
#define DutyList_ID     7;
typedef struct SingleListNode {
	WM_HMEM      handle;
	struct SingleListNode   *next;

}SingleListNode;                                   /* 声明SingleListNode为新的类型名*/
typedef SingleListNode    *SingleList;             /* 定义指针类型          */
typedef struct {
	WM_HMEM       FirstHandle;
	WM_HMEM       EndHandle;
}MenuInfo;    
extern GUI_COLOR  DesktopColor;
extern SingleList  OtherMenu_pHead;

extern u8 Menu_ID;                                 //菜单界面的ID
extern u8 TouchCmd;                                //标志着是点触还是滑动操作。
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



