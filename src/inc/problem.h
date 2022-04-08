#pragma once
#include <stdlib.h>
#pragma once

#include "board.h"
#include <stdint.h>
#include <stdio.h>

uint32_t month_location(uint32_t month);

uint32_t day_location(uint32_t day);

uint32_t index_location(uint32_t index);

board_t make_problem(uint32_t pos1, uint32_t pos2);

board_t make_from_date(uint32_t day, uint32_t month);