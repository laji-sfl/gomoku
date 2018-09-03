/*
 *	链表的实现
 *		1、没有对malloc的返回值进行判断
 */

#include "list.h"		//链表的函数与结构的声明
#include <stdlib.h>

//创建新的链表
struct node* createList()
{
	struct node *head = (struct node *)malloc(sizeof(struct node));
	head->pdata = NULL;
	head->pnext = NULL;
	return head;
}

//插入节点
int insertNode(struct node *head, void *beInsert)
{
	if(head == NULL || beInsert == NULL) 
		return -1;

	while(head->pnext != NULL) {	//找到最后一个节点
		head = head->pnext;
	}

	//创建新结点，并插入
	struct node *newNode = (struct node *)malloc(sizeof(struct node));
	newNode->pdata = beInsert;
	newNode->pnext = NULL;
	head->pnext = newNode;

	return 0;
}

//删除节点,返回存储的数据的指针
void* deleteNode(struct node *head, void *beDelete)
{
	if(head == NULL || beDelete == NULL) 
		return NULL;

	struct node *current = findNode(head, beDelete);	//找到要删除结点的前一个
	struct node *tmp = current->pnext;	//保存要删除的结点
	void *ret = tmp->pdata;				//保存数据的指针
	current->pnext = tmp->pnext;		//连接链表
	free(tmp);
	return ret;
}

//得到链表的长度
int getLength(struct node *head)
{
	if(head == NULL)
		return -1;

	int len = 0;
	while(head->pnext != NULL) {
		len++;
		head = head->pnext;
	}
	return len;
}

//查找节点，返回前一个节点的指针
struct node* findNode(struct node *head, void *beFind)
{
	if(head == NULL || beFind == NULL)
		return NULL;

	while(head->pnext != NULL) {
		if(head->pnext->pdata == beFind)
			return head;
		else
			head = head->pnext;
	}
	return NULL;
}
