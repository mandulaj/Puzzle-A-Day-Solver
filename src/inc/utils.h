#pragma once

#include "config.h"
#include <stdint.h>

typedef enum errors {
  STATUS_OK = 0,
  WARNING = 1,
  ERROR = 2,
  WRONG_INPUT = 3,
  MEMORY_ERROR = 4,
  INVALID_POSITION = 5,
  DUPLICATE_PIECE = 6
} status_t;

const char *get_error_description(status_t s);

int isNumber(char s[]);
uint32_t parse_location(char *str);
