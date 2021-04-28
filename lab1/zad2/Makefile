CC = gcc
CFLAGS = -Wall -O0 -std=c11

static:
	$(CC) $(CC_FLAGS) -c library.c -o library.o
	ar rcs library.a library.o

shared:
	$(CC) $(CFLAGS) -fPIC -shared library.c -o library_shared.so


main: clean static
	 $(CC) $(CC_FLAGS) -c main.c -o output.o
	 $(CC) $(CC_FLAGS) -static output.o -L. library.a -o output

test: clean static
	 $(CC) $(CC_FLAGS) -c main.c -o output.o
	 $(CC) $(CC_FLAGS) -static output.o -L. library.a -o output
	 ./output test
	 make clean

main_shared: clean shared
	$(CC) $(CFLAGS) -o output main.c library_shared.so -L. -Wl,-rpath=. 
	@echo 'Bibloteka dzielona' >> raport3a.txt
	make run

main_dynamic: clean shared
	$(CC) $(CFLAGS) -o output main_dynamic.c -ldl
	@echo 'Bibloteka dynamiczna' >> raport3a.txt
	make run


main_static: clean static
	 $(CC) $(CC_FLAGS) -c main.c -o output.o
	 $(CC) $(CC_FLAGS) -static output.o -L. library.a -o output
	 @echo 'Bibloteka statyczna' >> raport3a.txt
	 make run
	 
run:
	./output test >> raport3a.txt
	make clean

clean: 
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f *file_number*

zad3b: clean

	@echo 'kompilacja O1 \n' >> raport3b.txt

	@echo 'Bibloteka statyczna\n' >> raport3b.txt
	make static
	$(CC) -Wall -O1 -std=c11 -c main.c -o output.o
	$(CC) -Wall -O1 -std=c11  -static output.o -L. library.a -o output
	./output test >> raport3b.txt
	make clean

	@echo '\nBibloteka dzielona\n' >> raport3b.txt
	make shared
	$(CC) -Wall -O1 -std=c11 -o output main.c library_shared.so -L. -Wl,-rpath=.
	./output test >> raport3b.txt
	make clean

	@echo '\nBibloteka dynamiczna\n' >> raport3b.txt
	make shared
	$(CC) -Wall -O1 -std=c11 -o output main_dynamic.c -ldl
	./output test >> raport3b.txt
	make clean


	@echo '\n kompilacja O2 \n' >> raport3b.txt

	@echo 'Bibloteka statyczna\n' >> raport3b.txt
	make static
	$(CC) -Wall -O2 -std=c11 -c main.c -o output.o
	$(CC) -Wall -O2 -std=c11  -static output.o -L. library.a -o output
	./output test >> raport3b.txt
	make clean

	@echo '\nBibloteka dzielona\n' >> raport3b.txt
	make shared
	$(CC) -Wall -O2 -std=c11 -o output main.c library_shared.so -L. -Wl,-rpath=.
	./output test >> raport3b.txt
	make clean

	@echo '\nBibloteka dynamiczna\n' >> raport3b.txt
	make shared
	$(CC) -Wall -O2 -std=c11 -o output main_dynamic.c -ldl
	./output test >> raport3b.txt
	make clean

	@echo '\n kompilacja O3 \n' >> raport3b.txt


	@echo 'Bibloteka statyczna\n' >> raport3b.txt
	make static
	$(CC) -Wall -O3 -std=c11 -c main.c -o output.o
	$(CC) -Wall -O3 -std=c11  -static output.o -L. library.a -o output
	./output test >> raport3b.txt
	make clean

	@echo '\nBibloteka dzielona\n' >> raport3b.txt
	make shared
	$(CC) -Wall -O3 -std=c11 -o output main.c library_shared.so -L. -Wl,-rpath=.
	./output test >> raport3b.txt
	make clean

	@echo '\nBibloteka dynamiczna\n' >> raport3b.txt
	make shared
	$(CC) -Wall -O3 -std=c11 -o output main_dynamic.c -ldl
	./output test >> raport3b.txt
	make clean