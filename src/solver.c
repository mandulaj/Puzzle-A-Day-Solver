#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "piece.h"
#include "problem.h"
#include "solver.h"
#include "utils.h"
#include <immintrin.h>
#include <stdbool.h>
#include <string.h>

status_t init_partial_solution(solutions_t *sol, const problem_t *problem,
                               struct solution_restrictions restrictions,
                               piece_location_t *placed_pieces,
                               size_t n_placed_pieces) {
  status_t ret;
  if (n_placed_pieces >= problem->n_pieces) {
    printf("Cant solve for more pieces than the problem has");
    return WRONG_INPUT;
  }

  board_t partial_solution;
  ret = check_partial_solution(problem, placed_pieces, n_placed_pieces,
                               &partial_solution);
  if (ret != STATUS_OK) {
    return ret;
  }

  sol->problem = problem->problem;
  sol->num_solutions = 0;
  sol->current_level = 0;
  sol->n_pieces = problem->n_pieces; // Number of pieces left

  // Go through all pieces
  for (int i = 0; i < problem->n_pieces; i++) {
    bool placed_piece = false;
    sol->sol_patterns_num[i] = 0;
    sol->sol_pattern_index[i] = 0;

    // Check if the piece is already placed
    for (int j = 0; j < n_placed_pieces; j++) {
      if (i == placed_pieces[j].piece_id) {
        piece_t p =
            get_piece(problem->pieces, problem->n_pieces, placed_pieces[j]);
        if (p == 0) {
          printf("Invalid piece location\n");
          return INVALID_POSITION;
        }

        sol->sol_patterns[i] = aligned_alloc(32, 4 * sizeof(piece_t));
        memset(sol->sol_patterns[i], 0xFF, 4 * sizeof(piece_t));
        sol->sol_patterns[i][0] = p;
        sol->sol_patterns_num[i] = 1;
        placed_piece = true;
        break;
      }
    }

    if (!placed_piece) {

      sol->sol_patterns[i] =
          aligned_alloc(32, problem->piece_position_num[i] * sizeof(piece_t));
      if (sol->sol_patterns[i] == NULL) {
        printf("Failed ot allocate viable_sub_solutions array.\n");
        return MEMORY_ERROR;
      }
      memset(sol->sol_patterns[i], 0xFF,
             problem->piece_position_num[i] * sizeof(piece_t));

      // Optimized sol_patterns positions (eliminating invalid positions)
      sol->sol_patterns_num[i] =
          make_positions(problem->pieces[i], problem->piece_props[i],
                         partial_solution, sol->sol_patterns[i], restrictions);
    }
  }

  sol->max_solutions = SOLUTIONS_BUFFER_SIZE;
  sol->solutions = calloc(sol->max_solutions, sizeof(solution_t));
  if (sol->solutions == NULL) {
    printf("Failed ot allocate solutions array.\n");
    return MEMORY_ERROR;
  }
  return STATUS_OK;
}

status_t init_solutions(solutions_t *sol, const problem_t *problem,
                        struct solution_restrictions restrictions) {
  // Initialize problem formulation

  // printf("Popcount: %d\n",
  //  __builtin_popcountl(problem->problem ^ problem->blank));
  // print_raw(problem->problem ^ problem->blank);

  sol->problem = problem->problem;
  sol->num_solutions = 0;
  sol->current_level = 0;
  sol->n_pieces = problem->n_pieces;

  // Allocate memory for subsolutions
  for (int i = 0; i < sol->n_pieces; i++) {
    sol->sol_patterns_num[i] = 0;
    sol->sol_pattern_index[i] = 0;
    sol->sol_patterns[i] =
        aligned_alloc(32, problem->piece_position_num[i] * sizeof(piece_t));
    if (sol->sol_patterns[i] == NULL) {
      printf("Failed ot allocate viable_sub_solutions array.\n");
      exit(1);
    }
    memset(sol->sol_patterns[i], 0xFF,
           problem->piece_position_num[i] * sizeof(piece_t));

    // Optimized sol_patterns positions (eliminating invalid positions)
    sol->sol_patterns_num[i] =
        make_positions(problem->pieces[i], problem->piece_props[i],
                       sol->problem, sol->sol_patterns[i], restrictions);
  }

  sol->max_solutions = SOLUTIONS_BUFFER_SIZE;
  sol->solutions = calloc(sol->max_solutions, sizeof(solution_t));
  if (sol->solutions == NULL) {
    printf("Failed ot allocate solutions array.\n");
    exit(1);
  }
  return STATUS_OK;
}

status_t destroy_solutions(solutions_t *sol) {
  for (int i = 0; i < sol->n_pieces; i++) {
    free(sol->sol_patterns[i]);
  }
  free(sol->solutions);
  return STATUS_OK;
}

status_t push_solution(solutions_t *sol) {

  // Expand solutions buffer if needed
  if (sol->num_solutions + 1 >= sol->max_solutions) {
    // printf("Increasing solutions buffer size!\n");
    sol->max_solutions += SOLUTIONS_BUFFER_SIZE;
    if (sol->max_solutions > MAX_NUM_SOLUTIONS) {
      printf("Number of solutions over the limit, terminating!\n");
      return WARNING;
    }
    sol->solutions =
        realloc(sol->solutions, sol->max_solutions * sizeof(solution_t));
    if (sol->solutions == NULL) {
      printf("Failed to increasing number of solutions buffer!\n");
      exit(1);
    }
  }

  for (size_t i = 0; i < sol->n_pieces; i++) {
    sol->solutions[sol->num_solutions].pieces[i] =
        sol->sol_patterns[i][sol->sol_pattern_index[i]];
  }
  sol->num_solutions++;
  // printf("Found solution %ld!\n", sol->num_solutions);
  return STATUS_OK;
}

__attribute__((unused)) static status_t solve_rec(solutions_t *sol,
                                                  board_t problem) {
  const size_t current_level = sol->current_level;
  const size_t num_patterns = sol->sol_patterns_num[current_level];

  piece_t *p_patterns = sol->sol_patterns[current_level];

  status_t ret;

  for (size_t i = 0; i < num_patterns; i++) {
    board_t pp_and = p_patterns[i] & problem;
    board_t pp_or = p_patterns[i] | problem;

    if (!(pp_and)) {
      sol->sol_pattern_index[current_level] = i;

      // We placed the last peice so have a full board, push it
      if (pp_or == 0xFFFFFFFFFFFFFFFF) {
        ret = push_solution(sol);
        if (ret) {
          if (ret == WARNING)
            break;
          return ret;
        }
        // We are at the end, we will not fit anywhere else
        sol->current_level--;
        return STATUS_OK;
      } else {
        // Place next piece
        sol->current_level++;
        ret = solve_rec(sol, pp_or);
        if (ret) {
          if (ret == WARNING) {
            break;
          }
          printf("Catching error\n");
          return ret;
        }
      }
    }
  }

  if (current_level > 0)
    sol->current_level--;
  return STATUS_OK;
}

static status_t solve_rec_smdi(solutions_t *sol, board_t problem) {
  size_t current_level = sol->current_level;
  status_t ret;

  __m256i vec_problem = _mm256_set1_epi64x(problem);
  __m256i vec_zero = _mm256_set1_epi64x(0x0000000000000000);

  piece_t pp_and_buffer[4] __attribute__((aligned(32)));
  piece_t pp_or_buffer[4] __attribute__((aligned(32)));

  piece_t *p_patterns = sol->sol_patterns[current_level];

  for (size_t i = 0; i < sol->sol_patterns_num[current_level]; i += 4) {
    __m256i vec_patterns = _mm256_load_si256((__m256i *)p_patterns);

    __m256i vec_pp_and = _mm256_and_si256(vec_problem, vec_patterns);

    // Set to 0xFFFF.. if any is zero
    __m256i vec_test_zero = _mm256_cmpeq_epi64(vec_pp_and, vec_zero);

    // If any was zero, test will be false and we check them one at a time
    if (!_mm256_testz_si256(vec_test_zero, vec_test_zero)) {

      _mm256_store_si256((__m256i *)&pp_and_buffer, vec_pp_and);

      __m256i vec_pp_or = _mm256_or_si256(vec_problem, vec_patterns);
      _mm256_store_si256((__m256i *)&pp_or_buffer, vec_pp_or);

      for (size_t j = 0; j < 4; j++) {

        if (pp_and_buffer[j] == 0) {
          sol->sol_pattern_index[current_level] = i + j;

          if (pp_or_buffer[j] == 0xFFFFFFFFFFFFFFFF) {
            ret = push_solution(sol);
            if (ret) {
              if (ret == WARNING)
                break;
              return ret;
            }

            sol->current_level--;
            return STATUS_OK; // We are at the end, we will not fit anywhere
                              // else
          } else {
            sol->current_level++;
            ret = solve_rec_smdi(sol, pp_or_buffer[j]);
            if (ret) {
              if (ret == WARNING) {
                break;
              }
              printf("Catching error\n");
              return ret;
            }
          }
        }
      }
    }

    p_patterns += 4;
  }

  if (current_level > 0)
    sol->current_level--;
  return STATUS_OK;
}

status_t solve(solutions_t *sol) {
  status_t res;
#if defined(USE_SMDI)
  res = solve_rec_smdi(sol, sol->problem);
#else
  res = solve_rec(sol, sol->problem);
#endif
  return res;
}

uint64_t make_positions(piece_t piece, piece_properties_t props,
                        board_t problem, piece_t *dest,
                        struct solution_restrictions restrictions) {
  uint64_t positions = 0;
  uint64_t invalid = 0;
  piece_t current = piece;
  piece_t old = (board_t)0x00;
  bool done_asymetric = true;

  if (restrictions.use_faceup && restrictions.use_facedown) {
    // Allow both symetries
    done_asymetric = false;
  }

  // Do both symetries
  do {
    if (props.asymetric && restrictions.use_facedown &&
        !restrictions.use_faceup) {
      current = piece_origin(piece_flip(piece)); // Flip and set to origin
    }
    // Do all rotations
    for (int i = 0; i < props.rotations; i++) {
      // Do all positions
      do {
        current = piece_place_left(current);
        do {
          old = current;
          // print_2_raw(current, problem);
          if (current & problem) {
            invalid += 1;
          } else {
            dest[positions] = current;
            positions += 1;
          }

          current = piece_sft_right(current);

        } while (current != old);
        current = piece_sft_down(current);
      } while (current != old);

      current = piece_origin(piece_rotate(current));
    }

    if (props.asymetric && !done_asymetric) {
      done_asymetric = true;
      current = piece_origin(piece_flip(piece)); // Flip and set to origin
    } else {
      break;
    }

  } while (1);
  // printf("Positions: %ld\n", positions);
  return positions;
}

status_t check_partial_solution(const problem_t *problem,
                                const piece_location_t *pieces, size_t n_pieces,
                                board_t *result) {
  piece_t prob = problem->problem;
  for (int i = 0; i < n_pieces; i++) {

    // Check for duplicates
    for (int j = i + 1; j < n_pieces; j++) {
      if (pieces[i].piece_id == pieces[j].piece_id) {
        return DUPLICATE_PIECE;
      }
    }

    // Check if piece is in valid location
    piece_t p = get_piece(problem->pieces, problem->n_pieces, pieces[i]);

    if (p & prob) {
      return INVALID_POSITION;
    }
    prob |= p;
  }
  if (result != NULL) {
    *result = prob;
  }
  return STATUS_OK;
}