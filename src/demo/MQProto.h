#ifndef _LITE_MQPROTO_H_
#define _LITE_MQPROTO_H_

/*
 * 头部
 */
typedef struct {
	uint32_t	dwLength;		// 长度
	uint32_t	dwVersion;		// 协议版本号
	uint32_t	dwTime;			// 操作时间
	uint16_t	wCmd;			// 命令号
} MQHead;

/*
 * 握手请求包
 */
typedef struct {
	uint8_t		cStart;
	MQHead		stHead;
	uint32_t	dwHandFlag;
	uint8_t		cEnd;
} MQHandReq;

/*
 * 握手响包
 */
typedef struct {
	uint8_t		cStart;
	MQHead		stHead;
	uint32_t	dwRet;			// 
	uint8_t		cEnd;
} MQHandRsp;


#endif