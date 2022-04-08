#pragma once
#include <stdint.h>

#define N_PIECES 8

typedef uint64_t board_t;
typedef uint64_t piece_t;

typedef struct solution_t {
  piece_t pieces[N_PIECES];
} solution_t;

extern const board_t BLANK_BOARD;

void print_raw(uint64_t soution);

extern char *reverse_lookup[];