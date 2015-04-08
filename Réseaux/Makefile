SRC=src/sgetline.c

CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -I./
DBGFLAGS=-g3 -ggdb3

all: chat server

chat: ${OBJ} client.c
	$(CC) $(CFLAGS) ${DBGFLAGS} client.c -o client ${OBJ} -pthread -lreadline

server: ${OBJ} server.c
	$(CC) $(CFLAGS) ${DBGFLAGS} server.c -o server ${OBJ} -pthread -lreadline


clean:
	${RM} -v *.o
