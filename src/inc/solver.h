#pragma once

#include "board.h"
#include "config.h"
#include "utils.h"
#include <stdint.h>
#include <stdlib.h>

struct solution_restrictions {
  bool use_faceup;
  bool use_facedown;
};

typedef struct solutions_t {
  // Problem (with the target invalidated)
  board_t problem;

  // Current working piece
  size_t current_level;

  // Number of pieces available
  size_t n_pieces;

  // Array current stack of solutions pattern
  size_t sol_pattern_index[MAX_PIECES];

  size_t sol_patterns_num[MAX_PIECES];   // Total number of subsolutions
  piece_t *sol_patterns[MAX_PIECES];     // Viable solution patterns
  piece_t *sol_partials[MAX_PIECES];     // Viable solution patterns
  size_t *sol_partials_idxs[MAX_PIECES]; // Viable solution patterns

  size_t sorted_sol_indexes[MAX_PIECES]; // Sorted solution indexes

  // Found Valid soutions
  size_t num_solutions;
  size_t max_solutions;
  solution_t *solutions;

} solutions_t;

status_t init_solutions(solutions_t *sol, const problem_t *problem,
                        struct solution_restrictions restrictions);

status_t init_partial_solution(solutions_t *sol, const problem_t *problem,
                               struct solution_restrictions restrictions,
                               piece_location_t *placed_pieces,
                               size_t n_placed_pieces);

status_t destroy_solutions(solutions_t *sol);

status_t push_solution(solutions_t *sol);

status_t solve_parallel(solutions_t *sol);
status_t solve(solutions_t *sol);

uint64_t make_positions(piece_t piece, piece_properties_t props,
                        board_t problem, piece_t *dest,
                        struct solution_restrictions restrictions);

status_t check_partial_solution(const problem_t *problem,
                                const piece_location_t *pieces, size_t n_pieces,
                                board_t *result);