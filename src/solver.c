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

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

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

// static void reshuffle(size_t *indexes, size_t n) {

//   size_t tmp[MAX_PIECES];
//   memcpy(tmp, indexes, n * sizeof(size_t));

//   if (n % 2 == 0) {
//     //[ 0 1 2 3 4 5 6 7 ]
//     //[ 6 4 2 0 1 3 5 7 ]
//     int indx = 0;
//     for (int i = (n / 2) - 1; i >= 0; i--) {
//       indexes[i] = tmp[indx];
//       indx += 2;
//     }
//     indx = 1;
//     for (int i = (n / 2); i < n; i++) {
//       indexes[i] = tmp[indx];
//       indx += 2;
//     }
//   } else {
//     //[ 0 1 2 3 4 5 6 7 8 ]
//     //[ 7 5 3 1 0 2 4 6 8]
//     int indx = 0;
//     for (int i = (n / 2); i < n; i++) {
//       indexes[i] = tmp[indx];
//       indx += 2;
//     }
//     indx = 1;
//     for (int i = (n / 2) - 1; i >= 0; i--) {
//       indexes[i] = tmp[indx];
//       indx += 2;
//     }
//   }
// }

#endif

status_t init_all_dates_solution(solver_t *sol, const problem_t *problem,
                                 struct solution_restrictions restrictions) {
  status_t res = init_partial_solution(sol, problem, restrictions, NULL, 0);
  sol->date_solutions = calloc(64 * 64, sizeof(*sol->date_solutions));
  if (sol->date_solutions == NULL)
    return ERROR;
  else
    return res;
}

status_t init_partial_solution(solver_t *sol, const problem_t *problem,
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
  sol->date_solutions = NULL;
  sol->num_solutions = 0;
  sol->num_placed = 0;
  sol->n_pieces = problem->n_pieces; // Number of pieces left

  // Go through all pieces
  for (int i = 0; i < problem->n_pieces; i++) {
    bool placed_piece = false;
    sol->num_piece_positions[i] = 0;

    // Check if the piece is already placed
    for (int j = 0; j < n_placed_pieces; j++) {
      if (i == placed_pieces[j].piece_id) {
        piece_t p =
            get_piece(problem->pieces, problem->n_pieces, placed_pieces[j]);
        if (p == 0) {
          printf("Invalid piece location\n");
          return INVALID_POSITION;
        }

        // Add piece to solution stack
        sol->solution_stack[i] = p;

        // Allocate both piece buffers at the same time
        piece_t *buffers =
            aligned_alloc(CACHE_LINE_SIZE, (4 * sizeof(piece_t) * 3));

        if (buffers == NULL) {
          return MEMORY_ERROR;
        }

        // Assign the two buffer halfs
        sol->piece_positions[i] = buffers;
        sol->viable_pieces[i] = buffers + 4;
        sol->placed_viable_pieces[i] = buffers + 2 * 4;

        // Set up the 1 pattern
        memset(sol->piece_positions[i], 0xAA, 4 * sizeof(piece_t));
        sol->piece_positions[i][0] = p;
        sol->num_piece_positions[i] = 1;

        placed_piece = true;
        sol->num_placed++;
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

      sol->piece_positions[i] = buffers;

      sol->viable_pieces[i] = buffers + nearest_mul8;
      sol->placed_viable_pieces[i] = buffers + 2 * nearest_mul8;

      memset(sol->piece_positions[i], 0xAA, nearest_mul8 * sizeof(piece_t));

      // Optimized sol_patterns positions (eliminating invalid positions)
      sol->num_piece_positions[i] = make_positions(
          problem->pieces[i], problem->piece_props[i], partial_solution,
          sol->piece_positions[i], restrictions);
    }
  }

  // Sort by least number of indexes
  for (size_t i = 0; i < problem->n_pieces; i++) {
    sol->sorted_pieces_idxs[i] = i;
  }
#ifdef SORT_PATTERNS
  qsort_r(sol->sorted_pieces_idxs, problem->n_pieces,
          sizeof(sol->sorted_pieces_idxs[0]), cmpfunc,
          sol->num_piece_positions);

  // reshuffle(sol->sorted_sol_indexes, problem->n_pieces);
  // for (int i = 0; i < problem->n_pieces; i++) {
  //   printf("%d, %ld\n", i,
  //   sol->sol_patterns_num[sol->sorted_sol_indexes[i]]);
  // }

#endif

  sol->max_solutions = SOLUTIONS_BUFFER_SIZE;
  sol->solutions = calloc(sol->max_solutions, sizeof(solution_t));
  if (sol->solutions == NULL) {
    printf("Failed to allocate solutions array.\n");
    return MEMORY_ERROR;
  }
  return STATUS_OK;
}

status_t init_solutions(solver_t *sol, const problem_t *problem,
                        struct solution_restrictions restrictions) {
  // Initialize problem formulation

  return init_partial_solution(sol, problem, restrictions, NULL, 0);
}

status_t destroy_solutions(solver_t *sol) {
  for (int i = 0; i < sol->n_pieces; i++) {
    free(sol->piece_positions[i]); // Release all buffers
  }
  free(sol->solutions);
  if (sol->date_solutions != NULL) {
    free(sol->date_solutions);
  }
  return STATUS_OK;
}

status_t push_solution(solver_t *sol) {
  // Expand solutions buffer if needed
  // printf("Found solution %ld\n", sol->num_solutions);
  sol->num_solutions++;

  if (sol->num_solutions >= sol->max_solutions) {
    sol->max_solutions += SOLUTIONS_BUFFER_SIZE;
    // printf("Increasing solutions buffer size to %ld!\n", sol->max_solutions);
    if (sol->max_solutions > MAX_NUM_SOLUTIONS) {
      printf("Number of solutions over the limit, terminating!\n");
      sol->num_solutions--;
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
  // printf("Found solution %ld!\n", sol->num_solutions);
  return STATUS_OK;
}

status_t add_date_solution(solver_t *sol, board_t b) {
  // Expand solutions buffer if needed
  // printf("Found solution %ld\n", sol->num_solutions);
  size_t pos1;
  size_t pos2;
  get_date(b, &pos1, &pos2);

  if (pos1 == 1000 || pos2 == 1000) {
    // invalid date
  } else {
    // printf("%ld, %ld\n", pos1, pos2);
    // print_raw_color(b, 0);
    sol->date_solutions[pos1 * 64 + pos2]++;
  }
  // printf("%d/%d\n", month, day);

  return STATUS_OK;
}

static status_t solve_rec(solver_t *sol, board_t problem,
                          size_t current_level) {

  const size_t current_index = sol->sorted_pieces_idxs[current_level];
  status_t ret;

  const size_t num_positions = sol->num_piece_positions[current_index];
  const piece_t *p_patterns = sol->piece_positions[current_index];
  __builtin_prefetch(p_patterns);
  const piece_t *p_patterns_end = p_patterns + num_positions;
  size_t matches = 0;

  piece_t *p_viable = sol->viable_pieces[current_index];
  piece_t *p_placed_viable = sol->placed_viable_pieces[current_index];

#if defined(SIMD_AVX2)

  const __m256i vec_problem = _mm256_set1_epi64x(problem);
  const __m256i vec_zero = _mm256_set1_epi64x(0);

  do {

    __m256i vec_patterns = _mm256_load_si256((__m256i *)p_patterns);
    p_patterns += 4;
    __builtin_prefetch(p_patterns);

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

      __m256i filt_viable = _mm256_permutevar8x32_epi32(vec_patterns, shufmask);
      __m256i filt_placed_viable =
          _mm256_permutevar8x32_epi32(vec_pp_or, shufmask);

      _mm256_storeu_si256((__m256i *)p_viable, filt_viable);
      _mm256_storeu_si256((__m256i *)p_placed_viable, filt_placed_viable);

      p_viable += num_matches;
      p_placed_viable += num_matches;
      matches += num_matches;
    }

  } while (p_patterns < p_patterns_end);

#elif defined(SIMD_AVX512)

  const __m512i vec_problem = _mm512_set1_epi64(problem);
  const __m512i vec_zero = _mm512_set1_epi64(0x0000000000000000);

  for (size_t i = 0; i < num_positions; i += 8) {

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

      _mm512_mask_compressstoreu_epi64(p_viable, mask, vec_patterns);
      _mm512_mask_compressstoreu_epi64(p_placed_viable, mask, vec_pp_or);

      p_viable += num_matches;
      p_placed_viable += num_matches;
      matches += num_matches;
    }

    p_patterns += 8;
  }

#else
  for (size_t i = 0; i < num_positions; i++) {
    board_t pp_and = p_patterns[i] & problem;
    board_t pp_or = p_patterns[i] | problem;

    if (!pp_and) {
      *p_placed_viable++ = pp_or;
      *p_viable++ = p_patterns[i];
    }
  }

#endif

  p_viable = sol->viable_pieces[current_index];
  p_placed_viable = sol->placed_viable_pieces[current_index];

  if (likely(current_level + 1 < sol->n_pieces)) {
    for (int i = 0; i < matches; i++) {
      // printf("Level %d, IDX: %d \n", current_level, p_partials_idx[i]);
      // print_piece(p_partials[i], current_level);

      sol->solution_stack[current_index] = p_viable[i];

      if (check_holes(p_placed_viable[i])) {
        ret = solve_rec(sol, p_placed_viable[i], current_level + 1);

        if (ret) {
          if (ret == WARNING) {
            break;
          }
          printf("Catching error\n");
          return ret;
        }
      }
    }
  } else {
    for (int i = 0; i < matches; i++) {
      sol->solution_stack[current_index] = p_viable[i];

      ret = push_solution(sol);
      if (ret) {
        if (ret == WARNING)
          break;
        return ret;
      }
    }
  }

  return STATUS_OK;
}

static status_t enum_rec_simd(solver_t *sol, board_t problem,
                              size_t current_level) {

  const size_t current_index = sol->sorted_pieces_idxs[current_level];
  status_t ret;

  piece_t *p_patterns = sol->piece_positions[current_index];
  piece_t *p_partials = sol->placed_viable_pieces[current_index];
  // piece_t *p_partials_idx = sol->sol_partials_idxs[current_index];

  size_t matches = 0;

#if defined(SIMD_AVX2)

  __m256i vec_problem = _mm256_set1_epi64x(problem);
  __m256i vec_zero = _mm256_set1_epi64x(0);

  for (size_t i = 0; i < sol->num_piece_positions[current_index]; i += 4) {

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
          0x0706050403020100; // the identity shuffle for vpermps, packed to
                              // one index per byte
      uint64_t wanted_indices = _pext_u64(identity_indices, expanded_mask);

      // for (int b = 63; b >= 0; b--) {
      //   printf("%d", 0x01 & (wanted_indices >> b));
      // }
      // printf("\n");

      __m128i bytevec = _mm_cvtsi64_si128(wanted_indices);
      __m256i shufmask = _mm256_cvtepu8_epi32(bytevec);

      __m256i filt_partials = _mm256_permutevar8x32_epi32(vec_pp_or, shufmask);

      _mm256_storeu_si256((__m256i *)p_partials, filt_partials);
      p_partials += num_matches;
      matches += num_matches;
    }

    p_patterns += 4;
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

  p_partials = sol->placed_viable_pieces[current_index];
  // p_partials_idx = sol->sol_partials_idxs[current_index];

  for (int i = 0; i < matches; i++) {
    // printf("Level %d, IDX: %d \n", current_level, p_partials_idx[i]);
    // print_piece(p_partials[i], current_level);

    // sol->sol_pattern_index[current_index] = p_partials_idx[i];

    if (current_level < sol->n_pieces - 1) {
      // printf("Checking Holes\n");
      ret = enum_rec_simd(sol, p_partials[i], current_level + 1);
      // printf("Holes passed, recursing\n");
      // printf("Done recursing %d\n", ret);

      if (ret) {
        if (ret == WARNING) {
          break;
        }
        printf("Catching error\n");
        return ret;
      }

    } else {
      // printf("Found Solution\n");
      ret = add_date_solution(sol, p_partials[i]);
      if (ret) {
        if (ret == WARNING)
          break;
        return ret;
      }
    }
  }

  return STATUS_OK;
}

status_t enumerate_solutions(solver_t *sol) {
  return enum_rec_simd(sol, sol->problem, sol->num_placed);
}

status_t enumerate_solutions_parallel(solver_t *sol) {
  status_t res = STATUS_OK;

  const size_t current_level = sol->num_placed;
  const size_t current_index = sol->sorted_pieces_idxs[current_level];
  const size_t n_patterns_first_level = sol->num_piece_positions[current_index];
  board_t problem = sol->problem;

  solver_t *sol_works = calloc(sizeof(solver_t), n_patterns_first_level);
  if (sol_works == NULL) {
    return MEMORY_ERROR;
  }

  for (size_t i = 0; i < n_patterns_first_level; i++) {
    // Copy base
    memcpy(&sol_works[i], sol, sizeof(solver_t));

    for (int j = 0; j < sol->n_pieces; j++) {
      size_t nearest_mul8 =
          ((sol_works[i].num_piece_positions[j] + 8 - 1) / 8) * 8;

      piece_t *buffers =
          aligned_alloc(CACHE_LINE_SIZE, 2 * nearest_mul8 * sizeof(piece_t));

      sol_works[i].viable_pieces[j] = buffers;
      sol_works[i].placed_viable_pieces[j] = buffers + nearest_mul8;

      if (buffers == NULL) {
        printf("Failed to allocate solutions array.\n");
        return MEMORY_ERROR;
      }
    }

    sol_works[i].date_solutions =
        calloc(64 * 64, sizeof(*sol_works[i].date_solutions));
    if (sol_works[i].date_solutions == NULL) {
      printf("Failed to allocate date_solutions array.\n");
      return MEMORY_ERROR;
    }
  }

#pragma omp parallel for schedule(dynamic)
  for (size_t i = 0; i < n_patterns_first_level; i++) {
    if ((sol_works[i].piece_positions[current_index][i] & problem) == 0) {
      enum_rec_simd(&sol_works[i],
                    problem | sol_works[i].piece_positions[current_index][i],
                    current_level + 1);
    }
  }

  for (size_t i = 0; i < n_patterns_first_level; i++) {

    // Increase buffer if required
    sol->num_solutions += sol_works[i].num_solutions;

    for (int pos1 = 0; pos1 < 64; pos1++)
      for (int pos2 = 0; pos2 < 64; pos2++) {

        sol->date_solutions[pos1 * 64 + pos2] +=
            sol_works[i].date_solutions[pos1 * 64 + pos2];
      }

    for (int j = 0; j < sol->n_pieces; j++) {
      free(sol_works[i].viable_pieces[j]);
    }
    free(sol_works[i].date_solutions);
  }
  free(sol_works);

  return res;
}

status_t solve(solver_t *sol) {
  status_t res;

  res = solve_rec(sol, sol->problem, sol->num_placed);

  return res;
}

status_t solve_parallel(solver_t *sol) {
  status_t res = STATUS_OK;

  const size_t current_level = sol->num_placed;
  const size_t current_index = sol->sorted_pieces_idxs[current_level];
  const size_t n_patterns_first_level = sol->num_piece_positions[current_index];
  board_t problem = sol->problem;

  solver_t *sol_works = calloc(sizeof(solver_t), n_patterns_first_level);
  if (sol_works == NULL) {
    return MEMORY_ERROR;
  }

  for (size_t i = 0; i < n_patterns_first_level; i++) {
    // Copy base
    memcpy(&sol_works[i], sol, sizeof(solver_t));

    // Give each an individual solutions buffer
    sol_works[i].solutions =
        calloc(sol_works[i].max_solutions, sizeof(solution_t));

    for (int j = 0; j < sol->n_pieces; j++) {
      size_t nearest_mul8 =
          ((sol_works[i].num_piece_positions[j] + 8 - 1) / 8) * 8;

      piece_t *buffers =
          aligned_alloc(CACHE_LINE_SIZE, 2 * nearest_mul8 * sizeof(piece_t));

      if (buffers == NULL) {
        printf("Failed to allocate viable array.\n");
        return MEMORY_ERROR;
      }
      sol_works[i].viable_pieces[j] = buffers;
      sol_works[i].placed_viable_pieces[j] = buffers + nearest_mul8;
    }

    if (sol_works[i].solutions == NULL) {
      printf("Failed to allocate solutions array.\n");
      return MEMORY_ERROR;
    }
  }

#pragma omp parallel for schedule(dynamic)
  for (size_t i = 0; i < n_patterns_first_level; i++) {
    if ((sol_works[i].piece_positions[current_index][i] & problem) == 0) {
      sol_works[i].solution_stack[current_index] =
          sol_works[i].piece_positions[current_index][i];

      solve_rec(&sol_works[i],
                sol_works[i].piece_positions[current_index][i] | problem,
                sol->num_placed + 1);
    }
  }

  for (size_t i = 0; i < n_patterns_first_level; i++) {

    // Get current solutions end
    size_t solutions_end = sol->num_solutions;

    // Increase buffer if required
    sol->num_solutions += sol_works[i].num_solutions;

    if (sol->num_solutions >= sol->max_solutions) {
      // Assign next multiple of SOLUTIONS_BUFFER_SIZE
      sol->max_solutions = ((sol->num_solutions + SOLUTIONS_BUFFER_SIZE - 1) /
                            SOLUTIONS_BUFFER_SIZE) *
                           SOLUTIONS_BUFFER_SIZE;

      if (sol->max_solutions > MAX_NUM_SOLUTIONS) {
        printf("Number of solutions over the limit, terminating!\n");
        sol->num_solutions -= sol_works[i].num_solutions;
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
      free(sol_works[i].viable_pieces[j]);
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
