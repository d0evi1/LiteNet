#include "Serializer.h"

#include "LiteType.h"

// 
char* Serializer_GetByte(char* szBuf, uint8_t* pByte)
{
	*pByte = (uint8_t)szBuf[0];
	return szBuf+sizeof(uint8_t);
}

//
char* Serializer_GetWord(char* szBuf, uint16_t* pWord)
{
	uint16_t* iWord = (uint16_t*)szBuf;
	*pWord = ntohs(*iWord);
	return szBuf+sizeof(uint16_t);
}

// 
char* Serializer_GetDWord(char* szBuf, uint32_t* pDWord)
{
	uint32_t* iDWord = (uint32_t*)szBuf;
	*pDWord = ntohl(*iDWord);
	return szBuf+sizeof(uint32_t);
}

//
char* Serializer_GetDDWord(char* szBuf, uint64_t* pDDWord)
{
	uint64_t* iDDWord = (uint64_t*)szBuf;
	*pDDWord = ntohll(*iDDWord);
	return szBuf+sizeof(uint64_t);
}

// 
char* Serializer_SetByte(char* szBuf, uint8_t cVal)
{
	szBuf[0] = (char)cVal;
	return szBuf+sizeof(uint8_t);
}

// 
char* Serializer_SetWord(char* szBuf, uint16_t wVal)
{
	uint16_t wTmp = htons(wVal);
	memcpy(szBuf, &wTmp, sizeof(uint16_t));
	return szBuf+sizeof(uint16_t);
}

// 
char* Serializer_SetDWord(char* szBuf, uint32_t dwVal)
{
	uint32_t dwTmp = htonl(dwVal);
	memcpy(szBuf, &dwTmp, sizeof(uint32_t));
	return szBuf+sizeof(uint32_t);
}

// 
char* Serializer_SetDDWord(char* szBuf, uint64_t ddwVal)
{
	uint64_t ddwTmp = htonll(ddwVal);
	memcpy(szBuf, &ddwVal, sizeof(uint64_t));
	return szBuf+sizeof(uint64_t);
}