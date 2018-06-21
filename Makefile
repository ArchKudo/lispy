CC=gcc
CFLAGS=-std=c99 \
		-pedantic -O3 -g -Wall -Werror -Wextra \

LDFLAGS=-lreadline



prompt: prompt.o mpc.o lval.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm prompt *.o
