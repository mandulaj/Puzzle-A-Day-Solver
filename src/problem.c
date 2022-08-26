#include "problem.h"
#include "solver.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

const char *reverse_lookup_weekdays[] = {
    "",    "Sun", "Sat", "Fri", "",    "",    "",    "",    "",    "Thr", "Wed",
    "Tue", "Mon", "31",  "30",  "29",  "",    "28",  "27",  "26",  "25",  "24",
    "23",  "22",  "",    "21",  "20",  "19",  "18",  "17",  "16",  "15",  "",
    "14",  "13",  "12",  "11",  "10",  "9",   "8",   "",    "7",   "6",   "5",
    "4",   "3",   "2",   "1",   "",    "",    "Dec", "Nov", "Oct", "Sep", "Aug",
    "Jul", "",    "",    "Jun", "May", "Apr", "Mar", "Feb", "Jan"};

const char *reverse_lookup_standard[] = {
    "",    "",   "",   "",    "",    "",    "",    "",    "",    "",    "",
    "",    "",   "31", "30",  "29",  "",    "28",  "27",  "26",  "25",  "24",
    "23",  "22", "",   "21",  "20",  "19",  "18",  "17",  "16",  "15",  "",
    "14",  "13", "12", "11",  "10",  "9",   "8",   "",    "7",   "6",   "5",
    "4",   "3",  "2",  "1",   "",    "",    "Dec", "Nov", "Oct", "Sep", "Aug",
    "Jul", "",   "",   "Jun", "May", "Apr", "Mar", "Feb", "Jan"};

const char *reverse_lookup_generic[] = {
    "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]",
    "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]",
    "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]",
    "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]",
    "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]",
    "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]"};

#define EMPTY_BLANK                                                            \
  0b0000000000000000000000000000000000000000000000000000000000000000
#define STANDARD_BLANK                                                         \
  0b0000001100000011000000010000000100000001000000010001111111111111
#define WEEKDAYS_BLANK                                                         \
  0b0000001100000011000000010000000100000001000000010000000111110001

problem_t problem_types[] = {
    // Standard Problem
    {.blank = STANDARD_BLANK,
     .reverse_lookup = reverse_lookup_standard,
     .n_pieces = 8,
     .piece_position_num = {48, 80, 82, 96, 151, 154, 154, 196},
     .pieces = {0xE0E0000000000000, 0x8080E00000000000, 0x20E0800000000000,
                0xA0E0000000000000, 0x80F0000000000000, 0x20F0000000000000,
                0x30E0000000000000, 0xC0E0000000000000},
     .piece_props = {{2, false},
                     {4, false},
                     {2, true},
                     {4, false},
                     {4, true},
                     {4, true},
                     {4, true},
                     {4, true}}},

    // Standard Problem T
    {.blank = STANDARD_BLANK,
     .reverse_lookup = reverse_lookup_standard,
     .n_pieces = 8,
     .piece_position_num = {48, 80, 82, 96, 151, 154, 154, 196},
     .pieces = {0xE0E0000000000000, 0x8080E00000000000, 0x20E0800000000000,
                0xA0E0000000000000, 0x80F0000000000000, 0x20E0200000000000,
                0x30E0000000000000, 0xC0E0000000000000},
     .piece_props = {{2, false},
                     {4, false},
                     {2, true},
                     {4, false},
                     {4, true},
                     {4, false},
                     {4, true},
                     {4, true}}},
    // Weekdays problem
    {.blank = WEEKDAYS_BLANK,
     .reverse_lookup = reverse_lookup_weekdays,
     .n_pieces = 10,
     .piece_position_num = {55, 100, 102, 102, 118, 122, 191, 194, 240, 242},
     .pieces = {0xF000000000000000, 0x8080E00000000000, 0x20E0800000000000,
                0x20E0200000000000, 0xA0E0000000000000, 0x60C0000000000000,
                0x80F0000000000000, 0xE030000000000000, 0xE0C0000000000000,
                0x80E0000000000000},
     .piece_props = {{2, false},
                     {4, false},
                     {2, true},
                     {4, false},
                     {4, false},
                     {2, true},
                     {4, true},
                     {4, true},
                     {4, true},
                     {4, true}}},
    {.blank = EMPTY_BLANK,
     .reverse_lookup = reverse_lookup_generic,
     .n_pieces = 11,
     .piece_position_num = {512, 512, 512, 512, 512, 512, 512, 512, 512, 512,
                            512},
     .pieces = {0xF0B0800000000000, 0xF090000000000000, 0xC0E0600000000000,
                0x8080E00000000000, 0x20E0200000000000, 0x80F0000000000000,
                0xE0C0000000000000, 0x80E0E00000000000, 0x80C0600000000000,
                0x40E0400000000000, 0xE040600000000000

     },
     .piece_props = {{4, true},
                     {4, false},
                     {2, false},
                     {4, false},
                     {4, false},
                     {4, true},
                     {4, true},
                     {4, true},
                     {4, false},
                     {1, false},
                     {4, true}}},
    {.blank = STANDARD_BLANK,
     .reverse_lookup = reverse_lookup_standard,
     .n_pieces = 8,
     .piece_position_num = {512, 512, 512, 512, 512, 512, 512, 512},
     .pieces = {0xE0E0000000000000, 0xC0E0000000000000, 0xC0E0000000000000,
                0xA0E0000000000000, 0x80F0000000000000, 0xC0E0000000000000,
                0x30E0000000000000, 0xC0E0000000000000},
     .piece_props = {{2, false},
                     {4, true},
                     {4, true},
                     {4, false},
                     {4, true},
                     {4, true},
                     {4, true},
                     {4, true}}}

};

int parse_standard_problem(FILE *fp, problem_t *p) {
  p->reverse_lookup = reverse_lookup_standard;
  p->blank = STANDARD_BLANK;
  p->n_pieces = 0;

  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  size_t n = 0;
  while (n < MAX_NUM_SOLUTIONS && (read = getline(&line, &len, fp)) != -1) {
    piece_t piece = strtoul(line, NULL, 16);
    if (piece == 0) {
      return 1;
    }

    p->n_pieces++;
    p->piece_position_num[n] = 512;
    p->pieces[n] = piece;
    p->piece_props[n] = get_piece_properties(piece);

    printf("%s", line);
    n++;
  }

  if (line)
    free(line);
  return 0;
}

uint32_t month_location(uint32_t month) {
  if (month > 12) {
    return 64 - 1;
  }

  if (month <= 6) {
    return 64 - month;
  } else {
    return 62 - month;
  }
}

uint32_t day_location(uint32_t day) {

  if (day > 31) {
    return 48 - 1; // First day
  }

  if (day <= 7) {
    return 48 - day;
  } else if (day <= 14) {
    return 47 - day;
  } else if (day <= 21) {
    return 46 - day;
  } else if (day <= 28) {
    return 45 - day;
  } else {
    return 44 - day;
  }
}

uint32_t weekday_location(uint32_t day) {
  if (day < 4) {
    return 12 - day;
  } else {
    return 7 - day;
  }
}

uint32_t index_location(uint32_t index) {
  if (index < 6) {
    return 63 - index;
  } else if (index < 12) {
    return 61 - index;
  } else if (index < 19) {
    return 59 - index;
  } else if (index < 26) {
    return 58 - index;
  } else if (index < 33) {
    return 57 - index;
  } else if (index < 40) {
    return 56 - index;
  } else if (index < 47) {
    return 55 - index;
  } else {
    return 50 - index;
  }
}

status_t make_generic(problem_t *prob) {
  memcpy(prob, &problem_types[3], sizeof(problem_t));

  return STATUS_OK;
}

status_t make_problem(problem_t *prob, piece_t *pieces, uint32_t pos1,
                      uint32_t pos2) {
  if (pos1 >= 64 || pos2 >= 64 || pos1 == pos2) {
    return WRONG_INPUT;
  }

  prob->n_pieces = problem_types[0].n_pieces;
  prob->blank = problem_types[0].blank;
  prob->problem = problem_types[0].blank;
  prob->reverse_lookup = problem_types[0].reverse_lookup;

  struct solution_restrictions res = {true, true};

  for (int i = 0; i < prob->n_pieces; i++) {
    prob->pieces[i] = pieces[i];
    prob->piece_props[i] = get_piece_properties(pieces[i]);
    prob->piece_position_num[i] = make_positions(
        prob->pieces[i], prob->piece_props[i], prob->blank, NULL, res);
  }

  prob->problem =
      prob->blank | ((board_t)0x01 << pos1) | ((board_t)0x01 << pos2);
  return STATUS_OK;
}

status_t make_problem_standard(problem_t *prob, uint32_t pos1, uint32_t pos2) {
  if (pos1 >= 64 || pos2 >= 64 || pos1 == pos2) {
    return WRONG_INPUT;
  }

  memcpy(prob, &problem_types[0],
         sizeof(problem_t)); // Copy problem from template

  prob->problem =
      prob->blank | ((board_t)0x01 << pos1) | ((board_t)0x01 << pos2);
  return STATUS_OK;
}
status_t make_problem_t(problem_t *prob, uint32_t pos1, uint32_t pos2) {
  if (pos1 >= 64 || pos2 >= 64 || pos1 == pos2) {
    return WRONG_INPUT;
  }

  memcpy(prob, &problem_types[1],
         sizeof(problem_t)); // Copy problem from template

  prob->problem =
      prob->blank | ((board_t)0x01 << pos1) | ((board_t)0x01 << pos2);
  return STATUS_OK;
}

status_t make_problem_weekday(problem_t *prob, uint32_t pos1, uint32_t pos2,
                              uint32_t pos3) {
  if (pos1 >= 64 || pos2 >= 64 || pos3 >= 64 || pos1 == pos2 || pos1 == pos3 ||
      pos2 == pos3) {
    return WRONG_INPUT;
  }

  memcpy(prob, &problem_types[2],
         sizeof(problem_t)); // Copy problem from template

  prob->problem = prob->blank | ((board_t)0x01 << pos1) |
                  ((board_t)0x01 << pos2) | ((board_t)0x01 << pos3);
  return STATUS_OK;
}

status_t make_from_date(problem_t *prob, uint32_t day, uint32_t month) {

  return make_problem_standard(prob, month_location(month), day_location(day));
}
status_t make_from_date_weekday(problem_t *prob, uint32_t day, uint32_t month,
                                uint32_t wd) {

  return make_problem_weekday(prob, month_location(month), day_location(day),
                              weekday_location(wd));
}
