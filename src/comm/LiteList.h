/*--------------------------------------------------------
 * �ο���linux/list.h���
 *
 * @brief	��ʹ���ⲿ�������(��д��ĸ��ͷ)�� 
 *			"__"��ͷ�ĺ����޸�ͷ�ļ��ڲ�ʹ��
 * @author:	jolly wang
 * @date:	2012.9.10
 *--------------------------------------------------------*/

#ifndef _LITE_LIST_H_
#define _LITE_LIST_H_

#include <stdlib.h>

#define LIST_POISON1  ((void *) 0x00100101)
#define LIST_POISON2  ((void *) 0x00200201)

/*
 * ��ͷ
 */
typedef struct list_head {
	struct list_head*	next;
	struct list_head*	prev;
} ListHead;

/*
 * ��ʼ����ͷ
 */
#define LIST_HEAD_INIT(name) { &(name), &(name) }

/*
 * ��ʼ����ͷ
 */
#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

/*
 * �ڲ�����
 */
static inline void __list_add(ListHead* cur, ListHead* prev, ListHead *next)
{
	next->prev = cur;
	cur->next = next;
	cur->prev = prev;
	prev->next = cur;
}

/*
 * �ڲ�����
 */
static inline void __list_del(ListHead * prev, ListHead * next)
{
	next->prev = prev;
	prev->next = next;
}

/*
 * �ڲ�����
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
 * �ⲿ����
 * ��ӵ���ͷ
 */
static inline void ListAdd(ListHead* cur, ListHead *head)
{
	__list_add(cur, head, head->next);
}

/*
 * �ⲿ����
 * ��ӵ�����β
 */
static inline void ListAddTail(ListHead* cur, ListHead *head)
{
	__list_add(cur, head->prev, head);
}

/*
 * �ⲿ����
 * ɾ��һ���ڵ�
 */
static inline void ListDel(ListHead *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = NULL; // (struct list_head*)LIST_POISON1;
	entry->prev = NULL; // (struct list_head*)LIST_POISON2;
}

/*
 * �ⲿ����
 * ɾ��һ���ڵ㣬�����³�ʼ��
 */
static inline void ListDelInit(ListHead *entry)
{
	__list_del(entry->prev, entry->next);
	INIT_LIST_HEAD(entry);
}		

/*
 * �ⲿ����
 * ɾ��һ�����������һ����Ϊ��ͷ
 */
static inline void ListMove(ListHead *list, ListHead *head)
{
	__list_del(list->prev, list->next);
	ListAdd(list, head);
}

/*
 * �ⲿ����
 * ɾ��һ�����������һ����Ϊ��ͷ
 */
static inline void ListMoveTail(ListHead *list, ListHead *head)
{
	__list_del(list->prev, list->next);
	ListAddTail(list, head);
}

/*
 * �ⲿ����
 * �ж������Ƿ�Ϊ��
 */
static inline int ListIsEmpty(const ListHead *head)
{
	return head->next == head;
}

/*
 * �ⲿ����
 * ������������
 */
static inline void ListSplice(ListHead *list, ListHead *head)
{
	if (!ListIsEmpty(list))
	{
		__list_splice(list, head);
	}
}

/*
 * �ⲿ����
 * ������������, �����³�ʼ��
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
 * �ⲿ����
 * ��ȡ���ڵ��г�ʼ��ַ��ע������: 
 * ���㷽ʽ: src_address = (head - offset)
 */
#define LIST_ENTRY(ptr, type, member) \
	((type*)((char*)(ptr)-(uint32_t)(&((type*)0)->member)))


/*
 * �ⲿ��
 * ǰ�����
 */
#define LIST_FOR_EACH(pos, head) \
			for (pos = (head)->next; pos != (head); pos = pos->next)

/*
 * �ⲿ��
 * ��ȫ����
 */
#define LIST_FOR_EACH_SAFE(pos, n, head) \
			for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

/*
 * �ⲿ��
 * �������
 */
#define LIST_FOR_EACH_PREV(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)


//end
#endif