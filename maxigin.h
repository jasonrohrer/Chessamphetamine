/*
  Maxigin: a maximally portable 100% pure C89 platform-independent
           single-player video game engine with only one dependency (mingin.h)
           
           by Jason Rohrer
          
  This work is not copyrighted.  I place it into the public domain.

  
  ===================================================
  Table of Contents                    [jumpContents]
  ===================================================

  Jump to a section by searching for the corresponding [keyString]

  -- How to compile                    [jumpCompile]

  -- Static setting                    [jumpSettings] 

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

  #define MAXIGIN_IMPLEMENTATION
  #include "maxigin.h"


  Maxigin implements the four functions needed by a Mingin game
  (see mingin.h), so your game shouldn't implement these directly.

  You can still include maxigin.h in your code and use the provided functions
  directly (see [jumpMingin] in mingin.h for a list of functions).

  Maxigin also handles the #define MINGIN_IMPLEMENTATION needed to compile
  the mingin.h implementation code.

*/



/*
  ===================================================
  Static settings                      [jumpSettings]
  ===================================================

  The following compile-time settings can be changed for each game that
  is build against Maxigin.
  
*/



/*
  The game image's native width and height.  This is the ideal size for
  the game's pixel content.
*/
#define MAXIGIN_GAME_NATIVE_W  640
#define MAXIGIN_GAME_NATIVE_H  480





/*
  ==============================================
  Why Maxigin?                         [jumpWhy]
  ==============================================
  
  Maxigin is a single C89 include file that provides platform-independent
  infrastructure and common utility functions for games built on top
  of the Mingin platform.

  I'm making multiple games on top of mingin, and this header file collects
  all of that common code.

  This code isn't just baked into mingin.h, though, because I don't want
  to force you to use it.  If you want to use mingin.h for cross-platform
  portability only, and do things your own way, you're free to do that.
  

  My goal is to make this code ridiculously portable:  100% C89, with absolutely
  no includes or dependences beyond mingin.h

  For maxium portability, along with being strict C89, this code plays by
  the following rules:

  --No includes at all, beyond mingin.h, which itself makes no includes,
    except for platform-specific system includes.

  --Since there are no includes, Maxigin obeys the following constraints:

    --None of the C Standard Library functions are used, not even the
      "freestanding" subset.  Maxigin never calls malloc, free, fopen, printf,
      assert, etc.  If you want to use these in your game, you're free to do so,
      but Mingin doesn't force your game to depend on these.

    --No dynamic memory allocation.

    --No filesystem access (mingin functions are used for bulk data access,
      loggin, persistent data storage, and so on).

  --No floating point types or math.

  --No platform-dependent code surrounded by #ifdefs.  The same code compiles
    and runs on every platform.

  --No program entry point (beyond whatever entry point is provided by mingin.h)
    
*/




/*
  ===============================================
  How to make a Maxigin game            [jumpGame]
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
  This is the ONLY maxiginGame_function where you can call
  mingin-provided "mingin_" functions.
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
  Do not call mingin-provided "mingin_" functions from this function.
  ****
  
  ****
  Do not call maxigin-provided "maxigin_" functions from this function.
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
  (though safe) behavior, and will result in an error message will be written
  to the log.
  ****
*/






#ifdef MAXIGIN_IMPLEMENTATION

/*
  ===================================================
  Maxigin internal code                [jumpInternal]
  ===================================================

  The rest of this file contains Maxigin's internal implementation.

*/


#define MINGIN_IMPLEMENTATION

#include "mingin.h"




#endif