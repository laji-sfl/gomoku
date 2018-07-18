/*
 *	简易的网络通信协议
 *	1、数据报的格式，和代表的意义
 *	2、各个任务通信的基本过程
 */
#ifndef _DATATYPE_H_
#define _DATATYPE_H_

#include "list.h"		//链表的函数与结构的声明

#define NAMELEN 16		//玩家的名字和密码的最大长度都是16
						//接收玩家的昵称和密码的数据格式是：(一个标志)(16字节的数据name)(16字节的pwd)

#define LOGIN		'1'	//登录
	/*
	 *server返回的数据格式：11成功登录，10不可以登录
	 *server接收的数据格式：1（16位name）（16位pwd）
 	*/

#define STARTMATCH	'2'	//开始匹配对手，加入等待链表，匹配之后记录匹配结果
	/*
	 *server返回的数据格式：2，1或者0，对手name，1表示加入等待队列
	 *server接收的数据格式：2，发起请求者name，
	*/

#define REGISTER 	'3'	//注册账号
	/*
	 *server返回的数据格式：31成功，30失败
	 *server接收的数据格式：3name（16）pwd（16）
	*/

#define MOVE		'4'	//移动棋子
	/*
	 *server返回的数据格式：不返回
	 *server接收的数据格式：4（2x）（2y）
	*/

#define UNDOMOVE	'5'	//悔棋
	/*
	 *server返回的数据格式：TODO:
	 *server接收的数据格式：
	*/

#define TALK		'6'	//聊天
	/*
	 *server返回的数据格式：不返回
	 *server接收的数据格式：6，数据
	*/

#define CHANGERIVAL '7'	//交换对手
	/*
	 *server返回的数据格式：给另一方发送70，表示对方想交换对手
	 *server接收的数据格式：7
	*/

//#define TIMEOUT	'8'	//时间倒计时结束
	/*
	 *server返回的数据格式：第一个版本不做那么详细，服务器不处理，客户端也不发，客户端自己计算
	 *server接收的数据格式：
	*/

//#define	COLSE		'9'	//客户端断开连接
	/*
	 *server返回的数据格式：不返回
	 *server接收的数据格式：9，name
	*/

#define 	UPDATEMSG	'A'	//修改密码
	/*
	 *	server返回的数据格式：A '0'成功，’1‘失败
	 *	server接收的数据格式：A（16name）（16新的pwd）
	 */

#define		UPDATEIMG 	'B'	//上传图片
	/*
	 * 	server返回数据：‘0’成功，‘1’失败
     * 	server接收数据：'B'name
	 */

#define     CHANGEKEY 'C'	//传输公钥
	/*
	 * 	server主动发送公钥到客户端
	 * 	server接收到客户端的公钥之后，用公钥将aes秘钥加密，发送给客户端
	 */

#define 	CREATEMSGKEY 'D'	//传输AES密钥
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
