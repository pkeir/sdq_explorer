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
  {{15,13, 0, 0},{15, 6, 5,13},{15, 6, 5,13},{15, 6, 5,13},{ 0, 0,15,16},}, // 0
  {{ 8,18, 0, 0},{ 8,18, 2,26},{ 8,18, 2,26},{ 8,18, 2,26},{ 0, 0, 7,21},}, // 1
  {{15,13, 0, 0},{10,26,10, 8},{10,18, 5,28},{10,11, 5,21},{ 0, 0,30, 8},}, // 2
  {{15,13, 0, 0},{10,26,10, 8},{13,18, 0, 0},{15, 6, 2,36},{ 0, 0,15,16},}, // 3
  {{10,21, 0, 0},{16,13, 4,21},{16, 8, 7,16},{15, 6, 5,13},{ 0, 0, 7,26},}, // 4
  {{30, 6, 0, 0},{22, 6, 0, 0},{15, 6, 7,13},{ 8,28, 2,36},{ 0, 0,15,16},}, // 5
  {{15,13, 0, 0},{ 7, 6,10,13},{27, 6, 0, 0},{15, 6, 5,13},{ 0, 0,15,16},}, // 6
  {{30, 6, 0, 0},{ 7,26, 5,33},{ 8,23, 2,31},{ 7,21, 3,28},{ 0, 0, 8,18},}, // 7
  {{15,13, 0, 0},{15, 6, 5,13},{25, 8, 0, 0},{15, 6, 5,13},{ 0, 0,15,16},}, // 8
  {{15,13, 0, 0},{15, 6, 5,13},{25,11, 2,36},{ 8,28, 2,36},{ 0, 0,15,16},}, // 9
};

#endif // SCORE_DIGITS_HPP
