CC=gcc
OMP=-fopenmp
DIROPT=-Ofast	
BINARY_PAR=./studentspar
SOURCE_PAR=./studentspar.c 
BINARY_SEQ=./studentsseq
SOURCE_SEQ=./studentsseq.c 
BINARY_SEQOPT=./studentsseq_opt


sequential:
	$(CC) $(SOURCE_SEQ) -o $(BINARY_SEQ) $(OMP) -lm

sequential_optimized:
	$(CC) $(SOURCE_SEQ) -o $(BINARY_SEQOPT) $(OMP) $(DIROPT) -lm 

parallel:
	$(CC) $(SOURCE_PAR) -o $(BINARY_PAR) $(OMP) -lm

parallel_optimized:
	$(CC) $(SOURCE_PAR) -o $(BINARY_PAR) $(DIROPT) $(OMP) -lm
 
run_sequential:
	$(BINARY_SEQ)

run_sequential_opt:
	$(BINARY_SEQOPT)

run_parallel:
	$(BINARY_PAR)

valgrind:
	$(CC) $(SOURCE) -o $(BINARY) $(OMP) -lm -g

valgrind-run:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(BINARY)

valgrind-seq:
	$(CC) $(SOURCE_SEQ) -o $(BINARY_SEQ) $(OMP) -lm -g

valgrind-run-seq:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(BINARY_SEQ)
