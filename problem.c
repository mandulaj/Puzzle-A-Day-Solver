#include "problem.h"

uint32_t month_location(uint32_t month) {
  if (month <= 6) {
    return 64 - month;
  } else {
    return 62 - month;
  }
}

uint32_t day_location(uint32_t day) {

  if (day <= 7) {
    return 48 - day;
  } else if (day <= 14) {
    return 47 - day;
  } else if (day <= 21) {
    return 46 - day;
  } else if (day <= 28) {
    return 45 - day;
  } else {
    return 44 - day;
  }
}

board_t make_problem(uint32_t pos1, uint32_t pos2) {
  if (pos1 >= 64 || pos2 >= 64 || pos1 == pos2) {
    return 0xFFFFFFFFFFFFFFFF;
  }

  return BLANK_BOARD | ((board_t)0x01 << pos1) | ((board_t)0x01 << pos2);
}
board_t make_from_date(uint32_t day, uint32_t month) {

  return make_problem(month_location(month), day_location(day));
}