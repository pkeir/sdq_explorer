#include <iostream>
#include <cstring>     // memset
#include <chrono>
#include <thread>
#include <Imlib2.h>

// sudo apt-get install libpng-dev libfreetype6-dev libimlib2-dev
// XStringToKeysym and XKeysymToString can be useful in tracking down
// the strings expected by libxdo's xdo_send_keysequence_window.
// e.g. XK_Left is "Left" and XK_space is "space"
// To use them #include <X11/keysym.h>; though the symbol definitions
// are in /usr/include/X11/keysymdef.h

extern "C" {   // xdo.h assumes a C compiler, so let's wrap it in extern "C"
#include <xdo.h> // sudo apt-get install libxdo-dev
};

#include "sdq_x.hpp"
#include "sdq_rgb.hpp"
#include "score_digits.hpp"

// #define SCREENSHOT  
//#define MORE_DEBUG
//#define SAVE_SCANLINES // Can be used with MORE_DEBUG

// g++ -std=c++11 sdq_grep.cpp -lX11 -lImlib2 -lxdo -o sdq_grep

inline bool cmp(unsigned tol, unsigned x, unsigned y) {
  return (x>=y) ? (x-y) <= tol : (y-x) <= tol;
}

template <typename ...Ts>
inline bool cmp(unsigned tol, unsigned x, unsigned y,
                unsigned x2,  unsigned y2, Ts ...ts) {
  return cmp(tol,x,y) && cmp(tol,x2,y2,ts...);
}

inline unsigned sdq_dist (unsigned x, unsigned y) { return x>y ? x-y : y-x; }
inline float    sdq_distf(unsigned x, unsigned y) {
  return x>y ? static_cast<float>(x-y) : static_cast<float>(y-x);
}
// normalised distance
inline float    sdq_distn(unsigned x, unsigned y) {
  return x>y ? static_cast<float>(x-y)/x : static_cast<float>(y-x)/y;
}

template <unsigned X, unsigned Y>
unsigned match_digit(const unsigned (&a)[X][Y], const unsigned (&b)[X][Y])
{
}

template <std::size_t X, std::size_t Y>
inline
unsigned calc_score(const std::array<std::array<unsigned,Y>,X> &white_run,
                    const std::array<std::array<unsigned,Y>,X> &white_start,
                    const unsigned (&white)[X][Y][2])
{
  unsigned digits[X]{}, xorigin = 20;
//[18,11] : [12, 6] : [18,11] :  // 0
//[18,11] : [12, 6] : [18,11] : 
//[18,11] : [12, 6] : [18,11] : 
//[ 6,18] : [ 6,18] : [ 6,18] :  // 1
//[ 6,18] : [ 6,18] : [ 6,18] : 
//[18,11] : [ 8,21] : [28, 6] :  // 2 (125)
//[18,11] : [ 8,21] : [28, 6] : 
//[18,11] : [ 9,18] : [18,11] :  // 3
//[18,11] : [ 9,18] : [18,11] : 
//[16,12] : [ 9,18] : [18,11] :
//[ 8,21] : [13,10] : [ 6,23] :  // 4 (153)
//[ 8,21] : [15, 8] : [ 6,23] : 
//[28, 6] : [15, 6] : [18,11] :  // 5 (127)
//[28, 6] : [15, 6] : [18,11] :
//[28, 6] : [18, 5] : [19,10] :  // (After) icon 1 (before icon 2)
//[18,11] : [23, 6] : [18,11] :  // 6 (143)
//[16,12] : [22, 6] : [18,11] :
//[19,10] : [24, 6] : [18,11] : 
//[18,11] : [24, 5] : [18,11] :
//[28, 6] : [ 6,23] : [ 6,16] :  // 7 (119)
//[28, 6] : [ 6,23] : [ 6,16] :
//[18,11] : [18,11] : [18,11] :  // 8 (72)
//[18,11] : [18,11] : [18,11] : 
//[18,11] : [26, 8] : [19,10] :  // 9 (25)
//[17,12] : [25, 8] : [18,11] : 

  for (unsigned digit = 0; digit < X; digit++) {
/*    unsigned r0  = white_run  [digit][0];
    unsigned s0  = white_start[digit][0];
    unsigned sn0 = s0 - xorigin;
    unsigned r1  = white_run  [digit][1];
    unsigned s1  = white_start[digit][1];
    unsigned sn1 = s1 - xorigin;
    unsigned r2  = white_run  [digit][2];
    unsigned s2  = white_start[digit][2];
    unsigned sn2 = s2 - xorigin;
*/
    unsigned r0  = white[digit][0][0];
    unsigned s0  = white[digit][0][1];
    unsigned sn0 = s0 - xorigin;
    unsigned r1  = white[digit][1][0];
    unsigned s1  = white[digit][1][1];
    unsigned sn1 = s1 - xorigin;
    unsigned r2  = white[digit][2][0];
    unsigned s2  = white[digit][2][1];
    unsigned sn2 = s2 - xorigin;

// unsigned match_digit(const unsigned (&a)[X][Y], const unsigned (&b)[X][Y])
    const float poor_match = 7.77f;
    float match[10]{}, best_match{poor_match};    // 0 is a perfect match
    float match2[10]{}, best_match2{poor_match};    // 0 is a perfect match
    unsigned best{};

    for (unsigned i = 0; i < 10; i++) {
//      printf("%d-%d (%d %d) %f\n",
//        digit, i, r0, score_digits[i][0][0],
//        sdq_distn(r0,score_digits[i][0][0]));
      for (unsigned h = 0; h < score_digits::num_hsamples; h++) {
        unsigned r = white[digit][h][0];
        unsigned s = white[digit][h][1] - xorigin;
        match [i] += sdq_distn(r, score_digits::digits[i][h][0]);
        match2[i] += sdq_distn(s, score_digits::digits[i][h][1]);
#ifdef MORE_DEBUG
//        printf("(%.3f)-", match[i]);
        printf("[%.3f]-", match2[i]);
#endif
      }

/*      match [i] += sdq_distn(r0, score_digits::digits[i][0][0]);
      match [i] += sdq_distn(r1, score_digits::digits[i][1][0]);
      match [i] += sdq_distn(r2, score_digits::digits[i][2][0]);
      match2[i] += sdq_distn(sn0,score_digits::digits[i][0][1]);
      match2[i] += sdq_distn(sn1,score_digits::digits[i][1][1]);
      match2[i] += sdq_distn(sn2,score_digits::digits[i][2][1]);
*/
#ifdef MORE_DEBUG
      printf("%.3f %.3f %.3f\n", match[i], match2[i], best_match);
#endif
      match[i]+=match2[i];
      if (match[i]<best_match) {
        best       = i;
        best_match = match[i];
      }
    }

    digits[digit] = best;
#ifdef MORE_DEBUG
    printf("[%d][%f]\n", best, best_match);
#endif

#ifdef  MORE_DEBUG
//    printf("[%2d,%2d] : ", r0, sn0);
//    printf("[%2d,%2d] : ", r1, sn1);
//    printf("[%2d,%2d] : ", r2, sn2);
      for (unsigned h = 0; h < score_digits::num_hsamples; h++) {
        unsigned r = white[digit][h][0];
        unsigned s = white[digit][h][1] - xorigin;
        printf("[%2d,%2d] : ", r, s);
      }
    printf("\n");
#endif

    /*const int tol = 3;
    if        (cmp(tol,r0,18,sn0,11,r1,12,sn1, 6,r2,18,sn2,11)) {
      digits[digit] = 0;
    } else if (cmp(tol,r0, 6,sn0,18,r1, 6,sn1,18,r2, 6,sn2,18)) {
      digits[digit] = 1;
    } else if (cmp(tol,r0,18,sn0,11,r1, 8,sn1,21,r2,28,sn2, 6)) {
      digits[digit] = 2;
    } else if (cmp(tol,r0,17,sn0,11,r1, 9,sn1,18,r2,18,sn2,11)) {
      digits[digit] = 3;
    } else if (cmp(tol,r0, 8,sn0,21,r1,14,sn1, 9,r2, 6,sn2,23)) {
      digits[digit] = 4;
    } else if (cmp(tol,r0,28,sn0, 6,r1,15,sn1, 6,r2,18,sn2,11)) {
      digits[digit] = 5;
    } else if (cmp(tol,r0,17,sn0,11,r1,22,sn1, 6,r2,18,sn2,11)) {
      digits[digit] = 6;
    } else if (cmp(tol,r0,28,sn0, 6,r1, 6,sn1,23,r2, 6,sn2,16)) {
      digits[digit] = 7;
    } else if (cmp(tol,r0,18,sn0,11,r1,18,sn1,11,r2,18,sn2,11)) {
      digits[digit] = 8;
    } else if (cmp(tol,r0,17,sn0,11,r1,25,sn1, 8,r2,18,sn2,10)) {
      digits[digit] = 9;
    }*/

    xorigin += 40;
  }
//  for (auto d : digits) { printf("%d", d); }
//  printf("\n");
  unsigned score{};
  score += 100000 * digits[0];
  score += 10000  * digits[1];
  score += 1000   * digits[2];
  score += 100    * digits[3];
  score += 10     * digits[4];
  score += 1      * digits[5];
#ifdef MORE_DEBUG
  printf("score: %d\n", score);
#endif // MORE_DEBUG
  return score;
}

icon find_icon(DATA32 const *data, int width, int height, unsigned &score)
{
  const DATA32 arrow_red    = -65536;    // ffff0000  // ARGB
  const DATA32 arrow_blue   = -16711704; // ff00ffe8
  const DATA32 button_green = -16711936; // ff00ff00
  const DATA32 score_white  = 0xffffffea; // grabc can identify pixel colours

  static icon last = nothing;
//  unsigned score     = 0;
  unsigned red_run   = 0,   red_start;
  unsigned blue_run  = 0,  blue_start;
  unsigned green_run = 0, green_start;
  using ua63_t = std::array<std::array<unsigned,score_digits::num_hsamples>,6>; // std::array allows:
  ua63_t white_run{}, white_start{}, zero{};           //  white_run = zero;
  unsigned white[6][score_digits::num_hsamples][2]{};
  bool     arrow_tail_up_or_down = false;
 
  for (int i = 0; i < height; i++) {
    int count = 0;
    for (int j = 1; j < width; j++) {

      DATA32 curr = data[i*width+j  ];
      DATA32 prev = data[i*width+j-1];

      if      (curr==arrow_red) {
        if (curr!=prev) { red_run = 1;   red_start  = j;  }
        else            { red_run++;                      }
      }
      else if (curr==arrow_blue) {
        if (curr!=prev) { blue_run = 1;  blue_start  = j; }
        else            { blue_run++;                     }
      }
      else if (curr==button_green) {
        if (curr!=prev) { green_run = 1; green_start = j; }
        else            { green_run++;                    }
      }
//      else if (curr==score_white) {
      else if (rgb_dist_lte(curr,score_white,2)) {
        int vslice  = (j-20)/40 > 5 ? 5 : (j-20)/40; // 0-5
        //for (unsigned const hsample : score_digits::hsamples) {
        for (unsigned h = 0; h < score_digits::num_hsamples; h++) {
          if (i==score_digits::hsamples[h]) {
            unsigned &wr = white[vslice][h][0];
            unsigned &ws = white[vslice][h][1];
            if (0 == wr) { ws = j; }
            wr++;
          }
        }
        /*int hsample = (68==i) ? 0 : (86==i) ? 1 : (107==i) ? 2 : -1; // 0-2
        if (-1 != hsample) {
//          unsigned &wr =   white_run[vslice][hsample];
//          unsigned &ws = white_start[vslice][hsample];
          unsigned &wr = white[vslice][hsample][0];
          unsigned &ws = white[vslice][hsample][1];
//          if (vslice == 3 || vslice == 2) {
//            printf("%d %3d : %2d %3d - %x (%x)\n", vslice, i, wr, ws, data[i*width+j+1], score_white);
// isplay images/all_icons/img025.png 

          if (0 == wr) { ws = j; }
          wr++;
        }*/
        //if (curr!=prev) { wr = 1; ws = j; }
        //else            { wr++;           }
      }
    }

    if (112==i) // At this point the score has been scanned
      score = calc_score(white_run,white_start,white);
//      score = calc_score(white_run,white_start);

    if ((red_run==4)&&(blue_run>80)) // left or right
    {
      if      (red_start==blue_start-(red_run+1))
{
//        if (last != right)
//          printf("%3d %3d ", red_start, i);
//        last = right;
        return right;
}
      else if (blue_start==red_start-(blue_run+1))
{
//        if (last != left)
//          printf("%3d %3d ", blue_start, i);
//        last = left;
        return left;
}
    }
    if (red_run>30)
      arrow_tail_up_or_down = true;
    if ((red_run==1)&&(blue_run==31))
    {
      if (arrow_tail_up_or_down)
{
//        if (last!=down)
//          printf("%3d %3d ", red_start, i);
//        last = down;
        return down;
}
      else
{
//        if (last != up)
//          printf("%3d %3d ", red_start, i);
//        last = up;
        return up;
}
    }
    if (green_run > 40)
{
//      if (last != button)
//        printf("%3d %3d ", green_start, i);
//      last = button;
      return button;
}
/*    if (white_run[0][0] > 0) {
      printf("[%d,%2d,%d] (%5s) : ", i, white_run[0][0], white_start[0][0], "white");
//          for (int i = 0; i < 4; i++) data[i*width+j-i] = 0;
      white_run = zero;
      printf("\n");
    }
*/

// #define DEBUG_ARROWS
#ifdef DEBUG_ARROWS
    if (red_run > 0)  {
      printf("[%d,%2d,%d] (%5s) : ", i, red_run, red_start, "red");
//          for (int i = 0; i < 4; i++) data[i*width+j-i] = 0;
      red_run=0;
      if (blue_run == 0) printf("\n");
    }
    if (blue_run > 0)  {
      printf("[%d,%2d,%d] (%5s) : ", i, blue_run, blue_start, "blue");
//          for (int i = 0; i < 4; i++) data[i*width+j-i] = 0;
      blue_run=0;
      printf("\n");
    }
    if (green_run > 0) {
      printf("[%d,%2d,%d] (%5s) : ", i, green_run, green_start, "green");
//          for (int i = 0; i < 4; i++) data[i*width+j-i] = 0;
      green_run = 0;
      printf("\n");
    }
#endif
  }

  last = nothing;
  return nothing;
}

int main(int argc, char *argv[])
{
  Imlib_Image image   = NULL; // ImlibImage (no underscore)
  Imlib_Image img_arr[16];
  Window      target  = 0;
  int         x, y, w, h;
  const char daphne[] =
#ifdef  MORE_DEBUG
    "ImageMagick";
#else
    "DAPHNE: First Ever Multiple Arcade Laserdisc Emulator =]";
#endif
//    "DAPHNE: Now";

  xdo_t *xdo = xdo_new(NULL);
  //target = find_window(xdo, "ImageMagick");
  target = find_window(xdo, daphne);
  get_coords(target,x,y,w,h);

#ifdef MORE_DEBUG 
  Imlib_Image img = imlib_create_image_from_drawable(0,x,y,w,h,1);
  imlib_context_set_image(img);
  DATA32 *data = imlib_image_get_data();
  unsigned score;
  icon ic = find_icon(data,w,h,score); 

  // Draw vertical/horizontal lines between the score digits

#ifdef SAVE_SCANLINES
  for (unsigned x = 0; x < h; x++) {
  for (unsigned y = 0; y < w; y++) {
    DATA32 &curr = data[x*w+y  ];
/*    if (220 == y) { curr = 0; }
    if (180 == y) { curr = 0; }
    if (140 == y) { curr = 0; }
    if (100 == y) { curr = 0; }
    if ( 60 == y) { curr = 0; }
    if ( 20 == y) { curr = 0; }
*/
//    if ( 68==x) { curr=0; }
//    if ( 86==x) { curr=0; }
//    if (107==x) { curr=0; }
    for (unsigned const hsample : score_digits::hsamples) {
      if (x==hsample) { curr=0; }
    }
  }
  }
  imlib_image_set_format("png");
  imlib_save_image("scorelines2.png");
#endif // SAVE_SCANLINES

  xdo_free(xdo);
  return 0;
#endif

  icon last_icon = nothing;
  unsigned last_score = 0;
//  for (int i = 0; i < nimages; i++) {
  unsigned icon_count = 0;
//  char cH = '0', cT = '0', cU = '0';
  do {
    // img_arr[i] = imlib_create_image_from_drawable(0,x,y,w,h,1);
    // imlib_context_set_image(img_arr[i]);
    Imlib_Image img = imlib_create_image_from_drawable(0,x,y,w,h,1);
    imlib_context_set_image(img);
    //im->data = malloc(width * height * sizeof(DATA32)); // ARGB32
    //See $HOME/apps/imlib2-1.4.4/src/lib/api.c
//    DATA32 *data = img_arr[i]->data;
    //DATA32 const *data = imlib_image_get_data_for_reading_only();
    DATA32 *data = imlib_image_get_data();
    //printf("w:%d h:%d\n", w,h);
    // 640x480
    unsigned score;
    icon ic = find_icon(data,w,h,score); 
#ifndef SCREENSHOT
    if (score != last_score) { printf("%5d %d\n", score, score-last_score); }
#endif
    if (last_icon == nothing && ic!=nothing) {
      send_key(xdo, target, ic);
      icon_count++;
    }

#ifdef  SCREENSHOT
//    if (last_icon==nothing && ic!=nothing) {
    if (score != last_score) {
      char filename_in[64];// = "images/all_icons/img???_00000.png";
      sprintf(filename_in,
        "%s_%03d_%05d.png", "images/all_icons/img", icon_count, score);

      printf("%s\n", filename_in);
      imlib_context_set_image(img);
      imlib_image_set_format("png");
      imlib_save_image(filename_in);
    }
#endif

    imlib_free_image();
    last_score = score;
    last_icon  = ic;
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
  } while (1);

  xdo_free(xdo);
  return 0;
}
