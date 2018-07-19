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

//将mainServe.c中的数据结构的头指针拿过来
struct node *alreadyMt;		//已经匹配的玩家链表
struct node *waitMt;		//还没有匹配的玩家链表

//暂时的使用固定的AES秘钥
char myaes_Key[16] = "4Dskdj3241DEAe4";

//线程任务的实际处理入口函数，参数是fd
extern void threadCall(struct TaskNote *arg);

//读取fd中的内容,存储在堆区，并将指针传出，内存释放在分析调用之后
void readFd(int fd, char **msg, int epollfd);

//分析msg，来调用不同的函数
void analyzeMsg(char *msg, int fd);

//读写锁
pthread_rwlock_t rwlock;

//根据宏来区分的不同功能函数
void login(int fd, char* msg);			//登录
void startMatch(int fd, char* msg);		//开始匹配对手
void registerCount(int fd, char* msg);	//注册账号
void move(int fd, char* msg);			//移动棋子
void undoMove(int fd, char* msg);		//悔棋
void talk(int fd, char* msg);			//聊天
void changeRival(int fd, char* msg);	//交换对手
//void timeOut(int fd, char* msg);		//时间到了
//void closeClie(int fd, char* msg);	//关闭连接
void updateMsg(int fd, char *msg);		//修改密码
void updateImg(int fd, char *msg);		//更换头像
void changeKey(int fd, char *msg);		//接收客户端的公钥

//辅助函数
struct node* findFd(int fd, struct node *head);		//根据文件描述符，查找出所在链表的位置
void transmit(int fd, char *msg);					//查找已匹配的游戏，转发两个人之间的消息
void getNamePwd(char *msg, char *name, char *pwd);	//从msg中提取出名字和密码
void cleanFd(int fd, int epollfd);					//将监听的fd去除，删除链表的节点，通知另一方

#endif
