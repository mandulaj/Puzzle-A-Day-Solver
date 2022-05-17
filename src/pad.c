#include "board.h"
#include "piece.h"
#include "problem.h"
#include "solver.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int isNumber(char s[]) {
  for (int i = 0; s[i] != '\0'; i++) {
    if (isdigit(s[i]) == 0)
      return 0;
  }
  return 1;
}

uint32_t parse_location(char *str) {
  char *months[] = {"jan", "feb", "mar", "apr", "may", "jun",
                    "jul", "aug", "sep", "oct", "nov", "dec"};

  char *weekdays[] = {"mon", "tue", "wed", "thr", "fri", "sat", "sun"};

  if (isNumber(str)) {
    int num = 0;
    sscanf(str, "%d", &num);
    if (num >= 1 && num <= 31) {
      return day_location(num);
    } else {
      printf("Day must be between 1 and 31\n");
      return 0;
    }
  } else {
    for (int i = 0; i < 12; i++) {
      if (strcmp(months[i], str) == 0) {
        return month_location(i + 1);
      }
    }

    for (int i = 0; i < 7; i++) {
      if (strcmp(weekdays[i], str) == 0) {
        return weekday_location(i);
      }
    }
    printf("%s is an invalid weekday or month\n", str);

    return 0;
  }

  return 1;
}

void print_usage() {
  printf("Usage: ./main [day/month/weekday] [day/month/weekday] "
         "{day/month/weekday} [faceup/facedown] [t]\n");
}

int main(int argc, char *argv[]) {
  problem_t problem;
  solutions_t sol;
  struct solution_restrictions restrictions = {true, true};
  bool res;

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

  uint32_t location1 = parse_location(argv[1]);
  uint32_t location2 = parse_location(argv[2]);
  uint32_t location3 = location2;

  if (n_arguments == 3) {
    week_day_mode = true;
    location3 = parse_location(argv[3]);
  }

  if (location1 == 0 || location2 == 0 || location3 == 0) {
    exit(1);
  }

  if (week_day_mode) {
    res = make_problem_weekday(&problem, location1, location2, location3);
  } else {
    if (use_t_mode) {
      res = make_problem_t(&problem, location1, location2);
    } else {
      res = make_problem_standard(&problem, location1, location2);
    }
  }

  init_solutions(&sol, &problem, restrictions);

  uint64_t num = solve(&sol);

  // Print Date
  if (week_day_mode) {
    printf("%s %s %s", problem.reverse_lookup[location1],
           problem.reverse_lookup[location2],
           problem.reverse_lookup[location3]);

  } else {
    printf("%s %s", problem.reverse_lookup[location1],
           problem.reverse_lookup[location2]);
  }

  printf(" - Found %ld solutions:\n", num);

  for (int i = 0; i < num; i++) {
    print_solution(&sol.solutions[i], &problem);
  }

  // Also print at the bottom if we have too many solutions
  if (num > 3) {
    // Print Date
    if (week_day_mode) {
      printf("%s %s %s", problem.reverse_lookup[location1],
             problem.reverse_lookup[location2],
             problem.reverse_lookup[location3]);

    } else {
      printf("%s %s", problem.reverse_lookup[location1],
             problem.reverse_lookup[location2]);
    }

    printf(" - Found %ld solutions:\n", num);
  }
  destroy_solutions(&sol);
}
