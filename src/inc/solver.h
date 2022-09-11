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

/**
 * Structure representing a solution with an array of the solution pieces
 */
typedef struct solution_t {
  piece_t pieces[MAX_PIECES];
} solution_t;

typedef struct solver_t {
  // Problem (with the target invalidated)
  board_t problem;

  // Number of pieces available
  size_t n_pieces;
  size_t num_placed;

  // Array current stack of solutions pattern
  piece_t solution_stack[MAX_PIECES];

  size_t num_piece_positions[MAX_PIECES];    // Total number of subsolutions
  piece_t *piece_positions[MAX_PIECES];      // Viable solution patterns
  piece_t *placed_viable_pieces[MAX_PIECES]; // Viable solution patterns
  piece_t *viable_pieces[MAX_PIECES];        // Viable solution patterns

  size_t sorted_pieces_idxs[MAX_PIECES]; // Sorted solution indexes

  // Found Valid soutions
  size_t num_solutions;
  size_t max_solutions;
  size_t face_up_solutions;
  size_t face_down_solutions;
  solution_t *solutions;

  uint32_t date_solutions[64][64];

} solver_t;

status_t init_all_dates_solution(solver_t *sol, const problem_t *problem,
                                 struct solution_restrictions restrictions);

status_t init_solutions(solver_t *sol, const problem_t *problem,
                        struct solution_restrictions restrictions);

status_t init_partial_solution(solver_t *sol, const problem_t *problem,
                               struct solution_restrictions restrictions,
                               const piece_location_t *placed_pieces,
                               size_t n_placed_pieces);

status_t destroy_solutions(solver_t *sol);

status_t push_solution(solver_t *sol);

status_t enumerate_solutions(solver_t *sol);
status_t solve_parallel(solver_t *sol);
status_t solve(solver_t *sol);

uint64_t make_positions(piece_t piece, piece_properties_t props,
                        board_t problem, piece_t *dest,
                        struct solution_restrictions restrictions);

status_t check_partial_solution(const problem_t *problem,
                                const piece_location_t *pieces, size_t n_pieces,
                                board_t *result);