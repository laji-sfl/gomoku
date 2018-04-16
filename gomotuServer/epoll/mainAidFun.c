/*
 *	服务器主函数的辅助函数的实现
 */
#include "mainAidFun.h"

void setFdNoBlock(int fd)
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

int CreateSocket(int port, char *ip)
{
    if(ip == NULL || port <= 0)
        return -1;

    //包括创建socket、初始化地址结构、bind，listen
    int sockfd = 0;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serv_addr, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(port);
    bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
    listen(sockfd, 10);     //连接等待队列的长度设置为10，是随便设置的

    return sockfd;
}

void epollAddFd(int epollFd, int netfd)
{
    struct epoll_event event;
    event.data.fd = netfd;
    event.events = EPOLLIN | EPOLLET;   //读事件，边沿触发，暂时不弄epolloneshot
    if(0 != epoll_ctl(epollFd, EPOLL_CTL_ADD, netfd, &event)) {
        printf("添加epoll监听事件失败，fd=%d;\n", netfd);
    }
}

//设置日志文件操作
void set_log(char *str)
{
	time_t rawtime;
    struct tm *tim;
	int fd = 0;
    char buf[2048 + 50] = {0};

	//打开日志文件
	fd = open("gomotu.log", O_RDWR | O_CREAT | O_APPEND, 0664);

	//获取当前时间，并格式化写入文件的内容
	time(&rawtime);
    tim = localtime(&rawtime);

    sprintf(buf, "Time:%d月%d日-%d:%d:%d,log:%s\n", tim->tm_mon+1,
            tim->tm_mday, tim->tm_hour, tim->tm_min, tim->tm_sec, str);

    write(fd, buf, strlen(buf));
    close(fd);
}

//void epollModFd(int fd)
//{

//}
