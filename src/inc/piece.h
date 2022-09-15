
#pragma once

#include "config.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <unistd.h>

typedef uint64_t piece_t;

typedef struct piece_location_t {
  size_t x;
  size_t y;
  size_t rot;
  size_t piece_id;
  bool flip;
} piece_location_t;

typedef struct piece_properties_t {
  uint8_t rotations;
  bool asymetric;
} piece_properties_t;

extern const piece_t LEFT_EDGE;
extern const piece_t RIGHT_EDGE;
extern const piece_t TOP_EDGE;
extern const piece_t BOTTOM_EDGE;

piece_t piece_sft_left(piece_t piece);
piece_t piece_sft_right(piece_t piece);
piece_t piece_sft_up(piece_t piece);
piece_t piece_sft_down(piece_t piece);

piece_t piece_flip(piece_t piece);
piece_t piece_rotate(piece_t piece);

piece_t piece_origin(piece_t piece);

piece_t piece_place_left(piece_t piece);
piece_t piece_place_up(piece_t piece);

piece_t get_piece(const piece_t *pieces, size_t n, piece_location_t location);

piece_properties_t get_piece_properties(const piece_t piece);

uint64_t piece_order(piece_t p);

bool same_piece(piece_t p1, piece_t p2);

ssize_t parse_standard_pieces(FILE *fp, piece_t *p);