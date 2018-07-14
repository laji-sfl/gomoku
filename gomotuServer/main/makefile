#整个服务器程序的makefile文件，这个版本将模块直接一起编译进来
#等重写之后的版本可以将其他模块以链接库的方式编译进来

mainServ : mainServer.c mainAidFun.o threadPoll.o accessMysql.o encryption.o threadFun.o list.o
	gcc -o mainServ mainServer.c mainAidFun.o threadPoll.o accessMysql.o encryption.o threadFun.o list.o `mysql_config --cflags --libs` -l crypto -Wall -pthread -g

threadPoll.o : threadPoll.c threadPoll.h


mainAidFun.o : mainAidFun.h mainAidFun.c


accessMysql.o : accessMysql.h accessMysql.c


encryption.o : encryption.h encryption.c


threadFun.o : threadFun.c threadFun.h


list.o : list.h list.c


clean :
	-rm *.o
	-rm mainServ
