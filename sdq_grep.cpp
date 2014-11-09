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

inline bool cmp(unsigned tol, unsigned x, unsigned y) {
  return (x>=y) ? (x-y) <= tol : (y-x) <= tol;
}

template <typename ...Ts>
inline bool cmp(unsigned tol, unsigned x, unsigned y,
                unsigned x2,  unsigned y2, Ts ...ts) {
  return cmp(tol,x,y) && cmp(tol,x2,y2,ts...);
}

template <std::size_t X, std::size_t Y>
inline
unsigned calc_score(const std::array<std::array<unsigned,Y>,X> &white_run,
                    const std::array<std::array<unsigned,Y>,X> &white_start)
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
    unsigned r0  = white_run  [digit][0];
    unsigned s0  = white_start[digit][0];
    unsigned sn0 = s0 - xorigin;
    unsigned r1  = white_run  [digit][1];
    unsigned s1  = white_start[digit][1];
    unsigned sn1 = s1 - xorigin;
    unsigned r2  = white_run  [digit][2];
    unsigned s2  = white_start[digit][2];
    unsigned sn2 = s2 - xorigin;

/*
    printf("[%2d,%2d] : ", r0, sn0);
    printf("[%2d,%2d] : ", r1, sn1);
    printf("[%2d,%2d] : ", r2, sn2);
    printf("\n");
*/

    if        (cmp(2,r0,18,sn0,11,r1,12,sn1, 6,r2,18,sn2,11)) {
      digits[digit] = 0;
    } else if (cmp(2,r0, 6,sn0,18,r1, 6,sn1,18,r2, 6,sn2,18)) {
      digits[digit] = 1;
    } else if (cmp(2,r0,18,sn0,11,r1, 8,sn1,21,r2,28,sn2, 6)) {
      digits[digit] = 2;
    } else if (cmp(2,r0,17,sn0,11,r1, 9,sn1,18,r2,18,sn2,11)) {
      digits[digit] = 3;
    } else if (cmp(2,r0, 8,sn0,21,r1,14,sn1, 9,r2, 6,sn2,23)) {
      digits[digit] = 4;
    } else if (cmp(2,r0,28,sn0, 6,r1,15,sn1, 6,r2,18,sn2,11)) {
      digits[digit] = 5;
    } else if (cmp(2,r0,17,sn0,11,r1,22,sn1, 6,r2,18,sn2,11)) { // Yes 2
      digits[digit] = 6;
    } else if (cmp(2,r0,28,sn0, 6,r1, 6,sn1,23,r2, 6,sn2,16)) {
      digits[digit] = 7;
    } else if (cmp(2,r0,18,sn0,11,r1,18,sn1,11,r2,18,sn2,11)) {
      digits[digit] = 8;
    } else if (cmp(2,r0,17,sn0,11,r1,25,sn1, 8,r2,18,sn2,10)) {
      digits[digit] = 9;
    }

    /*if (r0>15&&r0<18&&sn0>10&&sn0<13&&r1==6&&sn1==28&&
        r2>15&&r2<18&&sn2>10&&sn2<13) {
      digits[digit] = 0;
    } else if (r0==6&&sn0==18&&r1==6&&sn1==18&&r2==6&&sn2==18) {
      digits[digit] = 1;
    } else if (r0==18&&sn0==11&&r1==9&&sn1==18&&r2==18&&sn2==11) {
      digits[digit] = 3;
    } else if (r0==8&&sn0==21&&r1==6&&sn1==23&&r2==6&&sn2==23) {
      digits[digit] = 4;
    } else if (r0==28&&sn0==6&&r1==8&&sn1==26&&r2==18&&sn2==11) {
      digits[digit] = 5;
    } else if (r0==28&&sn0==6&&r1==6&&sn1==23&&r2==6&&sn2==16) {
      digits[digit] = 7;
    } else if (r0==28&&sn0==6&&r1==6&&sn1==23&&r2==6&&sn2==16) {
      digits[digit] = 7;
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
  return score;
}

enum icon { nothing, left, right, up, down, button };

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
        int hsample = (68==i) ? 0 : (86==i) ? 1 : (107==i) ? 2 : -1; // 0-2
        if (-1 != hsample) {
          unsigned &wr =   white_run[vslice][hsample];
          unsigned &ws = white_start[vslice][hsample];
//          if (vslice == 3 || vslice == 2) {
//            printf("%d %3d : %2d %3d - %x (%x)\n", vslice, i, wr, ws, data[i*width+j+1], score_white);
// isplay images/all_icons/img025.png 

          if (0 == wr) { ws = j; }
          wr++;
        }
        //if (curr!=prev) { wr = 1; ws = j; }
        //else            { wr++;           }
      }
    }

    if (112==i) // At this point the score has been scanned
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
    "DAPHNE: First Ever Multiple Arcade Laserdisc Emulator =]";
//    "ImageMagick";
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

#ifdef MORE_DEBUG
  Imlib_Image img = imlib_create_image_from_drawable(0,rx,ry,rw,rh,1);
  imlib_context_set_image(img);
  DATA32 *data = imlib_image_get_data();
  unsigned score;
  icon x = find_icon(data,rw,rh,score); 

  // Draw vertical lines between the score digits

/*  for (int x = 0; x < rh; x++) {
  for (int y = 0; y < rw; y++) {
    DATA32 &curr = data[x*rw+y  ];
    if (220 == y) { curr = 0; }
    if (180 == y) { curr = 0; }
    if (140 == y) { curr = 0; }
    if (100 == y) { curr = 0; }
    if ( 60 == y) { curr = 0; }
    if ( 20 == y) { curr = 0; }

//  if (x == 86) { curr = 0; }
  }
  }
  imlib_image_set_format("png");
  imlib_save_image("scorelines.png");
*/

  xdo_free(xdo);
  return 0;
#endif


  icon last_icon = nothing;
  unsigned last_score = 0;
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
    unsigned score = 0;
    icon x = find_icon(data,rw,rh,score); 
    if (score != last_score) { printf("%d\n", score); }
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
    last_score = score;
    last_icon  = x;
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
  } while (1);

  xdo_free(xdo);
  return 0;
}
