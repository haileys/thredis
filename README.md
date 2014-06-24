# Thredis

Thredis is a Hiredis wrapper designed for heavily threaded C programs.

It allows multiple threads to execute Redis commands at the same time over a single connection. Commands sent to Redis are pipelined, and a dedicated reader thread takes care of reading the responses in the right order.

Thredis is an extraction from a highly-trafficked production C daemon where it seems to be quite stable. That said, keep in mind that this extraction may have subtle bugs that aren't present in the original version. If you run into anything nasty, just open an issue!

## Usage

Before you start executing Redis commands with Thredis, you'll need to create a `thredis_t*` context with `thredis_new`:

```c
redisContext* redis = redisConnect("127.0.0.1", 6379);

if(redis->err) {
    fprintf(stderr, "redisConnect: %s\n", redis->errstr);
    exit(EXIT_FAILURE);
}

thredis_t* thredis = thredis_new(redis);

if(!thredis) {
    fprintf(stderr, "thread_new failed\n");
    exit(EXIT_FAILURE);
}
```

Now you can start executing Redis commands with `thredis_command`. This is completely thread-safe and does not require any extra locking - that's all handled for you by Thredis.

```c
redisReply* reply = thredis_command(thredis, "SET %s %d", "my_key", 1234);

if(!reply) {
    fprintf(stderr, "thredis_command failed\n");
    exit(EXIT_FAILURE);
}

// don't forget to free reply through hiredis when you're done:
freeReplyObject(reply);
```

Once you're done, you can clean up with `thredis_close`. This will free all resources allocated by Hiredis, but you'll still need to take care of freeing your `redisContext*` through Hiredis.

`thredis_close` will wait for currently executing commands to finish before freeing resources. However you must be careful not to issue any more Redis commands on other threads once you've called this command.

## License

MIT.
