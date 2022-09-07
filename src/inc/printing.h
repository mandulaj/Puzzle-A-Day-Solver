#pragma once

#include "piece.h"
#include "printing.h"
#include "problem.h"
#include "solver.h"


typedef uint32_t rgb_color_t;

void print_solution(const solution_t *solution, const problem_t *problem);
void print_piece(piece_t p, int color);
void print_piece_board(piece_t p, const problem_t *problem, int color);
void print_partial_solution(const piece_location_t *pieces, size_t n_p,
                            const problem_t *problem);
void print_problem(const problem_t *prob);

void print_usage();

void print_partial_help();
void print_raw_color(piece_t pattern, int color);

void print_color(const char *text, int c);

void print_rgb(const char *text, int r, int g, int b);

int get_piece_line(piece_t p, int color, int line, char *buffer);