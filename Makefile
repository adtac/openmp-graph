components = argparse.o

vertexcol: vertexcol.o $(components)
	gcc -o vertexcol vertexcol.o $(components) -fopenmp

vertexcol.o: vertexcol.c
argparse.o: argparse.c

clean:
	rm -rf *.o
	rm vertexcol
