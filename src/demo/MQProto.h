#ifndef _LITE_MQPROTO_H_
#define _LITE_MQPROTO_H_

/*
 * ͷ��
 */
typedef struct {
	uint32_t	dwLength;		// ����
	uint32_t	dwVersion;		// Э��汾��
	uint32_t	dwTime;			// ����ʱ��
	uint16_t	wCmd;			// �����
} MQHead;

/*
 * ���������
 */
typedef struct {
	uint8_t		cStart;
	MQHead		stHead;
	uint32_t	dwHandFlag;
	uint8_t		cEnd;
} MQHandReq;

/*
 * �������
 */
typedef struct {
	uint8_t		cStart;
	MQHead		stHead;
	uint32_t	dwRet;			// 
	uint8_t		cEnd;
} MQHandRsp;


#endif