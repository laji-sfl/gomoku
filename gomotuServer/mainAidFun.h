/*
 *	服务器主函数的辅助函数
 */

#ifndef __MAINAIDFUN_H__
#define __MAINAIDFUN_H__

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <strings.h>
#include <string.h>
#include <stdarg.h>
#include <sys/epoll.h>
#include <time.h>
#include <stdio.h>
#include "encryption.h" //加密函数接口

//线程函数的参数
struct thArg {
	int epollfd;
	int fd;
};

//创建一个socket套接字，ip4协议，tcp协议。
//绑定到ip和端口
int CreateSocket(int port, char *ip);


//设置文件描述符为非阻塞的
void setFdNoBlock(int fd);


//添加epoll的EL事件和epolloneshot事件	//实现没有使用epolloneshot，没有实现大文件传输
void epollAddFd(int epollFd, int netfd);


//日志文件操作			//这个版本日志非常简单只是打开文件写入文件
void set_log(char *str, ...);

//创建一个新的线程，并且将新连接上的fd加入epoll监听队列
void newThreadToAddEpoll(struct thArg *sockfd);
void *threadFunAccept(void *arg);

//发送公钥给客户端
void sendPubKeyToClient(int fd);

//给一个信号注册响应函数
void set_signalHandler(int signo, void(*f)(int));

//将数值转化为字符串，power表示的是进制，但是只实现了十进制的转化
int itoa(int num, char *str, int power);

//修改描述符的epoll监听事件
//void epollModFd(int fd)

#endif
