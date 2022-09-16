CC=gcc
CFLAGS_release = -O3 -fomit-frame-pointer
CFLAGS_debug = -O0 -g -fno-omit-frame-pointer  
BUILD=release
ARCH=native
USE_AVX=avx2

N_BENCHMARKS=5

ifeq ($(USE_AVX), avx2)
	AVX_FLAGS=-mavx2 -mno-avx512f
endif

ifeq ($(USE_AVX), avx512)
	AVX_FLAGS=-mavx2 -mavx512f	
endif

CFLAGS=-Wall -std=c17 ${CFLAGS_${BUILD}} -mbmi2 ${AVX_FLAGS} -march=${ARCH} -fopenmp -I ./src/inc  -Lbuild

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
	@bash -c "for i in {1..${N_BENCHMARKS}}; do time ./statistics > /dev/null; done"

.PHONY: test
test: statistics
	@echo "Expect:"
	@echo 8780a56f0a41807851df53042d245a92b36fde872d9970ff270172f1e1c2f821
	@bash -c ./statistics | sort | sha256sum


.PHONY: clean
clean:
	rm -f *.o build/* pad statistics
