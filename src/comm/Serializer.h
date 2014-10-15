/**************************************************************
 * libevent��������װ: Light c++ wrapper Of Libevent  (LOL)
 * Packer.h
 * @brief	��������������
 * @author  d0evi1	
 * @date	2012.7.16
 **************************************************************/
#ifndef _LOL_SERIALIZER_H_
#define _LOL_SERIALIZER_H_

#include <stdio.h>
#include <stdint.h>

// ֻת��, szBuf [inout]
char* Serializer_GetByte(char* szBuf, uint8_t* pByte);

char* Serializer_GetWord(char* szBuf, uint16_t* pWord);

char* Serializer_GetDWord(char* szBuf, uint32_t* pDWord);

char* Serializer_GetDDWord(char* szBuf, uint64_t* pDDWord);

// ֻ
char* Serializer_SetByte(char* szBuf, uint8_t cVal);

char* Serializer_SetWord(char* szBuf, uint16_t wVal);

char* Serializer_SetDWord(char* szBuf, uint32_t dwVal);

char* Serializer_SetDDWord(char* szBuf, uint64_t ddwVal);

#endif
