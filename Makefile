CC=gcc
FLAGS=-Wall -std=c11 -O3 -g -mbmi2  -mavx2 -march=native -fopenmp -I ./src/inc  -Lbuild

BUILD_DIR=build
SRC_DIR=src


.PHONY: all
all: pad statistics


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(FLAGS) -c -o $@ $<

pad: $(BUILD_DIR)/solver.o $(BUILD_DIR)/board.o $(BUILD_DIR)/piece.o $(BUILD_DIR)/problem.o $(BUILD_DIR)/utils.o $(BUILD_DIR)/printing.o $(BUILD_DIR)/pad.o
	$(CC) $(FLAGS) -o $@ $^

statistics: $(BUILD_DIR)/solver.o $(BUILD_DIR)/board.o $(BUILD_DIR)/piece.o $(BUILD_DIR)/problem.o $(BUILD_DIR)/utils.o $(BUILD_DIR)/printing.o $(BUILD_DIR)/statistics.o
	$(CC) $(FLAGS) -o $@ $^


.PHONY: clean
clean:
	rm -f *.o build/* pad statistics
