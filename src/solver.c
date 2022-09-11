#define _GNU_SOURCE // In order to ignore the qsort_r warning

#include <stdio.h>

#include <stdlib.h>

#include "board.h"
#include "config.h"
#include "piece.h"
#include "printing.h"
#include "problem.h"
#include "solver.h"
#include "utils.h"
#include <immintrin.h>
#include <x86intrin.h>

#include <stdbool.h>
#include <string.h>

#ifdef SORT_PATTERNS

int cmpfunc(const void *a, const void *b, void *args) {

  size_t *array = (size_t *)args;

  int val1 = array[*(int *)a];
  int val2 = array[*(int *)b];
  int ret = 0;
  if (val2 < val1) {
    /* swap string positions */
    ret = 1;
  } else if (val2 > val1) {
    ret = -1;
  }

  return ret;
}
#endif

status_t init_partial_solution(solutions_t *sol, const problem_t *problem,
                               struct solution_restrictions restrictions,
                               const piece_location_t *placed_pieces,
                               size_t n_placed_pieces) {
  status_t ret;
  if (n_placed_pieces >= problem->n_pieces) {
    printf("Cant solve for more pieces than the problem has");
    return WRONG_INPUT;
  }

  board_t partial_solution;
  if (n_placed_pieces > 0) {
    ret = check_partial_solution(problem, placed_pieces, n_placed_pieces,
                                 &partial_solution);
    if (ret != STATUS_OK) {
      return ret;
    }
  } else {
    partial_solution = problem->problem;
  }

  sol->problem = problem->problem;
  sol->num_solutions = 0;
  sol->current_level = 0;
  sol->n_pieces = problem->n_pieces; // Number of pieces left

  // Go through all pieces
  for (int i = 0; i < problem->n_pieces; i++) {
    bool placed_piece = false;
    sol->sol_patterns_num[i] = 0;

    // Check if the piece is already placed
    for (int j = 0; j < n_placed_pieces; j++) {
      if (i == placed_pieces[j].piece_id) {
        piece_t p =
            get_piece(problem->pieces, problem->n_pieces, placed_pieces[j]);
        if (p == 0) {
          printf("Invalid piece location\n");
          return INVALID_POSITION;
        }

        // Allocate both piece buffers at the same time
        piece_t *buffers =
            aligned_alloc(CACHE_LINE_SIZE, (4 * sizeof(piece_t) * 3));

        if (buffers == NULL) {
          return MEMORY_ERROR;
        }

        // Assign the two buffer halfs
        sol->sol_patterns[i] = buffers;
        sol->sol_partials[i] = buffers + 4;
        sol->candidate_pattern[i] = buffers + 2 * 4;

        // Set up the 1 pattern
        memset(sol->sol_patterns[i], 0xAA, 4 * sizeof(piece_t));
        sol->sol_patterns[i][0] = p;
        sol->sol_patterns_num[i] = 1;

        placed_piece = true;
        break;
      }
    }

    if (!placed_piece) {
      size_t nearest_mul8 = ((problem->piece_position_num[i] + 8 - 1) / 8) * 8;

      // Allocate both piece buffers at the same time
      piece_t *buffers =
          aligned_alloc(CACHE_LINE_SIZE, (nearest_mul8 * sizeof(piece_t) * 3));

      if (buffers == NULL) {
        printf("Failed ot allocate viable_sub_solutions array.\n");
        return MEMORY_ERROR;
      }

      sol->sol_patterns[i] = buffers;

      sol->sol_partials[i] = buffers + nearest_mul8;
      sol->candidate_pattern[i] = buffers + 2 * nearest_mul8;

      memset(sol->sol_patterns[i], 0xAA, nearest_mul8 * sizeof(piece_t));

      // Optimized sol_patterns positions (eliminating invalid positions)
      sol->sol_patterns_num[i] =
          make_positions(problem->pieces[i], problem->piece_props[i],
                         partial_solution, sol->sol_patterns[i], restrictions);
    }
  }

  // Sort by least number of indexes
  for (size_t i = 0; i < problem->n_pieces; i++) {
    sol->sorted_sol_indexes[i] = i;
  }
#ifdef SORT_PATTERNS
  qsort_r(sol->sorted_sol_indexes, problem->n_pieces,
          sizeof(sol->sorted_sol_indexes[0]), cmpfunc, sol->sol_patterns_num);
#endif

  sol->max_solutions = SOLUTIONS_BUFFER_SIZE;
  sol->solutions = calloc(sol->max_solutions, sizeof(solution_t));
  if (sol->solutions == NULL) {
    printf("Failed to allocate solutions array.\n");
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

  piece_location_t placed_pieces;

  return init_partial_solution(sol, problem, restrictions, &placed_pieces, 0);
}

status_t destroy_solutions(solutions_t *sol) {
  for (int i = 0; i < sol->n_pieces; i++) {
    free(sol->sol_patterns[i]); // Release all buffers
  }
  free(sol->solutions);
  return STATUS_OK;
}

status_t push_solution(solutions_t *sol) {
  // Expand solutions buffer if needed
  // printf("Found solution %ld\n", sol->num_solutions);
  if (sol->num_solutions + 1 >= sol->max_solutions) {
    sol->max_solutions += SOLUTIONS_BUFFER_SIZE;
    // printf("Increasing solutions buffer size to %ld!\n", sol->max_solutions);
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
    sol->solutions[sol->num_solutions].pieces[i] = sol->solution_stack[i];
  }
  sol->num_solutions++;
  // printf("Found solution %ld!\n", sol->num_solutions);
  return STATUS_OK;
}

__attribute__((unused)) static status_t solve_rec(solutions_t *sol,
                                                  board_t problem) {
  const size_t current_level = sol->current_level;
  const size_t current_index = sol->sorted_sol_indexes[current_level];

  const size_t num_patterns = sol->sol_patterns_num[current_index];

  piece_t *p_patterns = sol->sol_patterns[current_index];

  status_t ret;

  for (size_t i = 0; i < num_patterns; i++) {
    board_t pp_and = p_patterns[i] & problem;
    board_t pp_or = p_patterns[i] | problem;

    if (!(pp_and)) {
      sol->solution_stack[current_index] = p_patterns[i];

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

__attribute__((unused)) static status_t solve_rec_simd_old(solutions_t *sol,
                                                           board_t problem) {
  const size_t current_level = sol->current_level;
  const size_t current_index = sol->sorted_sol_indexes[current_level];

  status_t ret;

  __m256i vec_problem = _mm256_set1_epi64x(problem);
  __m256i vec_zero = _mm256_set1_epi64x(0x0000000000000000);

  piece_t pp_and_buffer[4] __attribute__((aligned(CACHE_LINE_SIZE)));
  piece_t pp_or_buffer[4] __attribute__((aligned(CACHE_LINE_SIZE)));

  piece_t *p_patterns = sol->sol_patterns[current_index];

  for (size_t i = 0; i < sol->sol_patterns_num[current_index]; i += 4) {

    __m256i vec_patterns = _mm256_stream_load_si256((__m256i *)p_patterns);

    __m256i vec_pp_and = _mm256_and_si256(vec_problem, vec_patterns);

    // Set to 0xFFFF.. if any is zero
    __m256i vec_test_zero = _mm256_cmpeq_epi64(vec_pp_and, vec_zero);

    // If any was zero, test will be false and we check them one at a time
    if (!_mm256_testz_si256(vec_test_zero, vec_test_zero)) {

      __m256i vec_pp_or = _mm256_or_si256(vec_problem, vec_patterns);

      _mm256_store_si256((__m256i *)&pp_and_buffer, vec_pp_and);

      _mm256_store_si256((__m256i *)&pp_or_buffer, vec_pp_or);

      for (size_t j = 0; j < 4; j++) {

        if (pp_and_buffer[j] == 0) {
          sol->solution_stack[current_index] = p_patterns[j];

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
            if (check_holes(pp_or_buffer[j])) {
              sol->current_level++;
              ret = solve_rec_simd_old(sol, pp_or_buffer[j]);
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
    }

    p_patterns += 4;
  }

  if (current_level > 0)
    sol->current_level--;

  return STATUS_OK;
}

static status_t solve_rec_simd(solutions_t *sol, board_t problem) {

  const size_t current_level = sol->current_level;
  const size_t current_index = sol->sorted_sol_indexes[current_level];
  status_t ret;

  piece_t *p_patterns = sol->sol_patterns[current_index];
  piece_t *p_partials = sol->sol_partials[current_index];
  piece_t *p_candidate_pattern = sol->candidate_pattern[current_index];

  size_t matches = 0;

#if defined(SIMD_AVX2)

  __m256i vec_problem = _mm256_set1_epi64x(problem);
  __m256i vec_zero = _mm256_set1_epi64x(0);
  __m256i vec_idx = _mm256_set_epi64x(3, 2, 1, 0);
  __m256i vec_fours = _mm256_set1_epi64x(4);

  for (size_t i = 0; i < sol->sol_patterns_num[current_index]; i += 4) {

    __m256i vec_patterns = _mm256_stream_load_si256((__m256i *)p_patterns);

    __m256i vec_pp_and = _mm256_and_si256(vec_problem, vec_patterns);

    // Set to 0xFFFF.. if any is zero
    __m256i vec_test_zero = _mm256_cmpeq_epi64(vec_pp_and, vec_zero);

    // printf("%d  ", i);
    // for (int b = 3; b >= 0; b--) {
    //   printf("%d ", 0x01 & (mask >> b));
    // }
    // printf(" PopCnt: %d\n", __popcntd(mask));

    // print_piece(p_patterns[0] | problem, i % 10);

    int mask = _mm256_movemask_pd((__m256d)vec_test_zero);
    if (mask) {

      int num_matches = __popcntd(mask);
      __m256i vec_pp_or = _mm256_or_si256(vec_problem, vec_patterns);
      // 1 1 0 1

      // 11 10 01 00

      // 11 11 10 00

      uint64_t expanded_mask =
          _pdep_u64(mask, 0x001000100010001); // unpack each bit to a byte
      expanded_mask *= 0xFFFF;

      // for (int b = 63; b >= 0; b--) {
      //   printf("%d", 0x01 & (expanded_mask >> b));
      // }
      // printf("\n");

      const uint64_t identity_indices =
          0x0706050403020100; // the identity shuffle for vpermps, packed to one
                              // index per byte
      uint64_t wanted_indices = _pext_u64(identity_indices, expanded_mask);

      // for (int b = 63; b >= 0; b--) {
      //   printf("%d", 0x01 & (wanted_indices >> b));
      // }
      // printf("\n");

      __m128i bytevec = _mm_cvtsi64_si128(wanted_indices);
      __m256i shufmask = _mm256_cvtepu8_epi32(bytevec);

      __m256i filt_partials = _mm256_permutevar8x32_epi32(vec_pp_or, shufmask);
      __m256i filt_candidate =
          _mm256_permutevar8x32_epi32(vec_patterns, shufmask);

      _mm256_storeu_si256((__m256i *)p_partials, filt_partials);
      _mm256_storeu_si256((__m256i *)p_candidate_pattern, filt_candidate);

      p_partials += num_matches;
      p_candidate_pattern += num_matches;
      matches += num_matches;
    }

    p_patterns += 4;
    vec_idx = _mm256_add_epi64(vec_idx, vec_fours);
  }

#elif defined(SIMD_AVX512)

  __m512i vec_problem = _mm512_set1_epi64(problem);
  __m512i vec_zero = _mm512_set1_epi64(0x0000000000000000);
  __m512i vec_idx = _mm512_set_epi64(7, 6, 5, 4, 3, 2, 1, 0);
  __m512i vec_eights = _mm512_set1_epi64(8);

  for (size_t i = 0; i < sol->sol_patterns_num[current_index]; i += 8) {

    __m512i vec_patterns = _mm512_stream_load_si512((__m256i *)p_patterns);

    __m512i vec_pp_and = _mm512_and_si512(vec_problem, vec_patterns);

    // Set to 0xFFFF.. if any is zero
    // __m512i vec_test_zero = _mm256_cmpeq_epi64(vec_pp_and, vec_zero);

    __mmask8 mask = _mm512_cmpeq_epi64_mask(vec_pp_and, vec_zero);

    // printf("%d  ", i);
    // for (int b = 3; b >= 0; b--) {
    //   printf("%d ", 0x01 & (mask >> b));
    // }
    // printf(" PopCnt: %d\n", __popcntd(mask));

    // print_piece(p_patterns[0] | problem, i % 10);

    // int mask = _mm256_movemask_pd((__m256d)vec_test_zero);
    if (mask) {

      int num_matches = __popcntd(mask);
      __m512i vec_pp_or = _mm512_or_si512(vec_problem, vec_patterns);

      _mm512_mask_compressstoreu_epi64(p_partials, mask, vec_pp_or);
      _mm512_mask_compressstoreu_epi64(p_partials_idx, mask, vec_idx);

      p_partials += num_matches;
      p_partials_idx += num_matches;
      matches += num_matches;
    }

    p_patterns += 8;
    vec_idx = _mm512_add_epi64(vec_idx, vec_eights);
  }

#else
#error "Cant Use SIMD without AVX support"
#endif

  p_partials = sol->sol_partials[current_index];
  p_candidate_pattern = sol->candidate_pattern[current_index];

  for (int i = 0; i < matches; i++) {
    // printf("Level %d, IDX: %d \n", current_level, p_partials_idx[i]);
    // print_piece(p_partials[i], current_level);

    sol->solution_stack[current_index] =
        sol->candidate_pattern[current_index][i];

    if (p_partials[i] == 0xFFFFFFFFFFFFFFFF) {
      // printf("Found Solution\n");
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
      // printf("Checking Holes\n");
      if (check_holes(p_partials[i])) {
        // printf("Holes passed, recursing\n");
        sol->current_level++;
        ret = solve_rec_simd(sol, p_partials[i]);
        // printf("Done recursing %d\n", ret);

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

status_t solve(solutions_t *sol) {
  status_t res = STATUS_OK;

#ifdef USE_SIMD

#ifdef USE_OLD_SIMD
  res = solve_rec_simd_old(sol, sol->problem);

#else
  res = solve_rec_simd(sol, sol->problem);
#endif

#else
  res = solve_rec(sol, sol->problem);
#endif
  return res;
}

status_t solve_parallel(solutions_t *sol) {
  status_t res = STATUS_OK;

  const size_t current_level = sol->current_level;
  const size_t current_index = sol->sorted_sol_indexes[current_level];
  const size_t n_patterns_first_level = sol->sol_patterns_num[current_index];
  board_t problem = sol->problem;

  solutions_t *sol_works = calloc(sizeof(solutions_t), n_patterns_first_level);
  if (sol_works == NULL) {
    return MEMORY_ERROR;
  }

  for (size_t i = 0; i < n_patterns_first_level; i++) {
    // Copy base
    memcpy(&sol_works[i], sol, sizeof(solutions_t));

    // Give each an individual solutions buffer
    sol_works[i].solutions =
        calloc(sol_works[i].max_solutions, sizeof(solution_t));

    for (int j = 0; j < sol->n_pieces; j++) {
      size_t nearest_mul8 =
          ((sol_works[i].sol_patterns_num[j] + 8 - 1) / 8) * 8;

      piece_t *buffers =
          aligned_alloc(CACHE_LINE_SIZE, 2 * nearest_mul8 * sizeof(piece_t));

      sol_works[i].sol_partials[j] = buffers;
      sol_works[i].candidate_pattern[j] = buffers + nearest_mul8;
    }

    if (sol_works[i].solutions == NULL) {
      printf("Failed to allocate solutions array.\n");
      return MEMORY_ERROR;
    }
  }

#pragma omp parallel for schedule(dynamic)
  for (size_t i = 0; i < n_patterns_first_level; i++) {
    if ((sol_works[i].sol_patterns[current_index][i] & problem) == 0) {
      sol_works[i].solution_stack[current_index] =
          sol_works[i].sol_patterns[current_index][i];

      sol_works[i].current_level++;

#ifdef USE_SIMD
      solve_rec_simd(&sol_works[i],
                     sol_works[i].sol_patterns[current_index][i] | problem);
#else
      solve_rec(&sol_works[i],
                sol_works[i].sol_patterns[current_index][i] | problem);
#endif
    }
  }

  for (size_t i = 0; i < n_patterns_first_level; i++) {

    // Get current solutions end
    size_t solutions_end = sol->num_solutions;

    // Increase buffer if required
    sol->num_solutions += sol_works[i].num_solutions;
    if (sol->num_solutions + 1 >= sol->max_solutions) {
      sol->max_solutions += SOLUTIONS_BUFFER_SIZE;
      // printf("Increasing solutions buffer size to %ld!\n",
      // sol->max_solutions);
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

    // Copy the solutions over
    memcpy(&sol->solutions[solutions_end], sol_works[i].solutions,
           sizeof(solution_t) * sol_works[i].num_solutions);

    for (int j = 0; j < sol->n_pieces; j++) {
      free(sol_works[i].sol_partials[j]);
    }

    free(sol_works[i].solutions);
  }
  free(sol_works);

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
            if (dest != NULL) {
              dest[positions] = current;
            }
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
