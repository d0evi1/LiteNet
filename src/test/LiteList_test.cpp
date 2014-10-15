#include "comm/LiteList.h"

#include <stdint.h>
#include <memory.h>
#include <stdio.h>

typedef struct 
{
	ListHead	head;
	uint32_t	data;
} TestNode;

/*
 *
 */
void single_list_test()
{
	printf("--------------------------\n");
	// Í·½Úµã
	TestNode	stList;
	bzero(&stList, sizeof(TestNode));
	stList.data = 100;

	INIT_LIST_HEAD(&stList.head);

	TestNode stNode1;
	bzero(&stNode1, sizeof(TestNode));
	stNode1.data = 1;
	ListAdd(&stNode1.head, &stList.head);

	TestNode stNode2;
	bzero(&stNode2, sizeof(TestNode));
	stNode2.data = 2;
	ListAdd(&stNode2.head, &stNode1.head);

	TestNode stNode3;
	bzero(&stNode3, sizeof(TestNode));
	stNode3.data = 3;
	ListAdd(&stNode3.head, &stNode2.head);

	TestNode stNode4;
	bzero(&stNode4, sizeof(TestNode));
	stNode4.data = 4;
	ListAdd(&stNode4.head, &stNode3.head);

	TestNode stNode5;
	bzero(&stNode5, sizeof(TestNode));
	stNode5.data = 5;
	ListAdd(&stNode5.head, &stNode4.head);

	ListHead* pstNext;
	ListHead* pstHead = &stList.head;
	ListHead* pstCur = pstHead->next;

	while(pstCur != pstHead)
	{
		TestNode* pstCurNode = LIST_ENTRY(pstCur, TestNode, head);
		pstNext = pstCur->next;
		// ListDel(pstCur); // no need.
		printf("%d\n", pstCurNode->data);
		pstCur = pstNext;
	}
}

/*
 *
 */
int main()
{
	//
	single_list_test();

	return 0;
}