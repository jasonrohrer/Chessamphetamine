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
  your settings will be used instead.

  Each setting is tagged below with   [jumpSettings]
  
*/



/*
  The game image's native width and height.  This is the ideal size for
  the game's pixel content.

  [jumpSettings]
*/
#ifndef MAXIGIN_GAME_NATIVE_W
    #define MAXIGIN_GAME_NATIVE_W  640
#endif
#ifndef MAXIGIN_GAME_NATIVE_H
    #define MAXIGIN_GAME_NATIVE_H  480
#endif



/*
  Enable the game recording and playback infrastructure, which requires a
  bigger static memory footprint.

  To disable recording, do this:

      #define MAXIGIN_ENABLE_RECORDING 0

  [jumpSettings]
*/
#ifndef MAXIGIN_ENABLE_RECORDING
    #define MAXIGIN_ENABLE_RECORDING 1
#endif


/*
  If recording is enabled, what is the maximum total size of the
  static memory that the game will register with:
  
      maxigin_initRegisterStaticMemory

  Maxigin will actually use 2x this amount of static memory internally as
  part of its incremental diff recording process.

  This static memory is ONLY compiled into the program if
  MAXIGIN_ENABLE_RECORDING is set to 1 (which is the default).

  If the game registers MORE than this amount, recording will still work,
  but incremental diff recording will be disabled.

  To set the max static size to 256, do this:
  
      #define MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES  256
      
  [jumpSettings]
*/
#ifndef MAXIGIN_MAX_RECORDING_STATIC_MEMORY_MAX_BYTES
    #define MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES  4096
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
      assert, etc., and it doesn't expect uint64_t to exist.
      If you want to use these in your game, you're free to do so,
      but Maxigin doesn't force your game to depend on these.

    --No dynamic memory allocation.

    --No explicit filesystem access (mingin functions are used for bulk data
      access, logging, persistent data storage, and so on).

  --No floating point types or math operations.

  --No assumptions about the size of int or long or short, beyond
    the minimum sizes required by C89.

  --No platform-dependent code surrounded by #ifdefs.  The same code compiles
    and runs on every platform.

  --No program entry point (beyond whatever entry point is provided by
    the platform-specific code in mingin.h)

  --Some functionality MAY depend on char and unsigned char being exactly
    8 bits long (like conversion of a byte array to ASCII Hex, which will
    generate two hex characters per byte), but other functionality makes
    no explicit assumptions about the size of char, beyond the C89 requirement
    that it's at least 8 bits.  Operation should stil be safe on platforms
    with larger chars.
    
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
  All general-purpose maxigin_ functions  CAN be called from this function.
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
  Get the next game-native-size image full pixels
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

  Also possible to save space by not saving entire memory each step, but just
  a diff from a keyframe.
  
  A few things to consider:

  1. This will require two extra static buffers that are at least as big as all
     of the registered memory regions, so that we can compute a diff at
     each timestep (and keep track of the diff-summed state currently in our
     file).  Probably give user a #define that lets them set this size, or
     turn this off on platforms where we don't want 3x memory usage.
     
  2. Diff steps will vary in size, which means we can't just seek to a known
     location in the file for accurate time-jumping.  However, we could
     potentially write a separate index file in parallel that records
     file pos offsets of keyframes every 5 seconds or whatever, and then,
     when the recording is done we can append this index to our recording,
     with a pointer at the end of the file that points back into the file
     at the location where this index starts.
*/




/*
  Converts an int into a \0-terminated string.
  
  Returns a static buffer that must be used before next call to intToString.

  [jumpMaxiginGeneral]
*/
const char *maxigin_intToString( int inInt );



/*
  Converts a \0-terminated string to an int.

  Extra characters beyond the last digit charager are ignored.

  Empty string, or string starting with no digit or no - character results
  in a 0 return value.

  [jumpMaxiginGeneral]
*/
int maxigin_stringToInt( const char *inString );




/*
  Logs a labeled int value to the game engine log with a newline.

  [jumpMaxiginGeneral]
*/
void maxigin_logInt( const char *inLabel, int inVal );



/*
  Logs a labeled \0-terminated string value to the game engine log with a
  newline.

  [jumpMaxiginGeneral]
*/
void maxigin_logString( const char *inLabel, const char *inVal );



/*
  Gets the length of a \0-terminated string.

  [jumpMaxiginGeneral]
*/
int maxigin_stringLength( const char *inString );



/*
  Returns 1 if two \0-terminated strings are equal, 0 if not.

  [jumpMaxiginGeneral]
*/
char maxigin_stringsEqual( const char *inStringA, const char *inStringB );



/*
  Generates a ASCII hex-encoding of byte buffer, in uppercase hex.

  inHexBuffer must have room for at least 2 * inNumBytes + 1 characters.

  [jumpMaxiginGeneral]
*/
void maxigin_hexEncode( const unsigned char *inBytes, int inNumBytes,
                        char *inHexBuffer );



/*
  Computes the finalized flexHash of a buffer of bytes.

  This is the single-call mode of flexHash.  See the incremental mode
  below.

  A flexHash produces a variable-length hash matching the length requested
  by the caller, and fills the caller's provided hash buffer with that hash.

  Longer hash results have a slightly larger fixed cost, but that extra
  cost does not scale with the length of the input.

  Implementation makes use of only unsigned character buffers for internal
  state, so it doesn't depend on int or long endian representations,
  and makes no assumptions about unsigned char beyond those
  established by C89 (that unsigned chars are at least 8 bits
  and unsigned chars can contain values of at least 255).

  The flexHash algorithm is a multibyte extension of a Pearson Hash.

  Has several nice properties:

  1. Around 20% faster at producing a 20-byte hash than SHA1 on my test hardware.
  
  2. For larger hashes (like 10 bytes and above), a hash of a series of single
     0 byte inputs produces a series of hash values that, when strung together,
     can pass 61/62 tests in the Dieharder RNG test suite.

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

  [jumpMaxiginGeneral]
*/
void maxigin_flexHash( const unsigned char *inBytes, int inNumBytes,
                       unsigned char *inHashBuffer, int inHashLength );



/*
  This structure is for the incremental mode of flexHash, where
  data can be added one block at a time.

  [jumpMaxiginGeneral]
*/
typedef struct FlexHashState {
        int j;
        unsigned char n;
        unsigned char *hashBuffer;
        int hashLength;
        unsigned char lastInputByte;
    } FlexHashState;



/*
  Initialize an incremental mode run of flexHash.

  The caller-provided hash buffer is incorporated into the FlexHashState.

  [jumpMaxiginGeneral]
*/
void maxigin_flexHashInit( FlexHashState *inState,
                           unsigned char *inHashBuffer,
                           int inHashLength );



/*
  Adds another block of data to an incremental mode run of flexHash.
  
  [jumpMaxiginGeneral]
*/
void maxigin_flexHashAdd( FlexHashState *inState,
                          const unsigned char *inBytes,
                          int inNumBytes );

/*
  Finishes an incremental mode run of flexHash.

  After calling flexHashFinish, adding additional data with flexHashAdd will
  result in an incorrect hash value.

  The resulting hash is in the caller-supplied buffer originally provided
  in the flexHashInit call.

  This hash result buffer can also be accessed in inState->hashBuffer

  [jumpMaxiginGeneral]
*/
void maxigin_flexHashFinish( FlexHashState *inState );







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
    PLAYBACK_START_STOP,
    PLAYBACK_FASTER,
    PLAYBACK_SLOWER,
    PLAYBACK_PAUSE,
    PLAYBACK_NORMAL,
    PLAYBACK_JUMP_HALF_AHEAD,
    PLAYBACK_JUMP_HALF_BACK,
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


static void initRecording( void );

static void stepRecording( void );

static void finalizeRecording( void );

/* returns 1 if playback started, 0 if not */
static char initPlayback( void );

/* returns 1 of playback happening, 0 if not */
static char playbackStep( void );

static void gameInit( void );

static void saveGame( void );


void minginGame_step( char inFinalStep ) {

    if( ! initDone ) {
        if( inFinalStep ) {
            /* ended before we even got to init, do nothing */
            return;
            }
        
        gameInit();
        initDone = 1;
        }
    

    /* handle both case where platform forced us to end and
       where user decided to quit */
    if( inFinalStep || mingin_isButtonDown( QUIT ) ) {

        if( inFinalStep ) {
            mingin_log( "Forced to quit by platform\n" );
            }
        else {
            mingin_log( "Got quit key\n" );
            }

        saveGame();

        finalizeRecording();
        
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

    if( ! playbackStep() ) {
        
        areWeInMaxiginGameStepFunction = 1;
    
        maxiginGame_step();
    
        areWeInMaxiginGameStepFunction = 0;

        stepRecording();
        }
    }



static MinginButton quitMapping[] = { MGN_KEY_Q, MGN_KEY_ESCAPE, MGN_MAP_END };
static MinginButton fullscreenMapping[] = { MGN_KEY_F, MGN_MAP_END };

static MinginButton playbackMappings[7][2] =
    { { MGN_KEY_BACKSLASH, MGN_MAP_END },
      { MGN_KEY_EQUAL, MGN_MAP_END },
      { MGN_KEY_MINUS, MGN_MAP_END },
      { MGN_KEY_0, MGN_MAP_END },
      { MGN_KEY_9, MGN_MAP_END },
      { MGN_KEY_BRACKET_L, MGN_MAP_END },
      { MGN_KEY_BRACKET_R, MGN_MAP_END } };



static void gameInit( void ) {
    int p;
    
    mingin_registerButtonMapping( QUIT, quitMapping );
    mingin_registerButtonMapping( FULLSCREEN_TOGGLE, fullscreenMapping );

    for( p = PLAYBACK_START_STOP; p <= PLAYBACK_JUMP_HALF_BACK; p++ ) {
        
        mingin_registerButtonMapping(
            p, playbackMappings[ p - PLAYBACK_START_STOP ] );
        }  
    
    areWeInMaxiginGameInitFunction = 1;
    
    maxiginGame_init();

    areWeInMaxiginGameInitFunction = 0;

    if( ! initPlayback() ) {
        initRecording();
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

static int totalMemoryRecordsBytes = 0;


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
    totalMemoryRecordsBytes += inNumBytes;
    }







static const char *saveGameDataStoreName = "maxigin_save.bin";


#define MAXIGIN_FINGERPRINT_LENGTH  10
static unsigned char fingerprintBuffer[ MAXIGIN_FINGERPRINT_LENGTH ];

#define MAXIGIN_FINGERPRINT_HEX_LENGTH \
    ( MAXIGIN_FINGERPRINT_LENGTH * 2 + 1 )

static char fingerprintHexBuffer[ MAXIGIN_FINGERPRINT_HEX_LENGTH ];


static char *getMemRecordsFingerprint( int *outTotalMemBytes ) {
    FlexHashState s;
    int i;
    int totalNumBytes = 0;
    
    maxigin_flexHashInit( &s, fingerprintBuffer,
                          MAXIGIN_FINGERPRINT_LENGTH );
    
    for( i=0; i<numMemRecords; i++ ) {
        totalNumBytes += memRecords[i].numBytes;
        maxigin_flexHashAdd( &s,
                             (unsigned char *)( memRecords[i].description ),
                             maxigin_stringLength( memRecords[i].description ) );
        }
    *outTotalMemBytes = totalNumBytes;

    maxigin_hexEncode( fingerprintBuffer, MAXIGIN_FINGERPRINT_LENGTH,
                       fingerprintHexBuffer );
    
    return fingerprintHexBuffer;
    }




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
        /* didn't find termination in data store
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
    else if( inBuffer[i] != '\0' && readNum == -1 ) {
        mingin_log( "Error:  Got read failure when trying to read string "
                    "from persistent data store.\n" );
        return 0;
        }

    return 1;
    }




#define MAXIGIN_READ_SHORT_STRING_LEN 64
static char shortStringReadBuffer[ MAXIGIN_READ_SHORT_STRING_LEN ];

/*
  Reads a \0-terminated short string (< 63 chars long) into a static buffer.

   Returns 0 on failure.
*/
static const char *readShortStringFromPersistData( int inStoreReadHandle ) {
    
    char success = readStringFromPersistData( inStoreReadHandle,
                                              MAXIGIN_READ_SHORT_STRING_LEN,
                                              shortStringReadBuffer );
    if( success ) {
        return shortStringReadBuffer;
        }
    else {
        return 0;
        } 
    }





/*
  Reads a \0-terminated string representation of an int from data store.

  Returns 1 on success, 0 on failure.
*/
static char readIntFromPersistData( int inStoreReadHandle,
                                    int *outInt ) {
    const char *read = readShortStringFromPersistData( inStoreReadHandle );

    if( read == 0 ) {
        return 0;
        }
    
    *outInt = maxigin_stringToInt( read );

    return 1;
    }


/*
  Writes a \0-terminated string representation of an int to data store.

  Returns 1 on success, 0 on failure.
*/
static char writeStringToPeristentData( int inStoreWriteHandle,
                                        const char *inString ) {
    
    return mingin_writePersistData( inStoreWriteHandle,
                                    maxigin_stringLength( inString ) + 1,
                                    (unsigned char*)inString );
    }


/*
  Writes a \0-terminated string representation of an int to data store.

  Returns 1 on success, 0 on failure.
*/
static char writeIntToPerisistentData( int inStoreWriteHandle,
                                       int inInt ) {
    return writeStringToPeristentData( inStoreWriteHandle,
                                       maxigin_intToString( inInt ) );
    }



#define MAXIGIN_PADDED_INT_LENGTH 12

static unsigned char intPadding[ MAXIGIN_PADDED_INT_LENGTH ];

/*
  Writes a \0-terminated string representation of an int to data store,
  and pads it with \0 characters afterwards to fill paddedIntLength total.

  Returns 1 on success, 0 on failure.
*/
static char writePaddedIntToPerisistentData( int inStoreWriteHandle,
                                       int inInt ) {
    const char *intString = maxigin_intToString( inInt );
    char success;
    int b=0;
    int len;
    
    success = mingin_writePersistData( inStoreWriteHandle,
                                       maxigin_stringLength( intString ),
                                       (unsigned char*)intString );

    if( ! success ) {
        return 0;
        }
    
    /* pad with \0     */
    len = maxigin_stringLength( intString );
    while( len < MAXIGIN_PADDED_INT_LENGTH ) {
        intPadding[b] = '\0';
        len++;
        b++;
        }

    /* write padding out */
    return mingin_writePersistData( inStoreWriteHandle, b, intPadding);
    }



/* returns 1 on success, 0 on failure */
static char saveGameToDataStore( int inStoreWriteHandle ) {
    char *fingerprint;
    int numTotalBytes;
    char success;
    int i;

    fingerprint = getMemRecordsFingerprint( &numTotalBytes );


    success = writeIntToPerisistentData( inStoreWriteHandle, numTotalBytes );

    if( ! success ) {
        MAXIGIN_SAVED_GAME_WRITE_FAILURE:
        maxigin_logString( "Failed to write to saved game data: ",
                           saveGameDataStoreName );
        return 0;
        }


    success = writeIntToPerisistentData( inStoreWriteHandle, numMemRecords );

    if( ! success ) {
        goto MAXIGIN_SAVED_GAME_WRITE_FAILURE;
        }

    
    success = writeStringToPeristentData( inStoreWriteHandle, fingerprint );
    
    if( ! success ) {
        goto MAXIGIN_SAVED_GAME_WRITE_FAILURE;
        }

    /* first write all the descriptions and sizes
       on loading, we can bail out if these don't match, before
       overwriting anything */
    for( i=0; i<numMemRecords; i++ ) {
        const char *des = memRecords[i].description;
    
        success = writeStringToPeristentData( inStoreWriteHandle, des );

        if( ! success ) {
            goto MAXIGIN_SAVED_GAME_WRITE_FAILURE;
            }
        
        success = writeIntToPerisistentData( inStoreWriteHandle,
                                             memRecords[i].numBytes );
        if( ! success ) {
            goto MAXIGIN_SAVED_GAME_WRITE_FAILURE;
            }
        }

    /* now write the actual memory regions */
    for( i=0; i<numMemRecords; i++ ) {
        
        /* write numBytes from memory location into storage */
        success = mingin_writePersistData(
            inStoreWriteHandle,
            memRecords[i].numBytes,
            (unsigned char*)memRecords[i].pointer );
        
        if( ! success ) {
            goto MAXIGIN_SAVED_GAME_WRITE_FAILURE;
            }
        }

    return 1;
    }



static void saveGame( void ) {
    int outHandle;
    
    if( numMemRecords == 0 ) {
        return;
        }
    
    outHandle = mingin_startWritePersistData( saveGameDataStoreName );

    if( outHandle == -1 ) {
        maxigin_logString( "Failed to open saved game for writing: ",
                           saveGameDataStoreName );
        return;
        }

    saveGameToDataStore( outHandle );
    
    mingin_endWritePersistData( outHandle );

    mingin_log( "Saved game.\n" );
    }



/* returns 1 on success, 0 on failure */
static char restoreStaticMemoryFromDataStore( int inStoreReadHandle ) {
    char *fingerprint;
    int numTotalBytes;
    int readNumTotalBytes;
    int readNumMemRecords;
    char success;
    int i;

    const char *readFingerprint;

    if( numMemRecords == 0 ) {
        return 0;
        }
    
    fingerprint = getMemRecordsFingerprint( &numTotalBytes );
    
 
    success = readIntFromPersistData( inStoreReadHandle, &readNumTotalBytes );

    if( ! success ) {
        mingin_log( "Failed to read total num bytes from save data.\n" );
        return 0;
        }

    if( readNumTotalBytes != numTotalBytes ) {
        mingin_log( "Save data does not match current total memory bytes, "
                    "ignoring.\n" );
        maxigin_logInt( "Save data has numTotalBytes = ", readNumTotalBytes );
        maxigin_logInt( "Current live numTotalBytes = ", numTotalBytes );
        
        return 0;
        }

    success = readIntFromPersistData( inStoreReadHandle, &readNumMemRecords );

    if( ! success ) {
        mingin_log( "Failed to read num memory records from save data.\n" );
        return 0;
        }
    
    if( readNumMemRecords != numMemRecords ) {
        mingin_log( "Save data does not match current numMemRecords, "
                    "ignoring.\n" );
        maxigin_logInt( "Save data has numMemRecords = ", readNumMemRecords );
        maxigin_logInt( "Current live numMemRecords = ", numMemRecords );
        
        return 0;
        }
    


    readFingerprint = readShortStringFromPersistData( inStoreReadHandle );
    
    if( readFingerprint == 0 ) {
        mingin_log( "Failed to read fingerprint from save data.\n" );
        return 0;
        }
    
    
    if( ! maxigin_stringsEqual( fingerprint, readFingerprint ) ) {
        mingin_log( "Save data does not match current memory fingerprint, "
                    "ignoring.\n" );
        maxigin_logString( "Save data has fingerprint = ",
                           readFingerprint );
        maxigin_logString( "Current live has fingerprint = ", fingerprint );
        
        return 0;
        }

    /* now read the memory records from the saved data */

    /* first read the descriptions and sizes to make sure they all match */
    for( i=0; i<numMemRecords; i++ ) {
        const char *liveDes = memRecords[i].description;
        const char *readDes =
            readShortStringFromPersistData( inStoreReadHandle );
        
        int readNumBytes;
        
        if( readDes == 0 ) {
            maxigin_logInt( "Failed to read saved description for record # = ",
                            i );
            return 0;
            }
        
        if( ! maxigin_stringsEqual( liveDes, readDes ) ) {
            maxigin_logInt(
                "Save data has wrong description for record # = ", i );
            
            maxigin_logString( "Save data has description = ", readDes );

            maxigin_logString( "Live description = ", liveDes );
        
            return 0;
            }
        
        
        success = readIntFromPersistData( inStoreReadHandle, &readNumBytes );
        
        if( ! success ) {
            maxigin_logInt( "Failed to read saved numBytes for record # = ",
                            i );
            return 0;
            }

        if( readNumBytes != memRecords[i].numBytes ) {
            maxigin_logInt(
                "Save data has wrong numBytes for record # = ", i );
            
            maxigin_logInt( "Save data has numBytes = ", readNumBytes );

            maxigin_logInt( "Live numBytes = ", memRecords[i].numBytes );

            return 0;
            }

        }

    
    /* Now it's safe to reach the memory regions into memory
       since everything above matches */
    
    for( i=0; i<numMemRecords; i++ ) {
        
        /* read numBytes from memory location into storage */
        int numRead = mingin_readPersistData(
            inStoreReadHandle,
            memRecords[i].numBytes,
            (unsigned char*)memRecords[i].pointer );
        
        if( numRead !=  memRecords[i].numBytes ) {
            maxigin_logInt(
                "Failed to read memory data from save data for record # = ", i );

            return 0;
            }
        }

    return 1;
    }




void maxigin_initRestoreStaticMemoryFromLastRun( void ) {
    char success;

    int storeSize;
    int readHandle;
    
    if( ! areWeInMaxiginGameInitFunction ) {
        mingin_log( "Game tried to call "
                    "maxigin_initRestoreStaticMemoryFromLastRun "
                    "from outside of maxiginGame_init\n" );
        return;
        }
    
    readHandle = mingin_startReadPersistData( saveGameDataStoreName,
                                              &storeSize );

    if( readHandle == -1 ) {
        maxigin_logString( "Failed to open saved game for reading: ",
                           saveGameDataStoreName );
        return;
        }

    success = restoreStaticMemoryFromDataStore( readHandle );

    mingin_endReadPersistData( readHandle );

    
    if( success ) {
        mingin_log( "Restored live memory from saved game data.\n" );
        }
    }







#if( MAXIGIN_ENABLE_RECORDING == 0 )
    /* recording is off, shrink our recording buffer down to nothing */
    #undef MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES
    #define MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES 1
#endif

/*
  buffer for our last state represented in the recording data store
  and our current state, used for computing the next diff.
*/
static unsigned char recordingBuffers[2][
    MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES ];

static int latestRecordingIndex = -1;


static const char *recordingDataStoreName = "maxigin_recording.bin";
static const char *recordingIndexDataStoreName = "maxigin_recordingIndex.bin";

static int recordingDataStoreHandle = -1;
static int recordingIndexDataStoreHandle = -1;

static char diffRecordingEnabled = 1;

static char recordingRunning = 0;

static int numDiffsSinceLastFullSnapshot = 0;

static int diffsBetweenSnapshots = 60;


static void copyMemoryIntoRecordingBuffer( int inIndex ) {
    int b = 0;
    int r = 0;
    unsigned char *buffer = recordingBuffers[ inIndex ];
    
    if( ! diffRecordingEnabled ) {
        return;
        }

    for( r=0; r<numMemRecords; r++ ) {

        int rb = 0;
        int recSize = memRecords[r].numBytes;
        unsigned char *recPointer = (unsigned char*)( memRecords[r].pointer );
        
        while( rb < recSize ) {
            buffer[b] = recPointer[ rb ];
            b++;
            rb++;
            }
        }
    
    latestRecordingIndex = inIndex; 
    }


static void closeRecordingDataStores( void ) {

    if( recordingDataStoreHandle != -1 ) {
        mingin_endWritePersistData( recordingDataStoreHandle );
        }
    if( recordingIndexDataStoreHandle != -1 ) {
        mingin_endWritePersistData( recordingIndexDataStoreHandle );
        }

    recordingDataStoreHandle = -1;
    recordingIndexDataStoreHandle = -1;

    recordingRunning = 0;
    }



static void recordFullMemorySnapshot( void ) {
    int r;
    int startPos = mingin_getPersistDataPosition( recordingDataStoreHandle );
    char success;
    
    /* write the starting pos of this full snapshot into our index data store
       use a padded int so that we can jump by 12 bytes to go "frame by frame"
       through the index.
    */
    success =
        writePaddedIntToPerisistentData( recordingIndexDataStoreHandle,
                                         startPos );

    if( ! success ) {
        maxigin_logString(
            "Failed to write data block to recording index data: ",
            recordingIndexDataStoreName );
        
        closeRecordingDataStores();
        return;
        }
    

    /* write our full snapshot header */
    success = writeStringToPeristentData( recordingDataStoreHandle, "F" );

    if( ! success ) {
        mingin_log(
            "Failed to write full memory snapshot header in recording\n" );
        
        closeRecordingDataStores();
        return;
        }
    
    for( r=0; r<numMemRecords; r++ ) {
        int recSize = memRecords[r].numBytes;
        unsigned char *recPointer = (unsigned char*)( memRecords[r].pointer );

        success =
            mingin_writePersistData( recordingDataStoreHandle,
                                     recSize, recPointer );

        if( ! success ) {
            maxigin_logString( "Failed to write data block to recording data: ",
                               recordingDataStoreName );
            closeRecordingDataStores();
            return;
            }
        }
    }



static char checkHeader( int inStoreReadHandle, const char inTargetLetter ) {
    const char *header = readShortStringFromPersistData( inStoreReadHandle );

    if( header == 0 ||
        header[0] != inTargetLetter ||
        header[1] != '\0' ) {
        
        /* bad header */
        return 0;
        }
    
    return 1;
    }
    


/*
  Restores a full memory snapshot from current position in a data store.

  Returns 1 on success, 0 on failure.
*/
static char restoreFromFullMemorySnapshot( int inStoreReadHandle ) {
    int r;
    
    if( ! checkHeader( inStoreReadHandle, 'F' ) ) {
        return 0;
        }
    

    for( r=0; r<numMemRecords; r++ ) {
        int recSize = memRecords[r].numBytes;
        unsigned char *recPointer = (unsigned char*)( memRecords[r].pointer );

        int numRead =
            mingin_readPersistData( inStoreReadHandle,
                                    recSize, recPointer );

        if( numRead != recSize ) {
            /* failed to read block */
            return 0;
            }
        }

    /* read all blocks */
    return 1;
    }


    
    
static void recordMemoryDiff( void ) {
    int prevIndex = latestRecordingIndex;
    int newIndex = 0;
    int b;
    int lastWritten = 0;
    char success;
    
    if( ! diffRecordingEnabled ) {
        return;
        }

    if( prevIndex == 0 ) {
        newIndex = 1;
        }
    
    copyMemoryIntoRecordingBuffer( newIndex );

    /* header for a diff */
    success = writeStringToPeristentData( recordingDataStoreHandle, "D" );

    if( ! success ) {
        mingin_log( "Failed to write memory diff header in recording\n" );
        
        closeRecordingDataStores();
        return;
        }
    
    for( b=0; b<MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES; b++ ) {
        if( recordingBuffers[prevIndex][b] !=
            recordingBuffers[newIndex][b] ) {
            /* a byte has changed */

            /* write its position offset from the previous one recorded */
            success = writeIntToPerisistentData( recordingDataStoreHandle,
                                                 b - lastWritten );

            if( ! success ) {
                mingin_log( "Failed to write diff position in recording\n" );
        
                closeRecordingDataStores();
                return;
                }
            
            lastWritten = b;

            /* write its value */
            success = mingin_writePersistData(
                recordingDataStoreHandle, 1,
                &( recordingBuffers[newIndex][b] ) );
            
            if( ! success ) {
                mingin_log( "Failed to write diff byte in recording\n" );
        
                closeRecordingDataStores();
                return;
                }
            }
        }

    /* write -1 to end of diff, so that we know it's over
       (each line in the diff starts with a valid non-negative position
       in our memory snapshot */
    
    success = writeIntToPerisistentData( recordingDataStoreHandle, -1 );
    
    if( ! success ) {
        mingin_log( "Failed to write diff footer in recording\n" );
        
        closeRecordingDataStores();
        return;
        }
    }


/*
  Restores memory from diff at current position in a data store.

  Returns 1 on success, 0 on failure.
*/
static char restoreFromMemoryDiff( int inStoreReadHandle ) {
    char success;
    int readInt;
    int curRecord = 0;
    int curRecordByte = 0;
    unsigned char *curRecordPointer = 0;
    
    int numRead;
    
    if( ! checkHeader( inStoreReadHandle, 'D' ) ) {
        return 0;
        }

    if( numMemRecords == 0 ) {
        /* trying to restore diff into no live memory records */
        return 0;
        }

    curRecordPointer = (unsigned char*)( memRecords[ curRecord ].pointer );
    
    

    success = readIntFromPersistData( inStoreReadHandle, &readInt );

    if( ! success ) {
        /* must have at least 1 int, at least the -1 at the end,
           even if diff is empty with no changes */
        return 0;
        }
    
    while( readInt != -1 ) {
        /* readInt is our offset into our full static memory space
           with static regions butted end-to-end
           This offset is relative to the position of the last byte
           in the diff */

        curRecordByte += readInt;

        while( curRecordByte > memRecords[ curRecord ].numBytes ) {
            /* gone past the end of our current record
               start marching into next record from 0 in that record */
            curRecordByte -= memRecords[ curRecord ].numBytes;
                
            curRecord ++;
            if( curRecord >= numMemRecords ) {
                /* diff includes offsets that go beyond our last
                   live memory recor */
                return 0;
                }
            curRecordPointer =
                (unsigned char*)( memRecords[ curRecord ].pointer );
            }
        
        /* we've found a place for the offset read from the diff to land */

        numRead = mingin_readPersistData(
            inStoreReadHandle, 1,
            &( curRecordPointer[ curRecordByte ] ) );

        if( numRead != 1 ) {
            return 0;
            }

        success = readIntFromPersistData( inStoreReadHandle, &readInt );
        
        if( ! success ) {
            return 0;
            }
        }

    /* got here, then we read the -1 terminator int at the end of
       our diff block */

    return 1;   
    }







    


static void initRecording( void ) {
    int b, i;
    int success;
    
    recordingRunning = 0;
    
    if( ! MAXIGIN_ENABLE_RECORDING ) {
        return;
        }
    if( numMemRecords == 0 ) {
        return;
        }
    if( MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES < totalMemoryRecordsBytes ) {
        diffRecordingEnabled = 0;
        }

    maxigin_logString( "Starting recording into data store: ",
                       recordingDataStoreName );
    
    recordingDataStoreHandle =
        mingin_startWritePersistData( recordingDataStoreName );

    if( recordingDataStoreHandle != -1 ) {
        recordingIndexDataStoreHandle =
            mingin_startWritePersistData( recordingIndexDataStoreName );
        }

    if( recordingDataStoreHandle == -1 ||
        recordingIndexDataStoreHandle == -1 ) {

        mingin_log( "Failed to open recording data stores for writing\n" );

        closeRecordingDataStores();
        return;
        }

    /* start by writing a normal saved game to the start of the recording data
       which is our current state that should be restored before playback */
    success = saveGameToDataStore( recordingDataStoreHandle );

    if( !success ) {
        mingin_log( "Failed to write save game header to recording data.\n" );
        
        closeRecordingDataStores();
        return;
        }
    
    /* zero out both buffers */
    for( i=0; i<2; i++ ) {
        for( b=0; b<MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES; b++ ) {
            recordingBuffers[i][b] = 0;
            }
        }
    
    recordFullMemorySnapshot();
    
    copyMemoryIntoRecordingBuffer( 0 );

    numDiffsSinceLastFullSnapshot = 0;

    recordingRunning = 1;
    }



static void stepRecording( void ) {
    if( ! MAXIGIN_ENABLE_RECORDING || ! recordingRunning ) {
        return;
        }

    
    if( numDiffsSinceLastFullSnapshot < diffsBetweenSnapshots ) {
        recordMemoryDiff();
        numDiffsSinceLastFullSnapshot ++;
        }
    else {
        recordFullMemorySnapshot();
        numDiffsSinceLastFullSnapshot = 0;
        }
    }



#define MAXIGIN_DATA_COPY_BUFFER_SIZE  512
static unsigned char dataCopyBuffer[ MAXIGIN_DATA_COPY_BUFFER_SIZE ];


/*
  Copies bytes from one open read data store into the end
  of an open write data store.

  Returns 1 on success, 0 on failure.
*/
static char copyIntoDataStore( int inStoreReadHandle,
                               int inStoreWriteHandle,
                               int inNumBytesToCopy ) {
    int numCopied = 0;

    while( numCopied < inNumBytesToCopy ) {
        int numLeft = inNumBytesToCopy - numCopied;
        int numThisTime = numLeft;
        int numRead;
        char success;
        
        if( numThisTime > MAXIGIN_DATA_COPY_BUFFER_SIZE ) {
            numThisTime = MAXIGIN_DATA_COPY_BUFFER_SIZE;
            }
        
        numRead = mingin_readPersistData( inStoreReadHandle, numThisTime,
                                          dataCopyBuffer );

        if( numRead == -1 || numRead < numThisTime ) {
            /* error in read, or reached end of data before we got
               inNumBytesToCopy */
            return 0;
            }
        success = mingin_writePersistData( inStoreWriteHandle, numRead,
                                           dataCopyBuffer );

        if( ! success ) {
            return 0;
            }
        numCopied += numRead;
        }
    
    return 1;
    }



static void finalizeRecording( void ) {
    if( ! MAXIGIN_ENABLE_RECORDING ) {
        return;
        }

    if( recordingRunning ) {
        int indexLength;
        char success;
        int recordingIndexReadHandle;
        
        mingin_endWritePersistData( recordingIndexDataStoreHandle );
        recordingIndexDataStoreHandle = -1;

        recordingIndexReadHandle =
            mingin_startReadPersistData( recordingIndexDataStoreName,
                                         &indexLength );

        if( recordingIndexReadHandle == -1 ) {
            mingin_log( "Failed to re-open recording index data at end "
                        "of recording data.\n" );
            
            mingin_endWritePersistData( recordingDataStoreHandle );
            recordingDataStoreHandle = -1;
            return;
            }
        
        success = copyIntoDataStore( recordingIndexReadHandle,
                                     recordingDataStoreHandle, indexLength );

        mingin_endReadPersistData( recordingIndexReadHandle );
            
        if( ! success ) {
            mingin_log( "Failed to copy recording index into end "
                        "of recording data.\n" );
            mingin_endWritePersistData( recordingDataStoreHandle );
            recordingDataStoreHandle = -1;
            
            return;
            }

        /* successfully added index to end, can delete index now */
        mingin_deletePersistData( recordingIndexDataStoreName );
        

        /* now append length of index
           padded so we know how far to jump back to read it during playback */
        success = writePaddedIntToPerisistentData( recordingDataStoreHandle,
                                                   indexLength );

        if( ! success ) {
            mingin_log( "Failed write length of index into end "
                        "of recording data.\n" );
            }
            
        mingin_endWritePersistData( recordingIndexDataStoreHandle );
        recordingIndexDataStoreHandle = -1;

        maxigin_logString( "Game recording finalized: ",
                           recordingDataStoreName );
        }

    closeRecordingDataStores();
    }







static const char *playbackDataStoreName = "maxigin_playback.bin";
static int playbackDataStoreHandle = -1;

static int playbackDataLength;

static char playbackRunning = 0;

static int playbackFullSnapshotLastPlayed = 0;
static int playbackIndexStartPos = 0;
static int playbackNumFullSnapshots = 0;


/* returns 1 on success, 0 on failure */
static char seekAndReadInt( int inStoreReadHandle, int inPos, int *outInt ) {
    
    char success = mingin_seekPersistData( inStoreReadHandle, inPos );
    
    if( ! success ) {
        return 0;
        }

    return readIntFromPersistData( inStoreReadHandle, outInt );
    }
    


static char initPlayback( void ) {
    char success;
    int indexLengthDataPos;
    int indexLength;

    int firstFullSnapshotDataPos;

    playbackRunning = 0;

    if( numMemRecords == 0 ) {
        return 0;
        }
    
    /* user copies "playback" data store into place if they want playback
       
       fixme:  eventually, there's some menu option to start playback,
       and we shouldn't be overwriting last recording with new one
       (last recording should be copied to playback store at startup,
       and then they can select "playback" from the menu) */
    
    playbackDataStoreHandle =
        mingin_startReadPersistData( playbackDataStoreName,
                                     &playbackDataLength );
    
    if( playbackDataStoreHandle == -1 ) {
        /* store doesn't exist, no playback */
        return 0;
        }

    maxigin_logString( "Loading save data header from playback data store: ",
                       playbackDataStoreName );
    
    success = restoreStaticMemoryFromDataStore( playbackDataStoreHandle );

    if( ! success ) {
        mingin_log( "Failed to restore state from saved data "
                    "in playback data store." );
        mingin_endReadPersistData( playbackDataStoreHandle );
        return 0;
        }

    /* jump to end and read padded int */
    indexLengthDataPos = playbackDataLength - MAXIGIN_PADDED_INT_LENGTH;

    success = seekAndReadInt( playbackDataStoreHandle,
                              indexLengthDataPos, &indexLength );
    
    if( ! success ) {
        maxigin_logInt( "Failed to seek to this position and read index "
                        "length in playback data store: ",
                        indexLengthDataPos );
        
        mingin_endReadPersistData( playbackDataStoreHandle );
        return 0;
        }

    playbackIndexStartPos = playbackDataLength -
        MAXIGIN_PADDED_INT_LENGTH - indexLength;

    success = seekAndReadInt( playbackDataStoreHandle,
                              playbackIndexStartPos,
                              &firstFullSnapshotDataPos );

    if( ! success ) {
        maxigin_logInt( "Failed to seek to this position and read first "
                        "index entry in playback data store: ",
                        playbackIndexStartPos );
        
        mingin_endReadPersistData( playbackDataStoreHandle );
        return 0;
        }

    /* now jump to that first full snapshot */

    success = mingin_seekPersistData( playbackDataStoreHandle,
                                      firstFullSnapshotDataPos );

    if( ! success ) {
        maxigin_logInt( "Failed to seek to this position and read first "
                        "full snapshot in playback data store: ",
                        firstFullSnapshotDataPos );
        
        mingin_endReadPersistData( playbackDataStoreHandle );
        return 0;
        }

    success = restoreFromFullMemorySnapshot( playbackDataStoreHandle );

    if( ! success ) {

        maxigin_logInt( "Failed to restore first full memory snapshot from "
                        "playback data store at this position: ",
                        firstFullSnapshotDataPos );
        return 0;
        }

    
    playbackFullSnapshotLastPlayed = 0;
    playbackNumFullSnapshots = indexLength / MAXIGIN_PADDED_INT_LENGTH;
    
    maxigin_logInt( "Playback started successfully with num snapshots: ",
                    playbackNumFullSnapshots );
    
    playbackRunning = 1;
    
    return 1;
    }


static void playbackEnd( void ) {
    if( playbackDataStoreHandle != -1 ) {
        mingin_endReadPersistData( playbackDataStoreHandle );
        playbackDataStoreHandle = -1;
        }
    playbackRunning = 0;
    }


static char playbackStep( void ) {
    int curDataPos;
    char success;
    
    if( ! playbackRunning ) {
        return 0;
        }

    /* 
       try to read a diff from current location
       if that fails, rewind and try to read full snapshot
    */
    curDataPos = mingin_getPersistDataPosition( playbackDataStoreHandle );

    if( curDataPos == -1 ) {
        mingin_log( "Playback failed to get current position from playback "
                    "data source." );
        playbackEnd();
        return 0;
        }

    success = restoreFromMemoryDiff( playbackDataStoreHandle );

    if( ! success ) {
        /* diff reading failed
           try reading a whole snapshot */

        if( playbackFullSnapshotLastPlayed == playbackNumFullSnapshots - 1 ) {
            /* reached end */
            maxigin_logInt( "Reached end of playback with num snapshots: ",
                            playbackNumFullSnapshots );
            playbackEnd();
            return 0;
            }

        /* rewind */
        success = mingin_seekPersistData( playbackDataStoreHandle,
                                          curDataPos );

        if( !success ) {
            mingin_log( "Seek-back failed in playback data source." );

            playbackEnd();
            return 0;
            }
        success = restoreFromFullMemorySnapshot( playbackDataStoreHandle );

        if( !success ) {
            mingin_log( "Neither full-memory snapshot nor partial diff "
                        "restored successfully from playback data source." );
            playbackEnd();
            return 0;
            }
        
        playbackFullSnapshotLastPlayed ++;
        
        maxigin_logInt( "Just played snapshot: ",
                        playbackFullSnapshotLastPlayed );
        }

    
    
    return 1;
    }








#define MAXIGIN_LABELED_LOG_MAX_LENGTH  256

static char labeledLogBuffer[ MAXIGIN_LABELED_LOG_MAX_LENGTH ];


void maxigin_logString( const char *inLabel, const char *inVal ) {
    int i = 0;
    int j = 0;
    
    while( i < MAXIGIN_LABELED_LOG_MAX_LENGTH - 2 &&
           inLabel[i] != '\0' ) {
        labeledLogBuffer[i] = inLabel[i];
        i++;
        }
    while( i < MAXIGIN_LABELED_LOG_MAX_LENGTH - 2 &&
           inVal[j] != '\0' ) {
        labeledLogBuffer[i] = inVal[j];
        i++;
        j++;
        }
    labeledLogBuffer[i] = '\n';
    labeledLogBuffer[i+1] = '\0';

    mingin_log( labeledLogBuffer );
    }



void maxigin_logInt( const char *inLabel, int inVal ) {
    const char *valString = maxigin_intToString( inVal );

    maxigin_logString( inLabel, valString );
    }


int maxigin_stringLength( const char *inString ) {
    int len = 0;
    while( inString[len] != '\0' ) {
        len++;
        }
    return len;
    }



char maxigin_stringsEqual( const char *inStringA, const char *inStringB ) {
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
    int sign = 1;
    int i = 0;
    int val = 0;
    
    if( inString[i] == '-' ) {
        sign = -1;
        i++;
        }
    
    if( inString[i] < '0' || inString[i] > '9' ) {
        /* starts with non-digit */
        return 0;
        }
    
    while( inString[i] >= '0' && inString[i] <= '9' ) {
        val = val * 10 + ( inString[i] - '0' );
        i++;
        }
    
    return val * sign;
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



void maxigin_flexHashInit( FlexHashState *inState,
                           unsigned char *inHashBuffer,
                           int inHashLength ) {

    /*
      The following code inits the hash buffer with the following properties:

      1. Each freshly inited has buffer of a different length contains
         different values.

      2. Every freshly inited has buffer starts with the byte 0x77

      3. For very long hash buffers, the repeat cycle length of data in the
         fresly inited buffer is very very long.  This has been tested up
         to buffers with 100,000,000 bytes with no cycling.
    */
    
    int j;
    unsigned char i, n, k, m, index;

    int run;
    
    int hashLength = inHashLength;
    unsigned int jBits;
    unsigned char jBitsChar;
    
    i = 0;
    k = 199;
    n = 17;
    m = 107;
    
    /* zero our buffer to start */
    for( j=0; j < hashLength; j++ ) {
        inHashBuffer[j] = 0;
        }

    /* run twice, and xor second run into bytes from first
       we xor into our all-0 buffer in the first run */
    for( run=0; run<2; run++ ) {
        
        /* offset each run by 1, in case anything about the cycling
           of our variables lines up perfectly with hashLength.
           This also means that we only do one run if hashLength = 1 */
        
        for( j=run; j < hashLength; j++ ) {
            /* walk through table values in order (by incrementing i)
               and use those values to mix with our running n, plus our
               current buffer value (0 at first, or what we computed last run
               later to get our next buffer value */

            /* also add in mix of bits from j, which makes our basic
               repeat cycle much longer.
               The pattern of jBits is more complex than a counter from 0..255
               that simply wraps around.  */
        
            jBits = (unsigned int)j;
            while( jBits > 255 ) {
                jBits = ( jBits >> 8 ) ^ ( jBits & 0xFF );
                }
            jBitsChar = (unsigned char)jBits;
            
            /* we increment i by 1 below, which walks through every value in the
               table in order.  But we also phase shift this walk according to k
               and m, and whenever k or m increments, this phase shift changes
               dramatically, due to the flexHashTable lookups.
           
               The result is that we periodically jump to a different spot in
               the table and start incrementally walking from there,
               which makes our cycle period extremely long. */
        

            /* don't assume char is not larger than 8 bits
               so we can't count on wrap-around behavior above 255 */

            index = (unsigned char)( ( i +
                                       flexHashTable[ k ] +
                                       flexHashTable[m] )
                                     & 0xFF );

        
            n = n ^ flexHashTable[ index ] ^ jBitsChar ^ inHashBuffer[j];
        
            inHashBuffer[j] = n;
        
        
            i = ( i + 1 ) & 0xFF;

        
            /* k increments much more slowly, roughly 1/64 as often as i
               but because we use n to decide when k increments, this happens
               on a very chaotic schedule that doesn't seem to have a pattern */
            if( n == 13 || n == 101 || n == 173  || n == 207 ) {
                k = ( k + 1 ) & 0xFF;
                }

            /* m increments roughly 1/256 as often as i */
            if( n == 67 ) {
                m = ( m + 1 ) & 0xFF;
                }
            }
        }
    

    /* push n forward one more time, so n is not equal to the first
       byte in our buffer in the inHashLength=1 case */
    n = n ^ flexHashTable[i];

    inState->j = 0;
    inState->n = n;
    inState->hashBuffer = inHashBuffer;
    inState->hashLength = hashLength;
    inState->lastInputByte = 0;
    }



void maxigin_flexHashAdd( FlexHashState *inState,
                          const unsigned char *inBytes,
                          int inNumBytes ) {
    
    int j;
    unsigned char n;
    int b;
    int numBytes = inNumBytes;
    int hashLength = inState->hashLength;
    unsigned char *hashBuffer = inState->hashBuffer;
    int bLimit, jLimit;
    
    n = inState->n;
    j = inState->j;
    
    /* mix in each byte of our hash buffer */


    /*
      This mixing opration is a multi-byte extension of Pearson Hashing,
      with the added twist that n holds the last hash buffer value
      that we touched.  This means that even with input that is all
      zero bytes, we still get complex mixing, as each byte in our buffer
      mixes with the previous byte in the buffer

      The basic form of the mixing operation for a sequence of bytes looks
      like this:

           while( b < numBytes ) {
               n = flexHashTable[ hashBuffer[j] ^ inBytes[b] ^ n ];
               hashBuffer[j] = n;
        
               j++;
        
               if( j >= hashLength ) {
                   j = 0;
                   }
               b++;
               }

       We unroll this below to improve performance, but the actual algorithm
       is the above loop.
    */

    /*
      These limits tell us we're in the middle of our input buffer and
      hash buffer, which allows us to use the unrolled version.
      Note that if our hashLength is < 4, or our input data buffer is shorter
      than 4, we don't use these unrolled versions at all.
    */
    bLimit = numBytes - 4;
    jLimit = hashLength - 4;

    b = 0;

    while( b < bLimit ) {
        
        if( j < jLimit ) {
            /* safe to unroll 4x */
            
            n = flexHashTable[ hashBuffer[j] ^ inBytes[b] ^ n ];
            hashBuffer[j] = n;
            j++;
            b++;
            
            n = flexHashTable[ hashBuffer[j] ^ inBytes[b] ^ n ];
            hashBuffer[j] = n;
            j++;
            b++;
            
            n = flexHashTable[ hashBuffer[j] ^ inBytes[b] ^ n ];
            hashBuffer[j] = n;
            j++;
            b++;
            
            n = flexHashTable[ hashBuffer[j] ^ inBytes[b] ^ n ];
            hashBuffer[j] = n;
            j++;
            b++;
            } 
        

        /* back to regular loop for final j values */
        
        n = flexHashTable[ hashBuffer[j] ^ inBytes[b] ^ n ];
        hashBuffer[j] = n;
        
        j++;
        
        if( j >= hashLength ) {
            j = 0;
            }
        b++;
        }


    /* and our most basic loop for final b values, toward end of buffer */
    while( b < numBytes ) {
        n = flexHashTable[ hashBuffer[j] ^ inBytes[b] ^ n ];
        hashBuffer[j] = n;
        
        j++;
        
        if( j >= hashLength ) {
            j = 0;
            }
        b++;
        }
    
        

    inState->j = j;
    inState->n = n;

    if( inNumBytes > 0 ) {
        inState->lastInputByte = inBytes[ inNumBytes - 1 ];
        }
    }



void maxigin_flexHashFinish( FlexHashState *inState ) {
    int j, run;
    unsigned char n;
    int hashLength = inState->hashLength;
    unsigned char *hashBuffer = inState->hashBuffer;
    unsigned char lastByte = inState->lastInputByte;
    
    n = inState->n;
    
    /*
      mix last input byte in 4 more times

      We do this with lastByte to improve behavior dramatically in
      few-byte-input cases (if we only have 1 input byte, for example,
      we want it to touch every byte in our hash directly)
     */
    
    for( run=0; run<4; run++ ) {
        for( j=0; j<hashLength; j++ ) {
            n = flexHashTable[ hashBuffer[j] ^ lastByte ^ n ];
            hashBuffer[j] = n;
            }
        }
    }



void maxigin_flexHash( const unsigned char *inBytes, int inNumBytes,
                       unsigned char *inHashBuffer, int inHashLength ) {
    FlexHashState s;
    maxigin_flexHashInit( &s, inHashBuffer, inHashLength );

    maxigin_flexHashAdd( &s, inBytes, inNumBytes );

    maxigin_flexHashFinish( &s );
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
