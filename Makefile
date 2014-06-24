CFLAGS = -Wall -Werror -pedantic -Wextra -O3 -I vendor/hiredis -std=c99

ifeq ($(shell uname -s),Linux)
	CFLAGS += -pthread
	LDFLAGS += -lrt
endif

TEST_CFLAGS = -iquote .

.PHONY: all test clean

all: thredis.o

test: test/stress
	test/stress

clean:
	rm -f test/stress thredis.o
	git submodule foreach git clean -dfx

thredis.o: vendor/hiredis/hiredis.h

test/stress: test/stress.c thredis.o vendor/hiredis/libhiredis.a
	$(CC) -o $@ $(LDFLAGS) $(CFLAGS) $(TEST_CFLAGS) $^

vendor/hiredis/%: vendor/hiredis/Makefile
	$(MAKE) -C vendor/hiredis $(subst vendor/hiredis/,,$@)

vendor/hiredis/Makefile:
	git submodule update --init
