#pragma once

#include "config.h"
#include "piece.h"
#include <stdint.h>

typedef uint64_t board_t;

typedef struct solution_t {
  piece_t pieces[MAX_PIECES];
} solution_t;

void print_raw(uint64_t soution);
void print_2_raw(uint64_t pattern1, uint64_t pattern2);
