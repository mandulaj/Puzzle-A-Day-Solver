

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

piece_t get_piece(const piece_t *pieces, size_t n, piece_location_t location) {
  if (location.piece_id >= n) {
    return 0x00;
  }

  piece_t p = pieces[location.piece_id];

  if (location.flip) {
    p = piece_flip(p);
  }

  for (int i = 0; i < location.rot && i < 4; i++) {
    p = piece_rotate(p);
  }

  p = piece_origin(p);

  for (int x = 0; x < location.x && x < 8; x++) {
    p = piece_sft_right(p);
  }
  for (int y = 0; y < location.y && y < 8; y++) {
    p = piece_sft_down(p);
  }
  return p;
}

piece_properties_t get_piece_properties(piece_t piece) {
  // Move piece to corner
  piece = piece_origin(piece);
  piece_properties_t ppts = {1, true};

  piece_t rot1 = piece_origin(piece_rotate(piece));
  piece_t rot2 = piece_origin(piece_rotate(rot1));

  if (piece == rot2 && piece != rot1) {
    ppts.rotations = 2;
  } else if (piece != rot2 && piece != rot1) {
    ppts.rotations = 4;
  }

  piece_t flipped = piece_flip(piece);

  // Chek all orientations if flipped piece matches
  for (int i = 0; i < 4; i++) {
    flipped = piece_origin(piece_rotate(flipped));

    if (flipped == piece) {
      ppts.asymetric = false;
      break;
    }
  }

  return ppts;
}

uint64_t piece_order(piece_t p) { return __popcntq(p); }

bool same_piece(piece_t p1, piece_t p2) {
  if (piece_order(p1) != piece_order(p2)) {
    return false;
  }

  p1 = piece_origin(p1);
  p2 = piece_origin(p2);

  for (int reflection = 0; reflection < 2; reflection++) {
    for (int rotation = 0; rotation < 4; rotation++) {
      p2 = piece_origin(piece_rotate(p2));
      if (p1 == p2) {
        return true;
      }
    }
    p2 = piece_flip(p2);
  }
  return false;
}
