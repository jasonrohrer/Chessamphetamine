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

  [jumpMaxiginGeneral]
*/
const char *maxigin_intToString( int inInt );



/*
  Converts a \0-terminated string to an int.

  [jumpMaxiginGeneral]
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
  Generates a ASCII hex-encoding of a string, in uppercase hex.

  inHexBuffer must have room for at least 2 * inNumBytes + 1 characters.

  [jumpMaxiginGeneral]
*/
void maxigin_hexEncode( const unsigned char *inBytes, int inNumBytes,
                        char *inHexBuffer );


/*
  Computes the flexHash of a buffer of bytes.

  A flexHash produces a variable-length hash matching the length requested
  by the caller, and fills the caller's provided hash buffer with that hash.

  Longer hash results are slower.

  Implementation makes use of only unsigned character buffers for internal
  state, so it doesn't depend on int or long endian representations,
  and makes no assumptions about unsigned char beyond those
  established by C89 (that unsigned chars are at least 8 bits
  and unsigned chars can contain values of at least 255).

  Has several nice properties:

  1. Around 20% faster at producing a 20-byte hash than SHA1 on my test hardware.
  
  2. For larger hashes (like 10 bytes and above), a hash of a series of single
     0 byte inputs produces a series of hash values that, when strung together,
     can pass 58/62 tests in the Dieharder RNG test suite.

  3. Excellent avalanche behavior:  Testing random 1000-byte inputs hashed into
     20-byte hashes, over 10 trials, doing a single bit flip of one of the 8000
     input bytes and the rehashing, the average percentage of bits flipped
     was between 44 and 56%, and the worst-case range for a single trial was
     between 30 and 70%.  Each of the 8000 bit positions was tested separately
     over 10 trials, demonstrating no sensitive bits positions.  Similar
     sets of 10 trials with flips of every individual bit position were run
     with 1-byte, 2-byte, ..., 9-byte, and 10-byte randomized inputs, with the
     same bounds observed.

     For randomized 8-byte imputs with every bit position flip tested over
     100,000 trials (6.4 million bit flips), the most extreme percentage of bits
     in the 20-byte hash that flipped in response to one bit flip were 70.6% and
     28.8%.

     Testing a 1-byte input hashed into a 2000-byte output hash over 10,000
     trials, the same bounds were observed.
     
  3. For hashes equal in length to a small input sizes, each unique input
     hashes to a unique hash value, with no collisions.  This has been tested
     exhaustively for all possible 1-byte inputs into 1-byte hashes, and
     all possible 2-byte inputs into 2-byte hashes.
     
  4. For hashes that are longer than a small input size, each unique input
     hashes to a unique hash value, with no collisions.  This has been tested
     exhaustively for all 1-byte inputs hashed into 2-, 3-, ..., 31-, and
     32-byte hashes, and all 2-byte inputs hashed into 3-, 4-, ..., 19-, and
     20-byte hashes.

  [jumpMaxiginGeneral]
*/
void maxigin_flexHash( const unsigned char *inBytes, int inNumBytes,
                       unsigned char *inHashBuffer, int inHashLength );




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




#define hashTestSize  3200
unsigned char hashInputBuffer[ hashTestSize ];
unsigned char hashBuffer[hashTestSize];
char hexBuffer[ hashTestSize * 2 + 1 ];


#include <stdio.h>

static void gameInit( void ) {
    
    mingin_registerButtonMapping( QUIT, quitMapping );
    mingin_registerButtonMapping( FULLSCREEN_TOGGLE, fullscreenMapping );

    areWeInMaxiginGameInitFunction = 1;
    
    maxiginGame_init();

    areWeInMaxiginGameInitFunction = 0;


    /* fixme:  test hash */
    if( 1 ) {
        int longestFound = 0;
        int foundA;
        int hexLen = sizeof( hexBuffer ) - 1;
        int a, b, o;
        
        maxigin_flexHash( 0, 0, hashBuffer, hashTestSize );
        
        maxigin_hexEncode( hashBuffer, hashTestSize, hexBuffer );

        for( a=0; a<hexLen; a++ ) {
            for( b=a+1; b<hexLen; b++ ) {
                if( hexBuffer[a] == hexBuffer[b] ) {
                    o = 1;
                    while( hexBuffer[a+o] == hexBuffer[b+o] ) {
                        o++;

                        if( hexBuffer[b+o] == '\0' ) {
                            break;
                            }  
                        }
                    if( o > longestFound ) {
                        longestFound = o;
                        foundA = a;
                        }
                    }
                }
            }
        
        if( longestFound > 0 ) {
            char old = hexBuffer[foundA + longestFound];
            hexBuffer[foundA + longestFound] = '\0';
            
            maxigin_logInt( "Found longest repeat string: ", longestFound );
            mingin_log( &( hexBuffer[foundA] ) );
            mingin_log( "\n" );
            hexBuffer[foundA + longestFound] = old;
            }

        if( 1 ) {
            FILE *f = fopen( "out.txt", "w" );
            fprintf( f, "%s", hexBuffer );
            fclose( f );
            }
        

        if( 1 ) {
            int hSize = 1;

            for( hSize = 1; hSize < 4; hSize ++ ) {
                
                int c;

                for( c = 'a'; c< 'z'; c++ ) {
                    char letter[2];
                    letter[0] = (char)( c );
                    letter[1] = '\0';
            
                    maxigin_flexHash( (unsigned char *)letter, 1,
                                      hashBuffer, hSize );
        
                    maxigin_hexEncode( hashBuffer, hSize, hexBuffer );

                    mingin_log( "Hash of " );
                    mingin_log( letter );
                    mingin_log( " = " );
                    mingin_log( hexBuffer );
                    mingin_log( "\n" );
                    }
                }
            }

        if( 1 ) {
            int hSize = 1;

            for( hSize = 1; hSize < 40; hSize ++ ) {

                int h;
                for( h=0; h<hSize; h++ ) {
                    hashInputBuffer[h] = 0;
                    }
            
                maxigin_flexHash( hashInputBuffer, hSize,
                                  hashBuffer, 10 );
        
                maxigin_hexEncode( hashBuffer, 10, hexBuffer );

                maxigin_logInt( "Hash of zeros: ", hSize );

                mingin_log( hexBuffer );
                mingin_log( "\n" );
                }
            }
        }
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
    
    if( ! areWeInMaxiginGameInitFunction ) {
        mingin_log( "Game tried to call maxigin_initRegisterStaticMemory "
                    "from outside of maxiginGame_init\n" );
        return;
        }
    
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
    int readHandle;

    if( ! areWeInMaxiginGameInitFunction ) {
        mingin_log( "Game tried to call "
                    "maxigin_initRestoreStaticMemoryFromLastRun "
                    "from outside of maxiginGame_init\n" );
        return;
        }
    
    readHandle = mingin_startReadPersistData( saveGameFileName,
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
    int j = 0;
    
    while( i < MAXIGIN_LOG_INT_MAX_LENGTH - 2 &&
           inLabel[i] != '\0' ) {
        logIntBuffer[i] = inLabel[i];
        i++;
        }
    while( i < MAXIGIN_LOG_INT_MAX_LENGTH - 2 &&
           valString[j] != '\0' ) {
        logIntBuffer[i] = valString[j];
        i++;
        j++;
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



static const unsigned char flexHashTable[256] = {
    108,   35,   77,  207,    9,  111,  203,  175,
     70,  142,  194,  252,  115,  141,   32,  182,
    174,   15,  129,   33,   16,   43,  160,  144,
    149,   30,  197,  185,   54,  246,   75,  169,
    103,   66,   57,  240,   23,   68,  244,  232,
     81,   91,  147,  150,  223,   85,  124,  187,
    167,  110,  222,   80,   20,   95,  131,   74,
    242,   65,   49,   11,  139,   64,  181,    2,
    138,  176,   73,  239,  208,   39,  155,  163,
    168,  233,   50,  107,  173,  134,  180,  196,
      0,   69,  121,   82,  132,   17,   42,  212,
    143,   51,  192,  119,   21,   13,  137,  172,
    186,   60,  211,   47,  237,   46,   25,  254,
     53,  195,  198,    3,  250,   71,  227,  213,
    102,   78,  220,  146,  243,   37,  166,    4,
    190,   97,  178,  251,   45,  100,  159,  165,
    219,   34,  116,   22,  202,  179,  157,   38,
    230,  214,  118,  171,  151,  101,  199,   14,
    158,   98,  156,  117,   96,  231,  148,   92,
     12,  130,  161,  206,  218,  113,  193,  245,
     59,   24,    6,  112,  205,   55,  153,  247,
     88,  128,   36,  229,  170,  120,  210,  145,
    209,   26,    8,  200,  221,  177,   67,   89,
    215,  188,  235,  152,  133,  154,  136,  104,
     31,  204,   99,  241,   63,  164,   62,  109,
      1,  248,  191,  106,  140,   84,  226,  189,
    225,   40,  184,  114,   61,  122,  126,  217,
    183,  224,   93,  162,   87,   58,   83,  255,
     10,  105,   76,   28,  201,    7,   56,   52,
    123,  236,   72,  249,  216,  253,   19,   41,
     44,   48,  135,   27,   79,   29,   94,  238,
      5,   18,  228,  127,  125,   86,  234,   90
    };



void maxigin_flexHash( const unsigned char *inBytes, int inNumBytes,
                       unsigned char *inHashBuffer, int inHashLength ) {
    unsigned int j, i, n;
    unsigned int b;
    unsigned int numBytes = (unsigned)inNumBytes;
    unsigned int hashLength = (unsigned)inHashLength;
    unsigned int jBits, bBits;
    
    /* initialized output with the hash of a 0-byte string */
    i = 0;
    n = 0;
    
    for( j=0; j < hashLength; j++ ) {
        /* walk through table values in order (by incrementing i)
           and use those values to jump n forward in table, and take
           the byte found at position n as our next hash byte */

        /* also add in mix of bits from j to our n jump,
           so that as hash gets longer and
           longer, the init pattern has a much longer repeat cycle length
           (the current tested repeat cycle length is 65536) */
        jBits = j;
        while( jBits > 255 ) {
            jBits = ( jBits >> 8 ) ^ ( jBits & 0xFF );
            }
        
        /* cheap mod 256 */
        n = ( n + flexHashTable[i] + jBits ) & 0xFF;
        
        inHashBuffer[j] = flexHashTable[ n ];
        
        i = ( i + 1 ) & 0xFF;
        }

    
    /* now mix in each byte of our input */
    for( b=0; b< numBytes; b++ ) {
        unsigned char byte = inBytes[b];

        /* jump i forward by this byte value */
        
        i = ( i + byte ) & 0xFF;

        
        for( j=0; j < hashLength; j++ ) {
            /* now walk i forward incrementally from that point,
               and use value at i spot in table to jump n around in our table */

            n = ( n + flexHashTable[i] ) & 0xFF;
            
            inHashBuffer[j] = inHashBuffer[j] ^ flexHashTable[ n ];
            
            i = ( i + 1 )  & 0xFF;
            }
        }
    }



/* for 4-bit nibbles */
static char nibbleToHex( unsigned char inNibble ) {
    if( inNibble < 10 ) {
        return (char)( '0' + inNibble );
        }
    else {
        return (char)( 'A' + ( inNibble - 10 ) );
        }
    }



void maxigin_hexEncode( const unsigned char *inBytes, int inNumBytes,
                        char *inHexBuffer ) {
    int stringPos = 0;
    int i;
    
    for( i=0; i < inNumBytes; i++ ) {
        unsigned char b = inBytes[i];
        unsigned char upper = ( b >> 4 );
        unsigned char lower = b & 0x0F;

        inHexBuffer[ stringPos ] = nibbleToHex( upper );

        stringPos++;
        
        inHexBuffer[ stringPos ] = nibbleToHex( lower );
        
        stringPos++;
        }
    
    inHexBuffer[stringPos] = '\0';
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