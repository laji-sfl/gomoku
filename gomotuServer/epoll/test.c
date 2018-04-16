#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

void sslog(char *str)
{
	time_t rawtime;
    struct tm *tim;
	int fd = 0;
	char buf[2050] = {0};

	//打开日志文件
	fd = open("gomotu.log", O_RDWR | O_CREAT | O_APPEND, 0664);

	//获取当前时间，并格式化写入文件的内容
	time(&rawtime);
    tim = localtime(&rawtime);

    sprintf(buf, "Time:%d月%d日-%d:%d:%d,log:%s\n", tim->tm_mon+1,
            tim->tm_mday, tim->tm_hour, tim->tm_min, tim->tm_sec, str);

    write(fd, buf, strlen(buf));
    close(fd);
}

int main()
{
	char buf[100] = {0};
	while(1){
	read(1, buf, 100);
	sslog(buf);
	}
		
	return 0;
}
