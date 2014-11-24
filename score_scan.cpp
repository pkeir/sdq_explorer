#include <stdio.h>
#include <Imlib2.h>
#include "sdq_rgb.hpp"

// g++ -std=c++11 score_scan.cpp -lImlib2 -o score_scan

int main(int argc, char *argv[])
{
  unsigned hsamples[] = {68,86,107,};
  const DATA32 score_white  = 0xffffffea;
  const unsigned num_hsamples = sizeof(hsamples)/sizeof(hsamples[0]);
  const unsigned num_digits   = 10;
  unsigned white_run  [num_digits][num_hsamples]{};
  unsigned white_start[num_digits][num_hsamples]{};
  Imlib_Image img[num_digits];
  char str[] = "images/?_score.png";
//  unsigned digit = 0;

  printf("#ifndef SCORE_DIGITS_HPP\n");
  printf("#define SCORE_DIGITS_HPP\n\n");
  printf("struct score_digits {\n");
  printf("  static const unsigned num_digits   = 10;\n");
  printf("  static const unsigned num_hsamples = 3;\n");
  printf("  static const unsigned hsamples[num_hsamples];\n");
  printf("  static const unsigned digits[num_digits][num_hsamples][2];\n");
  printf("};\n\n");

//  printf("  static const unsigned hsamples[%d] = {", num_hsamples);
  printf("const unsigned score_digits::hsamples[num_hsamples] = {");
  for (const auto s : hsamples) { printf("%d,", s); }
  printf("};\n");
//  printf("  static const unsigned num_hsamples = ");
//  printf("sizeof(hsamples)/sizeof(hsamples[0]);\n");
 printf("const unsigned score_digits::digits[num_digits][num_hsamples][2]={\n");

  for (unsigned digit = 0; digit < num_digits; digit++) {
    str[7]     = '0'+digit;
    img[digit] = imlib_load_image(str);
  } 

//  for (char c = '0'; c <= '9'; c++)
  for (unsigned digit = 0; digit < num_digits; digit++)
  {
//    str[7] = c;
//    Imlib_Image img = imlib_load_image(str);
//    str[7] = '0'+digit;
    imlib_context_set_image(img[digit]);
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

//    printf("    {");
    printf("  {");
    for (unsigned i = 0; i < num_hsamples; i++) {
      if (0==white_run[digit][i]) {
        fprintf(stderr,"error: division by zero would occur in sdq_grep");
      }
      printf("{%2d,%2d},", white_run[digit][i], white_start[digit][i]);
    }
    printf("}, // %d\n", digit);
  }
//  printf("  };\n}; // struct score_digits\n\n");
  printf("};\n\n");
  printf("#endif // SCORE_DIGITS_HPP\n");

  for (unsigned digit = 0; digit < num_digits; digit++) {
    imlib_context_set_image(img[digit]);
    imlib_free_image();
  } 
  return 0;
}
