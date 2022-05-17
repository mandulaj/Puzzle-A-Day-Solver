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
  struct solution_restrictions restrictions_faceup = {true, false};
  struct solution_restrictions restrictions_facedown = {false, true};

#ifdef ONLY_VALID_DATES

#pragma omp parallel for schedule(dynamic)
  for (int month = 1; month <= 12; month++) {
    for (int day = 1; day <= 31; day++) {

      problem_t problem;
      make_from_date(&problem, day_location(day), month_location(month));
      solutions_t sol1;
      solutions_t sol2;
      solutions_t sol3;

      init_solutions(&sol1, &problem, restrictions);
      init_solutions(&sol2, &problem, restrictions_faceup);
      init_solutions(&sol3, &problem, restrictions_facedown);

      uint64_t numTotal = solve(&sol1);

      uint64_t numFaceUp = solve(&sol2);

      uint64_t numFaceDown = solve(&sol3);

      destroy_solutions(&sol1);
      destroy_solutions(&sol2);
      destroy_solutions(&sol3);

      printf("%s,%s,%ld, %ld, %ld\n", problem.reverse_lookup[day_location(day)],
             problem.reverse_lookup[month_location(month)], numTotal, numFaceUp,
             numFaceDown);
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
