components = argparse.o \
			 utils.o \
			 graphgen.o \
			 graph.o

vertexcol: vertexcol.o $(components)
	gcc -o vertexcol vertexcol.o $(components) -fopenmp

LDFLAGS=-fopenmp

argparse.o: argparse.c
utils.o: utils.c
graphgen.o: graphgen.c
graph.o: graph.c

vertexcol.o: $(components)
	gcc -o vertexcol.o -c vertexcol.c -fopenmp

clean:
	rm -rf *.o
	rm vertexcol
