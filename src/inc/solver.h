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

typedef struct solutions_t {
  // Problem
  board_t problem;

  // Current working piece
  size_t current_level;

  size_t n_pieces;

  // Array current stack of solutions pattern
  size_t sol_pattern_index[MAX_PIECES];

  size_t sol_patterns_num[MAX_PIECES]; // Total number of subsolutions
  piece_t *sol_patterns[MAX_PIECES]; // Viable solution patterns
  
  piece_t *sub_solutions[MAX_PIECES];

  // Found Valid soutions
  size_t num_solutions;
  solution_t *solutions;
} solutions_t;

void init_solutions(solutions_t *sol, problem_t *problem,
                    struct solution_restrictions restrictions);
void destroy_solutions(solutions_t *sol);
void push_solution(solutions_t *sol);

uint64_t solve(solutions_t *sol);

uint64_t make_positions(piece_t piece, piece_properties_t props,
                        board_t problem, piece_t *dest, struct solution_restrictions restrictions);

void print_solution(solution_t *solution, problem_t *problem);