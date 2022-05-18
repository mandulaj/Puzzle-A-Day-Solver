

#include "piece.h"
#include "problem.h"
#include "solver.h"
#include <stdio.h>

static void print_color_square(int i) {
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