#include "board.h"
#include <stdio.h>

const board_t BLANK_BOARD =
    0b0000001100000011000000010000000100000001000000010001111111111111;

void print_raw(uint64_t pattern) {

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (pattern & 0x8000000000000000) {
        printf("X");
        // printf("%s", 0xe296a0);
      } else {
        printf(".");
      }
      pattern <<= 1;
    }
    printf("\n");
  }
  printf("\n");
}
