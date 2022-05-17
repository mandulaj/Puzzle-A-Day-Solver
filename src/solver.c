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

#define MAX_NUM_SOLUTIONS 2048

void init_solutions(solutions_t *sol, problem_t *problem,
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

  // for (int i = 0; i < sol->n_pieces; i++) {
  //   printf("Positions: %ld\n\n", sol->sol_patterns_num[i]);
  //   print_raw(problem->pieces[i]);
  // }

  sol->solutions = calloc(MAX_NUM_SOLUTIONS, sizeof(solution_t));
  if (sol->solutions == NULL) {
    printf("Failed ot allocate solutions array.\n");
  }
}

void destroy_solutions(solutions_t *sol) {
  for (int i = 0; i < sol->n_pieces; i++) {
    free(sol->sol_patterns[i]);
  }
  free(sol->solutions);
}

void push_solution(solutions_t *sol) {

  if (sol->num_solutions > MAX_NUM_SOLUTIONS) {
    printf("Found too may solutions\n!");
    exit(1);
  }
  for (size_t i = 0; i < sol->n_pieces; i++) {
    sol->solutions[sol->num_solutions].pieces[i] =
        sol->sol_patterns[i][sol->sol_pattern_index[i]];
  }
  sol->num_solutions++;
  // printf("Found solution %ld!\n", sol->num_solutions);
}

static void solve_rec(solutions_t *sol, board_t problem) {
  size_t current_level = sol->current_level;

  __m256i vec_problem = _mm256_set1_epi64x(problem);
  piece_t pp_and_buffer[8] __attribute__((aligned(32)));
  piece_t pp_or_buffer[8] __attribute__((aligned(32)));

  piece_t *p_patterns = sol->sol_patterns[current_level];

  for (size_t i = 0; i < sol->sol_patterns_num[current_level]; i += 4) {
    __m256i vec_patterns = _mm256_load_si256((__m256i *)p_patterns);
    p_patterns += 4;

    __m256i vec_pp_and = _mm256_and_si256(vec_problem, vec_patterns);
    _mm256_store_si256((__m256i *)&pp_and_buffer, vec_pp_and);

    __m256i vec_pp_or = _mm256_or_si256(vec_problem, vec_patterns);
    _mm256_store_si256((__m256i *)&pp_or_buffer, vec_pp_or);

    for (size_t j = 0; j < 4; j++) {

      if (pp_and_buffer[j] == 0) {
        sol->sol_pattern_index[current_level] = i + j;
        sol->current_level++;
        if (sol->current_level >= sol->n_pieces) {
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

uint64_t solve(solutions_t *sol) {
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

void print_color_square(int i) {
  char *colors[] = {"\x1b[41m",  "\x1b[42m", "\x1b[43m", "\x1b[44m",
                    "\x1b[45m",  "\x1b[46m", "\x1b[47m", "\x1b[103m",
                    "\x1b[102m", "\x1b[104m"};
  char *reset = "\x1b[0m";

  printf("%s%s%s", colors[i], "  ", reset);
}

void print_solution(solution_t *solution, problem_t *problem) {
  piece_t bit = 0x8000000000000000;
  int piece = 0;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (bit & problem->blank) {
        printf("  ");
      } else {
        for (piece = 0; piece < problem->n_pieces; piece++) {
          if (bit & solution->pieces[piece]) {
            // printf("%d", piece + 1);
            print_color_square(piece);
            break;
          }
        }
        if (piece == problem->n_pieces) {
          const char *repr = problem->reverse_lookup[63 - (i * 8 + j)];
          printf("%2.2s", repr);
        }
      }
      bit >>= 1;
    }
    printf("\n");
  }
  printf("\n");
}