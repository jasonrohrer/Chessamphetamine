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
  
      #define   MINGIN_IMPLEMENTATION
  
      #define   MAXIGIN_IMPLEMENTATION
  
      #include  "maxigin.h"

  Maxigin implements the four functions needed by a Mingin game
  (see mingin.h), so your game shouldn't implement these directly.

  You can still include mingin.h in your code and use the provided functions
  directly (see [jumpMingin] in mingin.h for a list of functions).

*/



#ifndef  MAXIGIN_H_INCLUDED
#define  MAXIGIN_H_INCLUDED



/*
  ===================================================
  Static settings                      [jumpSettings]
  ===================================================

  The following compile-time settings can be changed for each game that
  is built against Maxigin.

  If these are defined in your C file before #include "maxigin.h"
  your settings will be used instead.

  Each setting is tagged below with   [jumpSettings]
  
*/



/*
  The game image's native width and height.  This is the ideal size for
  the game's pixel content.

  To set the native pixel size to 1280x720, do this:

      #define  MAXIGIN_GAME_NATIVE_W  1280
      #define  MAXIGIN_GAME_NATIVE_H  720

  [jumpSettings]
*/
#ifndef  MAXIGIN_GAME_NATIVE_W
    #define  MAXIGIN_GAME_NATIVE_W  640
#endif
#ifndef  MAXIGIN_GAME_NATIVE_H
    #define  MAXIGIN_GAME_NATIVE_H  480
#endif



/*
  Enable the game recording and playback infrastructure, which requires a
  bigger static memory footprint.

  To disable recording, do this:

      #define  MAXIGIN_ENABLE_RECORDING  0

  [jumpSettings]
*/
#ifndef  MAXIGIN_ENABLE_RECORDING
    #define  MAXIGIN_ENABLE_RECORDING  1
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
  
      #define  MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES   256
      
  [jumpSettings]
*/
#ifndef  MAXIGIN_MAX_RECORDING_STATIC_MEMORY_MAX_BYTES
    #define  MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES  4096
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
  portability only, and do things your own way otherwise, you're free to do that.

  My goal is to make this code ridiculously portable:  100% C89, with absolutely
  no includes or dependences, beyond mingin.h

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
  guaranteed.  For example, during recorded game playback mode, many calls
  to maxiginGame_getNativePixels might be called with no calls to
  maxiginGame_step in between.
  
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
  All general-purpose maxigin_ functions CAN be called from this function.
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
  All general-purpose maxigin_ functions CAN be called from this function.
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
  at least one call to maxiginGame_step()

  Parameters:

      inRGBBuffer   buffer of pixel byte values in RGBRGB... row-major
                    order starting at top left corner
                    
  [jumpGameRequired]
*/
void maxiginGame_getNativePixels( unsigned char  *inRGBBuffer );





/*
  ==================================================
  What Maxigin provides                [jumpMaxigin]
  ==================================================
  
  Maxigin provides two sets of functions.

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

  Parameters:

      inPointer       the start of the static memory area.
  
      inNumBytes      the length of the memory area.

      inDescription   a unique descriptor string
  
  [jumpMaxiginInit]
*/
void maxigin_initRegisterStaticMemory( void        *inPointer,
                                       int          inNumBytes,
                                       const char  *inDescription );



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
  Maxigin listens to certain buttons for its own functionality that's not
  game-specific (like accessing a settings screen, toggling fullscreen, etc.)

  If a game registers button handles directly with mingin_registerButtonMapping,
  those registrations may clobber Maxigin's registrations.

  Games that want to take full advantage of Maxigin's functionality should
  call this instead of mingin_registerButtonMapping.

  See mingin_registerButtonMapping in mingin.h for full documentation.

  Parameters:

      inButtonHandle   the game-defined button action to map

      inMapping        an array of MinginButton values, ending with MGN_MAP_END,
                       that should trigger this game-defined action
                       
  Returns:

      1   on success
      
      0   on failure (if inButtonHandle is out of the supported range)  

  [jumpMaxiginGeneral]
*/
char maxigin_registerButtonMapping( int                 inButtonHandle,
                                    const MinginButton  inMapping[] );



/*
  Check whether a previously-mapped button handle is currently held down.

  Note that Maxigin might block reporting of certain buttons being down
  depending on its own internal functionality.

  See mingin_isButtonDown in mingin.h for full documentation.

  Games that want to take full advantage of Maxigin's functionality should
  call this instead of mingin_isButtonDown.
  
  Parameters:

      inButtonHandle   the game-defined button action to check

  Returns:

      1   if pressed

      0   if not pressed
  
  [jumpMaxiginGeneral]
*/
char maxigin_isButtonDown( int  inButtonHandle );



/*
  Reads an integer value from a persistent setting.

  Parameters:

      inSettingName    the name of the setting to read

      inDefaultValue   the default value for the setting

  Returns:

      setting value    if found

      inDefaultValue   if not found
                          or reading fails
      
   [jumpMaxiginGeneral]
*/     
int maxigin_readIntSetting( const char  *inSettingName,
                            int          inDefaultValue );



/*
  Writes an integer value to a persistent setting.

  On write failure, this call has no effect.

  Parameters:

      inSettingName    the name of the setting to write

      inValue          the value to write
      
   [jumpMaxiginGeneral]
*/   
void maxigin_writeIntSetting( const char  *inSettingName,
                              int          inValue );



/*
  Converts an int into a \0-terminated string.
  
  Returns a static buffer that must be used before next call to intToString.

  Positive int values up to 9999999999 (under 10 billion) and negative
  int values down to -9999999999 (above -10 billon) are supported.

  Note that the int data type on some platforms might not support this
  entire range.

  Paramters:

      inInt   the int value to convert

  Return:

      \0-terminated string   if conversion succeeded

      "[int_format_error]"   if conversion failed

  [jumpMaxiginGeneral]
*/
const char *maxigin_intToString( int  inInt );



/*
  Converts a \0-terminated string to an int.

  Extra characters beyond the last digit character are ignored.

  Empty string, or string starting with no digit or no - character, results
  in a 0 return value.

  Note that this function makes no assumptions about the maxiumum or minimum
  value of an int.  C89 guarantees that an int can support at least the range
  [-32767, 32767], so truly platform independent code should not exceed this
  range.

  Parameters:

      inString   the \0-terminated string to convert.

  Returns:

      int value   if conversion succeeds

      0           if conversion fails

  [jumpMaxiginGeneral]
*/
int maxigin_stringToInt( const char  *inString );



/*
  Concatonates two strings.

  Returns a static buffer from a rotating pool of 10 static buffers,
  which allows for nested concatonations.

  Max resulting string is 64 characters long, including the \0 termination.

  If the resulting concatonation exceeds this length, it will be truncated.
  
  Parameters:

      inStringA   the first \0-terminated string to concatonate

      inStringB   the second \0-terminated string to concatonate

  Returns:

      Concatonated \0-terminated string
  
  [jumpMaxiginGeneral]    
*/
const char *maxigin_stringConcat( const char  *inStringA,
                                  const char  *inStringB );



/*
  Logs a labeled int value to the game engine log with a newline.

  Parameters:

      inLabel   the \0-terminated string label for the value in the log

      inVal     the int value to log

  [jumpMaxiginGeneral]
*/
void maxigin_logInt( const char  *inLabel,
                     int          inVal );



/*
  Logs a labeled \0-terminated string value to the game engine log with a
  newline.

  Parameters:

      inLabel   the \0-terminated string label for the value in the log

      inVal     the \0-terminated string value to log  

  [jumpMaxiginGeneral]
*/
void maxigin_logString( const char  *inLabel,
                        const char  *inVal );



/*
  Gets the length of a \0-terminated string.
  
  Parameters:

      inString   the \0-terminated string to measure.

  Returns:

      length of string
      
  [jumpMaxiginGeneral]
*/
int maxigin_stringLength( const char  *inString );



/*
  Tests whether strings are equal.
    
  Parameters:

      inStringA   the first \0-terminated string to compare

      inStringB   the second \0-terminated string to compare

  Returns:

      1   if two strings are equal

      0   if not

  [jumpMaxiginGeneral]
*/
char maxigin_stringsEqual( const char  *inStringA,
                           const char  *inStringB );



/*
  Generates a ASCII hex-encoding of byte buffer, in uppercase hex.

  Parameters:

      inNumBytes    the number of bytes to encode
      
      inBytes       the buffer of bytes to encode

      inHexBuffer   the buffer to fill with the resulting hex encoding as
                    a \0-terminated string.  Must have room for at least
                    2 * inNumBytes + 1  characters.

  [jumpMaxiginGeneral]
*/
void maxigin_hexEncode( int                   inNumBytes,
                        const unsigned char  *inBytes,
                        char                 *inHexBuffer );



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

  flexHash has several nice properties:

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

     
  Parameters:

      inNumBytes     number of bytes to hash

      inBytes        buffer of bytes to hash

      inHashLength   length of hash to produce

      inHashBuffer   the buffer to fill with the resulting hash

  [jumpMaxiginGeneral]
*/
void maxigin_flexHash( int                   inNumBytes,
                       const unsigned char  *inBytes,
                       int                   inHashLength,
                       unsigned char        *inHashBuffer );



/*
  This structure is for the incremental mode of flexHash, where
  data can be added one block at a time.

  [jumpMaxiginGeneral]
*/
typedef struct MaxiginFlexHashState {
        int             j;
        unsigned char   n;
        int             hashLength;
        unsigned char  *hashBuffer;
        unsigned char   lastInputByte;
    } MaxiginFlexHashState;



/*
  Initialize an incremental mode run of flexHash.

  The caller-provided hash buffer is incorporated into the MaxiginFlexHashState.

  Parameters:

      inState        the hash structure to initialize
  
      inHashLength   the byte length of the resulting hash

      inHashBuffer   buffer where the resulting hash should be accumulated
      
  [jumpMaxiginGeneral]
*/
void maxigin_flexHashInit( MaxiginFlexHashState  *inState,
                           int                    inHashLength,
                           unsigned char         *inHashBuffer );



/*
  Adds another block of data to an incremental mode run of flexHash.

  Parameters:

      inState      the hash structure to update 
  
      inNumBytes   the number of bytes to hash

      inBytes      the bytes to hash
  
  [jumpMaxiginGeneral]
*/
void maxigin_flexHashAdd( MaxiginFlexHashState  *inState,
                          int                    inNumBytes,
                          const unsigned char   *inBytes );



/*
  Finishes an incremental mode run of flexHash.

  After calling flexHashFinish, adding additional data with flexHashAdd will
  result in an incorrect hash value.

  The resulting hash is in the caller-supplied buffer originally provided
  in the flexHashInit call.

  This hash result buffer can also be accessed in inState->hashBuffer

  Parameters:

      inState      the hash structure to finish

  [jumpMaxiginGeneral]
*/
void maxigin_flexHashFinish( MaxiginFlexHashState  *inState );







/*
  ===================================================
  Maxigin internal code                [jumpInternal]
  ===================================================

  The rest of this file contains Maxigin's internal implementation.

*/






#ifdef MAXIGIN_IMPLEMENTATION





typedef enum MaxiginUserAction {
    QUIT,
    FULLSCREEN_TOGGLE,
    PLAYBACK_START_STOP,
    PLAYBACK_FASTER,
    PLAYBACK_SLOWER,
    PLAYBACK_PAUSE,
    PLAYBACK_NORMAL,
    PLAYBACK_REVERSE,
    PLAYBACK_JUMP_HALF_BACK,
    PLAYBACK_JUMP_HALF_AHEAD,
    LAST_MAXIGIN_USER_ACTION
    } MaxiginUserAction;


static  char  mx_areWeInMaxiginGameInitFunction  =  0;

static  char  mx_areWeInMaxiginGameStepFunction  =  0;

static  char  mx_initDone                        =  0;

static  char  mx_recordingRunning                =  0;
static  char  mx_playbackRunning                 =  0;
static  char  mx_playbackPaused                  =  0;
static  int   mx_playbackSpeed                   =  1;
                                                    /* -1 for backward */
static  char  mx_playbackDirection               =  1;

static  char  mx_buttonsDown[ LAST_MAXIGIN_USER_ACTION ];







void minginGame_getMinimumViableScreenSize( int  *outWide,
                                            int  *outHigh ) {
    *outWide = MAXIGIN_GAME_NATIVE_W;
    *outHigh = MAXIGIN_GAME_NATIVE_H;
    }



void minginGame_getScreenPixels( int             inWide,
                                 int             inHigh,
                                 unsigned char  *inRGBBuffer ) {
    /* RGB pixels of game's native image size */
    static  unsigned char  gameImageBuffer[ MAXIGIN_GAME_NATIVE_W *
                                            MAXIGIN_GAME_NATIVE_H * 3 ];
    
            int            numPixels      =  inWide * inHigh;
            int            numPixelBytes  =  numPixels * 3;
            int            p;
            
            int            x;
            int            y;
            
            int            scaleFactor;
            
            int            scaleW;
            int            scaleH;
            
            int            scaledGameW;
            int            scaledGameH;
            
            int            offsetX;
            int            offsetY;

    maxiginGame_getNativePixels( gameImageBuffer );

    scaleW = inWide / MAXIGIN_GAME_NATIVE_W;

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
    

    if( offsetX > 0
        ||
        offsetY > 0 ) {
        
        /* black background beyond edges of our centered image */
        for( p = 0;
             p < numPixelBytes;
             p ++ ) {
            
            inRGBBuffer[p] = 0;
            }
        }

    /* naive nearest neighbor scaling */
    for( y = offsetY;
         y < offsetY + scaledGameH;
         y ++ ) {
        
        int rowStartDest = y * inWide * 3;
        int ySrcScaled = y - offsetY;
        int ySrcOrig = ySrcScaled /  scaleFactor;
        
        int rowStartSrcOrig = ySrcOrig * MAXIGIN_GAME_NATIVE_W * 3;
        
        for( x = offsetX;
             x < offsetX + scaledGameW;
             x ++ ) {
            
            int xSrcScaled = x - offsetX;
            int xSrcOrig = xSrcScaled /  scaleFactor;
        
            int pixDest = rowStartDest + x * 3;
            int pixSrcOrig = rowStartSrcOrig + xSrcOrig * 3;

            inRGBBuffer[ pixDest ]      =  gameImageBuffer[ pixSrcOrig ];
            inRGBBuffer[ pixDest + 1 ]  =  gameImageBuffer[ pixSrcOrig + 1 ];
            inRGBBuffer[ pixDest + 2 ]  =  gameImageBuffer[ pixSrcOrig + 2 ];
            }
        }
    }



static void mx_initRecording( void );

static void mx_stepRecording( void );

static void mx_finalizeRecording( void );

static void mx_recordingCrashRecovery( void );


/* returns 1 if playback started, 0 if not */
static char mx_initPlayback( void );


/* returns 1 of playback happening, 0 if not
   ignores pause state */
static char mx_playbackStep( void );

/* executes multi-steps, or delayed steps, basked on mx_playbackSpeed
   obeys pause state */
static char mx_playbackSpeedStep( void );

static void mx_playbackJumpHalfAhead( void );
static void mx_playbackJumpHalfBack( void );

static void mx_playbackEnd( void );



static void mx_gameInit( void );

static void mx_saveGame( void );


static char mx_isActionFreshPressed( MaxiginUserAction  inAction ) {

    char fresh = 0;
    
    if( mingin_isButtonDown( inAction ) ) {
        if( ! mx_buttonsDown[ inAction ] ) {
            fresh = 1;
            }
        mx_buttonsDown[ inAction ] = 1;
        }
    else {
        mx_buttonsDown[ inAction ] = 0;
        }

    return fresh;
    }



static char mx_playbackInterruptedRecording = 0;



void minginGame_step( char  inFinalStep ) {

    if( ! mx_initDone ) {
        if( inFinalStep ) {
            /* ended before we even got to init, do nothing */
            return;
            }
        
        mx_gameInit();
        mx_initDone = 1;
        }
    

    /* handle both case where platform forced us to end and
       where user decided to quit */
    if( inFinalStep
        ||
        mingin_isButtonDown( QUIT ) ) {

        if( inFinalStep ) {
            mingin_log( "Forced to quit by platform\n" );
            }
        else {
            mingin_log( "Got quit key\n" );
            }

        mx_saveGame();

        mx_finalizeRecording();
        
        mingin_quit();
        return;
        }


    if( mx_isActionFreshPressed( FULLSCREEN_TOGGLE ) ) {
        mingin_toggleFullscreen( ! mingin_isFullscreen() );
        }

    
    if( mx_isActionFreshPressed( PLAYBACK_START_STOP ) ) {
        if( mx_playbackRunning ) {
            mx_playbackEnd();
            mx_initRecording();
            mx_playbackInterruptedRecording = 0;
            }
        else {
            if( mx_recordingRunning ) {
                mx_finalizeRecording();
                mx_playbackInterruptedRecording = 1;
                }
            mx_initPlayback();
            }
        }

    if( mx_playbackRunning ) {
        if( mx_isActionFreshPressed( PLAYBACK_PAUSE ) ) {
            mx_playbackPaused = ! mx_playbackPaused;
            }
        if( mx_isActionFreshPressed( PLAYBACK_NORMAL ) ) {
            mx_playbackPaused = 0;
            mx_playbackSpeed = 1;
            mx_playbackDirection = 1;
            }
        if( mx_isActionFreshPressed( PLAYBACK_REVERSE ) ) {
            mx_playbackPaused = 0;
            if( mx_playbackDirection == 1 ) {
                mx_playbackDirection = -1;
                }
            else {
                mx_playbackDirection = 1;
                }
            }
        if( mx_isActionFreshPressed( PLAYBACK_JUMP_HALF_AHEAD ) ) {
            mx_playbackJumpHalfAhead();
            }
        if( mx_isActionFreshPressed( PLAYBACK_JUMP_HALF_BACK ) ) {
            mx_playbackJumpHalfBack();
            }
                        
        if( mx_isActionFreshPressed( PLAYBACK_FASTER ) ) {
            if( mx_playbackPaused ) {
                /* faster button jumps ahead by one step when paused */
                mx_playbackStep();
                }
            else {
                /* not paused, faster adjusts speed */
            
                if( mx_playbackSpeed >= 1 ) {
                    mx_playbackSpeed ++;
                    }
                else if( mx_playbackSpeed == -2 ) {
                    /* coming out of half-speed mode */
                    mx_playbackSpeed = 1;
                    }
                else if( mx_playbackSpeed <= -4 ) {
                    /* in slow down mode, get 2x faster per step
                       to get out of it */
                    
                    mx_playbackSpeed /= 2;
                    }
                }
            }
        if( mx_isActionFreshPressed( PLAYBACK_SLOWER ) ) {
            if( mx_playbackSpeed > 1 ) {
                mx_playbackSpeed--;
                }
            else if( mx_playbackSpeed == 1 ) {
                mx_playbackSpeed = -2;
                }
            else if( mx_playbackSpeed < 0 ) {
                /* twice as slow */
                mx_playbackSpeed *= 2;
                }
            }
        }
    

    if( ! mx_playbackSpeedStep() ) {

        if( mx_playbackInterruptedRecording ) {
            /* playback has ended, resume recording */
            mx_initRecording();
            mx_playbackInterruptedRecording = 0;
            }
        
        mx_areWeInMaxiginGameStepFunction = 1;
    
        maxiginGame_step();
    
        mx_areWeInMaxiginGameStepFunction = 0;

        mx_stepRecording();
        }
    }



static  MinginButton  mx_quitMapping[] = { MGN_KEY_Q,
                                           MGN_KEY_ESCAPE,
                                           MGN_MAP_END };

static  MinginButton  mx_fullscreenMapping[] = { MGN_KEY_F,
                                                 MGN_MAP_END };

static  MinginButton  mx_playbackMappings[8][2] =
    { { MGN_KEY_BACKSLASH, MGN_MAP_END },   /* start-stop */
      { MGN_KEY_EQUAL,     MGN_MAP_END },   /* faster */
      { MGN_KEY_MINUS,     MGN_MAP_END },   /* slower */
      { MGN_KEY_0,         MGN_MAP_END },   /* pause */
      { MGN_KEY_9,         MGN_MAP_END },   /* normal speed */
      { MGN_KEY_8,         MGN_MAP_END },   /* reverse playback */
      { MGN_KEY_BRACKET_L, MGN_MAP_END },   /* jump back */
      { MGN_KEY_BRACKET_R, MGN_MAP_END } }; /* jump ahead */



static void mx_gameInit( void ) {

    int  p;
    
    mingin_registerButtonMapping( QUIT,
                                  mx_quitMapping );
    
    mingin_registerButtonMapping( FULLSCREEN_TOGGLE,
                                  mx_fullscreenMapping );

    for( p =  PLAYBACK_START_STOP;
         p <= PLAYBACK_JUMP_HALF_AHEAD;
         p ++ ) {
        
        mingin_registerButtonMapping(
            p,
            mx_playbackMappings[ p - PLAYBACK_START_STOP ] );
        }

    /* all buttons start out unpressed */
    for( p = QUIT;
         p < LAST_MAXIGIN_USER_ACTION;
         p ++ ) {
        
        mx_buttonsDown[ p ] = 0;
        }
    
    
    mx_areWeInMaxiginGameInitFunction = 1;
    
    maxiginGame_init();

    mx_areWeInMaxiginGameInitFunction = 0;

    mx_recordingCrashRecovery();

    mx_initRecording();
    }






char maxigin_registerButtonMapping( int                 inButtonHandle,
                                    const MinginButton  inMapping[] ) {

    /* push it up so it doesn't interfere with our mappings */
    inButtonHandle += LAST_MAXIGIN_USER_ACTION;

    return mingin_registerButtonMapping( inButtonHandle,
                                         inMapping );
    }



char maxigin_isButtonDown( int  inButtonHandle ) {
    
    /* push it up so it doesn't interfere with our mappings */
    inButtonHandle += LAST_MAXIGIN_USER_ACTION;

    return mingin_isButtonDown( inButtonHandle );
    }




typedef struct MaxiginMemRec {
        void        *pointer;
        int          numBytes;
        const char  *description;
    } MaxiginMemRec;


#define  MAXIGIN_MAX_MEM_RECORDS  1024

static  MaxiginMemRec  mx_memRecords[ MAXIGIN_MAX_MEM_RECORDS ];

static  int            mx_numMemRecords            =  0;
static  int            mx_totalMemoryRecordsBytes  =  0;


void maxigin_initRegisterStaticMemory( void        *inPointer,
                                       int          inNumBytes,
                                       const char  *inDescription ) {
    
    if( ! mx_areWeInMaxiginGameInitFunction ) {
        mingin_log( "Game tried to call maxigin_initRegisterStaticMemory "
                    "from outside of maxiginGame_init\n" );
        return;
        }
    
    if( mx_numMemRecords >= MAXIGIN_MAX_MEM_RECORDS ) {
        maxigin_logInt( "Game tried to register more than max memory records: ",
                        MAXIGIN_MAX_MEM_RECORDS );
        return;
        }
    mx_memRecords[ mx_numMemRecords ].pointer      =  inPointer;
    mx_memRecords[ mx_numMemRecords ].numBytes     =  inNumBytes;
    mx_memRecords[ mx_numMemRecords ].description  =  inDescription;

    mx_numMemRecords ++;
    mx_totalMemoryRecordsBytes += inNumBytes;
    }







static  const char   *mx_saveGameDataStoreName  =  "maxigin_save.bin";



static char *mx_getMemRecordsFingerprint( int  *outTotalMemBytes ) {
    
    enum{  FINGERPRINT_LENGTH  =  10,
           HEX_LENGTH          =  FINGERPRINT_LENGTH * 2 + 1  };
    
    static  unsigned char  fingerprintBuffer[ FINGERPRINT_LENGTH ];
    static  char           hexBuffer[ HEX_LENGTH ];

    MaxiginFlexHashState  s;
    int                   i;
    int                   totalNumBytes  =  0;

    
    maxigin_flexHashInit( &s,
                          FINGERPRINT_LENGTH,
                          fingerprintBuffer );
    
    for( i = 0;
         i < mx_numMemRecords;
         i ++ ) {
        
        totalNumBytes += mx_memRecords[i].numBytes;
        
        maxigin_flexHashAdd(
            &s,
            maxigin_stringLength( mx_memRecords[i].description ),
            (unsigned char *)( mx_memRecords[i].description ) );
        }
    *outTotalMemBytes = totalNumBytes;

    maxigin_hexEncode( FINGERPRINT_LENGTH,
                       fingerprintBuffer, 
                       hexBuffer );
    
    return hexBuffer;
    }




/*
  Reads a \0-terminated string from data store.

  inMaxBytes is the length of the buffer, including the \0 byte for termination.

  Returns 1 on success, 0 on failure.
*/
static char mx_readStringFromPersistData( int    inStoreReadHandle,
                                          int    inMaxBytes,
                                          char  *inBuffer ) {
    int  i        =  0;

    int  readNum  =  mingin_readPersistData(
                         inStoreReadHandle,
                         1,
                         (unsigned char *)&( inBuffer[i] ) );
    
    while( readNum == 1
           &&
           i < inMaxBytes - 1
           &&
           inBuffer[i] != '\0' ) {
        i++;
        readNum = mingin_readPersistData( inStoreReadHandle,
                                          1,
                                          (unsigned char *)&( inBuffer[i] ) );
        }
    if( inBuffer[i] != '\0'
        &&
        readNum == 1 ) {
        /* didn't find termination in data store
           because string was too long for buffer */
        mingin_log( "Error:  Buffer overflow when trying to read string from "
                    "persistent data store.\n" );
        return 0;
        }
    else if( inBuffer[i] != '\0'
             &&
             readNum == 0 ) {
        mingin_log( "Error:  Reached end of store when trying to read string "
                    "from persistent data store.\n" );
        return 0;
        }
    else if( inBuffer[i] != '\0'
             &&
             readNum == -1 ) {
        mingin_log( "Error:  Got read failure when trying to read string "
                    "from persistent data store.\n" );
        return 0;
        }

    return 1;
    }



/*
  Reads a \0-terminated short string (< 63 chars long) into a static buffer.

  Returns 0 on failure.
*/
static const char *mx_readShortStringFromPersistData( int  inStoreReadHandle ) {

    enum{  BUFFER_LEN  =  64  };
    
    static  char  buffer[ BUFFER_LEN ];
    
    char  success;
    
    
    success = mx_readStringFromPersistData( inStoreReadHandle,
                                            BUFFER_LEN,
                                            buffer );
    if( success ) {
        return buffer;
        }
    else {
        return 0;
        } 
    }



/*
  Reads a \0-terminated string representation of an int from data store.

  Returns 1 on success, 0 on failure.
*/
static char mx_readIntFromPersistData( int   inStoreReadHandle,
                                       int  *outInt ) {
    
    const char  *read  =  mx_readShortStringFromPersistData( inStoreReadHandle );

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
static char mx_writeStringToPeristentData( int          inStoreWriteHandle,
                                           const char  *inString ) {
    
    return mingin_writePersistData( inStoreWriteHandle,
                                    maxigin_stringLength( inString ) + 1,
                                    (unsigned char*)inString );
    }


/*
  Writes a \0-terminated string representation of an int to data store.

  Returns 1 on success, 0 on failure.
*/
static char mx_writeIntToPerisistentData( int  inStoreWriteHandle,
                                          int  inInt ) {
    
    return mx_writeStringToPeristentData( inStoreWriteHandle,
                                          maxigin_intToString( inInt ) );
    }



#define  MAXIGIN_PADDED_INT_LENGTH  12

static  unsigned char  mx_intPadding[ MAXIGIN_PADDED_INT_LENGTH ];



/*
  Writes a \0-terminated string representation of an int to data store,
  and pads it with \0 characters afterwards to fill paddedIntLength total.

  Returns 1 on success, 0 on failure.
*/
static char mx_writePaddedIntToPerisistentData( int  inStoreWriteHandle,
                                                int  inInt ) {
    
    const char  *intString  =  maxigin_intToString( inInt );
    char         success;
    int          b          =  0;
    int          len;
    
    success = mingin_writePersistData( inStoreWriteHandle,
                                       maxigin_stringLength( intString ),
                                       (unsigned char*)intString );

    if( ! success ) {
        return 0;
        }
    
    /* pad with \0     */
    len = maxigin_stringLength( intString );
    
    while( len < MAXIGIN_PADDED_INT_LENGTH ) {
        mx_intPadding[b] = '\0';
        len++;
        b++;
        }

    /* write padding out */
    return mingin_writePersistData( inStoreWriteHandle,
                                    b,
                                    mx_intPadding );
    }



/*
  Reads int and jumps ahead MAXIGIN_PADDED_INT_LENGTH total bytes, to skip
  all padding.

  Returns 1 on success, 0 on failure.
*/
static char mx_readPaddedIntFromPeristentData( int   inStoreReadHandle,
                                               int  *outInt ) {
    
    int  numRead  =  mingin_readPersistData( inStoreReadHandle,
                                             MAXIGIN_PADDED_INT_LENGTH,
                                             mx_intPadding );

    if( numRead != MAXIGIN_PADDED_INT_LENGTH ){
        return 0;
        }

    *outInt = maxigin_stringToInt( (char *)mx_intPadding );
    return 1;
    }



/* returns 1 on success, 0 on failure */
static char mx_saveGameToDataStore( int  inStoreWriteHandle ) {
    
    char  *fingerprint;
    int    numTotalBytes;
    char   success;
    int    i;

    fingerprint = mx_getMemRecordsFingerprint( &numTotalBytes );


    success = mx_writeIntToPerisistentData( inStoreWriteHandle,
                                            numTotalBytes );

    if( ! success ) {
        
MAXIGIN_SAVED_GAME_WRITE_FAILURE:
        
        maxigin_logString( "Failed to write to saved game data: ",
                           mx_saveGameDataStoreName );
        return 0;
        }


    success = mx_writeIntToPerisistentData( inStoreWriteHandle,
                                            mx_numMemRecords );

    if( ! success ) {
        goto MAXIGIN_SAVED_GAME_WRITE_FAILURE;
        }

    
    success = mx_writeStringToPeristentData( inStoreWriteHandle,
                                             fingerprint );
    
    if( ! success ) {
        goto MAXIGIN_SAVED_GAME_WRITE_FAILURE;
        }

    /* first write all the descriptions and sizes
       on loading, we can bail out if these don't match, before
       overwriting anything */
    for( i = 0;
         i < mx_numMemRecords;
         i ++ ) {
        
        const char *des = mx_memRecords[i].description;
    
        success = mx_writeStringToPeristentData( inStoreWriteHandle,
                                                 des );

        if( ! success ) {
            goto MAXIGIN_SAVED_GAME_WRITE_FAILURE;
            }
        
        success = mx_writeIntToPerisistentData( inStoreWriteHandle,
                                                mx_memRecords[i].numBytes );
        if( ! success ) {
            goto MAXIGIN_SAVED_GAME_WRITE_FAILURE;
            }
        }

    /* now write the actual memory regions */
    for( i = 0;
         i < mx_numMemRecords;
         i ++ ) {
        
        /* write numBytes from memory location into storage */
        success = mingin_writePersistData(
            inStoreWriteHandle,
            mx_memRecords[i].numBytes,
            (unsigned char*)mx_memRecords[i].pointer );
        
        if( ! success ) {
            goto MAXIGIN_SAVED_GAME_WRITE_FAILURE;
            }
        }

    return 1;
    }



static void mx_saveGame( void ) {
    
    int  outHandle;
    
    if( mx_numMemRecords == 0 ) {
        return;
        }
    
    outHandle = mingin_startWritePersistData( mx_saveGameDataStoreName );

    if( outHandle == -1 ) {
        maxigin_logString( "Failed to open saved game for writing: ",
                           mx_saveGameDataStoreName );
        return;
        }

    mx_saveGameToDataStore( outHandle );
    
    mingin_endWritePersistData( outHandle );

    mingin_log( "Saved game.\n" );
    }



/* returns 1 on success, 0 on failure */
static char mx_restoreStaticMemoryFromDataStore( int inStoreReadHandle ) {
    
    char        *fingerprint;
    int          numTotalBytes;
    int          readNumTotalBytes;
    int          readNumMemRecords;
    char         success;
    int          i;
    const char  *readFingerprint;

    if( mx_numMemRecords == 0 ) {
        return 0;
        }
    
    fingerprint = mx_getMemRecordsFingerprint( &numTotalBytes );
    
 
    success = mx_readIntFromPersistData( inStoreReadHandle,
                                         &readNumTotalBytes );

    if( ! success ) {
        mingin_log( "Failed to read total num bytes from save data.\n" );
        return 0;
        }

    if( readNumTotalBytes != numTotalBytes ) {
        mingin_log( "Save data does not match current total memory bytes, "
                    "ignoring.\n" );
        
        maxigin_logInt( "Save data has numTotalBytes = ",
                        readNumTotalBytes );
        
        maxigin_logInt( "Current live numTotalBytes = ",
                        numTotalBytes );
        
        return 0;
        }

    success = mx_readIntFromPersistData( inStoreReadHandle,
                                         &readNumMemRecords );

    if( ! success ) {
        mingin_log( "Failed to read num memory records from save data.\n" );
        return 0;
        }
    
    if( readNumMemRecords != mx_numMemRecords ) {
        mingin_log( "Save data does not match current mx_numMemRecords, "
                    "ignoring.\n" );
        
        maxigin_logInt( "Save data has mx_numMemRecords = ",
                        readNumMemRecords );
        
        maxigin_logInt( "Current live mx_numMemRecords = ",
                        mx_numMemRecords );
        
        return 0;
        }
    


    readFingerprint = mx_readShortStringFromPersistData( inStoreReadHandle );
    
    if( readFingerprint == 0 ) {
        mingin_log( "Failed to read fingerprint from save data.\n" );
        return 0;
        }
    
    
    if( ! maxigin_stringsEqual( fingerprint,
                                readFingerprint ) ) {
        
        mingin_log( "Save data does not match current memory fingerprint, "
                    "ignoring.\n" );
        
        maxigin_logString( "Save data has fingerprint = ",
                           readFingerprint );
        
        maxigin_logString( "Current live has fingerprint = ",
                           fingerprint );
        
        return 0;
        }

    /* now read the memory records from the saved data */

    /* first read the descriptions and sizes to make sure they all match */
    for( i = 0;
         i < mx_numMemRecords;
         i ++ ) {
        
        const char *liveDes = mx_memRecords[i].description;
        const char *readDes =
            mx_readShortStringFromPersistData( inStoreReadHandle );
        
        int readNumBytes;
        
        if( readDes == 0 ) {
            maxigin_logInt( "Failed to read saved description for record # = ",
                            i );
            return 0;
            }
        
        if( ! maxigin_stringsEqual( liveDes,
                                    readDes ) ) {
            maxigin_logInt( "Save data has wrong description for record # = ",
                            i );
            
            maxigin_logString( "Save data has description = ",
                               readDes );

            maxigin_logString( "Live description = ",
                               liveDes );
            
            return 0;
            }
        
        
        success = mx_readIntFromPersistData( inStoreReadHandle,
                                             &readNumBytes );
        
        if( ! success ) {
            maxigin_logInt( "Failed to read saved numBytes for record # = ",
                            i );
            return 0;
            }

        if( readNumBytes != mx_memRecords[i].numBytes ) {
            maxigin_logInt( "Save data has wrong numBytes for record # = ",
                            i );
            
            maxigin_logInt( "Save data has numBytes = ",
                            readNumBytes );

            maxigin_logInt( "Live numBytes = ",
                            mx_memRecords[i].numBytes );

            return 0;
            }

        }

    
    /* Now it's safe to reach the memory regions into memory
       since everything above matches */
    
    for( i = 0;
         i < mx_numMemRecords;
         i ++ ) {
        
        /* read numBytes from memory location into storage */
        int numRead = mingin_readPersistData(
            inStoreReadHandle,
            mx_memRecords[i].numBytes,
            (unsigned char*)mx_memRecords[i].pointer );
        
        if( numRead !=  mx_memRecords[i].numBytes ) {
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
    
    if( ! mx_areWeInMaxiginGameInitFunction ) {
        mingin_log( "Game tried to call "
                    "maxigin_initRestoreStaticMemoryFromLastRun "
                    "from outside of maxiginGame_init\n" );
        return;
        }
    
    readHandle = mingin_startReadPersistData( mx_saveGameDataStoreName,
                                              &storeSize );

    if( readHandle == -1 ) {
        maxigin_logString( "Failed to open saved game for reading: ",
                           mx_saveGameDataStoreName );
        return;
        }

    success = mx_restoreStaticMemoryFromDataStore( readHandle );

    mingin_endReadPersistData( readHandle );

    
    if( success ) {
        mingin_log( "Restored live memory from saved game data.\n" );
        }
    }







#if( MAXIGIN_ENABLE_RECORDING == 0 )
    /* recording is off, shrink our recording buffer down to nothing */
    #undef   MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES
    #define  MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES  1
#endif


/*
  buffer for our last state represented in the recording data store
  and our current state, used for computing the next diff.
*/
static unsigned char mx_recordingBuffers[2][
    MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES ];


static  const char  *mx_recordingDataStoreName         =
                                                   "maxigin_recording.bin";
static  const char  *mx_recordingIndexDataStoreName    =
                                                   "maxigin_recordingIndex.bin";
static  int          mx_latestRecordingIndex           =  -1;
static  int          mx_recordingDataStoreHandle       =  -1;
static  int          mx_recordingIndexDataStoreHandle  =  -1;

static  char         mx_diffRecordingEnabled           =  1;

static  int          mx_numDiffsSinceLastFullSnapshot  =  0;
static  int          mx_diffsBetweenSnapshots          =  60;

static  const char  *mx_recordingMagicFooter           =  "MX_RECORDING";


/*
  Copies snapshot of memory into the next rotating slot of  mx_recordingBuffers
  and updates mx_latestRecordingIndex to continue the rotation.
*/
static void mx_copyMemoryIntoRecordingBuffer( void ) {
    
    int             r;
    int             b           =  0;
    int             nextBuffer  =  mx_latestRecordingIndex + 1;
    unsigned char  *buffer;

    if( nextBuffer > 1 ) {
        nextBuffer = 0;
        }

    buffer =  mx_recordingBuffers[ nextBuffer ];
    
    
    if( ! mx_diffRecordingEnabled ) {
        return;
        }

    for( r = 0;
         r < mx_numMemRecords;
         r ++ ) {

        int             rb          =  0;
        int             recSize     =  mx_memRecords[r].numBytes;
        unsigned char  *recPointer  =
                            (unsigned char*)( mx_memRecords[r].pointer );
        
        while( rb < recSize ) {
            buffer[b] = recPointer[ rb ];
            b++;
            rb++;
            }
        }
    
    mx_latestRecordingIndex = nextBuffer; 
    }



static void mx_closeRecordingDataStores( void ) {

    if( mx_recordingDataStoreHandle != -1 ) {
        mingin_endWritePersistData( mx_recordingDataStoreHandle );
        }
    if( mx_recordingIndexDataStoreHandle != -1 ) {
        mingin_endWritePersistData( mx_recordingIndexDataStoreHandle );
        }

    mx_recordingDataStoreHandle = -1;
    mx_recordingIndexDataStoreHandle = -1;

    mx_recordingRunning = 0;
    }



/*
  Writes a full memory snapshot to mx_recordingDataStoreHandle

  Does NOT copy current memory state into rotating memory buffer
*/
static void mx_recordFullMemorySnapshot( void ) {
    
    int   r;
    int   startPos  =
              mingin_getPersistDataPosition( mx_recordingDataStoreHandle );
    char  success;

    if( startPos == -1 ) {
        mingin_log( "Failed to get current recording data store postion.\n" );

        mx_closeRecordingDataStores();
        return;
        }
    
        
    /* write the starting pos of this full snapshot into our index data store
       use a padded int so that we can jump by 12 bytes to go "frame by frame"
       through the index.
    */
    success =
        mx_writePaddedIntToPerisistentData( mx_recordingIndexDataStoreHandle,
                                            startPos );

    if( ! success ) {
        maxigin_logString(
            "Failed to write data block to recording index data: ",
            mx_recordingIndexDataStoreName );
        
        mx_closeRecordingDataStores();
        return;
        }
    

    /* write our full snapshot header */
    success = mx_writeStringToPeristentData( mx_recordingDataStoreHandle,
                                             "F" );

    if( ! success ) {
        mingin_log(
            "Failed to write full memory snapshot header in recording\n" );
        
        mx_closeRecordingDataStores();
        return;
        }
    
    for( r = 0;
         r < mx_numMemRecords;
         r ++ ) {
        
        int recSize = mx_memRecords[r].numBytes;
        unsigned char *recPointer = (unsigned char*)( mx_memRecords[r].pointer );

        success =
            mingin_writePersistData( mx_recordingDataStoreHandle,
                                     recSize,
                                     recPointer );

        if( ! success ) {
            maxigin_logString( "Failed to write data block to recording data: ",
                               mx_recordingDataStoreName );
            
            mx_closeRecordingDataStores();
            return;
            }
        }
    
    /* write the position of this block start, as a padded int.
       this will help us during reverse playback */
    success = mx_writePaddedIntToPerisistentData( mx_recordingDataStoreHandle,
                                                  startPos );

    if( ! success ) {
        mingin_log( "Failed to write recording full snapshot start position "
                    " at end of snapshot block.\n" );
        mx_closeRecordingDataStores();
        return;
        }
    }



/* Returns 1 if header found matching inTargetLetter, 0 if not */
static char mx_checkHeader( int         inStoreReadHandle,
                            const char  inTargetLetter ) {
    
    const char  *header  =
                     mx_readShortStringFromPersistData( inStoreReadHandle );

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
static char mx_restoreFromFullMemorySnapshot( int inStoreReadHandle ) {
    
    int   r;
    int   startPos;
    char  success;
    
    if( ! mx_checkHeader( inStoreReadHandle, 'F' ) ) {
        return 0;
        }
    

    for( r = 0;
         r < mx_numMemRecords;
         r ++ ) {
        
        int             recSize     =  mx_memRecords[r].numBytes;
        
        unsigned char  *recPointer  =
                            (unsigned char*)( mx_memRecords[r].pointer );
        
        int             numRead     =  mingin_readPersistData( inStoreReadHandle,
                                                               recSize,
                                                               recPointer );

        if( numRead != recSize ) {
            /* failed to read block */
            return 0;
            }
        }

    /* read all blocks */

    /* now read start position footer, just to get past it */
    success = mx_readPaddedIntFromPeristentData( inStoreReadHandle,
                                                 &startPos );

    if( ! success ) {
        return 0;
        }
    
    return 1;
    }


    
/*
  Writes a memory diff to mx_recordingDataStoreHandle

  Copies current memory state into rotating memory buffer
*/
static void mx_recordMemoryDiff( void ) {
    
    int   prevIndex    =  mx_latestRecordingIndex;
    int   newIndex     =  0;
    int   b;
    int   lastWritten  =  0;
    char  success;
    int   startPos;
    
    if( ! mx_diffRecordingEnabled ) {
        return;
        }

    startPos = mingin_getPersistDataPosition( mx_recordingDataStoreHandle );
    
    if( startPos == -1 ) {
        mingin_log( "Failed to get current recording data store postion.\n" );

        mx_closeRecordingDataStores();
        return;
        }
    
    mx_copyMemoryIntoRecordingBuffer();

    newIndex = mx_latestRecordingIndex;

    
    if( newIndex == prevIndex ) {
        mingin_log( "Failed to update mx_latestRecordingIndex when recording "
                    "memory diff." );
        
        mx_closeRecordingDataStores();
        return;
        }
    
        

    /* header for a diff */
    success = mx_writeStringToPeristentData( mx_recordingDataStoreHandle,
                                             "D" );

    if( ! success ) {
        mingin_log( "Failed to write memory diff header in recording\n" );
        
        mx_closeRecordingDataStores();
        return;
        }
    
    for( b = 0;
         b < MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES;
         b ++ ) {
        
        if( mx_recordingBuffers[prevIndex][b] !=
            mx_recordingBuffers[newIndex][b] ) {

            /* a byte has changed */
            
            unsigned char  xorValue  =
                mx_recordingBuffers[prevIndex][b]
                ^
                mx_recordingBuffers[newIndex][b];
            
                
            /* write its position offset from the previous one recorded */
            success = mx_writeIntToPerisistentData( mx_recordingDataStoreHandle,
                                                    b - lastWritten );

            if( ! success ) {
                mingin_log( "Failed to write diff position in recording\n" );
        
                mx_closeRecordingDataStores();
                return;
                }
            
            lastWritten = b;

            /* write its xor with previous value */
            success = mingin_writePersistData(
                mx_recordingDataStoreHandle, 1,
                &( xorValue ) );
            
            if( ! success ) {
                mingin_log( "Failed to write diff byte in recording\n" );
        
                mx_closeRecordingDataStores();
                return;
                }
            }
        }

    /* write -1 to end of diff, so that we know it's over
       (each line in the diff starts with a valid non-negative position
       in our memory snapshot */
    
    success = mx_writeIntToPerisistentData( mx_recordingDataStoreHandle,
                                            -1 );
    
    if( ! success ) {
        mingin_log( "Failed to write diff footer in recording\n" );
        
        mx_closeRecordingDataStores();
        return;
        }

    
    /* write the position of this block start, as a padded int.
       this will help us during reverse playback */
    success = mx_writePaddedIntToPerisistentData( mx_recordingDataStoreHandle,
                                                  startPos );

    if( ! success ) {
        mingin_log( "Failed to write recording diff snapshot start position "
                    " at end of snapshot block.\n" );
        mx_closeRecordingDataStores();
        return;
        } 
    }



/*
  Restores memory from diff at current position in a data store.

  Returns 1 on success, 0 on failure.
*/
static char mx_restoreFromMemoryDiff( int inStoreReadHandle ) {
    
    /* I learned it by watching you, John */
    char            success;
    int             readInt;
    int             curRecord          =  0;
    int             curRecordByte      =  0;
    unsigned char  *curRecordPointer;
    int             numRead;
    int             startPos;
    
    
    if( ! mx_checkHeader( inStoreReadHandle, 'D' ) ) {
        return 0;
        }

    if( mx_numMemRecords == 0 ) {
        /* trying to restore diff into no live memory records */
        return 0;
        }

    curRecordPointer = (unsigned char*)( mx_memRecords[ curRecord ].pointer );
    
    

    success = mx_readIntFromPersistData( inStoreReadHandle,
                                         &readInt );

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

        unsigned char  xorValue;
        
        curRecordByte += readInt;

        while( curRecordByte >= mx_memRecords[ curRecord ].numBytes ) {
            /* gone past the end of our current record
               start marching into next record from 0 in that record */
            curRecordByte -= mx_memRecords[ curRecord ].numBytes;
                
            curRecord ++;
            if( curRecord >= mx_numMemRecords ) {
                /* diff includes offsets that go beyond our last
                   live memory recor */
                return 0;
                }
            curRecordPointer =
                (unsigned char*)( mx_memRecords[ curRecord ].pointer );
            }
        
        /* we've found a place for the offset read from the diff to land */

        numRead = mingin_readPersistData(
            inStoreReadHandle,
            1,
            &xorValue );

        if( numRead != 1 ) {
            return 0;
            }
        
        /* apply our xor */
        curRecordPointer[ curRecordByte ] ^= xorValue;

        success = mx_readIntFromPersistData( inStoreReadHandle,
                                             &readInt );
        
        if( ! success ) {
            return 0;
            }
        }

    /* got here, then we read the -1 terminator int at the end of
       our diff block */
    
    /* now read start position footer, just to get past it */
    success = mx_readPaddedIntFromPeristentData( inStoreReadHandle,
                                                 &startPos );

    if( ! success ) {
        return 0;
        }

    

    return 1;   
    }







    


static void mx_initRecording( void ) {
    
    int  b;
    int  i;
    int  success;
    
    mx_recordingRunning = 0;
    
    if( ! MAXIGIN_ENABLE_RECORDING ) {
        return;
        }
    if( mx_numMemRecords == 0 ) {
        return;
        }
    if( MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES <
        mx_totalMemoryRecordsBytes ) {
        
        mx_diffRecordingEnabled = 0;
        }

    maxigin_logString( "Starting recording into data store: ",
                       mx_recordingDataStoreName );
    
    mx_recordingDataStoreHandle =
        mingin_startWritePersistData( mx_recordingDataStoreName );

    if( mx_recordingDataStoreHandle != -1 ) {
        mx_recordingIndexDataStoreHandle =
            mingin_startWritePersistData( mx_recordingIndexDataStoreName );
        }

    if( mx_recordingDataStoreHandle == -1 ||
        mx_recordingIndexDataStoreHandle == -1 ) {

        mingin_log( "Failed to open recording data stores for writing\n" );

        mx_closeRecordingDataStores();
        return;
        }

    /* start by writing a normal saved game to the start of the recording data
       which is our current state that should be restored before playback */
    success = mx_saveGameToDataStore( mx_recordingDataStoreHandle );

    if( !success ) {
        mingin_log( "Failed to write save game header to recording data.\n" );
        
        mx_closeRecordingDataStores();
        return;
        }
    
    /* zero out both buffers */
    for( i = 0;
         i < 2;
         i ++ ) {
        
        for( b = 0;
             b < MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES;
             b ++ ) {
            
            mx_recordingBuffers[i][b] = 0;
            }
        }
    
    mx_recordFullMemorySnapshot();

    mx_numDiffsSinceLastFullSnapshot = 0;
    
    mx_copyMemoryIntoRecordingBuffer();

    mx_recordingRunning = 1;
    }



static void mx_stepRecording( void ) {
    if( ! MAXIGIN_ENABLE_RECORDING
        ||
        ! mx_recordingRunning ) {
        return;
        }

    
    if( mx_numDiffsSinceLastFullSnapshot < mx_diffsBetweenSnapshots ) {
        mx_recordMemoryDiff();
        mx_numDiffsSinceLastFullSnapshot ++;
        }
    else {
        /* always record a diff right before our snapshot so that
           we can play the change recorded in our snapshot backwards */
        mx_recordMemoryDiff();
        
        mx_recordFullMemorySnapshot();
        
        mx_numDiffsSinceLastFullSnapshot = 0;
        }
    }



/*
  Copies bytes from one open read data store into the end
  of an open write data store.

  Returns 1 on success, 0 on failure.
*/
static char mx_copyIntoDataStore( int inStoreReadHandle,
                                  int inStoreWriteHandle,
                                  int inNumBytesToCopy ) {
    
    enum{  BUFFER_LEN  =  512  };
    
    static  unsigned char  buffer[ BUFFER_LEN ];

    int  numCopied  =  0;
            

    while( numCopied < inNumBytesToCopy ) {
        
        int   numLeft      =  inNumBytesToCopy - numCopied;
        int   numThisTime  =  numLeft;
        int   numRead;
        char  success;
        
        if( numThisTime > BUFFER_LEN ) {
            numThisTime = BUFFER_LEN;
            }
        
        numRead = mingin_readPersistData( inStoreReadHandle,
                                          numThisTime,
                                          buffer );

        if( numRead == -1 || numRead < numThisTime ) {
            /* error in read, or reached end of data before we got
               inNumBytesToCopy */
            return 0;
            }
        
        success = mingin_writePersistData( inStoreWriteHandle,
                                           numRead,
                                           buffer );

        if( ! success ) {
            return 0;
            }
        numCopied += numRead;
        }
    
    return 1;
    }



static void mx_finalizeRecording( void ) {
    
    if( ! MAXIGIN_ENABLE_RECORDING ) {
        return;
        }

    if( mx_recordingRunning ) {
        
        int   indexLength;
        char  success;
        int   recordingIndexReadHandle;
        
        mingin_endWritePersistData( mx_recordingIndexDataStoreHandle );
        mx_recordingIndexDataStoreHandle = -1;

        recordingIndexReadHandle =
            mingin_startReadPersistData( mx_recordingIndexDataStoreName,
                                         &indexLength );

        if( recordingIndexReadHandle == -1 ) {
            mingin_log( "Failed to re-open recording index data at end "
                        "of recording data.\n" );
            
            mingin_endWritePersistData( mx_recordingDataStoreHandle );
            mx_recordingDataStoreHandle = -1;
            return;
            }
        
        success = mx_copyIntoDataStore( recordingIndexReadHandle,
                                        mx_recordingDataStoreHandle,
                                        indexLength );

        mingin_endReadPersistData( recordingIndexReadHandle );
            
        if( ! success ) {
            mingin_log( "Failed to copy recording index into end "
                        "of recording data.\n" );
            mingin_endWritePersistData( mx_recordingDataStoreHandle );
            mx_recordingDataStoreHandle = -1;
            
            return;
            }

        /* successfully added index to end, can delete index now */
        mingin_deletePersistData( mx_recordingIndexDataStoreName );
        

        /* now append length of index
           padded so we know how far to jump back to read it during playback */
        success = mx_writePaddedIntToPerisistentData(
            mx_recordingDataStoreHandle,
            indexLength );

        if( ! success ) {
            mingin_log( "Failed write length of index into end "
                        "of recording data.\n" );
            mingin_endWritePersistData( mx_recordingDataStoreHandle );
            mx_recordingDataStoreHandle = -1;
            
            return;
            }

        success =  mingin_writePersistData(
            mx_recordingDataStoreHandle,
            /* include the \0 termination */
            maxigin_stringLength( mx_recordingMagicFooter ) + 1,
            (unsigned char*)mx_recordingMagicFooter );

        if( ! success ) {
            mingin_log( "Failed write magic footer into end "
                        "of recording data.\n" );
            mingin_endWritePersistData( mx_recordingDataStoreHandle );
            mx_recordingDataStoreHandle = -1;
            
            return;
            }
            
            
        mingin_endWritePersistData( mx_recordingDataStoreHandle );
        mx_recordingDataStoreHandle = -1;

        maxigin_logString( "Game recording finalized: ",
                           mx_recordingDataStoreName );
        }

    mx_closeRecordingDataStores();
    }



/* returns a unique recovery file name in a static buffer */
static const char *mx_getRecordingRecoveryFileName( void ) {
    int          recoveryNumber;
    const char  *settingName      =  "maxigin_nextRecoveryNumber.bin";

    const char  *returnVal;
    
    recoveryNumber = maxigin_readIntSetting( settingName, 0 );


    returnVal = maxigin_stringConcat(
        maxigin_stringConcat( "maxigin_crashRecording_",
                              maxigin_intToString( recoveryNumber ) ),
        ".bin" );
    

    recoveryNumber++;

    maxigin_writeIntSetting( settingName, recoveryNumber );

    return returnVal;
    }



void mx_recordingCrashRecovery( void ) {

    int          recordingReadHandle;
    int          indexReadHandle;
    const char  *recoveryFileName;
    int          recoveryWriteHandle;
    int          recordingLength;
    int          indexLength;
    char         success;
    
    indexReadHandle =
        mingin_startReadPersistData( mx_recordingIndexDataStoreName,
                                     &indexLength );

    if( indexReadHandle == -1 ) {
        /* no index found, nothing to recover */
        return;
        }

    maxigin_logString( "Found recording index file, "
                       "attempting crash recovery: ",
                       mx_recordingIndexDataStoreName );

    recordingReadHandle =
        mingin_startReadPersistData( mx_recordingDataStoreName,
                                     &recordingLength );

    if( recordingReadHandle == -1 ) {
        maxigin_logString( "Failed to open recording file, "
                           "aborting crash recovery: ",
                           mx_recordingDataStoreName );
        mingin_endReadPersistData( indexReadHandle );
        return;
        }

    /* fixme:
       open crash file for writing, etc.*/

    recoveryFileName = mx_getRecordingRecoveryFileName();
    
    recoveryWriteHandle =
        mingin_startWritePersistData( recoveryFileName );

    if( recoveryWriteHandle == -1 ) {
        maxigin_logString( "Failed to open recording recovery file "
                           "for writing: ",
                           recoveryFileName );
        mingin_endReadPersistData( indexReadHandle );
        mingin_endReadPersistData( recordingReadHandle );
        return;
        }



    success = mx_copyIntoDataStore( recordingReadHandle,
                                    recoveryWriteHandle,
                                    recordingLength );

    mingin_endReadPersistData( recordingReadHandle );
            
    if( ! success ) {
        mingin_log( "Failed to copy recording data into recovery file.\n" );

        mingin_endReadPersistData( indexReadHandle );
        
        mingin_endWritePersistData( recoveryWriteHandle );
            
        return;
        }

    success = mx_copyIntoDataStore( indexReadHandle,
                                    recoveryWriteHandle,
                                    indexLength );

    mingin_endReadPersistData( indexReadHandle );
            
    if( ! success ) {
        mingin_log( "Failed to copy recording index into recovery file.\n" );
        
        mingin_endWritePersistData( recoveryWriteHandle );
            
        return;
        }

    /* successfully added index to end, can delete index now */
    mingin_deletePersistData( mx_recordingIndexDataStoreName );
        

    /* now append length of index
       padded so we know how far to jump back to read it during playback */
    success = mx_writePaddedIntToPerisistentData(
        recoveryWriteHandle,
        indexLength );

    if( ! success ) {
        mingin_log( "Failed write length of index into end "
                    "of recording recovery file.\n" );
        mingin_endWritePersistData( recoveryWriteHandle );
            
        return;
        }

    success =  mingin_writePersistData(
        recoveryWriteHandle,
        /* include the \0 termination */
        maxigin_stringLength( mx_recordingMagicFooter ) + 1,
        (unsigned char*)mx_recordingMagicFooter );

    if( ! success ) {
        mingin_log( "Failed write magic footer into end "
                    "of recording recovery file.\n" );
        mingin_endWritePersistData( recoveryWriteHandle );
            
        return;
        }
            
            
    mingin_endWritePersistData( recoveryWriteHandle );

    maxigin_logString( "Recording recovery saved into: ",
                       recoveryFileName );
    }







static  const char  *mx_playbackDataStoreName           =
                                                        "maxigin_playback.bin";
static  int          mx_playbackDataStoreHandle         =  -1;

static  int          mx_playbackDataLength;

static  int          mx_playbackFullSnapshotLastPlayed  =  0;
static  int          mx_playbackIndexStartPos           =  0;
static  int          mx_playbackNumFullSnapshots        =  0;


/* returns 1 on success, 0 on failure */
static char mx_seekAndReadInt( int   inStoreReadHandle,
                               int   inPos,
                               int  *outInt ) {
    
    char  success  =  mingin_seekPersistData( inStoreReadHandle,
                                              inPos );
    
    if( ! success ) {
        return 0;
        }

    return mx_readIntFromPersistData( inStoreReadHandle,
                                      outInt );
    }
    


static char mx_initPlayback( void ) {
    
    char   success;
    int    indexLengthDataPos;
    int    indexLength;
    int    magicFooterDataPos;
    char   magicFooterBuffer[ 20 ];
    int    numRead;
    
    int    firstFullSnapshotDataPos;

    mx_playbackRunning    =  0;
    mx_playbackSpeed      =  1;
    mx_playbackPaused     =  0;
    mx_playbackDirection  =  1;
    
    if( mx_numMemRecords == 0 ) {
        return 0;
        }
    
    /* user copies "playback" data store into place if they want playback
       
       fixme:  eventually, there's some menu option to start playback,
       and we shouldn't be overwriting last recording with new one
       (last recording should be copied to playback store at startup,
       and then they can select "playback" from the menu) */
    
    mx_playbackDataStoreHandle =
        mingin_startReadPersistData( mx_playbackDataStoreName,
                                     &mx_playbackDataLength );
    
    if( mx_playbackDataStoreHandle == -1 ) {
        /* store doesn't exist, no playback */
        return 0;
        }

    maxigin_logString( "Loading save data header from playback data store: ",
                       mx_playbackDataStoreName );
    
    success = mx_restoreStaticMemoryFromDataStore( mx_playbackDataStoreHandle );

    if( ! success ) {
        mingin_log( "Failed to restore state from saved data "
                    "in playback data store." );
        mingin_endReadPersistData( mx_playbackDataStoreHandle );
        return 0;
        }

    /* jump to end and make sure magic footer is there */
    magicFooterDataPos = mx_playbackDataLength
                         - maxigin_stringLength( mx_recordingMagicFooter )
                         - 1;
    
    if( magicFooterDataPos < 0 ) {
        mingin_log( "Playback file too short to even contain magic footer.\n" );
        
        mingin_endReadPersistData( mx_playbackDataStoreHandle );
        return 0;
        }
    
    success = mingin_seekPersistData( mx_playbackDataStoreHandle,
                                      magicFooterDataPos );
    
    if( ! success ) {
        maxigin_logInt( "Failed to seek to this position to read magic footer "
                        "in playback data store: ",
                        magicFooterDataPos );
        
        mingin_endReadPersistData( mx_playbackDataStoreHandle );
        return 0;
        }

    numRead = mingin_readPersistData( mx_playbackDataStoreHandle,
                                      sizeof( magicFooterBuffer ),
                                      (unsigned char*)magicFooterBuffer );

    if( numRead != maxigin_stringLength( mx_recordingMagicFooter ) + 1
        ||
        ! maxigin_stringsEqual( mx_recordingMagicFooter,
                                magicFooterBuffer ) ) {

        maxigin_logString( "Failed to find magic footer string at end of "
                           "playback file: ", mx_recordingMagicFooter );
        
        mingin_endReadPersistData( mx_playbackDataStoreHandle );
        return 0;
        }
        
    
    /* jump to end and read padded int */
    indexLengthDataPos = magicFooterDataPos - MAXIGIN_PADDED_INT_LENGTH;

    if( indexLengthDataPos < 0 ) {
        mingin_log( "Playback file too short to even contain index lenth.\n" );
        
        mingin_endReadPersistData( mx_playbackDataStoreHandle );
        return 0;
        }

    
    success = mx_seekAndReadInt( mx_playbackDataStoreHandle,
                                 indexLengthDataPos,
                                 &indexLength );
    
    if( ! success ) {
        maxigin_logInt( "Failed to seek to this position and read index "
                        "length in playback data store: ",
                        indexLengthDataPos );
        
        mingin_endReadPersistData( mx_playbackDataStoreHandle );
        return 0;
        }

    mx_playbackIndexStartPos = indexLengthDataPos - indexLength;

    success = mx_seekAndReadInt( mx_playbackDataStoreHandle,
                                 mx_playbackIndexStartPos,
                                 &firstFullSnapshotDataPos );

    if( ! success ) {
        maxigin_logInt( "Failed to seek to this position and read first "
                        "index entry in playback data store: ",
                        mx_playbackIndexStartPos );
        
        mingin_endReadPersistData( mx_playbackDataStoreHandle );
        return 0;
        }

    /* now jump to that first full snapshot */

    success = mingin_seekPersistData( mx_playbackDataStoreHandle,
                                      firstFullSnapshotDataPos );

    if( ! success ) {
        maxigin_logInt( "Failed to seek to this position and read first "
                        "full snapshot in playback data store: ",
                        firstFullSnapshotDataPos );
        
        mingin_endReadPersistData( mx_playbackDataStoreHandle );
        return 0;
        }

    success = mx_restoreFromFullMemorySnapshot( mx_playbackDataStoreHandle );

    if( ! success ) {

        maxigin_logInt( "Failed to restore first full memory snapshot from "
                        "playback data store at this position: ",
                        firstFullSnapshotDataPos );
        return 0;
        }

    
    mx_playbackFullSnapshotLastPlayed = 0;
    mx_playbackNumFullSnapshots = indexLength / MAXIGIN_PADDED_INT_LENGTH;
    
    maxigin_logInt( "Playback started successfully with num snapshots: ",
                    mx_playbackNumFullSnapshots );
    
    mx_playbackRunning = 1;
    
    return 1;
    }


static void mx_playbackEnd( void ) {
    if( mx_playbackDataStoreHandle != -1 ) {
        mingin_endReadPersistData( mx_playbackDataStoreHandle );
        mx_playbackDataStoreHandle = -1;
        }
    mx_playbackRunning = 0;
    }






static char mx_playbackSpeedStep( void ) {
    /* failure of a single step means failure of the whole thing */

    static  int   stepsSinceLastPlaybackStep  =  0;
            char  success                     =  1;

        
    if( ! mx_playbackRunning ) {
        return 0;
        }

    if( mx_playbackPaused ) {
        return 1;
        }
    
    if( mx_playbackSpeed >= 1 ) {
        int i;
        /* we can't skip steps because diffs are accumulative  */
        for( i = 0;
             i < mx_playbackSpeed;
             i ++ ) {
            
            success = success && mx_playbackStep();
            }
        }
    else if( mx_playbackSpeed < 0 ) {
        /* negative speeds mean fractional */
        int stepsPerPlaybackStep = - mx_playbackSpeed;

        if( stepsSinceLastPlaybackStep >= stepsPerPlaybackStep ) {
            success = success && mx_playbackStep();
            stepsSinceLastPlaybackStep = 0;
            }
        else {
            stepsSinceLastPlaybackStep++;
            }
        }
    
    return success;
    }



static char mx_playbackStepForward( void );

static char mx_playbackStepBackward( void );



static char mx_playbackStep( void ) {
    if( mx_playbackDirection == 1 ) {
        return mx_playbackStepForward();
        }
    else if( mx_playbackDirection == -1 ) {
        return mx_playbackStepBackward();
        }
    return 0;    
    }



static char mx_playbackStepForward( void ) {
        
    int   curDataPos;
    char  success;
    
    if( ! mx_playbackRunning ) {
        return 0;
        }

    /* 
       try to read a diff from current location
       if that fails, rewind and try to read full snapshot
    */
    curDataPos = mingin_getPersistDataPosition( mx_playbackDataStoreHandle );

    if( curDataPos == -1 ) {
        mingin_log( "Playback failed to get current position from playback "
                    "data source.\n" );
        mx_playbackEnd();
        return 0;
        }

    success = mx_restoreFromMemoryDiff( mx_playbackDataStoreHandle );

    if( ! success ) {
        /* diff reading failed
           try reading a whole snapshot */

        if( mx_playbackFullSnapshotLastPlayed ==
            mx_playbackNumFullSnapshots - 1 ) {
            
            /* reached end */
            maxigin_logInt( "Reached end of playback with num snapshots: ",
                            mx_playbackNumFullSnapshots );
            mx_playbackEnd();
            return 0;
            }

        /* rewind */
        success = mingin_seekPersistData( mx_playbackDataStoreHandle,
                                          curDataPos );

        if( !success ) {
            mingin_log( "Seek-back failed in playback data source.\n" );

            mx_playbackEnd();
            return 0;
            }
        success = mx_restoreFromFullMemorySnapshot( mx_playbackDataStoreHandle );

        if( !success ) {
            mingin_log( "Neither full-memory snapshot nor partial diff "
                        "restored successfully from playback data source.\n" );
            mx_playbackEnd();
            return 0;
            }
        
        mx_playbackFullSnapshotLastPlayed ++;
        
        maxigin_logInt( "Just played snapshot: ",
                        mx_playbackFullSnapshotLastPlayed );

        /* Snapshot is always a redundant frame when playing forward
           (it is preceeded by a diff that takes us to the exact memory
           state in the snapshot ).
           Thus, if we failed to read a diff, and found a snapshot
           instead, we should go on to the NEXT diff and apply it now
           to actually take a step. */

        success = mx_restoreFromMemoryDiff( mx_playbackDataStoreHandle );

        if( !success ) {
            mingin_log( "Failed to restore from the next diff after "
                        "our just-played snapshot." );
            mx_playbackEnd();
            return 0;
            }
        }
    
    
    return 1;
    }




static char mx_playbackStepBackward( void ) {
        
    int   curDataPos;
    char  success;
    int   blockStartPos;
    
    if( ! mx_playbackRunning ) {
        return 0;
        }

    
    curDataPos = mingin_getPersistDataPosition( mx_playbackDataStoreHandle );


    /* read the start pos from the end of the block before this position */

    curDataPos -= MAXIGIN_PADDED_INT_LENGTH;

    success = mingin_seekPersistData( mx_playbackDataStoreHandle,
                                      curDataPos );

    if( ! success ) {
        mingin_log( "Reverse playback failed to seek back to read start "
                    "position from previous block.\n" );
        mx_playbackEnd();
        return 0;
        }

    success = mx_readIntFromPersistData( mx_playbackDataStoreHandle,
                                         &blockStartPos );

    if( ! success ) {
        mingin_log( "Reverse playback failed to read start position "
                    "from previous block.\n" );
        mx_playbackEnd();
        return 0;
        }

    success = mingin_seekPersistData( mx_playbackDataStoreHandle,
                                      blockStartPos );

    if( ! success ) {
        mingin_log( "Reverse playback failed to seek back to start "
                    "of previous block.\n" );
        mx_playbackEnd();
        return 0;
        }

    success = mx_restoreFromMemoryDiff( mx_playbackDataStoreHandle );


    if( success ) {
        /* diff reading success */
        
        /* rewind, so we're ready for next reverse playback step */
        success = mingin_seekPersistData( mx_playbackDataStoreHandle,
                                          blockStartPos );

        if( !success ) {
            mingin_log( "Seek-back failed in playback data source.\n" );

            mx_playbackEnd();
            return 0;
            }
        }
    else {
        /* diff reading failed
           try reading a whole snapshot */

        if( mx_playbackFullSnapshotLastPlayed ==
            0 ) {
            
            /* reached start (reverse end) */
            mingin_log( "Reached start during reverse playback\n" );
            
            mx_playbackEnd();
            return 0;
            }

        /* rewind */
        success = mingin_seekPersistData( mx_playbackDataStoreHandle,
                                          blockStartPos );
        if( !success ) {
            mingin_log( "Seek-back failed in playback data source.\n" );

            mx_playbackEnd();
            return 0;
            }
        
        success = mx_restoreFromFullMemorySnapshot( mx_playbackDataStoreHandle );

        if( !success ) {
            mingin_log( "Neither full-memory snapshot nor partial diff "
                        "restored successfully from playback data source.\n" );
            mx_playbackEnd();
            return 0;
            }
        
        mx_playbackFullSnapshotLastPlayed --;
        
        maxigin_logInt( "Just reverse-played snapshot: ",
                        mx_playbackFullSnapshotLastPlayed );

        /* Snapshot is always a redundant frame when playing forward/backward
           (it is preceeded by a diff that takes us to the exact memory
           state in the snapshot ).
           Thus, if we failed to read a diff, and found a snapshot
           instead, we should go back to the previous diff and apply it now
           to actually take a step backward. */

        
        /* rewind first, to prepare for next backward step*/
        success = mingin_seekPersistData( mx_playbackDataStoreHandle,
                                          blockStartPos );
        if( !success ) {
            mingin_log( "Seek-back failed in playback data source.\n" );

            mx_playbackEnd();
            return 0;
            }

        /* now just take another backward step, which should be
           a diff step before this full snapshot */
        success = mx_playbackStepBackward();

        if( !success ) {
            mingin_log( "Failed to reverse-play diff step that should "
                        "occur before a snapshot." );
            mx_playbackEnd();
            return 0;
            }
        }
    
    
    return 1;
    }



static void mx_playbackJumpToFullSnapshot( int inFullSnapshotIndex ) {

    int   indexJumpPos  =  mx_playbackIndexStartPos +
                           MAXIGIN_PADDED_INT_LENGTH * inFullSnapshotIndex;
    int   readPos;
    char  success;
    
    success = mingin_seekPersistData( mx_playbackDataStoreHandle,
                                      indexJumpPos );

    if( ! success ) {
        maxigin_logInt( "Playback jump failed to seek into index at pos: ",
                        indexJumpPos );
        mx_playbackEnd();
        return;
        }
    
    success = mx_readIntFromPersistData( mx_playbackDataStoreHandle,
                                         &readPos );
    
    if( ! success ) {
        mingin_log( "Playback jump failed to read jump pos from index\n" );
        mx_playbackEnd();
        return;
        }

    success = mingin_seekPersistData( mx_playbackDataStoreHandle,
                                      readPos );

    if( ! success ) {
        maxigin_logInt( "Playback jump failed to seek to full frame at pos: ",
                        readPos );
        mx_playbackEnd();
        return;
        }
    mx_playbackFullSnapshotLastPlayed = inFullSnapshotIndex - 1;

    if( mx_playbackPaused ) {
        /* step now, to insta-jump to our jump snapshot and apply it
           if not paused, the next play step will do this anyway,
           and we don't want to double-step */

        /* note that we force FORWARD here, regardless of playback direction
           to fully apply our snapshot */
        mx_playbackStepForward();
        }
    else if( mx_playbackDirection == -1 ) {
        /* not paused, but playing in reverse */

        /* force a FORWARD step here to fully apply our snapshot */

        mx_playbackStepForward();
        }
    }



static void mx_playbackJumpHalfAhead( void ) {

    int numToJump =
        ( mx_playbackNumFullSnapshots - mx_playbackFullSnapshotLastPlayed ) / 2;

    if( numToJump > 0 ) {
        mx_playbackJumpToFullSnapshot(
            mx_playbackFullSnapshotLastPlayed + numToJump );
        }
    }



static void mx_playbackJumpHalfBack( void ) {

    int destToJump =
        ( mx_playbackFullSnapshotLastPlayed ) / 2;

    if( destToJump < mx_playbackFullSnapshotLastPlayed ) {
        mx_playbackJumpToFullSnapshot( destToJump );
        }
    }











void maxigin_logString( const char  *inLabel,
                        const char  *inVal ) {
    
    enum{  BUFFER_LEN  =  256  };
    
    static  char  buffer[ BUFFER_LEN ];

    int  i  =  0;
    int  j  =  0;

    
    while( i < BUFFER_LEN - 2
           &&
           inLabel[i] != '\0' ) {
        
        buffer[i] = inLabel[i];
        i++;
        }
    
    while( i < BUFFER_LEN - 2
           &&
           inVal[j] != '\0' ) {
        
        buffer[i] = inVal[j];
        i++;
        j++;
        }
    
    buffer[i] = '\n';
    buffer[i+1] = '\0';

    mingin_log( buffer );
    }



void maxigin_logInt( const char  *inLabel,
                     int          inVal ) {
    
    const char  *valString  =  maxigin_intToString( inVal );

    maxigin_logString( inLabel, valString );
    }



int maxigin_stringLength( const char  *inString ) {
    
    int  len  =  0;
    
    while( inString[len] != '\0' ) {
        len++;
        }
    return len;
    }



char maxigin_stringsEqual( const char  *inStringA,
                           const char  *inStringB ) {
    
    int i  =  0;
    
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







const char *maxigin_intToString( int  inInt ) {

    static  char  buffer[20];

    unsigned int  c            =  0;
                  /* start with billions */
    int           divisor      =  1000000000;
    const char   *formatError  =  "[int_format_error]";
                  /* skip 0 digits until our first non-zero digit */
    int           qLowerLimit  =  1;

    
    if( inInt == 0 ) {
        return "0";
        }
    if( inInt < 0 ) {
        buffer[c] = '-';
        c++;
        inInt *= -1;
        }
    while( divisor >= 1 ) {
        
        int  q  =  inInt / divisor;

        if( q >= qLowerLimit ) {
            if( q > 9 ) {
                return formatError;
                }
            if( c >= sizeof( buffer ) - 1 ) {
                /* out of room? */
                return formatError;
                }
            
            buffer[c] = (char)( '0' + q );
            c++;
            /* we've seen at least one non-zero digit,
               so start allowing zeros now */
            qLowerLimit = 0;
            }
        inInt -= q * divisor;
        divisor /= 10;
        }
    
    /* terminate */
    buffer[c] = '\0';
    
    return buffer;  
    }



int maxigin_stringToInt( const char  *inString ) {
    
    int  sign  =  1;
    int  i     =  0;
    int  val   =  0;
    
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



static const unsigned char mx_flexHashTable[256] = {
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



void maxigin_flexHashInit( MaxiginFlexHashState  *inState,
                           int                    inHashLength,
                           unsigned char         *inHashBuffer ) {

    /*
      The following code inits the hash buffer with the following properties:

      1. Each freshly inited has buffer of a different length contains
         different values.

      2. Every freshly inited has buffer starts with the byte 0x77

      3. For very long hash buffers, the repeat cycle length of data in the
         fresly inited buffer is very very long.  This has been tested up
         to buffers with 100,000,000 bytes with no cycling.
    */
    
    int            j;
    
    unsigned char  i;
    unsigned char  n;
    unsigned char  k;
    unsigned char  m;
    unsigned char  index;
    
    int            run;
    
    int            hashLength  =  inHashLength;
    unsigned int   jBits;
    unsigned char  jBitsChar;
    
    i = 0;
    k = 199;
    n = 17;
    m = 107;
    
    /* zero our buffer to start */
    for( j = 0;
         j < hashLength;
         j ++ ) {
        
        inHashBuffer[j] = 0;
        }

    /* run twice, and xor second run into bytes from first
       we xor into our all-0 buffer in the first run */
    for( run = 0;
         run < 2;
         run++ ) {
        
        /* offset each run by 1, in case anything about the cycling
           of our variables lines up perfectly with hashLength.
           This also means that we only do one run if hashLength = 1 */
        
        for( j = run;
             j < hashLength;
             j ++ ) {
            
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
               dramatically, due to the mx_flexHashTable lookups.
           
               The result is that we periodically jump to a different spot in
               the table and start incrementally walking from there,
               which makes our cycle period extremely long. */
        

            /* don't assume char is not larger than 8 bits
               so we can't count on wrap-around behavior above 255 */

            index = (unsigned char)( ( i +
                                       mx_flexHashTable[ k ] +
                                       mx_flexHashTable[m] )
                                     & 0xFF );

        
            n = n ^ mx_flexHashTable[ index ] ^ jBitsChar ^ inHashBuffer[j];
        
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
    n = n ^ mx_flexHashTable[i];

    inState->j              =  0;
    inState->n              =  n;
    inState->hashBuffer     =  inHashBuffer;
    inState->hashLength     =  hashLength;
    inState->lastInputByte  =  0;
    }



void maxigin_flexHashAdd( MaxiginFlexHashState  *inState,
                          int                    inNumBytes, 
                          const unsigned char   *inBytes ) {
    
    int             j           =  inState->j;
    unsigned char   n           =  inState->n;
    int             b;
    int             numBytes    =  inNumBytes;
    int             hashLength  =  inState->hashLength;
    unsigned char  *hashBuffer  =  inState->hashBuffer;
    int             bLimit;
    int             jLimit;

    
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
               n = mx_flexHashTable[ hashBuffer[j] ^ inBytes[b] ^ n ];
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
            
            n = mx_flexHashTable[ hashBuffer[j] ^ inBytes[b] ^ n ];
            hashBuffer[j] = n;
            j++;
            b++;
            
            n = mx_flexHashTable[ hashBuffer[j] ^ inBytes[b] ^ n ];
            hashBuffer[j] = n;
            j++;
            b++;
            
            n = mx_flexHashTable[ hashBuffer[j] ^ inBytes[b] ^ n ];
            hashBuffer[j] = n;
            j++;
            b++;
            
            n = mx_flexHashTable[ hashBuffer[j] ^ inBytes[b] ^ n ];
            hashBuffer[j] = n;
            j++;
            b++;
            } 
        

        /* back to regular loop for final j values */
        
        n = mx_flexHashTable[ hashBuffer[j] ^ inBytes[b] ^ n ];
        hashBuffer[j] = n;
        
        j++;
        
        if( j >= hashLength ) {
            j = 0;
            }
        b++;
        }


    /* and our most basic loop for final b values, toward end of buffer */
    while( b < numBytes ) {
        n = mx_flexHashTable[ hashBuffer[j] ^ inBytes[b] ^ n ];
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



void maxigin_flexHashFinish( MaxiginFlexHashState  *inState ) {
    
    int             j;
    int             run;
    unsigned char   n           =  inState->n;
    int hashLength              =  inState->hashLength;
    unsigned char  *hashBuffer  =  inState->hashBuffer;
    unsigned char   lastByte    =  inState->lastInputByte;

    
    /*
      mix last input byte in 4 more times

      We do this with lastByte to improve behavior dramatically in
      few-byte-input cases (if we only have 1 input byte, for example,
      we want it to touch every byte in our hash directly)
     */
    
    for( run = 0;
         run < 4;
         run ++ ) {
        
        for( j = 0;
             j < hashLength;
             j ++ ) {
            
            n = mx_flexHashTable[ hashBuffer[j] ^ lastByte ^ n ];
            hashBuffer[j] = n;
            }
        }
    }



void maxigin_flexHash( int                   inNumBytes,
                       const unsigned char  *inBytes,
                       int                   inHashLength, 
                       unsigned char        *inHashBuffer ) {
    
    MaxiginFlexHashState  s;
    
    maxigin_flexHashInit( &s,
                          inHashLength,
                          inHashBuffer );

    maxigin_flexHashAdd( &s,
                         inNumBytes,
                         inBytes );

    maxigin_flexHashFinish( &s );
    }





/* for 4-bit nibbles */
static char mx_nibbleToHex( unsigned char  inNibble ) {
    if( inNibble < 10 ) {
        return (char)( '0' + inNibble );
        }
    else {
        return (char)( 'A' + ( inNibble - 10 ) );
        }
    }



void maxigin_hexEncode( int                   inNumBytes,
                        const unsigned char  *inBytes, 
                        char                 *inHexBuffer ) {
    
    int  stringPos  =  0;
    int  i;
    
    for( i = 0;
         i < inNumBytes;
         i ++ ) {
        
        unsigned char  b      =  inBytes[i];
        unsigned char  upper  =  ( b >> 4 );
        unsigned char  lower  =  b & 0x0F;

        inHexBuffer[ stringPos ] = mx_nibbleToHex( upper );

        stringPos++;
        
        inHexBuffer[ stringPos ] = mx_nibbleToHex( lower );
        
        stringPos++;
        }
    
    inHexBuffer[stringPos] = '\0';
    }




int maxigin_readIntSetting( const char  *inSettingName,
                            int          inDefaultValue ) {
    
    int   readHandle;
    int   v;
    char  success;
    
    readHandle = mingin_startReadPersistData( inSettingName,
                                              &v );

    if( readHandle == -1 ) {
        return inDefaultValue;
        }

    success = mx_readIntFromPersistData( readHandle,
                                         &v );

    mingin_endReadPersistData( readHandle );
    
    if( ! success ) {
        return inDefaultValue;
        }

    return v;
    }


  
void maxigin_writeIntSetting( const char  *inSettingName,
                              int          inValue ) {
    
    int   writeHandle;

    writeHandle = mingin_startWritePersistData( inSettingName );

    if( writeHandle == -1 ) {
        /* failed */
        return;
        }

    /* on failure, writing a setting is a no-op */
    
    mx_writeIntToPerisistentData( writeHandle,
                                  inValue );

    mingin_endWritePersistData( writeHandle );
    }



const char *maxigin_stringConcat( const char  *inStringA,
                                  const char  *inStringB ) {
    
    enum{  NUM_BUFFERS  =  10,
           BUFFER_LEN   =  64 };

    static  char  buffers[ NUM_BUFFERS ][ BUFFER_LEN ];
    static  int   nextBuffer                             =  0;

    int    i           =  0;
    int    a           =  0;
    int    b           =  0;
    char  *returnVal;
    
    
    while( i < BUFFER_LEN - 1
           &&
           inStringA[ a ] != '\0' ) {

        buffers[ nextBuffer ][i] = inStringA[ a ];

        a++;
        i++;
        }

    while( i < BUFFER_LEN - 1
           &&
           inStringB[ b ] != '\0' ) {

        buffers[ nextBuffer ][i] = inStringB[ b ];

        b++;
        i++;
        }

    /* terminate */

    buffers[ nextBuffer ][i] = '\0';

    returnVal = buffers[ nextBuffer ];

    nextBuffer++;

    if( nextBuffer >= NUM_BUFFERS ) {
        nextBuffer = 0;
        }

    return returnVal;
    }





/* end #ifdef MAXIGIN_IMPLEMENTATION */
#endif





/* end of #ifndef MAXIGIN_H_INCLUDED */
#endif
