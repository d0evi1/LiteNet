/**************************************************************
 * LiteTCP ȫ�ֶ���
 * 
 * ServerComm.h
 *
 * @author 	jolly wang
 * @date	2012.7.16
 **************************************************************/

#ifndef _LITE_TYPE_H_
#define _LITE_TYPE_H_


#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <sys/shm.h>


//-------------------------------------------
//
//-------------------------------------------
//��Ч��IDֵ
#define INVALID_ID		-1


//-------------------------------------------
// TRUE �� FALSE �Ķ���
//-------------------------------------------
#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

//-------------------------------------------
// Assert����
//-------------------------------------------

#define Assert(expr)		do { \
								if(!(expr)){ \
									__assert__(__FILE__,__LINE__,__PRETTY_FUNCTION__,#expr); \
								} \
							} while(0)

//-------------------------------------------
// �����Сֵ
//-------------------------------------------
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


//---------------------------------------------
// ������: ɾ��ָ��
//---------------------------------------------
#ifndef SAFE_DELETE
#define SAFE_DELETE(x)		if((x)!=NULL) {delete (x); (x)=NULL;}
#endif

//---------------------------------------------
// ������: ɾ��ָ������
//---------------------------------------------
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x)	if((x)!=NULL) {delete[] (x); (x)=NULL;}
#endif

//---------------------------------------------
// htonll�Ķ���
//---------------------------------------------
#ifndef SWAP_DDWORD
#define SWAP_DDWORD(x) \
	((((x) & 0xff00000000000000ull) >> 56)	\
	| (((x) & 0x00ff000000000000ull) >> 40)	\
	| (((x) & 0x0000ff0000000000ull) >> 24) \
	| (((x) & 0x000000ff00000000ull) >> 8)	\
	| (((x) & 0x00000000ff000000ull) << 8)	\
	| (((x) & 0x0000000000ff0000ull) << 24) \
	| (((x) & 0x000000000000ff00ull) << 40)	\
	| (((x) & 0x00000000000000ffull) << 56))
#endif

#if defined (BIG_ENDIAN)
#define htonll(x)	(x)
#define ntohll(x)	(x)
#elif defined (LITTLE_ENDIAN)
#define htonll(x)	SWAP_DDWORD(x)
#define ntohll(x)	SWAP_DDWORD(x)
#endif

//IP��ַ���ַ���󳤶�
#define IP_SIZE			24

//---------------------------------------------
// ���
//---------------------------------------------
#define PACK_COMPART			"$-$"//����ָ���
#define PACK_COMPART_SIZE		strlen(PACK_COMPART)

#define START_TAG		0x6	
#define END_TAG			0x9

//----------------------------------------------
// ���ط�ʽ
//----------------------------------------------
#define RET_LN()    {return -__LINE__;}

/*
 * �򻯺�������
 * EQ   =>  '='
 * NEQ  =>  '!='
 */
#define RET_EQ(func, val, ret)      if((func)==(val)){return (ret);}
#define RET_NEQ(func, val, ret)     \
    do{\
        int iFuncRet=(func);\
        if(iFuncRet != (val)) \
        {\
            return (ret);\
        }} while(0) 

/*
 * EZ (equal zero)   => "!=0"
 * EN (equal null)  => "!=NULL"
 */
#define RET_EZ(func, ret)   RET_EQ((func), (0), (ret))
#define RET_NN(func, ret)   RET_EQ((func), (NULL), (ret))

/*
 * NZ (not zero)  => "!=0"
 * NN (not null)  => "!=NULL"
 */
#define RET_NZ(func, ret)   RET_NEQ((func), (0), (ret))
#define RET_NN(func, ret)   RET_NEQ((func), (NULL), (ret))




#endif
