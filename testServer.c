#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

int main(void)
{
	struct sockaddr_in serv_sock;	//server网络地址结构
	struct sockaddr_in clie_sock;	//client网络地址结构
	int serv_ret, clie_ret;	//套接字描述符
	size_t ret_size;	//客户端地址结构的长度
	char buf[1024] = {0};
	int i = 0, j = 0;

	//创建服务器套接字
	serv_ret = socket(AF_INET, SOCK_STREAM, 0);

	//初始化地址结构
	bzero(&serv_sock, sizeof(serv_sock));
	bzero(&clie_sock, sizeof(serv_sock));
	serv_sock.sin_family = AF_INET;
	serv_sock.sin_addr.s_addr = htonl(INADDR_ANY);	//网络地址
	serv_sock.sin_port = htons(9996);

	bind(serv_ret, (struct sockaddr *)&serv_sock, sizeof(struct sockaddr));
	listen(serv_ret, 2);

	ret_size = sizeof(struct sockaddr_in);
	while(1)
	{
		clie_ret = accept(serv_ret, (struct sockaddr *)&clie_sock, &ret_size);
		printf("connected : %s \n", inet_ntoa(clie_sock.sin_addr));
		while(1)
		{
			int len = read(clie_ret, buf, 1024);
			if(len <= 0)
				break;
			
			printf("recv:%s\n",buf);
			if(buf[0] == '2')
				write(clie_ret, "21hel", 5);
			else if(buf[0] == '4')
			{
				if(i == 0)
				{
					i++;
					write(clie_ret, buf, len);
				}
				else
				{
					sleep(15);
					memset(buf,0,100);
					sprintf(buf, "40%d0%d",i++,j++);
					write(clie_ret, buf, 5);
				}
			}
			memset(buf,0,100);
		}
	}

	close(clie_ret);	//父进程
	return 0;
}
