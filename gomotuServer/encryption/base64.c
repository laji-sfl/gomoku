/*
 *	base64编码的测试文件
 *	经过测试解码时一次最多解码出48字节
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
 
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
 
int base64_encode(char *in_str, int in_len, char *out_str)
{
    BIO *b64, *bio;
    BUF_MEM *bptr = NULL;
    size_t size = 0;
 
    if (in_str == NULL || out_str == NULL)
        return -1;
 
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
 
    BIO_write(bio, in_str, in_len);
    BIO_flush(bio);
 
    BIO_get_mem_ptr(bio, &bptr);
    memcpy(out_str, bptr->data, bptr->length);
    out_str[bptr->length] = '\0';
    size = bptr->length;
 
    BIO_free_all(bio);
    return size;
}
 
int base64_decode(char *in_str, int in_len, char *out_str)
{
    BIO *b64, *bio;
    int size = 0;
 
    if (in_str == NULL || out_str == NULL)
        return -1;
 
    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
 
    bio = BIO_new_mem_buf(in_str, in_len);
    bio = BIO_push(b64, bio);
 
    size = BIO_read(bio, out_str, in_len);
    out_str[size] = '\0';
 
    BIO_free_all(bio);
    return size;
}
 
int main(void)
{
	char instr[256] = {0};
	char outstr1[1024] = {0};
//	char bejiema[1024] = {0};
	char outstr2[1024] = {0};
	char *p = outstr1;
	char *q = outstr2;
	int base64_en = 0;// i = 1;

	for(int i = 0;i < 256; ++i) {
		instr[i] = (i % 20) + 'a';
	}


	base64_en = base64_encode(instr,strlen(instr),outstr1);
	printf("encode = %d\n", base64_en);
	printf("base64:%s\n",outstr1);
 
	/*
	bejiema[0] = outstr1[0];
	while (*p != '\0') {
		bejiema[i] = *p;
		if (i % 65 == 0) {
			bejiema[++i] = '\r';
			bejiema[++i] = '\n';
		}
		i++;
		p++;
	}*/

	printf("decode = %d\n",	base64_decode(p,65,q));
	printf("decode = %d\n",	base64_decode(p+65,65,q+48));
	printf("decode = %d\n",	base64_decode(p+65*2,65,q+48*2));
	printf("decode = %d\n",	base64_decode(p+65*3,65,q+48*3));
	printf("decode = %d\n",	base64_decode(p+65*4,65,q+48*4));
	printf("decode = %d\n",	base64_decode(p+65*5,base64_en - 65*5,q+48*5));
	printf("str:%s\n",outstr2);

	return 0;
}
