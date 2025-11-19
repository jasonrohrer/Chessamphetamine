
/*
  Include wherever like so:

  #include "mingin.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

  #define MINGIN_IMPLEMENTATION
  #include "mingin.h"

*/



/*

  Mingin provides a simple wrapper for the platform-specific infrastructure
  necessary to make a single-player computer game.

  In order to achieve maximum portability across past and future platforms
  that support compiling C89 code, we make no assumptions about the plaforms
  themselves and what they provide.  For example:

  --This code makes no includes that are assumed to be available on every
    plaform.  Any includes present are strictly platform-specific for
    accessing operating system resources and are surrounded
    by #ifdef's that will remove them from compilation on other platforms.
    None of the C Standard Library includes are used, not even the freestanding
    include subset.

  --This code makes no assumptions about any particular metaphors or program
    structures, and works just fine on platforms without "filesystems" or
    "heap allocation" or "threads"

  --This code makes no assumptions about availability of specialized hardware
    across platforms, so it works just fine on platforms without floating
    point math or 3D graphics cards.

  --This code makes no assumptions about entry points (like main) that must
    be present on every platform.  Platform-specific entry points are possible.

  --The code makes no assumptions about what hardware is present, and can
    be used to run games on platforms that can only provide a subset of
    the required infrastructure.

  Doom is widely celebrated for its portability.  Doom will compile and run
  on your toaster.  Mingin aims to be just as portable, if not more portable
  than Doom.
    

  Single-player computer games need seven things to function, which they
  can't do on their own without the help of the underlying platform, in
  order of importance:

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
  without speakers can run a silent version of the game, or a platform
  with no persistent data store can still run a memoryless version of the
  game that starts fresh each time it is launched, or a platform with
  no bulk data device at all might compile needed bulk data resources directly
  into the program code for access by the game---remember Resource Forks on
  ancient versions of MacOS?).

  As long as regular time steps pass, the game can still function in some
  capacity.
*/



/*
  First, the game itself must implement these FOUR functions.
  These are called by the plaform, and the platform is in charge
  of doing any necessary color or sample format conversions with the
  data provided by the game.

  For example, this interface is in 24-bit color, so a platform that
  only supports 8-bit color must do the color conversion.  The game code
  itself can assume 24-bit color on every platform.
*/


/*
  Gets this minimum screen size that would allow the game graphics
  to be legible.  For a pixel art game, this would be the minimum pixel
  resolution with no upscaling.

  Note that this minium screen size is not guaranteed by a given platform,
  and getScreenPixels may be called with smaller sizes.  However,
  the game doesn't need to produce legible output in those cases (it could
  downscale in a crude way, or crop its display, perhaps).

  On platforms that operate with a flexible window size, windows with
  interger multiples of this miniumum viable size might be used.
*/
void minginGame_getMinimumViableScreenSize( int *outWide, int *outHigh );


/*
  Get the next screen full of pixels
  Each R, G, and B color component is 8 bytes, interleaved in
  RGBRGBRGB... in row-major order, starting from the top left corner
  of the screen and going left-to-right and top-to-bottom.

  Note that inWide and inHigh may change from call to call and aren't
  necessarily fixed across the entire run of a game (for example,
  the game may be switched between windowed and fullscreen mode mid-run,
  and the platform may want to use a different window size after the switch).
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
  That's it for what a game must implement to run on Mingin.

  Note that there is no "init" function that the platform will call.

  A game that needs special init code can do that the first time its
  minginGame_step is called.
*/






/*
  Game can call these functions, which are provided by mingin.

  All of these functions are available on all platforms, though some of them
  might not do anything.
*/

/*
  What's the step rate, in steps per second, that the platform
  is running the game at?

  Every platform will return a positive value here, even if it's not accurate.
*/
int mingin_getStepsPerSecond( void );



#define MGN_MAP_END 0

/*
  This enum lists all the buttons and keys that can possibly be checked
  on some potential platforms.

  For a game to function on as many platforms as possible in an intelligent way,
  the game should cast a wide net for each call to mingin_registerButtonMapping
  below.  Platforms will auto-map their actual controls to the symbols
  in this enum, but they won't cross-map.  For example, a platform with a weird
  "star button" might map that to MGN_BUTTON_SQUARE,
  but a Playstation won't auto map MGN_BUTTON_SQUARE to MGN_BUTTON_A.
  If a game wants a particular control to be triggered by both "square" on PS
  and "A" on XBox, it needs to say so.
  
  Note that not all keys are pressable, even on platforms with keyboards
  that show those symbols.
  
  For example, on US keyboards, it's impossible to hit the % key, since
  that's just the 5 key while SHIFT is held down.
  
  Platforms are generally expected to deal with raw button presses and
  will not automatically map multi-key combos like SHIFT-5 to %
*/
typedef enum MinginButton {
    MGN_BUTTON_NONE = MGN_MAP_END,
    MGN_KEY_BACKSPACE,
    MGN_KEY_TAB,
    MGN_KEY_RETURN,
    MGN_KEY_ESCAPE,
    MGN_KEY_DELETE,
    MGN_KEY_HOME,
    MGN_KEY_LEFT,
    MGN_KEY_UP,
    MGN_KEY_RIGHT,
    MGN_KEY_DOWN,
    MGN_KEY_PAGE_UP,
    MGN_KEY_PAGE_DOWN,
    MGN_KEY_END,
    MGN_KEY_NUM_LOCK,
    MGN_KEY_F1,
    MGN_KEY_F2,
    MGN_KEY_F3,
    MGN_KEY_F4,
    MGN_KEY_F5,
    MGN_KEY_F6,
    MGN_KEY_F7,
    MGN_KEY_F8,
    MGN_KEY_F9,
    MGN_KEY_F10,
    MGN_KEY_F11,
    MGN_KEY_F12,
    MGN_KEY_F13,
    MGN_KEY_F14,
    MGN_KEY_F15,
    MGN_KEY_F16,
    MGN_KEY_F17,
    MGN_KEY_F18,
    MGN_KEY_F19,
    MGN_KEY_F20,
    MGN_KEY_F21,
    MGN_KEY_F22,
    MGN_KEY_F23,
    MGN_KEY_F24,
    MGN_KEY_F25,
    MGN_KEY_F26,
    MGN_KEY_F27,
    MGN_KEY_F28,
    MGN_KEY_F29,
    MGN_KEY_F30,
    MGN_KEY_F31,
    MGN_KEY_F32,
    MGN_KEY_F33,
    MGN_KEY_F34,
    MGN_KEY_F35,
    MGN_KEY_SHIFT_L,
    MGN_KEY_SHIFT_R,
    MGN_KEY_CONTROL_L,
    MGN_KEY_CONTROL_R,
    MGN_KEY_CAPS_LOCK,
    MGN_KEY_META_L,
    MGN_KEY_META_R,
    MGN_KEY_ALT_L,
    MGN_KEY_ALT_R,
    MGN_KEY_SUPER_L,
    MGN_KEY_SUPER_R,
    MGN_KEY_SPACE,
    MGN_KEY_EXCLAMATION,
    MGN_KEY_DOUBLE_QUOTE,
    MGN_KEY_NUMBER_SIGN,
    MGN_KEY_DOLLAR,
    MGN_KEY_PERCENT,
    MGN_KEY_AMPERSAND,
    MGN_KEY_APOSTROPHE,
    MGN_KEY_PAREN_L,
    MGN_KEY_PAREN_R,
    MGN_KEY_ASTERISK,
    MGN_KEY_PLUS,
    MGN_KEY_COMMA,
    MGN_KEY_MINUS,
    MGN_KEY_PERIOD,
    MGN_KEY_SLASH,
    MGN_KEY_0,
    MGN_KEY_1,
    MGN_KEY_2,
    MGN_KEY_3,
    MGN_KEY_4,
    MGN_KEY_5,
    MGN_KEY_6,
    MGN_KEY_7,
    MGN_KEY_8,
    MGN_KEY_9,
    MGN_KEY_COLON,
    MGN_KEY_SEMICOLON,
    MGN_KEY_LESS,
    MGN_KEY_EQUAL,
    MGN_KEY_GREATER,
    MGN_KEY_QUESTION,
    MGN_KEY_AT_SIGN,
    MGN_KEY_BRACKET_L,
    MGN_KEY_BACKSLASH,
    MGN_KEY_BRACKET_R,
    MGN_KEY_CIRCUMFLEX,
    MGN_KEY_UNDERSCORE,
    MGN_KEY_BACK_TICK,
    MGN_KEY_A,
    MGN_KEY_B,
    MGN_KEY_C,
    MGN_KEY_D,
    MGN_KEY_E,
    MGN_KEY_F,
    MGN_KEY_G,
    MGN_KEY_H,
    MGN_KEY_I,
    MGN_KEY_J,
    MGN_KEY_K,
    MGN_KEY_L,
    MGN_KEY_M,
    MGN_KEY_N,
    MGN_KEY_O,
    MGN_KEY_P,
    MGN_KEY_Q,
    MGN_KEY_R,
    MGN_KEY_S,
    MGN_KEY_T,
    MGN_KEY_U,
    MGN_KEY_V,
    MGN_KEY_W,
    MGN_KEY_X,
    MGN_KEY_Y,
    MGN_KEY_Z,
    MGN_KEY_BRACE_L,
    MGN_KEY_VERTICAL_BAR,
    MGN_KEY_BRACE_R,
    MGN_KEY_TILDE,
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
  Gets the last button pressed before this function was called, and clears
  the memory of the last button pressed.

  This is useful for games that have control settings panels and want to let
  the user "live poke" a key or button to change an assigned button.

  In that case, this function would normally be called once to clear the memory
  (which will include whatever button was pressed last, like the mouse
  button for clicking the setting),
  and then repeatedly every minginGame_step until a value is returned.

  Returns MGN_BUTTON_NONE if no button has been pressed since the last time
  the memory was cleared.
*/
MinginButton mingin_getLastButtonPressed( void );


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
  Toggle between fullscreen and windowed mode on platforms that support
  this.

  Note that some platforms might recall the last setting on future startups.

  Returns 1 if toggling supported, 0 if toggling not supported.
*/
char mingin_toggleFullscreen( char inFullscreen );



/*
  Returns 1 if fullscreen, 0 if windowed.
*/
char mingin_isFullscreen( void );



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

char minginPlatform_toggleFullscreen( char inFullscreen );

char minginPlatform_isFullscreen( void );

MinginButton minginPlatform_getLastButtonPressed( void );






int mingin_getStepsPerSecond( void ) {
    return minginPlatform_getStepsPerSecond();
    }


void mingin_log( const char *inString ) {
    minginPlatform_log( inString );
    }


void mingin_quit( void ) {
    minginPlatform_quit();
    }


char mingin_toggleFullscreen( char inFullscreen ) {
    return minginPlatform_toggleFullscreen( inFullscreen );
    }


char mingin_isFullscreen( void ) {
    return minginPlatform_isFullscreen();
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

        if( inMapping[i] <= MGN_BUTTON_NONE ||
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


MinginButton mingin_getLastButtonPressed( void ) {
    return minginPlatform_getLastButtonPressed();
    }




/*
  Linux platform with X11 windows
  Supports only 24-bit color
*/
#ifdef __linux__

#define MAX_WIN_W 4096
#define MAX_WIN_H 2160

#define LINUX_TARGET_FPS 60

/* needed for nanosleep in time.h */
#define _POSIX_C_SOURCE 199309L 
#include <time.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <GL/glx.h>

#include <unistd.h>



/* game's expected buffer is RGB */
static unsigned char gameScreenBuffer[ MAX_WIN_W * MAX_WIN_H * 3 ];

static char shouldQuit = 0;

static int windowW = 0;
static int windowH = 0;



static void getMonitorSize( Display *inXDisplay,
                            int *outW, int *outH );


static void getMonitorSize( Display *inXDisplay,
                            int *outW, int *outH ) {
    Window win = DefaultRootWindow( inXDisplay );
    XWindowAttributes getWinAttr;
    XGetWindowAttributes( inXDisplay, win, &getWinAttr );

    *outW = getWinAttr.width;
    *outH = getWinAttr.height;
    }




int minginPlatform_getStepsPerSecond( void ) {
    return LINUX_TARGET_FPS;
    }


void minginPlatform_quit( void ) {
    shouldQuit = 1;
    }

static char xFullscreen = 0;

char minginPlatform_toggleFullscreen( char inFullscreen ) {
    xFullscreen = inFullscreen;
    return 1;
    }

char minginPlatform_isFullscreen( void ) {
    return xFullscreen;
    }

    

/* memory for getLastButtonPressed call */
static MinginButton lastButtonPressed = MGN_BUTTON_NONE;

/* status tracking pressed/released state */
static char buttonDown[ MGN_NUM_BUTTONS ];

/* maps each Mingin key to an X11 XK_ keysym */
static KeySym buttonToXKeyMap[ MGN_NUM_BUTTONS ];

/* fixme:
   still need to handle controller input on Linux */

char minginPlatform_isButtonDown( MinginButton inButton ) {
    if( inButton <= MGN_BUTTON_NONE ||
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
    return MGN_BUTTON_NONE;
    }


MinginButton minginPlatform_getLastButtonPressed( void ) {
    MinginButton last = lastButtonPressed;
    lastButtonPressed = MGN_BUTTON_NONE;
    return last;
    }
    


static int stringLength( const char *inString );

    

static int stringLength( const char *inString ) {
    int i = 0;
    while( inString[i] != '\0' ) {
        i++;
        }
    return i;
    }
        

void minginPlatform_log( const char *inString ) {
    write( STDOUT_FILENO, inString, (unsigned int)stringLength( inString ) );
    }

/*
  Returns a static buffer that must be used before next call to intToString.
*/
static const char *intToString( int inInt );


static char intToStringBuffer[20];


static const char *intToString( int inInt ) {
    unsigned int c = 0;
    /* start with billions */
    int divisor = 1000000000;
    const char *formatError = "[int_format_error]";
    
    /* skip 0 digits until our first non-zero digit */
    int qLowerLimit = 1;
    
    if( inInt == 0 ) {
        return "0";
        }
    if( inInt < 0 ) {
        intToStringBuffer[c] = '-';
        c++;
        inInt *= -1;
        }
    while( divisor >= 1 ) {
        int q = inInt / divisor;
        if( q >= qLowerLimit ) {
            if( q > 9 ) {
                return formatError;
                }
            if( c >= sizeof( intToStringBuffer ) - 1 ) {
                /* out of room? */
                return formatError;
                }
            
            intToStringBuffer[c] = (char)( '0' + q );
            c++;
            /* we've seen at least one non-zero digit,
               so start allowing zeros now */
            qLowerLimit = 0;
            }
        inInt -= q * divisor;
        divisor /= 10;
        }
    
    /* terminate */
    intToStringBuffer[c] = '\0';
    
    return intToStringBuffer;  
    }



static void setupX11KeyMap( void );

static void xSetFullscreen( Display *inXDisplay, Window inXWindow,
                            char inToggle );


static void xSetFullscreen( Display *inXDisplay, Window inXWindow,
                            char inToggle ) {
    XEvent ev;
    Atom atom;

    ev.type = ClientMessage;
    ev.xclient.window = inXWindow;
    ev.xclient.message_type = XInternAtom( inXDisplay, "_NET_WM_STATE", False );
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = inToggle;
    atom = XInternAtom( inXDisplay, "_NET_WM_STATE_FULLSCREEN", False );
    ev.xclient.data.l[1] = (long int)atom;
    ev.xclient.data.l[2] = (long int)atom;
    XSendEvent( inXDisplay,
                DefaultRootWindow( inXDisplay ), False, ClientMessage, &ev );
    }




/*
  reconfigures based on xFullscreen and game's minimum viable screen size
*/
static void reconfigureWindowSize( Display *inXDisplay );

    

static void reconfigureWindowSize( Display *inXDisplay ) {
    if( xFullscreen ) {
        getMonitorSize( inXDisplay, &windowW, &windowH );
        }
    else {
        int monW, monH;
        int gameW, gameH;
        int smallestMult;
        
        getMonitorSize( inXDisplay, &monW, &monH );
        minginGame_getMinimumViableScreenSize( &gameW, &gameH );

        if( monW < gameW || monH < gameH ) {
            /*
            physical monitor too small for game
            have window fill monitor
            */
            windowW = monW;
            windowH = monH;
            }
        else {
            /* monitor big enough for game */
            int wMult = monW / gameW;
            int hMult = monH / gameH;

            smallestMult = wMult;
            if( hMult < smallestMult ) {
                smallestMult = hMult;
                }
            
            windowW = smallestMult * gameW;
            windowH = smallestMult * gameH;

            while( smallestMult > 1 &&
                ( windowW > ( 9 * monW ) / 10 ||
                  windowH > ( 9 * monH ) / 10 ) ) {

                /* window filling more than 90% of monitor, too big */
                smallestMult --;
                
                windowW = smallestMult * gameW;
                windowH = smallestMult * gameH;
                }
            }
        }
    
    /* make sure we're never bigger than our statically allocated
       framebuffer */
    if( windowW > MAX_WIN_W ) {
        windowW = MAX_WIN_W;
        }
    if( windowH > MAX_WIN_H ) {
        windowH = MAX_WIN_H;
        }
    mingin_log( "Window = " );
    mingin_log( intToString( windowW ) );
    mingin_log( "," );
    mingin_log( intToString( windowH ) );
    mingin_log( "\n" );
    }





int main( void );


typedef struct XWindowSetup {
        Display *xDisplay;
        Window xWindow;
        int xScreen;
        XVisualInfo *xVisual;
        GLXContext glxContext;
    } XWindowSetup;
        

/* returns 1 on success, 0 on failure */
static char openXWindow( XWindowSetup *inSetup );
static void closeXWindow( XWindowSetup *inSetup );



static char openXWindow( XWindowSetup *inSetup ) {
    long unsigned int xBlackColor;
    int glxAttributes[] = { GLX_RGBA, GLX_DOUBLEBUFFER, None };

    XWindowSetup *s = inSetup;
    
    s->xDisplay = XOpenDisplay( NULL );

    reconfigureWindowSize( s->xDisplay );

    
    s->xScreen = DefaultScreen( s->xDisplay );

    
    s->xVisual = glXChooseVisual( s->xDisplay, s->xScreen,
                                  glxAttributes );

    if( ! s->xVisual ) {
        mingin_log( "No Visual found for GLX\n" );
        XCloseDisplay( s->xDisplay );
        return 0;
        }

    xBlackColor = BlackPixel( s->xDisplay, s->xScreen );
    

    s->xWindow = XCreateSimpleWindow(
        s->xDisplay, DefaultRootWindow( s->xDisplay ),
        0, 0, (unsigned int)windowW, (unsigned int)windowH, 0,
        xBlackColor, xBlackColor );

    
    XSelectInput( s->xDisplay, s->xWindow,
                  StructureNotifyMask | KeyPressMask | KeyReleaseMask );

    XMapWindow( s->xDisplay, s->xWindow );



    s->glxContext = glXCreateContext( s->xDisplay, s->xVisual, NULL, GL_TRUE );
    
    if( ! s->glxContext ) {
        mingin_log( "Failed to create GLX context\n" );
        XDestroyWindow( s->xDisplay, s->xWindow );
        XFree( s->xVisual );
        XCloseDisplay( s->xDisplay );
        return 0;
        }


    /* wait for MapNotify */
    while( 1 ) {
        XEvent e;
        XNextEvent( s->xDisplay, &e);
        if( e.type == MapNotify ) {
            break;
            }
        }

    
    xSetFullscreen( s->xDisplay, s->xWindow, xFullscreen );

    
    glXMakeCurrent( s->xDisplay, s->xWindow, s->glxContext );

    return 1;
    }



static void closeXWindow( XWindowSetup *inSetup ) {
    XWindowSetup *s = inSetup;
    
    glXMakeCurrent( s->xDisplay, None, NULL );

    glXDestroyContext( s->xDisplay, s->glxContext );

    
    XDestroyWindow( s->xDisplay, s->xWindow );

    XCloseDisplay( s->xDisplay );
    }



        
int main( void ) {

    XWindowSetup xSetup;
    

    int b;

    char currentlyFullscreen = 0;
    xFullscreen = currentlyFullscreen;
    

    mingin_log( "Linux mingin platform starting up\n" );

    
    
    minginInternal_init();

    for( b=0; b<MGN_NUM_BUTTONS; b++ ) {
        buttonDown[b] = 0;
        buttonToXKeyMap[b] = 0;
        }

    setupX11KeyMap();
    

    if( ! openXWindow( &xSetup ) ) {
        mingin_log( "Opening X Window failed\n" );
        return 1;
        }


    
    while( ! shouldQuit ) {
        
        /* pump all events */
        while( XPending( xSetup.xDisplay ) > 0 ) {
            XEvent e;
            XNextEvent( xSetup.xDisplay, &e );

            if( e.type == KeyPress ) {
                
                KeySym ks = XLookupKeysym( &(e.xkey), 0 );
                
                MinginButton button = mapXKeyToButton( ks );
                
                if( button > MGN_BUTTON_NONE ) {
                    buttonDown[ button ] = 1;
                    /* a new press to remember */
                    lastButtonPressed = button;
                    }
                }
            else if( e.type == KeyRelease ) {
                KeySym ks = XLookupKeysym( &(e.xkey), 0 );
                
                MinginButton button = mapXKeyToButton( ks );
                
                if( button > MGN_BUTTON_NONE ) {
                    buttonDown[ button ] = 0;
                    }
                }
            }

        minginGame_step();
        
        minginGame_getScreenPixels( windowW, windowH, gameScreenBuffer );

        glRasterPos2f( -1, 1 );
        glPixelZoom( 1, -1 );
 
        glDrawPixels( (GLsizei)windowW, (GLsizei)windowH,
                      GL_RGB, GL_UNSIGNED_BYTE, gameScreenBuffer );

        glXSwapBuffers( xSetup.xDisplay, xSetup.xWindow ); 

        if( currentlyFullscreen != xFullscreen ) {
            int oldW = windowW;
            int oldH = windowH;

            reconfigureWindowSize( xSetup.xDisplay );

            if( oldW != windowW ||
                oldH != windowH ) {
                /* need to destroy and re-make window */

                mingin_log( "Calling resize window with " );
                mingin_log( intToString( windowW ) );
                mingin_log( "," );
                mingin_log( intToString( windowH ) );
                mingin_log( "\n" );

                closeXWindow( &xSetup );

                if( ! openXWindow( &xSetup ) ) {
                    mingin_log( "Failed to re-open X Window after toggling "
                                "fullscreen mode\n" );
                    return 1;
                    }
                }
            else {
                /* same window size after fullscreen toggle,
                   no need to remake it */
                xSetFullscreen( xSetup.xDisplay, xSetup.xWindow, xFullscreen );
                }


            
            currentlyFullscreen = xFullscreen;

            }
        }

    
    closeXWindow( &xSetup );
    
    
    return 1;
    }



static void setupX11KeyMap( void ) {
    buttonToXKeyMap[ MGN_KEY_BACKSPACE ] = XK_BackSpace;
    buttonToXKeyMap[ MGN_KEY_TAB ] = XK_Tab;
    buttonToXKeyMap[ MGN_KEY_RETURN ] = XK_Return;
    buttonToXKeyMap[ MGN_KEY_ESCAPE ] = XK_Escape;
    buttonToXKeyMap[ MGN_KEY_DELETE ] = XK_Delete;
    buttonToXKeyMap[ MGN_KEY_HOME ] = XK_Home;
    buttonToXKeyMap[ MGN_KEY_LEFT ] = XK_Left;
    buttonToXKeyMap[ MGN_KEY_UP ] = XK_Up;
    buttonToXKeyMap[ MGN_KEY_RIGHT ] = XK_Right;
    buttonToXKeyMap[ MGN_KEY_DOWN ] = XK_Down;
    buttonToXKeyMap[ MGN_KEY_PAGE_UP ] = XK_Page_Up;
    buttonToXKeyMap[ MGN_KEY_PAGE_DOWN ] = XK_Page_Down;
    buttonToXKeyMap[ MGN_KEY_END ] = XK_End;
    buttonToXKeyMap[ MGN_KEY_NUM_LOCK ] = XK_Num_Lock;
    buttonToXKeyMap[ MGN_KEY_F1 ] = XK_F1;
    buttonToXKeyMap[ MGN_KEY_F2 ] = XK_F2;
    buttonToXKeyMap[ MGN_KEY_F3 ] = XK_F3;
    buttonToXKeyMap[ MGN_KEY_F4 ] = XK_F4;
    buttonToXKeyMap[ MGN_KEY_F5 ] = XK_F5;
    buttonToXKeyMap[ MGN_KEY_F6 ] = XK_F6;
    buttonToXKeyMap[ MGN_KEY_F7 ] = XK_F7;
    buttonToXKeyMap[ MGN_KEY_F8 ] = XK_F8;
    buttonToXKeyMap[ MGN_KEY_F9 ] = XK_F9;
    buttonToXKeyMap[ MGN_KEY_F10 ] = XK_F10;
    buttonToXKeyMap[ MGN_KEY_F11 ] = XK_F11;
    buttonToXKeyMap[ MGN_KEY_F12 ] = XK_F12;
    buttonToXKeyMap[ MGN_KEY_F13 ] = XK_F13;
    buttonToXKeyMap[ MGN_KEY_F14 ] = XK_F14;
    buttonToXKeyMap[ MGN_KEY_F15 ] = XK_F15;
    buttonToXKeyMap[ MGN_KEY_F16 ] = XK_F16;
    buttonToXKeyMap[ MGN_KEY_F17 ] = XK_F17;
    buttonToXKeyMap[ MGN_KEY_F18 ] = XK_F18;
    buttonToXKeyMap[ MGN_KEY_F19 ] = XK_F19;
    buttonToXKeyMap[ MGN_KEY_F20 ] = XK_F20;
    buttonToXKeyMap[ MGN_KEY_F21 ] = XK_F21;
    buttonToXKeyMap[ MGN_KEY_F22 ] = XK_F22;
    buttonToXKeyMap[ MGN_KEY_F23 ] = XK_F23;
    buttonToXKeyMap[ MGN_KEY_F24 ] = XK_F24;
    buttonToXKeyMap[ MGN_KEY_F25 ] = XK_F25;
    buttonToXKeyMap[ MGN_KEY_F26 ] = XK_F26;
    buttonToXKeyMap[ MGN_KEY_F27 ] = XK_F27;
    buttonToXKeyMap[ MGN_KEY_F28 ] = XK_F28;
    buttonToXKeyMap[ MGN_KEY_F29 ] = XK_F29;
    buttonToXKeyMap[ MGN_KEY_F30 ] = XK_F30;
    buttonToXKeyMap[ MGN_KEY_F31 ] = XK_F31;
    buttonToXKeyMap[ MGN_KEY_F32 ] = XK_F32;
    buttonToXKeyMap[ MGN_KEY_F33 ] = XK_F33;
    buttonToXKeyMap[ MGN_KEY_F34 ] = XK_F34;
    buttonToXKeyMap[ MGN_KEY_F35 ] = XK_F35;
    buttonToXKeyMap[ MGN_KEY_SHIFT_L ] = XK_Shift_L;
    buttonToXKeyMap[ MGN_KEY_SHIFT_R ] = XK_Shift_R;
    buttonToXKeyMap[ MGN_KEY_CONTROL_L ] = XK_Control_L;
    buttonToXKeyMap[ MGN_KEY_CONTROL_R ] = XK_Control_R;
    buttonToXKeyMap[ MGN_KEY_CAPS_LOCK ] = XK_Caps_Lock;
    buttonToXKeyMap[ MGN_KEY_META_L ] = XK_Meta_L;
    buttonToXKeyMap[ MGN_KEY_META_R ] = XK_Meta_R;
    buttonToXKeyMap[ MGN_KEY_ALT_L ] = XK_Alt_L;
    buttonToXKeyMap[ MGN_KEY_ALT_R ] = XK_Alt_R;
    buttonToXKeyMap[ MGN_KEY_SUPER_L ] = XK_Super_L;
    buttonToXKeyMap[ MGN_KEY_SUPER_R ] = XK_Super_R;
    buttonToXKeyMap[ MGN_KEY_SPACE ] = XK_space;
    buttonToXKeyMap[ MGN_KEY_EXCLAMATION ] = XK_exclam;
    buttonToXKeyMap[ MGN_KEY_DOUBLE_QUOTE ] = XK_quotedbl;
    buttonToXKeyMap[ MGN_KEY_NUMBER_SIGN ] = XK_numbersign;
    buttonToXKeyMap[ MGN_KEY_DOLLAR ] = XK_dollar;
    buttonToXKeyMap[ MGN_KEY_PERCENT ] = XK_percent;
    buttonToXKeyMap[ MGN_KEY_AMPERSAND ] = XK_ampersand;
    buttonToXKeyMap[ MGN_KEY_APOSTROPHE ] = XK_apostrophe;
    buttonToXKeyMap[ MGN_KEY_PAREN_L ] = XK_parenleft;
    buttonToXKeyMap[ MGN_KEY_PAREN_R ] = XK_parenright;
    buttonToXKeyMap[ MGN_KEY_ASTERISK ] = XK_asterisk;
    buttonToXKeyMap[ MGN_KEY_PLUS ] = XK_plus;
    buttonToXKeyMap[ MGN_KEY_COMMA ] = XK_comma;
    buttonToXKeyMap[ MGN_KEY_MINUS ] = XK_minus;
    buttonToXKeyMap[ MGN_KEY_PERIOD ] = XK_period;
    buttonToXKeyMap[ MGN_KEY_SLASH ] = XK_slash;
    buttonToXKeyMap[ MGN_KEY_0 ] = XK_0;
    buttonToXKeyMap[ MGN_KEY_1 ] = XK_1;
    buttonToXKeyMap[ MGN_KEY_2 ] = XK_2;
    buttonToXKeyMap[ MGN_KEY_3 ] = XK_3;
    buttonToXKeyMap[ MGN_KEY_4 ] = XK_4;
    buttonToXKeyMap[ MGN_KEY_5 ] = XK_5;
    buttonToXKeyMap[ MGN_KEY_6 ] = XK_6;
    buttonToXKeyMap[ MGN_KEY_7 ] = XK_7;
    buttonToXKeyMap[ MGN_KEY_8 ] = XK_8;
    buttonToXKeyMap[ MGN_KEY_9 ] = XK_9;
    buttonToXKeyMap[ MGN_KEY_COLON ] = XK_colon;
    buttonToXKeyMap[ MGN_KEY_SEMICOLON ] = XK_semicolon;
    buttonToXKeyMap[ MGN_KEY_LESS ] = XK_less;
    buttonToXKeyMap[ MGN_KEY_EQUAL ] = XK_equal;
    buttonToXKeyMap[ MGN_KEY_GREATER ] = XK_greater;
    buttonToXKeyMap[ MGN_KEY_QUESTION ] = XK_question;
    buttonToXKeyMap[ MGN_KEY_AT_SIGN ] = XK_at;
    buttonToXKeyMap[ MGN_KEY_BRACKET_L ] = XK_bracketleft;
    buttonToXKeyMap[ MGN_KEY_BACKSLASH ] = XK_backslash;
    buttonToXKeyMap[ MGN_KEY_BRACKET_R ] = XK_bracketright;
    buttonToXKeyMap[ MGN_KEY_CIRCUMFLEX ] = XK_asciicircum;
    buttonToXKeyMap[ MGN_KEY_UNDERSCORE ] = XK_underscore;
    buttonToXKeyMap[ MGN_KEY_BACK_TICK ] = XK_grave;
    buttonToXKeyMap[ MGN_KEY_A ] = XK_a;
    buttonToXKeyMap[ MGN_KEY_B ] = XK_b;
    buttonToXKeyMap[ MGN_KEY_C ] = XK_c;
    buttonToXKeyMap[ MGN_KEY_D ] = XK_d;
    buttonToXKeyMap[ MGN_KEY_E ] = XK_e;
    buttonToXKeyMap[ MGN_KEY_F ] = XK_f;
    buttonToXKeyMap[ MGN_KEY_G ] = XK_g;
    buttonToXKeyMap[ MGN_KEY_H ] = XK_h;
    buttonToXKeyMap[ MGN_KEY_I ] = XK_i;
    buttonToXKeyMap[ MGN_KEY_J ] = XK_j;
    buttonToXKeyMap[ MGN_KEY_K ] = XK_k;
    buttonToXKeyMap[ MGN_KEY_L ] = XK_l;
    buttonToXKeyMap[ MGN_KEY_M ] = XK_m;
    buttonToXKeyMap[ MGN_KEY_N ] = XK_n;
    buttonToXKeyMap[ MGN_KEY_O ] = XK_o;
    buttonToXKeyMap[ MGN_KEY_P ] = XK_p;
    buttonToXKeyMap[ MGN_KEY_Q ] = XK_q;
    buttonToXKeyMap[ MGN_KEY_R ] = XK_r;
    buttonToXKeyMap[ MGN_KEY_S ] = XK_s;
    buttonToXKeyMap[ MGN_KEY_T ] = XK_t;
    buttonToXKeyMap[ MGN_KEY_U ] = XK_u;
    buttonToXKeyMap[ MGN_KEY_V ] = XK_v;
    buttonToXKeyMap[ MGN_KEY_W ] = XK_w;
    buttonToXKeyMap[ MGN_KEY_X ] = XK_x;
    buttonToXKeyMap[ MGN_KEY_Y ] = XK_y;
    buttonToXKeyMap[ MGN_KEY_Z ] = XK_z;
    buttonToXKeyMap[ MGN_KEY_BRACE_L ] = XK_braceleft;
    buttonToXKeyMap[ MGN_KEY_VERTICAL_BAR ] = XK_bar;
    buttonToXKeyMap[ MGN_KEY_BRACE_R ] = XK_braceright;
    buttonToXKeyMap[ MGN_KEY_TILDE ] = XK_asciitilde;
    }





/* end #ifdef __linux__ */
#endif




/* end #ifdef MINGIN_IMPLEMENTATION */
#endif
