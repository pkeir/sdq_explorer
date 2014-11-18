#include <stdio.h>
#include <Imlib2.h>
#include "sdq_rgb.hpp"

// g++ -std=c++11 score_scan.cpp -lImlib2 -o score_scan

int main(int argc, char *argv[])
{
  unsigned hsamples[] = {68,86,107};
  const DATA32 score_white  = 0xffffffea;
  const unsigned num_hsamples = sizeof(hsamples)/sizeof(hsamples[0]);
  const unsigned num_digits   = 10;
  unsigned white_run  [num_digits][num_hsamples]{};
  unsigned white_start[num_digits][num_hsamples]{};
  char str[] = "images/?_score.png";
  unsigned digit = 0;

  printf("#ifndef SCORE_DIGITS_HPP\n");
  printf("#define SCORE_DIGITS_HPP\n\n");
  printf("unsigned score_digits[][%d][2]={\n", num_hsamples);

  for (char c = '0'; c <= '9'; c++)
  {
    str[7] = c;
    Imlib_Image img = imlib_load_image(str);
    imlib_context_set_image(img);
    DATA32 *data = imlib_image_get_data();
    int w        = imlib_image_get_width();
    int h        = imlib_image_get_height();

    for (unsigned i = 0; i < num_hsamples; i++) {
      const unsigned y = hsamples[i];
      for (unsigned j = 60; j < 100 ; j++) {     // digit 2: the 10,000s
        DATA32 curr = data[y*w+j];
        if (rgb_dist_lte(curr,score_white,2)) {
          if (white_run[digit][i]==0) { white_start[digit][i] = j-60; }
          white_run[digit][i]++;
        }
      }
    }

    imlib_free_image();

    printf("  {");
    for (unsigned i = 0; i < num_hsamples; i++) {
      printf("{%2d,%2d},", white_run[digit][i], white_start[digit][i]);
    }
    printf("},\n");
    digit++;
  }
  printf("};\n\n");
  printf("#endif // SCORE_DIGITS_HPP\n");

  return 0;
}
