#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <cstring>     // memset
#include <chrono>
#include <thread>
#include <climits>
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
#include "sdq_game_params.hpp"
#include "sdq_rgb.hpp"
#include "score_digits.hpp"

//#define SCREENSHOT  

// g++ -std=c++11 sdq_grep.cpp -lX11 -lImlib2 -lxdo -o sdq_grep

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

// offset: how many *pixels* into an image is this icon (no switch on pairs).
level_id_t level_from_icon_offset(const unsigned icon_offset)
{
  switch (icon_offset) {
    case 138628: return bats;           //(388,216)
    case 188506: return gulley;         //(346,294)
    case 106583: return pyramid_steps;  //(343,166)
    case 128191: return water_lift;     //(191,200)
    case 116583: return serpents;       //(103,182)
    case 138568: return mummy;          //(328,216)
    case 200011: return totem;          //(331,312)
    case 179388: return fire_woman;     //(188,280)
    case 148888: return skeletons;      //(408,232)
    case 179528: return hands;          //(328,280)
    case 179508: return dragon;         //(308,280)
    case 168083: return jellyfish;      //(403,262)
    case 199911: return snake;          //(231,312)
    case 167946: return river_jump;     //(266,262)
    case 107908: return river_logs;     //(388,168)
    case 128388: return river_raft;     //(388,200)
    case 189651: return windmill;       //(211,296)
    case 135986: return chariots;       //(306,212)
    case  97471: return stair_chute;    //(191,152)
    case 176886: return closing_walls;  //(246,276)
    case 147483: return winged_goblins; //(283,230)
    case 148791: return laser_eyes;     //(311,232)
    case 159168: return witch;          //(448,248)
    default: printf("error: offset not recognised."); return bats;
  }
}

bool level_completed(const level_id_t level,const unsigned level_icon_count) {

  unsigned level_size;
   
  switch (level) {
    case bats           : level_size = level_t<bats>::type::size(); break;
    case totem          : level_size = level_t<totem>::type::size(); break;
    case fire_woman     : level_size = level_t<fire_woman>::type::size(); break;
    case pyramid_steps  : level_size = level_t<pyramid_steps>::type::size(); break;
    case water_lift     : level_size = level_t<water_lift>::type::size(); break;
    case serpents       : level_size = level_t<serpents>::type::size(); break;
    case mummy          : level_size = level_t<mummy>::type::size(); break;
    case gulley         : level_size = level_t<gulley>::type::size(); break;
    case skeletons      : level_size = level_t<skeletons>::type::size(); break;
    case hands          : level_size = level_t<hands>::type::size(); break;
    case snake          : level_size = level_t<snake>::type::size(); break;
    case dragon         : level_size = level_t<dragon>::type::size(); break;
    case jellyfish      : level_size = level_t<jellyfish>::type::size(); break;
    case river_jump     : level_size = level_t<river_jump>::type::size(); break;
    case river_logs     : level_size = level_t<river_logs>::type::size(); break;
    case river_raft     : level_size = level_t<river_raft>::type::size(); break;
    case windmill       : level_size = level_t<windmill>::type::size(); break;
    case chariots       : level_size = level_t<chariots>::type::size(); break;
    case stair_chute    : level_size = level_t<stair_chute>::type::size(); break;
    case closing_walls  : level_size = level_t<closing_walls>::type::size(); break;
    case winged_goblins : level_size = level_t<winged_goblins>::type::size(); break;
    case laser_eyes     : level_size = level_t<laser_eyes>::type::size(); break;
    case witch          : level_size = level_t<witch>::type::size(); break;
    default: printf("error: no matching level in level_completed"); break;
  };
  return level_size==level_icon_count;
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
    }

    digits[digit] = best;

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

inline const char *prompt_to_string(const prompt_t icon) {
  switch (icon) {
    case nothing: return "nothing";
    case L:       return "left";
    case R:       return "right";
    case U:       return "up";
    case D:       return "down";
    case X:       return "button";
  };
  return "error in prompt_to_string";
}

inline char prompt_to_char(const prompt_t icon) {
  switch (icon) {
    case nothing: return '_';
    case L:       return 'L';
    case R:       return 'R';
    case U:       return 'U';
    case D:       return 'D';
    case X:       return 'X';
  };
  return '0';
}

inline const char *level_id_to_string(const level_id_t level) {
  switch (level) {
    case bats           : return "bats";
    case totem          : return "totem";
    case fire_woman     : return "fire_woman";
    case pyramid_steps  : return "pyramid_steps";
    case water_lift     : return "water_lift";
    case serpents       : return "serpents";
    case mummy          : return "mummy";
    case gulley         : return "gulley";
    case skeletons      : return "skeletons";
    case hands          : return "hands";
    case snake          : return "snake";
    case dragon         : return "dragon";
    case jellyfish      : return "jellyfish";
    case river_jump     : return "river_jump";
    case river_logs     : return "river_logs";
    case river_raft     : return "river_raft";
    case windmill       : return "windmill";
    case chariots       : return "chariots";
    case stair_chute    : return "stair_chute";
    case closing_walls  : return "closing_walls";
    case winged_goblins : return "winged_goblins";
    case laser_eyes     : return "laser_eyes";
    case witch          : return "witch";
    default             : return "error in level_id_to_string";
  };
}

prompt_t find_prompt(const DATA32 *data, int width, int height, unsigned &score,
                     unsigned &icon_offset)
{
  const DATA32 arrow_red    = 0xfffd0100; // ARGB - via grabc
  const DATA32 arrow_blue   = 0xff00ffd8;
  const DATA32 button_green = 0xff00fe00;
  const DATA32 score_white  = 0xfffcffd9;
  const DATA32 score_red    = 0xfffd0100; // same as arrow_red

  static prompt_t last = nothing;
  unsigned red_run   = 0,   red_start, red_i;
  unsigned blue_run  = 0,  blue_start, blue_i;
  unsigned green_run = 0, green_start, green_i;
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
        if (curr!=prev) { red_run = 1;    red_start  = j; red_i = i;  }
        else            { red_run++;                                  }
      }
      else if (curr==arrow_blue) {
        if (curr!=prev) { blue_run = 1;  blue_start  = j; blue_i = i; }
        else            { blue_run++;                                 }
      }
      else if (curr==button_green) {
        if (curr!=prev) { green_run = 1; green_start = j; green_i = i; }
        else            { green_run++;                                 }
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
        // xcoord = blue_start; ycoord = blue_i;
        icon_offset = (blue_i*width)+blue_start;
        return prompt_t::R;
}
//      else if (blue_start==red_start-(blue_run+1))
      else if (blue_start==red_start-blue_run)
{
//        if (last != left)
//          printf("%3d %3d ", blue_start, i);
//        last = left;
        // xcoord = blue_start; ycoord = blue_i;
        icon_offset = (blue_i*width)+blue_start;
        return prompt_t::L;
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
        // xcoord = blue_start; ycoord = blue_i;
        icon_offset = (blue_i*width)+blue_start;
        return prompt_t::D;
}
      else
{
//        if (last != up)
//          printf("%3d %3d ", red_start, i);
//        last = up;
        // xcoord = blue_start; ycoord = blue_i;
        icon_offset = (blue_i*width)+blue_start;
        return prompt_t::U;
}
    }
    if (green_run > 40)
{
//      if (last != button)
//        printf("%3d %3d ", green_start, i);
//      last = button;
      // xcoord = green_start; ycoord = green_i;
      icon_offset = (green_i*width)+green_start;
      return prompt_t::X;
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

// Pertinent data relating to a single Quick Time Event (QTE)
struct qte_info {
  std::list<unsigned> bonuses;
  std::list<prompt_t> moves;
  unsigned normal_bonus = 0;
  unsigned attempts     = 0;
};

int main(int argc, char *argv[])
{
  Imlib_Image image   = NULL; // ImlibImage (no underscore)
  Imlib_Image img_arr[16];
  Window      target  = 0;
  int         x, y, w, h;
  const char daphne[] =
    "DAPHNE: First Ever Multiple Arcade Laserdisc Emulator =]";
  setbuf(stdout,NULL);    // Flush ok; else use fprintf(stderr,"hello");

  xdo_t *xdo = xdo_new(NULL);
  //target = find_window(xdo, "ImageMagick");
  target = find_window(xdo, daphne);
  get_coords(target,x,y,w,h);          // x, y, w and h are returned

  std::vector<prompt_t> moves[level_id_t::num_levels];
  moves[ bats ]           = {R,R,R,L,X,X,X};
  moves[ totem ]          = {L,R,L,R,D,U};
  moves[ fire_woman ]     = {R,L,U,X,U,X};
  moves[ pyramid_steps ]  = {X,X,X,R,X,X,R,X};
  moves[ water_lift ]     = {L};
  moves[ serpents ]       = {X,X,X,R,L,X,X,D};
  moves[ mummy ]          = {R,U,X,R,X};
  moves[ gulley ]         = {D,L,R,L,L,L};
  moves[ skeletons ]      = {R,X,X,X,X,R,X,X,X,X,X,X,X,R,X,X};
  moves[ hands ]          = {R,U,X,L,X,X,L,X,X,X,X};
  moves[ snake ]          = {L,U,U,L,U,L,R};
  moves[ dragon ]         = {R,L,D,R,U,L,L,L,U,R,X};
  moves[ jellyfish ]      = {X,X,X,X,X,X,X,X,U,X};
  moves[ river_jump ]     = {D,U,R,D,R,U};
  moves[ river_logs ]     = {R,L,R,L,L,U,D,L,R,U};
  moves[ river_raft ]     = {R,R,U,R,L,L,D,R};
  moves[ windmill ]       = {L,U,X,R,U};
  moves[ chariots ]       = {U,U,U,U,U,U};
  moves[ stair_chute ]    = {L};
  moves[ closing_walls ]  = {L};
  moves[ winged_goblins ] = {X,X};
  moves[ laser_eyes ]     = {L,R,U,L};
  moves[ witch ]          = {R,X,L,U,R,L,X,X,R,L,X};

  prompt_t prev_icon = nothing;
  unsigned prev_score = 0;
  unsigned level_icon_count = 0;
  prompt_t live_icon = nothing;
  level_id_t   level = bats;
  qte_info qte_tried[level_id_t::num_levels][skeletons_t::size()];
 
  unsigned live_icon_offset;
  bool ignoring_icon = false;
  bool running = true;
  do {
    // img_arr[i] = imlib_create_image_from_drawable(0,x,y,w,h,1);
    // imlib_context_set_image(img_arr[i]);
    Imlib_Image img = imlib_create_image_from_drawable(0,x,y,w,h,1);
    imlib_context_set_image(img);
    //im->data = malloc(width * height * sizeof(DATA32)); // ARGB32
    //See $HOME/apps/imlib2-1.4.4/src/lib/api.c
//    DATA32 *data = img_arr[i]->data;
    //DATA32 const *data = imlib_image_get_data_for_reading_only();
    DATA32 const *data = imlib_image_get_data_for_reading_only();
    //printf("w:%d h:%d\n", w,h);
    //printf("%p\n", data);
    // 640x480
    unsigned score;
    // unsigned xcoord, ycoord; 
    unsigned icon_offset;
    prompt_t icon = find_prompt(data,w,h,score,icon_offset); 
#ifndef SCREENSHOT
    if (score != prev_score) {
      const char *ib = (live_icon != nothing) ? prompt_to_string(live_icon)
                                              : "(bonus)";
      const char *lstr = level_id_to_string(level);
      unsigned bonus = score-prev_score;
      if (ignoring_icon) {
        if (live_icon != nothing) {
          printf("\n%2d %8s %5d %5d %s\n", level_icon_count,
                                           ib, bonus, score, lstr);
          qte_tried[level][level_icon_count].bonuses.push_front(bonus);
          qte_tried[level][level_icon_count].moves.push_front(live_icon);
        }
      } else {
        qte_tried[level][level_icon_count].normal_bonus=bonus;
      }
      /*if (live_icon == nothing) {
        printf("\n    %8s %5d %5d %s\n", ib, score-prev_score, score, lstr);
//        level_icon_count = 0; // But there's no bonus if you die on a level.
      } else {
        printf("\n%2d %8s %5d %5d %s\n", level_icon_count,
                                         ib, score-prev_score, score, lstr);
      }*/
      live_icon = nothing;        // n.b.
      // qte_tried[level][level_icon_count].normal_bonus
    }
#endif
    if (prev_icon == nothing && icon != nothing) {
      // live_icon is reset the last time the score was increased
      if (live_icon != nothing) {
        // printf("There's been a murder!\n");
        level_icon_count = 0;
      }
      if (level_completed(level,level_icon_count)) {
        level_icon_count = 0;  // then level has been successfully completed
      }
      if (level_icon_count==0) {
        level = level_from_icon_offset(icon_offset);
      }
      unsigned &attempts = qte_tried[level][level_icon_count].attempts;
      ignoring_icon    = false;
      if (attempts < 5) {
        prompt_t p = static_cast<prompt_t>(prompt_t::L + attempts);
        if (p==icon) {
          p=static_cast<prompt_t>((icon==prompt_t::X)?prompt_t::L:p+1);
          attempts++;
          ignoring_icon = false; //    Do move as indicated by on-screen prompt
        } else {
          ignoring_icon = true;  // Don't move as indicated by on-screen prompt
        }
        icon = p;
        attempts++;
      }
      printf("%c", prompt_to_char(icon));
      live_icon        = icon;
      live_icon_offset = icon_offset;
      level_icon_count++;

      send_key(xdo, target, icon);
    }

#ifdef  SCREENSHOT
//    if (prev_icon==nothing && icon != nothing) {
    if (score != prev_score) {
      char filename_in[64];// = "images/playthrough/img???_00000.png";
      sprintf(filename_in,
        "%s_%03d_%06d.png", "images/playthrough/img", level_icon_count, score);

      printf("%s\n", filename_in);
      imlib_context_set_image(img);
      imlib_image_set_format("png");
      imlib_save_image(filename_in);
    }
#endif

    imlib_free_image();
    prev_score = score;
    prev_icon  = icon;
    if (level == witch && level_icon_count == 11) {
      printf("game completed.\n");
      bool attempt_missing = false;
      unsigned attempts = 0, attempt_level = 0, attempt_icon = 0;
      for (unsigned i = 0; i < level_id_t::num_levels; i++) {
        for (unsigned j = 0; j < skeletons_t::size(); j++) { // too much
          if (qte_tried[i][j].attempts<5) {
            attempt_missing = true;
            attempts = qte_tried[i][j].attempts;
            attempt_level = i;
            attempt_icon  = j;
          }
        }
      }
      if (attempt_missing) {
        printf("Only %d attempts on level %d, icon %d.\n",
                attempts, attempt_level, attempt_icon);
        printf("Starting again in 5 minutes...\n");
        std::this_thread::sleep_for(std::chrono::minutes(5));
        printf("Starting now.\n");
        xdo_send_keysequence_window_down(xdo, target, "1", 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        xdo_send_keysequence_window_up  (xdo, target, "1", 0);
      } else {
        printf("all done.\n");
        running = false;
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
  } while (running);

  xdo_free(xdo);
  return 0;
}
