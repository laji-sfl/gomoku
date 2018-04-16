/*
 *	实现客户端与服务器之间的通信数据的简单加密
 *  利用openssl库中的加密模块来实现
 *  对称加密DES、AES
 *  非对称加密RSA、DSA
 */
#include <openssl/des.h>
#include <openssl/pkcs7.h>


//加密
int
encrypt_data(const char *_key, const char *_vt,char *_raw_ptr,size_t _raw_size
             , char **_dst_buf, size_t *_dst_size) {

    DES_key_schedule schedule;
    uchar key1[8];
    des_cblock *iv3;
    int pading ;
    size_t i, vt_size ;
    char *mid_buf;

    memset( key1,0,8);
    memcpy( key1, _key, 8 );
    DES_set_key_unchecked( (const_DES_cblock*)&key1, &schedule);

    vt_size = strlen( _vt );
    iv3 = (des_cblock *)malloc(vt_size * sizeof(uchar));
    memcpy(iv3,_vt,vt_size);

    pading = 8 - (_raw_size % 8);
    *_dst_size = _raw_size + pading;
    mid_buf = (char*)malloc(*_dst_size);
    memcpy(mid_buf,_raw_ptr,_raw_size );
    for (i = _raw_size ; i < *_dst_size; i++ ) {
        mid_buf[i] = pading;
    }
    *_dst_buf = (char*)malloc(*_dst_size);
    DES_cbc_encrypt( (const uchar*)mid_buf, (unsigned char *)*_dst_buf, *_dst_size, &schedule, iv3, DES_ENCRYPT);

    free(iv3);
    free(mid_buf);

    return 1;
}

//解密
int
decrypt_data(const char *_key, const char *_vt,char *_raw_ptr,size_t _raw_size
             , char **_dst_buf, size_t *_dst_size ) {

    DES_key_schedule schedule;
    uchar key1[8];
    des_cblock *iv3;
    int pading ;
    size_t i, vt_size ;
    char *mid_buf;

    memset( key1,0,8);
    memcpy( key1, _key, 8 );
    DES_set_key_unchecked( (const_DES_cblock*)&key1, &schedule);

    vt_size = strlen( _vt );
    iv3 = (des_cblock *)malloc(vt_size * sizeof(uchar));
    memcpy(iv3,_vt,vt_size);

    *_dst_buf = (char*)malloc(_raw_size);
    DES_cbc_encrypt( (const uchar*)_raw_ptr, *_dst_buf, _raw_size, &schedule, iv3, DES_DECRYPT);

    free(iv3);
    return 1;
}


int main()
{
    return 0;
}
