CC = gcc

CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99 -D_POSIX_C_SOURCE=199309L
LIBS   = -lncurses

all:
	$(CC) $(CFLAGS) pong.c $(LIBS)
