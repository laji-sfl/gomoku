/*
 *	数据的格式与意义
 */
#ifndef _DATATYPE_H_
#define _DATATYPE_H_

#include "list.h"		//链表的函数与结构的声明

#define NAMELEN 16		//玩家名字的最大长度

#define LOGIN		'1'	//登录，登录时发起连接，连接之后发送登录验证字段
	/*
	 *server返回的数据格式：1表示功能，1或者0，表示可以登录或不可以登录
	 *server接收的数据格式：1，name:pwd&
	*/

#define STARTMATCH	'2'	//开始匹配对手，加入等待链表，匹配之后记录匹配结果
	/*
	 *server返回的数据格式：2，1或者0，对手name，1表示加入等待队列
	 *server接收的数据格式：2，发起请求者name，
	*/
#define REGISTER 	'3'	//注册账号，在文件中添加一条记录
	/*
	 *server返回的数据格式：3,1或者0，失败原因
	 *server接收的数据格式：3，name（16），pwd（16）
	*/
#define MOVE		'4'	//移动棋子，并且将位置存入链表
	/*
	 *server返回的数据格式：不返回
	 *server接收的数据格式：4，x， y
	*/
#define UNDOMOVE	'5'	//悔棋，并且将位置从链表删除
	/*
	 *server返回的数据格式：5，1或0，（用不用悔几步）,2表示拒绝
	 *server接收的数据格式：5，1或0来区分是提出悔棋还是对方的回应，原因
	*/
#define TALK		'6'	//聊天
	/*
	 *server返回的数据格式：不返回
	 *server接收的数据格式：6，数据
	*/
#define CHANGERIVAL '7'	//服务器接收到之后，直接断开两个人的连接
	/*
	 *server返回的数据格式：不返回
	 *server接收的数据格式：7
	*/
//#define TIMEOUT		'8'	//时间倒计时结束，直接让下一个人走
	/*
	 *server返回的数据格式：第一个版本不做那么详细，服务器不处理，客户端也不发，客户端自己计算
	 *server接收的数据格式：
	*/
//#define	COLSE		'9'	//客户端断开连接,客户端和服务器端任意一个close就会使另一个知道，所以不用发
	/*
	 *server返回的数据格式：不返回
	 *server接收的数据格式：9，name
	*/

//等待匹配的链表的结点
struct waitMatch {
	int fd;
	char name[NAMELEN];
};

//这表示一局游戏
struct game {
	int blackfd;		//黑棋的描述符
	int whitefd;		//白棋的描述符
};

/*
 *	创造两个链表，一个用作记录没有匹配到的玩家的描述符。
 				另一个记录已经匹配到的玩家的描述符。
 */

#endif
