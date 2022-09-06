CC=gcc
CFLAGS_release = -O3
CFLAGS_debug = -O0 -g
BUILD=release

AVX512=-mno-avx512f

CFLAGS=-Wall -std=c17 ${CFLAGS_${BUILD}} -mbmi2  -mavx2 ${AVX512} -march=native -fopenmp -I ./src/inc  -Lbuild

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



.PHONY: benchmark
benchmark: statistics
	@bash -c "for i in {1..5}; do time ./statistics > /dev/null; done"

.PHONY: test
test: statistics
	@echo "Expect:"
	@echo 8780a56f0a41807851df53042d245a92b36fde872d9970ff270172f1e1c2f821
	@bash -c ./statistics | sort | sha256sum


.PHONY: clean
clean:
	rm -f *.o build/* pad statistics
