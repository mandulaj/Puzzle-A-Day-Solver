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

int main() {
  struct solution_restrictions restrictions = {true, true};
  struct solution_restrictions restrictions_faceup = {true, false};
  struct solution_restrictions restrictions_facedown = {false, true};

#ifdef ONLY_VALID_DATES

#pragma omp parallel for schedule(dynamic) collapse(2)
  for (int day = 1; day <= 31; day++) {
    for (int month = 1; month <= 12; month++) {
#ifdef INCLUDE_WEEKDAYS
      for (int wd = 0; wd < 7; wd++) {
#endif

        problem_t problem;
        solutions_t sol1;
        solutions_t sol2;
        solutions_t sol3;

#ifdef INCLUDE_WEEKDAYS
        make_from_date_weekday(&problem, day, month, wd);
#else
      make_from_date(&problem, day, month);
#endif

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

#ifdef INCLUDE_WEEKDAYS

        printf("%s,%s,%s,%ld, %ld, %ld\n",
               problem.reverse_lookup[day_location(day)],
               problem.reverse_lookup[month_location(month)],
               problem.reverse_lookup[weekday_location(wd)], numTotal,
               numFaceUp, numFaceDown);
      }
#else
      printf("%s,%s,%ld, %ld, %ld\n", problem.reverse_lookup[day_location(day)],
             problem.reverse_lookup[month_location(month)], numTotal, numFaceUp,
             numFaceDown);
#endif
    }
  }

#else

#ifdef INCLUDE_WEEKDAYS
#pragma omp parallel for collapse(3)
  for (int idx1 = 0; idx1 < 50 - 2; idx1++) {
    for (int idx2 = idx1 + 1; idx2 < 50 - 1; idx2++) {
      for (int idx3 = idx2 + 1; idx3 < 50; idx3++) {

        problem_t problem;
        solutions_t sol1;
        solutions_t sol2;
        solutions_t sol3;

        make_problem_weekday(&problem, index_location(idx1),
                             index_location(idx2), index_location(idx3));

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

        printf("%s,%s,%s,%ld, %ld, %ld\n",
               problem.reverse_lookup[index_location(idx1)],
               problem.reverse_lookup[index_location(idx2)],
               problem.reverse_lookup[index_location(idx3)], numTotal,
               numFaceUp, numFaceDown);
      }
    }
  }
#else

#pragma omp parallel for schedule(dynamic) collapse(2)
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

#endif
}
