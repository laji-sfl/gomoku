/*
 *	链表数据结构的声明
 */
#ifndef _LIST_H_
#define _LIST_H_

//节点
struct node {
	void *pdata;		//任意类型的数据的指针
	struct node *pnext;	//下一个节点
};

struct node * createList();						//创建新的链表
int insertNode(struct node *, void *);			//插入节点，返回0成功，-1失败
void* deleteNode(struct node *, void *);		//返回被删除节点的数据
int  getLength(struct node *);
struct node* findNode(struct node *, void *);	//查找链表，返回前一个节点的指针

#endif
