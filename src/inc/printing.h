#pragma once

#include "piece.h"
#include "printing.h"
#include "problem.h"
#include "solver.h"

void print_solution(const solution_t *solution, const problem_t *problem);
void print_piece(piece_t p, int color);
void print_piece_board(piece_t p, const problem_t *problem, int color);
void print_partial_solution(const piece_location_t *pieces, size_t n_p,
                            const problem_t *problem);

void print_usage();

void print_partial_help();
