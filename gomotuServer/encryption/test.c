#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encryption.h"

int main(void)
{
	char buf[2048]  = "hello";
	char tmp[2048]  = {0};
	char jjj[2048] = {0};
	jjj[0] = '1';
	char plain[2048]  = {0};
	char *msg = NULL;
	char *key = NULL;

	createRSAkey();

	readRSAKey("./pub_str_key", &key);
	printf("pub len %d\n", strlen(key));

	pubcrypt(key, buf, tmp);

	readRSAKey("./pri_str_key", &msg);
	printf("pri len %d\n", strlen(msg));

	//pricrypt(msg, jjj+1, plain);
	pricrypt(msg, tmp, plain);

	printf("\n%s\n", plain);


	return 0;
}
