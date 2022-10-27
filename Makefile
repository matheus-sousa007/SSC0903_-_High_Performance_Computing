CC=gcc
OMP=-fopenmp
DIROPT=-O3	
BINARY=./studentspar
SOURCE=./studentspar.c 
BINARY_SEQ=./seq
SOURCE_SEQ=./seq.c 

all:
	$(CC) $(SOURCE) -o $(BINARY) $(OMP) -lm

optimized:
	$(CC) $(SOURCE) -o $(BINARY) $(DIROPT) $(OMP) -lm
 
run:
	$(BINARY)

valgrind:
	$(CC) $(SOURCE) -o $(BINARY) $(OMP) -lm -g

valgrind-run:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(BINARY)

valgrind-seq:
	$(CC) $(SOURCE_SEQ) -o $(BINARY_SEQ) $(OMP) -lm -g

valgrind-run-seq:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(BINARY_SEQ)