#include "encryption.h"
/*
 * 加密接口的实现
 */

void readRSAKey(char *str, char **key)
{
    int fd = 0;
    char *tmp = (char *)malloc(KEY_LEN);
    memset(tmp, 0, KEY_LEN);
    *key = tmp;

    fd = open(str, O_RDONLY);
    read(fd, tmp, KEY_LEN);
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

    //写入磁盘
    fdu = open("./pub_str_key", O_RDWR | O_CREAT | O_TRUNC, 0664);
    fdr = open("./pri_str_key", O_RDWR | O_CREAT | O_TRUNC, 0664);
    write(fdu, pub_str, pub_len);
    write(fdr, pri_str, pri_len);
    close(fdr);
    close(fdu);

    //释放内存
    RSA_free(keypair);
    BIO_free_all(pub);
    BIO_free_all(pri);
    free(pri_str);
    free(pub_str);
}

int pubcrypt(char *pubKey, char *plainText, char *ciphetext)
{
    BIO *keybio = BIO_new_mem_buf(pubKey, strlen(pubKey));
    RSA *rsa = RSA_new();
    PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);

    int len = RSA_size(rsa);
    char *beEncrypt = (char *)malloc(len + 1);
    memset(beEncrypt, 0, len + 1);

    int ret = RSA_public_encrypt(strlen(plainText), (unsigned char *)plainText,
                       (unsigned char *)beEncrypt, rsa, RSA_PKCS1_PADDING);
    memcpy(ciphetext, beEncrypt, ret);

    free(beEncrypt);
    BIO_free_all(keybio);
    RSA_free(rsa);

	return ret;	//加密后的长度
}

void pricrypt(char *priKey, char *ciphetext, char *plaintext)
{
    BIO *keybio = BIO_new_mem_buf(priKey, strlen(priKey));
    RSA *rsa = RSA_new();
    PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);

    int len = RSA_size(rsa);
    char *beDecrypt = (char *)malloc(len + 1);
    memset(beDecrypt, 0, len + 1);

    int ret = RSA_private_decrypt(len, (unsigned char*)ciphetext,
                                  (unsigned char*)beDecrypt, rsa,
                                  RSA_PKCS1_PADDING);
    memcpy(plaintext, beDecrypt, ret);

    free(beDecrypt);
    BIO_free_all(keybio);
    RSA_free(rsa);
}

void getMD5(char *plainText, char *out)
{
    MD5((unsigned char *)plainText, strlen(plainText), (unsigned char *)out);
}

int aesCrypt(char *plainText, char *ciphetext, char *key)
{
    AES_KEY aes;

    AES_set_encrypt_key((unsigned char *)key, 128, &aes);

    //循环的将所有数据加密
    int plaintextLen = strlen(plainText);
    int encryptLen = 0; //已经加密的长度

    while(encryptLen < plaintextLen)
    {
        AES_encrypt((unsigned char *)plainText, (unsigned char *)ciphetext, &aes);
        plainText += AES_BLOCK_SIZE;    //明文、密文、已加密的长度都加上加密块的大小
        ciphetext += AES_BLOCK_SIZE;
        encryptLen += AES_BLOCK_SIZE;
    }
	return encryptLen;
}

int aesDecrypt(char *ciphetext, char *plaintext, char *key, int ciphelen)
{
    AES_KEY aes;
    AES_set_decrypt_key((unsigned char *)key, 128, &aes);

//  int ciphetextLen = strlen(ciphetext);
    int ciphetextLen = ciphelen;
//	printf("miwen len aes = %d\n", ciphetextLen);
    int decryptLen = 0; //已经加密的长度

    while(decryptLen < ciphetextLen)
    {
        AES_decrypt((unsigned char *)ciphetext, (unsigned char *)plaintext, &aes);
        plaintext += AES_BLOCK_SIZE;    //明文、密文、已加密的长度都加上加密块的大小
        ciphetext += AES_BLOCK_SIZE;
        decryptLen += AES_BLOCK_SIZE;
    }

	return decryptLen;
}

//base64解码
int base64_decode_help(char *in_str, int in_len, char *out_str)
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

int base64_decode(char *in_str, int in_len, char *out_str)
{
	//由于不了解openssl库的实现原理，所以根据自己的测试发现，解码只能一次从65字节解码出48字节
	char *in = in_str, *out = out_str;
	int len = in_len, i = 0, ret = 0;

	while (len > 0) {
		if (len > 65) {
			ret += base64_decode_help(in + i * 65, 65, out + i * 48);
		}
		else {
			ret += base64_decode_help(in + i * 65, in_len - i * 65, out + i * 48);
		}
		i++;	//每次执行 in向后偏移 65×i 字节，out 48×i 字节
		len -= 65;				//每执行一次少65字节
	}

	return ret;
}

//base64编码
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

