#ifndef SCORE_DIGITS_HPP
#define SCORE_DIGITS_HPP

struct score_digits {
  static const unsigned num_digits   = 10;
  static const unsigned num_hsamples = 3;
  static const unsigned hsamples[num_hsamples];
  static const unsigned digits[num_digits][num_hsamples][2];
};

const unsigned score_digits::hsamples[num_hsamples] = {68,86,107,};
const unsigned score_digits::digits[num_digits][num_hsamples][2]={
  {{18,11},{12, 6},{18,11},}, // 0
  {{ 6,18},{ 6,18},{ 6,18},}, // 1
  {{18,11},{ 8,21},{28, 6},}, // 2
  {{18,11},{ 9,18},{18,11},}, // 3
  {{ 8,21},{15, 8},{ 6,23},}, // 4
  {{28, 6},{16, 6},{18,11},}, // 5
  {{18,11},{23, 6},{18,11},}, // 6
  {{28, 6},{ 6,23},{ 6,16},}, // 7
  {{18,11},{18,11},{18,11},}, // 8
  {{20,10},{26, 8},{19,10},}, // 9
};

#endif // SCORE_DIGITS_HPP
