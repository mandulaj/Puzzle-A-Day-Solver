#include "board.h"
#include "piece.h"
#include "problem.h"
#include "solver.h"
#include <ctype.h>
#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define ONLY_VALID_DATES

int main() {
  struct solution_restrictions restrictions = {true, true};

#ifdef ONLY_VALID_DATES

#pragma omp parallel for schedule(dynamic)
  for (int month = 1; month <= 12; month++) {
    for (int day = 1; day <= 31; day++) {

      board_t problem = make_problem(day_location(day), month_location(month));
      struct solutions sol;

      init_solutions(&sol, problem, restrictions);

      uint64_t num = solve(&sol);

      printf("%s,%s,%ld\n", reverse_lookup[day_location(day)],
             reverse_lookup[month_location(month)], num);
      destroy_solutions(&sol);
    }
  }

#else

#pragma omp parallel for schedule(dynamic)
  for (int i1 = 0; i1 < 42; i1++) {
    for (int i2 = i1 + 1; i2 < 43; i2++) {

      board_t problem = make_problem(index_location(i1), index_location(i2));
      struct solutions sol;

      init_solutions(&sol, problem, restrictions);

      uint64_t num = solve(&sol);

      printf("%s,%s,%ld\n", reverse_lookup[index_location(i1)],
             reverse_lookup[index_location(i2)], num);
      destroy_solutions(&sol);
    }
  }

#endif
}