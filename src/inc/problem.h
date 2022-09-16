#pragma once
#include <stdlib.h>

#include "board.h"
#include "piece.h"
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "config.h"
#include "utils.h"

#define STANDARD_PROBLEM_INDEX 0
#define T_PROBLEM_INDEX 1
#define WEEKDAYS_PROBLEM_INDEX 2
#define FACEUP8_PROBLEM_INDEX 5

#define EMPTY_BLANK                                                            \
  (0b0000000000000000000000000000000000000000000000000000000000000000)
#define STANDARD_BLANK                                                         \
  (0b0000001100000011000000010000000100000001000000010001111111111111)
#define WEEKDAYS_BLANK                                                         \
  (0b0000001100000011000000010000000100000001000000010000000111110001)

enum puzzle_mode {
  NONE_PUZZLE,
  STANDARD_PUZZLE,
  CUSTOM_PIECES,
  T_PUZZLE,
  WEEKEND_PUZZLE,
  GENERIC8x8_PUZZLE,
  FACEUP8_PUZZLE
};
typedef struct problem_t {
  // Blank board
  board_t blank;

  // Problem
  board_t problem;

  const char **reverse_lookup;
  size_t n_pieces;
  size_t piece_position_num[MAX_PIECES];
  piece_t pieces[MAX_PIECES];
  piece_properties_t piece_props[MAX_PIECES];

} problem_t;

int parse_standard_problem(FILE *fp, problem_t *p);

uint32_t month_location(uint32_t month);

uint32_t day_location(uint32_t day);

uint32_t weekday_location(uint32_t day);

uint32_t index_location(uint32_t index);

status_t make_generic(problem_t *prob);
status_t make_problem(problem_t *prob, const piece_t *pieces, uint32_t pos1,
                      uint32_t pos2);
status_t make_problem_standard(problem_t *prob, uint32_t pos1, uint32_t pos2);
status_t make_problem_t(problem_t *prob, uint32_t pos1, uint32_t pos2);
status_t make_problem_weekday(problem_t *prob, uint32_t pos1, uint32_t pos2,
                              uint32_t pos3);
status_t make_from_date(problem_t *prob, uint32_t day, uint32_t month);
status_t make_from_date_weekday(problem_t *prob, uint32_t day, uint32_t month,
                                uint32_t wd);

status_t make_problem_faceup8(problem_t *prob, uint32_t pos1, uint32_t pos2);

status_t make_problem_nPcs(problem_t *prob, const piece_t *pieces, size_t n_pcs,
                           uint32_t pos1, uint32_t pos2);