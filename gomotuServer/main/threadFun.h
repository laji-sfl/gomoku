/*
 *	线程函数和线程调用函数的头文件
 */
#ifndef _THREADFUN_H_
#define _THREADFUN_H_

#include <pthread.h>
#include "dataType.h"
#include "threadPoll.h"
#include "accessMysql.h"
#include "encryption.h"

#define BUFSIZE 1024

//extern char aes_Key[16] = "4Dskdj3241DEAe4";
char myaes_Key[16] = "4Dskdj3241DEAe4";

//线程任务的实际处理入口函数，参数是fd
extern void threadCall(struct TaskNote *arg);

//读取fd中的内容,存储在堆区，并将指针传出，内存释放在分析调用之后
//疑惑：这个函数是否可以一次性读取完毕一个协议数据报，在epoll模式下，我在测试中发现epoll模式在读取的时候如果出现网络延迟或者其他我不明白的原因可能会造成串报，也会造成一个数据报分成两个读取，还要耐心细致的测试。
void readFd(int fd, char **msg, int epollfd);

//分析msg，来调用不同的函数
void analyzeMsg(char *msg, int fd);

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
void updateMsg(int fd, char *msg);
void updateImg(int fd, char *msg);
void changeKey(int fd, char *msg);//接收客户端的公钥

//辅助函数
int isExist(char *buf, char *msg);
struct node* findFd(int fd, struct node *head);		//根据文件描述符，查找出所在链表的位置
void transmit(int fd, char *msg);					//查找已匹配的游戏，转发两个人之间的消息

#endif
