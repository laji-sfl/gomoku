/*
 *	简易的网络通信协议
 *	1、数据报的格式，和代表的意义
 *	2、各个任务通信的基本过程
 */
#ifndef _DATATYPE_H_
#define _DATATYPE_H_

/*
 * 	需要客户端增加修改头像的功能，然后将头像的图片发来，（可以学习一下操作图片的库，比如压缩图片）
 * 	服务器在登录时，如果已经注册就查询出头像图片发给客户端，没注册就注册，然后给一个默认的图片
 * 	接到修改头像的指令，就把图片存起来，然后在更新数据库信息
 */

#include "list.h"		//链表的函数与结构的声明

#define NAMELEN 16		//玩家名字的最大长度
						//玩家的名字和密码的最大长度都是16
						//接收玩家的昵称和密码的数据格式是：(一位标志)name:pwd&   例如：1name:pwd&

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
#define 	UPDATEMSG	'A'	//修改密码
	/*
	 *	server返回的数据格式：'0'成功，’1‘失败
	 *	server接收的数据格式：Aname:pwd&
	 */
#define		UPDATEIMG 	'B'	//上传图片
	/*
	 * 	server返回数据：‘0’成功，‘1’失败
     * 	server接收数据：'B'name 图片
	 */
#define     CHANGEKEY 'C'	//交换公钥
	/*
	 * 	server主动发送公钥到客户端
	 * 	客户端收到服务器的公钥之后立马返回自己的公钥
	 */
#define 	CREATEMSGKEY 'D'	//服务器创建一个通信用的秘钥发给客户端
	/*
	 *	服务器发送客户端接收
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
