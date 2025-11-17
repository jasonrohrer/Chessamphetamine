
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
  First, the game itself must implement these THREE functions.
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
void minginGame_getScreenPixels( int inWide, int inHigh,
                                 unsigned char *inRGBBuffer );


/*
  Get the next buffer full of audio samples.
  Samples are in Signed 16-bit little-endian, with channels interleaved

  inSampleBuffer will have inNumSamples * inNumChannels * 2 bytes
*/
void minginGame_getAudioSamples( int inNumSamples,
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
  mingin_getStepsPerSecond()
*/
void minginGame_step( void );







/*
  Game can call these functions

*/

/*
  What's the step rate, in steps per second, that the platform
  is running the game at?
*/
int mingin_getStepsPerSecond( void );


void mingin_getScreenSize( int *outW, int *outH );


#define MGN_MAP_END 0

typedef enum MinginButton {
    MGN_DUMMY_FIRST_BUTTON = MGN_MAP_END,
    MGN_KEY_Q,
    MGN_KEY_SPACE,
    MGN_KEY_ESCAPE,
    MGN_DUMMY_LAST_BUTTON
    } MinginButton;

#define MGN_NUM_BUTTONS  MGN_DUMMY_LAST_BUTTON

    

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
  by both left mouse button and the X keyboard key with
  { MGN_BUTTON_MOUSE_LEFT, MGN_KEY_X, MGN_MAP_END }

  inButtonHandle can be in the range 0..255, which means 256 distinct
  user actions can be mapped to buttons.

  However, these can be remapped as many times as needed, allowing for
  modal changes to controls, giving way more than 256 user actions if needed.

  inMapping can contain at most 16 elements, including the final MGN_MAP_END

  If there are more than 15 non-END elements in a mapping, the extra ones
  will be ignored.
  
  Returns 1 on success, or 0 on failure (if inButtonHandle is out of
  the supported range)
*/
char mingin_registerButtonMapping( int inButtonHandle,
                                   const MinginButton inMapping[] );



/*
  Check whether a previously-mapped button handle is currently held down.
  Returns 1 if pressed, 0 if not pressed.
*/
char mingin_isButtonDown( int inButtonHandle );


/*
  Gets the current on-screen pointer location, if any.
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
  Writes a string to the log.
*/
void mingin_log( const char *inString );


/*
  Exits the game program
*/
void mingin_quit( void );








#ifdef MINGIN_IMPLEMENTATION


/*
  Platform must call this function once at startup.
*/
void minginInternal_init( void );


/*
  Platforms must implement these functions, which mingin will call internally.
*/

int minginPlatform_getStepsPerSecond( void );


void minginPlatform_getScreenSize( int *outW, int *outH );

void minginPlatform_quit( void );

char minginPlatform_isButtonDown( MinginButton inButton );

void minginPlatform_log( const char *inString );




void mingin_getScreenSize( int *outW, int *outH ) {
    minginPlatform_getScreenSize( outW, outH );
    }


int mingin_getStepsPerSecond( void ) {
    return minginPlatform_getStepsPerSecond();
    }


void mingin_log( const char *inString ) {
    minginPlatform_log( inString );
    }


void mingin_quit( void ) {
    minginPlatform_quit();
    }


#define MINGIN_NUM_BUTTON_MAPPINGS 256
#define MINGIN_MAX_BUTTON_MAPPING_ELEMENTS 16

static MinginButton minginButtonMappings[ MINGIN_NUM_BUTTON_MAPPINGS ]
                                        [ MINGIN_MAX_BUTTON_MAPPING_ELEMENTS ];


void minginInternal_init( void ) {
    int i;
    for( i=0; i< MINGIN_NUM_BUTTON_MAPPINGS; i++ ) {
        minginButtonMappings[ i ][ 0 ] = MGN_MAP_END;
        }
    }


char mingin_registerButtonMapping( int inButtonHandle,
                                   const MinginButton inMapping[] ) {
    int i = 0;
    
    if( inButtonHandle < 0 || inButtonHandle >= MINGIN_NUM_BUTTON_MAPPINGS ) {
        return 0;
        }

    /* leave room at end of our internal mapping for MGN_MAP_END */
    while( i < MINGIN_MAX_BUTTON_MAPPING_ELEMENTS - 1
           &&
           inMapping[ i ] != MGN_MAP_END ) {

        if( inMapping[i] <= MGN_DUMMY_FIRST_BUTTON ||
            inMapping[i] >= MGN_DUMMY_LAST_BUTTON ) {
            /* out of enum range
               end the mapping now */
            break;
            }
        minginButtonMappings[ inButtonHandle ][i] = inMapping[i];
        i++;
        }
    
    /* terminate */
    minginButtonMappings[ inButtonHandle ][i] = MGN_MAP_END;
    
    
    
    return 1;
    }


char mingin_isButtonDown( int inButtonHandle ) {
    int i = 0;
    
    if( inButtonHandle < 0 || inButtonHandle >= MINGIN_NUM_BUTTON_MAPPINGS ) {
        return 0;
        }
    
    while( minginButtonMappings[ inButtonHandle ][i] != MGN_MAP_END ) {
        if( minginPlatform_isButtonDown(
                minginButtonMappings[ inButtonHandle ][i] ) ) {
            return 1;
            }
        i++;
        }
    
    /* got through all possible buttons in mapping, and none
       are considered pressed by the platform */
    return 0;
    }





/*
  Linux platform with X11 windows
  Supports only 24-bit color
*/
#ifdef __linux__

#define WIN_W 700
#define WIN_H 700

#define LINUX_TARGET_FPS 60

/* needed for nanosleep in time.h */

#define _POSIX_C_SOURCE 199309L 

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <time.h>
#include <unistd.h>

/* x11 buffer is RGBA */
static unsigned char screenBuffer[ WIN_W * WIN_H * 4 ];

/* game's expected buffer is RGB */
static unsigned char gameScreenBuffer[ WIN_W * WIN_H * 3 ];

static char shouldQuit = 0;



void minginPlatform_getScreenSize( int *outW, int *outH ) {
    *outW = WIN_W;
    *outH = WIN_H;
    }


int minginPlatform_getStepsPerSecond( void ) {
    return LINUX_TARGET_FPS;
    }


void minginPlatform_quit( void ) {
    shouldQuit = 1;
    }


/* status tracking pressed/released state */
static char buttonDown[ MGN_NUM_BUTTONS ];

/* maps each Mingin key to an X11 XK_ keysym */
static KeySym buttonToXKeyMap[ MGN_NUM_BUTTONS ];

/* fixme:
   still need to handle controller input on Linux */

char minginPlatform_isButtonDown( MinginButton inButton ) {
    if( inButton <= MGN_DUMMY_FIRST_BUTTON ||
        inButton >= MGN_DUMMY_LAST_BUTTON ) {
        return 0;
        }
    
    if( buttonDown[ inButton ] ) {
        return 1;
        }
    
    return 0;
    }


static MinginButton mapXKeyToButton( KeySym inXKey ) {
    int i;
    for( i=0; i<MGN_NUM_BUTTONS; i++ ) {
        if( buttonToXKeyMap[ i ] == inXKey ) {
            return i;
            }
        }
    return MGN_DUMMY_FIRST_BUTTON;
    }

    


static unsigned int stringLength( const char *inString );

    

static unsigned int stringLength( const char *inString ) {
    unsigned int i = 0;
    while( inString[i] != '\0' ) {
        i++;
        }
    return i;
    }
        

void minginPlatform_log( const char *inString ) {
    write( STDOUT_FILENO, inString, stringLength( inString ) );
    }

    

static void frameSleep( void );

static struct timespec lastFrameTime = { 0, 0 };

    
static void frameSleep( void ) {
    struct timespec curTime;
    
    struct timespec sleepTime;
    time_t deltaSec;
    time_t deltaNanoSec;
    time_t targetFrameNanoSec = 1000000000 / LINUX_TARGET_FPS;
    
    clock_gettime( CLOCK_MONOTONIC_RAW, &curTime );

    deltaSec = curTime.tv_sec - lastFrameTime.tv_sec;
    deltaNanoSec = curTime.tv_nsec - lastFrameTime.tv_nsec;
    
    if( deltaSec > 0 || deltaNanoSec >= targetFrameNanoSec ) {
        clock_gettime( CLOCK_MONOTONIC_RAW, &lastFrameTime );
        return;
        }
    
    
    sleepTime.tv_sec = 0; 
    sleepTime.tv_nsec = targetFrameNanoSec - deltaNanoSec;

    nanosleep( &sleepTime, NULL );
    clock_gettime( CLOCK_MONOTONIC_RAW, &lastFrameTime );
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
    int b;
    
    minginInternal_init();

    for( b=0; b<MGN_NUM_BUTTONS; b++ ) {
        buttonDown[b] = 0;
        buttonToXKeyMap[b] = 0;
        }
    /* fixme
       map all keys */
    buttonToXKeyMap[ MGN_KEY_Q ] = XK_q;
    buttonToXKeyMap[ MGN_KEY_ESCAPE ] = XK_Escape;
    buttonToXKeyMap[ MGN_KEY_SPACE ] = XK_space;

    
    
    xDisplay = XOpenDisplay( NULL );

    xScreen = DefaultScreen( xDisplay );

    xBlackColor = BlackPixel( xDisplay, xScreen );
    xWhiteColor = WhitePixel( xDisplay, xScreen );
    
    xDepth = (unsigned int)DefaultDepth( xDisplay, xScreen );

    xWindow = XCreateSimpleWindow(
        xDisplay, DefaultRootWindow(xDisplay),
        0, 0, WIN_W, WIN_H, 0, xBlackColor, xBlackColor );

    XSelectInput( xDisplay, xWindow,
                  StructureNotifyMask | KeyPressMask | KeyReleaseMask );

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

    while( ! shouldQuit ) {
        int numPixels = WIN_W * WIN_H;
        int i;
        
        /* pump all events */
        while( XPending( xDisplay ) > 0 ) {
            XEvent e;
            XNextEvent( xDisplay, &e );

            if( e.type == KeyPress ) {
                
                KeySym ks = XLookupKeysym( &(e.xkey), 0 );
                
                MinginButton button = mapXKeyToButton( ks );
                
                if( b > MGN_DUMMY_FIRST_BUTTON ) {
                    buttonDown[ button ] = 1;
                    }
                }
            else if( e.type == KeyRelease ) {
                KeySym ks = XLookupKeysym( &(e.xkey), 0 );
                
                MinginButton button = mapXKeyToButton( ks );
                
                if( b > MGN_DUMMY_FIRST_BUTTON ) {
                    buttonDown[ button ] = 0;
                    }
                }
            }

        minginGame_step();
        
        minginGame_getScreenPixels( WIN_W, WIN_H, gameScreenBuffer );
        
        for( i=0; i<numPixels; i++ ) {
            int p = i * 4;
            int gp = i * 3;
            
            screenBuffer[ p ] = gameScreenBuffer[ gp ];
            screenBuffer[ p + 1 ] = gameScreenBuffer[ gp + 1 ];
            screenBuffer[ p + 2 ] = gameScreenBuffer[ gp + 2 ];
            screenBuffer[ p + 3 ] = 255;
            }
        
        XPutImage( xDisplay, xWindow, xGc, xImage, 0, 0, 0, 0, WIN_W, WIN_H );
        frameSleep();
        }

    /* data is static
       don't let xlib try destroying it */
    xImage->data = NULL;
    
    XDestroyImage( xImage );

    XFreeGC( xDisplay, xGc );

    XDestroyWindow( xDisplay, xWindow );

    XCloseDisplay( xDisplay );
    
    
    return 1;
    }


/* end #ifdef __linux__ */
#endif




/* end #ifdef MINGIN_IMPLEMENTATION */
#endif
