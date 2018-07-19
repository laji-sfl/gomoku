#ifndef SDD_H
#define SDD_H

#ifdef __cplusplus
extern "C"{
#endif

/*
 *   加密模块的接口
 */
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

//创建一个RSA秘钥对存储在磁盘上
void createRSAkey();

//读取磁盘上的公私秘钥,二级指针传出需要释放内存
void readRSAKey(char *str, char **key);

//用公钥加密
void pubcrypt(char *pubKey, char *plainText, char *ciphetext);

//用私钥解密
void pricrypt(char *priKey, char *ciphetext, char *plaintext);

//获取md5信息
void getMD5(char *plainText, char *out);

//aes加密
void aesCrypt(char *plainText, char *ciphetext, char *key);

//aes解密
void aesDecrypt(char *ciphetext, char *plaintext, char *key);

#ifdef __cplusplus
}
#endif

#endif // SDD_H
