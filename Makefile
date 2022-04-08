CC=gcc
FLAGS=-Wall -O2 -mbmi2  -mavx2 -march=native -fopenmp

.PHONY: all
all: solver


problem.o: problem.c problem.h board.h
	${CC} ${FLAGS} -c $@ problem.c

piece.o: piece.c piece.h
	${CC} ${FLAGS} -c $@ piece.c

board.o: board.c board.h
	${CC} ${FLAGS} -c $@ board.c


solver: solver.c solver.h board.o piece.o problem.o
	$(CC) $(FLAGS) -o $@ solver.c board.o piece.o problem.o


.PHONY: clean
clean:
	rm *.o solver
