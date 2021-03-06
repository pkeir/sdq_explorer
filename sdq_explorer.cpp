#include "sdq_explorer.hpp"

// sudo apt-get install libpng-dev libfreetype6-dev libimlib2-dev
// g++ -std=c++11 sdq_explorer.cpp -lX11 -lImlib2 -lxdo -o sdq_explorer

int main(int argc, char *argv[])
{
  bool select_pt = true;

  auto usage = [&argv]() {
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "sdq_explorer playthrough\nor\nsdq_explorer exhaustive\n");
    fprintf(stderr, "defaulting to playthrough...\n");
  };

  if (argc < 2) {
    usage();
  } else {
    if        (0==strncmp("playthrough", argv[1], strlen("playthrough"))) {
      select_pt = true;
    } else if (0==strncmp("exhaustive",  argv[1], strlen("exhaustive")))  {
      select_pt = false;
    } else {
      usage();
    }
  }

  if (select_pt) {
    playthrough();
  } else {
    exhaustive();
  }
  return 0;
}

void exhaustive()
{
  int        x, y, w, h;
  bool       score_changed;
  unsigned   prev_score       = 0;
  prompt_e   prev_icon        = nothing, chosen_move = nothing;
  level_e    level            = bats;
  unsigned   level_icon_count = 0;
  bool       cont             = true;  // Continue exhaustive search
  const char daphne[] =
    "DAPHNE: First Ever Multiple Arcade Laserdisc Emulator =]";

  xdo_t *xdo    = xdo_new(NULL);
  Window target = find_window(xdo, daphne);
  get_coords(target,x,y,w,h); // x, y, w and h are returned
  const sdq_moves move_bank;  // The full, conventional icon sequence for SDQ.
  sdq_moves_exhaustive move_bank_all(move_bank); // All 5 moves, for each QTE

  do {
    unsigned score, icon_offset;

    Imlib_Image img = imlib_create_image_from_drawable(0,x,y,w,h,1); // 640x480
    imlib_context_set_image(img);
    DATA32 const *data = imlib_image_get_data_for_reading_only();

    prompt_e icon = find_prompt(data, w, h, score, icon_offset); 
    if (prev_icon == nothing && icon != nothing) {    // A fresh & valid icon

      const bool death_occurred = !score_changed;

      if (death_occurred || level_completed(level,level_icon_count,move_bank)) {
        level = level_from_icon_offset(icon_offset);
        level_icon_count = 0;
      }

      auto &slist = move_bank_all.moves[level][level_icon_count];
      if (!slist.empty()) {
        icon = slist.front(); slist.pop_front();
      }

      chosen_move = icon;
      send_key(xdo, target, chosen_move);
      fprintf(stderr,"%c", prompt_to_char(chosen_move));

      level_icon_count++;
      score_changed = false;
    }

    if (score != prev_score) {
      if (chosen_move != move_bank.level_moves[level][level_icon_count-1]) {
        fprintf(stderr,"\n%s %d\n", level_to_string(level), level_icon_count);
      }
      score_changed = true;
      prev_score    = score;
      if (level == witch && level_icon_count == 11) {
        cont = restart_sdq(xdo,target,
                           move_bank_all,level,level_icon_count,prev_score);
      }
    }

    prev_icon = icon;
    imlib_free_image();
  } while (cont);

  fprintf(stderr, "\ndone.");
  xdo_free(xdo);
}

void playthrough(bool use_secret_moves)
{
  int        x, y, w, h;
  prompt_e   prev_icon        = nothing;
  level_e    level            = bats;
  unsigned   level_icon_count = 0;
  const char daphne[] =
    "DAPHNE: First Ever Multiple Arcade Laserdisc Emulator =]";

  xdo_t *xdo    = xdo_new(NULL);
  Window target = find_window(xdo, daphne);
  get_coords(target,x,y,w,h); // x, y, w and h are returned
  const sdq_moves move_bank;  // The full, conventional icon sequence for SDQ.

  do {
    unsigned score, icon_offset;

    Imlib_Image img = imlib_create_image_from_drawable(0,x,y,w,h,1); // 640x480
    imlib_context_set_image(img);
    DATA32 const *data = imlib_image_get_data_for_reading_only();

    prompt_e icon = find_prompt(data, w, h, score, icon_offset); 
    if (prev_icon == nothing && icon != nothing) {
      if (level_completed(level,level_icon_count,move_bank)) {
        level = level_from_icon_offset(icon_offset);
        level_icon_count = 0;
      }
      fprintf(stderr,"%s %d\n", level_to_string(level), level_icon_count+1);
      if (use_secret_moves)
        icon = find_secret_icon(level, icon, level_icon_count);
      send_key(xdo, target, icon);
      level_icon_count++;
    }
    prev_icon = icon;
    imlib_free_image();
  } while (!(level == witch && level_icon_count == 11));

  xdo_free(xdo);
}

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

      if (match[i] < best_match) {
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

prompt_e find_prompt(const DATA32 *data, const int width, const int height,
                     unsigned &score, unsigned &icon_offset)
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
      } // if (rgb_dist(curr,score_red,1))
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

// For a given level, icon, and icon count, either returns the given icon;
// or on 14 special occasions, returns an alternative bonus icon
inline prompt_e find_secret_icon(const level_e level,
                                 const prompt_e icon,
                                 const unsigned level_icon_count)
{
  prompt_e secret_icon = nothing;
  switch (level) {
    case mummy:
      if (level_icon_count==1) secret_icon=L;
      if (level_icon_count==3) secret_icon=X;
      break;
    case totem:
      if (level_icon_count==3) secret_icon=X;
      break; 
    case fire_woman:
      if (level_icon_count==4) secret_icon=L;
      break;
    case pyramid_steps:
      if (level_icon_count==6) secret_icon=X;
      break;
    case serpents:
      if (level_icon_count==7) secret_icon=L;
      break;
    case snake:
      if (level_icon_count==1) secret_icon=L;
      if (level_icon_count==2) secret_icon=R;
      break;
    case dragon:
      if (level_icon_count==3) secret_icon=D;
      if (level_icon_count==8) secret_icon=L;
      break;
    case river_jump:
      if (level_icon_count==3) secret_icon=L;
      break;
    case windmill:
      if (level_icon_count==3) secret_icon=X;
      if (level_icon_count==4) secret_icon=L;
      break;
    case witch:
      if (level_icon_count==9) secret_icon=X;
      break;
    default: break;
  }
  if (secret_icon != nothing) {
    // printf something
  } else {
    secret_icon = icon;
  }
  return secret_icon;
}

void take_screenshot(const char *filename, const Imlib_Image &img)
{
  imlib_context_set_image(img);  // already called?
  imlib_image_set_format("png");
  imlib_save_image(filename);
}
