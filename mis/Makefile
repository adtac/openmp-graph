components = argparse.o \
			 utils.o \
			 graphgen.o \
			 graph.o

mis: mis.o $(components)
	gcc -o mis mis.o $(components) -fopenmp

LDFLAGS=-fopenmp

argparse.o: argparse.c
utils.o: utils.c
graphgen.o: graphgen.c
graph.o: graph.c

mis.o: $(components)
	gcc -o mis.o -c mis.c -fopenmp

clean:
	rm -rf *.o
	rm mis
