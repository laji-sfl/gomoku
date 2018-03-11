/*
 * 	主程序的入口
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/stat.h>
#include "list.h"		//链表数据结构
#include "threadFun.h"	//线程的相关函数
#include <string.h>
#include <errno.h>

#define PORT 9669
#define IP	"127.0.0.1"
#define MAXNUM 128		//主要是listen的上限，epoll的限制不应该设置，之后在分析优化。

/* 创建全局的数据结构 */
struct node *alreadyMt;		//已经匹配的玩家链表
struct node *waitMt;		//还没有匹配的玩家链表

//设置文件描述符的非阻塞
void set_fd_noBlock(int fd)
{
	int flag;

	//获取文件描述符标志
	if((flag = fcntl(fd, F_GETFL, 0)) == -1)
		printf("error: fcntl get flag\n");

	//设置文件描述符标志
	flag |= O_NONBLOCK;
	if(fcntl(fd, F_SETFL, flag) == -1)
		printf("error: fcntl set flag\n");
}

int main(void)
{
	int 	sockfd;								//监听套接字
	int 	confd;								//数据套接字
	int 	epofd;								//epoll占用的fd
	int 	msgLen = 0;							//实际读到的长度
	char 	msgBuf[MAXNUM] = {0};				//数据信息的长度
	int 	clie_addr_len = 0;					//连接者地址结构的长度
	struct sockaddr_in 	serv_addr, clie_addr;	//本地结构，连接者结构
	struct epoll_event 	event;					//监听事件
	struct epoll_event 	*ptr_events = NULL;			//监听事件数组的首地址

	//创建链表
	alreadyMt = createList();
	waitMt = createList();

	/*初始化socket*/
	printf("init socket\n");
	sockfd = socket(AF_INET, SOCK_STREAM, 0);		//创建监听套接字
	bzero(&serv_addr, sizeof(struct sockaddr_in));	//清零地址结构体
	bzero(&clie_addr, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;					//为服务器套接字赋值
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = inet_addr(IP);
	bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));//绑定服务器套接字
	listen(sockfd, MAXNUM);							//设置监听数量

	/* epoll来监听套接字文件描述符的状态 */
	printf("create epoll instance\n");
	set_fd_noBlock(sockfd);				//设置监听套接字为非阻塞
	epofd = epoll_create(1);			//创建epoll句柄
	event.data.fd = sockfd;
	event.events = EPOLLIN | EPOLLET;	//设置sockfd的读事件，和边缘触发
	epoll_ctl(epofd, EPOLL_CTL_ADD, sockfd, &event); //将sockfd添加到epoll的监听中
	ptr_events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * MAXNUM);

	/* 将标准输入也加入进去 */
	//TODO:

	/* 初始化读写锁 */
	pthread_rwlock_init(&rwlock, NULL);

	/* epoll的循环监听描述符是否可读、可写、有错误 */
	printf("start epoll wait\n");
	while(1)
	{
		int fdNum;	//需要去处理的fd的个数
		int i;		//循环变量

		fdNum = epoll_wait(epofd, ptr_events, MAXNUM, -1);	//阻塞，直到有fd需要处理
		for(i = 0;i < fdNum; i++) {							//遍历所有可以操作的描述符
			if(sockfd == ptr_events[i].data.fd) {			//表示有新的连接
				while(1) {									//边缘触发+非阻塞fd，需要循环读取完
					if((confd = accept(sockfd, (struct sockaddr *)&clie_addr, &clie_addr_len)) == -1){
						if(errno == EAGAIN || errno == EWOULDBLOCK) {//判断连接是否处理完
//							printf("accept EAGAIN, OK!\n");
							break;
						}
						else {
							printf("error accept\n");
							break;
						}
					}
					set_fd_noBlock(confd);							//设置文件描述符非阻塞
					event.data.fd = confd;
					event.events = EPOLLIN | EPOLLET;
					epoll_ctl(epofd, EPOLL_CTL_ADD, confd, &event);	//将confd添加进epoll的监听
					printf("\nnew connected fd:%d\n",confd);
				}//accept
				continue;	//直接开始下一个的处理
			}//sockfd == ptr_events[i].data.fd
			else {
//				printf("create new thread to handle fd: %d!\n\n", ptr_events[i].data.fd);
				newThread(ptr_events[i].data.fd);	//创建新的线程
			}
		}//for(0 --> fdNum)
	}//epoll_wait while(1)

	/* 销毁资源，释放内存 */
	free(alreadyMt);
	free(waitMt);
	pthread_rwlock_destroy(&rwlock);
	free(ptr_events);
	close(epofd);
	return 0;
}
