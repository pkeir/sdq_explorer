#ifndef SDQ_X_HPP
#define SDQ_X_HPP

#include <cstring>     // memset

#include "sdq_game_params.hpp"

// XStringToKeysym and XKeysymToString can be useful in tracking down
// the strings expected by libxdo's xdo_send_keysequence_window.
// e.g. XK_Left is "Left" and XK_space is "space"
// To use them #include <X11/keysym.h>; though the symbol definitions
// are in /usr/include/X11/keysymdef.h

// This derived from int cmd_search(context_t *) in xdotool's cmd_search.c
Window find_window(const xdo_t *xdo, const char *str)
{
  Window *list, fnd_window = 0;
  unsigned nwindows;
  xdo_search_t search;

  memset(&search, 0, sizeof(xdo_search_t));
  search.max_depth   = -1;  
  search.require     = xdo_search_t::SEARCH_ANY;
  search.searchmask |= SEARCH_NAME;
  search.winname     = str;

  xdo_search_windows(xdo, &search, &list, &nwindows);
  if (nwindows == 1) {
    fnd_window = list[0];
  } else {
    std::cerr << "error: no one window matches: \"" << search.winname << "\"\n";
    exit(1);
  }

  free(list);
  return fnd_window;
}

inline void
send_key_sz(const xdo_t *xdo, const Window target, const char *sz_key)
{
  const int key_delay = 100;
  xdo_send_keysequence_window_down(xdo, target, sz_key, 0);
  std::this_thread::sleep_for(std::chrono::milliseconds(key_delay));
  xdo_send_keysequence_window_up  (xdo, target, sz_key, 0);
}

void send_key(const xdo_t *xdo, const Window target, const prompt_e x)
{
  const char *sz_key;
  switch (x) {
    case prompt_e::L: sz_key = "Left";  break;
    case prompt_e::R: sz_key = "Right"; break;
    case prompt_e::U: sz_key = "Up";    break;
    case prompt_e::D: sz_key = "Down";  break;
    case prompt_e::X: sz_key = "space"; break;
    default: printf("error: prompt unrecognised in send_key."); break;
  }

  send_key_sz(xdo, target, sz_key);
}

void get_coords(const Window target, int &x, int &y, int &w, int &h)
{
  Window   child, root = 0;
  Display *disp = NULL;
  Visual  *vis  = NULL;
  Screen  *scr  = NULL;

  disp = XOpenDisplay(NULL);
  scr  = ScreenOfDisplay(disp, DefaultScreen(disp));
  vis  = DefaultVisual(disp, XScreenNumberOfScreen(scr));
  root = RootWindow(disp, XScreenNumberOfScreen(scr));

  XWindowAttributes attr;
  XGetWindowAttributes(disp, target, &attr);
  w = attr.width; h = attr.height;

  XTranslateCoordinates(disp, target, root, 0, 0, &x, &y, &child);
  imlib_context_set_display(disp);
  imlib_context_set_visual(vis);
  imlib_context_set_drawable(root);
}

inline bool restart_sdq(const xdo_t *xdo, const Window target,
                        const sdq_moves_exhaustive &move_bank_all,
                        level_e &level, unsigned &level_icon_count,
                        unsigned &prev_score)
{
  if (move_bank_all.size())
  {
      fprintf(stderr, "restart_sdq says %d remain!\n", move_bank_all.size());
    send_key_sz(xdo, target, "F3");
    std::this_thread::sleep_for(std::chrono::seconds(5));
    send_key_sz(xdo, target, "5");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    send_key_sz(xdo, target, "1");

    for (unsigned i = 0; i < level_e::num_levels; ++i) {
      unsigned level_icon_count2 = 0;
      for (const std::forward_list<prompt_e> &ps : move_bank_all.moves[i]) {
        unsigned sz = std::distance(std::begin(ps), std::end(ps));
        if (sz) {
          fprintf(stderr,"level: %s level_icon_count: %d ",
                  level_to_string(static_cast<level_e>(i)), level_icon_count2);
          for (const prompt_e &p : ps) {
            fprintf(stderr, "%c", prompt_to_char(p));
          }
          fprintf(stderr,"\n");
        }
        level_icon_count2++;
      }
    }

    level            = bats;
    level_icon_count = 0;
    prev_score       = 0;
          fprintf(stderr,"Bye!\n");
    return true;
  }
  else
  {
    return false;
  }
}

#endif // SDQ_X_HPP
