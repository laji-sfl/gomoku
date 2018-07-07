/*
 *	线程池的头文件
 */

#ifndef __THREADPOLL_H__
#define __THREADPOLL_H__

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "mainAidFun.h"	//调用日志函数

//参数结构体
//线程池的控制结构
struct ThPoll
{
    pthread_mutex_t thmutext;   //全局锁
    pthread_cond_t thcond;      //全局条件变量
    struct TaskNote *head;      //任务链表的头结点
//    int thNum;  //池中线程的数量
    char isEnd; //线程池是否被销毁,'0'被销毁
};

//任务链表节点
struct TaskNote
{
    int fd; //文件描述符
    struct TaskNote *next;
};

//初始化线程池,'0'成功，‘1’失败，参数线程个数,二级指针将分配的内存地址传出
char initThreadPoll(int threadNum, struct ThPoll **thPoll);

//往任务队列添加任务，并唤醒一个线程执行,'0'ok,'1'no
char addTaskToList(int fd, struct ThPoll *thPoll);

//线程执行处理函数
void* threadFun(void *arg);

//线程池的销毁函数
void destroyThreadPoll(struct ThPoll *thPoll);

#endif
