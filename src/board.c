#include "board.h"
#include <immintrin.h>
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

#ifdef CHECK_HOLES

#ifdef CHECK_DOUBLE_HOLES
#define N_HOLES 176

static const board_t HOLE_MASKS[N_HOLES] __attribute__((aligned(32))) = {
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
    0x0000000000000103, 0xC0C0800000000000, 0xE0E0400000000000,
    0x7070200000000000, 0x3838100000000000, 0x1C1C080000000000,
    0x0E0E040000000000, 0x0707020000000000, 0x0303010000000000,
    0x80C0C08000000000, 0x40E0E04000000000, 0x2070702000000000,
    0x1038381000000000, 0x081C1C0800000000, 0x040E0E0400000000,
    0x0207070200000000, 0x0103030100000000, 0x0080C0C080000000,
    0x0040E0E040000000, 0x0020707020000000, 0x0010383810000000,
    0x00081C1C08000000, 0x00040E0E04000000, 0x0002070702000000,
    0x0001030301000000, 0x000080C0C0800000, 0x000040E0E0400000,
    0x0000207070200000, 0x0000103838100000, 0x0000081C1C080000,
    0x0000040E0E040000, 0x0000020707020000, 0x0000010303010000,
    0x00000080C0C08000, 0x00000040E0E04000, 0x0000002070702000,
    0x0000001038381000, 0x000000081C1C0800, 0x000000040E0E0400,
    0x0000000207070200, 0x0000000103030100, 0x0000000080C0C080,
    0x0000000040E0E040, 0x0000000020707020, 0x0000000010383810,
    0x00000000081C1C08, 0x00000000040E0E04, 0x0000000002070702,
    0x0000000001030301, 0x000000000080C0C0, 0x000000000040E0E0,
    0x0000000000207070, 0x0000000000103838, 0x0000000000081C1C,
    0x0000000000040E0E, 0x0000000000020707, 0x0000000000010303,
    0xE0C0000000000000, 0xF060000000000000, 0x7830000000000000,
    0x3C18000000000000, 0x1E0C000000000000, 0x0F06000000000000,
    0x0703000000000000, 0xC0E0C00000000000, 0x60F0600000000000,
    0x3078300000000000, 0x183C180000000000, 0x0C1E0C0000000000,
    0x060F060000000000, 0x0307030000000000, 0x00C0E0C000000000,
    0x0060F06000000000, 0x0030783000000000, 0x00183C1800000000,
    0x000C1E0C00000000, 0x00060F0600000000, 0x0003070300000000,
    0x0000C0E0C0000000, 0x000060F060000000, 0x0000307830000000,
    0x0000183C18000000, 0x00000C1E0C000000, 0x0000060F06000000,
    0x0000030703000000, 0x000000C0E0C00000, 0x00000060F0600000,
    0x0000003078300000, 0x000000183C180000, 0x0000000C1E0C0000,
    0x000000060F060000, 0x0000000307030000, 0x00000000C0E0C000,
    0x0000000060F06000, 0x0000000030783000, 0x00000000183C1800,
    0x000000000C1E0C00, 0x00000000060F0600, 0x0000000003070300,
    0x0000000000C0E0C0, 0x000000000060F060, 0x0000000000307830,
    0x0000000000183C18, 0x00000000000C1E0C, 0x0000000000060F06,
    0x0000000000030703, 0x000000000000C0E0, 0x00000000000060F0,
    0x0000000000003078, 0x000000000000183C, 0x0000000000000C1E,
    0x000000000000060F, 0x0000000000000307};

static const board_t HOLE_CROSSES[N_HOLES] __attribute__((aligned(32))) = {
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
    0x0000000000000102, 0x4040800000000000, 0xa0a0400000000000,
    0x5050200000000000, 0x2828100000000000, 0x1414080000000000,
    0x0a0a040000000000, 0x0505020000000000, 0x0202010000000000,
    0x8040408000000000, 0x40a0a04000000000, 0x2050502000000000,
    0x1028281000000000, 0x0814140800000000, 0x040a0a0400000000,
    0x0205050200000000, 0x0102020100000000, 0x0080404080000000,
    0x0040a0a040000000, 0x0020505020000000, 0x0010282810000000,
    0x0008141408000000, 0x00040a0a04000000, 0x0002050502000000,
    0x0001020201000000, 0x0000804040800000, 0x000040a0a0400000,
    0x0000205050200000, 0x0000102828100000, 0x0000081414080000,
    0x0000040a0a040000, 0x0000020505020000, 0x0000010202010000,
    0x0000008040408000, 0x00000040a0a04000, 0x0000002050502000,
    0x0000001028281000, 0x0000000814140800, 0x000000040a0a0400,
    0x0000000205050200, 0x0000000102020100, 0x0000000080404080,
    0x0000000040a0a040, 0x0000000020505020, 0x0000000010282810,
    0x0000000008141408, 0x00000000040a0a04, 0x0000000002050502,
    0x0000000001020201, 0x0000000000804040, 0x000000000040a0a0,
    0x0000000000205050, 0x0000000000102828, 0x0000000000081414,
    0x0000000000040a0a, 0x0000000000020505, 0x0000000000010202,
    0x20c0000000000000, 0x9060000000000000, 0x4830000000000000,
    0x2418000000000000, 0x120c000000000000, 0x0906000000000000,
    0x0403000000000000, 0xc020c00000000000, 0x6090600000000000,
    0x3048300000000000, 0x1824180000000000, 0x0c120c0000000000,
    0x0609060000000000, 0x0304030000000000, 0x00c020c000000000,
    0x0060906000000000, 0x0030483000000000, 0x0018241800000000,
    0x000c120c00000000, 0x0006090600000000, 0x0003040300000000,
    0x0000c020c0000000, 0x0000609060000000, 0x0000304830000000,
    0x0000182418000000, 0x00000c120c000000, 0x0000060906000000,
    0x0000030403000000, 0x000000c020c00000, 0x0000006090600000,
    0x0000003048300000, 0x0000001824180000, 0x0000000c120c0000,
    0x0000000609060000, 0x0000000304030000, 0x00000000c020c000,
    0x0000000060906000, 0x0000000030483000, 0x0000000018241800,
    0x000000000c120c00, 0x0000000006090600, 0x0000000003040300,
    0x0000000000c020c0, 0x0000000000609060, 0x0000000000304830,
    0x0000000000182418, 0x00000000000c120c, 0x0000000000060906,
    0x0000000000030403, 0x000000000000c020, 0x0000000000006090,
    0x0000000000003048, 0x0000000000001824, 0x0000000000000c12,
    0x0000000000000609, 0x0000000000000304};

#else
#define N_HOLES 64
static board_t HOLE_MASKS[N_HOLES] __attribute__((aligned(32))) = {
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

static board_t HOLE_CROSSES[N_HOLES] __attribute__((aligned(32))) = {
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

bool check_holes(board_t board) {
  for (int i = 0; i < N_HOLES; i++) {
    board_t sel = board & HOLE_MASKS[i];
    if (sel == HOLE_CROSSES[i]) {
      return false;
    }
  }
  return true;
}

bool check_holes_simd(board_t board) {

  __m256i vec_problem = _mm256_set1_epi64x(board);

  board_t *p_hole_masks = HOLE_MASKS;
  board_t *p_hole_crosses = HOLE_CROSSES;
  size_t nBlocks = N_HOLES / 4;

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
  } while (nBlocks--);

  return true;
}

#else
bool check_holes(board_t board) { return true; }
bool check_holes_simd(board_t board) { return true; }
#endif