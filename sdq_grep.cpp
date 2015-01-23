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

//#define SCREENSHOT  
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
  return x>y ?                   static_cast<float>(x-y)/x
             : 0==(y-x) ? 0.0f : static_cast<float>(y-x)/y;
}

template <unsigned X, unsigned Y>
unsigned match_digit(const unsigned (&a)[X][Y], const unsigned (&b)[X][Y])
{
}

template <std::size_t X, std::size_t Y>
inline
unsigned calc_score(const std::array<std::array<unsigned,Y>,X> &white_run,
                    const std::array<std::array<unsigned,Y>,X> &white_start,
                    const unsigned (&scan)[X][Y][score_digits::num_fields])
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
/*    unsigned r0  = scan[digit][0][0];
    unsigned s0  = scan[digit][0][1];
    unsigned sn0 = s0 - xorigin;
    unsigned r1  = scan[digit][1][0];
    unsigned s1  = scan[digit][1][1];
    unsigned sn1 = s1 - xorigin;
    unsigned r2  = scan[digit][2][0];
    unsigned s2  = scan[digit][2][1];
    unsigned sn2 = s2 - xorigin;
*/
// unsigned match_digit(const unsigned (&a)[X][Y], const unsigned (&b)[X][Y])
    const float poor_match = 4.44f;
    float match[10]{}, best_match{poor_match};    // 0 is a perfect match
    float match2[10]{}, best_match2{poor_match};    // 0 is a perfect match
    unsigned best{};

    for (unsigned i = 0; i < 10; i++) {
//      printf("%d-%d (%d %d) %f\n",
//        digit, i, r0, score_digits[i][0][0],
//        sdq_distn(r0,score_digits[i][0][0]));
      for (unsigned h = 0; h < score_digits::num_hsamples; h++) {
        unsigned wr = scan[digit][h][0];
        unsigned ws = scan[digit][h][1] - xorigin;
        unsigned rr = scan[digit][h][2];
        unsigned rs = scan[digit][h][3] - xorigin;
        match [i] += sdq_distn(wr, score_digits::digits[i][h][0]);
        match [i] += sdq_distn(rr, score_digits::digits[i][h][2]);
//        match2[i] += sdq_distn(ws, score_digits::digits[i][h][1]);
//        match2[i] += sdq_distn(rs, score_digits::digits[i][h][3]);
#ifdef MORE_DEBUG
        printf("(%.3f)|", match[i]);
//        printf("[%.3f]|", match2[i]);
#endif
      }

/*      match [i] += sdq_distn(r0, score_digits::digits[i][0][0]);
      match [i] += sdq_distn(r1, score_digits::digits[i][1][0]);
      match [i] += sdq_distn(r2, score_digits::digits[i][2][0]);
      match2[i] += sdq_distn(sn0,score_digits::digits[i][0][1]);
      match2[i] += sdq_distn(sn1,score_digits::digits[i][1][1]);
      match2[i] += sdq_distn(sn2,score_digits::digits[i][2][1]);
*/
//      match[i]+=match2[i];
      if (match[i]<best_match) {
        best       = i;
        best_match = match[i];
      }
#ifdef MORE_DEBUG
      printf("%.3f %.3f %.3f\n", match[i], match2[i], best_match);
#endif
    }

    digits[digit] = best;
#ifdef MORE_DEBUG
    printf("[%u][%f]\n", best, best_match);
#endif

#ifdef  MORE_DEBUG
//    printf("[%2d,%2d] : ", r0, sn0);
//    printf("[%2d,%2d] : ", r1, sn1);
//    printf("[%2d,%2d] : ", r2, sn2);
      for (unsigned h = 0; h < score_digits::num_hsamples; h++) {
        unsigned wr = scan[digit][h][0];
        unsigned ws = scan[digit][h][1] - xorigin;
        printf("[%2u,%2u] : ", wr, ws);
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

inline const char *icon_t_to_string(const icon_t x) {
  if (x==nothing) printf("huh! nothing!\n");
  switch (x) {
    case nothing: return "nothing";
    case left:    return "left";
    case right:   return "right";
    case up:      return "up";
    case down:    return "down";
    case button:  return "button";
  };
  return "";
}

icon_t find_icon(DATA32 *data, int width, int height, unsigned &score)
{
//  const DATA32 arrow_red    = -65536;    // ffff0000  // ARGB
//  const DATA32 arrow_blue   = -16711704; // ff00ffe8
//  const DATA32 button_green = -16711936; // ff00ff00
//  const DATA32 score_white  = 0xffffffea; // grabc can identify pixel colours
//  const DATA32 score_red    = 0xffff0000; // ""
  const DATA32 arrow_red    = 0xfffd0100; // via grabc
  const DATA32 arrow_blue   = 0xff00ffd8;
  const DATA32 button_green = 0xff00fe00;
  const DATA32 score_white  = 0xfffcffd9;
  const DATA32 score_red    = 0xfffd0100; // same as arrow_red

  static icon_t last = nothing;
//  unsigned score     = 0;
  unsigned red_run   = 0,   red_start;
  unsigned blue_run  = 0,  blue_start;
  unsigned green_run = 0, green_start;
  using ua63_t = std::array<std::array<unsigned,score_digits::num_hsamples>,6>; // std::array allows: white_run = zero; (now unneeded?)
  ua63_t white_run{}, white_start{}, zero{};
  unsigned white[6][score_digits::num_hsamples][score_digits::num_fields]{};
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
      else if (rgb_dist_lte(curr,score_white,1)) {
        if (j >= 20 && j < (20+6*40)) {
          int vslice  = (j-20)/40 > 5 ? 5 : (j-20)/40; // 0-5
          // data[i*width+j] = 0;
          //for (unsigned const hsample : score_digits::hsamples) {
          for (unsigned h = 0; h < score_digits::num_hsamples; h++) {
            if (i==score_digits::hsamples[h]) {
              unsigned &wr = white[vslice][h][0];
              unsigned &ws = white[vslice][h][1];
              if (0 == wr) { ws = j; }
              wr++;
            }
          }
        }   // if (j >= 20 || j < (20+5*40))
      }

      if (rgb_dist_lte(curr,score_red,1)) {
        if (j >= 20 && j < (20+6*40)) {
          int vslice  = (j-20)/40 > 5 ? 5 : (j-20)/40; // 0-5
          // data[i*width+j] = 0;
          //for (unsigned const hsample : score_digits::hsamples) {
          for (unsigned h = 0; h < score_digits::num_hsamples; h++) {
            if (i==score_digits::hsamples[h]) {
              unsigned &rr = white[vslice][h][2];
              unsigned &rs = white[vslice][h][3];
              if (0 == rr) { rs = j; }
              rr++;
            }
          }
        }   // if (j >= 20 || j < (20+5*40))
      }
    }

    if (112==i) // At this point the score has been scanned
      score = calc_score(white_run,white_start,white);
//      score = calc_score(white_run,white_start);

//    if ((red_run > 2)&&(blue_run>20))
//      printf("r %d %d\n", red_run, blue_run);
//    if ((red_run > 3)&&(blue_run>80))
//      printf("r %d %d\n", red_run, blue_run);
    // Was red_run==4 but now red_run==5; red_run>3 handles both
    if ((red_run>3)&&(blue_run>80)) // left or right
    {
//      if      (red_start==blue_start-(red_run+1))
      if      (red_start==blue_start-red_run)
{
//        if (last != right)
//          printf("%3d %3d ", red_start, i);
//        last = right;
        return right;
}
//      else if (blue_start==red_start-(blue_run+1))
      else if (blue_start==red_start-blue_run)
{
//        if (last != left)
//          printf("%3d %3d ", blue_start, i);
//        last = left;
        return left;
}
    }
    if (red_run>30)
      arrow_tail_up_or_down = true;
//    if ((red_run==1)&&(blue_run==31))
    if ((red_run==3)&&(blue_run==32))
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
  setbuf(stdout,NULL);    // Flush ok; else use fprintf(stderr,"hello");

  xdo_t *xdo = xdo_new(NULL);
  //target = find_window(xdo, "ImageMagick");
  target = find_window(xdo, daphne);
  get_coords(target,x,y,w,h);          // x, y, w and h are returned

#ifdef MORE_DEBUG 
  Imlib_Image img = imlib_create_image_from_drawable(0,x,y,w,h,1);
  imlib_context_set_image(img);
  DATA32 *data = imlib_image_get_data();
  unsigned score;
  icon_t icon = find_icon(data,w,h,score); 

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
#endif // MORE_DEBUG

  icon_t prev_icon = nothing;
  unsigned prev_score = 0;
//  for (int i = 0; i < nimages; i++) {
  unsigned icon_count = 0;
//  char cH = '0', cT = '0', cU = '0';
  icon_t live_icon = nothing;
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
    //printf("%p\n", data);
    // 640x480
    unsigned score;
    icon_t icon = find_icon(data,w,h,score); 
#ifndef SCREENSHOT
    if (score != prev_score) {
      const char *ib = (live_icon != nothing) ? icon_t_to_string(live_icon)
                                              : "(bonus)";
      if (live_icon == nothing)
        printf("    %8s %5d %5d\n",             ib, score-prev_score, score);
      else
        printf("%3d %8s %5d %5d\n", icon_count, ib, score-prev_score, score);
      live_icon = nothing;
    }
#endif
    if (prev_icon == nothing && icon != nothing) {
      send_key(xdo, target, icon);
      icon_count++;
      live_icon = icon;
    }

#ifdef  SCREENSHOT
//    if (prev_icon==nothing && icon != nothing) {
    if (score != prev_score) {
      char filename_in[64];// = "images/playthrough/img???_00000.png";
      sprintf(filename_in,
        "%s_%03d_%06d.png", "images/playthrough/img", icon_count, score);

      printf("%s\n", filename_in);
      imlib_context_set_image(img);
      imlib_image_set_format("png");
      imlib_save_image(filename_in);
    }
#endif

    imlib_free_image();
    prev_score = score;
    prev_icon  = icon;
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
  } while (1);

  xdo_free(xdo);
  return 0;
}
