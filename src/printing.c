

#include "piece.h"
#include "problem.h"
#include "solver.h"
#include <stdio.h>
#include <string.h>

static int output_color(int i, const char *text, char *buffer, size_t n) {
  char *colors[] = {"\x1b[41m",  "\x1b[42m",  "\x1b[43m", "\x1b[44m",
                    "\x1b[45m",  "\x1b[46m",  "\x1b[47m", "\x1b[103m",
                    "\x1b[102m", "\x1b[104m", "\x1b[7m",  "\x1b[0m"};
  char *reset = "\x1b[0m";

  return snprintf(buffer, n, "%s%s%s", colors[i], text, reset);
}

static void print_color(int i, const char *text) {
  char buffer[32];

  output_color(i, text, buffer, 32);
  printf("%s", buffer);
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

int get_piece_line(piece_t p, int color, int line, char *buffer) {
  p = piece_origin(p); // Move pice to corner
  char *p_start = buffer;

  piece_t bit = 0x8000000000000000 >> 8 * line;
  for (int i = 0; i < 8; i++) {
    if (bit & p) {
      buffer += output_color(color, "  ", buffer, 32);
    } else {
      buffer += output_color(11, "  ", buffer, 32);
    }
    bit >>= 1;
  }

  return buffer - p_start - 1;
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

void print_raw_color(piece_t pattern, int color) {
  piece_t bit = 0x8000000000000000;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (bit & pattern) {
        print_color_square(color);
      } else {
        printf("[]");
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

void print_problem(problem_t *prob) {
  printf("Problem with %ld pieces\n", prob->n_pieces);
  print_piece_board(0x00, prob, 0);
  for (int i = 0; i < prob->n_pieces; i++) {
    printf("Piece %d with %ld positions (%ld rotations & %s) :\n", i,
           prob->piece_position_num[i], prob->piece_props[i].rotations,
           prob->piece_props[i].asymetric ? "asymetric" : "symetric");
    print_piece(prob->pieces[i], i);
  }
}

void print_usageprint_partial_help() {
  printf("Usage: ./pad [day/month/weekday] [day/month/weekday] "
         "{day/month/weekday} [faceup/facedown] [t]\n");
}

void print_usage() {
  printf("Usage: ./pad [day/month/weekday] [day/month/weekday] "
         "{day/month/weekday} {faceup/facedown} {t} {id,y,x,rot,flip}\n");

  printf("\n");
  printf("  Eg. for 25. May, Sunday - ./pad may sun 25\n");
  printf("  faceup - restricts to only faceup solutions\n");
  printf("  facedown - restricts to only facedown solutions\n");
  printf("  t - uses the T version of the puzzle\n");
  printf("  {id,y,x,rot,flip} - allows finding solutions for partially filled "
         "puzzles\n");
  printf("                      specify piece id, x and y position, rotation "
         "and flip\n");
  printf("                      eg. 3,0,2,3,1 - places piece 3 rotated by 3 "
         "and flipped at (0,2)\n\n");
  problem_t problem1;
  problem_t problem2;
  problem_t problem3;
  problem_t problem4;
  make_problem_standard(&problem1, 14, 15);
  make_problem_t(&problem2, 14, 15);
  make_problem_weekday(&problem3, 14, 15, 16);
  make_generic(&problem4);
  // Print table
  printf("  Standard        T-Pieces      Weekend Pieces     8x8 puzzle\n");

  for (int i = 0; i < MAX_PIECES; i++) {
    printf("-------------------------------------------------------------\n");
    for (int line = 0; line < 3; line++) {
      char buffer[1024];
      char *pbuf = buffer;
      if (line == 0) {
        pbuf += snprintf(pbuf, 1024, "%2d ", i);
      } else {
        strcpy(pbuf, "   ");
        pbuf += 3;
      }

      if (i < problem1.n_pieces) {
        pbuf += get_piece_line(problem1.pieces[i], i, line, pbuf);
      } else {
        strcpy(pbuf, "                ");
        pbuf += 16;
      }

      if (i < problem2.n_pieces) {
        pbuf += get_piece_line(problem2.pieces[i], i, line, pbuf);
      } else {
        strcpy(pbuf, "                ");
        pbuf += 16;
      }

      if (i < problem3.n_pieces) {
        pbuf += get_piece_line(problem3.pieces[i], i, line, pbuf);
      } else {
        strcpy(pbuf, "                ");
        pbuf += 16;
      }

      if (i < problem4.n_pieces) {
        pbuf += get_piece_line(problem4.pieces[i], i, line, pbuf);
      } else {
        strcpy(pbuf, "                ");
        pbuf += 16;
      }
      printf("%s\n", buffer);
    }
  }
}
