components = argparse.o \
			 utils.o

vertexcol: vertexcol.o $(components)
	gcc -o vertexcol vertexcol.o $(components) -fopenmp

vertexcol.o: vertexcol.c
argparse.o: argparse.c
utils.o: utils.c

clean:
	rm -rf *.o
	rm vertexcol
