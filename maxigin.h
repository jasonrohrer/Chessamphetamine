#include "mingin.h"

/*
  Maxigin: a maximally portable, 100% pure C89, platform-independent,
           single-player video game engine with only one dependency (mingin.h)
           
           by Jason Rohrer
          
  This work is not copyrighted.  I place it into the public domain.

  
  ===================================================
  Table of Contents                    [jumpContents]
  ===================================================

  Jump to a section by searching for the corresponding [keyString]

  -- How to compile                    [jumpCompile]

  -- Static settings                   [jumpSettings] 

  -- Why Maxigin?                      [jumpWhy]

  -- How to make a Maxigin game        [jumpGame]

  -- What Maxigin provides             [jumpMaxigin]

  -- Maxigin internal code             [jumpInternal]

*/



/*
  ==================================================
  How to compile                       [jumpCompile]
  ==================================================
  
  Include in your C code wherever like so:

  #include "maxigin.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:
  
      #define MINGIN_IMPLEMENTATION
  
      #define MAXIGIN_IMPLEMENTATION
  
      #include "maxigin.h"

  Maxigin implements the four functions needed by a Mingin game
  (see mingin.h), so your game shouldn't implement these directly.

  You can still include mingin.h in your code and use the provided functions
  directly (see [jumpMingin] in mingin.h for a list of functions).

*/



#ifndef MAXIGIN_H_INCLUDED
#define MAXIGIN_H_INCLUDED



/*
  ===================================================
  Static settings                      [jumpSettings]
  ===================================================

  The following compile-time settings can be changed for each game that
  is built against Maxigin.

  If these are defined in your C file before #define MAXIGIN_IMPLEMENTATION
  your settings will be used instead
  
*/


/*
  The game image's native width and height.  This is the ideal size for
  the game's pixel content.
*/
#ifndef MAXIGIN_GAME_NATIVE_W
    #define MAXIGIN_GAME_NATIVE_W  640
#endif
#ifndef MAXIGIN_GAME_NATIVE_H
    #define MAXIGIN_GAME_NATIVE_H  480
#endif





/*
  ==============================================
  Why Maxigin?                         [jumpWhy]
  ==============================================
  
  Maxigin is a single C89 include file that provides platform-independent
  infrastructure and common utility functions for games built on top
  of the Mingin platform.

  I'm making multiple games on top of mingin, and this header file collects
  all of that common code.

  Why not just make all this part of mingin.h?  Because I don't want
  to force you to use it.  If you want to use mingin.h for cross-platform
  portability only, and do things your own way, you're free to do that.

  My goal is to make this code ridiculously portable:  100% C89, with absolutely
  no includes or dependences beyond mingin.h

  For maxium portability, along with being strict C89, this code plays by
  the following rules:

  --No includes at all, beyond mingin.h, which itself makes no includes,
    except for platform-specific system includes.

  --Since there are no includes, Maxigin obeys the following constraints:

    --None of the C Standard Library functions or macros are used, not even the
      "freestanding" subset.  Maxigin never calls malloc, free, fopen, printf,
      assert, etc., and it doesn't expect uint64 to exist.
      If you want to use these in your game, you're free to do so,
      but Maxigin doesn't force your game to depend on these.

    --No dynamic memory allocation.

    --No explicit filesystem access (mingin functions are used for bulk data
      access, logging, persistent data storage, and so on).

  --No floating point types or math operations.

  --No assumptions about the size of int or long or short.

  --No platform-dependent code surrounded by #ifdefs.  The same code compiles
    and runs on every platform.

  --No program entry point (beyond whatever entry point is provided by
    the platform-specific code in mingin.h)
    
*/




/*
  ===============================================
  How to make a Maxigin game           [jumpGame]
  ===============================================
  
  The game itself must implement these three functions:

      void maxiginGame_init();
      
      void maxiginGame_step();

      void maxiginGame_getNativePixels( unsigned char *inRGBBuffer );
  
  Each function is tagged below with   [jumpGameRequired]
  
  These are called by the engine, and the engine is in charge
  of doing any necessary image scaling to meet the needs of the mingin platform.

  Only maxiginGame_init and maxiginGame_step are guaranteed to be called.

  maxiginGame_getNativePixels might not be called on every platform
  (for example, in a headless test mode).

  maxiginGame_init is guaranteed to be called exactly once, befor any other
  maxiginGame_ function is called.
  
  minginGame_step is guaranteed to be called at least once, before
  any calls to maxiginGame_getNativePixels.
  
  Beyond that, the order and frequency of these function calls are not
  guaranteed.
  
  The only guarantee is that these functions will never be called concurrently.
*/



/*
  Initialize the game.

  This is called exactly once before any other maxiginGame_ calls are made.

  ****
  This is the only maxiginGame_ function where maxigin_init functions can
  be called.
  ****
  
  ****
  All general-purpose maxigin_ functions provided by mingin.h CAN be called
  from this function.
  ****
  
  ****
  All mingin_ functions provided by mingin.h CAN be called from this function.
  ****

  [jumpGameRequired]
*/
void maxiginGame_init( void );
  


/*
  Steps to the next time frame of the game.
  
  Note that beyond maxiginGame_init, this is the ONLY function of a game
  that Maxigin must actually call, so all game logic must be executed by
  this function.
  
  The game must not depend on being asked for pixels to advance things.

  The rate at which maxigin_gameStep is called can be found by calling

      mingin_getStepsPerSecond()

  ****
  Do not call maxigin-provided "maxigin_init" functions from this function.
  ****
  
  ****
  All general-purpose maxigin_ functions provided by mingin.h CAN be called
  from this function.
  ****
  
  ****
  All mingin_ functions provided by mingin.h CAN be called from this function.
  ****

  Will be called at least once, after maxiginGame_init and before any
  calls to maxiginGame_getNativePixels.
  
  [jumpGameRequired]
*/
void maxiginGame_step( void );



/*
  Get the next native image full pixels
  Each R, G, and B color component is 8 bytes, interleaved in
  RGBRGBRGB... in row-major order, starting from the top left corner
  of the screen and going left-to-right and top-to-bottom, with
  
      MAXIGIN_GAME_NATIVE_H * MAXIGIN_GAME_NATIVE_W
      
  total pixels.
  
  ****
  Do not call maxigin-provided "maxigin_" functions from this function.
  ****
  
  ****
  Do not call mingin-provided "mingin_" functions from this function.
  ****
  
  Will not necessarily be called.

  If called, it will be called after one call to maxiginGame_init() and
  at least one call to maxiginGame_step)_

  [jumpGameRequired]
*/
void maxiginGame_getNativePixels( unsigned char *inRGBBuffer );





/*
  ==================================================
  What Maxigin provides                [jumpMaxigin]
  ==================================================
  
  Maxigin provides two sets functions.

  The first set of init functions can ONLY be called from inside maxiginGame_init

  Each init function is tagged with   [jumpMaxiginInit]

  The second set of general functions can be called from either
  maxiginGame_init or maxiginGame_step.

  Each general function is tagged with  [jumpMaxiginGeneral]

  ****
  Calling these functions from the wrong function might result in unexpected
  (though safe) behavior, and will result in an error message being written
  to the log.
  ****
*/










/*
  ===================================================
  Maxigin internal code                [jumpInternal]
  ===================================================

  The rest of this file contains Maxigin's internal implementation.

*/



/*
  Maxigin listens to certain buttons for its own functionality that's not
  game-specific (like accessing a settings screen, toggling fullscreen, etc.)

  If a game registers button handles directly with mingin_registerButtonMapping,
  those registrations may clobber Maxigin's registrations.

  We redefine mingin_registerButtonMapping below, so that c files that
  include maxigin.h call the maxigin version of this function instead.

  See mingin_registerButtonMapping in mingin.h for full documentation.
*/
char maxiginInternal_registerButtonMapping( int inButtonHandle,
                                            const MinginButton inMapping[] );



/*
  Check whether a previously-mapped button handle is currently held down.
  Returns 1 if pressed, 0 if not pressed.

  Note that Maxigin might block reporting of certain buttons being down
  depending on its own internal functionality.

  We redefine mingin_isButtonDown below, so that c files that
  include maxigin.h call the maxigin version of this function instead.
*/
char maxiginInternal_isButtonDown( int inButtonHandle );





#ifdef MAXIGIN_IMPLEMENTATION





enum MaxiginUserAction {
    QUIT,
    FULLSCREEN_TOGGLE,
    LAST_MAXIGIN_USER_ACTION
    };




static char initDone = 0;

static char fullscreenTogglePressed = 0;



/* RGB pixels of game's native image size */
static unsigned char gameImageBuffer[ MAXIGIN_GAME_NATIVE_W *
                                      MAXIGIN_GAME_NATIVE_H * 3 ];



void minginGame_getMinimumViableScreenSize( int *outWide, int *outHigh ) {
    *outWide = MAXIGIN_GAME_NATIVE_W;
    *outHigh = MAXIGIN_GAME_NATIVE_H;
    }



void minginGame_getScreenPixels( int inWide, int inHigh,
                                 unsigned char *inRGBBuffer ) {
    int numPixels = inWide * inHigh;
    int numPixelBytes = numPixels * 3;
    int p;
    int x, y;
    
    int scaleFactor;
    int scaleW, scaleH;
    int scaledGameW, scaledGameH;
    int offsetX, offsetY;

    maxiginGame_getNativePixels( gameImageBuffer );

    scaleW = inWide /  MAXIGIN_GAME_NATIVE_W;

    scaleH = inHigh / MAXIGIN_GAME_NATIVE_H;

    scaleFactor = scaleW;

    if( scaleH < scaleFactor ) {
        scaleFactor = scaleH;
        }

    if( scaleFactor < 1 ) {
        /* screen isn't big enough for our native size?
           let it just get cut off for now */
        scaleFactor = 1;
        }
    
    scaledGameW = scaleFactor * MAXIGIN_GAME_NATIVE_W;
    scaledGameH = scaleFactor * MAXIGIN_GAME_NATIVE_H;


    /* pick offsets to center in screen, if possible */
    offsetX = 0;
    offsetY = 0;

    if( scaledGameW < inWide ) {
        offsetX = ( inWide - scaledGameW ) / 2;
        }
    if( scaledGameH < inHigh ) {
        offsetY = ( inHigh - scaledGameH ) / 2;
        }
    

    if( offsetX > 0 || offsetY > 0 ) {
        /* black background beyond edges of our centered image */
        for( p = 0; p<numPixelBytes; p++ ) {
            inRGBBuffer[p] = 0;
            }
        }

    /* naive nearest neighbor scaling */
    for( y = offsetY; y < offsetY + scaledGameH; y++ ) {
        int rowStartDest = y * inWide * 3;
        int ySrcScaled = y - offsetY;
        int ySrcOrig = ySrcScaled /  scaleFactor;
        
        int rowStartSrcOrig = ySrcOrig * MAXIGIN_GAME_NATIVE_W * 3;
        
        for( x = offsetX; x < offsetX + scaledGameW; x++ ) {
            int xSrcScaled = x - offsetX;
            int xSrcOrig = xSrcScaled /  scaleFactor;
        
            int pixDest = rowStartDest + x * 3;
            int pixSrcOrig = rowStartSrcOrig + xSrcOrig * 3;

            inRGBBuffer[ pixDest ] = gameImageBuffer[ pixSrcOrig ];
            inRGBBuffer[ pixDest + 1 ] = gameImageBuffer[ pixSrcOrig + 1 ];
            inRGBBuffer[ pixDest + 2 ] = gameImageBuffer[ pixSrcOrig + 2 ];
            }
        }
    }


static void gameInit( void );


void minginGame_step( char inFinalStep ) {

    if( inFinalStep ) {
        /* no clean-up needed so far */
        return;
        }
    
    if( ! initDone ) {
        gameInit();
        initDone = 1;
        }
    
    if( mingin_isButtonDown( QUIT ) ) {
        mingin_log( "Got quit key\n" );
        
        mingin_quit();
        return;
        }
    
    if( mingin_isButtonDown( FULLSCREEN_TOGGLE ) ) {

        if( ! fullscreenTogglePressed ) {
            fullscreenTogglePressed = 1;

            mingin_toggleFullscreen( ! mingin_isFullscreen() );
            }
        }
    else {
        fullscreenTogglePressed = 0;
        }

    maxiginGame_step();
    }



static MinginButton quitMapping[] = { MGN_KEY_Q, MGN_KEY_ESCAPE, MGN_MAP_END };
static MinginButton fullscreenMapping[] = { MGN_KEY_F, MGN_MAP_END };

static void gameInit( void ) {
    
    mingin_registerButtonMapping( QUIT, quitMapping );
    mingin_registerButtonMapping( FULLSCREEN_TOGGLE, fullscreenMapping );
    
    maxiginGame_init();
    }




char maxiginInternal_registerButtonMapping( int inButtonHandle,
                                            const MinginButton inMapping[] ) {

    /* push it up so it doesn't interfere with our mappings */
    inButtonHandle += LAST_MAXIGIN_USER_ACTION;

    return mingin_registerButtonMapping( inButtonHandle, inMapping );
    }



char maxiginInternal_isButtonDown( int inButtonHandle ) {
    
    /* push it up so it doesn't interfere with our mappings */
    inButtonHandle += LAST_MAXIGIN_USER_ACTION;

    return mingin_isButtonDown( inButtonHandle );
    }




/* end #ifdef MAXIGIN_IMPLEMENTATION */
#endif



/*
  Make sure our versions of these functions get called,
  so that our internal button mappings aren't clobbered.
*/
#define mingin_registerButtonMapping  maxiginInternal_registerButtonMapping
#define mingin_isButtonDown           maxiginInternal_isButtonDown




/* end of #ifndef MAXIGIN_H_INCLUDED */
#endif