CFLAGS=-I include -Wall

src/shell: src/list.o src/listString.o src/funcP2.o src/memory.o src/listProcess.o src/funcP3.o src/cmdp3.o

clean:
	rm src/shell src/*.o