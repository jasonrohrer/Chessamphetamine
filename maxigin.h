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
  Registers an area of static memory to be managed by Maxigin's hot-reloading
  system.

      inPointer       the start of the static memory area.
  
      inNumBytes      the length of the memory are.

      inDescription   a unique descriptor string
  
  [jumpMaxiginInit]
*/
void maxigin_initRegisterStaticMemory( void *inPointer, int inNumBytes,
                                       const char *inDescription );



/*
  During init, after registering all static memory of interest with
  maxigin_initRegisterStaticMemory, call this to potentially restore
  the contents of those memory locations from the last run.

  If the registered descriptions and sizes have changed since last run
  (due to a code change), this call has no effect.

  [jumpMaxiginInit]
*/
void maxigin_initRestoreStaticMemoryFromLastRun( void );






/*
  CONSIDER:

  Can we actually RECORD the state of this static memory to persistent storage
  after every step (storing a no-op if it hasn't changed, or maybe only storing
  a sparse map of which bytes have changed....)  and then use that to do
  full game playback, including smooth rewinding and time jumping forward
  and back?

  The old game engine recorded input events, the system, clock, etc, and then
  let the game code replay itself, which is useful for catching bugs, but also
  fragile, and doesn't allow time-jumping, which made it too much of a pain
  to use (if someone submitted a long game with a bug a the end, I didn't
  have time to watch the whole thing).

  What if this system allows fast forward and rewind, and then pause...

  And then you can see a bug happen (or it will just end, if there was a crash).

  Then you can rewind to the frame before the bug, and then "unleash" it
  to run on the real game code again, but stop playing back, and then
  try to trigger the bug yourself.  I.e., you get to the point right before
  the bug, and then you take the reigns by clicking and pressing buttons.
*/




/*
  Converts an int into a \0-terminated string.
  
  Returns a static buffer that must be used before next call to intToString.
*/
const char *maxigin_intToString( int inInt );



/*
  Converts a \0-terminated string to an int.
*/
int maxigin_stringToInt( const char *inString );




/*
  Logs a labeled int value to the game engine log.

  [jumpMaxiginGeneral]
*/
void maxigin_logInt( const char *inLabel, int inVal );



/*
  Gets the length of a string.

  [jumpMaxiginGeneral]
*/
int maxigin_stringLength( const char *inString );



/*
  Returns 1 if two strings are equal, 0 if not.

  [jumpMaxiginGeneral]
*/
char maxigin_equal( const char *inStringA, const char *inStringB );




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


static char areWeInMaxiginGameInitFunction = 0;

static char areWeInMaxiginGameStepFunction = 0;



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

static void saveGame( void );


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

        saveGame();
        
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

    areWeInMaxiginGameStepFunction = 1;
    
    maxiginGame_step();
    
    areWeInMaxiginGameStepFunction = 0;
    }



static MinginButton quitMapping[] = { MGN_KEY_Q, MGN_KEY_ESCAPE, MGN_MAP_END };
static MinginButton fullscreenMapping[] = { MGN_KEY_F, MGN_MAP_END };

static void gameInit( void ) {
    
    mingin_registerButtonMapping( QUIT, quitMapping );
    mingin_registerButtonMapping( FULLSCREEN_TOGGLE, fullscreenMapping );

    areWeInMaxiginGameInitFunction = 1;
    
    maxiginGame_init();

    areWeInMaxiginGameInitFunction = 0;
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




typedef struct MaxiginMemRec {
        void *pointer;
        int numBytes;
        const char *description;
    } MaxiginMemRec;


#define MAXIGIN_MAX_MEM_RECORDS 1024

static MaxiginMemRec memRecords[ MAXIGIN_MAX_MEM_RECORDS ];

static int numMemRecords = 0;


void maxigin_initRegisterStaticMemory( void *inPointer, int inNumBytes,
                                       const char *inDescription ) {
    if( numMemRecords >= MAXIGIN_MAX_MEM_RECORDS ) {
        maxigin_logInt( "Game tried to register more than max memory records: ",
                        MAXIGIN_MAX_MEM_RECORDS );
        return;
        }
    memRecords[ numMemRecords ].pointer = inPointer;
    memRecords[ numMemRecords ].numBytes = inNumBytes;
    memRecords[ numMemRecords ].description = inDescription;

    numMemRecords++;
    }


static const char *saveGameFileName = "save.bin";


#define MAXIGIN_MAX_FINGERPRINT_LENGTH  256
static char fingerprintBuffer[ MAXIGIN_MAX_FINGERPRINT_LENGTH ];

static char *getMemRecordsFingerprint( int *outTotalMemBytes ) {
    int i, c;
    int totalNumBytes = 0;
    
    for( c=0; c<MAXIGIN_MAX_FINGERPRINT_LENGTH; c++ ) {
        fingerprintBuffer[c] = '\0';
        }
    for( i=0; i<numMemRecords; i++ ) {
        totalNumBytes += memRecords[i].numBytes;

        c=0;
        while( memRecords[ i ].description[c] != '\0' &&
               c < MAXIGIN_MAX_FINGERPRINT_LENGTH - 1 ) {
            fingerprintBuffer[c] =
                memRecords[ i ].description[c] ^ fingerprintBuffer[c];
            }
        }
    *outTotalMemBytes = totalNumBytes;

    return fingerprintBuffer;
    }



#define MAXIGIN_FILE_BUFFER_SIZE 1024
static unsigned char maxiginFileBuffer[ MAXIGIN_FILE_BUFFER_SIZE ];


/*
  Reads a \0-terminated string from data store.

  inMaxBytes is the length of the buffer, including the \0 byte for termination.

  Returns 1 on success, 0 on failure.
*/
static char readStringFromPersistData( int inStoreReadHandle,
                                       int inMaxBytes,
                                       char *inBuffer ) {
    int i = 0;

    int readNum = mingin_readPersistData( inStoreReadHandle,
                                          1,
                                          (unsigned char *)&( inBuffer[i] ) );
    while( readNum == 1 &&
           i < inMaxBytes - 1 &&
           inBuffer[i] != '\0' ) {
        i++;
        readNum = mingin_readPersistData( inStoreReadHandle,
                                          1,
                                          (unsigned char *)&( inBuffer[i] ) );
        }
    if( inBuffer[i] != '\0' && readNum == 1 ) {
        /* didn't find termination in file
           because string was too long for buffer */
        mingin_log( "Error:  Buffer overflow when trying to read string from "
                    "persistent data store.\n" );
        return 0;
        }
    else if( inBuffer[i] != '\0' && readNum == 0 ) {
        mingin_log( "Error:  Reached end of store when trying to read string "
                    "from persistent data store.\n" );
        return 0;
        }

    return 1;
    }


static void saveGame( void ) {
    char *fingerprint;
    int numTotalBytes;
    const char *intString;
    int i;
    
    int outHandle = mingin_startWritePersistData( saveGameFileName );

    if( outHandle == -1 ) {
        mingin_log( "Failed to open saved game for writing: " );
        mingin_log( saveGameFileName );
        mingin_log( "\n" );
        
        return;
        }

    fingerprint = getMemRecordsFingerprint( &numTotalBytes );

    /* fixme:  make function to write int to data store as string */
    intString = maxigin_intToString( numTotalBytes );
    
    
    mingin_writePersistData( outHandle,
                             maxigin_stringLength( intString ) + 1,
                             (unsigned char*)intString );

    
    intString = maxigin_intToString( numMemRecords );
    
    mingin_writePersistData( outHandle,
                             maxigin_stringLength( intString ) + 1,
                             (unsigned char*)intString );
    
    mingin_writePersistData( outHandle,
                             maxigin_stringLength( fingerprint ) + 1,
                             (unsigned char*)fingerprint );

    for( i=0; i<numMemRecords; i++ ) {
        const char *des = memRecords[i].description;
        
        intString = maxigin_intToString( memRecords[i].numBytes );
    
        mingin_writePersistData( outHandle,
                             maxigin_stringLength( des ) + 1,
                             (unsigned char*)des );
        
        mingin_writePersistData( outHandle,
                             maxigin_stringLength( intString ) + 1,
                             (unsigned char*)intString );

        /* write numBytes from memory location into storage */
        mingin_writePersistData( outHandle,
                                 memRecords[i].numBytes,
                                 (unsigned char*)memRecords[i].pointer );
        }

    mingin_endWritePersistData( outHandle );
    }



void maxigin_initRestoreStaticMemoryFromLastRun( void ) {
    char *fingerprint;
    int numTotalBytes;
    char success;
    const char *intString;
    /*
    int i;*/
    int storeSize;
    
    int readHandle = mingin_startReadPersistData( saveGameFileName,
                                                  &storeSize );

    if( readHandle == -1 ) {
        mingin_log( "Failed to open saved game for reading: " );
        mingin_log( saveGameFileName );
        mingin_log( "\n" );
        
        return;
        }

    fingerprint = getMemRecordsFingerprint( &numTotalBytes );

    /* fixme:  make function to read int from file as string and convert
       to int */
    success = readStringFromPersistData( readHandle,
                                         MAXIGIN_FILE_BUFFER_SIZE,
                                         (char*)maxiginFileBuffer );

    if( ! success ) {
        mingin_endReadPersistData( readHandle );
        mingin_log( "Failed to read total num bytes from save file.\n" );
        return;
        }

    intString = maxigin_intToString( numTotalBytes );

    if( ! maxigin_equal( intString, (char*)maxiginFileBuffer ) ) {
        mingin_endReadPersistData( readHandle );
        mingin_log( "Save file does not match current total memory bytes, "
                    "ignoring.\n" );
        return;
        }

    success = readStringFromPersistData( readHandle,
                                         MAXIGIN_FILE_BUFFER_SIZE,
                                         (char*)maxiginFileBuffer );
    if( ! success ) {
        mingin_endReadPersistData( readHandle );
        mingin_log( "Failed to read fingerprint from save file.\n" );
        return;
        }

    if( ! maxigin_equal( fingerprint, (char*)maxiginFileBuffer ) ) {
        mingin_endReadPersistData( readHandle );
        mingin_log( "Save file does not match current memory fingerprint, "
                    "ignoring.\n" );
        return;
        }
    
    }




#define MAXIGIN_LOG_INT_MAX_LENGTH  256

static char logIntBuffer[ MAXIGIN_LOG_INT_MAX_LENGTH ];

void maxigin_logInt( const char *inLabel, int inVal ) {
    const char *valString = maxigin_intToString( inVal );
    int i = 0;

    while( i < MAXIGIN_LOG_INT_MAX_LENGTH - 2 &&
           inLabel[i] != '\0' ) {
        logIntBuffer[i] = inLabel[i];
        i++;
        }
    while( i < MAXIGIN_LOG_INT_MAX_LENGTH - 2 &&
           valString[i] != '\0' ) {
        logIntBuffer[i] = valString[i];
        i++;
        }
    logIntBuffer[i] = '\n';
    logIntBuffer[i+1] = '\0';

    mingin_log( logIntBuffer );
    }



int maxigin_stringLength( const char *inString ) {
    int len = 0;
    while( inString[len] != '\0' ) {
        len++;
        }
    return len;
    }



char maxigin_equal( const char *inStringA, const char *inStringB ) {
    int i = 0;
    while( inStringA[i] == inStringB[i] &&
           inStringA[i] != '\0' ) {
        i++;
        }

    if( inStringA[i] == inStringB[i] ) {
        /* both terminated */
        return 1;
        }
    return 0;
    }






static char intToStringBuffer[20];


const char *maxigin_intToString( int inInt ) {
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


int maxigin_stringToInt( const char *inString ) {
    /* fixme:  implement */
    if( inString[0] == '\0' ) {
        return 0;
        }
    return 0;
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