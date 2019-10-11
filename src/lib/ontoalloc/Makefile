all:
	ont_cpp -c malloc.cpp -o malloc.o
	llvm-ar rs libmalloc.a malloc.o
	cp libmalloc.a ../../../install/lib/

clean:
	rm malloc.o libmalloc.a

