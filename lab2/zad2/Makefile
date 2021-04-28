CC = gcc
CFLAGS = -Wall -O0 -std=c11


main: clean 
	 $(CC) $(CC_FLAGS) main.c -o output
	 make run
	 make clean

run:
	./output d a.txt


clean: 
	rm -f *output*