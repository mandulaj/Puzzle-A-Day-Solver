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

  if (argc < 3 || argc > 6) {
    print_usage();
    exit(1);
  }

  bool had_restriction = false;
  bool week_day_mode = false;
  bool use_t_mode = false;
  uint32_t n_arguments = 0;

  // Last argument is faceup/facedown or t
  for (int i = 1; i <= 2; i++) {
    if (strcmp(argv[argc - i], "faceup") == 0) {
      restrictions.use_facedown = false;
      had_restriction = true;
    } else if (strcmp(argv[argc - i], "facedown") == 0) {
      restrictions.use_faceup = false;
      had_restriction = true;
    } else if (strcmp(argv[argc - i], "t") == 0) {
      use_t_mode = true;
    }
  }

  // Work out how many day/month/weekday specifications we received
  n_arguments = argc - 1;
  if (had_restriction) {
    n_arguments--;
  }
  if (use_t_mode) {
    n_arguments--;
  }

  if (n_arguments != 2 && n_arguments != 3) {
    print_usage();
    exit(1);
  }

  uint32_t locations[3];

  for (int i = 0; i < n_arguments; i++) {
    locations[i] = parse_location(argv[i + 1]);
    if (locations[i] == 0) {
      printf("%s is an invalid weekday or month\n", argv[i + 1]);

      exit(1);
    }
  }

  if (n_arguments == 3) {
    week_day_mode = true;
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

  ret = init_solutions(&sol, &problem, restrictions);
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
