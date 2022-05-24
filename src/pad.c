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

int main(int argc, char *argv[]) {
  problem_t problem;
  status_t ret;
  solutions_t sol;
  struct solution_restrictions restrictions = {true, true};

  if (argc < 3) {
    print_usage();
    exit(1);
  }

  bool week_day_mode = false;
  bool use_t_mode = false;

  int i = 0;
  uint32_t locations[3] = {0, 0, 0};
  piece_location_t placed_pieces[MAX_PIECES];
  uint32_t n_placed_pieces = 0;

  for (i = 1; i <= 3 && i < argc; i++) {
    locations[i - 1] = parse_location(argv[i]);
    if (locations[i - 1] == 0 && i <= 2) {
      printf("%s is an invalid weekday or month\n", argv[i]);
    }

    // Only two of the arguments were valid date
    if (i == 3) {
      if (locations[i - 1] != 0) {
        week_day_mode = true;
      } else {
        break;
      }
    }
  }

  // Last argument is faceup/facedown or t
  for (; i < argc; i++) {
    if (strcmp(argv[i], "faceup") == 0) {
      restrictions.use_facedown = false;
    } else if (strcmp(argv[i], "facedown") == 0) {
      restrictions.use_faceup = false;
    } else if (strcmp(argv[i], "t") == 0) {
      use_t_mode = true;

    } else {
      piece_location_t *ploc = placed_pieces + n_placed_pieces;
      int temp_flip;
      int ret = sscanf(argv[i], "%ld,%ld,%ld,%ld,%d", &ploc->piece_id, &ploc->y,
                       &ploc->x, &ploc->rot, &temp_flip);
      ploc->flip = temp_flip;

      if (ret != 5) {
        printf("Placed pieces must be defined as number quintuplets\n");
        exit(1);
      }
      n_placed_pieces++;
    }
  }

  if (week_day_mode) {
    ret = make_problem_weekday(&problem, locations[0], locations[1],
                               locations[2]);
  } else {
    if (use_t_mode) {
      ret = make_problem_t(&problem, locations[0], locations[1]);
    } else {
      ret = make_problem_standard(&problem, locations[0], locations[1]);
    }
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

  ret = solve(&sol);

  uint64_t num = sol.num_solutions;

  // Print Date
  if (week_day_mode) {
    printf("%s %s %s", problem.reverse_lookup[locations[0]],
           problem.reverse_lookup[locations[1]],
           problem.reverse_lookup[locations[2]]);

  } else {
    printf("%s %s", problem.reverse_lookup[locations[0]],
           problem.reverse_lookup[locations[1]]);
  }

  printf(" - Found %ld solutions:\n", num);

  for (int i = 0; i < num; i++) {
    print_solution(&sol.solutions[i], &problem);
  }

  // Also print at the bottom if we have too many solutions
  if (num > 3) {
    // Print Date
    if (week_day_mode) {
      printf("%s %s %s", problem.reverse_lookup[locations[0]],
             problem.reverse_lookup[locations[1]],
             problem.reverse_lookup[locations[2]]);

    } else {
      printf("%s %s", problem.reverse_lookup[locations[0]],
             problem.reverse_lookup[locations[1]]);
    }

    printf(" - Found %ld solutions:\n", num);
  }
  destroy_solutions(&sol);
}
