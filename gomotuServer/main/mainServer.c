#include "mainAidFun.h"
#include "threadPoll.h"	//线程池

#define EVENTNUM    20  //epoll_wait返回的事件数组的元素个数，也是最大的个数

int main(int argc, char *argv[])
{
    //初始化变量
    int sockfd = 0;                 //服务器套接字
    int epollFd = 0;                //epoll的文件描述符
    struct epoll_event *event_ptr = NULL;//监听事件的存储位置的指针

    //判断命令行参数
    if(argc != 3) {
        printf("请输入IP和端口\n");
		set_log("请输入IP和端口");
        return -1;
    }

    //创建套接字
    if((sockfd = CreateSocket(atoi(argv[2]), argv[1])) == -1) {
        printf("创建套接字失败\n");
		set_log("创建套接字失败");
    }

    //创建epoll
    epollFd = epoll_create(1);

    //将服务器套接字加入监听
    epollAddFd(epollFd, sockfd);

    //为epoll_wait返回的监听事件数量分配内存，数量的随便分的
    event_ptr = (struct epoll_event *)malloc(sizeof(struct epoll_event) * EVENTNUM);

	//创建并初始化线程池
	struct ThPoll *thpoll = NULL;
	if ( '1' == initThreadPoll(5, &thpoll)) {	//暂时默认5个线程
		set_log("初始化线程池失败");
	}

    //开始循环的监听描述符
    while (1) {
        int retNum = 0; //epoll_wait返回的响应的文件描述符的个数
        int i = 0;  //循环变量

        //timeout网上都是将其置为-1，但是man手册说未定义
        retNum = epoll_wait(epollFd, event_ptr, EVENTNUM, -1);

		//test
		printf("epoll wait retNum: %d.\n", retNum);

        //循环的处理每一个响应的文件描述符
        for(i = 0;i < retNum; ++i) {

            //服务器套接字响应，表示有新的连接
            if(sockfd == event_ptr[i].data.fd) {
				struct thArg *arg = (struct thArg *)malloc(sizeof(struct thArg));//线程参数
				arg->epollfd = epollFd;
				arg->fd = sockfd;
				newThreadToAddEpoll(arg);//创建一个新的线程将新连接的客户fd加入epoll的监听
				//test
				//printf("new connect!!\n");
            }
            //其他的与客户端通信的套接字
            else {
                if('1' == addTaskToList(event_ptr[i].data.fd, thpoll)) { //添加到线程池的任务链表中
					set_log("mainServer addTaskToList失败");
				}

				//test
				//printf("recv msg!!\n");
            }
        }
    }

	//由于服务器程序暂时没有响应退出信号所以先暂时不释放资源
	destroyThreadPoll(thpoll);		//销毁线程池
    epoll_ctl(epollFd, EPOLL_CTL_DEL, sockfd, NULL);//将服务器套接字删除
    close(sockfd);  //关闭服务器套接字
    free(event_ptr);//释放事件数组的内存
    close(epollFd); //关闭epoll的套接字
    return 0;
}
