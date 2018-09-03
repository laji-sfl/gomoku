/*
 *	访问MySQL数据库的库函数
 *	不具有独立的移植性，只能作为五子棋服务器查询、写入玩家的信息
 */
#include "accessMysql.h"
#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>// 使用的是libmysqlclient-dev

//连接MySQL数据库，验证用户是否已经注册
// ‘0’已经注册，‘1’存在相同的用户名，‘2’数据库出现错误，‘3’密码错误
char isRegister(char *name, char *pwd, char **img_dir)
{
    if(name == NULL || pwd == NULL || img_dir == NULL)
        return '1';

    MYSQL mysql;
    MYSQL_RES *res = NULL;  //返回的结果集的指针
    MYSQL_ROW row = NULL;   //每一行的行结果集
    char *cmd = (char*)malloc(150);    // 100个字节应该够了,由调用者释放
    int rowNum = 0; // fieldNum = 0;//记录行数和列数

    //初始化MySQL对象
    if(NULL == mysql_init(&mysql)) {
        printf("mysql_init error: %s \n", mysql_error(&mysql));
		free(cmd);
        return '2';
    }

    //与MySQL服务器建立连接
    if(NULL == mysql_real_connect(&mysql, "localhost", "lmd_ubuntu", "123abc", "gomotu", 0, NULL, 0)) {
        printf("mysql_real_connect error: %s \n", mysql_error(&mysql));
		free(cmd);
        mysql_close(&mysql);
        return '2';
    }

    //设置查询的命令，命令不用加分号，多条语句需要加分号隔开，最后不用
    sprintf(cmd, "select player_name,player_pwd,player_img_dir from gomotu.playerNamePwd where player_name='%s'", name);

    //查询
    if(mysql_real_query(&mysql, cmd, strlen(cmd)) != 0) {
        printf("mysql_real_query error: %s \n", mysql_error(&mysql));
		free(cmd);
        mysql_close(&mysql);
        return '2';
    }

    //将查询的结果存储起来
    if((res = mysql_store_result(&mysql)) == NULL) {
        printf("mysql_store_result error: %s \n", mysql_error(&mysql));
		free(cmd);
        mysql_close(&mysql);
        return '2';
    }

    //获取行数,列数
    rowNum = mysql_num_rows(res);

    //没有查询到结果
    if(rowNum == 0) {
		free(cmd);
        mysql_free_result(res);
		mysql_close(&mysql);
        return '1';
    }
    else
    {
        row = mysql_fetch_row(res);//将结果的一行取出
        sprintf(cmd, "%s", row[1]);//将密码暂时存储起来

        if(strcmp(cmd, pwd) == 0) {
            sprintf(cmd, "%s", row[2]); //将文件的目录名传出
            *img_dir = cmd;
            mysql_free_result(res);     //和我的函数机制差不多，返回的指针还需要释放
            mysql_close(&mysql);
            return '0';     //成功
        }
        else {
            mysql_free_result(res);
            mysql_close(&mysql);
			free(cmd);
            return '3';    //密码错误
        }
    }
}


//连接MySQL数据库，将用户名和密码存储起来
// ‘0’成功，‘1’已经存在用户，‘2’数据库错误
char saveNameToMysql(char *name, char *pwd, char *img_dir)
{
    if(name == NULL || pwd == NULL || img_dir == NULL)
        return '1';

    MYSQL mysql;
    MYSQL_RES *res = NULL;
    char cmd[300] = {0};

    //查询是否用户名已经注册
    if(NULL == mysql_init(&mysql)) {
        printf("mysql_init error: %s \n", mysql_error(&mysql));
        return '2';
    }

    if(NULL == mysql_real_connect(&mysql, "localhost", "lmd_ubuntu", "123abc", "gomotu", 0, NULL, 0)) {
        printf("mysql_real_connect error: %s \n", mysql_error(&mysql));
        mysql_close(&mysql);
        return '2';
    }

    //先查询是否有相同的用户名
    sprintf(cmd, "select player_name from gomotu.playerNamePwd where player_name='%s'", name);

    if(mysql_real_query(&mysql, cmd, strlen(cmd)) != 0) {
        printf("mysql_real_query error: %s \n", mysql_error(&mysql));
        mysql_close(&mysql);
        return '2';
    }

    if((res = mysql_store_result(&mysql)) == NULL) {
        printf("mysql_store_result error: %s \n", mysql_error(&mysql));
        mysql_close(&mysql);
        return '2';
    }

    //表示已经存在用户名
    if(mysql_num_rows(res) != 0) {
        mysql_free_result(res);
        mysql_close(&mysql);
        return '1';
    }
    else {
        mysql_free_result(res);
        res = NULL;
    }

	//在高并发的条件下，可能在查询和插入之间，有其他的用户插入相同的用户名，因为数据库也做了唯一的约束，所以会返回错误
    //执行插入操作
    sprintf(cmd, "insert into gomotu.playerNamePwd(player_name,player_pwd,player_img_dir) values('%s','%s','%s')", name, pwd, img_dir);

    if(mysql_real_query(&mysql, cmd, strlen(cmd)) != 0) {
        printf("mysql_real_query error: %s \n", mysql_error(&mysql));
        mysql_close(&mysql);
        return '2';
    }
    else {
        //执行成功就返回
        mysql_close(&mysql);
        return '0';
    }
}


//更新信息
char updateUserMsg(char *name, char *pwd, char *img_dir)
{
    if(name == NULL)
        return '1';

    MYSQL mysql;
    MYSQL_RES *res = NULL;
    char cmd[300] = {0};
	int affectNum = 0;

    if(NULL == mysql_init(&mysql)) {
        printf("mysql_init error: %s \n", mysql_error(&mysql));
        return '1';
    }

    if(NULL == mysql_real_connect(&mysql, "localhost", "lmd_ubuntu", "123abc", "gomotu", 0, NULL, 0)) {
        printf("mysql_real_connect error: %s \n", mysql_error(&mysql));
        mysql_close(&mysql);
        return '1';
    }

    //根据姓名来查询到记录然后更新
    if(img_dir == NULL) {//只更新密码
        sprintf(cmd, "update gomotu.playerNamePwd set player_pwd='%s' where player_name='%s'", pwd, name);
    }
    else if(pwd == NULL){//只更新图片
    sprintf(cmd, "update gomotu.playerNamePwd set player_img_dir='%s' where player_name='%s'", img_dir, name);
    }

    if(mysql_real_query(&mysql, cmd, strlen(cmd)) != 0) {
        printf("mysql_real_query error: %s \n", mysql_error(&mysql));
        mysql_close(&mysql);
        return '1';
	}
	
	//根据返回的影响的行数来判断是否更新成功
	affectNum = mysql_affected_rows(&mysql);
	if (affectNum == 1) {
		mysql_close(&mysql);
		return '0';
	}
	else {
		mysql_close(&mysql);
		return '1';
	}
}

//返回‘0’表示合法的，否则返回‘1’,因为将数据库不在存储明文了，所以也不怕数据不合法了，都是随机的MD5值
//char isLegal(char *name)
//{
//    int flag = '1';
//    while(*name != '\0') {
//        if((*name >= '0' && *name <= '9') || (*name >= 'a' && *name <= 'z') || (*name >= 'A' && *name <= 'Z')) {
//            flag = '0';
//        }
//        else flag = '1';
//        ++name;
//    }
//    return flag;
//}
