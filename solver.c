#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "piece.h"
#include "problem.h"
#include "solver.h"
#include <immintrin.h>
#include <omp.h>
#include <stdbool.h>
#include <string.h>

#define N_PIECES 8

// #define STANDARD

#ifdef STANDARD
// Patterns of the 8 shapes
const piece_t PIECES[N_PIECES] = {0xE0E0000000000000, 0x8080E00000000000,
                                  0x20E0800000000000, 0xA0E0000000000000,
                                  0x80F0000000000000, 0x20F0000000000000,
                                  0x30E0000000000000, 0xC0E0000000000000};

// Rotation and symetry porperties of shapes
const piece_properties_t PIECE_PROPS[N_PIECES] = {
    {2, false}, {4, false}, {2, true}, {4, false},
    {4, true},  {4, true},  {4, true}, {4, true}};
#else

// Patterns of the 8 shapes
// USING T
const piece_t PIECES[N_PIECES] = {0xE0E0000000000000, 0x8080E00000000000,
                                  0x20E0800000000000, 0xA0E0000000000000,
                                  0x80F0000000000000, 0x20E0200000000000,
                                  0x30E0000000000000, 0xC0E0000000000000};

// Rotation and symetry porperties of shapes
const piece_properties_t PIECE_PROPS[N_PIECES] = {
    {2, false}, {4, false}, {2, true}, {4, false},
    {4, true},  {4, false}, {4, true}, {4, true}};

#endif

// Max number of piece positions (using a blank board)
size_t PIECE_POSITION_NUM[N_PIECES] = {48, 80, 82, 96, 151, 144, 154, 196};

void init_solutions(struct solutions *sol, board_t problem,
                    struct solution_restrictions restrictions) {
  // Initialize problem formulation
  sol->problem = problem;
  sol->num_solutions = 0;
  sol->current_level = 0;

  // Allocate memory for subsolutions
  for (int i = 0; i < N_PIECES; i++) {
    sol->sol_patterns_num[i] = 0;
    sol->sol_pattern_index[i] = 0;
    sol->sol_patterns[i] =
        aligned_alloc(32, PIECE_POSITION_NUM[i] * sizeof(piece_t));
    if (sol->sol_patterns[i] == NULL) {
      printf("Failed ot allocate viable_sub_solutions array.\n");
      exit(1);
    }
    memset(sol->sol_patterns[i], 0xFF, PIECE_POSITION_NUM[i] * sizeof(piece_t));

    // Optimized sol_patterns positions (eliminating invalid positions)
    sol->sol_patterns_num[i] =
        make_positions(PIECES[i], PIECE_PROPS[i], sol->problem,
                       sol->sol_patterns[i], restrictions);
  }

  sol->solutions = calloc(512, sizeof(solution_t));
  if (sol->solutions == NULL) {
    printf("Failed ot allocate solutions array.\n");
  }
}

void destroy_solutions(struct solutions *sol) {
  for (int i = 0; i < N_PIECES; i++) {
    free(sol->sol_patterns[i]);
  }
  free(sol->solutions);
}

void push_solution(struct solutions *sol) {

  for (ssize_t i = 0; i < N_PIECES; i++) {
    sol->solutions[sol->num_solutions].pieces[i] =
        sol->sol_patterns[i][sol->sol_pattern_index[i]];
  }
  sol->num_solutions++;
  // printf("Found solution %ld!\n", sol->num_solutions);
}

void solve_rec(struct solutions *sol, board_t problem) {
  ssize_t current_level = sol->current_level;

  __m256i vec_problem = _mm256_set1_epi64x(problem);
  piece_t pp_and_buffer[8] __attribute__((aligned(32)));
  piece_t pp_or_buffer[8] __attribute__((aligned(32)));

  piece_t *p_patterns = sol->sol_patterns[current_level];

  for (ssize_t i = 0; i < sol->sol_patterns_num[current_level]; i += 4) {
    __m256i vec_patterns = _mm256_load_si256((__m256i *)p_patterns);
    p_patterns += 4;

    __m256i vec_pp_and = _mm256_and_si256(vec_problem, vec_patterns);
    _mm256_store_si256((__m256i *)&pp_and_buffer, vec_pp_and);

    __m256i vec_pp_or = _mm256_or_si256(vec_problem, vec_patterns);
    _mm256_store_si256((__m256i *)&pp_or_buffer, vec_pp_or);

    for (ssize_t j = 0; j < 4; j++) {

      if (pp_and_buffer[j] == 0) {
        sol->sol_pattern_index[current_level] = i + j;
        sol->current_level++;
        if (sol->current_level >= N_PIECES) {
          push_solution(sol);
          sol->current_level -= 2;
          return; // We are at the end, we will not fit anywhere else
        } else {
          solve_rec(sol, pp_or_buffer[j]);
        }
      }
    }
  }

  if (current_level > 0)
    sol->current_level--;
}

uint64_t solve(struct solutions *sol) {
  solve_rec(sol, sol->problem);
  return sol->num_solutions;
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
          if (current & problem) {
            invalid += 1;
            // print_raw(current);
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
