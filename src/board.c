#include "board.h"
#include <stdio.h>

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

void print_2_raw(uint64_t pattern1, uint64_t pattern2) {

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      uint64_t bit1 = pattern1 & 0x8000000000000000;
      uint64_t bit2 = pattern2 & 0x8000000000000000;
      pattern1 <<= 1;
      pattern2 <<= 1;

      if (bit1 && bit2) {
        printf("X");
        // printf("%s", 0xe296a0);
      } else if (bit1) {
        printf("#");
      } else if (bit2) {
        printf("@");

      } else {
        printf(".");
      }
    }
    printf("\n");
  }
  printf("\n");
}
