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

enum puzzle_mode {
  NONE_PUZZLE,
  STANDARD_PUZZLE,
  T_PUZZLE,
  WEEKEND_PUZZLE,
  GENERIC8x8_PUZZLE
};

int main(int argc, char *argv[]) {
  problem_t problem;
  status_t ret;
  solutions_t sol;
  struct solution_restrictions restrictions = {true, true};
  enum puzzle_mode mode = NONE_PUZZLE;
  int arg_i = 0;

  if (argc >= 2 && strcmp(argv[1], "8x8") == 0) {
    arg_i = 2;
    mode = GENERIC8x8_PUZZLE;
  } else if (argc < 3) {
    print_usage();
    exit(1);
  }

  uint32_t locations[3] = {0, 0, 0};
  piece_location_t placed_pieces[MAX_PIECES];
  uint32_t n_placed_pieces = 0;
  bool print_solutions = true;

  if (mode == NONE_PUZZLE) {
    mode = STANDARD_PUZZLE;
    for (arg_i = 1; arg_i <= 3 && arg_i < argc; arg_i++) {
      locations[arg_i - 1] = parse_location(argv[arg_i]);
      if (locations[arg_i - 1] == 0 && arg_i <= 2) {
        printf("%s is an invalid weekday or month\n", argv[arg_i]);
      }

      // Only two of the arguments were valid date
      if (arg_i == 3) {
        if (locations[arg_i - 1] != 0) {
          mode = WEEKEND_PUZZLE;
        } else {
          break;
        }
      }
    }
  }

  // Last argument is faceup/facedown or t
  for (; arg_i < argc; arg_i++) {
    if (strcmp(argv[arg_i], "faceup") == 0) {
      restrictions.use_facedown = false;
    } else if (strcmp(argv[arg_i], "facedown") == 0) {
      restrictions.use_faceup = false;
    } else if (strcmp(argv[arg_i], "noprint") == 0) {
      print_solutions = false;
    } else if ((mode == NONE_PUZZLE || mode == STANDARD_PUZZLE) &&
               strcmp(argv[arg_i], "t") == 0) {
      mode = T_PUZZLE;
    } else {
      piece_location_t *ploc = placed_pieces + n_placed_pieces;
      int temp_flip;
      int ret = sscanf(argv[arg_i], "%ld,%ld,%ld,%ld,%d", &ploc->piece_id,
                       &ploc->y, &ploc->x, &ploc->rot, &temp_flip);
      ploc->flip = temp_flip;

      if (ret != 5) {
        printf("Placed pieces must be defined as number quintuplets\n");
        exit(1);
      }
      n_placed_pieces++;
    }
  }

  switch (mode) {
  case STANDARD_PUZZLE:
    printf("Soliving Standard Puzzle\n");
    ret = make_problem_standard(&problem, locations[0], locations[1]);
    break;
  case T_PUZZLE:
    printf("Soliving T Puzzle\n");
    ret = make_problem_t(&problem, locations[0], locations[1]);
    break;
  case WEEKEND_PUZZLE:
    printf("Soliving Weekend Puzzle\n");
    ret = make_problem_weekday(&problem, locations[0], locations[1],
                               locations[2]);
    break;
  case GENERIC8x8_PUZZLE:
    printf("Soliving Generic Puzzle\n");
    ret = make_generic(&problem);
    break;
  case NONE_PUZZLE:
  default:
    printf("Invalid Puzzle\n");
    exit(1);
  }

  ret = check_partial_solution(&problem, placed_pieces, n_placed_pieces, NULL);
  switch (ret) {
  case INVALID_POSITION:
    printf("Can place pieces on top of each other\n");
    exit(1);
    break;
  case DUPLICATE_PIECE:
    printf("You can only use each piece once\n");
    exit(1);
    break;
  case WARNING:
  case ERROR:
    exit(1);
    break;
  default:
    break;
  }

  print_partial_solution(placed_pieces, n_placed_pieces, &problem);

  ret = init_partial_solution(&sol, &problem, restrictions, placed_pieces,
                              n_placed_pieces);
  if (ret) {
    printf("Error: %s\n", get_error_description(ret));
  }

#ifdef USE_PARALLEL
  ret = solve_parallel(&sol);
#else
  ret = solve(&sol);
#endif

  uint64_t num = sol.num_solutions;

  // Print Date
  switch (mode) {
  case WEEKEND_PUZZLE:
    printf("%s %s %s - ", problem.reverse_lookup[locations[0]],
           problem.reverse_lookup[locations[1]],
           problem.reverse_lookup[locations[2]]);
    break;
  case STANDARD_PUZZLE:
  case T_PUZZLE:
    printf("%s %s - ", problem.reverse_lookup[locations[0]],
           problem.reverse_lookup[locations[1]]);
    break;
  default:
    break;
  }

  printf("Found %ld solutions:\n", num);

  if (print_solutions) {
    for (int i = 0; i < num; i++) {
      print_solution(&sol.solutions[i], &problem);
    }

    // Also print at the bottom if we have too many solutions
    if (num > 3) {
      // Print Date
      switch (mode) {
      case WEEKEND_PUZZLE:
        printf("%s %s %s - ", problem.reverse_lookup[locations[0]],
               problem.reverse_lookup[locations[1]],
               problem.reverse_lookup[locations[2]]);
        break;
      case STANDARD_PUZZLE:
      case T_PUZZLE:
        printf("%s %s - ", problem.reverse_lookup[locations[0]],
               problem.reverse_lookup[locations[1]]);
        break;
      default:
        break;
      }

      printf("Found %ld solutions:\n", num);
    }
  }
  destroy_solutions(&sol);
}
