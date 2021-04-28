CC = gcc
CFLAGS = -Wall -O0 -std=c11


main_fork:
	 $(CC) $(CC_FLAGS) main_fork.c -o output
	./output ignore
	./output handler
	./output mask
	./output pending
	make clean

main_exec:
	$(CC) $(CC_FLAGS) child_exec.c -o child_exec
	$(CC) $(CC_FLAGS) main_exec.c -o outputE
	./outputE ignore
	./outputE mask
	./outputE pending
	make clean

clean: 
	rm -f *output*
	rm -f child_exec