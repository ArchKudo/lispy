CC=gcc
CFLAGS=-Wall -Wextra -Werror -g -pedantic
LDFLAGS=-lreadline


prompt: prompt.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm prompt
