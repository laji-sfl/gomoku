#ifndef SDD_H
#define SDD_H
/*
 *   加密模块的接口
 */

#ifdef __cplusplus
extern "C" {
#endif

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
void readRSAKey(char *str, char **pubkey);

//用公钥加密
int pubcrypt(char *pubKey, char *plainText, char *ciphetext);

//用私钥解密
void pricrypt(char *priKey, char *ciphetext, char *plaintext );

//获取md5信息
void getMD5(char *plainText, char *out);

//aes加密
int aesCrypt(char *plainText, char *ciphetext, char *key);

//aes解密
int aesDecrypt(char *ciphetext, char *plaintext, char *key, int ciphelen);

//base64编码
int base64_encode(char *in_str, int in_len, char *out_str);

//base64解码
int base64_decode(char *in_str, int in_len, char *out_str);
int base64_decode_help(char *in_str, int in_len, char *out_str);

#ifdef __cplusplus
}
#endif

#endif // SDD_H
