//链表数据结构

#include "sys.h"
//链表结构申明
typedef struct SingleListNode
{
   uint16_t num;    //链表中的数据
   struct SingleListNode *next;  //指向链表的结点
}SingleListNode;
typedef SingleListNode  *SingleList;   //用指针定义链表类型
