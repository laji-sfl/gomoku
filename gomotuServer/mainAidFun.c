/*
 *	服务器主函数的辅助函数的实现
 *		没有对网络连接中出现的错误进行检测
 */
#include "mainAidFun.h"

void setFdNoBlock(int fd)
{
    int flag;

    //获取文件描述符标志
    if((flag = fcntl(fd, F_GETFL, 0)) == -1)
        set_log("error:fcntl,%s:%s:%d", __FILE__,__FUNCTION__,__LINE__);

    //设置文件描述符标志
    flag |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flag) == -1)
		set_log("error:fcntl,%s:%s:%d", __FILE__,__FUNCTION__,__LINE__);
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
    listen(sockfd, 128);

    return sockfd;
}

void epollAddFd(int epollFd, int netfd)
{
    struct epoll_event event;

	//设置添加的fd为非阻塞的
	setFdNoBlock(netfd);

    event.data.fd = netfd;
    event.events = EPOLLIN | EPOLLET;   //读事件，边沿触发，暂时不弄epolloneshot
    if(0 != epoll_ctl(epollFd, EPOLL_CTL_ADD, netfd, &event)) {
        printf("添加epoll监听事件失败，fd=%d;\n", netfd);
		set_log("add epoll fd defeat,%s:%s:%d",__FILE__,__FUNCTION__,__LINE__);
    }
}

int itoa(int num, char *str, int power)
{
	if(power != 10)
		return -1;
	int ret = 0;
	while(num)
	{
		*str = (num % 10) + '0';
		num /= 10;
		str++;
		ret++;
	}
	return ret;
}

#define CHECK_BUFLEN(m, n) m += n;if(m > 2048) break;

//设置日志文件操作,日志的每一句都不能超过1024
void set_log(char *str, ...)
{
	time_t rawtime;
    struct tm *tim;
	int fd = 0, inputlen = 0;
    char buf[2048 + 50] = {0};
	char strbuf[2048] = {0}, *pw = strbuf, numbuf[25] = {0}, *s = NULL;

	va_list multiArg = NULL;
	va_start(multiArg, str);	//第二个参数是可变参数前面的哪一个参数

	inputlen = strlen(str);	//计算字符串长度防止缓冲区溢出

	//根据占位符判断类型，组合字符串
	while(*str != '\0' && inputlen < 2048) {
		if(*str == '%'){
			switch(*(str+1))
			{
			case 'c':
				CHECK_BUFLEN(inputlen,1);
				*pw = (char)va_arg(multiArg, int);
				pw++; str++;;
				break;
			case 'd':
				CHECK_BUFLEN(inputlen, itoa(va_arg(multiArg, int), numbuf, 10));
				strcat(pw, numbuf);
				pw += strlen(pw);
				str++;
				break;
			case 's':
				s = va_arg(multiArg, char *);
				CHECK_BUFLEN(inputlen, strlen(s));
				strcat(pw, s);
				pw += strlen(pw);
				str++;
				break;
			default:
				break;
			}
		}else{
			*pw = *str;
			pw++;
		}
		str++;
	}
	va_end(multiArg);

	//打开日志文件
	fd = open("gomotu.log", O_RDWR | O_CREAT | O_APPEND, 0664);

	//获取当前时间，并格式化写入文件的内容
	time(&rawtime);
    tim = localtime(&rawtime);

    sprintf(buf, "Time:%d月%d日-%d:%d:%d,log:%s\n", tim->tm_mon+1,
            tim->tm_mday, tim->tm_hour, tim->tm_min, tim->tm_sec, strbuf);

    write(fd, buf, strlen(buf));
    close(fd);
}

void newThreadToAddEpoll(struct thArg *sockfd)
{
	pthread_t tid;
	pthread_create(&tid, NULL, threadFunAccept, sockfd);
	pthread_detach(tid);
}

void *threadFunAccept(void *arg)
{
	struct thArg *sockfd = (struct thArg *)arg;
	int confd = 0;
	struct sockaddr_in clieaddr;
	int clielen = 0;
	
	while(1) {
		if((confd = accept(sockfd->fd, (struct sockaddr *)&clieaddr, &clielen)) == -1) {
			if(errno == EAGAIN || errno == EWOULDBLOCK) {
//				printf("accept EAGAIN\n");
				break;
			} else {
				set_log("accept error,%s:%s:%d",__FILE__,__FUNCTION__,__LINE__);
				break;
			}
		}
		epollAddFd(sockfd->epollfd, confd);//加入epoll监听

        //连接成功之后，就直接将服务器的公钥发给客户端
        //如果由服务器产生对称加密用的秘钥，就不需要将服务器公钥发给客户了，只需要接收客户的公钥，然后将加密的秘钥发给客户端就可以了。
        sendPubKeyToClient(confd);
	}

    free(sockfd);   //参数
	pthread_exit(NULL);
}

//void epollModFd(int fd)
//{}

void sendPubKeyToClient(int fd)
{
//    char *key = NULL;
//    char buf[2048] = {0};
//    buf[0] = 'C';       //C表示接收到的是公钥

    //TODO:从磁盘读取秘钥，写在这里会造成每一个连接都会读取秘钥，也可以写在初始化过程将秘钥存储在内存中减少读取的开销。
//    readRSAKey("./pub_str_key", &key);  //TODO:路径不应该直接写出来的应该改为参数
    //TODO:在readPubKey中将二级指针改为栈区的buf，在内部直接调用strcat会效率更高并且省去了分配管理内存的麻烦。

//    strcat(buf, key);

    //发送给客户端TODO:test
//    write(fd, "C", 1);

    //free(key);
}

void set_signalHandler(int signo, void (*f)(int))
{
	//不使用信号屏蔽字
	struct sigaction act;
	act.sa_handler = f;
	act.sa_flags = 0;
	if(0 > sigaction(signo, &act, NULL))
		set_log("sigaction error,%s:%s:%d",__FILE__,__FUNCTION__,__LINE__);
}

