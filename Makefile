CC=gcc
CFLAGS_release = -O3
CFLAGS_debug = -O0 -g
BUILD=release

CFLAGS=-Wall -std=c11 ${CFLAGS_${BUILD}} -mbmi2  -mavx2 -march=native -fopenmp -I ./src/inc  -Lbuild

BUILD_DIR=build
$(shell mkdir -p ${BUILD_DIR})
SRC_DIR=src


.PHONY: all
all: pad statistics


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

pad: $(BUILD_DIR)/solver.o $(BUILD_DIR)/board.o $(BUILD_DIR)/piece.o $(BUILD_DIR)/problem.o $(BUILD_DIR)/utils.o $(BUILD_DIR)/printing.o $(BUILD_DIR)/pad.o
	$(CC) $(CFLAGS) -o $@ $^

statistics: $(BUILD_DIR)/solver.o $(BUILD_DIR)/board.o $(BUILD_DIR)/piece.o $(BUILD_DIR)/problem.o $(BUILD_DIR)/utils.o $(BUILD_DIR)/printing.o $(BUILD_DIR)/statistics.o
	$(CC) $(CFLAGS) -o $@ $^


.PHONY: clean
clean:
	rm -f *.o build/* pad statistics
