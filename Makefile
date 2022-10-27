CC=gcc
OMP=-fopenmp
DIROPT=-Ofast	
BINARY=./studentspar
SOURCE=./studentspar.c 

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