/*
** 2012-05-25
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
*/

/*
** DO NOT EDIT THIS MACHINE GENERATED FILE.
*/

#include "unicode2.h"
#include <assert.h>

/*
** If the argument is a codepoint corresponding to a lowercase letter
** in the ASCII range with a diacritic added, return the codepoint
** of the ASCII letter only. For example, if passed 235 - "LATIN
** SMALL LETTER E WITH DIAERESIS" - return 65 ("LATIN SMALL LETTER
** E"). The resuls of passing a codepoint that corresponds to an
** uppercase letter are undefined.
*/
static int so_fts5_remove_diacritic(int c, int bComplex) {
  unsigned short aDia[] = {
      0,     1797,  1848,  1859,  1891,  1928,  1940,  1995,  2024,  2040,
      2060,  2110,  2168,  2206,  2264,  2286,  2344,  2383,  2472,  2488,
      2516,  2596,  2668,  2732,  2782,  2842,  2894,  2954,  2984,  3000,
      3028,  3336,  3456,  3696,  3712,  3728,  3744,  3766,  3832,  3896,
      3912,  3928,  3944,  3968,  4008,  4040,  4056,  4106,  4138,  4170,
      4202,  4234,  4266,  4296,  4312,  4344,  4408,  4424,  4442,  4472,
      4488,  4504,  6148,  6198,  6264,  6280,  6360,  6429,  6505,  6529,
      61448, 61468, 61512, 61534, 61592, 61610, 61642, 61672, 61688, 61704,
      61726, 61784, 61800, 61816, 61836, 61880, 61896, 61914, 61948, 61998,
      62062, 62122, 62154, 62184, 62200, 62218, 62252, 62302, 62364, 62410,
      62442, 62478, 62536, 62554, 62584, 62604, 62640, 62648, 62656, 62664,
      62730, 62766, 62830, 62890, 62924, 62974, 63032, 63050, 63082, 63118,
      63182, 63242, 63274, 63310, 63368, 63390,
  };
#define HIBIT ((unsigned char)0x80)
  unsigned char aChar[] = {
      '\0',        'a',         'c',         'e',         'i',
      'n',         'o',         'u',         'y',         'y',
      'a',         'c',         'd',         'e',         'e',
      'g',         'h',         'i',         'j',         'k',
      'l',         'n',         'o',         'r',         's',
      't',         'u',         'u',         'w',         'y',
      'z',         'o',         'u',         'a',         'i',
      'o',         'u',         'u' | HIBIT, 'a' | HIBIT, 'g',
      'k',         'o',         'o' | HIBIT, 'j',         'g',
      'n',         'a' | HIBIT, 'a',         'e',         'i',
      'o',         'r',         'u',         's',         't',
      'h',         'a',         'e',         'o' | HIBIT, 'o',
      'o' | HIBIT, 'y',         '\0',        '\0',        '\0',
      '\0',        '\0',        '\0',        '\0',        '\0',
      'a',         'b',         'c' | HIBIT, 'd',         'd',
      'e' | HIBIT, 'e',         'e' | HIBIT, 'f',         'g',
      'h',         'h',         'i',         'i' | HIBIT, 'k',
      'l',         'l' | HIBIT, 'l',         'm',         'n',
      'o' | HIBIT, 'p',         'r',         'r' | HIBIT, 'r',
      's',         's' | HIBIT, 't',         'u',         'u' | HIBIT,
      'v',         'w',         'w',         'x',         'y',
      'z',         'h',         't',         'w',         'y',
      'a',         'a' | HIBIT, 'a' | HIBIT, 'a' | HIBIT, 'e',
      'e' | HIBIT, 'e' | HIBIT, 'i',         'o',         'o' | HIBIT,
      'o' | HIBIT, 'o' | HIBIT, 'u',         'u' | HIBIT, 'u' | HIBIT,
      'y',
  };

  unsigned int key = (((unsigned int)c) << 3) | 0x00000007;
  int iRes = 0;
  int iHi = sizeof(aDia) / sizeof(aDia[0]) - 1;
  int iLo = 0;
  while (iHi >= iLo) {
    int iTest = (iHi + iLo) / 2;
    if (key >= aDia[iTest]) {
      iRes = iTest;
      iLo = iTest + 1;
    } else {
      iHi = iTest - 1;
    }
  }
  assert(key >= aDia[iRes]);
  if (bComplex == 0 && (aChar[iRes] & 0x80))
    return c;
  return (c > (aDia[iRes] >> 3) + (aDia[iRes] & 0x07))
             ? c
             : ((int)aChar[iRes] & 0x7F);
}

/*
** Return true if the argument interpreted as a unicode codepoint
** is a diacritical modifier character.
*/
int so_sqlite3Fts5UnicodeIsdiacritic(int c) {
  unsigned int mask0 = 0x08029FDF;
  unsigned int mask1 = 0x000361F8;
  if (c < 768 || c > 817)
    return 0;
  return (c < 768 + 32) ? (mask0 & ((unsigned int)1 << (c - 768)))
                        : (mask1 & ((unsigned int)1 << (c - 768 - 32)));
}

/*
** Interpret the argument as a unicode codepoint. If the codepoint
** is an upper case character that has a lower case equivalent,
** return the codepoint corresponding to the lower case version.
** Otherwise, return a copy of the argument.
**
** The results are undefined if the value passed to this function
** is less than zero.
*/
int so_sqlite3Fts5UnicodeFold(int c, int eRemoveDiacritic) {
  /* Each entry in the following array defines a rule for folding a range
  ** of codepoints to lower case. The rule applies to a range of nRange
  ** codepoints starting at codepoint iCode.
  **
  ** If the least significant bit in flags is clear, then the rule applies
  ** to all nRange codepoints (i.e. all nRange codepoints are upper case and
  ** need to be folded). Or, if it is set, then the rule only applies to
  ** every second codepoint in the range, starting with codepoint C.
  **
  ** The 7 most significant bits in flags are an index into the aiOff[]
  ** array. If a specific codepoint C does require folding, then its lower
  ** case equivalent is ((C + aiOff[flags>>1]) & 0xFFFF).
  **
  ** The contents of this array are generated by parsing the CaseFolding.txt
  ** file distributed as part of the "Unicode Character Database". See
  ** http://www.unicode.org for details.
  */
  static const struct TableEntry {
    unsigned short iCode;
    unsigned char flags;
    unsigned char nRange;
  } aEntry[] = {
      {65, 14, 26},   {181, 64, 1},    {192, 14, 23},   {216, 14, 7},
      {256, 1, 48},   {306, 1, 6},     {313, 1, 16},    {330, 1, 46},
      {376, 116, 1},  {377, 1, 6},     {383, 104, 1},   {385, 50, 1},
      {386, 1, 4},    {390, 44, 1},    {391, 0, 1},     {393, 42, 2},
      {395, 0, 1},    {398, 32, 1},    {399, 38, 1},    {400, 40, 1},
      {401, 0, 1},    {403, 42, 1},    {404, 46, 1},    {406, 52, 1},
      {407, 48, 1},   {408, 0, 1},     {412, 52, 1},    {413, 54, 1},
      {415, 56, 1},   {416, 1, 6},     {422, 60, 1},    {423, 0, 1},
      {425, 60, 1},   {428, 0, 1},     {430, 60, 1},    {431, 0, 1},
      {433, 58, 2},   {435, 1, 4},     {439, 62, 1},    {440, 0, 1},
      {444, 0, 1},    {452, 2, 1},     {453, 0, 1},     {455, 2, 1},
      {456, 0, 1},    {458, 2, 1},     {459, 1, 18},    {478, 1, 18},
      {497, 2, 1},    {498, 1, 4},     {502, 122, 1},   {503, 134, 1},
      {504, 1, 40},   {544, 110, 1},   {546, 1, 18},    {570, 70, 1},
      {571, 0, 1},    {573, 108, 1},   {574, 68, 1},    {577, 0, 1},
      {579, 106, 1},  {580, 28, 1},    {581, 30, 1},    {582, 1, 10},
      {837, 36, 1},   {880, 1, 4},     {886, 0, 1},     {902, 18, 1},
      {904, 16, 3},   {908, 26, 1},    {910, 24, 2},    {913, 14, 17},
      {931, 14, 9},   {962, 0, 1},     {975, 4, 1},     {976, 140, 1},
      {977, 142, 1},  {981, 146, 1},   {982, 144, 1},   {984, 1, 24},
      {1008, 136, 1}, {1009, 138, 1},  {1012, 130, 1},  {1013, 128, 1},
      {1015, 0, 1},   {1017, 152, 1},  {1018, 0, 1},    {1021, 110, 3},
      {1024, 34, 16}, {1040, 14, 32},  {1120, 1, 34},   {1162, 1, 54},
      {1216, 6, 1},   {1217, 1, 14},   {1232, 1, 88},   {1329, 22, 38},
      {4256, 66, 38}, {4295, 66, 1},   {4301, 66, 1},   {7680, 1, 150},
      {7835, 132, 1}, {7838, 96, 1},   {7840, 1, 96},   {7944, 150, 8},
      {7960, 150, 6}, {7976, 150, 8},  {7992, 150, 8},  {8008, 150, 6},
      {8025, 151, 8}, {8040, 150, 8},  {8072, 150, 8},  {8088, 150, 8},
      {8104, 150, 8}, {8120, 150, 2},  {8122, 126, 2},  {8124, 148, 1},
      {8126, 100, 1}, {8136, 124, 4},  {8140, 148, 1},  {8152, 150, 2},
      {8154, 120, 2}, {8168, 150, 2},  {8170, 118, 2},  {8172, 152, 1},
      {8184, 112, 2}, {8186, 114, 2},  {8188, 148, 1},  {8486, 98, 1},
      {8490, 92, 1},  {8491, 94, 1},   {8498, 12, 1},   {8544, 8, 16},
      {8579, 0, 1},   {9398, 10, 26},  {11264, 22, 47}, {11360, 0, 1},
      {11362, 88, 1}, {11363, 102, 1}, {11364, 90, 1},  {11367, 1, 6},
      {11373, 84, 1}, {11374, 86, 1},  {11375, 80, 1},  {11376, 82, 1},
      {11378, 0, 1},  {11381, 0, 1},   {11390, 78, 2},  {11392, 1, 100},
      {11499, 1, 4},  {11506, 0, 1},   {42560, 1, 46},  {42624, 1, 24},
      {42786, 1, 14}, {42802, 1, 62},  {42873, 1, 4},   {42877, 76, 1},
      {42878, 1, 10}, {42891, 0, 1},   {42893, 74, 1},  {42896, 1, 4},
      {42912, 1, 10}, {42922, 72, 1},  {65313, 14, 26},
  };
  static const unsigned short aiOff[] = {
      1,     2,     8,     15,    16,    26,    28,    32,    37,    38,
      40,    48,    63,    64,    69,    71,    79,    80,    116,   202,
      203,   205,   206,   207,   209,   210,   211,   213,   214,   217,
      218,   219,   775,   7264,  10792, 10795, 23228, 23256, 30204, 54721,
      54753, 54754, 54756, 54787, 54793, 54809, 57153, 57274, 57921, 58019,
      58363, 61722, 65268, 65341, 65373, 65406, 65408, 65410, 65415, 65424,
      65436, 65439, 65450, 65462, 65472, 65476, 65478, 65480, 65482, 65488,
      65506, 65511, 65514, 65521, 65527, 65528, 65529,
  };

  int ret = c;

  assert(sizeof(unsigned short) == 2 && sizeof(unsigned char) == 1);

  if (c < 128) {
    if (c >= 'A' && c <= 'Z')
      ret = c + ('a' - 'A');
  } else if (c < 65536) {
    const struct TableEntry *p;
    int iHi = sizeof(aEntry) / sizeof(aEntry[0]) - 1;
    int iLo = 0;
    int iRes = -1;

    assert(c > aEntry[0].iCode);
    while (iHi >= iLo) {
      int iTest = (iHi + iLo) / 2;
      int cmp = (c - aEntry[iTest].iCode);
      if (cmp >= 0) {
        iRes = iTest;
        iLo = iTest + 1;
      } else {
        iHi = iTest - 1;
      }
    }

    assert(iRes >= 0 && c >= aEntry[iRes].iCode);
    p = &aEntry[iRes];
    if (c < (p->iCode + p->nRange) && 0 == (0x01 & p->flags & (p->iCode ^ c))) {
      ret = (c + (aiOff[p->flags >> 1])) & 0x0000FFFF;
      assert(ret > 0);
    }

    if (eRemoveDiacritic) {
      ret = so_fts5_remove_diacritic(ret, eRemoveDiacritic == 2);
    }
  }

  else if (c >= 66560 && c < 66600) {
    ret = c + 40;
  }

  return ret;
}

int so_sqlite3Fts5UnicodeCatParse(const char *zCat, u8 *aArray) {
  aArray[0] = 1;
  switch (zCat[0]) {
  case 'C':
    switch (zCat[1]) {
    case 'c':
      aArray[1] = 1;
      break;
    case 'f':
      aArray[2] = 1;
      break;
    case 'n':
      aArray[3] = 1;
      break;
    case 's':
      aArray[4] = 1;
      break;
    case 'o':
      aArray[31] = 1;
      break;
    case '*':
      aArray[1] = 1;
      aArray[2] = 1;
      aArray[3] = 1;
      aArray[4] = 1;
      aArray[31] = 1;
      break;
    default:
      return 1;
    }
    break;

  case 'L':
    switch (zCat[1]) {
    case 'l':
      aArray[5] = 1;
      break;
    case 'm':
      aArray[6] = 1;
      break;
    case 'o':
      aArray[7] = 1;
      break;
    case 't':
      aArray[8] = 1;
      break;
    case 'u':
      aArray[9] = 1;
      break;
    case 'C':
      aArray[30] = 1;
      break;
    case '*':
      aArray[5] = 1;
      aArray[6] = 1;
      aArray[7] = 1;
      aArray[8] = 1;
      aArray[9] = 1;
      aArray[30] = 1;
      break;
    default:
      return 1;
    }
    break;

  case 'M':
    switch (zCat[1]) {
    case 'c':
      aArray[10] = 1;
      break;
    case 'e':
      aArray[11] = 1;
      break;
    case 'n':
      aArray[12] = 1;
      break;
    case '*':
      aArray[10] = 1;
      aArray[11] = 1;
      aArray[12] = 1;
      break;
    default:
      return 1;
    }
    break;

  case 'N':
    switch (zCat[1]) {
    case 'd':
      aArray[13] = 1;
      break;
    case 'l':
      aArray[14] = 1;
      break;
    case 'o':
      aArray[15] = 1;
      break;
    case '*':
      aArray[13] = 1;
      aArray[14] = 1;
      aArray[15] = 1;
      break;
    default:
      return 1;
    }
    break;

  case 'P':
    switch (zCat[1]) {
    case 'c':
      aArray[16] = 1;
      break;
    case 'd':
      aArray[17] = 1;
      break;
    case 'e':
      aArray[18] = 1;
      break;
    case 'f':
      aArray[19] = 1;
      break;
    case 'i':
      aArray[20] = 1;
      break;
    case 'o':
      aArray[21] = 1;
      break;
    case 's':
      aArray[22] = 1;
      break;
    case '*':
      aArray[16] = 1;
      aArray[17] = 1;
      aArray[18] = 1;
      aArray[19] = 1;
      aArray[20] = 1;
      aArray[21] = 1;
      aArray[22] = 1;
      break;
    default:
      return 1;
    }
    break;

  case 'S':
    switch (zCat[1]) {
    case 'c':
      aArray[23] = 1;
      break;
    case 'k':
      aArray[24] = 1;
      break;
    case 'm':
      aArray[25] = 1;
      break;
    case 'o':
      aArray[26] = 1;
      break;
    case '*':
      aArray[23] = 1;
      aArray[24] = 1;
      aArray[25] = 1;
      aArray[26] = 1;
      break;
    default:
      return 1;
    }
    break;

  case 'Z':
    switch (zCat[1]) {
    case 'l':
      aArray[27] = 1;
      break;
    case 'p':
      aArray[28] = 1;
      break;
    case 's':
      aArray[29] = 1;
      break;
    case '*':
      aArray[27] = 1;
      aArray[28] = 1;
      aArray[29] = 1;
      break;
    default:
      return 1;
    }
    break;
  }
  return 0;
}

static u16 so_aFts5UnicodeBlock[] = {
    0,    1471, 1753, 1760, 1760, 1760, 1760, 1760, 1760,
    1760, 1760, 1760, 1760, 1760, 1760, 1763, 1765,
};
static u16 so_aFts5UnicodeMap[] = {
    0,     32,    33,    36,    37,    40,    41,    42,    43,    44,    45,
    46,    48,    58,    60,    63,    65,    91,    92,    93,    94,    95,
    96,    97,    123,   124,   125,   126,   127,   160,   161,   162,   166,
    167,   168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
    178,   180,   181,   182,   184,   185,   186,   187,   188,   191,   192,
    215,   216,   223,   247,   248,   256,   312,   313,   329,   330,   377,
    383,   385,   387,   388,   391,   394,   396,   398,   402,   403,   405,
    406,   409,   412,   414,   415,   417,   418,   423,   427,   428,   431,
    434,   436,   437,   440,   442,   443,   444,   446,   448,   452,   453,
    454,   455,   456,   457,   458,   459,   460,   461,   477,   478,   496,
    497,   498,   499,   500,   503,   505,   506,   564,   570,   572,   573,
    575,   577,   580,   583,   584,   592,   660,   661,   688,   706,   710,
    722,   736,   741,   748,   749,   750,   751,   768,   880,   884,   885,
    886,   890,   891,   894,   900,   902,   903,   904,   908,   910,   912,
    913,   931,   940,   975,   977,   978,   981,   984,   1008,  1012,  1014,
    1015,  1018,  1020,  1021,  1072,  1120,  1154,  1155,  1160,  1162,  1217,
    1231,  1232,  1329,  1369,  1370,  1377,  1417,  1418,  1423,  1425,  1470,
    1471,  1472,  1473,  1475,  1476,  1478,  1479,  1488,  1520,  1523,  1536,
    1542,  1545,  1547,  1548,  1550,  1552,  1563,  1566,  1568,  1600,  1601,
    1611,  1632,  1642,  1646,  1648,  1649,  1748,  1749,  1750,  1757,  1758,
    1759,  1765,  1767,  1769,  1770,  1774,  1776,  1786,  1789,  1791,  1792,
    1807,  1808,  1809,  1810,  1840,  1869,  1958,  1969,  1984,  1994,  2027,
    2036,  2038,  2039,  2042,  2048,  2070,  2074,  2075,  2084,  2085,  2088,
    2089,  2096,  2112,  2137,  2142,  2208,  2210,  2276,  2304,  2307,  2308,
    2362,  2363,  2364,  2365,  2366,  2369,  2377,  2381,  2382,  2384,  2385,
    2392,  2402,  2404,  2406,  2416,  2417,  2418,  2425,  2433,  2434,  2437,
    2447,  2451,  2474,  2482,  2486,  2492,  2493,  2494,  2497,  2503,  2507,
    2509,  2510,  2519,  2524,  2527,  2530,  2534,  2544,  2546,  2548,  2554,
    2555,  2561,  2563,  2565,  2575,  2579,  2602,  2610,  2613,  2616,  2620,
    2622,  2625,  2631,  2635,  2641,  2649,  2654,  2662,  2672,  2674,  2677,
    2689,  2691,  2693,  2703,  2707,  2730,  2738,  2741,  2748,  2749,  2750,
    2753,  2759,  2761,  2763,  2765,  2768,  2784,  2786,  2790,  2800,  2801,
    2817,  2818,  2821,  2831,  2835,  2858,  2866,  2869,  2876,  2877,  2878,
    2879,  2880,  2881,  2887,  2891,  2893,  2902,  2903,  2908,  2911,  2914,
    2918,  2928,  2929,  2930,  2946,  2947,  2949,  2958,  2962,  2969,  2972,
    2974,  2979,  2984,  2990,  3006,  3008,  3009,  3014,  3018,  3021,  3024,
    3031,  3046,  3056,  3059,  3065,  3066,  3073,  3077,  3086,  3090,  3114,
    3125,  3133,  3134,  3137,  3142,  3146,  3157,  3160,  3168,  3170,  3174,
    3192,  3199,  3202,  3205,  3214,  3218,  3242,  3253,  3260,  3261,  3262,
    3263,  3264,  3270,  3271,  3274,  3276,  3285,  3294,  3296,  3298,  3302,
    3313,  3330,  3333,  3342,  3346,  3389,  3390,  3393,  3398,  3402,  3405,
    3406,  3415,  3424,  3426,  3430,  3440,  3449,  3450,  3458,  3461,  3482,
    3507,  3517,  3520,  3530,  3535,  3538,  3542,  3544,  3570,  3572,  3585,
    3633,  3634,  3636,  3647,  3648,  3654,  3655,  3663,  3664,  3674,  3713,
    3716,  3719,  3722,  3725,  3732,  3737,  3745,  3749,  3751,  3754,  3757,
    3761,  3762,  3764,  3771,  3773,  3776,  3782,  3784,  3792,  3804,  3840,
    3841,  3844,  3859,  3860,  3861,  3864,  3866,  3872,  3882,  3892,  3893,
    3894,  3895,  3896,  3897,  3898,  3899,  3900,  3901,  3902,  3904,  3913,
    3953,  3967,  3968,  3973,  3974,  3976,  3981,  3993,  4030,  4038,  4039,
    4046,  4048,  4053,  4057,  4096,  4139,  4141,  4145,  4146,  4152,  4153,
    4155,  4157,  4159,  4160,  4170,  4176,  4182,  4184,  4186,  4190,  4193,
    4194,  4197,  4199,  4206,  4209,  4213,  4226,  4227,  4229,  4231,  4237,
    4238,  4239,  4240,  4250,  4253,  4254,  4256,  4295,  4301,  4304,  4347,
    4348,  4349,  4682,  4688,  4696,  4698,  4704,  4746,  4752,  4786,  4792,
    4800,  4802,  4808,  4824,  4882,  4888,  4957,  4960,  4969,  4992,  5008,
    5024,  5120,  5121,  5741,  5743,  5760,  5761,  5787,  5788,  5792,  5867,
    5870,  5888,  5902,  5906,  5920,  5938,  5941,  5952,  5970,  5984,  5998,
    6002,  6016,  6068,  6070,  6071,  6078,  6086,  6087,  6089,  6100,  6103,
    6104,  6107,  6108,  6109,  6112,  6128,  6144,  6150,  6151,  6155,  6158,
    6160,  6176,  6211,  6212,  6272,  6313,  6314,  6320,  6400,  6432,  6435,
    6439,  6441,  6448,  6450,  6451,  6457,  6464,  6468,  6470,  6480,  6512,
    6528,  6576,  6593,  6600,  6608,  6618,  6622,  6656,  6679,  6681,  6686,
    6688,  6741,  6742,  6743,  6744,  6752,  6753,  6754,  6755,  6757,  6765,
    6771,  6783,  6784,  6800,  6816,  6823,  6824,  6912,  6916,  6917,  6964,
    6965,  6966,  6971,  6972,  6973,  6978,  6979,  6981,  6992,  7002,  7009,
    7019,  7028,  7040,  7042,  7043,  7073,  7074,  7078,  7080,  7082,  7083,
    7084,  7086,  7088,  7098,  7142,  7143,  7144,  7146,  7149,  7150,  7151,
    7154,  7164,  7168,  7204,  7212,  7220,  7222,  7227,  7232,  7245,  7248,
    7258,  7288,  7294,  7360,  7376,  7379,  7380,  7393,  7394,  7401,  7405,
    7406,  7410,  7412,  7413,  7424,  7468,  7531,  7544,  7545,  7579,  7616,
    7676,  7680,  7830,  7838,  7936,  7944,  7952,  7960,  7968,  7976,  7984,
    7992,  8000,  8008,  8016,  8025,  8027,  8029,  8031,  8033,  8040,  8048,
    8064,  8072,  8080,  8088,  8096,  8104,  8112,  8118,  8120,  8124,  8125,
    8126,  8127,  8130,  8134,  8136,  8140,  8141,  8144,  8150,  8152,  8157,
    8160,  8168,  8173,  8178,  8182,  8184,  8188,  8189,  8192,  8203,  8208,
    8214,  8216,  8217,  8218,  8219,  8221,  8222,  8223,  8224,  8232,  8233,
    8234,  8239,  8240,  8249,  8250,  8251,  8255,  8257,  8260,  8261,  8262,
    8263,  8274,  8275,  8276,  8277,  8287,  8288,  8298,  8304,  8305,  8308,
    8314,  8317,  8318,  8319,  8320,  8330,  8333,  8334,  8336,  8352,  8400,
    8413,  8417,  8418,  8421,  8448,  8450,  8451,  8455,  8456,  8458,  8459,
    8462,  8464,  8467,  8468,  8469,  8470,  8472,  8473,  8478,  8484,  8485,
    8486,  8487,  8488,  8489,  8490,  8494,  8495,  8496,  8500,  8501,  8505,
    8506,  8508,  8510,  8512,  8517,  8519,  8522,  8523,  8524,  8526,  8527,
    8528,  8544,  8579,  8581,  8585,  8592,  8597,  8602,  8604,  8608,  8609,
    8611,  8612,  8614,  8615,  8622,  8623,  8654,  8656,  8658,  8659,  8660,
    8661,  8692,  8960,  8968,  8972,  8992,  8994,  9001,  9002,  9003,  9084,
    9085,  9115,  9140,  9180,  9186,  9216,  9280,  9312,  9372,  9450,  9472,
    9655,  9656,  9665,  9666,  9720,  9728,  9839,  9840,  9985,  10088, 10089,
    10090, 10091, 10092, 10093, 10094, 10095, 10096, 10097, 10098, 10099, 10100,
    10101, 10102, 10132, 10176, 10181, 10182, 10183, 10214, 10215, 10216, 10217,
    10218, 10219, 10220, 10221, 10222, 10223, 10224, 10240, 10496, 10627, 10628,
    10629, 10630, 10631, 10632, 10633, 10634, 10635, 10636, 10637, 10638, 10639,
    10640, 10641, 10642, 10643, 10644, 10645, 10646, 10647, 10648, 10649, 10712,
    10713, 10714, 10715, 10716, 10748, 10749, 10750, 11008, 11056, 11077, 11079,
    11088, 11264, 11312, 11360, 11363, 11365, 11367, 11374, 11377, 11378, 11380,
    11381, 11383, 11388, 11390, 11393, 11394, 11492, 11493, 11499, 11503, 11506,
    11513, 11517, 11518, 11520, 11559, 11565, 11568, 11631, 11632, 11647, 11648,
    11680, 11688, 11696, 11704, 11712, 11720, 11728, 11736, 11744, 11776, 11778,
    11779, 11780, 11781, 11782, 11785, 11786, 11787, 11788, 11789, 11790, 11799,
    11800, 11802, 11803, 11804, 11805, 11806, 11808, 11809, 11810, 11811, 11812,
    11813, 11814, 11815, 11816, 11817, 11818, 11823, 11824, 11834, 11904, 11931,
    12032, 12272, 12288, 12289, 12292, 12293, 12294, 12295, 12296, 12297, 12298,
    12299, 12300, 12301, 12302, 12303, 12304, 12305, 12306, 12308, 12309, 12310,
    12311, 12312, 12313, 12314, 12315, 12316, 12317, 12318, 12320, 12321, 12330,
    12334, 12336, 12337, 12342, 12344, 12347, 12348, 12349, 12350, 12353, 12441,
    12443, 12445, 12447, 12448, 12449, 12539, 12540, 12543, 12549, 12593, 12688,
    12690, 12694, 12704, 12736, 12784, 12800, 12832, 12842, 12872, 12880, 12881,
    12896, 12928, 12938, 12977, 12992, 13056, 13312, 19893, 19904, 19968, 40908,
    40960, 40981, 40982, 42128, 42192, 42232, 42238, 42240, 42508, 42509, 42512,
    42528, 42538, 42560, 42606, 42607, 42608, 42611, 42612, 42622, 42623, 42624,
    42655, 42656, 42726, 42736, 42738, 42752, 42775, 42784, 42786, 42800, 42802,
    42864, 42865, 42873, 42878, 42888, 42889, 42891, 42896, 42912, 43000, 43002,
    43003, 43010, 43011, 43014, 43015, 43019, 43020, 43043, 43045, 43047, 43048,
    43056, 43062, 43064, 43065, 43072, 43124, 43136, 43138, 43188, 43204, 43214,
    43216, 43232, 43250, 43256, 43259, 43264, 43274, 43302, 43310, 43312, 43335,
    43346, 43359, 43360, 43392, 43395, 43396, 43443, 43444, 43446, 43450, 43452,
    43453, 43457, 43471, 43472, 43486, 43520, 43561, 43567, 43569, 43571, 43573,
    43584, 43587, 43588, 43596, 43597, 43600, 43612, 43616, 43632, 43633, 43639,
    43642, 43643, 43648, 43696, 43697, 43698, 43701, 43703, 43705, 43710, 43712,
    43713, 43714, 43739, 43741, 43742, 43744, 43755, 43756, 43758, 43760, 43762,
    43763, 43765, 43766, 43777, 43785, 43793, 43808, 43816, 43968, 44003, 44005,
    44006, 44008, 44009, 44011, 44012, 44013, 44016, 44032, 55203, 55216, 55243,
    55296, 56191, 56319, 57343, 57344, 63743, 63744, 64112, 64256, 64275, 64285,
    64286, 64287, 64297, 64298, 64312, 64318, 64320, 64323, 64326, 64434, 64467,
    64830, 64831, 64848, 64914, 65008, 65020, 65021, 65024, 65040, 65047, 65048,
    65049, 65056, 65072, 65073, 65075, 65077, 65078, 65079, 65080, 65081, 65082,
    65083, 65084, 65085, 65086, 65087, 65088, 65089, 65090, 65091, 65092, 65093,
    65095, 65096, 65097, 65101, 65104, 65108, 65112, 65113, 65114, 65115, 65116,
    65117, 65118, 65119, 65122, 65123, 65124, 65128, 65129, 65130, 65136, 65142,
    65279, 65281, 65284, 65285, 65288, 65289, 65290, 65291, 65292, 65293, 65294,
    65296, 65306, 65308, 65311, 65313, 65339, 65340, 65341, 65342, 65343, 65344,
    65345, 65371, 65372, 65373, 65374, 65375, 65376, 65377, 65378, 65379, 65380,
    65382, 65392, 65393, 65438, 65440, 65474, 65482, 65490, 65498, 65504, 65506,
    65507, 65508, 65509, 65512, 65513, 65517, 65529, 65532, 0,     13,    40,
    60,    63,    80,    128,   256,   263,   311,   320,   373,   377,   394,
    400,   464,   509,   640,   672,   768,   800,   816,   833,   834,   842,
    896,   927,   928,   968,   976,   977,   1024,  1064,  1104,  1184,  2048,
    2056,  2058,  2103,  2108,  2111,  2135,  2136,  2304,  2326,  2335,  2336,
    2367,  2432,  2494,  2560,  2561,  2565,  2572,  2576,  2581,  2585,  2616,
    2623,  2624,  2640,  2656,  2685,  2687,  2816,  2873,  2880,  2904,  2912,
    2936,  3072,  3680,  4096,  4097,  4098,  4099,  4152,  4167,  4178,  4198,
    4224,  4226,  4227,  4272,  4275,  4279,  4281,  4283,  4285,  4286,  4304,
    4336,  4352,  4355,  4391,  4396,  4397,  4406,  4416,  4480,  4482,  4483,
    4531,  4534,  4543,  4545,  4549,  4560,  5760,  5803,  5804,  5805,  5806,
    5808,  5814,  5815,  5824,  8192,  9216,  9328,  12288, 26624, 28416, 28496,
    28497, 28559, 28563, 45056, 53248, 53504, 53545, 53605, 53607, 53610, 53613,
    53619, 53627, 53635, 53637, 53644, 53674, 53678, 53760, 53826, 53829, 54016,
    54112, 54272, 54298, 54324, 54350, 54358, 54376, 54402, 54428, 54430, 54434,
    54437, 54441, 54446, 54454, 54459, 54461, 54469, 54480, 54506, 54532, 54535,
    54541, 54550, 54558, 54584, 54587, 54592, 54598, 54602, 54610, 54636, 54662,
    54688, 54714, 54740, 54766, 54792, 54818, 54844, 54870, 54896, 54922, 54952,
    54977, 54978, 55003, 55004, 55010, 55035, 55036, 55061, 55062, 55068, 55093,
    55094, 55119, 55120, 55126, 55151, 55152, 55177, 55178, 55184, 55209, 55210,
    55235, 55236, 55242, 55246, 60928, 60933, 60961, 60964, 60967, 60969, 60980,
    60985, 60987, 60994, 60999, 61001, 61003, 61005, 61009, 61012, 61015, 61017,
    61019, 61021, 61023, 61025, 61028, 61031, 61036, 61044, 61049, 61054, 61056,
    61067, 61089, 61093, 61099, 61168, 61440, 61488, 61600, 61617, 61633, 61649,
    61696, 61712, 61744, 61808, 61926, 61968, 62016, 62032, 62208, 62256, 62263,
    62336, 62368, 62406, 62432, 62464, 62528, 62530, 62713, 62720, 62784, 62800,
    62971, 63045, 63104, 63232, 0,     42710, 42752, 46900, 46912, 47133, 63488,
    1,     32,    256,   0,     65533,
};
static u16 so_aFts5UnicodeData[] = {
    1025, 61,   117,   55,   117,   54,   50,   53,    57,    53,   49,   85,
    333,  85,   121,   85,   841,   54,   53,   50,    56,    48,   56,   837,
    54,   57,   50,    57,   1057,  61,   53,   151,   58,    53,   56,   58,
    39,   52,   57,    34,   58,    56,   58,   57,    79,    56,   37,   85,
    56,   47,   39,    51,   111,   53,   745,  57,    233,   773,  57,   261,
    1822, 37,   542,   37,   1534,  222,  69,   73,    37,    126,  126,  73,
    69,   137,  37,    73,   37,    105,  101,  73,    37,    73,   37,   190,
    158,  37,   126,   126,  73,    37,   126,  94,    37,    39,   94,   69,
    135,  41,   40,    37,   41,    40,   37,   41,    40,    37,   542,  37,
    606,  37,   41,    40,   37,    126,  73,   37,    1886,  197,  73,   37,
    73,   69,   126,   105,  37,    286,  2181, 39,    869,   582,  152,  390,
    472,  166,  248,   38,   56,    38,   568,  3596,  158,   38,   56,   94,
    38,   101,  53,    88,   41,    53,   105,  41,    73,    37,   553,  297,
    1125, 94,   37,    105,  101,   798,  133,  94,    57,    126,  94,   37,
    1641, 1541, 1118,  58,   172,   75,   1790, 478,   37,    2846, 1225, 38,
    213,  1253, 53,    49,   55,    1452, 49,   44,    53,    76,   53,   76,
    53,   44,   871,   103,  85,    162,  121,  85,    55,    85,   90,   364,
    53,   85,   1031,  38,   327,   684,  333,  149,   71,    44,   3175, 53,
    39,   236,  34,    58,   204,   70,   76,   58,    140,   71,   333,  103,
    90,   39,   469,   34,   39,    44,   967,  876,   2855,  364,  39,   333,
    1063, 300,  70,    58,   117,   38,   711,  140,   38,    300,  38,   108,
    38,   172,  501,   807,  108,   53,   39,   359,   876,   108,  42,   1735,
    44,   42,   44,    39,   106,   268,  138,  44,    74,    39,   236,  327,
    76,   85,   333,   53,   38,    199,  231,  44,    74,    263,  71,   711,
    231,  39,   135,   44,   39,    106,  140,  74,    74,    44,   39,   42,
    71,   103,  76,    333,  71,    87,   207,  58,    55,    76,   42,   199,
    71,   711,  231,   71,   71,    71,   44,   106,   76,    76,   108,  44,
    135,  39,   333,   76,   103,   44,   76,   42,    295,   103,  711,  231,
    71,   167,  44,    39,   106,   172,  76,   42,    74,    44,   39,   71,
    76,   333,  53,    55,   44,    74,   263,  71,    711,   231,  71,   167,
    44,   39,   42,    44,   42,    140,  74,   74,    44,    44,   42,   71,
    103,  76,   333,   58,   39,    207,  44,   39,    199,   103,  135,  71,
    39,   71,   71,    103,  391,   74,   44,   74,    106,   106,  44,   39,
    42,   333,  111,   218,  55,    58,   106,  263,   103,   743,  327,  167,
    39,   108,  138,   108,  140,   76,   71,   71,    76,    333,  239,  58,
    74,   263,  103,   743,  327,   167,  44,   39,    42,    44,   170,  44,
    74,   74,   76,    74,   39,    71,   76,   333,   71,    74,   263,  103,
    1319, 39,   106,   140,  106,   106,  44,   39,    42,    71,   76,   333,
    207,  58,   199,   74,   583,   775,  295,  39,    231,   44,   106,  108,
    44,   266,  74,    53,   1543,  44,   71,   236,   55,    199,  38,   268,
    53,   333,  85,    71,   39,    71,   39,   39,    135,   231,  103,  39,
    39,   71,   135,   44,   71,    204,  76,   39,    167,   38,   204,  333,
    135,  39,   122,   501,  58,    53,   122,  76,    218,   333,  335,  58,
    44,   58,   44,    58,   44,    54,   50,   54,    50,    74,   263,  1159,
    460,  42,   172,   53,   76,    167,  364,  1164,  282,   44,   218,  90,
    181,  154,  85,    1383, 74,    140,  42,   204,   42,    76,   74,   76,
    39,   333,  213,   199,  74,    76,   135,  108,   39,    106,  71,   234,
    103,  140,  423,   44,   74,    76,   202,  44,    39,    42,   333,  106,
    44,   90,   1225,  41,   41,    1383, 53,   38,    10631, 135,  231,  39,
    135,  1319, 135,   1063, 135,   231,  39,   135,   487,   1831, 135,  2151,
    108,  309,  655,   519,  346,   2727, 49,   19847, 85,    551,  61,   839,
    54,   50,   2407,  117,  110,   423,  135,  108,   583,   108,  85,   583,
    76,   423,  103,   76,   1671,  76,   42,   236,   266,   44,   74,   364,
    117,  38,   117,   55,   39,    44,   333,  335,   213,   49,   149,  108,
    61,   333,  1127,  38,   1671,  1319, 44,   39,    2247,  935,  108,  138,
    76,   106,  74,    44,   202,   108,  58,   85,    333,   967,  167,  1415,
    554,  231,  74,    333,  47,    1114, 743,  76,    106,   85,   1703, 42,
    44,   42,   236,   44,   42,    44,   74,   268,   202,   332,  44,   333,
    333,  245,  38,    213,  140,   42,   1511, 44,    42,    172,  42,   44,
    170,  44,   74,    231,  333,   245,  346,  300,   314,   76,   42,   967,
    42,   140,  74,    76,   42,    44,   74,   71,    333,   1415, 44,   42,
    76,   106,  44,    42,   108,   74,   149,  1159,  266,   268,  74,   76,
    181,  333,  103,   333,  967,   198,  85,   277,   108,   53,   428,  42,
    236,  135,  44,    135,  74,    44,   71,   1413,  2022,  421,  38,   1093,
    1190, 1260, 140,   4830, 261,   3166, 261,  265,   197,   201,  261,  265,
    261,  265,  197,   201,  261,   41,   41,   41,    94,    229,  265,  453,
    261,  264,  261,   264,  261,   264,  165,  69,    137,   40,   56,   37,
    120,  101,  69,    137,  40,    120,  133,  69,    137,   120,  261,  169,
    120,  101,  69,    137,  40,    88,   381,  162,   209,   85,   52,   51,
    54,   84,   51,    54,   52,    277,  59,   60,    162,   61,   309,  52,
    51,   149,  80,    117,  57,    54,   50,   373,   57,    53,   48,   341,
    61,   162,  194,   47,   38,    207,  121,  54,    50,    38,   335,  121,
    54,   50,   422,   855,  428,   139,  44,   107,   396,   90,   41,   154,
    41,   90,   37,    105,  69,    105,  37,   58,    41,    90,   57,   169,
    218,  41,   58,    41,   58,    41,   58,   137,   58,    37,   137,  37,
    135,  37,   90,    69,   73,    185,  94,   101,   58,    57,   90,   37,
    58,   527,  1134,  94,   142,   47,   185,  186,   89,    154,  57,   90,
    57,   90,   57,    250,  57,    1018, 89,   90,    57,    58,   57,   1018,
    8601, 282,  153,   666,  89,    250,  54,   50,    2618,  57,   986,  825,
    1306, 217,  602,   1274, 378,   1935, 2522, 719,   5882,  57,   314,  57,
    1754, 281,  3578,  57,   4634,  3322, 54,   50,    54,    50,   54,   50,
    54,   50,   54,    50,   54,    50,   54,   50,    975,   1434, 185,  54,
    50,   1017, 54,    50,   54,    50,   54,   50,    54,    50,   54,   50,
    537,  8218, 4217,  54,   50,    54,   50,   54,    50,    54,   50,   54,
    50,   54,   50,    54,   50,    54,   50,   54,    50,    54,   50,   54,
    50,   2041, 54,    50,   54,    50,   1049, 54,    50,    8281, 1562, 697,
    90,   217,  346,   1513, 1509,  126,  73,   69,    254,   105,  37,   94,
    37,   94,   165,   70,   105,   37,   3166, 37,    218,   158,  108,  94,
    149,  47,   85,    1221, 37,    37,   1799, 38,    53,    44,   743,  231,
    231,  231,  231,   231,  231,   231,  231,  1036,  85,    52,   51,   52,
    51,   117,  52,    51,   53,    52,   51,   309,   49,    85,   49,   53,
    52,   51,   85,    52,   51,    54,   50,   54,    50,    54,   50,   54,
    50,   181,  38,    341,  81,    858,  2874, 6874,  410,   61,   117,  58,
    38,   39,   46,    54,   50,    54,   50,   54,    50,    54,   50,   54,
    50,   90,   54,    50,   54,    50,   54,   50,    54,    50,   49,   54,
    82,   58,   302,   140,  74,    49,   166,  90,    110,   38,   39,   53,
    90,   2759, 76,    88,   70,    39,   49,   2887,  53,    102,  39,   1319,
    3015, 90,   143,   346,  871,   1178, 519,  1018,  335,   986,  271,  58,
    495,  1050, 335,   1274, 495,   2042, 8218, 39,    39,    2074, 39,   39,
    679,  38,   36583, 1786, 1287,  198,  85,   8583,  38,    117,  519,  333,
    71,   1502, 39,    44,   107,   53,   332,  53,    38,    798,  44,   2247,
    334,  76,   213,   760,  294,   88,   478,  69,    2014,  38,   261,  190,
    350,  38,   88,    158,  158,   382,  70,   37,    231,   44,   103,  44,
    135,  44,   743,   74,   76,    42,   154,  207,   90,    55,   58,   1671,
    149,  74,   1607,  522,  44,    85,   333,  588,   199,   117,  39,   333,
    903,  268,  85,    743,  364,   74,   53,   935,   108,   42,   1511, 44,
    74,   140,  74,    44,   138,   437,  38,   333,   85,    1319, 204,  74,
    76,   74,   76,    103,  44,    263,  44,   42,    333,   149,  519,  38,
    199,  122,  39,    42,   1543,  44,   39,   108,   71,    76,   167,  76,
    39,   44,   39,    71,   38,    85,   359,  42,    76,    74,   85,   39,
    70,   42,   44,    199,  199,   199,  231,  231,   1127,  74,   44,   74,
    44,   74,   53,    42,   44,    333,  39,   39,    743,   1575, 36,   68,
    68,   36,   63,    63,   11719, 3399, 229,  165,   39,    44,   327,  57,
    423,  167,  39,    71,   71,    3463, 536,  11623, 54,    50,   2055, 1735,
    391,  55,   58,    524,  245,   54,   50,   53,    236,   53,   81,   80,
    54,   50,   54,    50,   54,    50,   54,   50,    54,    50,   54,   50,
    54,   50,   54,    50,   85,    54,   50,   149,   112,   117,  149,  49,
    54,   50,   54,    50,   54,    50,   117,  57,    49,    121,  53,   55,
    85,   167,  4327,  34,   117,   55,   117,  54,    50,    53,   57,   53,
    49,   85,   333,   85,   121,   85,   841,  54,    53,    50,   56,   48,
    56,   837,  54,    57,   50,    57,   54,   50,    53,    54,   50,   85,
    327,  38,   1447,  70,   999,   199,  199,  199,   103,   87,   57,   56,
    58,   87,   58,    153,  90,    98,   90,   391,   839,   615,  71,   487,
    455,  3943, 117,   1455, 314,   1710, 143,  570,   47,    410,  1466, 44,
    935,  1575, 999,   143,  551,   46,   263,  46,    967,   53,   1159, 263,
    53,   174,  1289,  1285, 2503,  333,  199,  39,    1415,  71,   39,   743,
    53,   271,  711,   207,  53,    839,  53,   1799,  71,    39,   108,  76,
    140,  135,  103,   871,  108,   44,   271,  309,   935,   79,   53,   1735,
    245,  711,  271,   615,  271,   2343, 1007, 42,    44,    42,   1703, 492,
    245,  655,  333,   76,   42,    1447, 106,  140,   74,    76,   85,   34,
    149,  807,  333,   108,  1159,  172,  42,   268,   333,   149,  76,   42,
    1543, 106,  300,   74,   135,   149,  333,  1383,  44,    42,   44,   74,
    204,  42,   44,    333,  28135, 3182, 149,  34279, 18215, 2215, 39,   1482,
    140,  422,  71,    7898, 1274,  1946, 74,   108,   122,   202,  258,  268,
    90,   236,  986,   140,  1562,  2138, 108,  58,    2810,  591,  841,  837,
    841,  229,  581,   841,  837,   41,   73,   41,    73,    137,  265,  133,
    37,   229,  357,   841,  837,   73,   137,  265,   233,   837,  73,   137,
    169,  41,   233,   837,  841,   837,  841,  837,   841,   837,  841,  837,
    841,  837,  841,   901,  809,   57,   805,  57,    197,   809,  57,   805,
    57,   197,  809,   57,   805,   57,   197,  809,   57,    805,  57,   197,
    809,  57,   805,   57,   197,   94,   1613, 135,   871,   71,   39,   39,
    327,  135,  39,    39,   39,    39,   39,   39,    103,   71,   39,   39,
    39,   39,   39,    39,   71,    39,   135,  231,   135,   135,  39,   327,
    551,  103,  167,   551,  89,    1434, 3226, 506,   474,   506,  506,  367,
    1018, 1946, 1402,  954,  1402,  314,  90,   1082,  218,   2266, 666,  1210,
    186,  570,  2042,  58,   5850,  154,  2010, 154,   794,   2266, 378,  2266,
    3738, 39,   39,    39,   39,    39,   39,   17351, 34,    3074, 7692, 63,
    63,
};

int so_sqlite3Fts5UnicodeCategory(u32 iCode) {
  int iRes = -1;
  int iHi;
  int iLo;
  int ret;
  u16 iKey;

  if (iCode >= (1 << 20)) {
    return 0;
  }
  iLo = so_aFts5UnicodeBlock[(iCode >> 16)];
  iHi = so_aFts5UnicodeBlock[1 + (iCode >> 16)];
  iKey = (iCode & 0xFFFF);
  while (iHi > iLo) {
    int iTest = (iHi + iLo) / 2;
    assert(iTest >= iLo && iTest < iHi);
    if (iKey >= so_aFts5UnicodeMap[iTest]) {
      iRes = iTest;
      iLo = iTest + 1;
    } else {
      iHi = iTest;
    }
  }

  if (iRes < 0)
    return 0;
  if (iKey >= (so_aFts5UnicodeMap[iRes] + (so_aFts5UnicodeData[iRes] >> 5)))
    return 0;
  ret = so_aFts5UnicodeData[iRes] & 0x1F;
  if (ret != 30)
    return ret;
  return ((iKey - so_aFts5UnicodeMap[iRes]) & 0x01) ? 5 : 9;
}

void so_sqlite3Fts5UnicodeAscii(u8 *aArray, u8 *aAscii) {
  int i = 0;
  int iTbl = 0;
  while (i < 128) {
    int bToken = aArray[so_aFts5UnicodeData[iTbl] & 0x1F];
    int n = (so_aFts5UnicodeData[iTbl] >> 5) + i;
    for (; i < 128 && i < n; i++) {
      aAscii[i] = (u8)bToken;
    }
    iTbl++;
  }
  aAscii[0] = 0; /* 0x00 is never a token character */
}
