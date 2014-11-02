#include <iostream>
#include <sstream>
#include <cstring>     // memset
#include <chrono>
#include <thread>
#include <Imlib2.h>

// XStringToKeysym and XKeysymToString can be useful in tracking down
// the strings expected by libxdo's xdo_send_keysequence_window.
// e.g. XK_Left is "Left" and XK_space is "space"
// To use them #include <X11/keysym.h>; though the symbol definitions
// are in /usr/include/X11/keysymdef.h

extern "C" {
#include <xdo.h> // sudo apt-get install libxdo-dev
};
// xdo.h assumes a C compiler, so add the extern "C" stuff yourself:
/*
#define CURRENTWINDOW (0)
#define SEARCH_NAME (1UL << 2)
struct xdo_search_t;
struct xdo_t;
extern "C" xdo_t *xdo_new(const char *display);
extern "C" void   xdo_free(xdo_t *xdo);
extern "C" int    xdo_send_keysequence_window(
                         const xdo_t *xdo, Window window,
                         const char *keysequence, useconds_t delay
);
extern "C" int    xdo_search_windows(
                         const xdo_t *xdo, const xdo_search_t *search,
                         Window **windowlist_ret, unsigned int *nwindows_ret
);
*/

// g++ -std=c++11 sdq_grep.cpp -lX11 -lImlib2 -lxdo -o sdq_grep

XKeyEvent createKeyEvent(Display *display, Window &win,
                           Window &winRoot, bool press,
                           int keycode, int modifiers)
{
   XKeyEvent event;

   event.display     = display;
   event.window      = win;
   event.root        = winRoot;
   event.subwindow   = None;
   event.time        = CurrentTime;
   event.x           = 1;
   event.y           = 1;
   event.x_root      = 1;
   event.y_root      = 1;
   event.same_screen = True;
   event.keycode     = XKeysymToKeycode(display, keycode);
   event.state       = modifiers;

   if(press)
      event.type = KeyPress;
   else
      event.type = KeyRelease;

   return event;
}


enum icon { nothing, left, right, up, down, button };

icon find_icon(DATA32 const *data, int width, int height)
{
  const DATA32 arrow_red    = -65536;    // ffff0000  // ARGB
  const DATA32 arrow_blue   = -16711704; // ff00ffe8
  const DATA32 button_green = -16711936; // ff00ff00

  unsigned red_run   = 0, red_start;
  unsigned blue_run  = 0, blue_start;
  unsigned green_run = 0, green_start;
  bool     arrow_tail_up_or_down = false;
  
  for (int i = 0; i < height; i++) {
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
    }

    if ((red_run==4)&&(blue_run>80)) // left or right
    {
      if      (red_start==blue_start-(red_run+1))
        return right;
      else if (blue_start==red_start-(blue_run+1))
        return left;
    }
    if (red_run>30)
      arrow_tail_up_or_down = true;
    if ((red_run==1)&&(blue_run==31))
    {
      if (arrow_tail_up_or_down)
        return down;
      else
        return up;
    }
    if (green_run > 40)
      return button;

#ifdef _DEBUG
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

  return nothing;
}

int main(int argc, char *argv[])
{
  Imlib_Image image   = NULL; // ImlibImage (no underscore)
  Imlib_Image img_arr[16];
  Window      root    = 0;
  Window      target  = 0;
  Display    *disp    = NULL;
  Visual     *vis     = NULL;
  Screen     *scr     = NULL;
  int         rx, ry, rw, rh;

  xdo_t *xdo = xdo_new(NULL);

  // This derived from int cmd_search(context_t *) in xdotool's cmd_search.c
  {
    Window *list;
    unsigned nwindows;
    xdo_search_t search;
    const char daphne[] =
      "DAPHNE: First Ever Multiple Arcade Laserdisc Emulator =]";

    memset(&search, 0, sizeof(xdo_search_t));
    search.max_depth   = -1;  
    search.require     = xdo_search_t::SEARCH_ANY;
    search.searchmask |= SEARCH_NAME;
//    search.winname     = "ImageMagick";
    search.winname     = daphne;
//    search.winname     = "abcde";

    xdo_search_windows(xdo, &search, &list, &nwindows);
    if (nwindows == 1) {
      target = list[0];
    } else {
      std::cerr << "error: no window matching: \"" << search.winname << "\"\n";
      exit(1);
    }
    free(list);
  }

  //xdo_send_keysequence_window(xdo, target, "q", 0);    xdo_free(xdo);
//  xdo_send_keysequence_window(xdo, target, "6", 0);    xdo_free(xdo);
//  xdo_send_keysequence_window(xdo, target, "Left", 0);
//  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
/*  do {
    char c;
    const int delay = 0;
    const int delay2 = 100;
    scanf("%c", &c);
    if (c!=10&&c!=91) {
      if (c=='o') {
//        xdo_send_keysequence_window(xdo, target, "Left", delay);
        xdo_send_keysequence_window_down(xdo, target, "Left", delay);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay2));
        xdo_send_keysequence_window_up(xdo, target, "Left", delay);
        printf("send : Left\n");
      }
      else if (c=='p') {
//        xdo_send_keysequence_window(xdo, target, "Right", delay);
        xdo_send_keysequence_window_down(xdo, target, "Right", delay);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay2));
        xdo_send_keysequence_window_up(xdo, target, "Right", delay);
        printf("send : Right\n");
      }
      else if (c=='q') {
//        xdo_send_keysequence_window(xdo, target, "6", delay);
        xdo_send_keysequence_window_down(xdo, target, "6", delay);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay2));
        xdo_send_keysequence_window_up(xdo, target, "6", delay);
        printf("send : 6\n");
      }
      else if (c=='l') {
//        xdo_send_keysequence_window(xdo, target, "Ctrl", delay);
        xdo_send_keysequence_window_down(xdo, target, "space", delay);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay2));
        xdo_send_keysequence_window_up(xdo, target, "space", delay);
        printf("send : space\n");
      }
    }

  } while (1);
  return 0;
*/
/*
  if (argc != 2)
  {
    std::cout << "error: " << argv[0] << " needs an X window id argument.\n";
    return -1;
  }
  std::stringstream ss(argv[1]);
  ss >> target; // Otherwise obtained using scrot_get_window
*/
   
  disp = XOpenDisplay(static_cast<char *>(NULL));
  scr  = ScreenOfDisplay(disp, DefaultScreen(disp));
  vis  = DefaultVisual(disp, XScreenNumberOfScreen(scr));
  root = RootWindow(disp, XScreenNumberOfScreen(scr));
  imlib_context_set_display(disp);
  imlib_context_set_visual(vis);
//  XRaiseWindow(disp,target); // Needed?? Line 344 in scrot's main.c

  {
    Window child;
    XWindowAttributes attr;
    XGetWindowAttributes(disp, target, &attr);
    rw = attr.width;
    rh = attr.height;
    XTranslateCoordinates(disp, target, root, 0, 0, &rx, &ry, &child);
  }

/*  XKeyEvent event;
  event = createKeyEvent(disp, target, root, true,  XK_Escape, 0);
  int err = XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);
  printf("%d %d %d\n", err, BadWindow, BadValue);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  event = createKeyEvent(disp, target, root, false, XK_Escape, 0);
  XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);
*/


#if 0
//  image = gib_imlib_create_image_from_drawable(root,0,rx,ry,rw,rh,1);
  imlib_context_set_drawable(root);
  image = imlib_create_image_from_drawable(0,rx,ry,rw,rh,1);

//  imlib_context_set_image(image);
//  imlib_image_attach_data_value("quality", NULL, 75, NULL);

  char filename_in[] = "blah.png";
  //gib_imlib_save_image(image, filename_in);
  imlib_context_set_image(image);
  imlib_image_set_format("png");
  imlib_save_image(filename_in);
#endif
  icon last_icon = nothing;
//  for (int i = 0; i < nimages; i++) {
  imlib_context_set_drawable(root);
//  int icount = 0;
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
      int key_delay = 100;
      if       (x==left) {
//        xdo_send_keysequence_window(xdo, target, "Left", 0);
        xdo_send_keysequence_window_down(xdo, target, "Left", 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(key_delay));
        xdo_send_keysequence_window_up  (xdo, target, "Left", 0);
        printf("left\n");
      } else if (x==right) {
//        xdo_send_keysequence_window(xdo, target, "Right", 0);
        xdo_send_keysequence_window_down(xdo, target, "Right", 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(key_delay));
        xdo_send_keysequence_window_up  (xdo, target, "Right", 0);
        printf("right\n");
      } else if (x==up) {
//        xdo_send_keysequence_window(xdo, target, "Up", 0);
        xdo_send_keysequence_window_down(xdo, target, "Up", 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(key_delay));
        xdo_send_keysequence_window_up  (xdo, target, "Up", 0);
        printf("up\n");
/*
    char filename_in[] = "images/blah?.png";
    filename_in[11]='a'+icount; // n.b. element 11 is the question mark: ?
    printf("%s\n", filename_in);
    imlib_context_set_image(img);
    imlib_image_set_format("png");
    imlib_save_image(filename_in);
    icount++;
*/
      } else if (x==down) {
//        xdo_send_keysequence_window(xdo, target, "Down", 0);
        xdo_send_keysequence_window_down(xdo, target, "Down", 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(key_delay));
        xdo_send_keysequence_window_up  (xdo, target, "Down", 0);
        printf("down\n");
      } else if (x==button) {
//        xdo_send_keysequence_window(xdo, target, "space", 0);
        xdo_send_keysequence_window_down(xdo, target, "space", 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(key_delay));
        xdo_send_keysequence_window_up  (xdo, target, "space", 0);
        printf("button\n");
      }
    }
    last_icon = x;
  
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
  } while (1);

#ifdef _DEBUG
  for (int i = 0; i < nimages; i++) {
    char filename_in[] = "images/blah?.png";
    filename_in[11]='a'+i; // n.b. element 11 is the question mark: ?
    printf("%s\n", filename_in);
    imlib_context_set_image(img_arr[i]);
    imlib_image_set_format("png");
    imlib_save_image(filename_in);
  }
#endif

  xdo_free(xdo);
  return 0;
}
