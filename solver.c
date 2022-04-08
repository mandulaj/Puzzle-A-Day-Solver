#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "piece.h"
#include "problem.h"
#include "solver.h"
#include <omp.h>
#include <stdbool.h>

#define N_PIECES 8

const piece_t PIECES[N_PIECES] = {0xE0E0000000000000, 0x8080E00000000000,
                                  0x20E0800000000000, 0xA0E0000000000000,
                                  0x80F0000000000000, 0x20F0000000000000,
                                  0x30E0000000000000, 0xC0E0000000000000};

const piece_properties_t PIECE_PROPS[N_PIECES] = {
    {2, false}, {4, false}, {2, true}, {4, false},
    {4, true},  {4, true},  {4, true}, {4, true}};

// Max number of piece positions
size_t PIECE_POSITION_NUM[N_PIECES] = {48, 80, 82, 96, 151, 154, 154, 196};

void init_solutions(struct solutions *sol, board_t problem) {
  // Initialize problem formulation
  sol->problem = problem;
  sol->num_solutions = 0;
  sol->current_level = 0;

  // Allocate memory for subsolutions
  for (int i = 0; i < N_PIECES; i++) {
    sol->sub_solution_num[i] = 0;
    sol->sub_solution_index[i] = 0;
    sol->sub_solutions[i] = calloc(PIECE_POSITION_NUM[i], sizeof(piece_t));
    if (sol->sub_solutions[i] == NULL) {
      printf("Failed ot allocate viable_sub_solutions array.\n");
      exit(1);
    }

    // Optimized sub_solution positions (eliminating invalid positions)
    sol->sub_solution_num[i] = make_positions(
        PIECES[i], PIECE_PROPS[i], sol->problem, sol->sub_solutions[i]);
  }

  sol->solutions = calloc(512, sizeof(solution_t));
  if (sol->solutions == NULL) {
    printf("Failed ot allocate solutions array.\n");
  }
}

void destroy_solutions(struct solutions *sol) {
  for (int i = 0; i < N_PIECES; i++) {
    free(sol->sub_solutions[i]);
  }
  free(sol->solutions);
}

void push_solution(struct solutions *sol) {

  for (ssize_t i = 0; i < N_PIECES; i++) {
    sol->solutions[sol->num_solutions].pieces[i] =
        sol->sub_solutions[i][sol->sub_solution_index[i]];
  }
  sol->num_solutions++;
  // printf("Found solution %ld!\n", sol->num_solutions);
}

void solve_rec(struct solutions *sol, board_t problem) {
  ssize_t current_level = sol->current_level;

  for (ssize_t i = 0; i < sol->sub_solution_num[current_level]; i++) {
    // if (current_level == 0) {
    //   printf("Solving %ld/%ld\n", i, sol->sub_solution_num[current_level]);
    // }
    // If the tested position fits:
    if ((problem & sol->sub_solutions[current_level][i]) == 0) {
      sol->sub_solution_index[current_level] = i;
      sol->current_level++;
      if (sol->current_level >= N_PIECES) {
        push_solution(sol);
        sol->current_level -= 2;
        return; // We are at the end, we will not fit anywhere else
      } else {
        solve_rec(sol, problem | sol->sub_solutions[current_level][i]);
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
                        board_t problem, piece_t *dest) {
  uint64_t positions = 0;
  uint64_t invalid = 0;
  piece_t current = piece;
  piece_t old = (board_t)0x00;
  bool done_asymetric = false;

  // Do both symetries
  do {
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

int main() {

  board_t problem = make_problem(day_location(3), day_location(14));
  struct solutions sol;

  init_solutions(&sol, problem);

  uint64_t num = solve(&sol);

  printf("%d.%d. %ld solutions\n", 3, 14, num);
  destroy_solutions(&sol);

  problem = make_problem(day_location(6), day_location(28));

  init_solutions(&sol, problem);

  num = solve(&sol);

  printf("%d.%d. %ld solutions\n", 6, 28, num);
  destroy_solutions(&sol);

  problem = make_problem(day_location(6), day_location(9));

  init_solutions(&sol, problem);

  num = solve(&sol);

  printf("%d.%d. %ld solutions\n", 6, 9, num);
  destroy_solutions(&sol);

  problem = make_problem(day_location(9), day_location(11));

  init_solutions(&sol, problem);

  num = solve(&sol);

  printf("%d.%d. %ld solutions\n", 9, 11, num);
  destroy_solutions(&sol);

  exit(0);

#pragma omp parallel for schedule(dynamic)
  for (int month = 1; month <= 12; month++) {
    for (int day = 1; day <= 31; day++) {

      board_t problem = make_problem(month_location(month), day_location(day));
      struct solutions sol;

      init_solutions(&sol, problem);

      uint64_t num = solve(&sol);

      printf("%d.%d. %ld solutions\n", day, month, num);
      destroy_solutions(&sol);
    }
  }
}
