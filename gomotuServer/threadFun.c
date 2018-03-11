/*
 *	有关线程的函数，和线程调用到的函数
 */

#include "threadFun.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
 * 	下一个版本的服务器，将程序改为守护进程，将printf改为日志系统，将读写文件改为读写数据库
 * 	将线程改为线程池，把线程的锁搞清楚（我还不知道我这个版本的锁到底有没有用）
 * 	面对tcp的连接的异常情况要考虑进去，将网络原理搞明白
 * 	添加加密的模块将通信的数据进行加密处理
 */

//将mainServe.c中的数据结构的头指针拿过来
extern struct node *alreadyMt;		//已经匹配的玩家链表
extern struct node *waitMt;			//还没有匹配的玩家链表

//创建线程
void newThread(int confd)
{
	struct threadArg 	*arg;	//线程参数
	pthread_t 			*ptid;	//线程id
	arg = (struct threadArg *)malloc(sizeof(struct threadArg));//栈区不行
	ptid = (pthread_t *)malloc(sizeof(pthread_t));
	arg->fd = confd;

	pthread_create(ptid, NULL, threadCall, arg);	//创建一个线程
	pthread_detach(*ptid);		//设置线程分离
	free(ptid);
}

//主线程处理函数
void* threadCall(void *arg)
{
	int fd = ((struct threadArg *)arg)->fd;	//套接字描述符
	free((struct threadArg *)arg);			//释放参数的空间
	int len = 0;							//读取的长度
	char buf[BUFSIZE] = {0};				//读取的缓冲区
	char msg[BUFSIZE + 1] = {0};			//读到的全部内容，用作字符串处理

	//read
	while(1) {
		len = read(fd, buf, BUFSIZE);
		if(len == -1) {
		 	if(errno == EAGAIN) {			//非阻塞，表示读取完毕
//				printf("read over fd:%d!\n", fd);
				break;
			}
			else {
//				printf("read error:%d!\n", fd);
				close(fd);	//不管什么错误，先关闭连接
				break;
				//TODO:处理那些特殊的情况，连接断开，入去错误，还有没想到的错误。
			}
		}
		else if(len == 0) {
			printf("len == 0; close fd: %d\n", fd);
			close(fd);	//客户端主动断开连接,会自动将event中的这个描述符清除
			//遇到文件描述符如何处理
			//经测验qt端，如果之间关闭程序会到这里，退出线程就可以了，文件描述符可能是对端的主动关闭
			//还要把tcp协议多看懂
			//需要将链表释放
			struct node *tmp = NULL;
			struct game *beDel = NULL;
			if((tmp = findFd(fd, alreadyMt)) != NULL) {
				pthread_rwlock_rdlock(&rwlock);
				free(deleteNode(alreadyMt, tmp->pnext->pdata));	//删除链表中的节点
				pthread_rwlock_unlock(&rwlock);
			}
			if((tmp = findFd(fd, waitMt)) != NULL) {
				pthread_rwlock_rdlock(&rwlock);
				free(deleteNode(waitMt, tmp->pnext->pdata));	//删除链表中的节点
				pthread_rwlock_unlock(&rwlock);
			}
			pthread_exit(0);
			break;
		}
		else			//将buf清空，并且将读取的内容组合起来
		{
			strcat(msg, buf);	//拼接字符串
			memset(buf, 0, BUFSIZE);
		}
	}//while(1) read

	printf("recv msg::fd: %d, msg:%s\n", fd, msg);	//接受输入的时候最后的回车换行也接受了

	//dispose
	//处理函数都需要保证数据的同步，需要加锁
	switch(msg[0]) 
	{
	case LOGIN:
		login(fd, msg);
		break;
	case STARTMATCH:
		startMatch(fd, msg);
		break;
	case REGISTER:
		registerCount(fd, msg);
		break;
	case MOVE:
		move(fd, msg);
		break;
	case UNDOMOVE:
		undoMove(fd, msg);
		break;
	case TALK:
		talk(fd, msg);
		break;
	case CHANGERIVAL:
		changeRival(fd, msg);
		break;
//	case TIMEOUT:
//		timeOut(fd, msg);
//		break;
//	case COLSE:
//		closeClie(fd, msg);
//		break;
	default:
		printf("format illegal, must be 1--9.\n");
		break;
	}

	printf("thread over: fd: %d, threadID: %lu!\n\n", fd, pthread_self());
	pthread_exit(0);	
}

//辅助函数
int isExist(char *buf, char *msg)
{
	int i = 0;
	while(buf[i++] == msg[i]) {		//i++ < i相等
		if(buf[i] == '&')	//表示前面的都相等
			return 1;
	}
	return 0;
}

//寻找描述符在链表中的位置
struct node* findFd(int fd, struct node *head)
{
	while(head->pnext != NULL) {	//找到了就返回前面的哪一个
		if(((struct game *)(head->pnext->pdata))->blackfd == fd || 
			((struct game *)(head->pnext->pdata))->whitefd == fd )
			return head;
		head = head->pnext;
	}
	return NULL;
}

//查找已匹配的游戏，转发两个人之间的消息
void transmit(int fd, char *msg)
{
	pthread_rwlock_rdlock(&rwlock);
	struct node *tmp = findFd(fd, alreadyMt);
	pthread_rwlock_unlock(&rwlock);
	if(((struct game *)(tmp->pnext->pdata))->blackfd == fd)		//根据匹配的描述符直接进行转发
		write(((struct game *)(tmp->pnext->pdata))->whitefd, msg, strlen(msg));
	else
		write(((struct game *)(tmp->pnext->pdata))->blackfd, msg, strlen(msg));
}

//根据宏来区分的不同功能函数
void login(int fd, char* msg)
{
//	printf("\n ** %s ** \n", msg);
	/*打开文件，查询是否有注册*/
	char buf[36] = {0};						// 16 + 1 + 16 + 1 < 36
	int yesOrNo = 0;
	pthread_rwlock_rdlock(&rwlock);			//加读锁
	FILE *file = fopen("./namePwd", "r");	//只读打开
	while(fgets(buf, 36, file)) {
		if((yesOrNo = isExist(buf, msg)))	//返回1表示存在，
			break;
	}										//TODO：该为连接数据库直接查询效率会高
	fclose(file);
	pthread_rwlock_unlock(&rwlock);			//解开锁
	/*返回判断结果*/
	if(yesOrNo)
		write(fd, "11", 2);
	else
		write(fd, "10", 2);
	/*断开连接*/
	close(fd);
}

void startMatch(int fd, char* msg)
{
	struct waitMatch *tmp = NULL;	//等待队列节点
	struct game *newGame  = NULL;	//已匹配队列节点
	char bufTmp[16] = {0};

	pthread_rwlock_wrlock(&rwlock);
	if(getLength(waitMt) > 0) {
		tmp = deleteNode(waitMt, waitMt->pnext->pdata);				//删除第一个进队列的
		newGame = (struct game *)malloc(sizeof(struct game));		//创建新的已匹配节点
		newGame->blackfd = tmp->fd;		//从等待队列中取出的玩家
		newGame->whitefd = fd;			//正在匹配的玩家
		insertNode(alreadyMt, newGame);	//加入已匹配的队列

		sprintf(bufTmp, "21%s", tmp->name);			//格式化
		write(fd, bufTmp, strlen(bufTmp));//通知之前在等待队列中的人
		memset(bufTmp, 0, sizeof(bufTmp));
		sprintf(bufTmp, "21%s", msg+1);
		write(tmp->fd, bufTmp, strlen(bufTmp));
		free(tmp);						//释放从等待队列取出来的节点
		printf("new player matched\n");
	}
	else {
		tmp = (struct waitMatch *)malloc(sizeof(struct waitMatch));	//创建新的等待匹配的节点
		tmp->fd = fd;
		strcpy(tmp->name, msg+1);	//将name赋值
		insertNode(waitMt, tmp);	//将节点加入等待队列
		printf("new player startMatch\n");
		write(fd, "201", 3);			//没有匹配成功
	}
	pthread_rwlock_unlock(&rwlock);
}

void registerCount(int fd, char* msg)
{
	char buf[36] = {0};
	int yesOrNo = 0;
	pthread_rwlock_rdlock(&rwlock);			
	FILE *file = fopen("./namePwd", "r");	//读
	while(fgets(buf, 36, file)) {
		if((yesOrNo = isExist(buf, msg)))
			break;
	}			
	fclose(file);							
	pthread_rwlock_unlock(&rwlock);			

	if(yesOrNo)
		write(fd, "30already exist", 15);
	else {
		pthread_rwlock_wrlock(&rwlock);
		file = fopen("./namePwd", "a");		//追加
		fprintf(file, "%s\n", msg+1);		//每个字符1字节大，最多34个字符，name16，pwd16，分割2
		fclose(file);
		pthread_rwlock_unlock(&rwlock);
		write(fd, "31", 2);
	}

//	close(fd);	//注册之后不保持连接//辣鸡设计，为什么要断开，唉
}

void move(int fd, char* msg)
{
	transmit(fd, msg);
}

void undoMove(int fd, char* msg)
{
	transmit(fd, msg);
}

void talk(int fd, char* msg)
{
	transmit(fd, msg);
}

void changeRival(int fd, char* msg)
{
	struct node *tmp;
	struct game *beDel;
	int rival;	//通知给对方，已经不想和你打了
	pthread_rwlock_rdlock(&rwlock);
	tmp = findFd(fd, alreadyMt);
	if(tmp == NULL)
		return;
	beDel = deleteNode(alreadyMt, tmp->pnext->pdata);	//删除链表中的节点
	rival = (fd == beDel->blackfd) ? beDel->whitefd : beDel->blackfd;
	free(beDel);	//释放创建的已匹配的节点
	pthread_rwlock_unlock(&rwlock);
//	write(rival,"70",2);	//表示对方已重新匹配
	close(rival);			//断开未点击玩家的连接
//	startMatch(fd, msg);	//重新开始匹配,
	close(fd);				//服务器不帮他匹配，让他自己重新连接服务器去匹配
}

//void timeOut(int fd, char* msg)
//{
	//设计的真的是惨绝人寰，为什么要发送时间呢，两台机器分别自己记录就行了
	//到后期版本，服务器可以和两边的数据做对比防止作弊
//	transmit(fd, msg);
//}

//void closeClie(int fd, char* msg)
//{
	/*查询队列中是否还有这个描述符*/

//	struct node *tmp;
//	struct game *beDel;
//	if((tmp = findFd(fd, alreadyMt)) != NULL) {
//		pthread_rwlock_rdlock(&rwlock);
//		free(deleteNode(alreadyMt, tmp->pnext->pdata));	//删除链表中的节点
//		pthread_rwlock_unlock(&rwlock);
//	}
//	if((tmp = findFd(fd, waitMt)) != NULL) {
//		pthread_rwlock_rdlock(&rwlock);
//		free(deleteNode(waitMt, tmp->pnext->pdata));	//删除链表中的节点
//		pthread_rwlock_unlock(&rwlock);
//	}

//	close(fd);	//匹配失败客户主动断开连接
				//一局结束之后等待30秒，如果不重新匹配就不管他，直接再一次开始游戏
				//否则断开连接或者重新匹配
//}
