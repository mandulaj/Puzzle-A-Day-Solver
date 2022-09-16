#include "board.h"
#include "config.h"
#include "piece.h"
#include "printing.h"
#include "problem.h"
#include "solver.h"
#include "utils.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {

  if (argc != 2) {
    printf("Please provide a file with pieces to be analyzed");
    exit(EXIT_FAILURE);
  }

  FILE *file;
  status_t ret;

  board_t problem = 0;

  char *file_path = argv[1];

  if (!(file = fopen(file_path, "r"))) {

    printf("File %s doesn't exist", file_path);
    exit(1);
  }

  piece_t pieces[MAX_PIECES] = {0};
  ssize_t n_pieces;

  n_pieces = parse_pieces(file, pieces);
  fclose(file);

  if (n_pieces < 0) {
    printf("Failed parsing the file %s\n", file_path);
    exit(1);
  }

  for (int i = 0; i < n_pieces; i++) {
    printf("0x%016lX, ", pieces[i]);
  }
  printf("\n");

  for (int i = 0; i < n_pieces; i++) {
    piece_properties_t p = get_piece_properties(pieces[i]);
    printf("{%d, %s}, ", p.rotations, p.asymetric ? "true" : "false");
  }
  printf("\n");

  for (int i = 0; i < n_pieces; i++) {
    piece_properties_t p = get_piece_properties(pieces[i]);
    struct solution_restrictions restrictions = {true, true};
    uint64_t positions =
        make_positions(pieces[i], p, problem, NULL, restrictions);
    printf("%ld, ", positions);
  }
  printf("\n");
}
