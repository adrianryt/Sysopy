CC = gcc
CFLAGS = -Wall -O0 -std=c11

static:
	$(CC) $(CC_FLAGS) -c library.c -o library.o
	ar rcs library.a library.o

main: clean static
	 $(CC) $(CC_FLAGS) -c main.c -o output.o
	 $(CC) $(CC_FLAGS) -static output.o -L. library.a -o output

test: clean static
	 $(CC) $(CC_FLAGS) -c main.c -o output.o
	 $(CC) $(CC_FLAGS) -static output.o -L. library.a -o output
	 ./output test > raport2.txt
	 make clean

run:
	./output 3 a.txt:b.txt a.txt:b.txt a.txt:b.txt
	make clean

clean: 
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f *output*
	rm -f *file_number*

