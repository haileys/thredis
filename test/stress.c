#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "thredis.h"

#define THREAD_COUNT 128

#define FAIL(...) (fprintf(stderr, "\n"), fprintf(stderr, __VA_ARGS__), exit(1))

struct thread_ctx {
    pthread_t thread;
    int number;
    thredis_t* thredis;
};

void*
thread_main(void* ctx_)
{
    struct thread_ctx* ctx = ctx_;
    thredis_t* thredis = ctx->thredis;
    int number = ctx->number;

    for(int i = 0; i < 10000; i++) {
        char str[16];
        sprintf(str, "%d", i);

        redisReply* reply = thredis_command(thredis, "ECHO %s", str);

        if(!reply) {
            FAIL("thredis_command returned NULL\n");
        }

        if(reply->type != REDIS_REPLY_STRING) {
            FAIL("thredis_command did not return string\n");
        }

        if(strcmp(reply->str, str)) {
            FAIL("thredis_command string reply was incorrect\n");
        }

        freeReplyObject(reply);

        if((i % 100) == 0 && number == 0) {
            fprintf(stderr, ".");
        }
    }

    return NULL;
}

int main()
{
    const char* redis_host = getenv("REDIS_HOST");
    const char* redis_port = getenv("REDIS_PORT");

    if(!redis_host) {
        redis_host = "localhost";
    }

    if(!redis_port) {
        redis_port = "6379";
    }

    redisContext* redis = redisConnect(redis_host, atoi(redis_port));
    if(redis->err) {
        FAIL("redisConnect failed: %s\n", redis->errstr);
    }

    thredis_t* thredis = thredis_new(redis);
    if(!thredis) {
        FAIL("thredis_new failed\n");
    }

    struct thread_ctx threads[THREAD_COUNT];

    for(int i = 0; i < THREAD_COUNT; i++) {
        threads[i].thredis = thredis;
        threads[i].number = i;
        if(pthread_create(&threads[i].thread, NULL, thread_main, &threads[i])) {
            FAIL("pthread_create failed\n");
        }
    }

    for(int i = 0; i < THREAD_COUNT; i++) {
        if(pthread_join(threads[i].thread, NULL)) {
            FAIL("pthread_join failed\n");
        }
    }

    thredis_close(thredis);
    redisFree(redis);

    printf("\n");
}
