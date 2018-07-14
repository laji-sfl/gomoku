/*
 *	工作线程的任务处理函数
 */

#include "threadFun.h"	//此文件的函数声明
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*	TODO:
 * 	下一个版本的服务器，将程序改为守护进程，将printf改为日志系统
 * 	面对tcp的连接的异常情况要考虑进去，将网络原理搞明白
 * 	添加对SIGINT的注册，这样就可以安全结束程序，或者监听标准输入，结束程序（对服务器进行管理）
 */

//将mainServe.c中的数据结构的头指针拿过来
//extern struct node *alreadyMt;		//已经匹配的玩家链表
//extern struct node *waitMt;			//还没有匹配的玩家链表

//任务处理函数入口
void threadCall(struct TaskNote *arg)	//参数不需要在此函数释放
{
	char *msg = NULL;

	//读取内容
	readFd(arg->fd, &msg, arg->epollfd);

	//分析调用
    analyzeMsg(msg, arg->fd);
}

//读取fd中的数据报
void readFd(int fd, char **msg, int epollfd)
{
	int len = 0;	//读取的长度
	char tmp[1024] = {0};

	//传出msg的内存区域指针
	char *buf = (char *)malloc(BUFSIZE);	//直接分配1024个字节，不去节省内存了（这个之后再说）
	memset(buf, 0, BUFSIZE);
	*msg = buf;

	//循环读取
	while(1) {
		len = read(fd, buf, BUFSIZE);
		if(len == -1) {
		 	if(errno == EAGAIN) {	//读取完毕
				printf("read over fd:%d!\n", fd);
				//set_log("read fd over");	//还需要将函数设置成为可接受可变参数的
				break;
			}
			else {	//有错误说明连接出现了问题，就直接断开连接，取出epoll的监听
				printf("read error:%d!\n", fd);
				set_log("read error!;");
				close(fd);
				epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
				//TODO:清理匹配信息
				break;
			}
		}
		else if(len == 0) {	//遇到文件结尾
			printf("len == 0; close fd: %d\n", fd);
			set_log("close fd;");
			close(fd);	
			epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
			
			//TODO:清理匹配的队列信息
			/*
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
			*/
			break;
		}
		else {	//应该不会执行到这里，做个测试
			printf("fd=%d,len=%d;\n", fd, len);
			strcat(tmp, buf);
			memset(buf, 0, BUFSIZE);
		}
	}
    //解密,每次读的内容都解密，要求客户端每次都加密，服务器发出的内容都不加密
    aesDecrypt(tmp, buf, myaes_Key);

	printf("fd=%d, len=%d, msg=%s;\n", fd, len, *msg);	//看看最终的信息有没有读错
}

//分析数据报并且调用相应的函数
void analyzeMsg(char *msg, int fd)
{
	//处理函数都需要保证数据的同步，需要加锁
	switch(msg[0]) 
	{
	case LOGIN:
		login(fd, msg);
		break;
//	case STARTMATCH:
//		startMatch(fd, msg);
//		break;
	case REGISTER:
		registerCount(fd, msg);
		break;
//	case MOVE:
//		move(fd, msg);
//		break;
//	case UNDOMOVE:
//		undoMove(fd, msg);
//		break;
//	case TALK:
//		talk(fd, msg);
//		break;
//	case CHANGERIVAL:
//		changeRival(fd, msg);
//		break;
    case CHANGEKEY:
        changeKey(fd, msg);
		break;
	case UPDATEIMG:
		updateImg(fd, msg);
		break;
	case UPDATEMSG:
		updateMsg(fd, msg);
		break;
	default:
		printf("format illegal, please see dataType.h\n");
		break;
	}

    free(msg);//释放读取的内容
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
//void transmit(int fd, char *msg)
//{
//	pthread_rwlock_rdlock(&rwlock);
//	struct node *tmp = findFd(fd, alreadyMt);
//	pthread_rwlock_unlock(&rwlock);
//	if(((struct game *)(tmp->pnext->pdata))->blackfd == fd)		//根据匹配的描述符直接进行转发
//		write(((struct game *)(tmp->pnext->pdata))->whitefd, msg, strlen(msg));
//	else
//		write(((struct game *)(tmp->pnext->pdata))->blackfd, msg, strlen(msg));
//}

//根据宏来区分的不同功能函数
void login(int fd, char* msg)
{
	/*查询数据库是否允许登录*/
    char nameBuf[16] = {0};
    char *tmp = msg;
    char pwdBuf[16] = {0};
	char *img_dir = NULL;	//图片的目录
    char nameOut[16] = {0};
    char pwdOut[16] = {0};

	//将name和pwd读取出来
    for(int i = 0;i < 16; ++i) {
        nameBuf[i] = *(tmp + i + 1);    //因为msg的格式就是3(name16字节)(pwd16字节)
    }
    for(int i = 0;i < 16; ++i) {
        pwdBuf[i] = *(tmp + i + 17);
    }

    getMD5(nameBuf, nameOut);
    getMD5(pwdBuf, pwdOut);

    char yesOrNo = isRegister(nameOut, pwdOut, &img_dir);

	/*TODO:暂时不读取图片也不将图片发送给客户端*/
	printf("name:%s,img:%s\n", nameBuf, img_dir);
	free(img_dir);	//释放图片路径的资源

	/*返回判断结果*/
	if(yesOrNo == '0')		//可以登录
		write(fd, "11", 2);
	else					//不可以
		write(fd, "10", 2);
	/*断开连接*/
	close(fd);
}
/*
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
*/
void registerCount(int fd, char *msg)
{
    //将数据写入数据库
    char *tmp = msg;
    char ret = 0;
    char nameBuf[16] = {0};
    char pwdBuf[16] = {0};
    char nameOut[16] = {0};
    char pwdOut[16] = {0};

    for(int i = 0;i < 16; ++i) {
        nameBuf[i] = *(tmp + i + 1);    //因为msg的格式就是3(name16字节)(pwd16字节)
    }
    for(int i = 0;i < 16; ++i) {
        pwdBuf[i] = *(tmp + i + 17);
    }

    //数据库中密码存储MD5信息，不存明文
    getMD5(nameBuf, nameOut);
    getMD5(pwdBuf, pwdOut);

    //存储进数据库
    ret = saveNameToMysql(nameOut, pwdOut, "./girl.jpg"); //图片暂时不管
    if(ret == '0') {
        write(fd, "31", 2);
    } else if (ret == '1' || ret == '2') {
        write(fd, "30", 2);
        printf("fd=%d,注册失败\n", fd);
        set_log("注册失败。");
    }
}
/*
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
}*/


//void timeOut(int fd, char* msg)
//{
    //设计的真的是惨绝人寰，为什么要发送时间呢，两台机器分别自己记录就行了
    //到后期版本，服务器可以和两边的数据做对比防止作弊
//	transmit(fd, msg);
//}

//void closeClie(int fd, char* msg)
//{
    //查询队列中是否还有这个描述符

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


void updateMsg(int fd, char *msg)
{
    //更新密码
    char nameBuf[16] = {0};
    char *tmp = msg;
    char pwdBuf[16] = {0};
    char yesOrNo = '1';
    char nameOut[16] = {0}, pwdOut[16] = {0};

    //将name和pwd读取出来
    for(int i = 0;i < 16; ++i) {
        nameBuf[i] = *(tmp + i + 1);    //因为msg的格式就是3(name16字节)(pwd16字节)
    }
    for(int i = 0;i < 16; ++i) {
        pwdBuf[i] = *(tmp + i + 17);
    }

    getMD5(nameBuf, nameOut);
    getMD5(pwdBuf, pwdOut);

    //更新
    yesOrNo = updateUserMsg(nameOut, pwdOut, NULL);

    if(yesOrNo == '1')
        write(fd, "A1", 2);
    else
        write(fd, "A0", 2);
}

void updateImg(int fd, char *msg)
{
    char imgBuf[1024] = {0};    //存储图片
    char imgPwd[50] = {0};        //图片信息
    char yesOrNo = '1';
    int file_fd, n = 0;

    //读取
    //问题，一个线程处理一个文件描述符，处理结束后线程退出，文件描述符
    sprintf(imgPwd, "/home/sfl/download/%s",msg);
    file_fd = open(imgPwd, O_CREAT | O_RDWR | O_TRUNC, 0644);
    while((n = read(fd, imgBuf, 1024)) > 0) {
        write(file_fd, imgBuf, n);
    }
    close(file_fd);

    //更新
    yesOrNo = updateUserMsg(msg+1, NULL, imgPwd);

    if(yesOrNo == '1')
        write(fd, "A1", 2);
    else
        write(fd, "A0", 2);
}

//只负责接收客户端的公钥
void changeKey(int fd, char *msg)
{
    //产生一个aes加密用的秘钥，TODO:下个版本，需要产生随机的秘钥，是否对每一个fd一个秘钥需要考虑
    //接收客户端的公钥,加密AES秘钥
    char buf[1024] = {0};
    pubcrypt(msg+1, myaes_Key, buf);  //指针加一是否会造成错误，例如大小端？
    //将pubcrypt秘钥存储起来,使用全局的固定秘钥了～
    write(fd, buf, strlen(buf));    //是否可以对密文进行strlen
}
