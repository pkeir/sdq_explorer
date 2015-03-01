#ifndef SDQ_GREP_HPP
#define SDQ_GREP_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <chrono>
#include <thread>
#include <climits>
#include <cassert>
#include <Imlib2.h>

// sudo apt-get install libpng-dev libfreetype6-dev libimlib2-dev
// g++ -std=c++11 sdq_grep.cpp -lX11 -lImlib2 -lxdo -o sdq_grep

extern "C" {   // xdo.h assumes a C compiler, so let's wrap it in extern "C"
#include <xdo.h> // sudo apt-get install libxdo-dev
};

#include "sdq_x.hpp"
#include "sdq_game_params.hpp"
#include "sdq_rgb.hpp"
#include "score_digits.hpp"

inline float sdq_distn(unsigned, unsigned);
level_e level_from_icon_offset(const unsigned);
inline bool level_completed(const level_e, const unsigned, const sdq_moves &);
template <std::size_t X>
inline unsigned calc_score(
 const unsigned (&)[X][score_digits::num_hsamples][score_digits::num_fields]
);
inline const char *prompt_to_string(const prompt_e);
inline       char  prompt_to_char  (const prompt_e);
inline const char *level_to_string (const level_e);
prompt_e find_prompt(const DATA32 *, const int, const int,
                     unsigned &, unsigned &);
inline prompt_e find_secret_icon(const prompt_e, const unsigned, const level_e);
void take_screenshot(const char *, const Imlib_Image &);

// Pertinent data relating to a single Quick Time Event (QTE)
struct qte_info {
  std::list<unsigned> bonuses;
  std::list<prompt_e> moves;
  unsigned normal_bonus = 0;
  unsigned attempts     = 0;
};

#endif // SDQ_GREP_HPP
