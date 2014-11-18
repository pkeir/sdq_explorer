#ifndef SDQ_X_HPP
#define SDQ_X_HPP

enum icon { nothing, left, right, up, down, button };

// This derived from int cmd_search(context_t *) in xdotool's cmd_search.c
Window find_window(xdo_t *xdo, const char *str)
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

void send_key(xdo_t *xdo, Window target, icon x)
{
  int key_delay = 100;
  const char *sz_key;
  switch (x) {
    case left:   sz_key = "Left";  break;
    case right:  sz_key = "Right"; break;
    case up:     sz_key = "Up";    break;
    case down:   sz_key = "Down";  break;
    case button: sz_key = "space"; break;
    default:                       break;
  }

  xdo_send_keysequence_window_down(xdo, target, sz_key, 0);
  std::this_thread::sleep_for(std::chrono::milliseconds(key_delay));
  xdo_send_keysequence_window_up  (xdo, target, sz_key, 0);
}

void get_coords(Window target, int &x, int &y, int &w, int &h)
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

#endif // SDQ_X_HPP
