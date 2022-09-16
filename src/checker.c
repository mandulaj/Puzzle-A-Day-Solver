#include "board.h"
#include "config.h"
#include "piece.h"
#include "problem.h"
#include "solver.h"
#include "utils.h"
#include <ctype.h>
#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct one_location_set {
  uint8_t loc1;
  uint8_t loc2;
  uint8_t loc3;
};

int main(int argc, char *argv[]) {
  struct solution_restrictions restrictions = {true, true};

  enum puzzle_mode problem_type = STANDARD_PUZZLE;

  bool only_valid_dates = true;
  bool do_print = true;
  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      if (strcmp("weekdays", argv[i]) == 0) {
        problem_type = WEEKEND_PUZZLE;
      } else if (strcmp("fu8", argv[i]) == 0) {
        problem_type = FACEUP8_PUZZLE;
      } else if (strcmp("all", argv[i]) == 0) {
        only_valid_dates = false;
      } else if (strcmp("noprint", argv[i]) == 0) {
        do_print = false;
      } else if (strcmp("faceup", argv[i]) == 0) {
        restrictions.use_facedown = false;
      } else if (strcmp("facedown", argv[i]) == 0) {
        restrictions.use_faceup = false;
      }
    }
  }

#if defined(SIMD_AVX2)
  fprintf(stderr, "Using AVX2\n");
#elif defined(SIMD_AVX512)
  fprintf(stderr, "Using AVX512\n");
#else
  fprintf(stderr, "Not using AVX\n");
#endif

  solver_t sol;
  problem_t problem;

  switch (problem_type) {
  default:
  case STANDARD_PUZZLE:
    make_empty_problem(&problem, STANDARD_PROBLEM_INDEX);
    break;
  case FACEUP8_PUZZLE:
    make_empty_problem(&problem, FACEUP8_PROBLEM_INDEX);
    break;
  case T_PUZZLE:
    make_empty_problem(&problem, T_PROBLEM_INDEX);
    break;
  case WEEKEND_PUZZLE:
    make_empty_problem(&problem, WEEKDAYS_PROBLEM_INDEX);
    break;
  }

  init_all_dates_solution(&sol, &problem, restrictions);

#if defined(USE_PARALLEL)
  enumerate_solutions_parallel(&sol);

#else
  enumerate_solutions(&sol);
#endif
  uint64_t total = 0;
  switch (problem_type) {
  case WEEKEND_PUZZLE:
    for (int month = 1; month <= 12; month++) {
      for (int day = 1; day <= 31; day++) {
        for (int wd = 0; wd < 7; wd++) {
          size_t pos1 = day_location(day);
          size_t pos2 = month_location(month);
          size_t pos3 = weekday_location(wd);

          // 123
          // 132
          // 213
          // 231
          // 312
          // 321

          uint32_t total_solutions =
              sol.date_solutions[pos1 * 64 * 64 + pos2 * 64 + pos3] +
              sol.date_solutions[pos1 * 64 * 64 + pos3 * 64 + pos2] +
              sol.date_solutions[pos2 * 64 * 64 + pos1 * 64 + pos3] +
              sol.date_solutions[pos2 * 64 * 64 + pos3 * 64 + pos1] +
              sol.date_solutions[pos3 * 64 * 64 + pos1 * 64 + pos2] +
              sol.date_solutions[pos3 * 64 * 64 + pos2 * 64 + pos1];
          if (do_print)
            printf("%2d/%2d-%d = %d\n", month, day, wd, total_solutions);
          total += total_solutions;
        }
      }
    }
    break;

  case STANDARD_PUZZLE:
  case FACEUP8_PUZZLE:
  case T_PUZZLE:
  default:

    for (int month = 1; month <= 12; month++) {
      for (int day = 1; day <= 31; day++) {
        size_t pos1 = day_location(day);
        size_t pos2 = month_location(month);

        uint32_t total_solutions = sol.date_solutions[pos1 * 64 + pos2] +
                                   sol.date_solutions[pos2 * 64 + pos1];

        if (do_print)

          printf("%2d/%2d = %d\n", month, day, total_solutions);
        total += total_solutions;
      }
    }
  }

  if (do_print)
    printf("Total: %ld\n", total);
}
