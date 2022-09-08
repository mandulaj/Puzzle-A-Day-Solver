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

#if defined(SIMD_AVX2)
  fprintf(stderr, "Using AVX2\n");
#elif defined(SIMD_AVX512)
  fprintf(stderr, "Using AVX512\n");
#else
  fprintf(stderr, "Not using AVX\n");
#endif

  solver_t sol;
  problem_t problem;
  make_from_date(&problem, 1, 1);

  init_all_dates_solution(&sol, &problem, restrictions);

  enumerate_solutions(&sol);

  uint64_t total = 0;
  for (int month = 0; month < 12; month++) {
    for (int day = 0; day < 31; day++) {
      printf("%d/%d - %d solutions\n", day + 1, month + 1,
             sol.date_solutions[month][day]);
      total += sol.date_solutions[month][day];
    }
  }
  printf("Total: %ld\n", total);
}
