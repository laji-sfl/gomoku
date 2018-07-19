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
				printf("read over fd:%d.\n", fd);
				//set_log("read fd over");	//还需要将函数设置成为可接受可变参数的
				break;
			}
			else {	//有错误说明连接出现了问题，就直接断开连接，取出epoll的监听
				printf("read error:%d!\n", fd);
				set_log("read error!;");
				
				//清理匹配信息
				cleanFd(fd, epollfd);
				break;
			}
		}
		else if(len == 0) {	//遇到文件结尾
			printf("len == 0; close fd: %d\n", fd);
			set_log("close fd;");
			
			cleanFd(fd, epollfd);
			break;
		}
		else {	//肯定会执行到这里，不过一次调用执行两次的情况应该不会发生，read一次读完1024字节应该没有问题
			printf("len>0时: fd=%d,len=%d;\n", fd, len);

			strcat(tmp, buf);
			memset(buf, 0, BUFSIZE);
		}
	}
    //解密,每次读的内容都解密，要求客户端每次都加密，服务器发出的内容都不加密
    aesDecrypt(tmp, buf, myaes_Key);

	memcpy(*msg, tmp, BUFSIZE);
//	printf("readFd函数结束; fd=%d, len=%d, msg=%s\n", fd, len, *msg);	//看看最终的信息有没有读错
}

//分析数据报并且调用相应的函数
void analyzeMsg(char *msg, int fd)
{
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
	/*查询数据库是否允许登录*/
    char nameBuf[17] = {0};
    char *tmp = msg;
    char pwdBuf[17] = {0};
	char *img_dir = NULL;	//图片的目录
    char nameOut[17] = {0};
    char pwdOut[17] = {0};

	getNamePwd(msg, nameBuf, pwdBuf);

	//test:
	//printf("name:%s;pwd:%s\n", nameBuf, pwdBuf);

    getMD5(nameBuf, nameOut);
    getMD5(pwdBuf, pwdOut);

    char yesOrNo = isRegister(nameOut, pwdOut, &img_dir);//读取图片路径

	/*TODO:暂时不读取图片也不将图片发送给客户端*/


	/*返回判断结果*/
	if(yesOrNo == '0') {		//可以登录
		write(fd, "11", 2);
		free(img_dir);	//释放图片路径的资源
	}
	else	//不可登录
		write(fd, "10", 2);
}

//开始匹配
void startMatch(int fd, char* msg)
{
	struct waitMatch *newWait = NULL;	//新的等待队列节点
	struct waitMatch *beDel = NULL;		//从等待队列拿出来的节点
	struct game *newGame  = NULL;		//已匹配队列节点
	char bufTmp[20] = {0};
	char yesOrNo = '3';	

	//将代码从锁中拿出来，根据yesOrNo free
	newGame = (struct game *)malloc(sizeof(struct game));		//创建新的已匹配节点
	newWait = (struct waitMatch *)malloc(sizeof(struct waitMatch));	//创建新的等待匹配的节点

	//锁住链表进行操作
	pthread_rwlock_wrlock(&rwlock);	
	if(getLength(waitMt) > 0) {
		beDel = deleteNode(waitMt, waitMt->pnext->pdata);		//删除第一个进队列的
		newGame->blackfd = beDel->fd;	//从等待队列中取出的玩家
		newGame->whitefd = fd;			//正在匹配的玩家
		insertNode(alreadyMt, newGame);	//加入已匹配的队列
		yesOrNo = '1';					//表示匹配成功
	}
	else {
		newWait->fd = fd;
		strcpy(newWait->name, msg+1);	//将name赋值
		insertNode(waitMt, newWait);	//将节点加入等待队列
		yesOrNo = '0';					//表示没有匹配成功
	}
	pthread_rwlock_unlock(&rwlock);

	//为了减少锁住的代码量
	if (yesOrNo == '1') { 				//匹配成功
		sprintf(bufTmp, "21%s", beDel->name);
		write(fd, bufTmp, strlen(bufTmp));//通知之前在等待队列中的人
		memset(bufTmp, 0, sizeof(bufTmp));
		sprintf(bufTmp, "21%s", msg+1);
		write(beDel->fd, bufTmp, strlen(bufTmp));
		free(beDel);					//释放从等待队列取出来的节点
		free(newWait);					//因为匹配成功了，所以不存在新的等待节点
		printf("new player matched\n");
		set_log("new player matched");
	}
	else if (yesOrNo == '0') { 			//匹配不成功
		free(newGame);					//因为匹配失败，所以不存在新的游戏节点
		printf("new player startMatch\n");
		set_log("new player wait match");
		write(fd, "201", 3);			//没有匹配成功
	}
	else {
		printf("error in startMatch");
		set_log("error in startMatch");
	}
}

void registerCount(int fd, char *msg)
{
    //将数据写入数据库
    char *tmp = msg;
    char ret = 0;
    char nameBuf[17] = {0};//需要多一个字节做结尾不然printf这类就函数不知道结尾
    char pwdBuf[17] = {0};
    char nameOut[17] = {0};
    char pwdOut[17] = {0};

	getNamePwd(msg, nameBuf, pwdBuf);

	//test:
	//printf("name:%s;pwd:%s\n", nameBuf, pwdBuf);

    //数据库中密码存储MD5信息，不存明文
    getMD5(nameBuf, nameOut);
    getMD5(pwdBuf, pwdOut);

    //存储进数据库
    ret = saveNameToMysql(nameOut, pwdOut, "./girl.jpg"); //默认一个
    if(ret == '0') {
        write(fd, "31", 2);
    } 
	else if (ret == '1' || ret == '2') {
        write(fd, "30", 2);
        printf("fd=%d,注册失败\n", fd);
        set_log("注册失败。registerCount");
    }
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
	int rival;	//对方的文件描述符

	pthread_rwlock_wrlock(&rwlock);
	tmp = findFd(fd, alreadyMt);
	if(tmp == NULL) {	//不存在已经匹配的节点,说明对手已经发送过了
		pthread_rwlock_unlock(&rwlock);
		return;
	}
	beDel = deleteNode(alreadyMt, tmp->pnext->pdata);	//删除链表中的节点
	pthread_rwlock_unlock(&rwlock);

	rival = (fd == beDel->blackfd) ? beDel->whitefd : beDel->blackfd;
	free(beDel);	//释放创建的已匹配的节点
	
	//通知另一个人，对手已经不想和你玩了
	write(rival, "70", 2);
}


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
    char nameBuf[17] = {0}, pwdBuf[17] = {0};
    char nameOut[17] = {0}, pwdOut[17] = {0};
    char *tmp = msg, yesOrNo = '1';

    //将name和pwd读取出来
	getNamePwd(msg, nameBuf, pwdBuf);

    getMD5(nameBuf, nameOut);
    getMD5(pwdBuf, pwdOut);

    //更新
    yesOrNo = updateUserMsg(nameOut, pwdOut, NULL);
    //yesOrNo = updateUserMsg(nameBuf, pwdBuf, NULL);

    if(yesOrNo == '1')
        write(fd, "A0", 2);//更新失败
    else
        write(fd, "A1", 2);//成功
}

void updateImg(int fd, char *msg)
{
    char imgPwd[100] = {0};        //图片路径
	char nameBuf[17] = {0}, nameOut[17] = {0};
    char yesOrNo = '1';

	//提取name
	getNamePwd(msg, nameBuf, NULL);

    sprintf(imgPwd, "/home/sfl/download/%s", nameBuf);	//拼接图片的存储路径
	getMD5(nameBuf, nameOut);

    //更新
    yesOrNo = updateUserMsg(nameOut, NULL, imgPwd);
    //yesOrNo = updateUserMsg(nameBuf, NULL, imgPwd);

    if(yesOrNo == '1')
        write(fd, "A0", 2);//失败
    else
        write(fd, "A1", 2);//成功
}

//只负责接收客户端的公钥
void changeKey(int fd, char *msg)
{
    //接收客户端的公钥,加密AES秘钥,将加密后的AES秘钥发送给客户端
    char cipherBuf[1024] = {0};
	char msgBuf[1024] = {0};
	msgBuf[0] = 'D';	//D表示收到的是AES秘钥

    pubcrypt(msg+1, myaes_Key, cipherBuf); 
	strcat(msgBuf, cipherBuf);

    write(fd, msgBuf, strlen(msgBuf));   
}

//提取名字和密码
void getNamePwd(char *msg, char *name, char *pwd)
{
	int i = 0;
	if (msg == NULL) return;

	if (name != NULL) {
	    for(i = 0;i < 16; ++i) {
        	name[i] = *(msg + i + 1);    //因为msg的格式就是3(name16字节)(pwd16字节)
    	}
	}

	if (pwd != NULL) {
	    for(i = 0;i < 16; ++i) {
	        pwd[i] = *(msg + i + 17);
	    }
	}
}

//清理文件描述符
void cleanFd(int fd, int epollfd)
{
	struct node *tmped = NULL;	//已经匹配
	struct node *tmping = NULL;	//在等待匹配
	struct game *beDel;
	int tmpfd;	//对手
	
	//查找是否存在
	pthread_rwlock_rdlock(&rwlock);
	tmped = findFd(fd, alreadyMt);
	tmping = findFd(fd, waitMt);
	pthread_rwlock_unlock(&rwlock);

	if (tmping != NULL)	{
		pthread_rwlock_wrlock(&rwlock);
		free(deleteNode(waitMt, tmping->pnext->pdata));
		pthread_rwlock_unlock(&rwlock);
	}

	if (tmped != NULL) {
		beDel = (struct game *)(tmped->pnext->pdata);
		tmpfd = (beDel->whitefd == fd) ? beDel->blackfd : beDel->whitefd;
		//通知另一个，或者直接将另一个断开连接
		epoll_ctl(epollfd, EPOLL_CTL_DEL, tmpfd, NULL);
		close(tmpfd);

		free(deleteNode(alreadyMt, beDel));	
	}

	//根据epollfd删除
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
	close(fd);
}
