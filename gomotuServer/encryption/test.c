#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encryption.h"

int main(void)
{
	char jiamiqian[2048]  = "laji000000000000laji000000000000";
	char myaes_Key[16] = "4Dskdj3241DEAe4";
	char miwen[2048]  = {0};
	char jiemihou[2048]  = {0};
	char bianmahou[2048] = {0};
	char jiemahou[2048] = {0};
	char *buf;
//	char *pubkey = NULL;
//	char *prikey = NULL;
	int ret = 0 ;

//	createRSAkey();
//	readRSAKey("./pub_str_key", &pubkey);
//	ret = pubcrypt(pubkey, jiamiqian, miwen);

//	base64_encode(miwen, ret, bianmahou);

//	readRSAKey("./pri_str_key", &prikey);
//	base64_decode(bianmahou, strlen(bianmahou), jiemahou);

//	pricrypt(prikey, jiemahou, jiemihou );

	//输出解密后的明文
//	printf("\nmingwen:%s\n", jiemihou);

//	free(pubkey);
//	free(prikey);

	ret = aesCrypt(jiamiqian, miwen, myaes_Key);
	printf("len = %d, strlen = %d\n", ret, strlen(miwen));
	printf("base ret = %d\n",base64_encode(miwen, ret, bianmahou));
//	aesDecrypt(miwen, jiemihou, "1234qwer");
//	printf("mingwen:%s\n", jiemihou);

	buf = (char *)malloc(2048);
	ret = base64_decode(bianmahou, strlen(bianmahou), jiemahou);	
	printf("aes len = %d\n",aesDecrypt(jiemahou, buf+1, myaes_Key, ret));			//对密文解密
	printf("buf = %s\n", buf+1);

	return 0;
}
