//������
#include "include.h"
#include "SingleList.h"
SingleList  MainMenu_pHead;  /* ������Ŀؼ�����ͷָ�� */
SingleList  OtherMenu_pHead; /* ��������Ŀؼ������ͷָ��*/ 



/*********************************** 
** ����:      SingleListNodeCreate
** ����:       �������������ͷ�ڵ�
** �������:   ��
** �������:   ��
** ����ֵ:     ����ͷָ��
*************************************/
SingleList SingleListNodeCreate (void)
{
	SingleList pHead = (SingleList)malloc(sizeof(SingleListNode));
	pHead->next=NULL;
	return pHead;        //���������ͷ
}
/********************************************************************************************************* 
** ����:       SingleListInsert
** ����:       ���������������
** �������:   SingleList pList: Ҫ���������
               SingleList pos: Ҫ����Ľڵ�
               uint16_t x: Ҫ�������ֵ
** �������:   ��
** ����ֵ:     ����ڵ��ָ��
**********************************************************************************************************/
SingleList SingleListInsert (SingleList pList,SingleList pos, uint16_t x)
{
	SingleList ptr = (SingleList)malloc(sizeof(SingleListNode));         //���ָ��
	ptr-> num    = x;      //Ҫ�������ֵ
	ptr->next      = pos->next;    //ָ����һ���ڵ��
	pos->next      = ptr;
	return ptr;
}







