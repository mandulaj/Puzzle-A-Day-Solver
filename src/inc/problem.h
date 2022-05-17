#pragma once
#include <stdlib.h>
#pragma once

#include "board.h"
#include "piece.h"
#include <stdint.h>
#include <stdio.h>


#define MAX_PIECES 10


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



uint32_t month_location(uint32_t month);

uint32_t day_location(uint32_t day);

uint32_t weekday_location(uint32_t day);

uint32_t index_location(uint32_t index);

bool make_problem_standard(problem_t *prob, uint32_t pos1, uint32_t pos2);
bool make_problem_t(problem_t *prob, uint32_t pos1, uint32_t pos2);
bool make_problem_weekday(problem_t *prob, uint32_t pos1, uint32_t pos2,
                          uint32_t pos3);
bool make_from_date(problem_t *prob, uint32_t day, uint32_t month);