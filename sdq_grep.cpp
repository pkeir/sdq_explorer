#include <iostream>
//#include <sstream>
#include <cstring>     // memset
#include <chrono>
#include <thread>
#include <Imlib2.h>

// XStringToKeysym and XKeysymToString can be useful in tracking down
// the strings expected by libxdo's xdo_send_keysequence_window.
// e.g. XK_Left is "Left" and XK_space is "space"
// To use them #include <X11/keysym.h>; though the symbol definitions
// are in /usr/include/X11/keysymdef.h

extern "C" {   // xdo.h assumes a C compiler, so let's wrap it in extern "C"
#include <xdo.h> // sudo apt-get install libxdo-dev
};

// g++ -std=c++11 sdq_grep.cpp -lX11 -lImlib2 -lxdo -o sdq_grep

template <std::size_t X, std::size_t Y>
inline
unsigned calc_score(const std::array<std::array<unsigned,Y>,X> &white_run,
                    const std::array<std::array<unsigned,Y>,X> &white_start)
{
  for (unsigned digit = 0; digit < X; digit++) {
    for (unsigned sline = 0; sline < Y; sline++) {
      unsigned run   =   white_run[digit][sline];
      unsigned start = white_start[digit][sline];
      printf("[%2d,%d] : ", run, start);
    }
    printf("\n");
  }
}

enum icon { nothing, left, right, up, down, button };

icon find_icon(DATA32 const *data, int width, int height)
{
  const DATA32 arrow_red    = -65536;    // ffff0000  // ARGB
  const DATA32 arrow_blue   = -16711704; // ff00ffe8
  const DATA32 button_green = -16711936; // ff00ff00
  const DATA32 score_white  = 0xffffffea; // grabc can identify pixel colours

  static icon last = nothing;
  unsigned score     = 0;
  unsigned red_run   = 0,   red_start;
  unsigned blue_run  = 0,  blue_start;
  unsigned green_run = 0, green_start;
  using ua63_t = std::array<std::array<unsigned,3>,6>; // std::array allows:
  ua63_t white_run{}, white_start{}, zero{};           //  white_run = zero;
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
      else if (curr==score_white) {
        int vslice  = (j-20)/40 > 5 ? 5 : (j-20)/40; // 0-5
        int hsample = (68==i) ? 0 : (86==i) ? 1 : (107==i) ? 2 : -1;
        if (-1 != hsample) {
          unsigned &wr =   white_run[vslice][hsample];
          unsigned &ws = white_start[vslice][hsample];
          if (curr!=prev) { wr = 1; ws = j; }
          else            { wr++;           }
        }
      }
    }

    if (112==i)
      score = calc_score(white_run,white_start);

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
    case left:   sz_key = "Left";
    case right:  sz_key = "Right";
    case up:     sz_key = "Up";
    case down:   sz_key = "Down";
    case button: sz_key = "space";
    default: break;
  }

  xdo_send_keysequence_window_down(xdo, target, sz_key, 0);
  std::this_thread::sleep_for(std::chrono::milliseconds(key_delay));
  xdo_send_keysequence_window_up  (xdo, target, sz_key, 0);
}

int main(int argc, char *argv[])
{
  Imlib_Image image   = NULL; // ImlibImage (no underscore)
  Imlib_Image img_arr[16];
  Window      target  = 0;
  int         rx, ry, rw, rh;
  const char daphne[] =
//    "DAPHNE: First Ever Multiple Arcade Laserdisc Emulator =]";
    "ImageMagick";
//    "DAPHNE: Now";

  xdo_t *xdo = xdo_new(NULL);
  //target = find_window(xdo, "ImageMagick");
  target = find_window(xdo, daphne);

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
    rw = attr.width; rh = attr.height;

    XTranslateCoordinates(disp, target, root, 0, 0, &rx, &ry, &child);
    imlib_context_set_display(disp);
    imlib_context_set_visual(vis);
    imlib_context_set_drawable(root);
  }

  Imlib_Image img = imlib_create_image_from_drawable(0,rx,ry,rw,rh,1);
  imlib_context_set_image(img);
  DATA32 *data = imlib_image_get_data();
  icon x = find_icon(data,rw,rh); 
/*
  // Draw vertical lines between the score digits
  for (int x = 0; x < rh; x++) {
  for (int y = 0; y < rw; y++) {
    DATA32 &curr = data[x*rw+y  ];
    if (220 == y) { curr = 0; }
    if (180 == y) { curr = 0; }
    if (140 == y) { curr = 0; }
    if (100 == y) { curr = 0; }
    if ( 60 == y) { curr = 0; }
    if ( 20 == y) { curr = 0; }
  }
  }
  imlib_image_set_format("png");
  imlib_save_image("scorelines.png");
*/
  xdo_free(xdo);
  return 0;


  icon last_icon = nothing;
//  for (int i = 0; i < nimages; i++) {
  int icount = 0;
  char cH = '0', cT = '0', cU = '0';
  do {
    // img_arr[i] = imlib_create_image_from_drawable(0,rx,ry,rw,rh,1);
    // imlib_context_set_image(img_arr[i]);
    Imlib_Image img = imlib_create_image_from_drawable(0,rx,ry,rw,rh,1);
    imlib_context_set_image(img);
    //im->data = malloc(width * height * sizeof(DATA32)); // ARGB32
    //See $HOME/apps/imlib2-1.4.4/src/lib/api.c
//    DATA32 *data = img_arr[i]->data;
    //DATA32 const *data = imlib_image_get_data_for_reading_only();
    DATA32 *data = imlib_image_get_data();
    //printf("w:%d h:%d\n", rw,rh);
    // 640x480
    icon x = find_icon(data,rw,rh); 
    if (last_icon == nothing) {
//      send_key(xdo, target, x);
      int key_delay = 100;
      if       (x==left) {
//        xdo_send_keysequence_window(xdo, target, "Left", 0);
        xdo_send_keysequence_window_down(xdo, target, "Left", 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(key_delay));
        xdo_send_keysequence_window_up  (xdo, target, "Left", 0);
        printf("%3d left\n", icount++);
      } else if (x==right) {
//        xdo_send_keysequence_window(xdo, target, "Right", 0);
        xdo_send_keysequence_window_down(xdo, target, "Right", 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(key_delay));
        xdo_send_keysequence_window_up  (xdo, target, "Right", 0);
        printf("%3d right\n", icount++);
      } else if (x==up) {
//        xdo_send_keysequence_window(xdo, target, "Up", 0);
        xdo_send_keysequence_window_down(xdo, target, "Up", 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(key_delay));
        xdo_send_keysequence_window_up  (xdo, target, "Up", 0);
        printf("%3d up\n", icount++);

      } else if (x==down) {
//        xdo_send_keysequence_window(xdo, target, "Down", 0);
        xdo_send_keysequence_window_down(xdo, target, "Down", 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(key_delay));
        xdo_send_keysequence_window_up  (xdo, target, "Down", 0);
        printf("%3d down\n", icount++);
      } else if (x==button) {
//        xdo_send_keysequence_window(xdo, target, "space", 0);
        xdo_send_keysequence_window_down(xdo, target, "space", 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(key_delay));
        xdo_send_keysequence_window_up  (xdo, target, "space", 0);
        printf("%3d button\n", icount++);
      }
    }

// #define SCREENSHOT  
#ifdef  SCREENSHOT
    if (last_icon==nothing && x!=nothing) {
      char filename_in[] = "images/all_icons/img___.png";
      filename_in[20] = cH;
      filename_in[21] = cT;
      filename_in[22] = cU;
      if (cU=='9') {
        cU='0';
        if (cT=='9') {
          cT='0';
          if (cH=='9') { cH='0'; } else { cH++; }
        }
        else { cT++; }
      }
      else { cU++; }
//      filename_in[11]='a'+icount; // n.b. element 11 is the question mark: ?
      printf("%s\n", filename_in);
      imlib_context_set_image(img);
      imlib_image_set_format("png");
      imlib_save_image(filename_in);
      //icount++;
    }
#endif

    imlib_free_image();
    last_icon = x;
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
  } while (1);

  xdo_free(xdo);
  return 0;
}
