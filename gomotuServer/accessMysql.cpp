/*
 *	访问MySQL数据库的库函数
 *	不具有独立的移植性，只能作为五子棋服务器查询、写入玩家的信息
 */
#include "accessMysql.h"
#include <stdio.h>
#include <mysql/mysql.h>// 使用的是libmysqlclient-dev

//连接MySQL数据库，验证用户是否已经注册
// ‘0’已经注册，‘1’存在相同的用户名，‘2’数据库出现错误，‘3’密码错误
char isRegister(char *name)
{
    MYSQL mysql;
    MYSQL_RES *res = NULL;
    MYSQL_ROW row = NULL;

    //初始化MySQL对象
    if(NULL == mysql_init(&mysql)) {
        printf("mysql_init error: %s \n", mysql_error(&mysql));
        return '2';
    }

    //与MySQL服务器建立连接
    if(NULL == mysql_real_connect(&mysql, "localhost", "lmd_ubuntu", "123abc", "gomotu", 0, NULL, 0)) {
        printf("mysql_real_connect error: %s \n", mysql_error(&mysql));
        return '2';
    }

    printf("connect success \n");

    mysql_close(&mysql);

    return '0';
}


//连接MySQL数据库，将用户名和密码存储起来
// ‘0’成功，‘1’已经存在用户，‘2’数据库错误
char saveNameToMysql(char *name, char *pwd)
{

}
