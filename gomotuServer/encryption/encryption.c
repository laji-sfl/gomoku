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

void pubcrypt(char *pubKey, char *plainText, char *ciphetext)
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
}

void pricrypt(char *priKey, char *ciphetext, char *plaintext)
{
    printf("ljai 1");
    BIO *keybio = BIO_new_mem_buf(priKey, strlen(priKey));
    printf("ljai 2");
    RSA *rsa = RSA_new();
    printf("ljai 3");
    PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
    printf("ljai 4");

    int len = RSA_size(rsa);
    char *beDecrypt = (char *)malloc(len + 1);
    memset(beDecrypt, 0, len + 1);

    int ret = RSA_private_decrypt(strlen(ciphetext), (unsigned char*)ciphetext,
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

void aesCrypt(char *plainText, char *ciphetext, char *key)
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
}

void aesDecrypt(char *ciphetext, char *plaintext, char *key)
{
    AES_KEY aes;
    AES_set_decrypt_key((unsigned char *)key, 128, &aes);

    int ciphetextLen = strlen(ciphetext);
    int decryptLen = 0; //已经加密的长度

    while(decryptLen < ciphetextLen)
    {
        AES_decrypt((unsigned char *)ciphetext, (unsigned char *)plaintext, &aes);
        plaintext += AES_BLOCK_SIZE;    //明文、密文、已加密的长度都加上加密块的大小
        ciphetext += AES_BLOCK_SIZE;
        decryptLen += AES_BLOCK_SIZE;
    }
}
