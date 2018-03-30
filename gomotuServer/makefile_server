mainServ : mainServe.o threadFun.o list.o
	gcc -o mainServ mainServe.o threadFun.o list.o -Wall -g -pthread

threadFun.o : threadFun.c threadFun.h

list.o : list.c list.h

mainServe.o : mainServe.c

clean :
	-rm *.o
	-rm mainServ