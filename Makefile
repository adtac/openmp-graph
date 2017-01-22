components = argparse.o \
			 utils.o \
			 graphgen.o

vertexcol: vertexcol.o $(components)
	gcc -o vertexcol vertexcol.o $(components) -fopenmp

vertexcol.o: vertexcol.c
argparse.o: argparse.c
utils.o: utils.c
graphgen.o: graphgen.c

clean:
	rm -rf *.o
	rm vertexcol
