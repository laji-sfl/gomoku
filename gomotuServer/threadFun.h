/*
 *	线程函数和线程调用函数的头文件
 */
#ifndef _THREADFUN_H_
#define _THREADFUN_H_

#include <pthread.h>
#include "dataType.h"

#define BUFSIZE 1024

//线程的参数
struct threadArg {
	int fd;
};

void newThread(int confd);	//把响应的fd传入，处理fd的信息
void* threadCall(void *arg);//读取fd的内容并根据内容做出处理。

//读写锁
pthread_rwlock_t rwlock;

//根据宏来区分的不同功能函数
void login(int fd, char* msg);
void startMatch(int fd, char* msg);
void registerCount(int fd, char* msg);
void move(int fd, char* msg);
void undoMove(int fd, char* msg);
void talk(int fd, char* msg);
void changeRival(int fd, char* msg);
void timeOut(int fd, char* msg);
void closeClie(int fd, char* msg);

//辅助函数
int isExist(char *buf, char *msg);
struct node* findFd(int fd, struct node *head);		//根据文件描述符，查找出所在链表的位置
void transmit(int fd, char *msg);					//查找已匹配的游戏，转发两个人之间的消息

#endif