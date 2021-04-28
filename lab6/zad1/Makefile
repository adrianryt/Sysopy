CC = gcc
CC_FLAGS = -Wall -O0 -std=c11 -g


main: static server client

static:
	$(CC) $(CC_FLAGS) -c data_and_info.c -o data_and_info.o
	ar rcs data_and_info.a data_and_info.o

server: server.c
	$(CC) $(CC_FLAGS) -c server.c -o server.o
	$(CC) $(CC_FLAGS) -static server.o -L. data_and_info.a -o server

client: client.c
	$(CC) $(CC_FLAGS) -c client.c -o client.o
	$(CC) $(CC_FLAGS) -static client.o -L. data_and_info.a -o client

run: 
	./server
	./client

clean: 
	rm -f *.o
	rm -f *.a
	rm -f server client