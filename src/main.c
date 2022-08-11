#include "board.h"
#include "config.h"
#include "piece.h"
#include "printing.h"
#include "problem.h"
#include "solver.h"
#include "utils.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern problem_t problem_types[];

#define GET_BIT(p, x, y) (!!((p << ((y)*8 + (x))) & 0x8000000000000000))

#define SET_BIT(p, x, y) p |= (0x8000000000000000 >> ((y)*8 + (x)))
#define CLEAR_BIT(p, x, y) p &= ~(0x8000000000000000 >> ((y)*8 + (x)))

inline bool is_valid_position(piece_t p, int x, int y) {
  bool valid_r = false;
  bool valid_l = false;
  bool valid_u = false;
  bool valid_d = false;

  if (x > 0) {
    valid_l = GET_BIT(p, x - 1, y);
  }

  if (x < 8) {
    valid_r = GET_BIT(p, x + 1, y);
  }

  if (y < 8) {
    valid_d = GET_BIT(p, x, y + 1);
  }

  if (y > 0) {
    valid_d = GET_BIT(p, x, y - 1);
  }

  return valid_r || valid_l || valid_u || valid_d;
}

enum bit_direction {
  DIRECTION_RIGHT,
  DIRECTION_DOWN,
  DIRECTION_LEFT,
  DIRECTION_UP
};

piece_t attach_bit(piece_t p, uint32_t pos) {
  uint32_t c_pos = 0;
  enum bit_direction next_direction = DIRECTION_RIGHT;
  int x = 0;
  int y = 0;

  if (piece_order(p) > 7)
    return 0x00;

  p = piece_sft_right(piece_sft_down(piece_origin(p)));

  while (!GET_BIT(p, x, y + 1)) {
    x++;
  }

  while (1) {
    if (is_valid_position(p, x, y)) {
      c_pos++;
      if (c_pos > pos) {
        break;
      }
    }

    switch (next_direction) {
    case DIRECTION_RIGHT: // >
      if (!GET_BIT(p, x + 1, y + 1)) {
        next_direction = DIRECTION_DOWN;
        x += 1;
        y += 1;
      } else if (!GET_BIT(p, x + 1, y)) {
        x += 1;
      } else if (!GET_BIT(p, x + 1, y - 1)) {
        y -= 1;
        x += 1;
      } else if (!GET_BIT(p, x, y - 1)) {
        next_direction = DIRECTION_UP;
        y -= 1;
      } else if (!GET_BIT(p, x - 1, y - 1)) {
        next_direction = DIRECTION_UP;
        y -= 1;
        x -= 1;
      } else if (!GET_BIT(p, x - 1, y)) {
        next_direction = DIRECTION_RIGHT;
        x -= 1;
      } else {
        printf("Invalid shape\n");
        return 0;
      }
      break;
    case DIRECTION_DOWN: // V
      if (!GET_BIT(p, x - 1, y + 1)) {
        next_direction = DIRECTION_LEFT;
        x -= 1;
        y += 1;
      } else if (!GET_BIT(p, x, y + 1)) {
        y += 1;
      } else if (!GET_BIT(p, x + 1, y + 1)) {
        y += 1;
        x += 1;
      } else if (!GET_BIT(p, x + 1, y)) {
        next_direction = DIRECTION_RIGHT;
        x += 1;
      } else if (!GET_BIT(p, x + 1, y - 1)) {
        next_direction = DIRECTION_RIGHT;
        y -= 1;
        x += 1;
      } else if (!GET_BIT(p, x, y - 1)) {
        next_direction = DIRECTION_UP;
        y -= 1;
      } else {
        printf("Invalid shape\n");
        return 0;
      }
      break;
    case DIRECTION_LEFT: // <
      if (!GET_BIT(p, x - 1, y - 1)) {
        next_direction = DIRECTION_UP;
        x -= 1;
        y -= 1;
      } else if (!GET_BIT(p, x - 1, y)) {
        x -= 1;
      } else if (!GET_BIT(p, x - 1, y + 1)) {
        y += 1;
        x -= 1;
      } else if (!GET_BIT(p, x, y + 1)) {
        next_direction = DIRECTION_DOWN;
        y += 1;
      } else if (!GET_BIT(p, x + 1, y + 1)) {
        next_direction = DIRECTION_DOWN;
        y += 1;
        x += 1;
      } else if (!GET_BIT(p, x + 1, y)) {
        next_direction = DIRECTION_RIGHT;
        x += 1;
      } else {
        printf("Invalid shape\n");
        return 0;
      }
      break;
    case DIRECTION_UP: // ^
    default:
      if (!GET_BIT(p, x + 1, y - 1)) {
        next_direction = DIRECTION_RIGHT;
        x += 1;
        y -= 1;
      } else if (!GET_BIT(p, x, y - 1)) {
        y -= 1;
      } else if (!GET_BIT(p, x - 1, y - 1)) {
        y -= 1;
        x -= 1;
      } else if (!GET_BIT(p, x - 1, y)) {
        next_direction = DIRECTION_LEFT;
        x -= 1;
      } else if (!GET_BIT(p, x - 1, y + 1)) {
        next_direction = DIRECTION_LEFT;
        y += 1;
        x -= 1;
      } else if (!GET_BIT(p, x, y + 1)) {
        next_direction = DIRECTION_DOWN;
        y += 1;
      } else {
        printf("Invalid shape\n");
        return 0;
      }
      break;
    }
  }

  SET_BIT(p, x, y);

  return piece_origin(p);
}

piece_t make_piece(uint64_t gene, uint32_t length) {
  piece_t p = 0x00;
  // First 2 bits (3 pieces)
  if ((gene & 0x03) == 2) {
    p = 0x80C0000000000000;
  } else if ((gene & 0x03) == 3) {
    p = 0x40C0000000000000;
  } else {
    p = 0xE000000000000000;
  }

  // Next 3 bits (4 pieces)
  if (length >= 4) {
    p = attach_bit(p, (gene >> (2)) & 0x07);
  }

  // Next 4 bit sets
  if (length >= 5) {
    for (int i = 0; i < length - 4; i++) {
      p = attach_bit(p, (gene >> (5 + i * 4)) & 0x0F);
    }
  }

  return p;
}

void make_masks(board_t *masks, board_t *crosses_test) {
  // Corners
  masks[0] = 0xC080000000000000;
  crosses_test[0] = 0x4080000000000000;
  masks[7] = 0x0301000000000000;
  crosses_test[7] = 0x0201000000000000;
  masks[56] = 0x00000000000080C0;
  crosses_test[56] = 0x0000000000008040;
  masks[63] = 0x0000000000000103;
  crosses_test[63] = 0x0000000000000102;

  // Top/Bottom
  board_t top_mask = 0xE040000000000000;
  board_t top_cross = 0xA040000000000000;
  board_t bottom_mask = 0x00000000000040E0;
  board_t bottom_cross = 0x00000000000040A0;
  for (int i = 1; i < 7; i++) {
    masks[i] = top_mask;
    crosses_test[i] = top_cross;
    masks[56 + i] = bottom_mask;
    crosses_test[56 + i] = bottom_cross;

    top_mask >>= 1;
    top_cross >>= 1;
    bottom_mask >>= 1;
    bottom_cross >>= 1;
  }

  // Left Right
  board_t left_mask = 0x80C0800000000000;
  board_t left_cross = 0x8040800000000000;

  board_t right_mask = 0x0103010000000000;
  board_t right_cross = 0x0102010000000000;

  for (int j = 1; j < 7; j++) {
    masks[j * 8] = left_mask;
    crosses_test[j * 8] = left_cross;

    masks[j * 8 + 7] = right_mask;
    crosses_test[j * 8 + 7] = right_cross;

    left_mask >>= 8;
    left_cross >>= 8;
    right_mask >>= 8;
    right_cross >>= 8;
  }

  // Field
  board_t f_masks[] = {0x00,
                       0x40E0400000000000,
                       0x0040E04000000000,
                       0x000040E040000000,
                       0x00000040E0400000,
                       0x0000000040E04000,
                       0x000000000040E040};
  board_t f_crosses[] = {0x00,
                         0x40A0400000000000,
                         0x0040A04000000000,
                         0x000040A040000000,
                         0x00000040A0400000,
                         0x0000000040A04000,
                         0x000000000040A040};
  for (int i = 1; i < 7; i++) {
    for (int j = 1; j < 7; j++) {
      masks[j * 8 + i] = f_masks[j];
      f_masks[j] >>= 1;
      crosses_test[j * 8 + i] = f_crosses[j];
      f_crosses[j] >>= 1;
    }
  }
}

void make_masks2(board_t *masks, board_t *crosses) {

  for (int i = 0; i < 7; i++) {
    board_t center = 0x8080000000000000;
    center >>= i * 8;
    for (int j = 0; j < 8; j++) {
      board_t mask = piece_sft_up(center) | piece_sft_down(center) |
                     piece_sft_right(center) | piece_sft_left(center);
      board_t cross = mask ^ center;
      center >>= 1;
      *masks++ = mask;
      *crosses++ = cross;
    }
  }

  for (int i = 0; i < 8; i++) {
    board_t center = 0xC000000000000000;
    center >>= i * 8;
    for (int j = 0; j < 7; j++) {
      board_t mask = piece_sft_up(center) | piece_sft_down(center) |
                     piece_sft_right(center) | piece_sft_left(center);
      board_t cross = mask ^ center;
      center >>= 1;
      *masks++ = mask;
      *crosses++ = cross;
    }
  }
}

bool check_holess(board_t board, board_t *masks, board_t *crosses_test) {
  for (int i = 0; i < 64; i++) {
    board_t sel = board & masks[i];
    if (sel == crosses_test[i]) {
      return false;
    }
  }
  return true;
}

int main() {

  //   piece_t piece = 0x80C0000000000000;
  //   print_piece(piece, 0);

  // board_t masks[112];
  // board_t crosses_test[112];

  // make_masks2(masks, crosses_test);

  // for (int i = 0; i < 112; i++) {
  //   printf("0x%016lX,", masks[i]);
  // }
  // printf("\n");

  // for (int i = 0; i < 112; i++) {
  //   printf("0x%016lx,", crosses_test[i]);
  // }
  // printf("\n");

  board_t p[] = {0x0000000030000000, 0xFFFFFFEFFFFFFFFF, 0xFFFFFFFFFFFFFF00,
                 0xFFFFFFFFFFFFF000, 0xFFFFFFFFFFEFFFFF, 0xFFFFFFFFFFEFEFFF};

  for (int i = 0; i < 6; i++) {
    print_raw_color(p[i], 0);

    printf("%s\n", check_holes_single(p[i]) ? "pass" : "fail");
  }

  return 0;

  for (int i0 = 0; i0 < 4; i0++) {
    print_piece(make_piece(i0, 3), 0);

    for (uint64_t i1 = 0; i1 < 8; i1++) {
      print_piece(make_piece(i1 << 2 | i0, 4), 1);

      for (uint64_t i2 = 0; i2 < 16; i2++) {
        print_piece(make_piece(i2 << 5 | i1 << 2 | i0, 5), 2);
        for (uint64_t i3 = 0; i3 < 16; i3++) {
          print_piece(make_piece(i3 << 9 | i2 << 5 | i1 << 2 | i0, 5), 3);
          printf("================\n");
          for (uint64_t i4 = 0; i4 < 16; i4++) {

            piece_t new =
                make_piece(i4 << 13 | i3 << 9 | i2 << 5 | i1 << 2 | i0, 7);

            print_piece(new, (i4 + 4) % 10);
          }
        }
      }
    }
  }

  //   for (int i = 0; i < 4; i++) {
  //     printf("Problem %d\n", i);

  //     for (int j = 0; j < problem_types[i].n_pieces; j++) {
  //       piece_t piece = problem_types[i].pieces[j];

  //       piece_properties_t computed_ppts = get_piece_properties(piece);
  //       piece_properties_t true_ppts = problem_types[i].piece_props[j];
  //       print_piece(piece, j);
  //       printf("True rotations: %ld, true asymetry: %d || Comp rot: %ld,
  //       comp
  //       "
  //              "asym: %d\n",
  //              true_ppts.rotations, true_ppts.asymetric,
  //              computed_ppts.rotations, computed_ppts.asymetric);
  //       if ((true_ppts.rotations != computed_ppts.rotations) ||
  //           (true_ppts.asymetric != computed_ppts.asymetric)) {
  //         printf("NO MATCH!!!!!!!\n");
  //       }
  //     }
  //   }
}