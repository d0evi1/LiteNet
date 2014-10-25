/*
 * Description: 
 *     History: junle, 2014/10/25, create
 */

#ifndef __MYREDIS_API_H__
#define __MYREDIS_API_H__

#include "Logger.h"


/*
 * redis cmd.
 */
#define REDIS_CMD(redis, reply, fmt, log) \
    do{ \
        (reply) = NULL; \
        (reply) = (redisReply*)redisCommand(redis, fmt); \
        if((reply) == NULL) \
        { \
            XLOG_ERROR(log, "reply NULL, err:%s, errstr:%s",\
                    redis->err, redis->errstr);\
            (reply) = NULL; \
            return -__LINE__; \
        }\
        if(reply->type == REDIS_REPLY_ERROR) \
        {\
            XLOG_ERROR(log, "redisCommand err.err:(%s), errstr:(%s)",\
                    fmt, reply->str);\
            freeReplyObject(reply);\
            (reply) = NULL; \
            return -__LINE__;\
        }\
    } while(0) 


/*
 * rediscmd: multi args.
 */
#define REDIS_CMD_V2(redis, reply, command, fmt, log, args...) \
    do { \
        (reply) = NULL; \
        (reply) = (redisReply*) redisCommand(redis, "%s " fmt, command, ##args);\
        if((reply) == NULL) \
        {\
            XLOG_ERROR(log, "reply NULL, err: %d, errstr:(%s)", \
                fmt, reply->str); \
            freeReplyObject(reply);\
            (reply) = NULL; \
            return -__LINE__; \
        }\
        if(reply->type == REDIS_REPLY_ERROR) \
        {\
            XLOG_ERROR(log, "redisCommand err, errstr:%s", \
                    redis->str);\
            freeReplyObject(reply);\
            (reply) = NULL;\
            return -__LINE__;\
        }\
    } while(0)

/*
 * free redis object.
 */
#define REDIS_FREE(reply, log) \
    do {\
        if((reply) != NULL) {\
            XLOG_ERROR(log, "free redis object.");\
            freeReplyObject(reply); \
            (reply) = NULL;\
        } \
    } while(0)

#endif
