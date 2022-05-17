#include "problem.h"
#include <stdbool.h>
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

problem_t problem_types[] = {
    // Standard Problem
    {.blank =
         0b0000001100000011000000010000000100000001000000010001111111111111,
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
    {.blank =
         0b0000001100000011000000010000000100000001000000010001111111111111,
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
    {.blank =
         0b0000001100000011000000010000000100000001000000010000000111110001,
     .reverse_lookup = reverse_lookup_weekdays,
     .n_pieces = 10,
     .piece_position_num = {55, 100, 102, 102, 118, 122, 191, 194, 240, 242},
     .pieces = {0xF000000000000000, 0x8080E00000000000, 0x20E0800000000000,
                0x20E0200000000000, 0xA0E0000000000000, 0xC060000000000000,
                0x80F0000000000000, 0xE030000000000000, 0xC0E0000000000000,
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
                     {4, true}}}};

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
  } else {
    return 55 - index;
  }
}

bool make_problem_standard(problem_t *prob, uint32_t pos1, uint32_t pos2) {
  if (pos1 >= 64 || pos2 >= 64 || pos1 == pos2) {
    return false;
  }

  memcpy(prob, &problem_types[0],
         sizeof(problem_t)); // Copy problem from template

  prob->problem =
      prob->blank | ((board_t)0x01 << pos1) | ((board_t)0x01 << pos2);
  return true;
}
bool make_problem_t(problem_t *prob, uint32_t pos1, uint32_t pos2) {
  if (pos1 >= 64 || pos2 >= 64 || pos1 == pos2) {
    return false;
  }

  memcpy(prob, &problem_types[1],
         sizeof(problem_t)); // Copy problem from template

  prob->problem =
      prob->blank | ((board_t)0x01 << pos1) | ((board_t)0x01 << pos2);
  return true;
}

bool make_problem_weekday(problem_t *prob, uint32_t pos1, uint32_t pos2,
                          uint32_t pos3) {
  if (pos1 >= 64 || pos2 >= 64 || pos3 >= 64 || pos1 == pos2 || pos1 == pos3 ||
      pos2 == pos3) {
    return false;
  }

  memcpy(prob, &problem_types[2],
         sizeof(problem_t)); // Copy problem from template

  prob->problem = prob->blank | ((board_t)0x01 << pos1) |
                  ((board_t)0x01 << pos2) | ((board_t)0x01 << pos3);
  return true;
}

bool make_from_date(problem_t *prob, uint32_t day, uint32_t month) {

  return make_problem_standard(prob, month_location(month), day_location(day));
}