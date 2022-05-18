#pragma once

#include "config.h"


typedef enum errors {
  STATUS_OK = 0,
  WARNING = 1,
  ERROR = 2,
  WRONG_INPUT = 3,
  MEMORY_ERROR = 4
} status_t;


const char *get_error_description(status_t s);

