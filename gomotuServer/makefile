#整个服务器程序的makefile文件，这个版本将模块直接一起编译进来
#等重写之后的版本可以将其他模块以链接库的方式编译进来

MYSQLFLAG = `mysql_config --cflags --libs`
GCCFLAG = -l crypto -Wall -Werror -g -pthread
ALLOBJECT = mainAidFun.o threadPoll.o accessMysql.o encryption.o threadFun.o list.o

mainServ : mainServer.c $(ALLOBJECT)
	gcc -o mainServ mainServer.c $(ALLOBJECT) $(MYSQLFLAG) $(GCCFLAG)

threadPoll.o : threadPoll.c threadPoll.h

mainAidFun.o : mainAidFun.h mainAidFun.c

accessMysql.o : accessMysql.h accessMysql.c

encryption.o : encryption.h encryption.c

threadFun.o : threadFun.c threadFun.h

list.o : list.h list.c

.PHONY : clean

clean :
	-rm *.o
	#-rm mainServ
	-rm pri_str_key
	-rm pub_str_key
	-rm gomotu.log
