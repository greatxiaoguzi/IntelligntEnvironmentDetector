//链表创建
#include "include.h"
#include "SingleList.h"
SingleList  MainMenu_pHead;  /* 主界面的控件链表头指针 */
SingleList  OtherMenu_pHead; /* 其他界面的控件链表表头指针*/ 



/*********************************** 
** 名称:      SingleListNodeCreate
** 描述:       创建单向链表表头节点
** 输入参数:   无
** 输出参数:   无
** 返回值:     链表头指针
*************************************/
SingleList SingleListNodeCreate (void)
{
	SingleList pHead = (SingleList)malloc(sizeof(SingleListNode));
	pHead->next=NULL;
	return pHead;        //返回链表表头
}
/********************************************************************************************************* 
** 名称:       SingleListInsert
** 描述:       单向链表插入运算
** 输入参数:   SingleList pList: 要插入的链表
               SingleList pos: 要插入的节点
               uint16_t x: 要插入的数值
** 输出参数:   无
** 返回值:     插入节点的指针
**********************************************************************************************************/
SingleList SingleListInsert (SingleList pList,SingleList pos, uint16_t x)
{
	SingleList ptr = (SingleList)malloc(sizeof(SingleListNode));         //结点指针
	ptr-> num    = x;      //要插入的数值
	ptr->next      = pos->next;    //指向下一个节点的
	pos->next      = ptr;
	return ptr;
}







