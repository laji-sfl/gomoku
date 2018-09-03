/*
 *	数据库操作的头文件
 *  编译MySQL的库函数的时候，需要加上gcc的参数 `mysql_config --cflags --libs`
 */

#ifndef __ACCESSMYSQL_H__
#define __ACCESSMYSQL_H__

/*
 * 	数据库名gomotu，数据表名playerNamePwd，数据列player_name,player_pwd,player_img_dir
 * 	数据库中name和pwd都是编码后的数据，不是明文
 */

//连接MySQL数据库，验证用户是否已经注册
// ‘0’已经注册，‘1’不存在，‘2’数据库出现错误，‘3’密码错误
// 传入的指针img_dir需要调用者释放
char isRegister(char *name, char *pwd, char **img_dir);


//连接MySQL数据库，将用户名和密码存储起来
// ‘0’成功，‘1’已经存在用户，‘2’数据库错误
char saveNameToMysql(char *name, char *pwd, char *img_dir);


//更新用户的信息,不能修改用户名，只能该密码，换头像
//成功返回‘0’，失败返回‘1’
char updateUserMsg(char *name, char *pwd, char *img_dir);


//检测字符串的合法性
//char isLegal(char *name);

#endif
