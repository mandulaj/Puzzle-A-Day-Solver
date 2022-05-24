

#include "piece.h"
#include "problem.h"
#include "solver.h"
#include <stdio.h>

static void print_color(int i, char *text) {
  char *colors[] = {"\x1b[41m",  "\x1b[42m",  "\x1b[43m", "\x1b[44m",
                    "\x1b[45m",  "\x1b[46m",  "\x1b[47m", "\x1b[103m",
                    "\x1b[102m", "\x1b[104m", "\x1b[7m"};
  char *reset = "\x1b[0m";

  printf("%s%s%s", colors[i], text, reset);
}

static void print_color_square(int i) { print_color(i, "  "); }

void print_solution(const solution_t *solution, const problem_t *problem) {
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

void print_piece(piece_t p, int color) {
  p = piece_origin(p); // Move pice to corner

  piece_t bit = 0x8000000000000000;
  piece_t line = 0xFF00000000000000;
  for (int i = 0; i < 8; i++) {
    piece_t line_clear = p & line;
    line >>= 8;
    if (line_clear == 0) {
      bit >>= 8;
      continue;
    }

    for (int j = 0; j < 8; j++) {
      if (bit & p) {
        print_color_square(color);
      } else {
        printf("  ");
      }
      bit >>= 1;
    }
    printf("\n");
  }
  printf("\n");
}

void print_piece_board(piece_t p, const problem_t *problem, int color) {
  piece_t bit = 0x8000000000000000;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (bit & problem->blank) {
        printf("  ");
      } else if (bit & p) {

        print_color_square(color);
      } else {
        const char *repr = problem->reverse_lookup[63 - (i * 8 + j)];
        char buffer[4] = {0, 0, 0, 0};
        snprintf(buffer, 3, "%2.2s", repr);

        if (bit & problem->problem) {
          print_color(10, buffer);
        } else {
          printf("%s", buffer);
        }
      }
      bit >>= 1;
    }
    printf("\n");
  }
  printf("\n");
}

void print_partial_solution(const piece_location_t *pieces, size_t n_p,
                            const problem_t *problem) {
  piece_t bit = 0x8000000000000000;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (bit & problem->blank) {
        printf("  ");
        goto NEXT_POSITION_ITER;
      }

      for (int k = 0; k < n_p; k++) {
        piece_t p = get_piece(problem->pieces, problem->n_pieces, pieces[k]);
        if (bit & p) {
          print_color_square(pieces[k].piece_id);
          goto NEXT_POSITION_ITER;
        }
      }

      const char *repr = problem->reverse_lookup[63 - (i * 8 + j)];
      char buffer[4] = {0, 0, 0, 0};
      snprintf(buffer, 3, "%2.2s", repr);

      if (bit & problem->problem) {
        print_color(10, buffer);
      } else {
        printf("%s", buffer);
      }

    NEXT_POSITION_ITER:
      bit >>= 1;
    }
    printf("\n");
  }
  printf("\n");
}

void print_usageprint_partial_help() {
  printf("Usage: ./main [day/month/weekday] [day/month/weekday] "
         "{day/month/weekday} [faceup/facedown] [t]\n");
}

void print_usage() {
  printf("Usage: ./main [day/month/weekday] [day/month/weekday] "
         "{day/month/weekday} [faceup/facedown] [t] [id,y,x,rot,flip]\n");
  problem_t problem;

  make_problem_standard(&problem, 14, 15);
  printf("Standard Pieces:\n");
  for (int i = 0; i < problem.n_pieces; i++) {
    printf("%d\n", i);
    print_piece(problem.pieces[i], i);
  }

  make_problem_t(&problem, 14, 15);
  printf("T-Pieces:\n");
  for (int i = 0; i < problem.n_pieces; i++) {
    printf("%d\n", i);
    print_piece(problem.pieces[i], i);
  }

  make_problem_weekday(&problem, 14, 15, 16);
  printf("Weekend Pieces:\n");
  for (int i = 0; i < problem.n_pieces; i++) {
    printf("%d\n", i);
    print_piece(problem.pieces[i], i);
  }
}