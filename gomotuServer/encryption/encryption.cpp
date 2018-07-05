/*
 *	实现客户端与服务器之间的通信数据的简单加密
 *  利用openssl库中的加密模块来实现
 *
 *  学习加密库的难点：
 *      1、对于加密的原理不清楚，所以难以理解库函数的schedule，向量，等名称表示的是什么意思，不
 *  理解加密的字节长度要求的原因，造成学习库函数调用的时候，不知该调用那个函数，调用顺序是什么？
 *      2、英文水平不过关，阅读官方文档有障碍。
 *      3、查询Google发现没有完整的论述，只是写出了某种加密方法调用，难以理解（没有写出我不懂的）
 *  总结：
 *      1、从整体上了解对称加密和非对称加密，还有摘要的区别与实现
 *      2、了解到对称加密的不同方法还有不同模式的区别，但是整体的加密方法是不变的
 *      3、需要将以上问题及其答案总结出来，方便以后复习（加密算法的分类、原理、常用算法的应用）
 */
#include <stdio.h>
#include <string.h>
#include <openssl/des.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
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
 *  加密是块加密，需要将每次加密的文本长度限制在8字节（不同模式可能不同）。
 *  函数接口介绍：
 *  DES_random_key(),首先调用生成随机的key schedule
 *  DES_set_key_checked(),设置key，并检查奇偶校验位
 *  DES_ecb_encrypt(),加密，解密靠参数决定，ecb模式输入输出必须是8字节，需要自己分，
 *      密码也是8字节
 *  DES_ncbc_encrypt(),输入可以不是8字节的倍数，但是输出一定是，函数内部会填充，密码是8字节
 *  DES_set_key_unchecked,不检查奇偶校验
 *  DES_string_to_key,根据string生成一个随机的秘钥，秘钥8字节
 */
void des_encrypt_test(/*ecb模式，没有设置密码，没有对长字符串进行分解*/)
{
    //初始变量
    DES_cblock key;
    DES_key_schedule schedule;

    //设置密码key
    DES_random_key(&key);
    DES_set_key_checked(&key, &schedule);

    DES_cblock input = "hello", output; //类型为8字节的字节数组

    //加解密
    DES_ecb_encrypt(&input, &output, &schedule, DES_ENCRYPT);
    printf("jia mi hou: %s\n", output);
    DES_ecb_encrypt(&output, &input, &schedule, DES_DECRYPT);
    printf("jie mi hou: %s\n", input);
}

//cbc模式，自己设置密码，对长于8字节的原文本加解密
void des_encrypt_test2(unsigned char *plaintext,
                       unsigned char *ciphetext,
                       char *key)
{
    //初始化变量
    DES_cblock desKey;
    DES_cblock ivec;    //向量
    DES_key_schedule schedule;

    //根据密码来生成一个可以用来加密的key
    DES_string_to_key(key, &desKey);//密码长度为8字节（最多，自己的那个密码字符串可以更长）
    DES_set_key_unchecked(&desKey, &schedule);

    //设置向量值
    memset(&ivec, 0, 8);

    //test
    unsigned char in[16] = "helloooasdq";
    unsigned char out[16] = {0};
    unsigned char jiemihou[16] = {0};

    //加密
    DES_ncbc_encrypt(in, out, 16, &schedule, &ivec, DES_ENCRYPT);
    printf("jiamihou: %s\n", out);

    //解密
    memset(&ivec, 0, 8);        //当将向量置为最初的值时，可以解密出明文？？
    //帮助文档说ncbc会改变向量，而cbc不会（cbc是bug）
    DES_ncbc_encrypt(out, jiemihou, 16, &schedule, &ivec, DES_DECRYPT);
    printf("jiemihou: %s\n", jiemihou);
}

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

/*RSA非对称加密*/
/*
 *  生成一个公钥和私钥，公钥加密私钥解密，私钥加密公钥解密（就是签名）
 *  函数接口：
 *      RSA_generate_key(),产生一个秘钥对
 *      不了解BIO是什么和BIO操作的作用和方法
 */
//创建一个秘钥对,写入本地文件
void generateRSAkey(char *pubOut, char *priOut)
{
#define KEY_LEN 2048    //秘钥长度

    //产生一个秘钥对,不了解第2、3、4个参数
    RSA *keypair = RSA_generate_key(KEY_LEN, RSA_3, NULL, NULL);
//    RSA_generate_key_ex( (??如何定义变量), 2048, RSA_3, NULL);

    //不懂，这时什么接口函数
    BIO *pri = BIO_new(BIO_s_mem());
    BIO *pub = BIO_new(BIO_s_mem());

    //不懂函数原理
    PEM_write_bio_RSAPrivateKey(pri, keypair, NULL, NULL, 0, NULL, NULL);
    PEM_write_bio_RSAPublicKey(pub, keypair);

    //获取长度
    size_t pri_len = BIO_pending(pri);
    size_t pub_len = BIO_pending(pub);

    //分配秘钥字符串内存
    char *pri_str = (char *)malloc(pri_len + 1);
    char *pub_str = (char *)malloc(pub_len + 1);

    BIO_read(pri, pri_str, pri_len);
    BIO_read(pub, pub_str, pub_len);

    pri_str[pri_len] = '\0';
    pub_str[pub_len] = '\0';

    //传出参数，输出
    printf("pub_key len: %d\n", pub_len);
    printf("pri_key len: %d\n", pri_len);
    memcpy(pubOut, pub_str, pub_len+1);
    memcpy(priOut, pri_str, pri_len+1);

    //存储到磁盘（这种方式存储的是begin rsa public key/ begin rsa private key开头的）
    //fputs遇到\0就会结束，可能造成写入文件不完全（是否会存在\0）
    FILE *pubFile = fopen("./pub_key_str", "w");
    if (pubFile == NULL) {
        return;
    }
    fputs(pub_str, pubFile);
    fclose(pubFile);

    FILE *priFile = fopen("./pri_key_str", "w");
    if (priFile == NULL) {
        return;
    }
    fputs(pri_str, priFile);
    fclose(priFile);

    //释放资源
    RSA_free(keypair);
    BIO_free_all(pub);
    BIO_free_all(pri);

    free(pri_str);
    free(pub_str);
}

//RSA公钥加密
void rsa_pub_encrypt(char *plaintext, char *pubstr, char *ciphetext)
{
    //秘钥对,和秘钥字符串不同，需要在内存中根据字符串计算秘钥对
//    RSA *rsa = NULL;
    BIO *keybio = BIO_new_mem_buf((unsigned char *)pubstr, -1);
    RSA *rsa = RSA_new();
    rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);

    int len = RSA_size(rsa);
    printf("rsa_size len: %d\n", len);
    char *beEncrypt = (char *)malloc(len + 1);
    memset(beEncrypt, 0, len + 1);

    //加密
    int ret = RSA_public_encrypt(strlen(plaintext), (unsigned char *)plaintext,
                       (unsigned char *)beEncrypt, rsa, RSA_PKCS1_PADDING);
    printf("ret: %d\n", ret);
    printf("before: %s\n", plaintext);
    printf("after: %s\n", beEncrypt);
    memcpy(ciphetext, beEncrypt, ret);

    free(beEncrypt);
    BIO_free_all(keybio);
    RSA_free(rsa);
}

//私钥解密
void rsa_pri_decrypt(char *ciphetext, char *pristr)
{
    RSA *rsa = RSA_new();
    BIO *keybio;
    keybio = BIO_new_mem_buf((unsigned char *)pristr, -1);

    rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);

    int len = RSA_size(rsa);
    char *beDecrypt = (char *)malloc(len + 1);
    memset(beDecrypt, 0, len + 1);

    // 解密函数
    int ret = RSA_private_decrypt(strlen(ciphetext), (unsigned char*)ciphetext,
                                  (unsigned char*)beDecrypt, rsa,
                                  RSA_PKCS1_PADDING);
    printf("ret pri: %d\n", ret);
    printf("decrypt: %s\n", beDecrypt);

    // 释放内存
    free(beDecrypt);
    BIO_free_all(keybio);
    RSA_free(rsa);
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

//    des_encrypt_test();

//    unsigned char in[18] = "hello you really!";
//    unsigned char out[18] = {0};
//    char mima[8] = "qwertwe";
//    des_encrypt_test2(in, out, mima);

//    char pubstr[2049] = {0}, pristr[2049] = {0}, ciphetext[1024] = {0};
//    generateRSAkey(pubstr, pristr);
//    printf("pub: %s\n", pubstr);
//    printf("pri: %s\n", pristr);

//    char text[] = "hello i am your father!!";

//    rsa_pub_encrypt(text, pubstr, ciphetext);
//    rsa_pri_decrypt(ciphetext, pristr);

    return 0;
}
