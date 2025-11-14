
/*
  Include wherever like so:

  #include "mingin.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

  #define MINGIN_IMPLEMENTATION
  #include "mingin.h"

*/



/*

  mingin provides a simple wrapper for the platform-specific infrastructure
  necessary to make a single-player computer game.

  In order to achieve maximum portability across past and future platforms
  that support compiling C89 code, we make no assumptions about the plaforms
  themselves and what they provide.  For example:

  --This code makes no includes that are assumed to be available on every
    plaform.  Any includes present are strictly platform-specific and surrounded
    by #ifdef's that will remove them from compilation on other platforms

  --This code makes no assumptions about any particular metaphors or program
    structures, and works just fine on platforms without "filesystems" or
    "heap allocation" or "threads"

  --This code makes no assumptions about entry points (like main) that must
    be present on every platform.  Platform-specific entry points are possible.

  --The code makes no assumptions about what hardware is present, and can
    be used to run games on platforms that can only provide a subset of
    the required infrastructure.
    

  Such games need seven things to function, which they can't do on their own
  without the help of the underlying platform:

  1.  A way to have regular time steps pass so that game logic can update.

  2.  A way to receive input from the user (mouse, keyboard, controller, etc.).
  
  3.  A way to draw to a rectangular window or screen.
  
  4.  A way to play a stream of audio to some kind of audio output.
  
  5.  A way to read bulk data resources, like graphics and sounds, that
      aren't practical to compile into the program code itself.  For example,
      a game might stream music audio data that is too big to fit in RAM.
      
  6.  A way to read and write data that persists between runs of the game.

  7.  A way to log messages for the purposes of troubleshooting.


  
  Note that a given platform might not provide all of these thing for real,
  yet the game can still function on that plaform (for example, a platform
  without speakers can run a silent version of the game).

  As long as regular time steps pass, the game can still function.
*/



/*
  First, the game itself must implement these functions.
  These are called by the plaform, and the platform is in charge
  of doing any necessary color or sample format conversions with the
  data provided by the game.

  For example, this interface is in 24-bit color, so a platform that
  only supports 8-bit color must do the color conversion.
*/


/*
  Get the next screen full of pixels
  Each R, G, and B color component is 8 bytes, interleaved in
  RGBRGBRGB... in row-major order, starting from the top left corner
  of the screen and going left-to-right and top-to-bottom.
*/
void mingin_gameGetScreenPixels( int inWide, int inHigh,
                                  unsigned char *inRGBBuffer );

/*
  Get the next buffer full of audio samples.
  Samples are in Signed 16-bit little-endian, with channels interleaved

  inSampleBuffer will have inNumSamples * inNumChannels * 2 bytes
*/
void mingin_gameGetAudioSamples( int inNumSamples,
                                  int inNumChannels,
                                  int inSamplesPerSecond,
                                  unsigned char *inSampleBuffer );

/*
  Steps to the next frame of the game.
  
  Note that this is the ONLY function of a game that a platform must
  actually call, so all game logic must be executed by this function.
  
  The game must not depend on being asked for screens of pixels
  or buffers of audio to advance things.

  The rate at which mingin_gameStep is called can be found by calling
  mingin_stepsPerSecond()
*/
void mingin_gameStep( void );




/*
  Game can call these functions

*/

/*
  What's the step rate, in steps per second, that the platform
  is running the game at?
*/
int mingin_stepsPerSecond( void );


/*
  Registers a set of button types that should get mapped to one button
  handle that the game can check for press/release status with
  mingin_isButtonDown().

  The idea here is that the game can define action constants as-needed, like
  JUMP, and then map them to expected inputs on various platforms,
  like { MGN_KEY_SPACE, MGN_BUTTON_SQUARE, MGN_BUTTON_A, MGN_MAP_END }.

  So this would set up the int constant JUMP on keyboards and two different
  types of controllers.

  The game can also use this to map mutiple controls on the same platform
  to the same action constant.  For example, if FIRE can be triggered
  by both { MGN_BUTTON_MOUSE_LEFT, MGN_KEY_X, MGN_MAP_END }
*/
void mingin_registerButtonMapping( int inButtonHandle,
                                    const int inMapping[] );

/*
  Check whether a previously-mapped button handle is currently held down.
  Returns 1 if pressed, 0 if not pressed.
*/
char mingin_isButtonDown( int inButtonHandle );


/*
  Gets the current pointer location, if any.
  Point location is in screen space returned by mingin_getScreenSize(),
  with 0,0 at the top left corner of the screen.

  Returns 1 if pointer location is available, or 0 if not.
*/
char mingin_getPointerLocation( int *inX, int *inY );


/*
  Similar to registering a button mapping, game can define constants as-needed
  to refer to the stick axes that it is interested in.  For example,
  it might define AIM_UP and then map it to
  { MGN_LEFT_STICK_Y, MGN_MIDDLE_STICK_Y, MGN_MAP_END }
*/
void mingin_registerStickAxis( int inStickAxisHandle,
                                const int inMapping[] );



/*
  Platforms must implement these functions, which the game can call
*/





#ifdef MINGIN_IMPLEMENTATION





/*
  Linux platform with X11 windows
  Supports only 24-bit color
*/
#ifdef __linux__

#define WIN_W 200
#define WIN_H 200


/* needed for nanosleep in time.h */

#define _POSIX_C_SOURCE 199309L 

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <time.h>

static unsigned char screenBuffer[ WIN_W * WIN_H * 4 ];


static void frameSleep( void );

static void frameSleep( void ) {
    struct timespec req;
    req.tv_sec = 0; 
    req.tv_nsec = 16666666;

    nanosleep( &req, NULL );
    }



int main( void );

int main( void ) {
    Display *xDisplay = NULL;
    Window xWindow = 0;
    int xScreen = 0;
    XImage *xImage = NULL;
    unsigned int xDepth = 0;
    long unsigned int xBlackColor;
    long unsigned int xWhiteColor;
    GC xGc;
    unsigned char nextColorVal = 255;
        
    xDisplay = XOpenDisplay( NULL );

    xScreen = DefaultScreen( xDisplay );

    xBlackColor = BlackPixel( xDisplay, xScreen );
    xWhiteColor = WhitePixel( xDisplay, xScreen );
    
    xDepth = (unsigned int)DefaultDepth( xDisplay, xScreen );

    xWindow = XCreateSimpleWindow(
        xDisplay, DefaultRootWindow(xDisplay),
        0, 0, WIN_W, WIN_H, 0, xBlackColor, xBlackColor );

    XSelectInput( xDisplay, xWindow,
                  StructureNotifyMask | KeyPressMask | KeyReleaseMask);

    XMapWindow( xDisplay, xWindow );

    xGc = XCreateGC( xDisplay, xWindow, 0, NULL );
    
    XSetForeground( xDisplay, xGc, xWhiteColor );

    /* wait for MapNotify */
    while( 1 ) {
        XEvent e;
        XNextEvent( xDisplay, &e);
        if( e.type == MapNotify ) {
            break;
            }
        }
    
    xImage = XCreateImage( xDisplay,
                           DefaultVisual( xDisplay, xScreen ),
                           xDepth, ZPixmap, 0,
                           (char *)screenBuffer, WIN_W, WIN_H,
                           32, 0 );

    while( 1 ) {
        int numPixels = WIN_W * WIN_H;
        int i;
        
        /* pump all events */
        while( XPending( xDisplay) > 0 ) {
            XEvent e;
            XNextEvent( xDisplay, &e );
            }

        
        for( i=0; i<numPixels; i++ ) {
            int p = i * 4;
            screenBuffer[ p ] = nextColorVal;
            screenBuffer[ p + 1 ] = nextColorVal;
            screenBuffer[ p + 2 ] = nextColorVal;
            screenBuffer[ p + 3 ] = 255;
            }
        if( nextColorVal == 255 ) {
            nextColorVal = 0;
            }
        else {
            nextColorVal = 255;
            }
            
        XPutImage( xDisplay, xWindow, xGc, xImage, 0, 0, 0, 0, WIN_W, WIN_H );
        /* 60 fps */
        frameSleep();
        }

    XDestroyImage( xImage );
    
    return 1;
    }


/* end #ifdef __linux__ */
#endif




/* end #ifdef MINGIN_IMPLEMENTATION */
#endif
