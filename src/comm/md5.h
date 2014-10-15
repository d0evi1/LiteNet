/*
 * from openssl src code: md5 version.
 */
 
#ifndef MD5_INCLUDED
#define MD5_INCLUDED

#ifdef __cplusplus
extern "C" 
{
#endif


/*
 * This code has some adaptations for the Ghostscript environment, but it
 * will compile and run correctly in any environment with 8-bit chars and
 * 32-bit ints.  Specifically, it assumes that if the following are
 * defined, they have the same meaning as in Ghostscript: P1, P2, P3,
 * ARCH_IS_BIG_ENDIAN.
 */
 
typedef unsigned char		md5_byte_t;		// 8-bit byte 
typedef unsigned int		md5_word_t;		// 32-bit word 
 

typedef struct md5_state_s {
 md5_word_t			count[2];    /* message length in bits, lsw first */
 md5_word_t			abcd[4];     // digest 缓存
 md5_byte_t			buf[64];     // accumulate块
} md5_state_t;


 

/*
 * 初始化
 */
#ifdef P1
void md5_init(P1(md5_state_t* pms));
#else
void md5_init(md5_state_t* pms);
#endif

/*
 * append一个字符串
 */
#ifdef P3
void md5_append(P3(md5_state_t *pms, const md5_byte_t *data, int nbytes));
#else
void md5_append(md5_state_t* pms, const md5_byte_t* data, int nbytes);
#endif

/* Finish the message and return the digest. */
/*
 * 摘要
 */
#ifdef P2
void md5_finish(P2(md5_state_t* pms, md5_byte_t digest[16]));
#else
void md5_finish(md5_state_t* pms, md5_byte_t digest[16]);
#endif

/*
 * 加密
 */
void md5_passwd(char *oldpasswd, char *md5_passwd);

#ifdef __cplusplus
}  /* end extern "C" */
#endif

#endif /* md5_INCLUDED */