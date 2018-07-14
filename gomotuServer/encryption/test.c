#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/md5.h>
#include <openssl/aes.h>
#include <openssl/crypto.h>

#define KEY_LEN 2048    //秘钥长度

void readPubKey(char *str, char **pubkey)
{
	int fd = 0;
    char *pubstr = (char *)malloc(KEY_LEN / 2);
    *pubkey = pubstr;

    fd = open("./pri_str_key", O_RDWR);
	read(fd, pubstr, KEY_LEN / 2);
	close(fd);
}

void readPriKey(char *str, char **prikey)
{
	int fd = 0;
    char *pristr = (char *)malloc(KEY_LEN);
    *prikey = pristr;

    fd = open("./pub_str_key", O_RDWR);
	read(fd, pristr, KEY_LEN);
	close(fd);
}
void createRSAkey()
{
	int fdu, fdr;
    RSA *keypair = RSA_generate_key(KEY_LEN, RSA_3, NULL, NULL);

    BIO *pri = BIO_new(BIO_s_mem());
    BIO *pub = BIO_new(BIO_s_mem());

    PEM_write_bio_RSAPrivateKey(pri, keypair, NULL, NULL, 0, NULL, NULL);
    PEM_write_bio_RSAPublicKey(pub, keypair);

    size_t pri_len = BIO_pending(pri);
    size_t pub_len = BIO_pending(pub);

    char *pri_str = (char *)malloc(pri_len + 1);
    char *pub_str = (char *)malloc(pub_len + 1);

    BIO_read(pri, pri_str, pri_len);
    BIO_read(pub, pub_str, pub_len);

    pri_str[pri_len] = '\0';
    pub_str[pub_len] = '\0';

//	printf("%s", pri_str);
//	printf("%s", pub_str);

    fdu = open("./pri_str_key", O_RDWR | O_CREAT, 0664);
	fdr = open("./pub_str_key", O_RDWR | O_CREAT, 0664);
	write(fdu, pub_str, pub_len);
	write(fdr, pri_str, pri_len);
	close(fdr);
	close(fdu);

    RSA_free(keypair);
    BIO_free_all(pub);
    BIO_free_all(pri);

    free(pri_str);
    free(pub_str);
}

void pubcrypt(char *pubKey, char *plainText, char *ciphetext)
{
	printf("pub=%s\n", pubKey);
	printf("plain=%s\n", plainText);
	printf("cip=%s\n", ciphetext);

    BIO *keybio = BIO_new_mem_buf((unsigned char *)pubKey, -1);
    RSA *rsa = RSA_new();
    rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);

    int len = RSA_size(rsa);
    printf("len=%d\n", len);
    char *beEncrypt = (char *)malloc(len + 1);
    memset(beEncrypt, 0, len + 1);

    int ret = RSA_public_encrypt(strlen(plainText), (unsigned char *)plainText,
                       (unsigned char *)beEncrypt, rsa, RSA_PKCS1_PADDING);
    printf("ret=%d\n", ret);
    memcpy(ciphetext, beEncrypt, ret);
    printf("ciphetext: %s \n", ciphetext);

    free(beEncrypt);
    BIO_free_all(keybio);
    RSA_free(rsa);
}
void getMD5(char *plainText, char *out)
{
    MD5((unsigned char *)plainText, strlen(plainText), (unsigned char *)out);
}
void pricrypt(char *priKey, char *ciphetext, char *plaintext)
{
    RSA *rsa = RSA_new();
    BIO *keybio;
    keybio = BIO_new_mem_buf((unsigned char *)priKey, -1);

    rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);

    int len = RSA_size(rsa);
    char *beDecrypt = (char *)malloc(len + 1);
    memset(beDecrypt, 0, len + 1);

    int ret = RSA_private_decrypt(strlen(ciphetext), (unsigned char*)ciphetext,
                                  (unsigned char*)beDecrypt, rsa,
                                  RSA_PKCS1_PADDING);
    memcpy(plaintext, beDecrypt, ret);
	printf("plaintext: %s\n", plaintext);
    free(beDecrypt);
    BIO_free_all(keybio);
    RSA_free(rsa);
}

int main(void)
{
/*
	createRSAkey();
	char *pub = NULL, *pri = NULL;
	readPubKey("./pub_key_str", &pub);
	//printf("%s", pub);
	readPriKey("./pri_key_str", &pri);
	//printf("%s", pri);
	
	char buf[1024] = "qwertyuiop";
	char bufout[1024] = {0};
	printf("kai shi jia mi\n");
	pubcrypt(pub, buf, bufout);
	printf("jia mi hou : %s\n", bufout);
	pricrypt(pri, bufout, buf);
	printf("buf: %s \n", buf);
	printf("bufout: %s \n", bufout);
*/
	
	char buf[1024] = "hello";
	char out[16] = {0};
	getMD5(buf, out);
	printf("%s", out);
	
	
	return 0;
}
