

#pragma once

#define USE_SIMD // Use AVX for acceleration
//#define USE_OLD_SIMD // Use old method of AVX acceleration

#define USE_PARALLEL // Parallel Solution
#define MAX_PIECES 11
#define MAX_NUM_SOLUTIONS 1000000
#define SOLUTIONS_BUFFER_SIZE 4096
#define SORT_PATTERNS // Sort patterns based on number of positions

#define CHECK_HOLES
#define CHECK_DOUBLE_HOLES

// Statistics config

#define CACHE_LINE_SIZE 64

#define ONLY_VALID_DATES
//#define INCLUDE_WEEKDAYS

#define __AVX2__ 1

#ifdef USE_SIMD
#if (__AVX512F__ == 1)

// USE AVX512
#define SIMD_AVX512
#elif (__AVX2__ == 1)

// Use AVX2
#define SIMD_AVX2
#else
// No AVX detected, default to no SIMD
#undef USE_SIMD
#endif

#endif