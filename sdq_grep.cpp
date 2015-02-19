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

// normalised distance
inline float    sdq_distn(unsigned x, unsigned y) {
  return x>y ?                   static_cast<float>(x-y)/x
             : 0==(y-x) ? 0.0f : static_cast<float>(y-x)/y;
}

// offset: how many *pixels* into an image is this icon (no switch on pairs).
level_e level_from_icon_offset(const unsigned icon_offset)
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

inline bool level_completed(const level_e level,
                            const unsigned level_icon_count,
                            const sdq_moves &move_bank)
{
  return move_bank.level_moves[level].size() == level_icon_count;
}

template <std::size_t X>
inline unsigned calc_score(
 const unsigned (&scan)[X][score_digits::num_hsamples][score_digits::num_fields]
)
{
  unsigned digits[X], xorigin = 20;

  for (unsigned digit = 0; digit < X; digit++) {

    const float poor_match = 4.44f; // Low scores, like 0, still rank 6 digits
    float match[score_digits::num_digits]{};
    float best_match = poor_match;  // 0 is a perfect match
    unsigned best = 0;

    for (unsigned i = 0; i < score_digits::num_digits; i++) {
      for (unsigned h = 0; h < score_digits::num_hsamples; h++) {
        unsigned white_run   = scan[digit][h][0];
        unsigned white_start = scan[digit][h][1] - xorigin; // unneeded
        unsigned red_run     = scan[digit][h][2];
        unsigned red_start   = scan[digit][h][3] - xorigin; // ""
        match [i] += sdq_distn(white_run, score_digits::digits[i][h][0]);
        match [i] += sdq_distn(red_run,   score_digits::digits[i][h][2]);
      }

      if (match[i]<best_match) {
        best       = i;
        best_match = match[i];
      }
    }

    digits[digit] = best;
    xorigin += 40;
  }

  unsigned score = 0;
  score += 100000 * digits[0];
  score += 10000  * digits[1];
  score += 1000   * digits[2];
  score += 100    * digits[3];
  score += 10     * digits[4];
  score += 1      * digits[5];
  return score;
}

inline const char *prompt_to_string(const prompt_e icon) {
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

inline char prompt_to_char(const prompt_e icon) {
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

inline const char *level_to_string(const level_e level) {
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
    default             : return "error in level_to_string";
  };
}

prompt_e find_prompt(const DATA32 *data, int width, int height, unsigned &score,
                     unsigned &icon_offset)
{
  const DATA32 arrow_red    = 0xfffd0100; // ARGB - via grabc
  const DATA32 arrow_blue   = 0xff00ffd8;
  const DATA32 button_green = 0xff00fe00;
  const DATA32 score_white  = 0xfffcffd9;
  const DATA32 score_red    = 0xfffd0100; // same as arrow_red

  static prompt_e last = nothing;
  unsigned red_run   = 0,   red_start, red_i;
  unsigned blue_run  = 0,  blue_start, blue_i;
  unsigned green_run = 0, green_start, green_i;
  unsigned scan[6][score_digits::num_hsamples][score_digits::num_fields]{};
  bool     arrow_tail_up_or_down = false;

  for (int i = 0; i < height; i++) {
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
          for (unsigned h = 0; h < score_digits::num_hsamples; h++) {
            if (i==score_digits::hsamples[h]) {
              unsigned &white_run   = scan[vslice][h][0];
              unsigned &white_start = scan[vslice][h][1];
              if (0 == white_run) { white_start = j; }
              white_run++;
            }
          }
        }   // if (j >= 20 || j < (20+5*40))
      }

      if (rgb_dist_lte(curr,score_red,1)) {
        if (j >= 20 && j < (20+6*40)) {
          int vslice  = (j-20)/40 > 5 ? 5 : (j-20)/40; // 0-5
          for (unsigned h = 0; h < score_digits::num_hsamples; h++) {
            if (i==score_digits::hsamples[h]) {
              unsigned &red_run   = scan[vslice][h][2];
              unsigned &red_start = scan[vslice][h][3];
              if (0 == red_run) { red_start = j; }
              red_run++;
            }
          }
        }   // if (j >= 20 || j < (20+5*40))
      }
    }

    if (112==i) // At this point the score has been scanned
      score = calc_score(scan);

    if ((red_run>3)&&(blue_run>80)) // left or right
    {
      if      (red_start==blue_start-red_run)
      {
        icon_offset = (blue_i*width)+blue_start;
        return prompt_e::R;
      }
      else if (blue_start==red_start-blue_run)
      {
        icon_offset = (blue_i*width)+blue_start;
        return prompt_e::L;
      }
    }
    if (red_run>30)
      arrow_tail_up_or_down = true;
    if ((red_run==3)&&(blue_run==32))
    {
      if (arrow_tail_up_or_down)
      {
        icon_offset = (blue_i*width)+blue_start;
        return prompt_e::D;
      }
      else
      {
        icon_offset = (blue_i*width)+blue_start;
        return prompt_e::U;
      }
    }
    if (green_run > 40)
    {
      icon_offset = (green_i*width)+green_start;
      return prompt_e::X;
    }
  }

  last = nothing;
  return nothing;
}

// Pertinent data relating to a single Quick Time Event (QTE)
struct qte_info {
  std::list<unsigned> bonuses;
  std::list<prompt_e> moves;
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

  const sdq_moves move_bank; // The full, conventional icon sequence for SDQ.

  prompt_e prev_icon = nothing;
  unsigned prev_score = 0;
  unsigned level_icon_count = 0;
  prompt_e live_icon = nothing;
  level_e   level = bats;
//  qte_info qte_tried[level_e::num_levels][skeletons_t::size()];
  std::vector<qte_info> qte_tried[level_e::num_levels];

  for (unsigned int v = 0; v < level_e::num_levels; v++) {
    qte_tried[v].resize(move_bank.level_moves[v].size());
  }
 
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
    prompt_e icon = find_prompt(data,w,h,score,icon_offset); 
#ifndef SCREENSHOT
    if (score != prev_score) {
      const char *ib = (live_icon != nothing) ? prompt_to_string(live_icon)
                                              : "(bonus)";
      const char *lstr = level_to_string(level);
      unsigned bonus = score-prev_score;
      /*if (live_icon != nothing)// && ignoring_icon)
        printf("\n%2d %8s %5d %5d %s\n", level_icon_count,
                                         ib, bonus, score, lstr);
      */

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
      if (level_completed(level,level_icon_count,move_bank)) {
        level_icon_count = 0;  // then level has been successfully completed
      }
      if (level_icon_count==0) {
        level = level_from_icon_offset(icon_offset);
      }
// begin exhaustive section

      unsigned &attempts = qte_tried[level][level_icon_count].attempts;
      ignoring_icon    = false;
      if (attempts < 5) {
        prompt_e p = static_cast<prompt_e>(prompt_e::L + attempts);
        if (p==icon) {
          p=static_cast<prompt_e>((icon==prompt_e::X)?prompt_e::L:p+1);
          attempts++;
          ignoring_icon = false; //    Do move as indicated by on-screen prompt
        } else {
          ignoring_icon = true;  // Don't move as indicated by on-screen prompt
        }
        icon = p;
        attempts++;
      }
      printf("%c", prompt_to_char(icon));

// end exhaustive section
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
      for (unsigned i = 0; i < level_e::num_levels; i++) {
//        for (unsigned j = 0; j < skeletons_t::size(); j++) { // too much
        for (unsigned j = 0; j < qte_tried[i].size(); j++) {
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
