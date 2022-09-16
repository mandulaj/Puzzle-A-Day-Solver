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
  struct solution_restrictions restrictions_faceup = {true, false};
  struct solution_restrictions restrictions_facedown = {false, true};

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
      }
    }
  }

  // if (do_print) {
#if defined(SIMD_AVX2)
  fprintf(stderr, "Using AVX2\n");
#elif defined(SIMD_AVX512)
  fprintf(stderr, "Using AVX512\n");
#else
  fprintf(stderr, "Not using AVX\n");
#endif
  // }

  // Prepare problem indexes
  struct one_location_set *dates_to_test;
  size_t num_days = 0;
  switch (problem_type) {
  case WEEKEND_PUZZLE:
    if (only_valid_dates) {
      num_days = 31 * 12 * 7;
    } else {
      num_days = (50 * 49 * 48) / 6;
    }
    break;

  case STANDARD_PUZZLE:
  case FACEUP8_PUZZLE:
  default:
    if (only_valid_dates) {
      num_days = 31 * 12;
    } else {
      num_days = (43 * (43 - 1)) / 2;
    }
    break;
  }
  dates_to_test = calloc(num_days, sizeof(struct one_location_set));
  if (dates_to_test == NULL)
    exit(1);

  size_t i = 0;
  switch (problem_type) {
  case WEEKEND_PUZZLE:
    if (only_valid_dates) {
      for (int day = 1; day <= 31; day++) {
        for (int month = 1; month <= 12; month++) {
          for (int wd = 0; wd < 7; wd++) {
            dates_to_test[i].loc1 = day_location(day);
            dates_to_test[i].loc2 = month_location(month);
            dates_to_test[i].loc3 = weekday_location(wd);
            i++;
          }
        }
      }
    } else {
      for (int idx1 = 0; idx1 < 50 - 2; idx1++) {
        for (int idx2 = idx1 + 1; idx2 < 50 - 1; idx2++) {
          for (int idx3 = idx2 + 1; idx3 < 50; idx3++) {

            dates_to_test[i].loc1 = index_location(idx1);
            dates_to_test[i].loc2 = index_location(idx2);
            dates_to_test[i].loc3 = index_location(idx3);
            i++;
          }
        }
      }
    }
    break;

  case STANDARD_PUZZLE:
  case FACEUP8_PUZZLE:
  case T_PUZZLE:
  default:
    if (only_valid_dates) {
      for (int day = 1; day <= 31; day++) {
        for (int month = 1; month <= 12; month++) {
          dates_to_test[i].loc1 = day_location(day);
          dates_to_test[i].loc2 = month_location(month);
          dates_to_test[i].loc3 = 0;
          i++;
        }
      }
    } else {
      for (int idx1 = 0; idx1 < 42; idx1++) {
        for (int idx2 = idx1 + 1; idx2 < 43; idx2++) {
          dates_to_test[i].loc1 = index_location(idx1);
          dates_to_test[i].loc2 = index_location(idx2);
          dates_to_test[i].loc3 = 0;
          i++;
        }
      }
    }
    break;
  }

  // Run the solver on all days
  const struct one_location_set *const_dates_to_test = dates_to_test;

#pragma omp parallel for schedule(dynamic)
  for (size_t day = 0; day < num_days; day++) {
    problem_t problem;
    solver_t sol1;
    solver_t sol2;
    solver_t sol3;

    switch (problem_type) {
    default:
    case STANDARD_PUZZLE:
      make_problem_standard(&problem, const_dates_to_test[day].loc1,
                            const_dates_to_test[day].loc2);
      break;
    case FACEUP8_PUZZLE:
      make_problem_faceup8(&problem, const_dates_to_test[day].loc1,
                           const_dates_to_test[day].loc2);
      break;
    case T_PUZZLE:
      make_problem_t(&problem, const_dates_to_test[day].loc1,
                     const_dates_to_test[day].loc2);
      break;
    case WEEKEND_PUZZLE:
      make_problem_weekday(&problem, const_dates_to_test[day].loc1,
                           const_dates_to_test[day].loc2,
                           const_dates_to_test[day].loc3);
      break;
    }

    init_solutions(&sol1, &problem, restrictions);
    init_solutions(&sol2, &problem, restrictions_faceup);
    init_solutions(&sol3, &problem, restrictions_facedown);

    solve(&sol1);
    solve(&sol2);
    solve(&sol3);
    uint64_t numTotal = sol1.num_solutions;

    uint64_t numFaceUp = sol2.num_solutions;

    uint64_t numFaceDown = sol3.num_solutions;

    destroy_solutions(&sol1);
    destroy_solutions(&sol2);
    destroy_solutions(&sol3);

    if (!do_print)
      continue;

    switch (problem_type) {
    case WEEKEND_PUZZLE:
      printf("%s,%s,%s,%ld, %ld, %ld\n",
             problem.reverse_lookup[const_dates_to_test[day].loc1],
             problem.reverse_lookup[const_dates_to_test[day].loc2],
             problem.reverse_lookup[const_dates_to_test[day].loc3], numTotal,
             numFaceUp, numFaceDown);
      break;
    case T_PUZZLE:
    case FACEUP8_PUZZLE:
    case STANDARD_PUZZLE:
    default:
      printf("%s,%s,%ld, %ld, %ld\n",
             problem.reverse_lookup[const_dates_to_test[day].loc1],
             problem.reverse_lookup[const_dates_to_test[day].loc2], numTotal,
             numFaceUp, numFaceDown);
      break;
    }
  }
  free(dates_to_test);

  return 0;
}
