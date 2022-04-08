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

char *reverse_lookup[] = {
    "",    "",   "",   "",    "",    "",    "",    "",    "",    "",    "",
    "",    "",   "31", "30",  "29",  "",    "28",  "27",  "26",  "25",  "24",
    "23",  "22", "",   "21",  "20",  "19",  "18",  "17",  "16",  "15",  "",
    "14",  "13", "12", "11",  "10",  "9",   "8",   "",    "7",   "6",   "5",
    "4",   "3",  "2",  "1",   "",    "",    "Dec", "Nov", "Oct", "Sep", "Aug",
    "Jul", "",   "",   "Jun", "May", "Apr", "Mar", "Feb", "Jan"};

uint32_t parse_location(char *str) {
  char *months[] = {"jan", "feb", "mar", "apr", "may", "jun",
                    "jul", "aug", "sep", "oct", "nov", "dec"};
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
    printf("%s is an invalid month\n", str);

    return 0;
  }

  return 1;
}

void print_color_square(int i) {
  char *colors[] = {"\x1b[41m", "\x1b[42m", "\x1b[43m", "\x1b[44m",
                    "\x1b[45m", "\x1b[46m", "\x1b[47m", "\x1b[103m"};
  char *reset = "\x1b[0m";

  printf("%s%s%s", colors[i], "  ", reset);
}

void print_solution(solution_t *solution, board_t board) {
  piece_t bit = 0x8000000000000000;
  int piece = 0;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (bit & board) {
        printf("  ");
      } else {
        for (piece = 0; piece < N_PIECES; piece++) {
          if (bit & solution->pieces[piece]) {
            // printf("%d", piece + 1);
            print_color_square(piece);
            break;
          }
        }
        if (piece == N_PIECES) {
          char *repr = reverse_lookup[63 - (i * 8 + j)];
          printf("%2.2s", repr);
        }
      }
      bit >>= 1;
    }
    printf("\n");
  }
  printf("\n");
}

int main(int argc, char *argv[]) {
  struct solution_restrictions restrictions = {true, true};

  if (argc < 3) {
    printf("Usage: ./main [day/month] [day/month] [faceup/facedown]\n");
    exit(1);
  } else if (argc == 4) {
    if (strcmp(argv[3], "faceup") == 0) {
      restrictions.use_facedown = false;
    } else if (strcmp(argv[3], "facedown") == 0) {
      restrictions.use_faceup = false;
    }
  }

  uint32_t location1 = parse_location(argv[1]);
  uint32_t location2 = parse_location(argv[2]);

  if (location1 == 0 || location2 == 0) {
    exit(1);
  }

  board_t problem = make_problem(location1, location2);
  struct solutions sol;

  init_solutions(&sol, problem, restrictions);

  uint64_t num = solve(&sol);

  printf("%s %s - Found %ld solutions:\n", reverse_lookup[location1],
         reverse_lookup[location2], num);

  for (int i = 0; i < num; i++) {
    print_solution(&sol.solutions[i], BLANK_BOARD);
  }

  destroy_solutions(&sol);

  // #pragma omp parallel for schedule(dynamic)
  //   for (int month = 1; month <= 12; month++) {
  //     for (int day = 1; day <= 31; day++) {

  //       board_t problem = make_problem(month_location(month),
  //       day_location(day)); struct solutions sol;

  //       init_solutions(&sol, problem, restrictions);

  //       uint64_t num = solve(&sol);

  //       printf("%d.%d. %ld solutions\n", day, month, num);
  //       destroy_solutions(&sol);
  //     }
  // }
}