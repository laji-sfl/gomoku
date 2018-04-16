mysql_plm : testMain.o accessMysql.o
	gcc -o mysql_plm testMain.o accessMysql.o `mysql_config --cflags --libs` -Wall -g

testMain.o : testMain.c

accessMysql.o : accessMysql.h accessMysql.c

clean :
	-rm accessMysql.o
	-rm testMain.o
	-rm mysql_plm
