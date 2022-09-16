#include "board.h"
#include "printing.h"
#include <immintrin.h>
#include <x86intrin.h>

#include <stdio.h>

// void printBin(uint64_t b) {
//   for (int i = 63; i >= 0; i--) {
//     printf("%2d ", i);
//   }
//   printf("\n");
//   for (int i = 63; i >= 0; i--) {
//     printf("%2ld ", (b >> i) & 0x01);
//   }
//   printf("\n");
// }

void get_date(board_t b, size_t *position_1, size_t *position_2) {
  b = ~b;
  // printBin(b);
  // printf("%lx, popcont: %lld\n", b, __popcntq(b));
  *position_1 = 1000;
  *position_2 = 1000;

  if (__popcntq(b) != 2) {
    return;
  }

  uint64_t pos1 = __builtin_ctzl(b);

  b ^= (uint64_t)0x1 << pos1;
  // printBin((uint64_t)0x1 << pos1);

  // printBin(b);
  uint64_t pos2 = __builtin_ctzl(b);

  // printf("POsitions: %d, %d\n", pos1, pos2);
  *position_1 = pos1; // The bits are reversed
  *position_2 = pos2;
}

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

void print_rows(uint64_t pattern, uint64_t rows, int color) {

  char *colors[] = {"\x1b[41m",  "\x1b[42m",  "\x1b[43m", "\x1b[44m",
                    "\x1b[45m",  "\x1b[46m",  "\x1b[47m", "\x1b[103m",
                    "\x1b[102m", "\x1b[104m", "\x1b[7m",  "\x1b[0m"};
  char *reset = "\x1b[0m";

  for (int i = 0; i < 8 - rows; i++) {
    pattern <<= 8;
  }
  for (int i = 8 - rows; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (pattern & 0x8000000000000000) {
        printf("%s  %s", colors[color], reset);
        // printf("%s", 0xe296a0);
      } else {
        printf("= ");
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

#ifdef CHECK_HOLES

#ifdef CHECK_DOUBLE_HOLES
#define N_DOUBLE_HOLES 112
#define N_SINGLE_HOLES 64

static const board_t DOUBLE_HOLE_MASKS_RAW[N_DOUBLE_HOLES] = {
    0xC0C0800000000000, 0xE0E0400000000000, 0x7070200000000000,
    0x3838100000000000, 0x1C1C080000000000, 0x0E0E040000000000,
    0x0707020000000000, 0x0303010000000000, 0x80C0C08000000000,
    0x40E0E04000000000, 0x2070702000000000, 0x1038381000000000,
    0x081C1C0800000000, 0x040E0E0400000000, 0x0207070200000000,
    0x0103030100000000, 0x0080C0C080000000, 0x0040E0E040000000,
    0x0020707020000000, 0x0010383810000000, 0x00081C1C08000000,
    0x00040E0E04000000, 0x0002070702000000, 0x0001030301000000,
    0x000080C0C0800000, 0x000040E0E0400000, 0x0000207070200000,
    0x0000103838100000, 0x0000081C1C080000, 0x0000040E0E040000,
    0x0000020707020000, 0x0000010303010000, 0x00000080C0C08000,
    0x00000040E0E04000, 0x0000002070702000, 0x0000001038381000,
    0x000000081C1C0800, 0x000000040E0E0400, 0x0000000207070200,
    0x0000000103030100, 0x0000000080C0C080, 0x0000000040E0E040,
    0x0000000020707020, 0x0000000010383810, 0x00000000081C1C08,
    0x00000000040E0E04, 0x0000000002070702, 0x0000000001030301,
    0x000000000080C0C0, 0x000000000040E0E0, 0x0000000000207070,
    0x0000000000103838, 0x0000000000081C1C, 0x0000000000040E0E,
    0x0000000000020707, 0x0000000000010303, 0xE0C0000000000000,
    0xF060000000000000, 0x7830000000000000, 0x3C18000000000000,
    0x1E0C000000000000, 0x0F06000000000000, 0x0703000000000000,
    0xC0E0C00000000000, 0x60F0600000000000, 0x3078300000000000,
    0x183C180000000000, 0x0C1E0C0000000000, 0x060F060000000000,
    0x0307030000000000, 0x00C0E0C000000000, 0x0060F06000000000,
    0x0030783000000000, 0x00183C1800000000, 0x000C1E0C00000000,
    0x00060F0600000000, 0x0003070300000000, 0x0000C0E0C0000000,
    0x000060F060000000, 0x0000307830000000, 0x0000183C18000000,
    0x00000C1E0C000000, 0x0000060F06000000, 0x0000030703000000,
    0x000000C0E0C00000, 0x00000060F0600000, 0x0000003078300000,
    0x000000183C180000, 0x0000000C1E0C0000, 0x000000060F060000,
    0x0000000307030000, 0x00000000C0E0C000, 0x0000000060F06000,
    0x0000000030783000, 0x00000000183C1800, 0x000000000C1E0C00,
    0x00000000060F0600, 0x0000000003070300, 0x0000000000C0E0C0,
    0x000000000060F060, 0x0000000000307830, 0x0000000000183C18,
    0x00000000000C1E0C, 0x0000000000060F06, 0x0000000000030703,
    0x000000000000C0E0, 0x00000000000060F0, 0x0000000000003078,
    0x000000000000183C, 0x0000000000000C1E, 0x000000000000060F,
    0x0000000000000307};

static const board_t DOUBLE_HOLE_CROSSES_RAW[N_DOUBLE_HOLES] = {
    0x4040800000000000, 0xa0a0400000000000, 0x5050200000000000,
    0x2828100000000000, 0x1414080000000000, 0x0a0a040000000000,
    0x0505020000000000, 0x0202010000000000, 0x8040408000000000,
    0x40a0a04000000000, 0x2050502000000000, 0x1028281000000000,
    0x0814140800000000, 0x040a0a0400000000, 0x0205050200000000,
    0x0102020100000000, 0x0080404080000000, 0x0040a0a040000000,
    0x0020505020000000, 0x0010282810000000, 0x0008141408000000,
    0x00040a0a04000000, 0x0002050502000000, 0x0001020201000000,
    0x0000804040800000, 0x000040a0a0400000, 0x0000205050200000,
    0x0000102828100000, 0x0000081414080000, 0x0000040a0a040000,
    0x0000020505020000, 0x0000010202010000, 0x0000008040408000,
    0x00000040a0a04000, 0x0000002050502000, 0x0000001028281000,
    0x0000000814140800, 0x000000040a0a0400, 0x0000000205050200,
    0x0000000102020100, 0x0000000080404080, 0x0000000040a0a040,
    0x0000000020505020, 0x0000000010282810, 0x0000000008141408,
    0x00000000040a0a04, 0x0000000002050502, 0x0000000001020201,
    0x0000000000804040, 0x000000000040a0a0, 0x0000000000205050,
    0x0000000000102828, 0x0000000000081414, 0x0000000000040a0a,
    0x0000000000020505, 0x0000000000010202, 0x20c0000000000000,
    0x9060000000000000, 0x4830000000000000, 0x2418000000000000,
    0x120c000000000000, 0x0906000000000000, 0x0403000000000000,
    0xc020c00000000000, 0x6090600000000000, 0x3048300000000000,
    0x1824180000000000, 0x0c120c0000000000, 0x0609060000000000,
    0x0304030000000000, 0x00c020c000000000, 0x0060906000000000,
    0x0030483000000000, 0x0018241800000000, 0x000c120c00000000,
    0x0006090600000000, 0x0003040300000000, 0x0000c020c0000000,
    0x0000609060000000, 0x0000304830000000, 0x0000182418000000,
    0x00000c120c000000, 0x0000060906000000, 0x0000030403000000,
    0x000000c020c00000, 0x0000006090600000, 0x0000003048300000,
    0x0000001824180000, 0x0000000c120c0000, 0x0000000609060000,
    0x0000000304030000, 0x00000000c020c000, 0x0000000060906000,
    0x0000000030483000, 0x0000000018241800, 0x000000000c120c00,
    0x0000000006090600, 0x0000000003040300, 0x0000000000c020c0,
    0x0000000000609060, 0x0000000000304830, 0x0000000000182418,
    0x00000000000c120c, 0x0000000000060906, 0x0000000000030403,
    0x000000000000c020, 0x0000000000006090, 0x0000000000003048,
    0x0000000000001824, 0x0000000000000c12, 0x0000000000000609,
    0x0000000000000304};

#else
#define N_HOLES 64
static board_t HOLE_MASKS_RAW[N_HOLES] = {
    0xC080000000000000, 0xE040000000000000, 0x7020000000000000,
    0x3810000000000000, 0x1C08000000000000, 0x0E04000000000000,
    0x0702000000000000, 0x0301000000000000, 0x80C0800000000000,
    0x40E0400000000000, 0x2070200000000000, 0x1038100000000000,
    0x081C080000000000, 0x040E040000000000, 0x0207020000000000,
    0x0103010000000000, 0x0080C08000000000, 0x0040E04000000000,
    0x0020702000000000, 0x0010381000000000, 0x00081C0800000000,
    0x00040E0400000000, 0x0002070200000000, 0x0001030100000000,
    0x000080C080000000, 0x000040E040000000, 0x0000207020000000,
    0x0000103810000000, 0x0000081C08000000, 0x0000040E04000000,
    0x0000020702000000, 0x0000010301000000, 0x00000080C0800000,
    0x00000040E0400000, 0x0000002070200000, 0x0000001038100000,
    0x000000081C080000, 0x000000040E040000, 0x0000000207020000,
    0x0000000103010000, 0x0000000080C08000, 0x0000000040E04000,
    0x0000000020702000, 0x0000000010381000, 0x00000000081C0800,
    0x00000000040E0400, 0x0000000002070200, 0x0000000001030100,
    0x000000000080C080, 0x000000000040E040, 0x0000000000207020,
    0x0000000000103810, 0x0000000000081C08, 0x0000000000040E04,
    0x0000000000020702, 0x0000000000010301, 0x00000000000080C0,
    0x00000000000040E0, 0x0000000000002070, 0x0000000000001038,
    0x000000000000081C, 0x000000000000040E, 0x0000000000000207,
    0x0000000000000103};

static board_t HOLE_CROSSES_RAW[N_HOLES] = {
    0x4080000000000000, 0xa040000000000000, 0x5020000000000000,
    0x2810000000000000, 0x1408000000000000, 0x0a04000000000000,
    0x0502000000000000, 0x0201000000000000, 0x8040800000000000,
    0x40a0400000000000, 0x2050200000000000, 0x1028100000000000,
    0x0814080000000000, 0x040a040000000000, 0x0205020000000000,
    0x0102010000000000, 0x0080408000000000, 0x0040a04000000000,
    0x0020502000000000, 0x0010281000000000, 0x0008140800000000,
    0x00040a0400000000, 0x0002050200000000, 0x0001020100000000,
    0x0000804080000000, 0x000040a040000000, 0x0000205020000000,
    0x0000102810000000, 0x0000081408000000, 0x0000040a04000000,
    0x0000020502000000, 0x0000010201000000, 0x0000008040800000,
    0x00000040a0400000, 0x0000002050200000, 0x0000001028100000,
    0x0000000814080000, 0x000000040a040000, 0x0000000205020000,
    0x0000000102010000, 0x0000000080408000, 0x0000000040a04000,
    0x0000000020502000, 0x0000000010281000, 0x0000000008140800,
    0x00000000040a0400, 0x0000000002050200, 0x0000000001020100,
    0x0000000000804080, 0x000000000040a040, 0x0000000000205020,
    0x0000000000102810, 0x0000000000081408, 0x0000000000040a04,
    0x0000000000020502, 0x0000000000010201, 0x0000000000008040,
    0x00000000000040a0, 0x0000000000002050, 0x0000000000001028,
    0x0000000000000814, 0x000000000000040a, 0x0000000000000205,
    0x0000000000000102};

#endif

void init_hole_checker(board_t problem, struct hole_checker *hc) {

  hc->num_double_holes = 0;
  hc->double_hole_masks = aligned_alloc(32, N_DOUBLE_HOLES * sizeof(board_t));
  hc->double_hole_crosses = aligned_alloc(32, N_DOUBLE_HOLES * sizeof(board_t));

  // Only Pick those holes that are relevant
  for (int i = 0; i < N_DOUBLE_HOLES; i++) {
    // printf("%d\n", i);

    board_t affected = (DOUBLE_HOLE_CROSSES_RAW[i] ^ DOUBLE_HOLE_MASKS_RAW[i]);

    if ((affected & problem) == 0) {
      // print_raw(affected);
      hc->double_hole_masks[hc->num_double_holes] = DOUBLE_HOLE_MASKS_RAW[i];
      hc->double_hole_crosses[hc->num_double_holes] =
          DOUBLE_HOLE_CROSSES_RAW[i];
      hc->num_double_holes++;
    }
  }

  hc->num_double_blocks = (hc->num_double_holes + 3) / 4;

  // Clear the remainder in order to avoid issues in SIMD
  for (int i = hc->num_double_holes; i < N_DOUBLE_HOLES; i++) {
    hc->double_hole_masks[i] = 0x0000000000000000;
    hc->double_hole_crosses[i] = 0xFFFFFFFFFFFFFFFF;
  }

  // printf("Checking: %d/%d holes\n", hc->num_holes, N_HOLES);
}

void free_hole_checker(struct hole_checker *hc) {
  free(hc->double_hole_crosses);
  free(hc->double_hole_masks);
}

bool check_holes_simd_old_double(board_t board, const struct hole_checker *hc) {

  __m256i vec_problem = _mm256_set1_epi64x(board);

  board_t *p_hole_masks = hc->double_hole_masks;
  board_t *p_hole_crosses = hc->double_hole_crosses;
  size_t nBlocks = hc->num_double_blocks;

  do {
    __m256i vec_masks = _mm256_stream_load_si256((__m256i *)p_hole_masks);
    __m256i vec_crosses = _mm256_stream_load_si256((__m256i *)p_hole_crosses);

    __m256i vec_sel = _mm256_and_si256(vec_problem, vec_masks);

    __m256i vec_test = _mm256_cmpeq_epi64(vec_sel, vec_crosses);

    if (!_mm256_testz_si256(vec_test, vec_test)) {
      return false;
    }
    p_hole_masks += 4;
    p_hole_crosses += 4;
  } while (--nBlocks);

  return true;
}

static const uint32_t DOUBLE_HOLE_MASKS_VERTICAL[]
    __attribute__((aligned(CACHE_LINE_SIZE))) = {
        0xC0C08000, 0xE0E04000, 0x70702000, 0x38381000, 0x1C1C0800, 0x0E0E0400,
        0x07070200, 0x03030100, 0x80C0C080, 0x40E0E040, 0x20707020, 0x10383810,
        0x081C1C08, 0x040E0E04, 0x02070702, 0x01030301, 0x0080C0C0, 0x0040E0E0,
        0x00207070, 0x00103838, 0x00081C1C, 0x00040E0E, 0x00020707, 0x00010303};

static const uint32_t DOUBLE_HOLE_CROSSES_VERTICAL[]
    __attribute__((aligned(CACHE_LINE_SIZE))) = {
        0x40408000, 0xa0a04000, 0x50502000, 0x28281000, 0x14140800, 0x0a0a0400,
        0x05050200, 0x02020100, 0x80404080, 0x40a0a040, 0x20505020, 0x10282810,
        0x08141408, 0x040a0a04, 0x02050502, 0x01020201, 0x00804040, 0x0040a0a0,
        0x00205050, 0x00102828, 0x00081414, 0x00040a0a, 0x00020505, 0x00010202};

bool check_holes_simd_double(board_t board) {

#if defined(SIMD_AVX2) || defined(SIMD_AVX512)

  uint32_t upper_board = (uint32_t)(board >> 32);

  uint32_t middle1_board = (uint32_t)(board >> 24);
  uint32_t middle2_board = (uint32_t)(board >> 16);
  uint32_t middle3_board = (uint32_t)(board >> 8);

  uint32_t lower_board = (uint32_t)(board);

  __m256i vec_upper_problem = _mm256_set1_epi32(upper_board);
  __m256i vec_lower_problem = _mm256_set1_epi32(lower_board);
  __m256i vec_middle1_problem = _mm256_set1_epi32(middle1_board);
  __m256i vec_middle2_problem = _mm256_set1_epi32(middle2_board);
  __m256i vec_middle3_problem = _mm256_set1_epi32(middle3_board);

  const uint32_t *p_masks = DOUBLE_HOLE_MASKS_VERTICAL;
  const uint32_t *p_cross = DOUBLE_HOLE_CROSSES_VERTICAL;

  // Load Masks
  __m256i vec_upper_masks = _mm256_load_si256((__m256i *)p_masks);
  __m256i vec_upper_cross = _mm256_load_si256((__m256i *)p_cross);

  __m256i vec_middle_masks = _mm256_load_si256((__m256i *)(p_masks + 8));
  __m256i vec_middle_cross = _mm256_load_si256((__m256i *)(p_cross + 8));

  // Load Lower Masks
  __m256i vec_lower_masks = _mm256_load_si256((__m256i *)(p_masks + 16));
  __m256i vec_lower_cross = _mm256_load_si256((__m256i *)(p_cross + 16));

  // Select Rows
  __m256i vec_sel_row0 = _mm256_and_si256(vec_upper_problem, vec_upper_masks);
  __m256i vec_sel_row1 = _mm256_and_si256(vec_upper_problem, vec_middle_masks);

  __m256i vec_sel_row2 =
      _mm256_and_si256(vec_middle1_problem, vec_middle_masks);
  __m256i vec_sel_row3 =
      _mm256_and_si256(vec_middle2_problem, vec_middle_masks);
  __m256i vec_sel_row4 =
      _mm256_and_si256(vec_middle3_problem, vec_middle_masks);

  __m256i vec_sel_row5 = _mm256_and_si256(vec_lower_problem, vec_middle_masks);
  __m256i vec_sel_row6 = _mm256_and_si256(vec_lower_problem, vec_lower_masks);

  // Test Rows
  __m256i vec_test_row0 = _mm256_cmpeq_epi32(vec_sel_row0, vec_upper_cross);

  __m256i vec_test_row1 = _mm256_cmpeq_epi32(vec_sel_row1, vec_middle_cross);

  __m256i vec_test_row2 = _mm256_cmpeq_epi32(vec_sel_row2, vec_middle_cross);
  __m256i vec_test_row3 = _mm256_cmpeq_epi32(vec_sel_row3, vec_middle_cross);
  __m256i vec_test_row4 = _mm256_cmpeq_epi32(vec_sel_row4, vec_middle_cross);

  __m256i vec_test_row5 = _mm256_cmpeq_epi32(vec_sel_row5, vec_middle_cross);
  __m256i vec_test_row6 = _mm256_cmpeq_epi32(vec_sel_row6, vec_lower_cross);

  if (!_mm256_testz_si256(vec_test_row0, vec_test_row0) ||
      !_mm256_testz_si256(vec_test_row1, vec_test_row1) ||
      !_mm256_testz_si256(vec_test_row2, vec_test_row2) ||
      !_mm256_testz_si256(vec_test_row3, vec_test_row3) ||
      !_mm256_testz_si256(vec_test_row4, vec_test_row4) ||
      !_mm256_testz_si256(vec_test_row5, vec_test_row5) ||
      !_mm256_testz_si256(vec_test_row6, vec_test_row6)) {
    return false;
  } else {
    return true;
  }

#else
#error "Cant use Check holes without SIMD"
#endif
}

static const uint32_t SINGLE_HOLE_MASKS[32]
    __attribute__((aligned(CACHE_LINE_SIZE))) = {
        0xC0800000, 0xE0400000, 0x70200000, 0x38100000, 0x1C080000, 0x0E040000,
        0x07020000, 0x03010000, 0x80C08000, 0x40E04000, 0x20702000, 0x10381000,
        0x081C0800, 0x040E0400, 0x02070200, 0x01030100, 0x0080C080, 0x0040E040,
        0x00207020, 0x00103810, 0x00081C08, 0x00040E04, 0x00020702, 0x00010301,
        0x000080C0, 0x000040E0, 0x00002070, 0x00001038, 0x0000081C, 0x0000040E,
        0x00000207, 0x00000103};

static const uint32_t SINGLE_HOLE_CROSSES[32]
    __attribute__((aligned(CACHE_LINE_SIZE))) = {
        0x40800000, 0xa0400000, 0x50200000, 0x28100000, 0x14080000, 0x0a040000,
        0x05020000, 0x02010000, 0x80408000, 0x40a04000, 0x20502000, 0x10281000,
        0x08140800, 0x040a0400, 0x02050200, 0x01020100, 0x00804080, 0x0040a040,
        0x00205020, 0x00102810, 0x00081408, 0x00040a04, 0x00020502, 0x00010201,
        0x00008040, 0x000040a0, 0x00002050, 0x00001028, 0x00000814, 0x0000040a,
        0x00000205, 0x00000102};

bool check_holes_single(board_t board) {

#if defined(SIMD_AVX2) || defined(SIMD_AVX512)
  uint32_t upper_board = (uint32_t)(board >> 32);
  uint32_t middle_board = (uint32_t)(board >> 16);
  uint32_t lower_board = (uint32_t)(board);

  __m256i vec_upper_problem = _mm256_set1_epi32(upper_board);
  __m256i vec_lower_problem = _mm256_set1_epi32(lower_board);
  __m256i vec_middle_problem = _mm256_set1_epi32(middle_board);

  const uint32_t *p_masks = SINGLE_HOLE_MASKS;
  const uint32_t *p_crosses = SINGLE_HOLE_CROSSES;

  // Load Upper Masks
  __m256i vec_upper_masks = _mm256_load_si256((__m256i *)p_masks);
  __m256i vec_upper_crosses = _mm256_load_si256((__m256i *)p_crosses);

  // Skip first row for Lower Masks
  p_masks += 8;
  p_crosses += 8;

  // Load Lower Masks
  __m256i vec_lower_masks = _mm256_load_si256((__m256i *)p_masks);
  __m256i vec_lower_crosses = _mm256_load_si256((__m256i *)p_crosses);

  size_t row = 0;
  do {

    __m256i vec_upper_sel =
        _mm256_and_si256(vec_upper_problem, vec_upper_masks);
    __m256i vec_lower_sel =
        _mm256_and_si256(vec_lower_problem, vec_lower_masks);

    __m256i vec_middle_sel = _mm256_and_si256(
        vec_middle_problem,
        vec_lower_masks); // User Lower masks for middle problem (only 2)

    __m256i vec_upper_test =
        _mm256_cmpeq_epi32(vec_upper_sel, vec_upper_crosses);
    __m256i vec_lower_test =
        _mm256_cmpeq_epi32(vec_lower_sel, vec_lower_crosses);
    __m256i vec_middle_test =
        _mm256_cmpeq_epi32(vec_middle_sel, vec_lower_crosses);

    if (!_mm256_testz_si256(vec_upper_test, vec_upper_test) ||
        !_mm256_testz_si256(vec_lower_test, vec_lower_test) ||
        (row < 2 && !_mm256_testz_si256(vec_middle_test, vec_middle_test))) {
      return false;
    }

    // Break after 3 rows
    if ((++row) >= 3) {
      return true;
    }

    p_masks += 8;
    p_crosses += 8;

    // Reuse the lower masks as the old upper masks
    vec_upper_masks = vec_lower_masks;
    vec_upper_crosses = vec_lower_crosses;

    // Load new lower masks
    vec_lower_masks = _mm256_load_si256((__m256i *)p_masks);
    vec_lower_crosses = _mm256_load_si256((__m256i *)p_crosses);
  } while (1);
#else
#error "Cant use Check holes without SIMD"
#endif
}

bool check_holes(board_t board) {

  if (!check_holes_single(board)) {
    return false;
  }

  if (!check_holes_simd_double(board)) {
    return false;
  }

  //  Rotate bord to test horizontal double piece
  board = piece_rotate(board);

  if (!check_holes_simd_double(board)) {
    return false;
  }

  return true;
}

void print_hole_checker(struct hole_checker *hc) {
  for (int i = 0; i < hc->num_double_holes; i++) {
    printf("%d\n", i);
    print_raw_color(hc->double_hole_masks[i], i % 10);
    print_raw_color(hc->double_hole_crosses[i], i % 10);
  }
}

#else
bool check_holes(board_t board) { return true; }
bool check_holes_simd(board_t board) { return true; }
#endif