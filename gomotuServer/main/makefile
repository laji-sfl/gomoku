#整个服务器程序的makefile文件，这个版本将模块直接一起编译进来
#等重写之后的版本可以将其他模块以链接库的方式编译进来

mainServ : mainServer.c mainAidFun.o threadPoll.o
	gcc -o mainServ mainServer.c mainAidFun.o threadPoll.o -g -Wall -pthread

threadPoll.o : threadPoll.c
	cc -o threadPoll.o -c threadPoll.c -L threadPoll.h -pthread

mainAidFun.o : mainAidFun.h mainAidFun.c

clean :
	-rm *.o
	-rm mainServ
