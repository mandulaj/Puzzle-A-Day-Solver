

#include "piece.h"
#include <stdio.h>
#include <x86intrin.h>

const piece_t LEFT_EDGE = 0x8080808080808080;
const piece_t RIGHT_EDGE = 0x0101010101010101;
const piece_t TOP_EDGE = 0xFF00000000000000;
const piece_t BOTTOM_EDGE = 0x00000000000000FF;

piece_t piece_rotate(piece_t piece) {
  piece_t new = (piece_t)0x00;
  piece_t mask = LEFT_EDGE;

  for (int i = 0; i < 8; i++) {
    new |= _pext_u64(piece, mask) << 8 * i;
    mask >>= 1;
  }
  return new;
}

piece_t piece_flip(piece_t piece) {
  return ((piece & 0xFF) << 56) | ((piece & 0xFF00) << 40) |
         ((piece & 0xFF0000) << 24) | ((piece & 0xFF000000) << 8) |
         ((piece & 0xFF00000000) >> 8) | ((piece & 0xFF0000000000) >> 24) |
         ((piece & 0xFF000000000000) >> 40) |
         ((piece & 0xFF00000000000000) >> 56);
}

piece_t piece_origin(piece_t piece) {

  return piece_place_up(piece_place_left(piece));
}

piece_t piece_place_left(piece_t piece) {
  piece_t old;

  do {
    old = piece;
    piece = piece_sft_left(old);
  } while (piece != old);
  return piece;
}

piece_t piece_place_up(piece_t piece) {
  piece_t old;

  do {
    old = piece;
    piece = piece_sft_up(old);
  } while (piece != old);
  return piece;
}

piece_t piece_sft_left(piece_t piece) {
  if ((piece & LEFT_EDGE) == 0) {
    return piece << 1;
  }
  return piece;
}
piece_t piece_sft_right(piece_t piece) {
  if ((piece & RIGHT_EDGE) == 0) {
    return piece >> 1;
  }
  return piece;
}
piece_t piece_sft_up(piece_t piece) {
  if ((piece & TOP_EDGE) == 0) {
    return piece << 8;
  }
  return piece;
}

piece_t piece_sft_down(piece_t piece) {
  if ((piece & BOTTOM_EDGE) == 0) {
    return piece >> 8;
  }
  return piece;
}