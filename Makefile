CC=gcc
FLAGS=-Wall -O3 -g -mbmi2  -mavx2 -march=native -fopenmp

.PHONY: all
all: pad statistics


problem.o: problem.c problem.h board.h
	${CC} ${FLAGS} -c $@ problem.c

piece.o: piece.c piece.h
	${CC} ${FLAGS} -c $@ piece.c

board.o: board.c board.h
	${CC} ${FLAGS} -c $@ board.c


solver.o: solver.c solver.h
	$(CC) $(FLAGS) -c $@ solver.c 

pad: solver.o board.o piece.o problem.o main.c
	$(CC) $(FLAGS) -o $@ main.c solver.o board.o piece.o problem.o 

statistics: solver.o board.o piece.o problem.o statistics.c 
	$(CC) $(FLAGS) -o $@ statistics.c solver.o board.o piece.o problem.o 


.PHONY: clean
clean:
	rm *.o pad statistics
