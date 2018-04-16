/*
 *	用来存储，已连接的描述符，未连接的描述符，棋子的步骤
 */
#ifndef _LIST_H_
#define _LIST_H_

//节点
struct node {
	void *pdata;		//任意类型的数据的指针
	struct node *pnext;	//下一个节点
};

struct node * createList();						//创建新的链表
void insertNode(struct node *, void *);			//删除节点,返回存储的数据的指针
void* deleteNode(struct node *, void *);		//void* 表示的是在堆区申请的空间或静态区，在栈区会造成野指针
int  getLength(struct node *);
struct node* findNode(struct node *, void *);	//查找链表，返回前一个节点的指针

#endif