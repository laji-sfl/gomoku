/*
 *	实现客户端与服务器之间的通信数据的简单加密
 *  利用openssl库中的加密模块来实现
 */
#include <stdio.h>
#include <string.h>
#include <openssl/des.h>
#include <openssl/pkcs7.h>
#include <openssl/md5.h>
#include <openssl/aes.h>
#include <openssl/crypto.h>

/*使用MD5进行摘要计算*/
/*  共需要学习四个函数一般只用三个
 *
 *  MD5_init(),初始化需要使用到的数据结构，一般的C语言函数都是这种模式
 *  MD5_update(),循环的调用这个函数可以将需要计算的数据累加计算
 *  MD5_final(),输出计算后的结果
 *
 *  MD5(),一个函数直接计算出结果
 */
void md5_test()
{
    //初始化变量
    MD5_CTX ctx;
    unsigned char outmd5[17] = {0};//输出结果不是字符串，会乱码
                                   //是一个十六进制的数值

    //初始化MD5函数
    MD5_Init(&ctx);

    //添加数据进行计算
    MD5_Update(&ctx, "hel", 3); //参数依次是：结构体指针，数据字符串，字符串长度
    MD5_Update(&ctx, "lo", 2);  //同一个结构体的所有数据累加到一起计算，文件就是读一句调用算一次

    //得出最终结果
    MD5_Final(outmd5, &ctx);    //参数：传出指针，结构体指针
    for(int i = 0;i < 16; ++i)
    {
        printf("%02x ", outmd5[i]);
    }
    printf("\n");


    //一次计算出的函数
    memset(outmd5, 0, 17);
    MD5((unsigned char *)"hello", 5, outmd5);
    for(int i = 0;i < 16; ++i)
    {
        printf("%02x ", outmd5[i]);
    }
}

/*使用SHA1算法的计算函数与MD5相同，计算结果是160位，20字节，函数接口与MD5相似*/
/*
 *  SHA_init, SHA_update, SHA_final, SHA_transfrom, 也可以将SHA换成SHA256等其他相似算法
 */

/*DES对称秘钥加密*/
/*
 *  函数接口介绍：
 */

/*AES对称加密加密*/
/*
 *  函数接口介绍：
 *  输入的数据中如果包含'\0'，就只会加密解密之前的内容
 *  aes_set_encrypt_key(),设置加密秘钥
 *  AES_set_decrypt_key(),设置解密秘钥
 *  aes_encrypt(), 加密
 *  AES_decrypt(), 解密
 *  AES_cbc_encrypt();//cbc模式，与前面的函数相同的应用，参数直接写出来就可以了
 */
//加密函数
void aes_encrypt_test(char *plaintext, char *ciphetext, char *key)
{
    if(plaintext == NULL || ciphetext == NULL || key == NULL)
        return ;

    //初始化变量
    AES_KEY aes;

    //设置秘钥
    AES_set_encrypt_key((unsigned char *)key, 128, &aes);

    //循环的将所有数据加密
    int plaintextLen = strlen(plaintext);
    int encryptLen = 0; //已经加密的长度

    while(encryptLen < plaintextLen)
    {
        AES_encrypt((unsigned char *)plaintext, (unsigned char *)ciphetext, &aes);
        plaintext += AES_BLOCK_SIZE;    //明文、密文、已加密的长度都加上加密块的大小
        ciphetext += AES_BLOCK_SIZE;
        encryptLen += AES_BLOCK_SIZE;
    }
}
//解密函数
void aes_decrypt_test(char *plaintext, char *ciphetext, char *key)
{
    if(plaintext == NULL || ciphetext == NULL || key == NULL)
    {
        printf("laji\n");
        return ;
    }

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

int main()
{
//   md5_test();

//    char in[160] = "hellosdasdasdasfasasfasdsadsadasdasdsadsadsadsadsadas";
//    char key[17] = "laji1234jila3241";
//    char out[160] = {0};

//    memset(out, 0, 160);
//    aes_encrypt_test(in, out, key);
//    printf("jia mi hou: %s\n", out);

//    memset(in, 0, 160);
//    aes_decrypt_test(in, out, key);
//    printf("jie mi hou: %s\n", in);

    return 0;
}
