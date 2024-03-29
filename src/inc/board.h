
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
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

/**
 * Board state representing occupied positions as `1`
 */
typedef uint64_t board_t;

struct hole_checker {
  board_t *double_hole_masks;
  board_t *double_hole_crosses;
  uint64_t num_double_holes;
  uint64_t num_double_blocks;
};

/**
 * @brief Print raw solution
 */
void print_raw(uint64_t soution);

void print_2_raw(uint64_t pattern1, uint64_t pattern2);

void init_hole_checker(board_t problem, struct hole_checker *hc);
void free_hole_checker(struct hole_checker *hc);

// bool check_holes(board_t board, struct hole_checker *hc);
bool check_holes(board_t board);
void print_hole_checker(struct hole_checker *hc);
void get_date(board_t b, uint8_t *positions, size_t *n_positions);
