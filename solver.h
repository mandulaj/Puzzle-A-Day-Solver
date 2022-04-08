#pragma once

#include "board.h"
#include "piece.h"
#include <stdint.h>
#include <stdlib.h>

struct solutions {
  // Problem
  board_t problem;

  // Current working piece
  ssize_t current_level;

  // Array with all subsolution trees
  ssize_t sub_solution_index[N_PIECES]; // SubSolution Index

  ssize_t sub_solution_num[N_PIECES]; // Total number of subsolutions
  piece_t *sub_solutions[N_PIECES];

  // Found Valid soutions
  ssize_t num_solutions;
  solution_t *solutions;
};

void init_solutions(struct solutions *sol, board_t problem);
void destroy_solutions(struct solutions *sol);
void push_solution(struct solutions *sol);

void solve_rec(struct solutions *sol, board_t problem);

uint64_t solve(struct solutions *sol);

uint64_t make_positions(piece_t piece, piece_properties_t props,
                        board_t problem, piece_t *dest);

