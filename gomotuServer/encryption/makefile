plm : encryption.o test.c
	gcc encryption.o test.c -o plm -g -Wall -lcrypto

encryption.o : encryption.c encryption.h

.PHONY : clean

clean : 
	rm plm encryption.o pri_str_key pub_str_key -f
