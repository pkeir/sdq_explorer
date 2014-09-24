#include <iostream>
#include <sstream>
#include <unistd.h>
//#include <giblib/giblib.h>
#include <Imlib2.h>

// g++ -std=c++11 sdq_grep.cpp -lX11 -lgiblib -lImlib2 -o sdq_grep
// g++ -std=c++11 sdq_grep.cpp -lX11 -lImlib2 -o sdq_grep
// Ubuntu giblib seems to conflict with libsdl1.2-dev

int main(int argc, char *argv[])
{
  Imlib_Image image   = NULL; // ImlibImage (no underscore)
  Imlib_Image img_arr[16];
  Window      root    = 0;
  Window      target  = 0;
  Display    *disp    = NULL;
  Visual     *vis     = NULL;
  Screen     *scr     = NULL;
  int rx, ry, rw, rh;

  if (argc != 2)
  {
    std::cout << "error: " << argv[0] << " needs an X window id argument.\n";
    return -1;
  }
  std::stringstream ss(argv[1]);
  ss >> target; // Otherwise obtained using scrot_get_window
   
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
  for (int i = 0; i < 16; i++) {
    imlib_context_set_drawable(root);
    img_arr[i] = imlib_create_image_from_drawable(0,rx,ry,rw,rh,1);
    usleep(30);
  }

  for (int i = 0; i < 16; i++) {
    char filename_in[] = "blah?.png";
    filename_in[4]='a'+i;
    imlib_context_set_image(img_arr[i]);
    imlib_image_set_format("png");
    imlib_save_image(filename_in);
  }
  return 0;
}
