
/** @file board.h
 *
 * @brief Functions and structures related to the board
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Jakub Mandula  All rights reserved.
 */

#pragma once

#include "config.h"
#include "piece.h"
#include <stdint.h>

/**
 * Board state representing occupied positions as `1`
 */
typedef uint64_t board_t;

/**
 * Structure representing a solution with an array of the solution pieces
 */
typedef struct solution_t {
  piece_t pieces[MAX_PIECES];
} solution_t;

/**
 * @brief Print raw solution
 */
void print_raw(uint64_t soution);

void print_2_raw(uint64_t pattern1, uint64_t pattern2);
