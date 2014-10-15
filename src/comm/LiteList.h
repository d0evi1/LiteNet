/*--------------------------------------------------------
 * 参考自linux/list.h设计
 *
 * @brief	请使用外部函数或宏(大写字母开头)， 
 *			"__"开头的函数限该头文件内部使用
 * @author:	jolly wang
 * @date:	2012.9.10
 *--------------------------------------------------------*/

#ifndef _LITE_LIST_H_
#define _LITE_LIST_H_

#include <stdlib.h>

#define LIST_POISON1  ((void *) 0x00100101)
#define LIST_POISON2  ((void *) 0x00200201)

/*
 * 链头
 */
typedef struct list_head {
	struct list_head*	next;
	struct list_head*	prev;
} ListHead;

/*
 * 初始化链头
 */
#define LIST_HEAD_INIT(name) { &(name), &(name) }

/*
 * 初始化链头
 */
#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

/*
 * 内部函数
 */
static inline void __list_add(ListHead* cur, ListHead* prev, ListHead *next)
{
	next->prev = cur;
	cur->next = next;
	cur->prev = prev;
	prev->next = cur;
}

/*
 * 内部函数
 */
static inline void __list_del(ListHead * prev, ListHead * next)
{
	next->prev = prev;
	prev->next = next;
}

/*
 * 内部函数
 */
static inline void __list_splice(ListHead* list, ListHead* head)
{
	ListHead *first = list->next;
	ListHead *last = list->prev;
	ListHead *at = head->next;

	first->prev = head;
	head->next = first;

	last->next = at;
	at->prev = last;
}

/*
 * 外部函数
 * 添加到表头
 */
static inline void ListAdd(ListHead* cur, ListHead *head)
{
	__list_add(cur, head, head->next);
}

/*
 * 外部函数
 * 添加到链表尾
 */
static inline void ListAddTail(ListHead* cur, ListHead *head)
{
	__list_add(cur, head->prev, head);
}

/*
 * 外部函数
 * 删除一个节点
 */
static inline void ListDel(ListHead *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = NULL; // (struct list_head*)LIST_POISON1;
	entry->prev = NULL; // (struct list_head*)LIST_POISON2;
}

/*
 * 外部函数
 * 删除一个节点，并重新初始化
 */
static inline void ListDelInit(ListHead *entry)
{
	__list_del(entry->prev, entry->next);
	INIT_LIST_HEAD(entry);
}		

/*
 * 外部函数
 * 删除一个链表，添加另一个作为表头
 */
static inline void ListMove(ListHead *list, ListHead *head)
{
	__list_del(list->prev, list->next);
	ListAdd(list, head);
}

/*
 * 外部函数
 * 删除一个链表，添加另一个作为表头
 */
static inline void ListMoveTail(ListHead *list, ListHead *head)
{
	__list_del(list->prev, list->next);
	ListAddTail(list, head);
}

/*
 * 外部函数
 * 判断链表是否为空
 */
static inline int ListIsEmpty(const ListHead *head)
{
	return head->next == head;
}

/*
 * 外部函数
 * 连接两个链表
 */
static inline void ListSplice(ListHead *list, ListHead *head)
{
	if (!ListIsEmpty(list))
	{
		__list_splice(list, head);
	}
}

/*
 * 外部函数
 * 连接两个链表, 并重新初始化
 */
static inline void ListSpliceInit(ListHead *list, ListHead *head)
{
	if (!ListIsEmpty(list)) 
	{
		__list_splice(list, head);
		INIT_LIST_HEAD(list);
	}
}

/*
 * 外部函数
 * 获取链节点中初始地址，注意括号: 
 * 计算方式: src_address = (head - offset)
 */
#define LIST_ENTRY(ptr, type, member) \
	((type*)((char*)(ptr)-(uint32_t)(&((type*)0)->member)))


/*
 * 外部宏
 * 前向遍历
 */
#define LIST_FOR_EACH(pos, head) \
			for (pos = (head)->next; pos != (head); pos = pos->next)

/*
 * 外部宏
 * 安全遍历
 */
#define LIST_FOR_EACH_SAFE(pos, n, head) \
			for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

/*
 * 外部宏
 * 逆向遍历
 */
#define LIST_FOR_EACH_PREV(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)


//end
#endif