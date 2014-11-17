#include <stdio.h>
#include <Imlib2.h>

// g++ -std=c++11 score_scan.cpp -lImlib2 -o score_scan

int main(int argc, char *argv[])
{
  unsigned hsamples[] = {68,86,107};
  const DATA32 score_white  = 0xffffffea;
  const unsigned num_hsamples = sizeof(hsamples)/sizeof(int);
  unsigned white_run[num_hsamples]{}, white_start[num_hsamples]{};
  char str[] = "images/?_score.png";

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
      for (unsigned j = 60; j < 100 ; j++) {     // digit 2 - the 10,000s
        DATA32 curr = data[y*w+j];
        if (curr==score_white) {
          if (white_run[i]==0) { white_start[i] = j-60; }
          white_run[i]++;
        }
      }
    }

    imlib_free_image();

    for (unsigned i = 0; i < num_hsamples; i++) {
      printf("%d: %d %d\n", i, white_start[i], white_run[i]);
    }
  }

  return 0;
}
