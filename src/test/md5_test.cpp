/*
 *
 */
#include "comm/md5.h"
#include <stdio.h>

int main()
{
	char salt[12] = "jl@123";

	char szOldPwd[64] = "13561881276";
	
	char szMd51[64];

	char szStr[64] = {0};

	char szMd52[64];

	// 一次md5
	md5_passwd(szOldPwd, szMd51);
	
	// 加盐
	sprintf(szStr, "%s%s", szMd51, salt);
	printf("%s\n", szMd51);

	// 二次md5
	md5_passwd(szMd51, szMd52);
	printf("%s\n", szMd52);


	printf("%d\n", 3 < 1 << 2);
	printf("%d\n", (3 < 1) << 2);
	printf("%d\n", 3 < (1 << 2));

	return 0;
}