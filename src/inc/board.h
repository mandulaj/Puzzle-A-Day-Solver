#pragma once
#include "piece.h"
#include <stdint.h>

#define N_PIECES 10

typedef uint64_t board_t;

typedef struct solution_t {
  piece_t pieces[N_PIECES];
} solution_t;

void print_raw(uint64_t soution);
void print_2_raw(uint64_t pattern1, uint64_t pattern2);
