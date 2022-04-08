#pragma once

#include "board.h"
#include "piece.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

struct solution_restrictions {
  bool use_faceup;
  bool use_facedown;
};

struct solutions {
  // Problem
  board_t problem;

  // Current working piece
  size_t current_level;

  // Array current stack of solutions pattern
  size_t sol_pattern_index[N_PIECES];

  size_t sol_patterns_num[N_PIECES]; // Total number of subsolutions
  piece_t *sol_patterns[N_PIECES]; // Viable solution patterns
  
  piece_t *sub_solutions[N_PIECES];

  // Found Valid soutions
  size_t num_solutions;
  solution_t *solutions;
};

void init_solutions(struct solutions *sol, board_t problem,  struct solution_restrictions restrictions);
void destroy_solutions(struct solutions *sol);
void push_solution(struct solutions *sol);

void solve_rec(struct solutions *sol, board_t problem);

uint64_t solve(struct solutions *sol);

uint64_t make_positions(piece_t piece, piece_properties_t props,
                        board_t problem, piece_t *dest, struct solution_restrictions restrictions);

void print_solution(solution_t *solution, board_t board);