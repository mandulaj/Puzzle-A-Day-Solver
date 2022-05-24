
#include "utils.h"
#include "problem.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static char *ERROR_DESCRIPTIONS[] = {"Status Ok", "Warning", "General Error",
                                     "Wrong input provided",
                                     "Memory allocation error"};

const char *get_error_description(status_t s) {
  if (s < sizeof(ERROR_DESCRIPTIONS) / sizeof(ERROR_DESCRIPTIONS[0])) {
    return ERROR_DESCRIPTIONS[s];
  }
  return ERROR_DESCRIPTIONS[WRONG_INPUT];
}

int isNumber(char s[]) {
  for (int i = 0; s[i] != '\0'; i++) {
    if (isdigit(s[i]) == 0)
      return 0;
  }
  return 1;
}

uint32_t parse_location(char *str) {
  char *months[] = {"jan", "feb", "mar", "apr", "may", "jun",
                    "jul", "aug", "sep", "oct", "nov", "dec"};

  char *weekdays[] = {"mon", "tue", "wed", "thr", "fri", "sat", "sun"};

  if (isNumber(str)) {
    int num = 0;
    sscanf(str, "%d", &num);
    if (num >= 1 && num <= 31) {
      return day_location(num);
    } else {
      printf("Day must be between 1 and 31\n");
      return 0;
    }
  } else {
    for (int i = 0; i < 12; i++) {
      if (strcmp(months[i], str) == 0) {
        return month_location(i + 1);
      }
    }

    for (int i = 0; i < 7; i++) {
      if (strcmp(weekdays[i], str) == 0) {
        return weekday_location(i);
      }
    }

    return 0;
  }

  return 1;
}
