
#include "utils.h"

static char *ERROR_DESCRIPTIONS[] = {"Status Ok", "Warning", "General Error",
                                     "Wrong input provided",
                                     "Memory allocation error"};

const char *get_error_description(status_t s) {
  if (s < sizeof(ERROR_DESCRIPTIONS) / sizeof(ERROR_DESCRIPTIONS[0])) {
    return ERROR_DESCRIPTIONS[s];
  }
  return ERROR_DESCRIPTIONS[WRONG_INPUT];
}