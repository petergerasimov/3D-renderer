//----- x1.c simple xwindows example -----

#include <X11/Xlib.h> // must precede most other headers!
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

void update_screen();

int XRES=500;
int YRES=500;
int NPTS=25000;

//Need this array if we're plotting all at once using XDrawPoints
//XPoint pts[NPTS];

Display *dsp;
Window win;
GC gc;

int main(){

  dsp = XOpenDisplay( NULL );
  if( !dsp ){ return 1; }

  int screen = DefaultScreen(dsp);
  unsigned int white = WhitePixel(dsp,screen);
  unsigned int black = BlackPixel(dsp,screen);

  win = XCreateSimpleWindow(dsp,
                            DefaultRootWindow(dsp),
                            0, 0,   // origin
                            XRES, YRES, // size
                            0, black, // border width/clr
                            black);   // backgrd clr


  Atom wmDelete=XInternAtom(dsp, "WM_DELETE_WINDOW", True);
  XSetWMProtocols(dsp, win, &wmDelete, 1);

  gc = XCreateGC(dsp, win,
                 0,       // mask of values
                 NULL);   // array of values

  XSetForeground(dsp, gc, black);

  XEvent evt;
  long eventMask = StructureNotifyMask;
  eventMask |= ButtonPressMask|ButtonReleaseMask|KeyPressMask|KeyReleaseMask;
  XSelectInput(dsp, win, eventMask);

  KeyCode keyQ;
  keyQ = XKeysymToKeycode(dsp, XStringToKeysym("Q"));

  XMapWindow(dsp, win);

  // wait until window appears
  do { XNextEvent(dsp,&evt); } while (evt.type != MapNotify);

  srand(time(0)); // only 1 sec resolution so use once per run
  update_screen();

  int loop = 1;

  while (loop) {
    // for repeated fine seeding use:
    // struct timeval ti
    // gettimeofday(&ti,NULL);
    // srand((ti.tv_sec * 1000) + (ti.tv_usec / 1000));

    //update_screen();

    //XNextEvent(dsp, &evt);
  //   switch (evt.type) {

  //     case (ButtonRelease) :

  //       update_screen();
  //       break;

  //     case (KeyRelease) :

  //       if (evt.xkey.keycode == keyQ) loop = 0;
  //       else update_screen();
  //       break;

  //     case (ConfigureNotify) :

  //       // Check if window has been resized
  //       if (evt.xconfigure.width != XRES || evt.xconfigure.height != YRES)
  //       {
  //         XRES = evt.xconfigure.width;
  //         YRES = evt.xconfigure.height;
  //         update_screen();
  //       }
  //       break;

  //     case (ClientMessage) :

  //       if (evt.xclient.data.l[0] == wmDelete) loop = 0;
  //       break;

  //     default :
  //       //update_screen();
  //       break;
  //   }
  // } 
    update_screen();
  }
  XDestroyWindow(dsp, win);
  XCloseDisplay(dsp);

  return 0;
}

void update_screen()
{
  XClearWindow(dsp, win);

  XDrawLine(dsp, win, gc, 0, YRES/2, XRES-1, YRES/2); //from-to
  XDrawLine(dsp, win, gc, XRES/2, 0, XRES/2, YRES-1); //from-to

  long i;
  for (i=0; i<NPTS; i++) {
    XSetForeground(dsp,gc,0xbb00ff);
    
    XDrawPoint(dsp, win, gc, rand()%XRES, rand()%YRES);
    //pts[i].x = rand()%XRES;
    //pts[i].y = rand()%YRES;
  }
}
  