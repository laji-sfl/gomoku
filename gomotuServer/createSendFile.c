#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main()
{
	int n = 200;
   int fd = open("sendFile",O_CREAT | O_RDWR,0644);
   while(n > 0) {
	   write(fd, "1234567890qwertyuiopasdfghjklzxcvbnm", 36);
	   n--;
   }
   return 0;
}
