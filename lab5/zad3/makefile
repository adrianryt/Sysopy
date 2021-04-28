CC = gcc
CFLAGS = -Wall -O0 -std=c11


main:
	$(CC) $(CC_FLAGS) producer.c -o producer
	$(CC) $(CC_FLAGS) consumer.c -o consumer
	$(CC) $(CC_FLAGS) main.c -o output
	rm -f b.txt
	touch b.txt
	./output
	make clean

clean:
	rm -f fifo consumer producer output