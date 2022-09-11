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
  make_from_date(&problem, 2, 2);

  init_all_dates_solution(&sol, &problem, restrictions);

  for (int i = 0; i < problem.n_pieces; i++) {
    print_raw(problem.pieces[i]);
  }

  enumerate_solutions(&sol);

  uint64_t total = 0;
  for (int month = 1; month <= 12; month++) {
    for (int day = 1; day <= 31; day++) {
      size_t pos1 = day_location(day);
      size_t pos2 = month_location(month);

      uint32_t total_solutions =
          sol.date_solutions[pos1][pos2] + sol.date_solutions[pos2][pos1];

      printf("%d/%d - %d solutions\n", month, day, total_solutions);
      total += total_solutions;
    }
  }
  printf("Total: %ld\n", total);
}
