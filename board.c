#include "board.h"
#include <stdio.h>

const board_t BLANK_BOARD =
    0b0000001100000011000000010000000100000001000000010001111111111111;

char *reverse_lookup[] = {
    "",    "",   "",   "",    "",    "",    "",    "",    "",    "",    "",
    "",    "",   "31", "30",  "29",  "",    "28",  "27",  "26",  "25",  "24",
    "23",  "22", "",   "21",  "20",  "19",  "18",  "17",  "16",  "15",  "",
    "14",  "13", "12", "11",  "10",  "9",   "8",   "",    "7",   "6",   "5",
    "4",   "3",  "2",  "1",   "",    "",    "Dec", "Nov", "Oct", "Sep", "Aug",
    "Jul", "",   "",   "Jun", "May", "Apr", "Mar", "Feb", "Jan"};

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
