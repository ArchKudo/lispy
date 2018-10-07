CC=gcc
CFLAGS=-std=c99 \
		-pedantic -O3 -g -Wall -Werror -Wextra

LDFLAGS=-ledit -lncurses
IFLAGS=-Iinclude

vpath %.c src
vpath %.h include

prompt: prompt.o mpc.o lval.o last.o lbuiltin.o lenv.o lprint.o lwrap.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm prompt *.o
