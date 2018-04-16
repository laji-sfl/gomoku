/*
 * mysql 模块的测试程序
 */
#include <stdio.h>
#include <stdlib.h>
#include "accessMysql.h"

int main(int argc, char *argv[])
{
	printf("验证用户是否可以登录，1,需输入用户名，密码，程序输出结果\n");
	printf("注册用户需要输入,2,用户名，密码，头像路径\n");
	printf("修改信息输入,3,用户名，需要修改的内容（密码或头像路径）,不需要修改的向用0代替。\n");

	if (argc < 2)
		return 0;
	char *p = argv[1];

	switch(*p)
	{
	case '1':
		if(argc != 4)
			return 0;
		char *img_dir = NULL;
		switch(isRegister(argv[2], argv[3], &img_dir))
		{
		case '0':
			printf("允许登录，头像路径为:%s\n", img_dir);
			free(img_dir);
			break;
		case '1':
			printf("用户没有注册\n");
			break;
		case '2':
			printf("数据库错误\n");
			break;
		case '3':
			printf("密码错误\n");
			break;
		}
		break;
	case '2':
		if(argc != 5)
			return 0;
		switch(saveNameToMysql(argv[2], argv[3], argv[4]))
		{
		case '0':
			printf("注册成功\n");
			break;
		case '1':
			printf("已经存在用户名\n");
			break;
		case '2':
			printf("数据库错误\n");
			break;
		}
		break;
	case '3':
		if(argc != 5)
			return 0;
		char *k = argv[4];
		if(*k == '0') {
			if('0' == updateUserMsg(argv[2], argv[3], NULL))
				printf("修改密码成功\n");
			else
				printf("修改密码失败\n");
		}
		else {
			if('0' == updateUserMsg(argv[2], NULL,argv[4]))
				printf("修改路经成功\n");
			else
				printf("修改路径失败\n");
		}
		break;
	default:
		printf("只能是1,2,3\n");
		break;
	}
	

	return 0;
}
