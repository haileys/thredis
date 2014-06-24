#ifndef THREDIS_H
#define THREDIS_H

#include <hiredis.h>

typedef struct thredis
thredis_t;

thredis_t*
thredis_new(redisContext* redis_ctx);

void
thredis_close(thredis_t* thredis);

redisReply*
thredis_command(thredis_t* thredis, const char* format, ...);

#endif
