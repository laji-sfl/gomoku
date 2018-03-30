/*
 *	头文件，MySQL数据库的头文件
 *  编译MySQL的库函数的时候，需要加上gcc的参数 `mysql_config --cflags --libs`
 */

#ifndef __ACCESSMYSQL_H__
#define __ACCESSMYSQL_H__

//连接MySQL数据库，验证用户是否已经注册
// ‘0’已经注册，‘1’不存在，‘2’数据库出现错误，‘3’密码错误
// 传入的指针img_dir需要调用者释放
char isRegister(char *name, char *pwd, char **img_dir);


//连接MySQL数据库，将用户名和密码存储起来
// ‘0’成功，‘1’已经存在用户，‘2’数据库错误
char saveNameToMysql(char *name, char *pwd, char *img_dir);


//检测字符串的合法性
char isLegal(char *name);

#endif
