#ifndef SCORE_DIGITS_HPP
#define SCORE_DIGITS_HPP

struct score_digits {
  static const unsigned num_digits   = 10;
  static const unsigned num_hsamples = 5;
  static const unsigned num_fields   = 4;
  static const unsigned hsamples[num_hsamples];
  static const unsigned digits[num_digits][num_hsamples][num_fields];
};

const unsigned score_digits::hsamples[num_hsamples] = {66,81,88,94,111,};
const unsigned score_digits::digits[num_digits][num_hsamples][num_fields]={
  {{14,13, 0, 0},{12, 6, 2,13},{12, 6, 3,13},{12, 6, 3,13},{ 0, 0,15,15},}, // 0
  {{ 6,18, 0, 0},{ 6,18, 1,26},{ 6,18, 1,26},{ 6,18, 1,26},{ 0, 0, 6,21},}, // 1
  {{14,13, 0, 0},{ 8,26, 8, 8},{ 9,18, 4,28},{ 8,11, 3,21},{ 0, 0,29, 8},}, // 2
  {{14,13, 0, 0},{ 8,26, 8, 8},{11,18, 0, 0},{12, 6, 1,36},{ 0, 0,15,15},}, // 3
  {{ 8,21, 0, 0},{12,13, 2,21},{12, 8, 4,16},{12, 6, 2,13},{ 0, 0, 6,26},}, // 4
  {{28, 6, 0, 0},{21, 6, 0, 0},{12, 6, 5,13},{ 6,28, 1,36},{ 0, 0,13,16},}, // 5
  {{14,13, 0, 0},{ 6, 6, 7,13},{26, 6, 0, 0},{12, 6, 3,13},{ 0, 0,15,15},}, // 6
  {{28, 6, 0, 0},{ 6,26, 4,33},{ 6,23, 1,31},{ 6,21, 2,28},{ 0, 0, 7,18},}, // 7
  {{14,13, 0, 0},{12, 6, 3,13},{24, 8, 0, 0},{12, 6, 3,13},{ 0, 0,13,16},}, // 8
  {{14,13, 0, 0},{13, 5, 2,13},{24,10, 1,36},{ 6,28, 1,36},{ 0, 0,13,16},}, // 9
};

#endif // SCORE_DIGITS_HPP
