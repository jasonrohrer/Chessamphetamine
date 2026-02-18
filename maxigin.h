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
#ifndef  MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES
#define  MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES  4096
#endif



/*
  How many unique sprites are supported?

  To make room for 256 sprites, do this:

      #define  MAXIGIN_MAX_NUM_SPRITES  256

  [jumpSettings]
*/
#ifndef  MAXIGIN_MAX_NUM_SPRITES
#define  MAXIGIN_MAX_NUM_SPRITES  1024
#endif



/*
  How many unique sprite strips are supported?

  Note that for each loaded sprite strip, the strip is split
  up internally, and each sprite from the strip consumes one of the sprite
  slots counting toward MAXIGIN_MAX_NUM_SPRITES

  To make room for 256 sprite strips, do this:

      #define  MAXIGIN_MAX_NUM_SPRITE_STRIPS  256

  [jumpSettings]
*/
#ifndef  MAXIGIN_MAX_NUM_SPRITE_STRIPS
#define  MAXIGIN_MAX_NUM_SPRITE_STRIPS  64
#endif



/*
  Sprites are loaded into a statically allocated memory buffer.

  The default size has room for 10 128x128 RGBA sprites.

  To allocate room for 100 16x16 RGBA sprites, do this:

      #define  MAXIGIN_MAX_TOTAL_SPRITE_BYTES  102400

  [jumpSettings]
*/
#ifndef  MAXIGIN_MAX_TOTAL_SPRITE_BYTES
#define  MAXIGIN_MAX_TOTAL_SPRITE_BYTES  655360
#endif



/*
  How many unique sound effects are supported?

  To make room for 64 sprites, do this:

      #define  MAXIGIN_MAX_NUM_SOUND_EFFECTS  64

  [jumpSettings]
*/
#ifndef  MAXIGIN_MAX_NUM_SOUND_EFFECTS
#define  MAXIGIN_MAX_NUM_SOUND_EFFECTS  16
#endif



/*
  Sound effects are loaded into a statically allocated memory buffer.

  The default size has room for ten 5-second sound effects at 44100 Hz
  with 16 bit stereo samples.

  To allocate room for twenty 5-second sound effects at 44100 Hz
  with 16 bit stereo samples, do this:

      #define  MAXIGIN_MAX_TOTAL_SOUND_BYTES  17640000

  [jumpSettings]
*/
#ifndef  MAXIGIN_MAX_TOTAL_SOUND_BYTES
#define  MAXIGIN_MAX_TOTAL_SOUND_BYTES  8820000
#endif



/*
  Immediate mode GUI elements add themselves as draw components (lines, sprites,
  rects, etc.) into a MaxiginGUI structure that has a static amount space in
  it for draw components.

  Note different MaxiginGUI structures can be used to split up the GUI,
  so this setting defines the maximum number of draw components in a single
  MaxiginGUI instance.

  The default size has room for 64 draw components per MaxiginGUI instance.

  To make room for 1024 draw components per MaxiginGUI instance, do this:

      #define  MAXIGIN_MAX_TOTAL_GUI_DRAW_COMPONENTS  1024

  [jumpSettings]
*/
#ifndef  MAXIGIN_MAX_TOTAL_GUI_DRAW_COMPONENTS
#define  MAXIGIN_MAX_TOTAL_GUI_DRAW_COMPONENTS  64
#endif



/*
  How many unique fonts are supported?

  To make room for 10 fonts, do this:

      #define  MAXIGIN_MAX_NUM_FONTS  10

  [jumpSettings]
*/
#ifndef  MAXIGIN_MAX_NUM_FONTS
#define  MAXIGIN_MAX_NUM_FONTS  4
#endif



/*
  How many total characters in all fonts, combined, are supported?

  To make room for 10000 characters across all fonts, do this:

      #define MAXIGIN_MAX_TOTAL_FONT_CHARACTERS  10000
      
  [jumpSettings]
*/    
#ifndef  MAXIGIN_MAX_TOTAL_FONT_CHARACTERS
#define  MAXIGIN_MAX_TOTAL_FONT_CHARACTERS  5000
#endif



/*
  Maximum sprite tile height (including transparent areas) in pixels
  for any font.

  This value is used to allocate the kerning tables for fonts.

  To set maximum font height to 64, do this:

      #define MAXIGIN_MAX_FONT_SPRITE_HEIGHT  64
      
  [jumpSettings]
*/    
#ifndef  MAXIGIN_MAX_FONT_SPRITE_HEIGHT
#define  MAXIGIN_MAX_FONT_SPRITE_HEIGHT  32
#endif



/*
  How many language translation key strings are supported?

  To make room for 20 translation keys, do this:

      #define  MAXIGIN_MAX_NUM_TRANSLATION_KEYS  20

  [jumpSettings]
*/
#ifndef  MAXIGIN_MAX_NUM_TRANSLATION_KEYS
#define  MAXIGIN_MAX_NUM_TRANSLATION_KEYS  128
#endif



/*
  How many total translation string bytes are supported?

  To make room for 2048 translation string bytes, do this:

      #define  MAXIGIN_MAX_TOTAL_TRANSLATION_STRING_BYTES  2048

  [jumpSettings]
*/
#ifndef  MAXIGIN_MAX_TOTAL_TRANSLATION_STRING_BYTES
#define  MAXIGIN_MAX_TOTAL_TRANSLATION_STRING_BYTES  4096
#endif



/*
  How many languages are supported?

  To make room for 20 languages, do this:

      #define  MAXIGIN_MAX_NUM_LANGUAGES  20

  [jumpSettings]
*/
#ifndef  MAXIGIN_MAX_NUM_LANGUAGES
#define  MAXIGIN_MAX_NUM_LANGUAGES  16
#endif



/*
  How many language fonts are supported?

  Note that these are still included in MAXIGIN_MAX_NUM_FONTS above.

  To make room for 20 language fonts, do this:

      #define  MAXIGIN_MAX_NUM_LANGUAGE_FONTS  20 

  [jumpSettings]
*/
#ifndef  MAXIGIN_MAX_NUM_LANGUAGE_FONTS
#define  MAXIGIN_MAX_NUM_LANGUAGE_FONTS  2
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
  This is the ONLY maxiginGame_ function where "maxigin_init" functions can
  be called.
  ****

  ****
  Do not call maxigin-provided "maxigin_draw" functions from this function.
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

  The rate at which maxiginGame_step is called can be found by calling

      mingin_getStepsPerSecond()

  ****
  Do not call maxigin-provided "maxigin_init" functions from this function.
  ****
  
  ****
  Do not call maxigin-provided "maxigin_draw" functions from this function.
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
  This is the ONLY maxiginGame_ function where "maxigin_draw" functions can
  be called.
  ****
  
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
  
  Maxigin provides three sets of functions.

  
  The first set of init functions can ONLY be called from inside maxiginGame_init

  Each init function is tagged with   [jumpMaxiginInit]

  
  The second set of draw functions can ONLY be called from inside
  maxiginGame_getNativePixels

  Each init function is tagged with   [jumpMaxiginDraw]

  
  The third set of general functions can be called from either
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
  Loads a TGA-formatted sprite from the platform's bulk data store.

  Sprites must be in RGBA 32-bit uncompressed TGA format.

  Parameters:

      inBulkResourceName   the name of the bulk data resource to load the sprite
                           from

  Returns:

      sprite handle   on load success

      -1              on failure;

  [jumpMaxiginInit]      
*/
int maxigin_initSprite( const char  *inBulkResourceName );



/*
  Generates a blurred additive glow sprite for a given sprite.

  Calls to maxigin_drawSprite for the sprite handle will draw the underlying
  sprite and additively blend in the glow sprite.
  
  Parameters:

      inSpriteHandle     the sprite to add a glow to
      
      inBlurRadius       the blur radius for the glow, in pixels

      inBlurIterations   the number of iterations of the blur to apply

  [jumpMaxiginInit]  
*/
void maxigin_initMakeGlowSprite( int  inSpriteHandle,
                                 int  inBlurRadius,
                                 int  inBlurIterations );



/*
  Loads a TGA-formatted sprite strip from the platform's bulk data store.

  Sprites must be in RGBA 32-bit uncompressed TGA format, in a
  vertically oriented strip.

  Parameters:

      inBulkResourceName   the name of the bulk data resource to load the sprite
                           strip from

      inHeightPerSprite    the height in pixels of each sprite in the strip.
                           The strip total height must be an integer multiple
                           of this number.
  Returns:

      sprite strip handle   on load success

      -1                    on failure;

  [jumpMaxiginInit]      
*/
int maxigin_initSpriteStrip( const char  *inBulkResourceName,
                             int          inHeightPerSprite );



/*
  Same as maxigin_initMakeGlowSprite, but for all sprites in a sprite strip.
  
  Parameters:

      inSpriteStripHandle   the sprite strip to add a glow to
      
      inBlurRadius          the blur radius for the glow, in pixels

      inBlurIterations      the number of iterations of the blur to apply

  [jumpMaxiginInit]  
*/
void maxigin_initMakeGlowSpriteStrip( int  inSpriteStripHandle,
                                      int  inBlurRadius,
                                      int  inBlurIterations );


/*
  Initializes a UTF-8 font.

  The idea here is that translators will supply their font in a sprite strip
  along with a text file containing a list of single-code-point UTF-8 characters
  corresponding to each sprite in the strip.

  Parameters:
  
      inSpriteStripHandle       the sprite strip containing all the font sprites

      inMapBulkResourceName   the bulk resource name of text data containing
                              an ordered sequence of UTF-8 single code points
                              corresponding to each sprite in the sprite
                              strip.
                              These UTF-8 code points are separated by ASCII
                              space characters ( 0x20 in hex ) in the text
                              data.

      inCharSpacing           the space to insert between characters in the font
                              when drawing, in pixels.
                              If 0, characters will be drawn butted against
                              each other.

      inSpaceWidth            the width of any space characters (0x20) that occur
                              in strings rendered with this font.  Also applies
                              to any sprites in the strip that have no
                              non-transparent pixels.

      inFixedWidth            0       for variable-width characters
                              n > 0   to force all characters to be n pixels
                                      wide (with inSpacing added in between)
  Returns:

      font handle   on success

      -1            on failure

  [jumpMaxiginInit]      
*/
int maxigin_initFont( int          inSpriteStripHandle,
                      const char  *inMapBulkResourceName,
                      int          inCharSpacing,
                      int          inSpaceWidth,
                      int          inFixedWidth );


/*
  Registers a game-defined integer translation key that maps to a
  key string in language translation files.  After registration, the
  phrase key can be used in calls to maxigin_drawLangText().

  The idea here is that the game can define intger phrase constants as-needed,
  like lang_settings, lang_newGame, lang_quit, and so on, and then map them to
  the key strings that actually appear in the language files.

  For example, the game might do something like this:

      enum LanguageKeys {
          lang_settings,
          lang_newGame,
          lang_quit };

  And then make these calls:

      maxigin_initTranslationKey( lang_settings,
                                  "settings" );

      maxigin_initTranslationKey( lang_newGame,
                                  "newGame" );

  Parameters:

    inPhraseKey         game-defined integer phrase key.  Must be >= 0
                        and  < MAXIGIN_MAX_NUM_TRANSLATION_KEYS

    inPhraseKeyString   a \0-terminated phrase key string that occurs in the
                        language bulk data resources.  At most 32
                        bytes long including \0 termination.

  Returns:

    1   on success

    0   on failure

  [jumpMaxiginInit]      
*/
char maxigin_initTranslationKey( int          inPhraseKey,
                                 const char  *inPhraseKeyString );



/*
  Registers hint sprites to be used by Maxigin when drawing user
  input control hints.

  The following MinginButtons should have sprites assigned:

  --All  MGN_BUTTON      buttons, for various controller and mouse button sprites

  --The  MGN_ANY_KEY     button symbol, for a blank key cap sprite which
                         Maxigin will auto-fill in with various key
                         symbols using a font.

  For non-single-symbol keys, like Space and Enter and Escape, Maxigin
  will spell these out as words with a font if no sprite is provided.
  
  Additional sprites can be provided for other specific keys as-needed,
  to override this spelling-out-with-words fallback behavior.

  
  Parameters:

      inSpriteStripHandle   a handle to a strip of button/key sprites

      inMapping             an array of MinginButton values, ending with
                            MGN_MAP_END, corresponding to each sprite
                            in the provided strip.

  [jumpMaxiginInit]
*/
void maxigin_initKeyAndButtonHintSprites( int           inSpriteStripHandle,
                                          MinginButton  inMapping[] );



/*
  Sets TGA-formatted sprites to customize maxigin_guiSlider appearance.

  All parameters are names of TGA resource name in platform's bulk data
  store.

  Parameters:
  
      inLeftEndEmptySpriteResource        left end of slider bar when empty.
                                          center of sprite will be drawn
                                          at left extent of slider.
      
      inLeftEndFullSpriteResource         left end of slider bar when full.
                                          center of sprite will be drawn
                                          at left extent of slider
      
      inRightEndEmptySpriteResource       right end of slider bar when empty.
                                          center of sprite will be drawn
                                          at right extent of slider
      
      inRightEndFullSpriteResource        right end of slider bar when full.
                                          center of sprite will be drawn
                                          at right extent of slider
      
      inMiddleBarEmptySpriteResource      section of slider
                                          bar middle when empty
                                          
      inMiddleBarFullSpriteResource       section of slider
                                          bar middle when full
                                          
      inMiddleSliverEmptySpriteResource   single pixel wide sliver of slider
                                          bar middle when empty.
                                          used to pad out slider bar when
                                          slider width is not an even multiple
                                          of inMiddleBar sprites.
                                          
      inMiddleSliverFullSpriteResource    single pixel wide sliver of slider
                                          bar middle when full.
                                          used to pad out slider bar when
                                          slider width is not an even multiple
                                          of inMiddleBar sprites.
                                          
      inThumbSpriteResource               slider thumb (part that moves)
      
      inThumbActiveSpriteResource         slider thumb when clicked/active

  [jumpMaxiginInit]      
*/ 
void maxigin_initSliderSprites( const char  *inLeftEndEmptySpriteResource,
                                const char  *inLeftEndFullSpriteResource,
                                const char  *inRightEndEmptySpriteResource,
                                const char  *inRightEndFullSpriteResource,
                                const char  *inMiddleBarEmptyrSpriteResource,
                                const char  *inMiddleBarFullSpriteResource,
                                const char  *inMiddleSliverEmptyrSpriteResource,
                                const char  *inMiddleSliverFullSpriteResource,
                                const char  *inThumbSpriteResource,
                                const char  *inThumbHotSpriteResource,
                                const char  *inThumbActiveSpriteResource );



/*
  Sets TGA-formatted sprites to customize maxigin_guiStartPanel appearance.

  All parameters are names of TGA resource name in platform's bulk data
  store.

  Parameters:
  
      inTopLeftSpriteResource       top left corner.

      inTopRightSpriteResource      top right corner.

      inBottomLeftSpriteResource    bottom left corner.

      inBottomRightSpriteResource   bottom right corner.

      inLeftEdgeSpriteResource      left edge

      inRightEdgeSpriteResource     right edge

      inTopEdgeSpriteResource       top edge

      inBottomEdgeSpriteResource    bottom edge

      inFillSpriteResource          center fill

  [jumpMaxiginInit]      
*/ 
void maxigin_initPanelSprites( const char  *inTopLeftSpriteResource,
                               const char  *inTopRightSpriteResource,
                               const char  *inBottomLeftSpriteResource,
                               const char  *inBottomRightSpriteResource,
                               const char  *inLeftEdgeSpriteResource,
                               const char  *inRightEdgeSpriteResource,
                               const char  *inTopEdgeSpriteResource,
                               const char  *inBottomEdgeSpriteResource,
                               const char  *inFillSpriteResource );



/*
  Initializes the music loop that will play as background music in the game.

  Parameters:

      inBulkResourceName   the name of the bulk data resource containing
                           the WAV data of the music loop to play.
                           Must be a 2-channel file in S16LE PCM format.
                              
  [jumpMaxiginInit]      
*/
void maxigin_initMusicLoop( const char  *inBulkResourceName );



/*
  Loads a WAV-formatted sound effect from the platform's bulk data store.

  Sprites must be in RGBA 32-bit uncompressed TGA format.

  Parameters:

      inBulkResourceName   the name of the bulk data resource containing
                           the WAV data of the music loop to play.
                           Must be a 2-channel file in S16LE PCM format.

  Returns:

      sound effect handle   on load success

      -1                    on failure;

  [jumpMaxiginInit]      
*/
int maxigin_initSoundEffect( const char  *inBulkResourceName );





/*
  This structure stores results of immediate mode GUI calls that
  happen during the game's step function so they can be drawn later during the
  game's draw function.

  The user can allocate this structure as-needed for each gui instance,
  like this:

      MaxiginGUI myGUI;

  Then in init function:

      maxigin_initGUI( &myGUI );

  Then in step function:

      maxigin_startGUI( &myGUI );

      maxigin_guiSlider( &myGUI, ....  );

      ...  multiple maxigin_gui calls ...

      maxigin_endGUI( &myGUI );

  Then in draw function:

      maxigin_drawGUI( &myGUI );

  [jumpMaxiginInit]
*/
typedef struct MaxiginGUI MaxiginGUI;



/*
  Initializes a MaxiginGUI structure.
  
  Parameters:
  
      inGUI   pointer to the structure representing the GUI instance

  [jumpMaxiginInit]      
*/
void maxigin_initGUI( MaxiginGUI *inGUI );



/*
  Toggles additive blending for future maxigin_draw calls.

  Parameters:

      inAdditiveOn   1 to turn additive blending on
                     0 to turn additive blending off
                     
  [jumpMaxiginDraw]
*/
void maxigin_drawToggleAdditive( char  inAdditiveOn );



/*
  Gets current additive blend toggle.

  Returns:

      1   if in additive blend mode
      0   if not
      
  [jumpMaxiginDraw]
*/
char maxigin_drawGetAdditive( void );



/*
  Draws a sprite into the game's native pixel buffer.

  Parameters:

      inSpriteHandle   the sprite to draw

      inCenterX        the x position in the game's native pixel buffer of the
                       sprite's center
 
      inCenterY        the y position in the game's native pixel buffer of the
                       sprite's center
  [jumpMaxiginDraw]
*/
void maxigin_drawSprite( int  inSpriteHandle,
                         int  inCenterX,
                         int  inCenterY );



typedef enum{ MAXIGIN_LEFT    =  -1,
              MAXIGIN_CENTER  =   0,
              MAXIGIN_RIGHT   =   1 
    } MaxiginAlign;



/*
  Draws text using a font to the game's native pixel buffer.

  Parameters:

      inFontHandle   the font to use

      inText         \0-terminated string of UTF-8 single-code-point characters

      inLocationX    the x position in the game's native pixel buffer of the
                     drawn text
 
      inLocationY    the y position in the game's native pixel buffer of the
                     drawn text
                     
      inAlign        the horizontal alignment of the text around the point
                     (inLocationX, inLocationY).
                     The text is always vertically centered on this point

  [jumpMaxiginDraw]
*/
void maxigin_drawText( int           inFontHandle,
                       const char   *inText,
                       int           inLocationX,
                       int           inLocationY,
                       MaxiginAlign  inAlign );



/*
  Draws language text using the active language and corresponding font
  to the game's native pixel buffer.

  Parameters:

      inPhraseKey    a phrase key previously registered with
                     maxigin_initTranslationKey()

      inLocationX    the x position in the game's native pixel buffer of the
                     drawn text
 
      inLocationY    the y position in the game's native pixel buffer of the
                     drawn text
                     
      inAlign        the horizontal alignment of the text around the point
                     (inLocationX, inLocationY).
                     The text is always vertically centered on this point

  [jumpMaxiginDraw]
*/
void maxigin_drawLangText( int           inPhraseKey,
                           int           inLocationX,
                           int           inLocationY,
                           MaxiginAlign  inAlign );



/*
  Sets the active draw color for future calls to drawing functions.
  
  Defaults to opaque white (255, 255, 255, 255).

  The color components affect geometric shapes and changes the colors of
  sprites multiplicatively.

  The alpha value changes the transparency of both geometric shapes and sprites.

  Parameters:

      inRed     red   value in [0..255]
      
      inGreen   green value in [0..255]
      
      inBlue    blue  value in [0..255]
      
      inAlpha   alpha value in [0..255]
      
  [jumpMaxiginDraw]
*/
void maxigin_drawSetColor( unsigned char  inRed,
                           unsigned char  inGreen,
                           unsigned char  inBlue,
                           unsigned char  inAlpha );


/*
  Sets the active alpha value for future calls to drawing functions,
  without adjusting the R,G,B draw color.
  
  Defaults to opaque (255).

  The alpha value affects both geometric shapes and sprites.

  Parameters:
      
      inAlpha   alpha value in [0..255]
      
  [jumpMaxiginDraw]
*/
void maxigin_drawSetAlpha( unsigned char  inAlpha );



/*
  Resets the draw color back to the default, pure white, fully opaque alpha.
      
  [jumpMaxiginDraw]
*/
void maxigin_drawResetColor( void );



/*
  Draws a line into the game's native pixel buffer.

  Parameters:
  
      inStartX        the x position in the game's native pixel buffer of the
                      start of the line
                      
      inStartY        the Y position in the game's native pixel buffer of the
                      start of the line
 
      inEndX          the x position in the game's native pixel buffer of the
                      end of the line
                      
      inEndY          the Y position in the game's native pixel buffer of the
                      end of the line
       
  [jumpMaxiginDraw]
*/                      
void maxigin_drawLine( int  inStartX,
                       int  inStartY,
                       int  inEndX,
                       int  inEndY );



/*
  Draws an empty rectangle of lines into the game's native pixel buffer.

  Parameters:
  
      inStartX        the x position in the game's native pixel buffer of the
                      start corner of the rectangle
                      
      inStartY        the Y position in the game's native pixel buffer of the
                      start corner of the rectangle
 
      inEndX          the x position in the game's native pixel buffer of the
                      end corner of the rectangle
                      
      inEndY          the Y position in the game's native pixel buffer of the
                      end corner of the rectangle
       
  [jumpMaxiginDraw]
*/                      
void maxigin_drawRect( int  inStartX,
                       int  inStartY,
                       int  inEndX,
                       int  inEndY );



/*
  Draws a filled rectangle into the game's native pixel buffer.

  Parameters:
  
      inStartX        the x position in the game's native pixel buffer of the
                      start corner of the rectangle
                      
      inStartY        the Y position in the game's native pixel buffer of the
                      start corner of the rectangle
 
      inEndX          the x position in the game's native pixel buffer of the
                      end corner of the rectangle
                      
      inEndY          the Y position in the game's native pixel buffer of the
                      end corner of the rectangle
       
  [jumpMaxiginDraw]
*/                      
void maxigin_drawFillRect( int  inStartX,
                           int  inStartY,
                           int  inEndX,
                           int  inEndY );



/*
  Draws a GUI instance into the game's native pixel buffer.

  Parameters:
  
      inGUI   pointer to the structure representing the GUI instance to draw
       
  [jumpMaxiginDraw]
*/ 
void maxigin_drawGUI( MaxiginGUI *inGUI );



/*
  Draws the hint sprite associated with a given registered button mapping.

  Parameters:

      inButtonHandle   the game-defined button action to draw a sprite for
      
      inCenterX        the x position in the game's native pixel buffer of the
                       sprite's center
 
      inCenterY        the y position in the game's native pixel buffer of the
                       sprite's center
  [jumpMaxiginDraw]
*/
void maxigin_drawButtonHintSprite( int  inButtonHandle,
                                   int  inCenterX,
                                   int  inCenterY );


/*
  Gets the number of sprites in a sprite strip.

  Parameters:
    
      inSpriteStripHandle   handle to a loaded sprite strip
      
  Returns:

      number of sprites in the strip
     
  [jumpMaxiginGeneral]
*/
int maxigin_getNumSpritesInStrip( int  inSpriteStripHandle );



/*
  Gets the number of sprites in a sprite strip.

  Parameters:
    
      inSpriteStripHandle   handle to a loaded sprite strip

      inSpriteIndex         index in the strip of the sprite to get
      
  Returns:

      sprite handle   on success

      -1              on error
     
  [jumpMaxiginGeneral]
*/
int maxigin_getSpriteFromStrip( int  inSpriteStripHandle,
                                int  inSpriteIndex );



/*
  Plays a sound effect now.

  Parameters:
    
      inSoundEffectHandle   handle to a loaded sound effect

  [jumpMaxiginGeneral]
*/
void maxigin_playSoundEffect( int inSoundEffectHandle );



/*
  Initializes a GUI instance to prepare for immediate mode GUI calls.
  Generally called in a game's step function before laying out the GUI.

  Parameters:
    
      inGUI   pointer to the structure representing the GUI instance
              to initialize

  [jumpMaxiginGeneral]
*/
void maxigin_startGUI( MaxiginGUI *inGUI );



/*
  Finalizes a GUI instance after last immediate mode GUI call.
  Generally called in a game's step function after laying out the GUI.

  Parameters:
    
      inGUI   pointer to the structure representing the GUI instance
              to finalize

  [jumpMaxiginGeneral]
*/
void maxigin_endGUI( MaxiginGUI *inGUI );



/*
  Adds a slider to a GUI instance in immediate mode.

  X,Y parameters are relative to the containing GUI (or super-panel) center.

  Parameters:
    
      inGUI            pointer to the structure representing the GUI instance

      inCurrentValue   pointer to current value of the slider, which
                       may be changed as the end user manipulates the slider

      inMinValue       value returned by slider when thumb is far left

      inMaxValue       value returned by slider when thumb is far right

      inStartX         pixel x position of left edge of slider
      
      inEndX           pixel x position of right edge of slider

      inY              pixel y position of vertical center of slider

      inBarHeight      height of slider bar in pixels.
                       has no effect if bar sprite is defined.
                    
      inThumbHeight    height of slider thumb (the moving part) in pixels.
                       has no effect if thumb sprite is defined.

      inThumbWidth     width of slider thumb (the moving part) in pixels.
                       has no effect if thumb sprite is defined.
                        
      inForceMoving    1 to force the slider to listen to arrow keys, controller
                         etc. and force it to be in the moving state
                       0 to not force it to move
                   
  Returns:

      1   if slider is currently being dragged by user

      0   if not
                     
  [jumpMaxiginGeneral]
*/
char maxigin_guiSlider( MaxiginGUI  *inGUI,
                        int         *inCurrentValue,
                        int          inMinValue,
                        int          inMaxValue,
                        int          inStartX,
                        int          inEndX,
                        int          inY,
                        int          inBarHeight,
                        int          inThumbHeight,
                        int          inThumbWidth,
                        char         inForceMoving );



/*
  Starts a sub-panel in an immediate-mode GUI.

  All GUI items created by calls up to maxigin_guiEndPanel will relative
  to the center of this panel.

  X,Y parameters for this panel are relative to the GUI (or super-panel) center.

  Parameters:
    
      inGUI       pointer to the structure representing the GUI instance
      
      inCenterX   center x location of panel
      
      inCenterY   center y location of panel

      inWidth     panel width

      inHeight    panel height

  Return:

      panel handle   that must be passed to maxigin_guiEndPanel

      -1             on failure

  [jumpMaxiginGeneral]
*/
int maxigin_guiStartPanel( MaxiginGUI  *inGUI,
                           int          inCenterX,
                           int          inCenterY,
                           int          inWidth,
                           int          inHeight );



/*
  Ends a sub-panel in an immediate-mode GUI.

  GUI items drawn after this call will again be relative to the containing
  super panel (or window)

  Parameters:
    
      inGUI           pointer to the structure representing the GUI instance

      inPanelHandle   the panel handle returned by maxigin_guiStartPanel

  [jumpMaxiginGeneral]
*/
void maxigin_guiEndPanel( MaxiginGUI  *inGUI,
                          int          inPanelHandle );








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
  Same as mingin_getPlatformPrimaryButton, but accounts for Maxigin's
  own internal button mapping.

  Games that want to take full advantage of Maxigin's functionality should
  call this instead of mingin_getPlatformPrimaryButton.
  
  Parameters:

      inButtonHandle   the game-defined button action to check

  Returns:

      the primary assigned button   if one is available

      MGN_BUTTON_NONE               if none of the assigned buttons are
                                    available on this platform.
                        
  [jumpMaxiginGeneral]
*/
MinginButton maxigin_getPlatformPrimaryButton( int  inButtonHandle );



/*
  Gets the current on-screen pointer location, if any.

  Parameters:

      outX   pointer to where the x location should be returned
             returned value is in range 0 .. MAXIGIN_GAME_NATIVE_W

      outY   pointer to where the y location should be returned
             returned value is in range 0 .. MAXIGIN_GAME_NATIVE_H

  Return:

      1   if pointer location is on-screen and available

      0   if pointer location is off-screen or not available
                         
  [jumpMaxiginGeneral] 
 */
char maxigin_getPointerLocation( int  *outX,
                                 int  *outY );



/*
  Maxigin listens to certain joystick axes for its own functionality that's not
  game-specific (like moving through the settings screen, adjusting sliders,
  etc.)

  If a game registers sticks directly with mingin_registerStickAxis,
  those registrations may clobber Maxigin's registrations.

  Games that want to take full advantage of Maxigin's functionality should
  call this instead of mingin_registerStickAxis.

  See mingin_registerStickAxis in mingin.h for full documentation. 
  
  Parameters:

      inStickAxisHandle   the game-defined stick axis to map

      inMapping           an array of MinginStick values, ending with
                          MGN_MAP_END, that should map to the game-defined
                          stick axis.
                          
  Returns:

      1   on success
      
      0   on failure (if inStickAxisHandle is out of the supported range)
                       
  [jumpMaxiginGeneral]
*/
char maxigin_registerStickAxis( int                inStickAxisHandle,
                                const MinginStick  inMapping[] );



/*
  Checks the position of a previously-mapped joystick axis handle.

  See mingin_getStickPosition in mingin.h for full documentation.

  Games that want to take full advantage of Maxigin's functionality should
  call this instead of mingin_getStickPosition.
  
  Parameters:

      inStickAxisHandle   the game-defined stick axis to check

      outPosition         pointer to where the stick position should be returned

      outLowerLimit       pointer to where the lower limit for that stick
                          should be returned

      outUpperLimit       pointer to where the upper limit for that stick
                          should be returned                    
  Returns:

      1   if stick is available

      0   if not
                          
  [jumpMaxiginGeneral]
*/
char maxigin_getStickPosition( int   inStickAxisHandle,
                               int  *outPosition,
                               int  *outLowerLimit,
                               int  *outUpperLimit );





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

  Returns a static buffer from a rotating pool of 10 static buffers,
  which allows for nested conversions.

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

  Max resulting string is 128 characters long, including the \0 termination.

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
  These versions of stringConcat take varying numbers of strings.

  They still use the same pool of 10 static buffers described above.
  
  [jumpMaxiginGeneral] 
*/
const char *maxigin_stringConcat3( const char  *inStringA,
                                   const char  *inStringB,
                                   const char  *inStringC );

const char *maxigin_stringConcat4( const char  *inStringA,
                                   const char  *inStringB,
                                   const char  *inStringC,
                                   const char  *inStringD );

const char *maxigin_stringConcat5( const char  *inStringA,
                                   const char  *inStringB,
                                   const char  *inStringC,
                                   const char  *inStringD,
                                   const char  *inStringE );

const char *maxigin_stringConcat6( const char  *inStringA,
                                   const char  *inStringB,
                                   const char  *inStringC,
                                   const char  *inStringD,
                                   const char  *inStringE,
                                   const char  *inStringF );

/*
  Copies a string, including the terminating \0, into a destination
  byte buffer.

  The destination buffer must have room.
  
  Parameters:

      inString   the \0-terminated string to copy

      inDest     the destination byte buffer
  
  [jumpMaxiginGeneral]    
*/
void maxigin_stringCopy( const char  *inString,
                         char        *inDest );




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
  Logs two string-embedded ints to the game engine log with a newline.

  Parameters:

      inStringA   the \0-terminated first string

      inValB      the first int value

      inStringC   the \0-terminated middle string

      inValD      the second int value

      inStringE   the \0-terminated end string

  [jumpMaxiginGeneral]
*/
void maxigin_logInt2( const char  *inStringA,
                      int          inValB,
                      const char  *inStringC,
                      int          inValD,
                      const char  *inStringE );



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



/*
  This structure definition is down here, since the end user never
  needs to manipulate it.

  However, it's still outside the MAXIGIN_IMPLEMENTATION ifdef, so that
  the end user can allocate this structure for their gui instances.
*/
struct MaxiginGUI {


        /* the absolute position of GUI 0,0 in the game native screen
           starts in the center of the window for a new GUI,
           but changes as sub-panels are opened and closed */
        int  zeroOffsetX;
        int  zeroOffsetY;
        
        /* Track ID of hot/active component across steps.
           Hot means mouse is over.
           Active means mouse was clicked on component in past and is
           still being actively dragged. */
        void  *hot;
        void  *active;

        /* true if any active gui component has detected
           that the mouse is held down (even if it's not over any gui
           component  */
        char   mouseDown;
        

        /* These offsets are for when mouse is first clicked on a "handle"
           on a given component, and the relative offset of the mouse
           to that handle should be maintained as the mouse moves.
           For example, a scroll bar thumb shouldn't jump to center on the
           mouse when it is clicked slightly off-center.*/
        int    activeMouseOffsetX;
        int    activeMouseOffsetY;
        
        int    numDrawComponents;  

        struct {
                char           additiveBlend;
                unsigned char  red;
                unsigned char  green;
                unsigned char  blue;
                unsigned char  alpha;

                enum {
                    MX_GUI_DRAW_LINE,
                    MX_GUI_DRAW_RECT,
                    MX_GUI_FILL_RECT,
                    MX_GUI_DRAW_SPRITE,
                    MX_GUI_DRAW_SPRITE_SEQUENCE
                    } drawType;

                union {
                        struct {
                                int  startX;
                                int  startY;
                                int  endX;
                                int  endY;
                            } line;
                        
                        struct {
                                int  startX;
                                int  startY;
                                int  endX;
                                int  endY;
                            } rect;
                        
                        struct {
                                int  spriteHandle;
                                int  centerX;
                                int  centerY;
                            } sprite;

                        /* a repeated sequence of the same sprite,
                           with the center given for the first sprite,
                           and count sprites drawn total with an offset
                           between each sprite drawn */
                        struct {
                                int  spriteHandle;
                                int  startCenterX;
                                int  startCenterY;
                                int  offsetX;
                                int  offsetY;
                                int  count;
                            } spriteSequence;
                        
                    } drawParams;
                
            } drawComponents[ MAXIGIN_MAX_TOTAL_GUI_DRAW_COMPONENTS ];
    };




#ifdef MAXIGIN_IMPLEMENTATION



/* temporarily dumps pixels to out.raw */
static void mx_dumpRGBAPixels( unsigned char  *inStartByte,
                               int             inW,
                               int             inH ) {

    int  numBytes     =  inW * inH * 4;
    int  outHandle;
    char success;
    

    outHandle = mingin_startWritePersistData( "out.raw" );

    if( outHandle == -1 ) {
        mingin_log( "Failed to open 'out.raw' persistent data for writing "
                    "when trying to dump pixels.\n" );
        return;
        }

    success = mingin_writePersistData( outHandle,
                                       numBytes,
                                       inStartByte );

    if( ! success ) {
        mingin_log( "Failed to dump raw pixels to persistent data\n" );
        }

    mingin_endWritePersistData( outHandle );

    maxigin_logInt2( "Dumped RGBA pixels to out.raw with w = ",
                     inW,
                     ", h = ",
                     inH,
                     "" );
    }

    



/* all of the button actions that Maxigin registers internally */
typedef enum MaxiginUserAction {
    QUIT,
    FULLSCREEN_TOGGLE,
    LANG_SWITCH,
    SOUND_TOGGLE,
    SOUND_LOCK,
    PLAYBACK_START_STOP,
    PLAYBACK_FASTER,
    PLAYBACK_SLOWER,
    PLAYBACK_PAUSE,
    PLAYBACK_NORMAL,
    PLAYBACK_REVERSE,
    PLAYBACK_JUMP_HALF_BACK,
    PLAYBACK_JUMP_HALF_AHEAD,
    MAXIGIN_MOUSE_BUTTON,
    MAXIGIN_SLIDER_INCREASE,
    MAXIGIN_SLIDER_DECREASE,
    LAST_MAXIGIN_USER_ACTION
    } MaxiginUserAction;


/* all of the sticks that Maxigin registers internally */
typedef enum MaxiginSticks {
    MAXIGIN_STICK_SLIDER,
    LAST_MAXIGIN_STICK
    } MaxiginSticks;


#define  MAXIGIN_MAX_NUM_PLAYING_SOUND_EFFECTS           40

static  char        mx_areWeInMaxiginGameInitFunction  =  0;

static  char        mx_areWeInMaxiginGameStepFunction  =  0;

static  char        mx_areWeInMaxiginGameDrawFunction  =  0;


static  char        mx_initDone                        =  0;

static  char        mx_recordingRunning                =  0;
static  char        mx_playbackRunning                 =  0;
static  char        mx_playbackPaused                  =  0;
static  int         mx_playbackSpeed                   =  1;
                                                          /* -1 for backward */
static  char        mx_playbackDirection               =  1;
static  char        mx_playbackJumping                 =  0;

static  char        mx_buttonsDown[ LAST_MAXIGIN_USER_ACTION ];

static  MaxiginGUI  mx_internalGUI;

static  const char  *mx_playbackDataStoreName           =
                                                        "maxigin_playback.bin";
static  int          mx_playbackDataStoreHandle         =  -1;

static  int          mx_playbackDataLength;

static  int          mx_playbackFullSnapshotLastPlayed  =  0;
static  int          mx_playbackIndexStartPos           =  0;
static  int          mx_playbackNumFullSnapshots        =  0;
static  int          mx_diffsBetweenSnapshots           =  300;
static  int          mx_playbackTotalSteps              =  0;
static  int          mx_playbackCurrentStep             =  0;

static  const char  *mx_recordingDataStoreName          =
                                                   "maxigin_recording.bin";
static  const char  *mx_recordingIndexDataStoreName     =
                                                   "maxigin_recordingIndex.bin";

/* is inAction freshly pressed since last call to isActionFreshPressed */
static char mx_isActionFreshPressed( MaxiginUserAction  inAction );



void minginGame_getMinimumViableScreenSize( int  *outWide,
                                            int  *outHigh ) {
    *outWide = MAXIGIN_GAME_NATIVE_W;
    *outHigh = MAXIGIN_GAME_NATIVE_H;
    }



static void mx_computeScaling( int   inTargetWide,
                               int   inTargetHigh,
                               int  *outScaleFactor,
                               int  *outOffsetX,
                               int  *outOffsetY ) {
    
    int  scaleFactor;
    int  scaleW;
    int  scaleH;
    int  scaledGameW;
    int  scaledGameH;
            
    int  offsetX;
    int  offsetY;
    
    scaleW = inTargetWide / MAXIGIN_GAME_NATIVE_W;

    scaleH = inTargetHigh / MAXIGIN_GAME_NATIVE_H;

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

    if( scaledGameW < inTargetWide ) {
        offsetX = ( inTargetWide - scaledGameW ) / 2;
        }
    if( scaledGameH < inTargetHigh ) {
        offsetY = ( inTargetHigh - scaledGameH ) / 2;
        }

    *outScaleFactor = scaleFactor;
    *outOffsetX     = offsetX;
    *outOffsetY     = offsetY;
    }



/* RGB pixels of game's native image size */
static  unsigned char  mx_gameImageBuffer[ MAXIGIN_GAME_NATIVE_W *
                                           MAXIGIN_GAME_NATIVE_H * 3 ];


void minginGame_getScreenPixels( int             inWide,
                                 int             inHigh,
                                 unsigned char  *inRGBBuffer ) {
    
    int  numPixels      =  inWide * inHigh;
    int  numPixelBytes  =  numPixels * 3;
    int  p;

    int  x;
    int  y;
            
    int  scaleFactor;
            
    int  scaledGameW;
    int  scaledGameH;
            
    int  offsetX;
    int  offsetY;


    mx_areWeInMaxiginGameDrawFunction = 1;
    
    maxiginGame_getNativePixels( mx_gameImageBuffer );

    maxigin_drawGUI( &mx_internalGUI );
    
    mx_areWeInMaxiginGameDrawFunction = 0;

    
    mx_computeScaling( inWide,
                       inHigh,
                       & scaleFactor,
                       & offsetX,
                       & offsetY );
    
    scaledGameW = scaleFactor * MAXIGIN_GAME_NATIVE_W;
    scaledGameH = scaleFactor * MAXIGIN_GAME_NATIVE_H;

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

    
    /* nearest neighbor scaling
       reducing the math operations in the inner loop resulted in a
       35% - 40% speed up, depending on the scaling factor */
    
    for( y = offsetY;
         y < offsetY + scaledGameH;
         y ++ ) {
        
        int rowStartDest = y * inWide * 3;
        int ySrcScaled = y - offsetY;
        int ySrcOrig = ySrcScaled /  scaleFactor;
        
        int rowStartSrcOrig = ySrcOrig * MAXIGIN_GAME_NATIVE_W * 3;

        int pixDest = rowStartDest + offsetX * 3;

        int pixSrcOrig = rowStartSrcOrig;

        unsigned char r = mx_gameImageBuffer[ pixSrcOrig++ ];
        unsigned char g = mx_gameImageBuffer[ pixSrcOrig++ ];
        unsigned char b = mx_gameImageBuffer[ pixSrcOrig++ ];
        
        int xDestFillCount = 0;
        
        int xLimit = offsetX + scaledGameW;
        
        for( x = offsetX;
             x != xLimit;
             x ++ ) {

            /* we need to check this before incrementing
               xDestFillCount, so that we don't walk off the end
               of our source pixel row on our last dest pixel row */
            if( xDestFillCount == scaleFactor ) {
                /* we've used our source pixel scaleFactor times
                   go on to next source pixel */
                xDestFillCount = 0;
                
                r = mx_gameImageBuffer[ pixSrcOrig++ ];
                g = mx_gameImageBuffer[ pixSrcOrig++ ];
                b = mx_gameImageBuffer[ pixSrcOrig++ ];
                }
            
            inRGBBuffer[ pixDest++ ]  =  r;
            inRGBBuffer[ pixDest++ ]  =  g;
            inRGBBuffer[ pixDest++ ]  =  b;

            xDestFillCount ++;
            }
        }
    }



#define  MAXIGIN_SPRITE_MAX_BULK_NAME_LENGTH  64

#define  MAXIGIN_SPRITE_HASH_LENGTH            4


typedef struct MaxiginSprite {
        
        int            w;
        int            h;

        /* how far visible pixels (alpha > 0) extend beyond left and
           right of sprite center */
        int            leftVisibleRadius;
        int            rightVisibleRadius;

        
        /*
          Index for this sprite into font kerning table.

          Only used for sprites that are font glyphs.
          
          -1 if it hasn't been computed.
        */
        int            kerningTableIndex;

        
        /* index into mx_spriteBytes */
        int            startByte;

        char           bulkResourceName[ MAXIGIN_SPRITE_MAX_BULK_NAME_LENGTH ];

        char           pendingChange;

        int            retryCount;

        int            stepsUntilNextRetry;

        /* -1 if this is not a glow sprite */
        int            glowSpriteHandle;
        int            glowRadius;
        int            glowIterations;

        /* handle of full sprite strip that this is a sub-sprite of, or -1
           if not part of a strip */
        int            stripParentHandle;
        int            stripIndex;

        /* if this sprite is the parent of a strip of sprites, this
           is the handle to the sprite strip, or -1 if not */
        int            stripChildHandle;
        
        unsigned char  hash[ MAXIGIN_SPRITE_HASH_LENGTH ];
        
    } MaxiginSprite;



static  unsigned char  mx_spriteBytes  [ MAXIGIN_MAX_TOTAL_SPRITE_BYTES ];
static  MaxiginSprite  mx_sprites      [ MAXIGIN_MAX_NUM_SPRITES        ];

static  int            mx_numSpriteBytesUsed  =  0;
static  int            mx_numSprites          =  0;



static void mx_dumpSpriteRGBA( int  inSpriteHandle ) {

    MaxiginSprite  *s  =  &( mx_sprites[ inSpriteHandle ] );

    mx_dumpRGBAPixels( &( mx_spriteBytes[ s->startByte ] ),
                       s->w,
                       s->h );
    }



        
#define  MAXIGIN_TGA_BUFFER_SIZE  256

static  unsigned char  mx_tgaReadBuffer[ MAXIGIN_TGA_BUFFER_SIZE ];



#define  MAXIGIN_PADDED_INT_LENGTH  12


/*
  Reads int and jumps ahead MAXIGIN_PADDED_INT_LENGTH total bytes, to skip
  all padding.

  Returns 1 on success, 0 on failure.
*/
static char mx_readPaddedIntFromPeristentData( int   inStoreReadHandle,
                                               int  *outInt );



/*
  Writes a \0-terminated string representation of an int to data store,
  and pads it with \0 characters afterwards to fill paddedIntLength total.

  Returns 1 on success, 0 on failure.
*/
static char mx_writePaddedIntToPerisistentData( int  inStoreWriteHandle,
                                                int  inInt );



/*
  encapsulates both bulkReadHandle and persistentDataReadHandle
  this allows us to cache generated sprites in our persistent data
  and load them using the same code as fixed sprites loaded from bulk data
*/
typedef struct MinginOpenData {

        int   readHandle;
        /* 0 if persistent data, 1 if bulk data */
        char  isBulk;
        
    } MinginOpenData;



static int mx_readData( MinginOpenData  *inDataHandle,
                        int              inNumBytesToRead,
                        unsigned char   *inByteBuffer ) {

    if( inDataHandle->isBulk ) {
        return mingin_readBulkData( inDataHandle->readHandle,
                                    inNumBytesToRead,
                                    inByteBuffer );
        }
    else {
        return mingin_readPersistData( inDataHandle->readHandle,
                                       inNumBytesToRead,
                                       inByteBuffer );
        }
    }



static int mx_getDataPosition( MinginOpenData  *inDataHandle ) {
    if( inDataHandle->isBulk ) {
        return mingin_getBulkDataPosition( inDataHandle->readHandle );
        }
    else {
        return mingin_getPersistDataPosition( inDataHandle->readHandle );
        }
    }

        

/* removes sprite data from sprite data store and collapses
   subsequent sprite data back to fill the gap
*/
static void mx_removeSpriteData( int  inSpriteHandle ) {

    int  b;
    int  s;
    int  oldSpriteBytes  =  mx_sprites[ inSpriteHandle ].w *
                            mx_sprites[ inSpriteHandle ].h * 4;


    if( mx_sprites[ inSpriteHandle ].startByte == -1 ) {
        /* data already removed */
        return;
        }
        
    /* collapse memory location of existing sprite,
       moving all subsequent sprite data back */

    for( b = mx_sprites[ inSpriteHandle ].startByte;
         b < mx_numSpriteBytesUsed - oldSpriteBytes;
         b ++ ) {

        mx_spriteBytes[ b ] = mx_spriteBytes[ b + oldSpriteBytes ];
        }

    /* update all sprites that pointed into the now-moved sprite data */
    for( s = 0;
         s < mx_numSprites;
         s ++ ) {
            
        if( mx_sprites[ s ].startByte
            > 
            mx_sprites[ inSpriteHandle ].startByte ) {

            mx_sprites[ s ].startByte -= oldSpriteBytes;
            }
        }

    mx_numSpriteBytesUsed -= oldSpriteBytes;

    /* stick -1 in for byte index of this sprite record, since
       its data is gone */
    mx_sprites[ inSpriteHandle ].startByte = -1;
    }




static void mx_recomputeSpriteAttributes( int  inSpriteHandle ) {

    MaxiginSprite  *s            =  &( mx_sprites[ inSpriteHandle ] );
    
    int             leftRadius      =  0;
    int             rightRadius     =  0;
    int             w               =  s->w;
    int             h               =  s->h;
    int             xCenter         =  w / 2;
    int             y;
    int             startByte       =  s->startByte;
    int             numSpriteBytes  =  w * h * 4;
    
    /* find left/right visible extents */
    
    for( y = 0;
         y < h;
         y ++ ) {

        int  curByte  =  startByte + y * w * 4;
        int  x;
        
        for( x = 0;
             x < w;
             x ++ ) {

            unsigned char  a  =  mx_spriteBytes[ curByte + 3 ];

            if( a > 0 ) {

                if( x - xCenter > rightRadius ) {
                    rightRadius = x - xCenter;
                    }
                else if( xCenter - x > leftRadius ) {
                    leftRadius = xCenter - x;
                    }
                }
            
            /* on to next pixel */
            curByte += 4;
            }
        }

    s->leftVisibleRadius  = leftRadius;

    /* center position is actually off by 1 in favor of left radius */
    s->rightVisibleRadius = rightRadius + 1;
    
    


    /* hash bytes after origin flip and BGRA conversion */

    maxigin_flexHash( numSpriteBytes,
                      &( mx_spriteBytes[ startByte ] ),
                      MAXIGIN_SPRITE_HASH_LENGTH,
                      s->hash );
    }

    


static int mx_reloadSpriteFromOpenData( const char      *inBulkResourceName,
                                        int              inReloadHandle,
                                        MinginOpenData  *inReadHandle,
                                        int              inNumBytesLeft ) {
    int   numBytes;
    int   numRead;

    int   idFieldSize;
    char  originAtTop;
    int   w;
    int   h;
    
    int   neededSpriteBytes;
    int   newSpriteHandle;
    int   neededFileSpriteBytes;
    
    int   b;
    int   startByte;

    char  makingNewSprite     =  0;

    
    newSpriteHandle = inReloadHandle;

    if( newSpriteHandle == -1 ) {
        makingNewSprite = 1;
        newSpriteHandle = mx_numSprites;
        }
    
    
    if( makingNewSprite
        &&
        mx_numSprites >= MAXIGIN_MAX_NUM_SPRITES ) {

        if( inReloadHandle == -1 ) {
            /* suppress error messages when reloading */
            
            maxigin_logInt( "Alreaded loaded maximum number of sprites: ",
                            mx_numSprites );
            maxigin_logString( "Failed to load sprite: ",
                               inBulkResourceName );
            }
        return -1;
        }

    
    numBytes       = inNumBytesLeft;
    

    if( numBytes < 18 ) {
        if( inReloadHandle == -1 ) {
            maxigin_logString( "Sprite file too small to contain TGA header: ",
                               inBulkResourceName );
            }
        
        return -1;
        }

    numRead = mx_readData( inReadHandle,
                           18,
                           mx_tgaReadBuffer );

    if( numRead != 18 ) {
        if( inReloadHandle == -1 ) {
            maxigin_logString( "Failed to read TGA header: ",
                               inBulkResourceName );
            }
        
        return -1;
        }

    if( mx_tgaReadBuffer[2]  != 2       /* image type code */
        ||
        mx_tgaReadBuffer[1]  != 0       /* color map type */
        ||
        ( mx_tgaReadBuffer[16] != 24
          &&
          mx_tgaReadBuffer[16] != 32 ) ) {  /* bits per pixel */

        if( inReloadHandle == -1 ) {
            maxigin_logString(
                "Only uncompressed unmapped 32-bit RGBA and 24-bit RGB "
                "TGA files can be loaded: ",
                inBulkResourceName );
            }
        
        return -1;
        }

    idFieldSize = mx_tgaReadBuffer[ 0 ];
    
    w = mx_tgaReadBuffer[ 13 ] << 8
        |
        mx_tgaReadBuffer[ 12 ];

    h = mx_tgaReadBuffer[ 15 ] << 8
        |
        mx_tgaReadBuffer[ 14 ];
    

    /* image descriptor byte, with bit 5 indicating image vertical flip */
    originAtTop = mx_tgaReadBuffer[ 17 ]
                  &
                  ( 1 << 5 );

    
    /* now read the id field and ignore it */
    if( idFieldSize > 0 ) {
        numRead = mx_readData( inReadHandle,
                               idFieldSize,
                               mx_tgaReadBuffer );

        if( numRead != idFieldSize ) {
            if( inReloadHandle == -1 ) {
                maxigin_logString( "Failed to read id field from TGA data: ",
                                   inBulkResourceName );
                }
        
            return -1;
            }
        }

    /* now we're done reading the header
       image data next */
    
    /* TGA pixels are in BGRA order */

    neededSpriteBytes = w * h * 4;

    if( mx_tgaReadBuffer[16] == 32 ) {
        neededFileSpriteBytes = w * h * 4;
        }
    else {
        neededFileSpriteBytes = w * h * 3;
        }
        
    if( numBytes - mx_getDataPosition( inReadHandle )
        < neededFileSpriteBytes ) {

        if( inReloadHandle == -1 ) {
            maxigin_logString( "Full TGA pixel data truncated: ",
                               inBulkResourceName );
            }
        
        return -1;
        }

    
    if( ! makingNewSprite
        &&
        mx_sprites[ newSpriteHandle ].startByte == -1 ) {

        /* existing sprite data has been deleted before reload was called */

        makingNewSprite = 1;
        }

    
    if( ! makingNewSprite
        &&
        ( mx_sprites[ newSpriteHandle ].w != w
          ||
          mx_sprites[ newSpriteHandle ].h != h ) ) {

        /* mismatch with existing record, sprite changing size*/

        int  oldNeededSpriteBytes  =  mx_sprites[ newSpriteHandle ].w *
                                      mx_sprites[ newSpriteHandle ].h * 4;

        if( neededSpriteBytes > oldNeededSpriteBytes ) {
            /* reloaded sprite getting bigger */
            if( neededSpriteBytes + mx_numSpriteBytesUsed - oldNeededSpriteBytes
                >
                MAXIGIN_MAX_TOTAL_SPRITE_BYTES ) {

                maxigin_logString(
                    "Not enough space in static memory to reload sprite that "
                    "is increasing in size: ",
                    inBulkResourceName );
        
                return -1;
                }
            }


        mx_removeSpriteData( newSpriteHandle );
        
        makingNewSprite = 1;
        }

    

    if( makingNewSprite
        &&
        neededSpriteBytes + mx_numSpriteBytesUsed
        >
        MAXIGIN_MAX_TOTAL_SPRITE_BYTES ) {

        maxigin_logString( "Not enough space in static memory to load sprite: ",
                           inBulkResourceName );

        maxigin_logInt2( "",
                         MAXIGIN_MAX_TOTAL_SPRITE_BYTES,
                         " bytes total, ",
                         mx_numSpriteBytesUsed,
                         " bytes used" );
        
        return -1;
        }

    if( makingNewSprite ) {
        
        /* read into end of sprite bytes */
        
        numRead = mx_readData(
                      inReadHandle,
                      neededFileSpriteBytes,
                      &( mx_spriteBytes[ mx_numSpriteBytesUsed ] ) );
        }
    else {
        
        /* read into region pointed to by existing sprite record */
        
        startByte = mx_sprites[ newSpriteHandle ].startByte;
        
        numRead = mx_readData(
                      inReadHandle,
                      neededFileSpriteBytes,
                      &( mx_spriteBytes[ startByte ] ) );
        }

    
    if( numRead != neededFileSpriteBytes ) {
        maxigin_logString( "Failed to read full TGA pixel data: ",
                           inBulkResourceName );
        return -1;
        }
    
    
    mx_sprites[ newSpriteHandle ].w          =  w;
    mx_sprites[ newSpriteHandle ].h          =  h;

    if( makingNewSprite ) {
        mx_sprites[ newSpriteHandle ].startByte  =  mx_numSpriteBytesUsed;
        mx_numSpriteBytesUsed += neededSpriteBytes;
        }

    /* copy bulk resource name into struct */
    b = 0;
    while( inBulkResourceName[ b ] != '\0'
           &&
           b < MAXIGIN_SPRITE_MAX_BULK_NAME_LENGTH - 1 ) {
        
        mx_sprites[ newSpriteHandle ].bulkResourceName[ b ] =
                                    inBulkResourceName[ b ];
        b++;
        }
    mx_sprites[ newSpriteHandle ].bulkResourceName[ b ] = '\0';

    /* successfully loaded this sprite
       change to data no longer pending */
    mx_sprites[ newSpriteHandle ].pendingChange       = 0;
    mx_sprites[ newSpriteHandle ].retryCount          = 0;
    mx_sprites[ newSpriteHandle ].stepsUntilNextRetry = 0;

    mx_sprites[ newSpriteHandle ].stripParentHandle = -1;
    mx_sprites[ newSpriteHandle ].stripIndex = -1;
    
    if( newSpriteHandle == mx_numSprites ) {

        /* leave stripChildHandle alone if we're reloading an existing sprite */
        
        mx_sprites[ newSpriteHandle ].stripChildHandle  = -1;

        /* leave old glow handle in place if this isn't a new sprite record */
        
        mx_sprites[ newSpriteHandle ].glowSpriteHandle  = -1;

        mx_sprites[ newSpriteHandle ].kerningTableIndex = -1;
        
            
        mx_numSprites ++;
        }

    
    startByte = mx_sprites[ newSpriteHandle ].startByte;
    

    if( neededSpriteBytes == w * h * 4
        &&
        neededFileSpriteBytes == w * h * 3 ) {
        
        /* we want BRGA sprite data, but we read BRG data from the file */
        
        /* do BRG -> BRGA conversion, with all A values set to 255 */

        /* walk backward through file data, which is filled into our
           first 3/4 of the sprite buffer, while we
           also walk backward through our full sprite buffer,
           copying pixels last-first
        */
        int fb  =  startByte + neededFileSpriteBytes - 1;
        int sb  =  startByte + neededSpriteBytes     - 1;

        while( fb > startByte
               &&
               sb > startByte ) {

            /* each iteration, we move fb back by only 3 bytes
               we move sb back by 4 bytes */

            /* fixed, fully-opaque alpha */
            mx_spriteBytes[ sb -- ] =  255;

            /* copy g b r */
            mx_spriteBytes[ sb -- ] = mx_spriteBytes[ fb -- ];
            mx_spriteBytes[ sb -- ] = mx_spriteBytes[ fb -- ];
            mx_spriteBytes[ sb -- ] = mx_spriteBytes[ fb -- ];
            }
        }
    


    /* now do BGRA to RGBA conversion */

    
    b = startByte;

    while( b < startByte + neededSpriteBytes ) {
        
        unsigned char  blue  =  mx_spriteBytes[ b ];
        
        /* swap red and blue bytes */
        mx_spriteBytes[ b ] = mx_spriteBytes[ b + 2 ];

        mx_spriteBytes[ b + 2 ] = blue;

        b += 4;
        }
    
    if( ! originAtTop ) {

        int  stopRow  =  h / 2;
        int  r;

        for( r = 0;
             r < stopRow;
             r ++ ) {

            int  otherRow        =  ( h - 1 ) - r;
            int  rStartByte      =  r        * w * 4;
            int  otherStartByte  =  otherRow * w * 4;
            int  rowBytes        =  w * 4;

            for( b = 0;
                 b < rowBytes;
                 b ++ ) {

                unsigned char  temp  =  mx_spriteBytes[ rStartByte + b ];

                mx_spriteBytes    [ rStartByte     + b ] =
                    mx_spriteBytes[ otherStartByte + b ];
                
                mx_spriteBytes[ otherStartByte + b ] = temp;
                }
            }
        }

    
    mx_recomputeSpriteAttributes( newSpriteHandle );
     
    
    return newSpriteHandle;
    }



/* inReloadHandle is -1 if loading a new sprite */
static int mx_reloadSprite( const char  *inBulkResourceName,
                            int          inReloadHandle ) {
    
    int             numBytes;
    int             spriteHandle;
    MinginOpenData  openData;
    
    openData.readHandle = mingin_startReadBulkData( inBulkResourceName,
                                                    & numBytes );

    if( openData.readHandle == -1 ) {
        if( inReloadHandle == -1 ) {
            maxigin_logString( "Failed to open sprite: ",
                               inBulkResourceName );
            }
        return -1;
        }

    openData.isBulk = 1;

    spriteHandle = mx_reloadSpriteFromOpenData( inBulkResourceName,
                                                inReloadHandle,
                                                & openData,
                                                numBytes );

    mingin_endReadBulkData( openData.readHandle );
    
    return spriteHandle;
    }



int maxigin_initSprite( const char  *inBulkResourceName ) {

    if( ! mx_areWeInMaxiginGameInitFunction ) {
        mingin_log( "Game tried to call maxigin_initSprite "
                    "from outside of maxiginGame_init\n" );
        return -1;
        }

    return mx_reloadSprite( inBulkResourceName,
                            -1 );
    }



/* writes sprite pixel data to open handle as a TGA file */
static void mx_writeSpriteToOpenData( int  inSpriteHandle,
                                      int  inPersistentDataWriteHandle ) {

    int            w;
    int            h;
    int            p;
    int            b;
    int            numPixels;
    char           success;
    unsigned char  tgaHeader[18];
    

    w = mx_sprites[ inSpriteHandle ].w;
    h = mx_sprites[ inSpriteHandle ].h;

    numPixels = w * h * 4;

    /* empty ID field */
    tgaHeader[ 0 ] = 0;
    /* no color map */
    tgaHeader[ 1 ] = 0;
    /* image type, unmapped RGBA */
    tgaHeader[ 2 ] = 2;
    /* empty color map spec */
    tgaHeader[ 3 ] = 0;
    tgaHeader[ 4 ] = 0;
    tgaHeader[ 5 ] = 0;
    tgaHeader[ 6 ] = 0;
    tgaHeader[ 7 ] = 0;
    /* x origin 2-byte int */
    tgaHeader[ 8 ] = 0;
    tgaHeader[ 9 ] = 0;
    /* y origin 2-byte int */
    tgaHeader[ 10 ] = 0;
    tgaHeader[ 11 ] = 0;
    /* width, lo-hi byte */
    tgaHeader[ 12 ] = (unsigned char)( w & 0xFF );
    tgaHeader[ 13 ] = (unsigned char)( w >> 8 );
    /* height, lo-hi byte */
    tgaHeader[ 14 ] = (unsigned char)( h & 0xFF );
    tgaHeader[ 15 ] = (unsigned char)( h >> 8 );
    /* bits per pixel, RGBA is 32 */
    tgaHeader[ 16 ] = 32;
    /* 8-bit attribute (for the A component)
       and a 1 in the 5th position, since origin at upper left corner */
    tgaHeader[ 17 ] = 8 | 1 << 5;


    success = mingin_writePersistData( inPersistentDataWriteHandle,
                                       18,
                                       tgaHeader );

    if( ! success ) {
        mingin_log( "Failed to write TGA header to persistent data store.\n" );
        return;
        }

    /* perform RGBA to BGRA conversion as we write pixels to file */
    
    b = mx_sprites[ inSpriteHandle ].startByte;
    
    for( p = 0;
         p < numPixels;
         p ++ ) {

        unsigned char  bgra[4];
        
        bgra[2] = mx_spriteBytes[ b ++ ];
        bgra[1] = mx_spriteBytes[ b ++ ];
        bgra[0] = mx_spriteBytes[ b ++ ];
        bgra[3] = mx_spriteBytes[ b ++ ];
        
        success = mingin_writePersistData(
                      inPersistentDataWriteHandle,
                      4,
                      bgra );
        if( ! success ) {
            mingin_log(
                "Failed to write TGA pixel data to persistent data store.\n" );
            return;
            }
        }
    }



static void mx_blurSprite( int  inSpriteHandle,
                           int  inRadius,
                           int  inIterations ) {

    int  w                 =  mx_sprites[ inSpriteHandle ].w;
    int  h                 =  mx_sprites[ inSpriteHandle ].h;
    int  startByte         =  mx_sprites[ inSpriteHandle ].startByte;
    int  tempStartByte;
    int  sourceStartByte;
    int  destStartByte;
    int  i;
    int  b;
    int  y;
    int  x;
    int  neededExtraBytes  =  w * h * 4;
    
    if( mx_numSpriteBytesUsed + neededExtraBytes >
        MAXIGIN_MAX_TOTAL_SPRITE_BYTES ) {

        mingin_log( "Not enough extra sprites bytes in sprite data buffer "
                    "to perform mx_blurSprite.\n" );
        return;
        }
    
    tempStartByte = mx_numSpriteBytesUsed;

    /* zero out temp... black transparent pixels */
    for( b = 0;
         b < neededExtraBytes;
         b ++ ) {
        mx_spriteBytes[ tempStartByte + b ] = 0;
        }
    

    /* start with main sprite as source, blank buffer as dest */
    sourceStartByte = startByte;
    destStartByte   = tempStartByte;
    
    for( i = 0;
         i < inIterations;
         i ++ ) {

        int  tempTempStartByte;
        int  startY              =  inRadius;
        int  endY                =  h - inRadius;
        int  startX              =  inRadius;
        int  endX                =  w - inRadius;

        /* ignore edge cases
           start blur pushed into image far enough where
           our blur radius never goes outside of image */

        
        for( y = startY;
             y < endY;
             y ++ ) {

            long           boxAccume[3]  =  { 0, 0, 0 };
            long           alphaSum      =  0;
            int            py;
            int            px;
            int            pixelsInBox   =  0;
            int            rowStart;
            int            pix;
            int            c;
            unsigned char  alpha;
            
            /* first, fill box accume with entire box around first pixel
               in row */
            for( py = y - inRadius;
                 py <= y + inRadius;
                 py ++ ) {

                int  boxRowStart  =  py * w * 4 + sourceStartByte;
                
                for( px = 0;
                     px < inRadius + inRadius + 1;
                     px ++ ) {

                    pix   = boxRowStart + px * 4;
                    c     = 0;
                    alpha = mx_spriteBytes[ pix + 3 ];

                    /* pre-multiply by alpha to avoid
                       halos of residual color coming from
                       fully transparent areas */
                    
                    boxAccume[ c++ ] += alpha * mx_spriteBytes[ pix++ ];
                    boxAccume[ c++ ] += alpha * mx_spriteBytes[ pix++ ];
                    boxAccume[ c++ ] += alpha * mx_spriteBytes[ pix++ ];
                    alphaSum         += alpha;

                    pixelsInBox ++;
                    }
                }

            /* set blur value for first pixel in row */

            rowStart = y * w * 4 + destStartByte;
            pix      = rowStart + startX * 4;
            c        = 0;

            if( alphaSum == 0 ) {     
                mx_spriteBytes[ pix++ ] = 0;
                mx_spriteBytes[ pix++ ] = 0;
                mx_spriteBytes[ pix++ ] = 0;
                mx_spriteBytes[ pix++ ] = 0;
                }
            else { 
                mx_spriteBytes[ pix++ ] =    
                    (unsigned char)( boxAccume[ c++ ] / alphaSum );
                mx_spriteBytes[ pix++ ] =
                    (unsigned char)( boxAccume[ c++ ] / alphaSum );
                mx_spriteBytes[ pix++ ] =
                    (unsigned char)( boxAccume[ c++ ] / alphaSum );
                mx_spriteBytes[ pix++ ] =
                    (unsigned char)( alphaSum / pixelsInBox );
                }
            
            /* now process remaining pixels in row */
            for( x = startX + 1;
                 x < endX;
                 x ++ ) {

                /* subtract column to left that has fallen out of our
                   moving box, add in column to right that has
                   been added to our moving box */
                for( py = y - inRadius;
                     py <= y + inRadius;
                     py ++ ) {

                    int  boxRowStart  =  py * w * 4 + sourceStartByte;

                    pix   = boxRowStart + ( x - 1 - inRadius ) * 4;
                    c     = 0;
                    alpha = mx_spriteBytes[ pix + 3 ];

                    /* subtract pre-mulitiplied alpha left column */
                    boxAccume[ c++ ] -= alpha * mx_spriteBytes[ pix++ ];
                    boxAccume[ c++ ] -= alpha * mx_spriteBytes[ pix++ ];
                    boxAccume[ c++ ] -= alpha * mx_spriteBytes[ pix++ ];
                    alphaSum         -= alpha;

                    pix   = boxRowStart + ( x + inRadius ) * 4;
                    c     = 0;
                    alpha = mx_spriteBytes[ pix + 3 ];
                    
                    boxAccume[ c++ ] += alpha * mx_spriteBytes[ pix++ ];
                    boxAccume[ c++ ] += alpha * mx_spriteBytes[ pix++ ];
                    boxAccume[ c++ ] += alpha * mx_spriteBytes[ pix++ ];
                    alphaSum         += alpha;
                    }

                /* now our accume contains sum of all pixels
                   in box around our pixel */
                pix = rowStart + x * 4;
                c   = 0;

                if( alphaSum == 0 ) {
                    mx_spriteBytes[ pix++ ] = 0;
                    mx_spriteBytes[ pix++ ] = 0;
                    mx_spriteBytes[ pix++ ] = 0;
                    mx_spriteBytes[ pix++ ] = 0;
                    }
                else {
                    mx_spriteBytes[ pix++ ] =
                        (unsigned char)( boxAccume[ c++ ] / alphaSum );
                    mx_spriteBytes[ pix++ ] =
                        (unsigned char)( boxAccume[ c++ ] / alphaSum );
                    mx_spriteBytes[ pix++ ] =
                        (unsigned char)( boxAccume[ c++ ] / alphaSum );
                    mx_spriteBytes[ pix++ ] =
                        (unsigned char)( alphaSum / pixelsInBox );
                    }
                }
            
            }

        /* for next iteration, swap source and dest
           blur back the other way */
        tempTempStartByte = sourceStartByte;
        sourceStartByte = destStartByte;
        destStartByte = tempTempStartByte;
        }

    /* we're done with iterations
       where did our final image end up? */

    if( sourceStartByte == tempStartByte ) {

        /* ended up in our temp buffer
           copy result back in */
        for( b = 0;
             b < neededExtraBytes;
             b ++ ) {
            
            mx_spriteBytes    [ startByte     + b ] =
                mx_spriteBytes[ tempStartByte + b ];
            }
        }
    /* else ended up in-place in our original sprite byte buffer */
    /* don't need to do any copying */
    }



static void mx_regenerateGlowSprite( int  inMainSpriteHandle,
                                     int  inBlurRadius,
                                     int  inBlurIterations ) {

    MaxiginSprite  *mainSprite;
    const char     *glowSpriteDataName;
    int             persistReadHandle;
    int             numBytes;
    int             numRead;
    int             b;
    int             glowW;
    int             glowH;
    int             glowBorder;
    
    char           readGlowFromFile;
    unsigned char  hashBuffer[ MAXIGIN_SPRITE_HASH_LENGTH ];


    mainSprite = &( mx_sprites[ inMainSpriteHandle ] );


    glowBorder = inBlurRadius * inBlurIterations * 2;
    
    glowW = mainSprite->w
            +
            2 * glowBorder;
        
    glowH = mainSprite->h
            +
            2 * glowBorder;

    if( mainSprite->bulkResourceName[0] == '\0' ) {
        /* empty source resource name, check if this is part of a strip */

        if( mainSprite->stripParentHandle == -1 ) {
            mingin_log( "Failed to make glow sprite for non-strip sprite "
                        "without bulk resource name.\n" );
            return;
            }

        glowSpriteDataName =
            maxigin_stringConcat4(
                mx_sprites[ mainSprite->stripParentHandle ].bulkResourceName,
                "_strip_",
                maxigin_intToString( mainSprite->stripIndex ),
                ".glow" );
        }
    else {
        glowSpriteDataName = maxigin_stringConcat( mainSprite->bulkResourceName,
                                                   ".glow" );
        }

    readGlowFromFile = 0;
    
    persistReadHandle = mingin_startReadPersistData( glowSpriteDataName,
                                                     & numBytes );
    
    if( persistReadHandle != -1 ) {

        if( numBytes > MAXIGIN_SPRITE_HASH_LENGTH ) {

            numRead = mingin_readPersistData( persistReadHandle,
                                              MAXIGIN_SPRITE_HASH_LENGTH,
                                              hashBuffer );
            
            if( numRead == MAXIGIN_SPRITE_HASH_LENGTH ) {

                char  hashMatch        =  1;
                int   readRadius;
                int   readIterations;
                char  success          =  0;
                
                for( b = 0;
                     b < MAXIGIN_SPRITE_HASH_LENGTH;
                     b ++ ) {
                    
                    if( hashBuffer[ b ] !=
                        mainSprite->hash[ b ] ) {
                        
                        hashMatch = 0;
                        break;
                        }
                    }

                if( hashMatch ) {
                    success =
                        mx_readPaddedIntFromPeristentData(
                            persistReadHandle,
                            & readRadius );
                    
                    success = success
                        &&
                        mx_readPaddedIntFromPeristentData(
                            persistReadHandle,
                            & readIterations );

                    if( success
                        &&
                        ( readRadius != inBlurRadius
                          ||
                          readIterations != inBlurIterations ) ) {
                        success = 0;
                        }
                    }
                    
                
                if( success ) {
                    if( mainSprite->glowSpriteHandle ==
                        -1 ) {

                        MinginOpenData  openData;
                        
                        /* load from glow file */
                        openData.readHandle = persistReadHandle;
                        openData.isBulk = 0;

                        /* set a non-file-name (blank string )
                           as inBulkResourceName
                           since this glow sprite isn't actually in
                           our bulk data store */
                        mainSprite->glowSpriteHandle =
                            mx_reloadSpriteFromOpenData(
                                "",
                                -1,
                                & openData,
                                /* TGA data comes right after hash bytes */
                                numBytes - MAXIGIN_SPRITE_HASH_LENGTH );

                        if( mainSprite->glowSpriteHandle !=
                            -1 ) {
                            /* successfully loaded from file */
                            readGlowFromFile = 1;

                            mainSprite->glowRadius = inBlurRadius;
                            mainSprite->glowIterations = inBlurIterations;
                            
                            maxigin_logString( "Successfully read cached glow "
                                               "sprite from perisistent data "
                                               "store for ",
                                               mainSprite->bulkResourceName );
                            }
                        }
                    else {
                        /* glow sprite with hash and parameter
                           match already loaded */

                        readGlowFromFile = 1;
                        }
                    }  
                }
            }
        
        mingin_endReadPersistData( persistReadHandle );
        }

    if( ! readGlowFromFile ) {

        /* generate glow from scratch
           cache it to file for future */

        int  glowSpriteHandle;
        int  neededGlowBytes;
        int  glowCacheDataWriteHandle;
        int  glowStartByte;
        int  mainStartByte;
        int  x;
        int  y;
        int  mainW;
        int  mainH;
        
        glowSpriteHandle = mainSprite->glowSpriteHandle;


        if( glowSpriteHandle != -1 ) {
            mx_removeSpriteData( glowSpriteHandle );
            }
        else {
            /* stick new glow sprite at end of sprite list */
            glowSpriteHandle = mx_numSprites;

            if( mx_numSprites >= MAXIGIN_MAX_NUM_SPRITES ) {

                maxigin_logString(
                    "Already have too many sprites when trying "
                    "to create glow sprite for: ",
                    mainSprite->bulkResourceName );
                return;
                }
            }

        neededGlowBytes = glowW * glowH * 4;

        if( mx_numSpriteBytesUsed + neededGlowBytes
            >
            MAXIGIN_MAX_TOTAL_SPRITE_BYTES ) {

            maxigin_logString(
                "Already have too many sprite data bytes when trying "
                "to create glow sprite for: ",
                mainSprite->bulkResourceName );
            
            return;
            }

        
        mainSprite->glowSpriteHandle = glowSpriteHandle;
        mainSprite->glowRadius       = inBlurRadius;
        mainSprite->glowIterations   = inBlurIterations;

        glowStartByte  =  mx_numSpriteBytesUsed;
        
        mainW          =  mainSprite->w;
        mainH          =  mainSprite->h;
        mainStartByte  =  mainSprite->startByte;
        
        mx_sprites[ glowSpriteHandle ].w                = glowW;
        mx_sprites[ glowSpriteHandle ].h                = glowH;
        mx_sprites[ glowSpriteHandle ].startByte        = glowStartByte;
        mx_sprites[ glowSpriteHandle ].glowSpriteHandle = -1;

        mx_sprites[ glowSpriteHandle ].bulkResourceName[0] = '\0';

        mx_sprites[ glowSpriteHandle ].stripParentHandle = -1;
        mx_sprites[ glowSpriteHandle ].stripIndex = -1;
        mx_sprites[ glowSpriteHandle ].stripChildHandle = -1;
        
        mx_numSprites ++;
        mx_numSpriteBytesUsed += neededGlowBytes;


        /* generate glow sprite pixels */

        for( b = 0;
             b < neededGlowBytes;
             b ++ ) {
            /* zero out glow sprite */
            mx_spriteBytes[ b + glowStartByte ] = 0;
            }

        /* copy sprite into center of glow sprite, leaving border
           of black transparent pixels */

        for( y = 0;
             y < mainH;
             y ++ ) {

            int  mainRowStart  =  y * mainW * 4;
            int  glowY         =  y + glowBorder;
            int  glowRowStart  =  glowY * glowW * 4;
            
            for( x = 0;
                 x < mainW;
                 x ++ ) {
                
                int  pixStart      =  mainRowStart + x * 4 + mainStartByte;
                int  glowX         =  x + glowBorder;
                int  glowPixStart  =  glowRowStart + glowX * 4 + glowStartByte;

                mx_spriteBytes    [ glowPixStart     ++ ] =
                    mx_spriteBytes[ pixStart         ++ ];
                
                mx_spriteBytes    [ glowPixStart     ++ ] =
                    mx_spriteBytes[ pixStart         ++ ];
                
                mx_spriteBytes    [ glowPixStart     ++ ] =
                    mx_spriteBytes[ pixStart         ++ ];
                
                mx_spriteBytes    [ glowPixStart     ++ ] =
                    mx_spriteBytes[ pixStart         ++ ];
                }
            }

        
        mx_blurSprite( glowSpriteHandle,
                       inBlurRadius,
                       inBlurIterations );
        

        glowCacheDataWriteHandle =
            mingin_startWritePersistData( glowSpriteDataName );

        if( glowCacheDataWriteHandle == -1 ) {
            maxigin_logString( "Failed to open persistent "
                               "data cache file for writing: ",
                               glowSpriteDataName );
            return;
            }

        
        mingin_writePersistData( glowCacheDataWriteHandle,
                                 MAXIGIN_SPRITE_HASH_LENGTH,
                                 mainSprite->hash );

        /* next radius and iteration count as padded ints */
        
        mx_writePaddedIntToPerisistentData( glowCacheDataWriteHandle,
                                            inBlurRadius );
        
        mx_writePaddedIntToPerisistentData( glowCacheDataWriteHandle,
                                            inBlurIterations );

        
        mx_writeSpriteToOpenData( glowSpriteHandle,
                                  glowCacheDataWriteHandle );


        mingin_endWritePersistData( glowCacheDataWriteHandle );
        }
    
    }



void maxigin_initMakeGlowSprite( int  inSpriteHandle,
                                 int  inBlurRadius,
                                 int  inBlurIterations ) {

    if( ! mx_areWeInMaxiginGameInitFunction ) {
        mingin_log( "Game tried to call maxigin_initMakeGlowSprite "
                    "from outside of maxiginGame_init\n" );
        return;
        }

    if( inSpriteHandle < 0 ) {
        return;
        }

    mx_regenerateGlowSprite( inSpriteHandle,
                             inBlurRadius,
                             inBlurIterations );
    }



typedef struct MaxiginSliderSprites {

        /* index 0 for empty, 1 for full */
        int  left  [2];
        int  right [2];
        int  bar   [2];
        int  sliver[2];

        /* index 0 for cold, 1 for hot, 2 for active */
        int  thumb [3];
        
    } MaxiginSliderSprites;


char                   mx_sliderSpritesSet  =  0;
MaxiginSliderSprites   mx_sliderSprites;

    


void maxigin_initSliderSprites( const char  *inLeftEndEmptySpriteResource,
                                const char  *inLeftEndFullSpriteResource,
                                const char  *inRightEndEmptySpriteResource,
                                const char  *inRightEndFullSpriteResource,
                                const char  *inMiddleBarEmptySpriteResource,
                                const char  *inMiddleBarFullSpriteResource,
                                const char  *inMiddleSliverEmptySpriteResource,
                                const char  *inMiddleSliverFullSpriteResource,
                                const char  *inThumbSpriteResource,
                                const char  *inThumbHotSpriteResource,
                                const char  *inThumbActiveSpriteResource ) {
    
    mx_sliderSprites.left[ 0 ] =
        maxigin_initSprite( inLeftEndEmptySpriteResource );
    
    mx_sliderSprites.left[ 1 ] =
        maxigin_initSprite( inLeftEndFullSpriteResource );


    mx_sliderSprites.right[ 0 ] =
        maxigin_initSprite( inRightEndEmptySpriteResource );

    mx_sliderSprites.right[ 1 ] =
        maxigin_initSprite( inRightEndFullSpriteResource );

    
    mx_sliderSprites.bar[ 0 ] =
        maxigin_initSprite( inMiddleBarEmptySpriteResource );

    mx_sliderSprites.bar[ 1 ] =
        maxigin_initSprite( inMiddleBarFullSpriteResource );


    mx_sliderSprites.sliver[ 0 ] =
        maxigin_initSprite( inMiddleSliverEmptySpriteResource );

    mx_sliderSprites.sliver[ 1 ] =
        maxigin_initSprite( inMiddleSliverFullSpriteResource );


    mx_sliderSprites.thumb[ 0 ] =
        maxigin_initSprite( inThumbSpriteResource );

    mx_sliderSprites.thumb[ 1 ] =
        maxigin_initSprite( inThumbHotSpriteResource );

    mx_sliderSprites.thumb[ 2 ] =
        maxigin_initSprite( inThumbActiveSpriteResource );

    mx_sliderSpritesSet =
        ( mx_sliderSprites.left[ 0 ]   >= 0 )
        &&
        ( mx_sliderSprites.left[ 1 ]   >= 0 )
        &&
        ( mx_sliderSprites.right[ 0 ]  >= 0 )
        &&
        ( mx_sliderSprites.right[ 1 ]  >= 0 )
        &&
        ( mx_sliderSprites.bar[ 0 ]    >= 0 )
        &&
        ( mx_sliderSprites.bar[ 1 ]    >= 0 )
        &&
        ( mx_sliderSprites.sliver[ 0 ] >= 0 )
        &&
        ( mx_sliderSprites.sliver[ 1 ] >= 0 )
        &&
        ( mx_sliderSprites.thumb[ 0 ]  >= 0 )
        &&
        ( mx_sliderSprites.thumb[ 1 ]  >= 0 )
        &&
        ( mx_sliderSprites.thumb[ 2 ]  >= 0 );
    }



typedef struct MaxiginPanelSprites {

        /* tl, tr, bl, br */
        int  corners[4];
        
        /* l, r, t, b */
        int  sides  [4];
        
        int  fill;
        
    } MaxiginPanelSprites;


char                   mx_panelSpritesSet  =  0;
MaxiginPanelSprites    mx_panelSprites;



void maxigin_initPanelSprites( const char  *inTopLeftSpriteResource,
                               const char  *inTopRightSpriteResource,
                               const char  *inBottomLeftSpriteResource,
                               const char  *inBottomRightSpriteResource,
                               const char  *inLeftEdgeSpriteResource,
                               const char  *inRightEdgeSpriteResource,
                               const char  *inTopEdgeSpriteResource,
                               const char  *inBottomEdgeSpriteResource,
                               const char  *inFillSpriteResource ) {
    mx_panelSprites.corners[0] =
        maxigin_initSprite( inTopLeftSpriteResource );
    
    mx_panelSprites.corners[1] =
        maxigin_initSprite( inTopRightSpriteResource );
    
    mx_panelSprites.corners[2] =
        maxigin_initSprite( inBottomLeftSpriteResource );
    
    mx_panelSprites.corners[2] =
        maxigin_initSprite( inBottomRightSpriteResource );

    mx_panelSprites.sides[0] =
        maxigin_initSprite( inLeftEdgeSpriteResource );
    
    mx_panelSprites.sides[1] =
        maxigin_initSprite( inRightEdgeSpriteResource );
    
    mx_panelSprites.sides[2] =
        maxigin_initSprite( inTopEdgeSpriteResource );
    
    mx_panelSprites.sides[3] =
        maxigin_initSprite( inBottomEdgeSpriteResource );

    mx_panelSprites.fill =
        maxigin_initSprite( inFillSpriteResource );

    mx_panelSpritesSet =
        ( mx_panelSprites.corners[0] >= 0 )
        &&
        ( mx_panelSprites.corners[1] >= 0 )
        &&
        ( mx_panelSprites.corners[2] >= 0 )
        &&
        ( mx_panelSprites.corners[3] >= 0 )
        &&
        ( mx_panelSprites.sides[0]   >= 0 )
        &&
        ( mx_panelSprites.sides[1]   >= 0 )
        &&
        ( mx_panelSprites.sides[2]   >= 0 )
        &&
        ( mx_panelSprites.sides[3]   >= 0 )
        &&
        ( mx_panelSprites.fill       >= 0 );
    }



/* strip handle is -1 to make a new strip
   
   if inStripHandle is an existing strip, inHeightPerSprite should be -1,
   since the existing height per sprite will be used.
   
   returns handle to strip (which may equal inSpriteHandle if re-making
   an existing strip) */
static int mx_regenSpriteStripChildren( int  inMainSpriteHandle,
                                        int  inStripHandle,
                                        int  inHeightPerSprite );



static void mx_regenerateSpriteKerning( int  inSpriteHandle );



static void mx_postReloadStep( int  inSpriteHandle ) {
    
    MaxiginSprite  *s  =  &( mx_sprites[ inSpriteHandle ] );
    
    s->pendingChange       = 0;
    s->retryCount          = 0;
    s->stepsUntilNextRetry = 0;

    if( s->glowSpriteHandle != -1 ) {

        mx_regenerateGlowSprite(
            inSpriteHandle,
            s->glowRadius,
            s->glowIterations );
        }

    if( s->stripChildHandle != -1 ) {
        /* just reloaded a parent sprite of a strip
           need to make new pointers for child sprites */

        /* this will also re-gen glow sprites if needed */
        mx_regenSpriteStripChildren( inSpriteHandle,
                                     s->stripChildHandle,
                                     -1 );
        }

    if( s->kerningTableIndex != -1 ) {
        mx_regenerateSpriteKerning( inSpriteHandle );
        }
    }



static void mx_checkSpritesNeedReload( void ) {

    int  s;

    for( s = 0;
         s < mx_numSprites;
         s ++ ) {

        if( mx_sprites[ s ].pendingChange ) {

            if( mx_sprites[ s ].stepsUntilNextRetry <= 0 ) {

                int  retry   =  mx_sprites[ s ].retryCount;
                int  handle  =  mx_reloadSprite(
                                    mx_sprites[ s ].bulkResourceName,
                                    s );

                if( handle == -1 ) {
                    /* with each failure, we wait more and more
                       steps until we try again */
                    mx_sprites[ s ].pendingChange = 1;
                    mx_sprites[ s ].retryCount ++;
                    
                    mx_sprites[ s ].stepsUntilNextRetry =
                                        mx_sprites[ s ].retryCount;
                    }
                else {
                    /* success in reloading */

                    maxigin_logInt(
                        "Success in reloading sprite after retries: ",
                        retry );

                    mx_postReloadStep( s );
                    }
                }
            else {
                /* wait another step before retrying */
                mx_sprites[ s ].stepsUntilNextRetry --;
                }
            }
        else if(
            mx_sprites[ s ].bulkResourceName[ 0 ] != '\0'
            &&
            mingin_getBulkDataChanged( mx_sprites[ s ].bulkResourceName ) ) {

            /* skip checking any with blank bulkResourceNames,
               since those are our internally-generated sprites */
            
            int  handle  =  mx_reloadSprite( mx_sprites[ s ].bulkResourceName,
                                             s );
            
            /* we see a bulk data change for this sprite
               however, the bulk data might be in the middle of being
               written by an external editor, or may contain invalid
               data for other reasons.
               If that's the case, truncated TGA data will be found
               and skipped, and the old sprite will remain in place.
               But keep trying to reload it */

            if( handle == -1 ) {
                mx_sprites[ s ].pendingChange       = 1;
                mx_sprites[ s ].retryCount          = 1;
                mx_sprites[ s ].stepsUntilNextRetry = 1;
                }
            else {
                mx_postReloadStep( s );
                }
            }
        }
    }



/* each sprite strip points to sprites in the sprite pool
   the handles for these sub-sprites are stored here */
static  int  mx_stripSubSprites[ MAXIGIN_MAX_NUM_SPRITES ];

static  int  mx_numStripSubSprites  =  0;


typedef struct MaxiginSpriteStrip {

        int  numSubSprites;

        int  heightPerSprite;

        /* index of first strip sprite's handle in mx_stripSubSprites */
        int  startIndex;
        
    } MaxiginSpriteStrip;



static  MaxiginSpriteStrip  mx_spriteStrips[ MAXIGIN_MAX_NUM_SPRITE_STRIPS ];

static  int                 mx_numSpriteStrips  =  0;



int maxigin_initSpriteStrip( const char  *inBulkResourceName,
                             int          inHeightPerSprite ) {

    int                  mainSpriteHandle;
    
    if( mx_numSpriteStrips >= MAXIGIN_MAX_NUM_SPRITE_STRIPS ) {
        maxigin_logString( "Failed to load sprite strip because we already "
                           "have too many sprite strips loaded: ",
                           inBulkResourceName );
        return -1;
        }
    
    mainSpriteHandle = maxigin_initSprite( inBulkResourceName );

    if( mainSpriteHandle == -1 ) {
        return -1;
        }

    /* generate a brand new strip */
    return mx_regenSpriteStripChildren( mainSpriteHandle,
                                        -1,
                                        inHeightPerSprite );
    }



static int mx_regenSpriteStripChildren( int  inMainSpriteHandle,
                                        int  inStripHandle,
                                        int  inHeightPerSprite ) {

    MaxiginSprite       *mainSprite;
    int                  numSubSprites;
    int                  newStripHandle;
    MaxiginSpriteStrip  *newStrip;
    int                  i;
    int                  bytesPerSubSprite;
    int                  nextSubStartByte;
    char                 exists;
    
    mainSprite = &( mx_sprites[ inMainSpriteHandle ] );

    if( inHeightPerSprite == -1 ) {
        if( inStripHandle == -1 ) {
            maxigin_logString(
                "Failed to load sprite strip because strip height "
                "per sub-sprite not specified: ",
                mainSprite->bulkResourceName );
            return -1;
            }
        /* if height not given, extract it from existing strip */
        inHeightPerSprite = mx_spriteStrips[ inStripHandle ].heightPerSprite;
        }

    numSubSprites = mainSprite->h / inHeightPerSprite;

    if( inStripHandle != -1 ) {
        if( numSubSprites * inHeightPerSprite
            != mainSprite->h
            ||
            numSubSprites != mx_spriteStrips[ inStripHandle ].numSubSprites ) {

            /* some kind of mismatch */

            /* we actually allow a taller/shorter strip if can still contain
               the same number of sub-sprites that are all of a new,
               taller/shorter same size */

            /* number of sub sprites can't change, though */
            numSubSprites = mx_spriteStrips[ inStripHandle ].numSubSprites;

            /* we'll check if an interger multiple of this fits below */
            inHeightPerSprite = mainSprite->h / numSubSprites;
            }
        }
    
    if( numSubSprites * inHeightPerSprite
        !=
        mainSprite->h ) {

        maxigin_logString( "Failed to (re) load sprite strip because strip "
                           "height is not integer multiple of supplied height: ",
                           mainSprite->bulkResourceName );
        return -1;
        }

    



    if( inStripHandle == -1 ) {

        /* make sure there's room for this many new sub-sprites */

        if( mx_numStripSubSprites + numSubSprites
            >=
            MAXIGIN_MAX_NUM_SPRITES ) {
        
            maxigin_logString( "Failed to load sprite strip because we already "
                               "have too many strip sub sprites: ",
                               mainSprite->bulkResourceName );
            return -1;
            }

        if( mx_numSprites + numSubSprites
            >=
            MAXIGIN_MAX_NUM_SPRITES ) {
        
            maxigin_logString( "Failed to load sprite strip because we already "
                               "have too many sprites: ",
                               mainSprite->bulkResourceName );
            return -1;
            };
        
        newStripHandle = mx_numSpriteStrips;
        
        mx_numSpriteStrips ++;

        exists = 0;
        }
    else {
        /* an existing strip, there's already room for it */
        
        newStripHandle = inStripHandle;

        exists = 1;
        }
    

    newStrip = &( mx_spriteStrips[ newStripHandle ] );

    if( ! exists ) {
        newStrip->numSubSprites = numSubSprites;

        newStrip->startIndex = mx_numStripSubSprites;

        newStrip->heightPerSprite = inHeightPerSprite;
    
        mx_numStripSubSprites += numSubSprites;
        }
    else {
        if( numSubSprites != newStrip->numSubSprites ) {
            maxigin_logString( "Failed to hot re-load sprite strip because "
                               "of size mismatch with old strip: ",
                               mainSprite->bulkResourceName );
            return -1;
            }
        }
    
    nextSubStartByte = mainSprite->startByte;
    
    bytesPerSubSprite = mainSprite->w * inHeightPerSprite * 4;
    
    for( i = 0;
         i < numSubSprites;
         i ++ ) {

        int             subHandle;
        MaxiginSprite  *subSprite;

        if( ! exists ) {
            subHandle = mx_numSprites;
            
            mx_numSprites ++;

            mx_stripSubSprites[ i + newStrip->startIndex ] = subHandle;
            }
        else {
            /* re-use existing sub-sprite, but point it at new data
               from main sprite */
            subHandle = mx_stripSubSprites[ i + newStrip->startIndex ];
            }
        
        subSprite = &( mx_sprites[ subHandle ] );

        subSprite->w = mainSprite->w;

        subSprite->h = inHeightPerSprite;

        subSprite->startByte = nextSubStartByte;

        nextSubStartByte += bytesPerSubSprite;

        subSprite->bulkResourceName[0] = '\0';

        subSprite->pendingChange = 0;

        subSprite->stripParentHandle = inMainSpriteHandle;
        subSprite->stripIndex = i;
        
        
        mx_recomputeSpriteAttributes( subHandle );

        if( ! exists ) {
            subSprite->glowSpriteHandle  = -1;
            
            subSprite->kerningTableIndex = -1;
            }
        }

    /* now that they are all pointing to the right spots in the new
       parent sprite strip, we can regenerate glow sprites if needed.

       If we do this inline in the loop above, the deletion of glow
       sprites can cause our parent sprite strip to move,
       causing later children to point to the wrong place */

    for( i = 0;
         i < numSubSprites;
         i ++ ) {
        
        int             subHandle;
        MaxiginSprite  *subSprite;

        subHandle = mx_stripSubSprites[ i + newStrip->startIndex ];
        subSprite = &( mx_sprites[ subHandle ] );
        
        if( subSprite->glowSpriteHandle != -1 ) {
            mx_regenerateGlowSprite(
                subHandle,
                subSprite->glowRadius,
                subSprite->glowIterations );
            }
        if( subSprite->kerningTableIndex != -1 ) {
            mx_regenerateSpriteKerning( subHandle );
            }
        }

    mainSprite->stripChildHandle = newStripHandle;
    
    return newStripHandle;
    }



int maxigin_getNumSpritesInStrip( int  inSpriteStripHandle ) {
    return mx_spriteStrips[ inSpriteStripHandle ].numSubSprites;
    }



int maxigin_getSpriteFromStrip( int  inSpriteStripHandle,
                                int  inSpriteIndex ) {
    return mx_stripSubSprites[
        mx_spriteStrips[ inSpriteStripHandle ].startIndex + inSpriteIndex ];
    }



void maxigin_initMakeGlowSpriteStrip( int  inSpriteStripHandle,
                                      int  inBlurRadius,
                                      int  inBlurIterations ) {

    int  numSubSprites;
    int  i;

    
    if( inSpriteStripHandle < 0 ) {
        return;
        }
    
    numSubSprites = mx_spriteStrips[ inSpriteStripHandle ].numSubSprites;
    
    for( i = 0;
         i < numSubSprites;
         i ++ ) {
        
        int  subSpriteHandle  =
            mx_stripSubSprites[
                mx_spriteStrips[ inSpriteStripHandle ].startIndex + i ];

        maxigin_initMakeGlowSprite( subSpriteHandle,
                                    inBlurRadius,
                                    inBlurIterations );
        }
    }



static  char  mx_additiveBlend  =  0;



void maxigin_drawToggleAdditive( char  inAdditiveOn ) {
    mx_additiveBlend = inAdditiveOn;
    }



char maxigin_drawGetAdditive( void ) {
    return mx_additiveBlend;
    }



typedef union MaxiginColor {

        struct {
                
                unsigned char  red;
                
                unsigned char  green;
                
                unsigned char  blue;
                
                unsigned char  alpha;
                
            } comp;

        unsigned char val[ 4 ];
        
    } MaxiginColor;



static MaxiginColor  mx_drawColor  =  { { 255, 255, 255, 255 } };


void maxigin_drawSetColor( unsigned char  inRed,
                           unsigned char  inGreen,
                           unsigned char  inBlue,
                           unsigned char  inAlpha ) {

    mx_drawColor.comp.red    =  inRed;
    mx_drawColor.comp.green  =  inGreen;
    mx_drawColor.comp.blue   =  inBlue;
    mx_drawColor.comp.alpha  =  inAlpha;
    
    }



void maxigin_drawSetAlpha( unsigned char  inAlpha ) {
    
    mx_drawColor.comp.alpha  =  inAlpha;

    }



void maxigin_drawResetColor( void ) {
    mx_drawColor.comp.red    =  255;
    mx_drawColor.comp.green  =  255;
    mx_drawColor.comp.blue   =  255;
    mx_drawColor.comp.alpha  =  255;
    }



static void mx_drawRegularSprite( int  inSpriteHandle,
                                  int  inCenterX,
                                  int  inCenterY ) {

    int  startImageX;
    int  startImageY;
    int  endImageX;
    int  endImageY;
    
    int  startSpriteX;
    int  startSpriteY;
    int  endSpriteX;
    int  endSpriteY;
    
    int  x;
    int  y;
    int  imY;
    
    int  w;
    int  h;
    int  imW;
    int  imH;
    
    int  startByte;

    char drawAlphaSet;
    char drawColorSet;
    
    
    if( ! mx_areWeInMaxiginGameDrawFunction ) {

        mingin_log( "Game tried to call maxigin_drawSprite "
                    "from outside of maxiginGame_getNativePixels\n" );
        return;
        }

    if( inSpriteHandle == -1 ) {
        /* trying to draw a sprite that failed to load
           draw a place-holder rect so that it's not totally invisible*/

        maxigin_drawRect( inCenterX - 3,
                          inCenterY - 3,
                          inCenterX + 3,
                          inCenterY + 3 );

        maxigin_drawLine( inCenterX - 2,
                          inCenterY - 2,
                          inCenterX + 2,
                          inCenterY + 2 );
        return;
        }
    
    if( inSpriteHandle >= mx_numSprites ) {
        maxigin_logInt( "Game tried to draw an unknown sprite handle: ",
                        inSpriteHandle );
        }

    
    drawAlphaSet = 0;

    if( mx_drawColor.comp.alpha < 255 ) {
        drawAlphaSet = 1;
        
        if( mx_drawColor.comp.alpha == 0 ) {

            /* draw alpha of 0 means sprite is invisible, skip it */
            return;
            }
        }

    drawColorSet = 0;

    if( mx_drawColor.comp.red < 255
        ||
        mx_drawColor.comp.green < 255
        ||
        mx_drawColor.comp.blue < 255 ) {
        
        drawColorSet = 1;
        }
    
    
    w             =  mx_sprites[ inSpriteHandle ].w;
    h             =  mx_sprites[ inSpriteHandle ].h;
    startByte     =  mx_sprites[ inSpriteHandle ].startByte;

    imW           =  MAXIGIN_GAME_NATIVE_W;
    imH           =  MAXIGIN_GAME_NATIVE_H;
    
    startImageX   =  inCenterX - w / 2;
    startImageY   =  inCenterY - h / 2;
    endImageX     =  startImageX + w;
    endImageY     =  startImageY + h;

    startSpriteX  =  0;
    startSpriteY  =  0;
    endSpriteX    =  w;
    endSpriteY    =  h;

    /* handle case where sprite is cut off by edge of game image */

    if( startImageX < 0 ) {
        startSpriteX -= startImageX;
        startImageX = 0;
        }
    if( startImageY < 0 ) {
        startSpriteY -= startImageY;
        startImageY = 0;
        }

    if( endImageX > imW ) {
        endSpriteX -= ( endImageX - imW );
        }
    if( endImageY > imH ) {
        endSpriteY -= ( endImageY - imH );
        }

    if( startImageX >= imW
        ||
        startImageY >= imH
        ||
        endImageX   <=  0
        ||
        endImageY   <=  0 ) {

        /* sprite drawn completely out of bounds, affecting no pixels at all */
        return;
        }
    
    
    imY = startImageY;
    
    for( y = startSpriteY;
         y < endSpriteY;
         y ++ ) {

        int spriteByte  =  startByte  +    y * 4 *   w   +  4 * startSpriteX;
        int imageByte   =                imY * 3 * imW   +  3 * startImageX;

        if( mx_additiveBlend ) {
            /* different row loop for additive blending */

            for( x = startSpriteX;
                 x < endSpriteX;
                 x ++ ) {

                unsigned char  r;
                unsigned char  g;
                unsigned char  b;
                unsigned char  a  =  mx_spriteBytes[ spriteByte + 3 ];
                
                if( drawAlphaSet ) {
                    a = (unsigned char)( ( a * mx_drawColor.comp.alpha ) / 255 );
                    }
                
                if( a == 0 ) {
                    imageByte += 3;
                    spriteByte += 4;
                    continue;
                    }

                r = mx_spriteBytes[ spriteByte ++ ];
                g = mx_spriteBytes[ spriteByte ++ ];
                b = mx_spriteBytes[ spriteByte ++ ];

                if( drawColorSet ) {
                    r = (unsigned char)( ( r * mx_drawColor.comp.red   ) / 255 );
                    g = (unsigned char)( ( g * mx_drawColor.comp.green ) / 255 );
                    b = (unsigned char)( ( b * mx_drawColor.comp.blue  ) / 255 );
                    }
                
                if( a == 255 ) {

                    int  v;

                    /* red */
                    v = mx_gameImageBuffer[ imageByte  ] + r;

                    if( v > 255 ) {
                        v = 255;
                        }
                    mx_gameImageBuffer[ imageByte ] = (unsigned char)v;

                    imageByte  ++;

                    /* green */
                    v = mx_gameImageBuffer[ imageByte  ] + g;

                    if( v > 255 ) {
                        v = 255;
                        }
                    mx_gameImageBuffer[ imageByte ] = (unsigned char)v;

                    imageByte  ++;

                    /* blue */
                    v = mx_gameImageBuffer[ imageByte  ] + b;

                    if( v > 255 ) {
                        v = 255;
                        }
                    mx_gameImageBuffer[ imageByte ] = (unsigned char)v;

                    imageByte  ++;
                    }
                else {
                    /* alpha blending */

                    int  v;
                    
                    /* red */
                    v = mx_gameImageBuffer[ imageByte ]
                        +
                        ( r * a ) / 255;
                    
                    if( v > 255 ) {
                        v = 255;
                        }
                    mx_gameImageBuffer[ imageByte ] = (unsigned char)v;
                
                    imageByte  ++;

                    /* green */
                    v = mx_gameImageBuffer[ imageByte ]
                        +
                        ( g * a ) / 255;
                    
                    if( v > 255 ) {
                        v = 255;
                        }
                    mx_gameImageBuffer[ imageByte ] = (unsigned char)v;
                
                    imageByte  ++;

                    /* blue */
                    v = mx_gameImageBuffer[ imageByte ]
                        +
                        ( b * a ) / 255;
                    
                    if( v > 255 ) {
                        v = 255;
                        }
                    mx_gameImageBuffer[ imageByte ] = (unsigned char)v;
                
                    imageByte  ++;
                    }
            
                /* skip the alpha in the sprite
                   dest image has no alpha channel */
                spriteByte ++;
                }
            }
        else {
            /* non-additive blending row loop */
            
            for( x = startSpriteX;
                 x < endSpriteX;
                 x ++ ) {
            
                unsigned char  r;
                unsigned char  g;
                unsigned char  b;
                unsigned char  a  =  mx_spriteBytes[ spriteByte + 3 ];

                if( drawAlphaSet ) {
                    a = (unsigned char)( ( a * mx_drawColor.comp.alpha ) / 255 );
                    }
                                
                if( a == 0 ) {
                    imageByte += 3;
                    spriteByte += 4;
                    continue;
                    }

                r = mx_spriteBytes[ spriteByte ++ ];
                g = mx_spriteBytes[ spriteByte ++ ];
                b = mx_spriteBytes[ spriteByte ++ ];

                if( drawColorSet ) {
                    r = (unsigned char)( ( r * mx_drawColor.comp.red   ) / 255 );
                    g = (unsigned char)( ( g * mx_drawColor.comp.green ) / 255 );
                    b = (unsigned char)( ( b * mx_drawColor.comp.blue  ) / 255 );
                    }
                
                if( a == 255 ) {
                    /* RGBA bytes */
                    mx_gameImageBuffer[ imageByte  ++ ] = r;

                    mx_gameImageBuffer[ imageByte  ++ ] = g;

                    mx_gameImageBuffer[ imageByte  ++ ] = b;
                    }
                else {
                    /* alpha blending */

                    /* red */
                    mx_gameImageBuffer[ imageByte ] =
                        (unsigned char)( 
                            ( mx_gameImageBuffer[ imageByte ] * ( 255 - a )
                              +
                              r * a )
                            /
                            255 );
                
                    imageByte  ++;

                    /* green */
                    mx_gameImageBuffer[ imageByte ] =
                        (unsigned char)( 
                            ( mx_gameImageBuffer[ imageByte ] * ( 255 - a )
                              +
                              g * a )
                            /
                            255 );
                
                    imageByte  ++;

                    /* blue */
                    mx_gameImageBuffer[ imageByte ] =
                        (unsigned char)( 
                            ( mx_gameImageBuffer[ imageByte ] * ( 255 - a )
                              +
                              b * a )
                            /
                            255 );
                
                    imageByte  ++;
                    }
            
                /* skip the alpha in the sprite
                   dest image has no alpha channel */
                spriteByte ++;
                }
            }
        
        imY ++;
        }
    
    }



static void mx_drawGlowSprite( int  inSpriteHandle,
                             int  inCenterX,
                             int  inCenterY ) {

    char  oldAdditive  =  maxigin_drawGetAdditive();
    
    mx_drawRegularSprite( inSpriteHandle,
                          inCenterX,
                          inCenterY );

    if( mx_sprites[ inSpriteHandle ].glowSpriteHandle != -1 ) {

        maxigin_drawToggleAdditive( 1 );

        mx_drawRegularSprite( mx_sprites[ inSpriteHandle ].glowSpriteHandle,
                              inCenterX,
                              inCenterY );

        maxigin_drawToggleAdditive( oldAdditive );
        }
    }



void maxigin_drawSprite( int  inSpriteHandle,
                         int  inCenterX,
                         int  inCenterY ) {
    
    if( mx_sprites[ inSpriteHandle ].glowSpriteHandle != -1 ) {

        mx_drawGlowSprite( inSpriteHandle,
                           inCenterX,
                           inCenterY );
        }
    else {
        mx_drawRegularSprite( inSpriteHandle,
                              inCenterX,
                              inCenterY );
        }
    }

    



static int mx_abs( int  inValue ) {
    if( inValue < 0 ) {
        return - inValue;
        }
    else {
        return inValue;
        }
    }



static void mx_drawLineLow( int  inStartX,
                            int  inStartY,
                            int  inEndX,
                            int  inEndY ) {
    
    int  dX    =  inEndX - inStartX;
    int  dY    =  inEndY - inStartY;
    int  yDir  =  1;
    int  d;
    int  x;
    int  y;
    int  pixelStartByte;
    int  lineA            =  mx_drawColor.comp.alpha;
    
    /* color components with alpha pre-multiplied */
    int  linePreR         =  mx_drawColor.comp.red   * lineA;
    int  linePreG         =  mx_drawColor.comp.green * lineA;
    int  linePreB         =  mx_drawColor.comp.blue  * lineA;
    int  lineScaledR      =  linePreR / 255;
    int  lineScaledG      =  linePreG / 255;
    int  lineScaledB      =  linePreB / 255;

    
    if( dY < 0 ) {
        yDir = -1;
        dY   = -dY;
        }
    
    d = ( 2 * dY ) - dX;
    
    y = inStartY;


    /* easy bounds checking on end for x, truncate line */
    if( inEndX >= MAXIGIN_GAME_NATIVE_W ) {
        inEndX = MAXIGIN_GAME_NATIVE_W - 1;
        }

    /* walk through start of line until it is in-bounds in our image */
    while( inStartX < 0
           ||
           y < 0
           ||
           y >= MAXIGIN_GAME_NATIVE_H ) {

        inStartX ++;

        if( inStartX > inEndX ) {
            break;
            }
        
        if( d > 0 ) {
        
            y += yDir;
            
            d += 2 * ( dY - dX );
            }
        else {
            d += 2 * dY;
            }
        }

    if( inStartX > inEndX
        ||
        y >= MAXIGIN_GAME_NATIVE_H
        ||
        y < 0 ) {

        /* line is entirely outside of our image */
        return;
        }

    
    pixelStartByte =
        y * MAXIGIN_GAME_NATIVE_W * 3
        +
        inStartX * 3;
    
    for( x  = inStartX;
         x <= inEndX;
         x ++ ) {

        if( mx_additiveBlend ) {
            
            int  v;

            v = mx_gameImageBuffer[ pixelStartByte ] + lineScaledR;

            if( v > 255 ) {
                v = 255;
                }
            mx_gameImageBuffer[ pixelStartByte ] = (unsigned char)v;

                
            v = mx_gameImageBuffer[ pixelStartByte + 1 ] + lineScaledG;
                
            if( v > 255 ) {
                v = 255;
                }
            mx_gameImageBuffer[ pixelStartByte + 1 ] = (unsigned char)v;

                
            v = mx_gameImageBuffer[ pixelStartByte + 2 ] + lineScaledB;
                
            if( v > 255 ) {
                v = 255;
                }
            mx_gameImageBuffer[ pixelStartByte + 2 ] = (unsigned char)v;

            /* next col */
            pixelStartByte += 3;
            }
        else {
            if( lineA == 255 ) {
        
                /* no blend, pure replace */
                mx_gameImageBuffer[ pixelStartByte ++ ] =
                    mx_drawColor.comp.red;
                
                mx_gameImageBuffer[ pixelStartByte ++ ] =
                    mx_drawColor.comp.green;
                
                mx_gameImageBuffer[ pixelStartByte ++ ] =
                    mx_drawColor.comp.blue;
                
                }
            else {
                
                /* alpha weighted blend, most expensive case */
                
                mx_gameImageBuffer[ pixelStartByte ] =
                    (unsigned char)( 
                        ( mx_gameImageBuffer[ pixelStartByte ]
                          * ( 255 - lineA )
                          +
                          linePreR )
                        /
                        255 );

                mx_gameImageBuffer[ pixelStartByte + 1 ] =
                    (unsigned char)( 
                        ( mx_gameImageBuffer[ pixelStartByte + 1 ]
                          * ( 255 - lineA )
                          +
                          linePreG )
                        /
                        255 );

                
                mx_gameImageBuffer[ pixelStartByte + 2 ] =
                    (unsigned char)( 
                        ( mx_gameImageBuffer[ pixelStartByte + 2 ]
                          * ( 255 - lineA )
                          +
                          linePreB )
                        /
                        255 );
                
                /* next col */
                pixelStartByte += 3;
                }
            }
        
        if( d > 0 ) {
        
            y += yDir;


            /* bail out if y ever steps out of bounds */
            if( y < 0
                ||
                y >= MAXIGIN_GAME_NATIVE_H ) {
                break;
                }
            
            pixelStartByte += yDir * MAXIGIN_GAME_NATIVE_W * 3;
                
            d += 2 * ( dY - dX );
            }
        else {
            d += 2 * dY;
            }

        }
    }



static void mx_drawLineHigh( int  inStartX,
                             int  inStartY,
                             int  inEndX,
                             int  inEndY ) {
    
    int  dX               =  inEndX - inStartX;
    int  dY               =  inEndY - inStartY;
    int  xDir             =  1;
    int  d;
    int  x;
    int  y;
    int  pixelStartByte;
    int  lineA            =  mx_drawColor.comp.alpha;
    
    /* color components with alpha pre-multiplied */
    int  linePreR         =  mx_drawColor.comp.red   * lineA;
    int  linePreG         =  mx_drawColor.comp.green * lineA;
    int  linePreB         =  mx_drawColor.comp.blue  * lineA;
    int  lineScaledR      =  linePreR / 255;
    int  lineScaledG      =  linePreG / 255;
    int  lineScaledB      =  linePreB / 255;
    
    
    if( dX < 0 ) {
        xDir = -1;
        dX   = -dX;
        }
    
    d = ( 2 * dX ) - dY;
    
    x = inStartX;

    
    /* easy bounds checking on end for y, truncate line */
    if( inEndY >= MAXIGIN_GAME_NATIVE_H ) {
        inEndY = MAXIGIN_GAME_NATIVE_H - 1;
        }

    /* walk through start of line until it is in-bounds in our image */
    while( inStartY < 0
           ||
           x < 0
           ||
           x >= MAXIGIN_GAME_NATIVE_W ) {

        inStartY ++;

        if( inStartY > inEndY ) {
            break;
            }
        
        if( d > 0 ) {
        
            x += xDir;
            
            d += 2 * ( dX - dY );
            }
        else {
            d += 2 * dX;
            }
        }

    if( inStartY > inEndY
        ||
        x < 0
        ||
        x >= MAXIGIN_GAME_NATIVE_W ) {

        /* line is entirely outside of our image */
        return;
        }
    
    pixelStartByte =
        inStartY * MAXIGIN_GAME_NATIVE_W * 3
        +
        x * 3;
    
    for( y  = inStartY;
         y <= inEndY;
         y ++ ) {
        
        if( mx_additiveBlend ) {
            
            int  v;

            v = mx_gameImageBuffer[ pixelStartByte ] + lineScaledR;

            if( v > 255 ) {
                v = 255;
                }
            mx_gameImageBuffer[ pixelStartByte ] = (unsigned char)v;

                
            v = mx_gameImageBuffer[ pixelStartByte + 1 ] + lineScaledG;
                
            if( v > 255 ) {
                v = 255;
                }
            mx_gameImageBuffer[ pixelStartByte + 1 ] = (unsigned char)v;

                
            v = mx_gameImageBuffer[ pixelStartByte + 2 ] + lineScaledB;
                
            if( v > 255 ) {
                v = 255;
                }
            mx_gameImageBuffer[ pixelStartByte + 2 ] = (unsigned char)v;
            }
        else {
            if( lineA == 255 ) {
        
                /* no blend, pure replace */
                mx_gameImageBuffer[ pixelStartByte     ] =
                    mx_drawColor.comp.red;
                
                mx_gameImageBuffer[ pixelStartByte + 1 ] =
                    mx_drawColor.comp.green;
                
                mx_gameImageBuffer[ pixelStartByte + 2 ] =
                    mx_drawColor.comp.blue;
                
                }
            else {
                
                /* alpha weighted blend, most expensive case */
                
                mx_gameImageBuffer[ pixelStartByte ] =
                    (unsigned char)( 
                        ( mx_gameImageBuffer[ pixelStartByte ]
                          * ( 255 - lineA )
                          +
                          linePreR )
                        /
                        255 );

                mx_gameImageBuffer[ pixelStartByte + 1 ] =
                    (unsigned char)( 
                        ( mx_gameImageBuffer[ pixelStartByte + 1 ]
                          * ( 255 - lineA )
                          +
                          linePreG )
                        /
                        255 );

                
                mx_gameImageBuffer[ pixelStartByte + 2 ] =
                    (unsigned char)( 
                        ( mx_gameImageBuffer[ pixelStartByte + 2 ]
                          * ( 255 - lineA )
                          +
                          linePreB )
                        /
                        255 );
                }
            }
                
        
        if( d > 0 ) {
        
            x += xDir;

            /* bail out if x ever steps out of bounds */
            if( x < 0
                ||
                x >= MAXIGIN_GAME_NATIVE_W ) {
                break;
                }
            
            pixelStartByte += xDir * 3;
            
            d += 2 * ( dX - dY );
            }
        else {
            d += 2 * dX;
            }

        /* next row */
        pixelStartByte += MAXIGIN_GAME_NATIVE_W * 3;
        }
    }



/* Draws a horizontal line with no bounds or direction checking.

   Parameters:

       inStartX   the start x location of the line
                  >= 0
                  <  MAXIGIN_GAME_NATIVE_W
                  
       inEndX     the end x location of the line
                  >= 0
                  <  MAXIGIN_GAME_NATIVE_W
                  >= inStartX

       inY        the y location of the line
                  >= 0
                  <  MAXIGIN_GAME_NATIVE_H
*/
static void mx_drawFastHorizontalLine( int  inStartX,
                                       int  inEndX,
                                       int  inY ) {
            
    int  x;
    int  pixelStartByte;
    int  lineA            =  mx_drawColor.comp.alpha;
        
    /* color components with alpha pre-multiplied */
    int  linePreR         =  mx_drawColor.comp.red   * lineA;
    int  linePreG         =  mx_drawColor.comp.green * lineA;
    int  linePreB         =  mx_drawColor.comp.blue  * lineA;
    int  lineScaledR      =  linePreR / 255;
    int  lineScaledG      =  linePreG / 255;
    int  lineScaledB      =  linePreB / 255;
        
    pixelStartByte  =  inY * MAXIGIN_GAME_NATIVE_W * 3 + inStartX * 3;
        
    if( mx_additiveBlend ) {
        /* additive blend */
            
        for( x  = inStartX;
             x <= inEndX;
             x ++ ) {
                
            int  v;

            v = mx_gameImageBuffer[ pixelStartByte ] + lineScaledR;
            if( v > 255 ) {
                v = 255;
                }
            mx_gameImageBuffer[ pixelStartByte ] = (unsigned char)v;

            v = mx_gameImageBuffer[ pixelStartByte + 1 ] + lineScaledG;
            if( v > 255 ) {
                v = 255;
                }
            mx_gameImageBuffer[ pixelStartByte + 1 ] = (unsigned char)v;

            v = mx_gameImageBuffer[ pixelStartByte + 2 ] + lineScaledB;
            if( v > 255 ) {
                v = 255;
                }
            mx_gameImageBuffer[ pixelStartByte + 2 ] = (unsigned char)v; 

            /* next col */
            pixelStartByte += 3;
            }
        }
    else {
        if( lineA == 255 ) {
            /* no blend, pure replace */
                
            for( x  = inStartX;
                 x <= inEndX;
                 x ++ ) {
                    
                mx_gameImageBuffer[ pixelStartByte ++ ] =
                    mx_drawColor.comp.red;
                
                mx_gameImageBuffer[ pixelStartByte ++ ] =
                    mx_drawColor.comp.green;
                
                mx_gameImageBuffer[ pixelStartByte ++ ] =
                    mx_drawColor.comp.blue;
                }
            }
        else {
            /* weighted alpha blend */
                
            for( x  = inStartX;
                 x <= inEndX;
                 x ++ ) {
                    
                mx_gameImageBuffer[ pixelStartByte ] =
                    (unsigned char)( 
                        ( mx_gameImageBuffer[ pixelStartByte ]
                          * ( 255 - lineA )
                          +
                          linePreR )
                        /
                        255 );

                mx_gameImageBuffer[ pixelStartByte + 1 ] =
                    (unsigned char)( 
                        ( mx_gameImageBuffer[ pixelStartByte + 1 ]
                          * ( 255 - lineA )
                          +
                          linePreG )
                        /
                        255 );

                
                mx_gameImageBuffer[ pixelStartByte + 2 ] =
                    (unsigned char)( 
                        ( mx_gameImageBuffer[ pixelStartByte + 2 ]
                          * ( 255 - lineA )
                          +
                          linePreB )
                        /
                        255 );

                /* next col */
                pixelStartByte += 3;
                } 
            }
        }
    }



void maxigin_drawLine( int  inStartX,
                       int  inStartY,
                       int  inEndX,
                       int  inEndY ) {

    
    if( mx_drawColor.comp.alpha == 0 ) {
        return;
        }
    

    /* special case: vert line */
    if( inStartX == inEndX ) {

        int  y;
        int  temp;
        int  pixelStartByte;
        int  lineA            =  mx_drawColor.comp.alpha;
        int  rowHop;
        
        /* color components with alpha pre-multiplied */
        int  linePreR         =  mx_drawColor.comp.red   * lineA;
        int  linePreG         =  mx_drawColor.comp.green * lineA;
        int  linePreB         =  mx_drawColor.comp.blue  * lineA;
        int  lineScaledR      =  linePreR / 255;
        int  lineScaledG      =  linePreG / 255;
        int  lineScaledB      =  linePreB / 255;
        
        if( inStartX < 0
            ||
            inStartX >= MAXIGIN_GAME_NATIVE_W ) {
            
            /* vertical line outside left or right side of image */
            return;
            }

        if( inStartY < 0
            &&
            inEndY < 0 ) {
            /* completely outside top side of image */
            return;
            }
        if( inStartY >= MAXIGIN_GAME_NATIVE_H
            &&
            inEndY   >= MAXIGIN_GAME_NATIVE_H ) {
            /* completely outside bottom side of image */
            return;
            }

        /* truncate vertical line inside image */
        if( inStartY < 0 ) {
            inStartY = 0;
            }
        else if( inStartY >= MAXIGIN_GAME_NATIVE_H ) {
            inStartY = MAXIGIN_GAME_NATIVE_H - 1;
            }
        if( inEndY < 0 ) {
            inEndY = 0;
            }
        else if( inEndY >= MAXIGIN_GAME_NATIVE_H ) {
            inEndY = MAXIGIN_GAME_NATIVE_H - 1;
            }
        

        /* fix direction */
        if( inStartY > inEndY ) {
            temp     = inEndY;
            inEndY   = inStartY;
            inStartY = temp;
            }
        

        rowHop          =  MAXIGIN_GAME_NATIVE_W * 3;
        pixelStartByte  =  inStartY * rowHop + inStartX * 3;
        
        
        if( mx_additiveBlend ) {
            /* additive blend */
            
            for( y  = inStartY;
                 y <= inEndY;
                 y ++ ) {
                
                int  v;

                v = mx_gameImageBuffer[ pixelStartByte ] + lineScaledR;
                if( v > 255 ) {
                    v = 255;
                    }
                mx_gameImageBuffer[ pixelStartByte ] = (unsigned char)v;

                v = mx_gameImageBuffer[ pixelStartByte + 1 ] + lineScaledG;
                if( v > 255 ) {
                    v = 255;
                    }
                mx_gameImageBuffer[ pixelStartByte + 1 ] = (unsigned char)v;

                v = mx_gameImageBuffer[ pixelStartByte + 2 ] + lineScaledB;
                if( v > 255 ) {
                    v = 255;
                    }
                mx_gameImageBuffer[ pixelStartByte + 2 ] = (unsigned char)v; 

                /* next row */
                pixelStartByte += rowHop;
                }
            }
        else {
            if( lineA == 255 ) {
                /* no blend, pure replace */
                
                for( y  = inStartY;
                     y <= inEndY;
                     y ++ ) {
                    
                    mx_gameImageBuffer[ pixelStartByte     ] =
                        mx_drawColor.comp.red;
                
                    mx_gameImageBuffer[ pixelStartByte + 1 ] =
                        mx_drawColor.comp.green;
                
                    mx_gameImageBuffer[ pixelStartByte + 2 ] =
                        mx_drawColor.comp.blue;

                    /* next row */
                    pixelStartByte += rowHop;
                    }
                }
            else {
                /* weighted alpha blend */
                
                for( y  = inStartY;
                     y <= inEndY;
                     y ++ ) {
                    
                    mx_gameImageBuffer[ pixelStartByte ] =
                        (unsigned char)( 
                            ( mx_gameImageBuffer[ pixelStartByte ]
                              * ( 255 - lineA )
                              +
                              linePreR )
                            /
                            255 );

                    mx_gameImageBuffer[ pixelStartByte + 1 ] =
                        (unsigned char)( 
                            ( mx_gameImageBuffer[ pixelStartByte + 1 ]
                              * ( 255 - lineA )
                              +
                              linePreG )
                            /
                            255 );

                
                    mx_gameImageBuffer[ pixelStartByte + 2 ] =
                        (unsigned char)( 
                            ( mx_gameImageBuffer[ pixelStartByte + 2 ]
                              * ( 255 - lineA )
                              +
                              linePreB )
                            /
                            255 );
                    
                    /* next row */
                    pixelStartByte += rowHop;
                    }
                }
            }
        return;
        }

    
    /* special case: horizontal line */
    if( inStartY == inEndY ) {
        
        int  temp;

        if( inStartY < 0
            ||
            inStartY >= MAXIGIN_GAME_NATIVE_H ) {
            
            /* horizontal line above or below image */
            return;
            }

        if( inStartX < 0
            &&
            inEndX < 0 ) {
            /* completely outside left side of image */
            return;
            }
        if( inStartX >= MAXIGIN_GAME_NATIVE_W
            &&
            inEndX   >= MAXIGIN_GAME_NATIVE_W ) {
            /* completely outside right side of image */
            return;
            }
        
        /* truncate horizontal line inside image */
        if( inStartX < 0 ) {
            inStartX = 0;
            }
        else if( inStartX >= MAXIGIN_GAME_NATIVE_W ) {
            inStartX = MAXIGIN_GAME_NATIVE_W - 1;
            }
        if( inEndX < 0 ) {
            inEndX = 0;
            }
        else if( inEndX >= MAXIGIN_GAME_NATIVE_W ) {
            inEndX = MAXIGIN_GAME_NATIVE_W - 1;
            }

        /* fix direction */
        if( inStartX > inEndX ) {
            temp     = inEndX;
            inEndX   = inStartX;
            inStartX = temp;
            }

        mx_drawFastHorizontalLine( inStartX,
                                   inEndX,
                                   inStartY );
        return;
        }
    
    
    /* General-case Bresenham's line algorithm */

    if( mx_abs( inEndY - inStartY )
        <
        mx_abs( inEndX - inStartX ) ) {
        
        if( inStartX > inEndX ) {
            mx_drawLineLow( inEndX, inEndY, inStartX, inStartY );
            }
        else {
            mx_drawLineLow( inStartX, inStartY, inEndX, inEndY );
            }
        }
    else {
        if( inStartY > inEndY ) {
            mx_drawLineHigh( inEndX, inEndY, inStartX, inStartY );
            }
        else {
            mx_drawLineHigh( inStartX, inStartY, inEndX, inEndY );
            }
        }
    }



void maxigin_drawRect( int  inStartX,
                       int  inStartY,
                       int  inEndX,
                       int  inEndY ) {

    /* prevent overlapping pixels at corners.
       to make this easier, first ensure start is top left
       and end is bottom right. */

    int  temp;

    if( inStartX > inEndX ) {
        temp     = inEndX;
        inEndX   = inStartX;
        inStartX = temp;
        }
    if( inStartY > inEndY ) {
        temp     = inEndY;
        inEndY   = inStartY;
        inStartY = temp;
        }
        
    maxigin_drawLine( inStartX,
                      inStartY,
                      inStartX,
                      inEndY );
    
    maxigin_drawLine( inStartX + 1,  /* no overlap */
                      inEndY,
                      inEndX,
                      inEndY );
    
    maxigin_drawLine( inEndX,
                      inEndY - 1,    /* no overlap */
                      inEndX,
                      inStartY );
    
    maxigin_drawLine( inEndX - 1,    /* no overlap */
                      inStartY,
                      inStartX + 1,  /* no overlap */
                      inStartY );
    }



void maxigin_drawFillRect( int  inStartX,
                           int  inStartY,
                           int  inEndX,
                           int  inEndY ) {

    int  x;
    int  y;
    int  pixelStartByte;
    int  lineA            =  mx_drawColor.comp.alpha;
    int  rowHop;
    int  temp;
    
    /* color components with alpha pre-multiplied */   
    int  linePreR         =  mx_drawColor.comp.red   * lineA;
    int  linePreG         =  mx_drawColor.comp.green * lineA;
    int  linePreB         =  mx_drawColor.comp.blue  * lineA;
    int  lineScaledR      =  linePreR / 255;
    int  lineScaledG      =  linePreG / 255;
    int  lineScaledB      =  linePreB / 255;
    
    if( lineA == 0 ) {
        return;
        }

    /* skip rects that are completely out of bounds */
    if( inStartX < 0
        &&
        inEndX < 0 ) {
        
        return;
        }
    if( inStartX >= MAXIGIN_GAME_NATIVE_W
        &&
        inEndX   >= MAXIGIN_GAME_NATIVE_W ) {
        
        return;
        }
    if( inStartY < 0
        &&
        inEndY < 0 ) {
        
        return;
        }
    if( inStartY >= MAXIGIN_GAME_NATIVE_H
        &&
        inEndY   >= MAXIGIN_GAME_NATIVE_H ) {
        
        return;
        }
    
    /* trim to be in-bounds of image */
    if( inStartX < 0 ) {
        inStartX = 0;
        }
    if( inStartX >= MAXIGIN_GAME_NATIVE_W ) {
        inStartX = MAXIGIN_GAME_NATIVE_W - 1;
        }
    if( inEndX < 0 ) {
        inEndX = 0;
        }
    if( inEndX >= MAXIGIN_GAME_NATIVE_W ) {
        inEndX = MAXIGIN_GAME_NATIVE_W - 1;
        }
    if( inStartY < 0 ) {
        inStartY = 0;
        }
    if( inStartY >= MAXIGIN_GAME_NATIVE_H ) {
        inStartY = MAXIGIN_GAME_NATIVE_H - 1;
        }
    if( inEndY < 0 ) {
        inEndY = 0;
        }
    if( inEndY >= MAXIGIN_GAME_NATIVE_H ) {
        inEndY = MAXIGIN_GAME_NATIVE_H - 1;
        }

    /* fix draw direction */
    if( inStartX > inEndX ) {
        temp     = inEndX;
        inEndX   = inStartX;
        inStartX = temp;
        }
    if( inStartY > inEndY ) {
        temp     = inEndY;
        inEndY   = inStartY;
        inStartY = temp;
        }

    rowHop         = MAXIGIN_GAME_NATIVE_W * 3;
    pixelStartByte = inStartY * rowHop + inStartX * 3;

    if( mx_additiveBlend ) {
        /* additive blend */
        
        for( y =  inStartY;
             y <= inEndY;
             y ++ ) {

            pixelStartByte = y * rowHop + inStartX * 3;
            
            for( x =  inStartX;
                 x <= inEndX;
                 x ++ ) {
                
                int  v;

                v = mx_gameImageBuffer[ pixelStartByte ] + lineScaledR;
                if( v > 255 ) {
                    v = 255;
                    }
                mx_gameImageBuffer[ pixelStartByte ] = (unsigned char)v;

                v = mx_gameImageBuffer[ pixelStartByte + 1 ] + lineScaledG;
                if( v > 255 ) {
                    v = 255;
                    }
                mx_gameImageBuffer[ pixelStartByte + 1 ] = (unsigned char)v;

                v = mx_gameImageBuffer[ pixelStartByte + 2 ] + lineScaledB;
                if( v > 255 ) {
                    v = 255;
                    }
                mx_gameImageBuffer[ pixelStartByte + 2 ] = (unsigned char)v; 

                /* next col */
                pixelStartByte += 3;
                }
            }
        }
    else {

        if( lineA == 255 ) {
            /* replace color, no blend */
            
            for( y =  inStartY;
                 y <= inEndY;
                 y ++ ) {

                pixelStartByte = y * rowHop + inStartX * 3;

                for( x =  inStartX;
                     x <= inEndX;
                     x ++ ) {

                    mx_gameImageBuffer[ pixelStartByte ++ ] =
                        mx_drawColor.comp.red;
                    
                    mx_gameImageBuffer[ pixelStartByte ++ ] =
                        mx_drawColor.comp.green;
                    
                    mx_gameImageBuffer[ pixelStartByte ++ ] =
                        mx_drawColor.comp.blue;
                    }
                }
            }
        else {
            /* weighted alpha belnd */
                    
            for( y =  inStartY;
                 y <= inEndY;
                 y ++ ) {

                pixelStartByte = y * rowHop + inStartX * 3;

                for( x =  inStartX;
                     x <= inEndX;
                     x ++ ) {
                                     
                    mx_gameImageBuffer[ pixelStartByte ] =
                        (unsigned char)( 
                            ( mx_gameImageBuffer[ pixelStartByte ]
                              * ( 255 - lineA )
                              +
                              linePreR )
                            /
                            255 );

                    mx_gameImageBuffer[ pixelStartByte + 1 ] =
                        (unsigned char)( 
                            ( mx_gameImageBuffer[ pixelStartByte + 1 ]
                              * ( 255 - lineA )
                              +
                              linePreG )
                            /
                            255 );
                
                    mx_gameImageBuffer[ pixelStartByte + 2 ] =
                        (unsigned char)( 
                            ( mx_gameImageBuffer[ pixelStartByte + 2 ]
                              * ( 255 - lineA )
                              +
                              linePreB )
                            /
                            255 );

                    /* next col */
                    pixelStartByte += 3;
                    }
                }
            }
        }   
    }



void maxigin_initGUI( MaxiginGUI *inGUI ) {

    inGUI->zeroOffsetX        = MAXIGIN_GAME_NATIVE_W / 2;
    inGUI->zeroOffsetY        = MAXIGIN_GAME_NATIVE_H / 2;
    
    inGUI->hot                = 0;
    inGUI->active             = 0;
    inGUI->mouseDown          = 0;
    inGUI->activeMouseOffsetX = 0;
    inGUI->activeMouseOffsetY = 0;
    inGUI->numDrawComponents  = 0;
    }



void maxigin_drawGUI( MaxiginGUI *inGUI ) {

    int  i;
    int  drawType;
    int  xO         =  inGUI->zeroOffsetX;
    int  yO         =  inGUI->zeroOffsetY;

    for( i = 0;
         i < inGUI->numDrawComponents;
         i ++ ) {

        drawType = inGUI->drawComponents[i].drawType;
        
        maxigin_drawToggleAdditive( inGUI->drawComponents[i].additiveBlend );

        if( drawType == MX_GUI_DRAW_LINE
            ||
            drawType == MX_GUI_DRAW_RECT
            ||
            drawType == MX_GUI_FILL_RECT ) {

            maxigin_drawSetColor( inGUI->drawComponents[i].red,
                                  inGUI->drawComponents[i].green,
                                  inGUI->drawComponents[i].blue,
                                  inGUI->drawComponents[i].alpha );
            }
        else if( drawType == MX_GUI_DRAW_SPRITE
                 ||
                 drawType == MX_GUI_DRAW_SPRITE_SEQUENCE ) {

            maxigin_drawSetAlpha( inGUI->drawComponents[i].alpha );
            }

        switch( drawType ) {
            
            case MX_GUI_DRAW_LINE:
                maxigin_drawLine(
                    inGUI->drawComponents[i].drawParams.line.startX + xO,
                    inGUI->drawComponents[i].drawParams.line.startY + yO,
                    inGUI->drawComponents[i].drawParams.line.endX + xO,
                    inGUI->drawComponents[i].drawParams.line.endY + yO );
                break;
                
            case MX_GUI_DRAW_RECT:
                maxigin_drawRect(
                    inGUI->drawComponents[i].drawParams.rect.startX + xO,
                    inGUI->drawComponents[i].drawParams.rect.startY + yO,
                    inGUI->drawComponents[i].drawParams.rect.endX + xO,
                    inGUI->drawComponents[i].drawParams.rect.endY + yO );
                break;
                
            case MX_GUI_FILL_RECT:
                maxigin_drawFillRect(
                    inGUI->drawComponents[i].drawParams.rect.startX + xO,
                    inGUI->drawComponents[i].drawParams.rect.startY + yO,
                    inGUI->drawComponents[i].drawParams.rect.endX + xO,
                    inGUI->drawComponents[i].drawParams.rect.endY + yO );
                break;
                
            case MX_GUI_DRAW_SPRITE:
                maxigin_drawSprite(
                    inGUI->drawComponents[i].drawParams.sprite.spriteHandle,
                    inGUI->drawComponents[i].drawParams.sprite.centerX + xO,
                    inGUI->drawComponents[i].drawParams.sprite.centerY + yO );
                break;
            case MX_GUI_DRAW_SPRITE_SEQUENCE: {

                int  s;
                int  x   =  inGUI->drawComponents[i].
                                drawParams.spriteSequence.startCenterX + xO;
                int  y   =  inGUI->drawComponents[i].
                                drawParams.spriteSequence.startCenterY + yO;
                
                for( s = 0;
                     s < inGUI->drawComponents[i].
                         drawParams.spriteSequence.count;
                     s ++ ) {

                    maxigin_drawSprite(
                        inGUI->drawComponents[i].
                            drawParams.spriteSequence.spriteHandle,
                        x,
                        y );
                
                    x += inGUI->drawComponents[i].
                        drawParams.spriteSequence.offsetX;
                    y += inGUI->drawComponents[i].
                        drawParams.spriteSequence.offsetY;
                    }
                
                }
                break;            
            }
        }
    }



void maxigin_startGUI( MaxiginGUI *inGUI ) {
    inGUI->hot               = 0;
    inGUI->numDrawComponents = 0;
    }



void maxigin_endGUI( MaxiginGUI *inGUI ) {
    if( ! inGUI->mouseDown ) {

        if( mingin_isButtonDown( MAXIGIN_MOUSE_BUTTON ) ) {

            /* The button was pressed fresh this step, but NONE
               of our GUI components were hit by it.
               Mark it as pressed now, so none will react to it's pressed
               state by accident next step. */

            inGUI->mouseDown = 1;
            }
        }
    else {
        /* mouse marked as held down, has it released? */
        if( ! mingin_isButtonDown( MAXIGIN_MOUSE_BUTTON ) ) {

            inGUI->mouseDown = 0;

            /* endGUI called after all of our components have had a chance
               to update and notice that the mouse is no longer down.
               Any active component should have become non-active.
               However, if our GUI state is being loaded from a saved game
               state, this might not be the case (if a component was active
               when the game was saved). */
            
            inGUI->active = 0;
            }
        }
        
    }



static void mx_guiSetColor( MaxiginGUI    *inGUI,
                            int            inDrawComponentIndex,
                            char           inAdditiveBlend,
                            MaxiginColor  *inColor ) {

    int  i  =  inDrawComponentIndex;
    
    inGUI->drawComponents[i].additiveBlend = inAdditiveBlend;

    inGUI->drawComponents[i].red   = inColor->comp.red;
    inGUI->drawComponents[i].green = inColor->comp.green;
    inGUI->drawComponents[i].blue  = inColor->comp.blue;
    inGUI->drawComponents[i].alpha = inColor->comp.alpha;
    }



static void mx_guiAddLine( MaxiginGUI    *inGUI,
                           char           inAdditiveBlend,
                           MaxiginColor  *inColor,
                           int            inStartX,
                           int            inStartY,
                           int            inEndX,
                           int            inEndY ) {

    int  i  =  inGUI->numDrawComponents;

    if( i >= MAXIGIN_MAX_TOTAL_GUI_DRAW_COMPONENTS ) {
        mingin_log( "Error:  trying to add a line to a full "
                    "MaxiginGUI instance.\n" );
        return;
        }

    mx_guiSetColor( inGUI,
                    i,
                    inAdditiveBlend,
                    inColor );

    inGUI->drawComponents[i].drawType = MX_GUI_DRAW_LINE;
    
    inGUI->drawComponents[i].drawParams.line.startX = inStartX;
    inGUI->drawComponents[i].drawParams.line.startY = inStartY;
    inGUI->drawComponents[i].drawParams.line.endX   = inEndX;
    inGUI->drawComponents[i].drawParams.line.endY   = inEndY;

    inGUI->numDrawComponents ++;
    }



static void mx_guiAddRect( MaxiginGUI    *inGUI,
                           char           inAdditiveBlend,
                           MaxiginColor  *inColor,
                           int            inStartX,
                           int            inStartY,
                           int            inEndX,
                           int            inEndY,
                           int            inDrawType ) {

    int  i  =  inGUI->numDrawComponents;

    if( i >= MAXIGIN_MAX_TOTAL_GUI_DRAW_COMPONENTS ) {
        mingin_log( "Error:  trying to add a rectangle to a full "
                    "MaxiginGUI instance.\n" );
        return;
        }

    mx_guiSetColor( inGUI,
                    i,
                    inAdditiveBlend,
                    inColor );

    inGUI->drawComponents[i].drawType = inDrawType;
    
    inGUI->drawComponents[i].drawParams.rect.startX = inStartX;
    inGUI->drawComponents[i].drawParams.rect.startY = inStartY;
    inGUI->drawComponents[i].drawParams.rect.endX   = inEndX;
    inGUI->drawComponents[i].drawParams.rect.endY   = inEndY;

    inGUI->numDrawComponents ++;
    }


static void mx_guiAddDrawRect( MaxiginGUI    *inGUI,
                               char           inAdditiveBlend,
                               MaxiginColor  *inColor,
                               int            inStartX,
                               int            inStartY,
                               int            inEndX,
                               int            inEndY ) {

    mx_guiAddRect( inGUI,
                   inAdditiveBlend,
                   inColor,
                   inStartX,
                   inStartY,
                   inEndX,
                   inEndY,
                   MX_GUI_DRAW_RECT );
    }



static void mx_guiAddFillRect( MaxiginGUI    *inGUI,
                               char           inAdditiveBlend,
                               MaxiginColor  *inColor,
                               int            inStartX,
                               int            inStartY,
                               int            inEndX,
                               int            inEndY ) {
    
    mx_guiAddRect( inGUI,
                   inAdditiveBlend,
                   inColor,
                   inStartX,
                   inStartY,
                   inEndX,
                   inEndY,
                   MX_GUI_FILL_RECT );
    }


static void mx_guiAddSprite( MaxiginGUI    *inGUI,
                             char           inAdditiveBlend,
                             unsigned char  inAlpha,
                             int            inSpriteHandle,
                             int            inCenterX,
                             int            inCenterY ) {

    int           i  =  inGUI->numDrawComponents;
    MaxiginColor  c;
    
    if( i >= MAXIGIN_MAX_TOTAL_GUI_DRAW_COMPONENTS ) {
        mingin_log( "Error:  trying to add a sprite to a full "
                    "MaxiginGUI instance.\n" );
        return;
        }

    c.val[0] = 255;
    c.val[1] = 255;
    c.val[2] = 255;
    c.val[3] = inAlpha;
    
    mx_guiSetColor( inGUI,
                    i,
                    inAdditiveBlend,
                    &c );

    inGUI->drawComponents[i].drawType = MX_GUI_DRAW_SPRITE;
    
    inGUI->drawComponents[i].drawParams.sprite.spriteHandle = inSpriteHandle;
    inGUI->drawComponents[i].drawParams.sprite.centerX      = inCenterX;
    inGUI->drawComponents[i].drawParams.sprite.centerY      = inCenterY;

    inGUI->numDrawComponents ++;
    }



static void mx_guiAddSpriteSequence( MaxiginGUI    *inGUI,
                                     char           inAdditiveBlend,
                                     unsigned char  inAlpha,
                                     int            inSpriteHandle,
                                     int            inStartCenterX,
                                     int            inStartCenterY,
                                     int            inOffsetX,
                                     int            inOffsetY,
                                     int            inCount ) {

    int           i  =  inGUI->numDrawComponents;
    MaxiginColor  c;
    
    if( i >= MAXIGIN_MAX_TOTAL_GUI_DRAW_COMPONENTS ) {
        mingin_log( "Error:  trying to add a sprite sequence to a full "
                    "MaxiginGUI instance.\n" );
        return;
        }

    c.val[0] = 255;
    c.val[1] = 255;
    c.val[2] = 255;
    c.val[3] = inAlpha;
    
    mx_guiSetColor( inGUI,
                    i,
                    inAdditiveBlend,
                    &c );

    inGUI->drawComponents[i].drawType = MX_GUI_DRAW_SPRITE_SEQUENCE;
    
    inGUI->drawComponents[i].drawParams.spriteSequence.spriteHandle
        = inSpriteHandle;
    
    inGUI->drawComponents[i].drawParams.spriteSequence.startCenterX
        = inStartCenterX;
    
    inGUI->drawComponents[i].drawParams.spriteSequence.startCenterY
        = inStartCenterY;

    inGUI->drawComponents[i].drawParams.spriteSequence.offsetX
        = inOffsetX;
    
    inGUI->drawComponents[i].drawParams.spriteSequence.offsetY
        = inOffsetY;

    inGUI->drawComponents[i].drawParams.spriteSequence.count
        = inCount;

    inGUI->numDrawComponents ++;
    }



static void mx_makeColorGray( MaxiginColor   *inC,
                              unsigned char   inGrayVal ) {

    inC->val[0] = inGrayVal;
    inC->val[1] = inGrayVal;
    inC->val[2] = inGrayVal;
    }


static void mx_getSliderThumbRadius( int   inThumbWidth,
                                     int  *outLeftRadius,
                                     int  *outRightRadius ) {
    if( mx_sliderSpritesSet ) {
        
        int  thumbHandle  =  mx_sliderSprites.thumb[1];

        *outLeftRadius  = mx_sprites[ thumbHandle ].leftVisibleRadius;
        *outRightRadius = mx_sprites[ thumbHandle ].rightVisibleRadius;
        }
    else {
        *outRightRadius = inThumbWidth / 2;
        *outLeftRadius  = inThumbWidth / 2;
        }
    }



char maxigin_guiSlider( MaxiginGUI  *inGUI,
                        int         *inCurrentValue,
                        int          inMinValue,
                        int          inMaxValue,
                        int          inStartX,
                        int          inEndX,
                        int          inY,
                        int          inBarHeight,
                        int          inThumbHeight,
                        int          inThumbWidth,
                        char         inForceMoving  ) {

    MaxiginColor  c;
    int           thumbPixelCenter;
    int           v                       =  *inCurrentValue;
    int           fullRange               =   inMaxValue - inMinValue;
    int           tenPercent              =   fullRange / 10;
    int           thumbLeftR;
    int           thumbRightR;
    
    if( tenPercent < 1 ) {
        tenPercent = 1;
        }
    
    c.comp.red   = 255;
    c.comp.green = 255;
    c.comp.blue  = 255;
    c.comp.alpha = 255;
    
    thumbPixelCenter =
        ( ( v - inMinValue ) *
          ( inEndX - inStartX ) )
        / ( inMaxValue - inMinValue )
        + inStartX;
    
    mx_getSliderThumbRadius( inThumbWidth,
                             &thumbLeftR,
                             &thumbRightR );

    if( inGUI->active == inCurrentValue ) {
        /* previously manipulated with mouse */

        if( ! mingin_isButtonDown( MAXIGIN_MOUSE_BUTTON ) ) {

            inGUI->active = 0;
            inGUI->hot    = 0;
            }  
        }
    

    if( inGUI->active == 0 ) {
        /* no other component active */

        char avail;
        int  x;
        int  y;
            
        avail = maxigin_getPointerLocation( &x,
                                            &y );

        x -= inGUI->zeroOffsetX;
        y -= inGUI->zeroOffsetY;

        if( avail ) {
            char  over  =  0;

            if( y >= inY - inThumbHeight / 2
                &&
                y <= inY + inThumbHeight / 2 ) {
                
                if( x >= inStartX
                    &&
                    x <= inEndX ) {

                    over = 1;
                    }
                else {
                    /* mouse is not over horizontal extent of bar
                       check if mouse over thumb, which might stick
                       off the end of bar */
                    if( x >= thumbPixelCenter - thumbLeftR
                        &&
                        x <= thumbPixelCenter + thumbRightR ) {
                        
                        over = 1;
                        }
                    }
                }

            if( over ) {
                
                /* mouse over slider */

                if( ! inGUI->mouseDown ) {
                    /* mouse hovering, unpressed */
                    
                    inGUI->hot = inCurrentValue;
                    }
                }
            else if( inGUI->hot == inCurrentValue ) {
                /* mouse was over slider, but has moved out */
                inGUI->hot = 0;
                }
            }

        if( inGUI->hot == inCurrentValue ) {
            /* mouse over slider */
            if( ! inGUI->mouseDown
                &&
                mingin_isButtonDown( MAXIGIN_MOUSE_BUTTON ) ) {
                
                /* mouse newly pressed on slider */
                
                inGUI->active    = inCurrentValue;
                inGUI->mouseDown = 1;

                /* is mouse over thumb? */

                if( x <= thumbPixelCenter + thumbRightR
                    &&
                    /* extra bit seems to help on left side */
                    x >= thumbPixelCenter - thumbLeftR - 1 ) {

                    inGUI->activeMouseOffsetX = x - thumbPixelCenter;
                    }
                else {
                    /* thumb should jump to click position */
                    inGUI->activeMouseOffsetX = 0;
                    }

                inGUI->activeMouseOffsetY = 0;
                }
            }
        }
    

    if( inGUI->active == inCurrentValue ) {
        /* mouse controlling slider */

        char avail;
        int  x;
        int  y;
            
        avail = maxigin_getPointerLocation( &x,
                                            &y );

        x -= inGUI->zeroOffsetX;
        y -= inGUI->zeroOffsetY;

        if( avail ) {

            x -= inGUI->activeMouseOffsetX;
            y -= inGUI->activeMouseOffsetY;

            thumbPixelCenter = x;
            
            if( x < inStartX ) {
                v = inMinValue;
                thumbPixelCenter = inStartX;
                }
            else if( x > inEndX ) {
                v = inMaxValue;
                thumbPixelCenter = inEndX;
                }
            else {
                /* in between min and max */

                v = ( x - inStartX ) * ( inMaxValue - inMinValue )
                    /
                    ( inEndX - inStartX )
                    + inMinValue;

                if( v > inMaxValue ) {
                    v = inMaxValue;
                    }
                else if( v < inMinValue ) {
                    v = inMinValue;
                    }
                }
            } 
        }
    
    

    if( inForceMoving ) {
        inGUI->active = inCurrentValue;

        /* we're in force moving mode, which means the game has this slider
           active or selected, currently */
        
        /* listen to controller sticks, arrow keys, etc */

        if( mx_isActionFreshPressed( MAXIGIN_SLIDER_DECREASE ) ) {

            v -= tenPercent;
            }
        else if( mx_isActionFreshPressed( MAXIGIN_SLIDER_INCREASE ) ) {

            v += tenPercent;
            }
        else {

            /* move slider with sticks, potentially  */

            /* leaving these uninitialized generates
               a warning in some compilers */
            int  pos    =  0;
            int  lower  =  0;
            int  upper  =  0;
            
            if( mingin_getStickPosition( MAXIGIN_STICK_SLIDER,
                                         &pos,
                                         &lower,
                                         &upper ) ) {
                /* stick available */

                int  range           =  ( upper - lower );
                int  half            =  range / 2;
                int  mid             =  half + lower;
                int  dead            =  range / 1000;
                int  stepsPerSecond  =  mingin_getStepsPerSecond();
                int  jump            =  0;


                if( dead == 0 ) {
                    dead = 1;
                    }
                
                /* baseline:
                   on a 0-100 slider at 60 fps, we should
                   move 1 step per frame if stick partway pressed
                   and 2 steps per frame if stick all the way pressed.

                   For longer sliders, we should move faster.

                   On platforms with slower frame rates, we should move
                   faster.
                */
                
                if( pos > mid + dead ) {

                    jump = 1;

                    if( pos > upper - dead ) {
                        jump = 2;
                        }
                    }
                else if( pos < mid - dead ) {

                    jump = -1;

                    if( pos < lower + dead ) {
                        jump = -2;
                        }
                    }

                if( stepsPerSecond < 60 ) {
                    /* bigger jumps if fps lower */
                    jump = ( jump * 60 ) / stepsPerSecond;
                    }
                if( fullRange > 100 ) {
                    jump = ( jump * fullRange ) / 100;
                    }
                
                v += jump;
                }
            }
        if( v < inMinValue ) {
            v = inMinValue;
            }
        if( v > inMaxValue ) {
            v = inMaxValue;
            }
        }

    
    *inCurrentValue = v;

    if( ( inEndX - inStartX )
        >
        2 * ( inMaxValue - inMinValue ) ) {

        /* bar has way more pixels than legal values
           thumb should snap to legal values */
        
        thumbPixelCenter =
            ( ( v - inMinValue ) *
              ( inEndX - inStartX ) )
            / ( inMaxValue - inMinValue )
            + inStartX;
        }
    /* else bar has enough resolution, thumb should move
       smoothly with mouse and then snap to actual value when dropped */
    


    /* now draw slider */


    if( mx_sliderSpritesSet ) {
        /* use sprites */
        
        int  leftEndHandle;
        int  rightEndHandle;
        int  thumbHandle;
        
        if( v > inMinValue ) {
            leftEndHandle = mx_sliderSprites.left[1];
            }
        else {
            /* completely empty, show empty left end */
            leftEndHandle = mx_sliderSprites.left[0];
            }

        
        mx_guiAddSprite( inGUI,
                         0,
                         255,
                         leftEndHandle,
                         inStartX,
                         inY );

        if( thumbPixelCenter > inStartX ) {
            /* full bar to left of thumb */

            int  consumedWidth   =  0;
            int  barSprite       =  mx_sliderSprites.bar[1];
            int  barSpriteLeftR  =  mx_sprites[ barSprite ].leftVisibleRadius;
            
            int  barSpriteW      =  barSpriteLeftR +
                                    mx_sprites[ barSprite ].rightVisibleRadius;
            
            int  numBars         =  ( thumbPixelCenter - inStartX ) / barSpriteW;

            
            if( numBars > 0 ) {
                consumedWidth = numBars * barSpriteW;
                
                mx_guiAddSpriteSequence( inGUI,
                                         0,
                                         255,
                                         mx_sliderSprites.bar[1],
                                         inStartX + barSpriteLeftR,
                                         inY,
                                         barSpriteW,
                                         0,
                                         numBars );
                }

            if( consumedWidth < ( thumbPixelCenter - inStartX ) ) {

                /* fill rest with slivers */
                mx_guiAddSpriteSequence( inGUI,
                                         0,
                                         255,
                                         mx_sliderSprites.sliver[1],
                                         inStartX + consumedWidth,
                                         inY,
                                         1,
                                         0,
                                         thumbPixelCenter - inStartX
                                                          - consumedWidth );
                }
            }

        if( thumbPixelCenter < inEndX ) {
            /* empty bar to right of thumb */
            
            int  consumedWidth   =  0;
            int  barSprite       =  mx_sliderSprites.bar[0];
            int  barSpriteLeftR  =  mx_sprites[ barSprite ].leftVisibleRadius;
            
            int  barSpriteW      =  barSpriteLeftR +
                                    mx_sprites[ barSprite ].rightVisibleRadius;
            
            int  numBars         =  ( inEndX - thumbPixelCenter ) / barSpriteW;

            
            if( numBars > 0 ) {
                consumedWidth = numBars * barSpriteW;
                
                mx_guiAddSpriteSequence( inGUI,
                                         0,
                                         255,
                                         mx_sliderSprites.bar[0],
                                         thumbPixelCenter + barSpriteLeftR,
                                         inY,
                                         barSpriteW,
                                         0,
                                         numBars );
                }
            
            if( consumedWidth < ( inEndX - thumbPixelCenter ) ) {

                /* fill rest with slivers */
                mx_guiAddSpriteSequence( inGUI,
                                         0,
                                         255,
                                         mx_sliderSprites.sliver[0],
                                         thumbPixelCenter + consumedWidth,
                                         inY,
                                         1,
                                         0,
                                         inEndX - thumbPixelCenter
                                                - consumedWidth );
                }
            }
        

        if( v < inMaxValue ) {
            rightEndHandle = mx_sliderSprites.right[0];
            }
        else {
            /* completely fully, show full right end */
            rightEndHandle = mx_sliderSprites.right[1];
            }

        
        mx_guiAddSprite( inGUI,
                         0,
                         255,
                         rightEndHandle,
                         inEndX,
                         inY );

        if( inGUI->active == inCurrentValue ) {
            thumbHandle = mx_sliderSprites.thumb[2];
            }
        else if( inGUI->hot == inCurrentValue ) {
            thumbHandle = mx_sliderSprites.thumb[1];
            }
        else {
            thumbHandle = mx_sliderSprites.thumb[0];
            }

        mx_guiAddSprite( inGUI,
                         0,
                         255,
                         thumbHandle,
                         thumbPixelCenter,
                         inY );
        }
    else {
        /* no sprites set, draw directly with geometry  */


        /* bar */
        mx_makeColorGray( &c,
                          128 );
    
        mx_guiAddFillRect( inGUI,
                           0,
                           &c,
                           inStartX,
                           inY - inBarHeight / 2,
                           inEndX,
                           inY + inBarHeight / 2 );

        mx_makeColorGray( &c,
                          255 );
    
        mx_guiAddDrawRect( inGUI,
                           0,
                           &c,
                           inStartX,
                           inY - inBarHeight / 2,
                           inEndX,
                           inY + inBarHeight / 2 );


        /* thumb */


        /* shadow */
        mx_makeColorGray( &c,
                          64 );

        c.comp.alpha = 128;
    
        mx_guiAddFillRect( inGUI,
                           0,
                           &c,
                           thumbPixelCenter - inThumbWidth / 2 - 2,
                           inY - inBarHeight / 2,
                           thumbPixelCenter + inThumbWidth / 2 + 2,
                           inY + inBarHeight / 2 );

        c.comp.alpha = 255;
    
        if( inGUI->active == inCurrentValue ) {
            /* darker when moving */
            mx_makeColorGray( &c,
                              64 );
            }
        else if( inGUI->hot == inCurrentValue ) {
            /* brighter when hot */
            mx_makeColorGray( &c,
                              192 );
            }
        else {        
            mx_makeColorGray( &c,
                              128 );
            }
    
        mx_guiAddFillRect( inGUI,
                           0,
                           &c,
                           thumbPixelCenter - inThumbWidth / 2,
                           inY - inThumbHeight / 2,
                           thumbPixelCenter + inThumbWidth / 2,
                           inY + inThumbHeight / 2 );

        mx_makeColorGray( &c,
                          255 );
    
        mx_guiAddDrawRect( inGUI,
                           0,
                           &c,
                           thumbPixelCenter - inThumbWidth / 2,
                           inY - inThumbHeight / 2,
                           thumbPixelCenter + inThumbWidth / 2,
                           inY + inThumbHeight / 2 );

        /* suppress warning for now */
        if( 0 )
            mx_guiAddLine( inGUI,
                           0,
                           &c,
                           thumbPixelCenter - inThumbWidth / 2,
                           inY - inThumbHeight / 2,
                           thumbPixelCenter + inThumbWidth / 2,
                           inY + inThumbHeight / 2 );

        }
    

    /* return 1 if slider is active */
    return ( inGUI->active == inCurrentValue );
    }



int maxigin_guiStartPanel( MaxiginGUI  *inGUI,
                           int          inCenterX,
                           int          inCenterY,
                           int          inWidth,
                           int          inHeight  ) {

    /* temp fix warning */
    if( inGUI ||
        inCenterX || inCenterY || inWidth || inHeight ) {
        return -1;
        }

    return -1;
    
    }


void maxigin_guiEndPanel( MaxiginGUI  *inGUI,
                          int          inPanelHandle ) {
    if( inGUI || inPanelHandle ) {
        return;
        }
    }





/*
  Note about recording/playback:

  We actually RECORD the state of this static memory to persistent storage
  after every step (only storing a sparse map of which bytes have changed)
  and then use that to do full game playback, including smooth rewinding and
  time jumping forward and back.

  The old minorGems game engine recorded input events, the system clock, etc,
  and then let the game code replay itself, which is useful for catching bugs,
  but also fragile, and doesn't allow time-jumping, which made it too much of a
  pain to use (if someone submitted a long game with a bug a the end, I didn't
  have time to watch the whole thing).

  This system allows fast forward and rewind, and pause.

  And then you can see a bug happen (or it will just end, if there was a crash).

  Then you can rewind to the frame before the bug, and then "unleash" it
  to run on the real game code again, but stop playing back, and then
  try to trigger the bug yourself.  I.e., you get to the point right before
  the bug, and then you take the reigns by clicking and pressing buttons.

  We save space by not saving entire memory each step, but just a diff from a
  periodic full keyframe.
  
  A few things to consider:

  1. This requires two extra static buffers that are at least as big as all
     of the registered memory regions, so that we can compute a diff at
     each timestep (and keep track of the diff-summed state currently in our
     file).  We give the .h user a #define that lets them set this size, or
     turn this off on platforms where we don't want 3x memory usage.
     
  2. Diff steps will vary in size, which means we can't just seek to a known
     location in the file for accurate time-jumping.  We could write a separate
     index file in parallel that records file pos offsets of keyframes every 1
     second or whatever, and then, when the recording is done, we append this
     index to our recording, with a pointer at the end of the file that points
     back into the file at the location where this index starts.  During
     playback, we do instant time-jumping by jumping around in this
     index portion of the file, which will tell us the location of the keyframe
     to jump to in the main part of the file.

  3. Since each diff step is an XOR from the previous state, we can use it
     during playback to advance forward from the previous state, but we can
     also use it to go backwards from this state back to the previous state
     (xor can do and undo changes by applying it).  This allows reverse
     playback.

  4. Fast-forward and fast-rewind are achieved by simply playing multiple diff
     steps before updating the screen.  This seems to work great for 2x, 4x,
     8x, etc. during testing.  Of course, beyond that, instant time-jumping
     can be used.

  5. After a crash, the recording is not finalized (the index is not appended
     to the end of the recording and removed as a separate file).  On the next
     run, we do crash recovery and finalize this recording file.

  6. The game's step code is skipped completely during playback.  We assume
     that the step code is where the state is updated, and we've recorded
     the entire state, so we can update it without calling the game's step.
     However, the game's drawing code is still called to draw that changing
     state.
*/


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

static void mx_playbackJumpToFullSnapshot( int inFullSnapshotIndex );

static void mx_playbackJumpToStep( int inStepNumber );

static char mx_playbackStepForward( void );
static char mx_playbackStepBackward( void );


static void mx_gameInit( void );

static void mx_saveGame( void );

static char mx_copyIntoDataStore( int inStoreReadHandle,
                                  int inStoreWriteHandle,
                                  int inNumBytesToCopy );

static void mx_startPlayingMusic( const char  *inMusicBulkResourceName );

static void mx_stopPlayingMusic( void );



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



static  char  mx_playbackInterruptedRecording     =  0;
static  char  mx_playbackInstantReverseRecording  =  0;
static  char  mx_soundLocked                      =  0;
static  char  mx_quitting                         =  0;
static  char  mx_quittingReady                    =  0;
static  char  mx_playbackSliderActive             =  0;
static  char  mx_playbackBlockForwardSounds       =  0;


/* initiates and steps sound fade out, returning 1 when finally done
   returns 0 while still in-progress */
static char mx_stepSoundFadeOut( void );

/* maps playback speed/direction to sound speed and direction */
static void mx_setSoundSpeedAndDirection( int  inSpeed,
                                          int  inDirection );

static int mx_getMusicFilePos( void );

static void mx_setMusicFilePos( int  inPos );

static void mx_startSoundShortFadeIn( void );

static void mx_renewSoundStartFadeIn( void );

static void mx_startSoundPauseRamp( void );

static void mx_clearJustStartedSoundEffects( void );


static int mx_getNumJustStartedSoundEffects( void );
static int mx_getNumJustEndedSoundEffects( void );

/* returns sound effect handle of next, and clears it from
   list.

   returns -1 if none left */
static int mx_getNextJustStartedSoundEffect( void );
static int mx_getNextJustEndedSoundEffect( void );


static void mx_processDoneSoundEffects( void );


static void mx_endAllSoundEffectsNow( void );


/* passed-in buffers must have room for MAXIGIN_MAX_NUM_PLAYING_SOUND_EFFECTS */
static int mx_getLiveSoundEffects( int *outHandleBuffer,
                                   int *outDataPosBuffer );


static void mx_nextLang( void );


static void mx_checkLangNeedsReload( void );



void minginGame_step( char  inFinalStep ) {

    char  playbackPausedBySlider  =  0;
    
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
    if( inFinalStep ) {

        mingin_log( "Forced to quit by platform\n" );

        /* quit immediately, since this is our last step
           we can't wait for music to fade out nicely, etc. */
        mx_quitting  = 1;
        mx_quittingReady = 1;
        }

    if( ! mx_quitting
        &&
        mingin_isButtonDown( QUIT ) ) {

        mingin_log( "Got quit key, starting sound fade out\n" );
        
        mx_quitting  = 1;
        mx_quittingReady = 0;
        }

    if( mx_quitting
        &&
        ! mx_quittingReady ) {

        /* wait for sound to be done fading out */
        if( mx_stepSoundFadeOut() ) {
            mx_quittingReady = 1;
            }
        }

    if( mx_quitting
        &&
        mx_quittingReady ) {
        
        mx_saveGame();

        mx_finalizeRecording();

        mx_stopPlayingMusic();
        
        mingin_quit();

        return;
        }
        
    

    maxigin_startGUI( &mx_internalGUI );

    if( mx_isActionFreshPressed( FULLSCREEN_TOGGLE ) ) {
        mingin_toggleFullscreen( ! mingin_isFullscreen() );
        }

    if( mx_isActionFreshPressed( LANG_SWITCH ) ) {
        mx_nextLang();
        }
    

    if( mx_isActionFreshPressed( SOUND_LOCK ) ) {
        if( mx_soundLocked ) {
            mx_soundLocked = 0;
            mingin_unlockAudio();
            }
        else {
            mx_soundLocked = 1;
            mingin_lockAudio();
            }
        }
    

    
    if( mx_isActionFreshPressed( PLAYBACK_START_STOP ) ) {
        
        mx_playbackInstantReverseRecording = 0;
        
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

        int   oldPlaybackFrame;
        int   newPlaybackFrame;
        char  playbackSliderActive;
        int   oldSpeed               =  mx_playbackSpeed;
        int   oldDir                 =  mx_playbackDirection;
        char  oldPaused              =  mx_playbackPaused;
        
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

        if( oldSpeed  != mx_playbackSpeed
            ||
            oldDir    != mx_playbackDirection
            ||
            oldPaused != mx_playbackPaused ) {

            int  setSpeed  =  mx_playbackSpeed;

            if( mx_playbackPaused ) {
                setSpeed = 0;
                }
            
            mx_setSoundSpeedAndDirection( setSpeed,
                                          mx_playbackDirection );
            }


        oldPlaybackFrame = mx_playbackCurrentStep;

        newPlaybackFrame = oldPlaybackFrame;

        playbackSliderActive =
            maxigin_guiSlider( &mx_internalGUI,
                               &newPlaybackFrame,
                               0,
                               mx_playbackTotalSteps - 1,
                               20,
                               MAXIGIN_GAME_NATIVE_W - 40,
                               MAXIGIN_GAME_NATIVE_H - 30,
                               10,
                               20,
                               10,
                               0 );

        if( ! mx_playbackPaused
            &&
            playbackSliderActive ) {
            
            /* temporarily pause playback while slider moving */
            
            mx_playbackPaused = 1;
            playbackPausedBySlider = 1;
            mx_playbackSliderActive = 1;

            /* pause sound while slider moving */
            mx_setSoundSpeedAndDirection( 0,
                                          mx_playbackDirection );
            }
        

        if( mx_playbackSliderActive
            &&
            ! playbackSliderActive ) {
            /* slider was active before, not active now */

            /* unpause sound if necessary */
            int  setSpeed  =  mx_playbackSpeed;

            if( mx_playbackPaused ) {
                setSpeed = 0;
                }
            
            mx_setSoundSpeedAndDirection( setSpeed,
                                          mx_playbackDirection );

            mx_playbackSliderActive = 0;
            }
        

        if( oldPlaybackFrame != newPlaybackFrame ) {
            /* slider caused a change */

            mx_playbackJumpToStep( newPlaybackFrame );
            }
        }
    else {
        /* playback not running
           watch for "reverse playback button"
           to insta-rewind the current live game from this point back */

        if( mx_isActionFreshPressed( PLAYBACK_REVERSE ) ) {
            
            
            if( mx_recordingRunning ) {
                
                char  success  =  0;
                
                /* end the recording, copy the recording file into the
                   playback file, and then start playback from there */
                
                mx_finalizeRecording();
                mx_playbackInterruptedRecording = 1;
                mx_playbackInstantReverseRecording = 1;

                if( mingin_renamePersistData( mx_recordingDataStoreName,
                                              mx_playbackDataStoreName ) ) {
                    mingin_log( "Moved recording file into playback file "
                                "for instant playback.\n" );
                    success = 1;
                    }
                else {
                    /* simple/quick rename failed, try copying data
                       between files */
                    int   recordingLength;
                    int   recordingHandle;
                    int   playbackHandle;

                    mingin_log( "Moving recording file failed, trying "
                                "data copy into playback file instead.\n" );
                    
                    recordingHandle =
                        mingin_startReadPersistData( mx_recordingDataStoreName,
                                                     &recordingLength );

                    if( recordingHandle != -1 ) {

                        playbackHandle =
                            mingin_startWritePersistData(
                                mx_playbackDataStoreName );

                        if( playbackHandle != -1 ) {

                            success = mx_copyIntoDataStore( recordingHandle,
                                                            playbackHandle,
                                                            recordingLength );
                        
                            mingin_endWritePersistData( playbackHandle );
                            }
                        
                        mingin_endReadPersistData( recordingHandle );

                        if( success ) {
                            mingin_log(
                                "Copied recording file data into playback file "
                                "for instant playback.\n" );
                            
                            mingin_deletePersistData(
                                mx_recordingDataStoreName );
                            }
                        }
                    }
                
                if( success ) {
                    
                    /* we block sound triggers here
                       because we are initing playback from the first
                       recorded frame and then

                       fast-stepping forward to the
                       last step, and then instant-reversing from there
                       we don't want recorded sounds to trigger when
                       we do this init or fast-stepping */
                    mx_playbackBlockForwardSounds = 1;
                    
                    mx_initPlayback();

                    /* jump to last step */
                    mx_playbackJumpToStep( mx_playbackTotalSteps - 1 );

                    mx_playbackBlockForwardSounds = 0;

                    
                    mx_playbackDirection = -1;
                    mx_playbackSpeed     = 1;

                    mx_setSoundSpeedAndDirection( mx_playbackSpeed,
                                                  mx_playbackDirection );
                    }
                else {
                    /* playback failed for some reason */

                    mingin_log( "Instant-reverse playback mid-game failed\n" );

                    /* start recording again */
                    
                    mx_initRecording();
                    }
                }
            }
        }

    

    mx_checkSpritesNeedReload();
    mx_checkLangNeedsReload();
    
    mx_processDoneSoundEffects();

    if( ! mx_playbackSpeedStep() ) {

        if( mx_playbackInterruptedRecording ) {
            /* playback has ended, resume recording */
            mx_initRecording();
            mx_playbackInterruptedRecording = 0;
            mx_playbackInstantReverseRecording = 0;
            }
        
        mx_areWeInMaxiginGameStepFunction = 1;
    
        maxiginGame_step();
    
        mx_areWeInMaxiginGameStepFunction = 0;

        mx_stepRecording();

        mx_clearJustStartedSoundEffects();
        }

    /* if we jumped half-forward or half-back above
       we set this to 1.
       But those jumps waited for mx_playbackSpeedStep to actually
       apply the snapshot after the jump.
       So we need to leave mx_playbackJumping set until now, so
       that our music jumps correctly in mx_playbackSpeedStep.
    */
    mx_playbackJumping = 0;

    if( playbackPausedBySlider ) {
        /* unpause temporary playback slider pause */
        mx_playbackPaused = 0;
        }
    
    
    
    maxigin_endGUI( &mx_internalGUI );
    
    }



static  MinginButton  mx_quitMapping[] = { MGN_KEY_Q,
                                           MGN_KEY_ESCAPE,
                                           MGN_MAP_END };

static  MinginButton  mx_fullscreenMapping[] = { MGN_KEY_F,
                                                 MGN_MAP_END };

static  MinginButton  mx_langSwitchMapping[] = { MGN_KEY_L,
                                                 MGN_MAP_END };

static  MinginButton  mx_soundToggleMapping[] = { MGN_KEY_S,
                                                  MGN_MAP_END };

static  MinginButton  mx_soundLockMapping[] = { MGN_KEY_D,
                                                MGN_MAP_END };

static  MinginButton  mx_mouseButtonMapping[] = { MGN_BUTTON_MOUSE_LEFT,
                                                  MGN_MAP_END };

static  MinginButton  mx_sliderIncreaseMapping[] = { MGN_KEY_RIGHT,
                                                     MGN_BUTTON_DPAD_RIGHT,
                                                     MGN_MAP_END };

static  MinginButton  mx_sliderDecreaseMapping[] = { MGN_KEY_LEFT,
                                                     MGN_BUTTON_DPAD_LEFT,
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


static  MinginStick  mx_sliderStickMapping[] = { MGN_STICK_LEFT_X,
                                                 MGN_STICK_RIGHT_X,
                                                 MGN_MAP_END };


static void mx_clearTranslationKeys( void );

static  void mx_initLanguages( void );


static void mx_gameInit( void ) {

    int  p;
    
    mingin_registerButtonMapping( QUIT,
                                  mx_quitMapping );
    
    mingin_registerButtonMapping( FULLSCREEN_TOGGLE,
                                  mx_fullscreenMapping );
    
    mingin_registerButtonMapping( LANG_SWITCH,
                                  mx_langSwitchMapping );

    mingin_registerButtonMapping( SOUND_TOGGLE,
                                  mx_soundToggleMapping );

    mingin_registerButtonMapping( SOUND_LOCK,
                                  mx_soundLockMapping );

    mingin_registerButtonMapping( MAXIGIN_MOUSE_BUTTON,
                                  mx_mouseButtonMapping );

    mingin_registerButtonMapping( MAXIGIN_SLIDER_INCREASE,
                                  mx_sliderIncreaseMapping );
    
    mingin_registerButtonMapping( MAXIGIN_SLIDER_DECREASE,
                                  mx_sliderDecreaseMapping );

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


    mingin_registerStickAxis( MAXIGIN_STICK_SLIDER,
                              mx_sliderStickMapping );

    /* all translation keys start empty */
    mx_clearTranslationKeys();

    
    
    mx_areWeInMaxiginGameInitFunction = 1;
    
    maxigin_initGUI( &mx_internalGUI );
    
    maxiginGame_init();

    /* game set any translation keys during init, now we can load languages
       based on those keys */
    mx_initLanguages();

    /* leave init flag set until here, b/c init languages loads sprites, etc */
    mx_areWeInMaxiginGameInitFunction = 0;

    
    mx_recordingCrashRecovery();

    mx_initRecording();


    /* supress warning
       this function generally only called when debugging */
    if( 0 ) {
        mx_dumpSpriteRGBA( 0 );
        }
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



MinginButton maxigin_getPlatformPrimaryButton( int  inButtonHandle ) {
    /* push it up so it doesn't interfere with our mappings */
    inButtonHandle += LAST_MAXIGIN_USER_ACTION;

    return mingin_getPlatformPrimaryButton( inButtonHandle );
    }




char maxigin_getPointerLocation( int  *outX,
                                 int  *outY ) {
    int   rawX;
    int   rawY;
    int   maxX;
    int   maxY;
    char  avail;
    int   scaleFactor;
    int   offsetX;
    int   offsetY;
        
    avail = mingin_getPointerLocation( &rawX,
                                       &rawY,
                                       &maxX,
                                       &maxY );

    if( ! avail ) {
        return 0;
        }

    mx_computeScaling( maxX,
                       maxY,
                       & scaleFactor,
                       & offsetX,
                       & offsetY );

    rawX -= offsetX;
    rawX /= scaleFactor;

    rawY -= offsetY;
    rawY /= scaleFactor;

    *outX = rawX;
    *outY = rawY;

    return 1;
    }



char maxigin_registerStickAxis( int                inStickAxisHandle,
                                const MinginStick  inMapping[] ) {
    
    inStickAxisHandle += LAST_MAXIGIN_STICK;

    return mingin_registerStickAxis( inStickAxisHandle,
                                     inMapping );
    }



char maxigin_getStickPosition( int   inStickAxisHandle,
                               int  *outPosition,
                               int  *outLowerLimit,
                               int  *outUpperLimit ) {

    inStickAxisHandle += LAST_MAXIGIN_STICK;

    return mingin_getStickPosition( inStickAxisHandle,
                                    outPosition,
                                    outLowerLimit,
                                    outUpperLimit );
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

    if( readNum == 0
        &&
        i > 0 ) {
        /* read some chars, but last read hit EOF, back up */
        i --;
        }
    else if( readNum == 0
             &&
             i == 0 ) {
        /* never read any chars */
        return 0;
        }
    
    if( readNum == 1
        &&
        inBuffer[i] != '\0' ) {
        /* didn't find termination in data store
           because string was too long for buffer */
        mingin_log( "Error:  Buffer overflow when trying to read string from "
                    "persistent data store.\n" );
        return 0;
        }
    else if( readNum == 0
             &&
             inBuffer[i] != '\0' ) {
        /* reached end of data store without finding termination byte
           special case:  a data store that only contains one string, with
           no termination at the end (to make file editable in a text editor)
           Terminate our read string and return it */

        if( i < inMaxBytes - 1 ) {
            inBuffer[ i + 1 ] = '\0';
            return 1;
            }

        /* reached end of data store AND we reached end of buffer?
           While loop above should prevent this */
            
        mingin_log( "Error:  Reading string from persistent data store reached "
                    "unexpected case\n" );
        return 0;
        }
    else if( readNum == -1 ) {
        mingin_log( "Error:  Got read failure when trying to read string "
                    "from persistent data store.\n" );
        return 0;
        }

    return 1;
    }



/*
  Reads a \0-terminated short string (< 64 chars long) into a static buffer.

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
  Reads a whitespace-terminated short string token (< 64 chars long) into a
  static buffer.

  Skips any whitespace before the token, and consumes the first whitespace
  character after the token.

  Valid whitespace includes ' ', '\n', '\r', and '\t'

  Has 10 internal buffers that it uses, round-robin, for interleaved calls
  
  Returns \0-terminated string token with no trailing whitespace
  Returns 0 on failure.
*/
static const char *mx_readShortTokenFromBulkData( int  inBulkReadHandle ) {

    enum{         NUM_BUFFERS  =  10,
                  BUFFER_LEN   =  64  };
    int           i            =   0;
    int           readNum;
    char         *returnVal;
    char          c;
    
    static  char  buffer[NUM_BUFFERS][ BUFFER_LEN ];
    static  int   n                                   =  0;
    

    /* skip white space */
    readNum = mingin_readBulkData( inBulkReadHandle,
                                   1,
                                   (unsigned char *)&( c ) );

    while( readNum == 1
           &&
           ( c == '\0'
             ||
             c == ' '
             ||
             c == '\n'
             ||
             c == '\r'
             ||
             c == '\t' ) ) {

        readNum = mingin_readBulkData( inBulkReadHandle,
                                   1,
                                   (unsigned char *)&( c ) );
        }

    if( readNum != 1 ) {
        /* read to end without finding anything but spaces */
        return 0;
        }

    /* put first non-whitespace in our buffer */
    buffer[n][i] = c;

    i++;

    
    /* now continue filling buffer with non-whitespace tokens */
    
    readNum = mingin_readPersistData( inBulkReadHandle,
                                          1,
                                          (unsigned char *)&( buffer[n][i] ) );
    
    while( readNum == 1
           &&
           i < BUFFER_LEN - 1
           &&
           buffer[n][i] != '\0'
           &&
           buffer[n][i] != ' '
           &&
           buffer[n][i] != '\n'
           &&
           buffer[n][i] != '\r'
           &&
           buffer[n][i] != '\t' ) {
        i++;
        readNum = mingin_readPersistData( inBulkReadHandle,
                                          1,
                                          (unsigned char *)&( buffer[n][i] ) );
        }

    if( readNum == 0
        &&
        i > 0 ) {
        /* read some chars, but last read hit end of data, back up */
        i --;
        }
    else if( readNum == 0
             &&
             i == 0 ) {
        /* never read any chars */
        return 0;
        }
    
    if( readNum == 1
        &&
        buffer[n][i] != '\0'
        &&
        buffer[n][i] != ' '
        &&
        buffer[n][i] != '\n'
        &&
        buffer[n][i] != '\r'
        &&
        buffer[n][i] != '\t' ) {
        /* didn't find termination in data store
           because string was too long for buffer */
        mingin_log( "Error:  Buffer overflow when trying to read string "
                    "token from bulk data store.\n" );
        return 0;
        }
    else if( readNum == 0
             &&
             buffer[n][i] != '\0'
             &&
             buffer[n][i] != ' '
             &&
             buffer[n][i] != '\n'
             &&
             buffer[n][i] != '\r'
             &&
             buffer[n][i] != '\t' ) {
        /* reached end of data store without finding whitespace
           or termination byte
           special case:  a data store that only contains one string, with
           no whitespace at end
           Terminate our read string and return it */

        if( i < BUFFER_LEN - 1 ) {
            buffer[n][ i + 1 ] = '\0';

            returnVal = buffer[n];
            
            n++;

            if( n >= NUM_BUFFERS ) {
                n = 0;
                }
            
            return returnVal;
            }

        /* reached end of data store AND we reached end of buffer?
           While loop above should prevent this */
            
        mingin_log( "Error:  Reading string token from bulk data reached "
                    "unexpected case\n" );
        return 0;
        }
    else if( readNum == -1 ) {
        
        mingin_log( "Error:  Got read failure when trying to read string "
                    "token from bulk data.\n" );
        return 0;
        }

    /* got here, safe to terminate buffer at first whitespace character
       found */
    buffer[n][i] = '\0';

    returnVal = buffer[n];
    
    n++;

    if( n >= NUM_BUFFERS ) {
        n = 0;
        }

    return returnVal; 
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
  Reads a whitespace-delimited string representation of an int from
  a bulk resource.

  Skips any whitespace before the int, and consumes the first whitespace
  character after the int.

  Returns 1 on success, 0 on failure.
*/
static char mx_readIntTokenFromBulkData( int   inBulkReadHandle,
                                         int  *outInt ) {
    
    const char  *read  =  mx_readShortTokenFromBulkData( inBulkReadHandle );

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
static char mx_writeIntToPeristentData( int  inStoreWriteHandle,
                                        int  inInt ) {
    
    return mx_writeStringToPeristentData( inStoreWriteHandle,
                                          maxigin_intToString( inInt ) );
    }




static  unsigned char  mx_intPadding[ MAXIGIN_PADDED_INT_LENGTH ];




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


    success = mx_writeIntToPeristentData( inStoreWriteHandle,
                                          numTotalBytes );

    if( ! success ) {
        
MAXIGIN_SAVED_GAME_WRITE_FAILURE:
        
        maxigin_logString( "Failed to write to saved game data: ",
                           mx_saveGameDataStoreName );
        return 0;
        }


    success = mx_writeIntToPeristentData( inStoreWriteHandle,
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
        
        success = mx_writeIntToPeristentData( inStoreWriteHandle,
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

static  int          mx_latestRecordingIndex           =  -1;
static  int          mx_recordingDataStoreHandle       =  -1;
static  int          mx_recordingIndexDataStoreHandle  =  -1;

static  char         mx_diffRecordingEnabled           =  1;

static  int          mx_numDiffsSinceLastFullSnapshot  =  0;
static  int          mx_totalStepsRecorded             =  0;

static  const char  *mx_recordingMagicFooter           =  "MX_RECORDING";
static  char         mx_newPlaybackStarting            =  0;


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



/* returns 1 on success, 0 on error */
static char mx_recordSoundEffectsTriggers( int (*inCountFunction)( void ),
                                           int (*inGetNextFunction)( void ) ) {

    int   numSoundEffects;
    char  success;
    int   s;
    
    /* write number of sound effects */

    numSoundEffects = inCountFunction();

    success = mx_writeIntToPeristentData( mx_recordingDataStoreHandle,
                                          numSoundEffects );

    if( ! success ) {
        mingin_log( "Failed to write sound effects count "
                    "in recording\n" );
        
        return 0;
        }

    /* write handle of each just-started (or ended) sound effect, and clear it
       from list */
    s = inGetNextFunction();
    
    while( s != -1 ) {
        success = mx_writeIntToPeristentData( mx_recordingDataStoreHandle,
                                              s );

        if( ! success ) {
            mingin_log( "Failed to write sound effect handle "
                        "in recording\n" );
            return 0;
            }
        
        s = inGetNextFunction();
        }

    return 1;
    }



/* returns 1 on success, 0 on error */
static char mx_recordJustStartedSoundEffects( void ) {

    return mx_recordSoundEffectsTriggers( mx_getNumJustStartedSoundEffects,
                                          mx_getNextJustStartedSoundEffect );
    }



/* returns 1 on success, 0 on error */
static char mx_recordJustEndedSoundEffects( void ) {

    return mx_recordSoundEffectsTriggers( mx_getNumJustEndedSoundEffects,
                                          mx_getNextJustEndedSoundEffect );
    }



/* returns 1 on success, 0 on error */
static char mx_recordLiveSoundEffects( void ) {

    int   numSoundEffects;
    char  success;
    int   s;

    static  int  handles[ MAXIGIN_MAX_NUM_PLAYING_SOUND_EFFECTS ];
    static  int  dataPositions[ MAXIGIN_MAX_NUM_PLAYING_SOUND_EFFECTS ];
    
    
    /* write number of sound effects */

    numSoundEffects = mx_getLiveSoundEffects( handles,
                                              dataPositions );

    success = mx_writeIntToPeristentData( mx_recordingDataStoreHandle,
                                          numSoundEffects );

    if( ! success ) {
        mingin_log( "Failed to write live sound effects count "
                    "in recording\n" );
        
        return 0;
        }

    
    /* write handle and position of each */
    for( s = 0;
         s < numSoundEffects;
         s ++ ) {

        success = mx_writeIntToPeristentData( mx_recordingDataStoreHandle,
                                              handles[ s ] );

        if( ! success ) {
            mingin_log( "Failed to write live sound effect handle "
                        "in recording\n" );
            return 0;
            }

        success = mx_writeIntToPeristentData( mx_recordingDataStoreHandle,
                                              dataPositions[ s ] );

        if( ! success ) {
            mingin_log( "Failed to write live sound effect data position "
                        "in recording\n" );
            return 0;
            }
        }

    return 1;
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
    int   musicPos;
    
    if( startPos == -1 ) {
        mingin_log( "Failed to get current recording data store postion.\n" );

        mx_closeRecordingDataStores();
        return;
        }
    
        
    /* write the step number and starting pos of this full snapshot into our
       index data store
       use a padded int so that we can jump by 12 + 12 bytes to go
       "frame by frame" through the index.
    */
    
    success =
        mx_writePaddedIntToPerisistentData( mx_recordingIndexDataStoreHandle,
                                            mx_totalStepsRecorded );
    if( ! success ) {
        maxigin_logString(
            "Failed to write step number to recording index data: ",
            mx_recordingIndexDataStoreName );
        
        mx_closeRecordingDataStores();
        return;
        }

    
    success =
        mx_writePaddedIntToPerisistentData( mx_recordingIndexDataStoreHandle,
                                            startPos );
    if( ! success ) {
        maxigin_logString(
            "Failed to write data position to recording index data: ",
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


    /* write step number */
    success = mx_writeIntToPeristentData( mx_recordingDataStoreHandle,
                                          mx_totalStepsRecorded );

    if( ! success ) {
        mingin_log( "Failed to write memory full snapshot step number "
                    "in recording\n" );
        
        mx_closeRecordingDataStores();
        return;
        }

    
    /* write music position */
    musicPos = mx_getMusicFilePos();

    success = mx_writeIntToPeristentData( mx_recordingDataStoreHandle,
                                          musicPos );

    if( ! success ) {
        mingin_log( "Failed to write music position in recording\n" );
        
        mx_closeRecordingDataStores();
        return;
        }

    
    success = mx_recordJustStartedSoundEffects();

    if( ! success ) {
        mingin_log( "Failed to write just-started sound effects "
                    "in recording\n" );
        
        mx_closeRecordingDataStores();
        return;
        }


    success = mx_recordJustEndedSoundEffects();

    if( ! success ) {
        mingin_log( "Failed to write just-ended sound effects "
                    "in recording\n" );
        
        mx_closeRecordingDataStores();
        return;
        }

    
    success = mx_recordLiveSoundEffects();

    if( ! success ) {
        mingin_log( "Failed to write live sound effects "
                    "in recording\n" );
        
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
  inStartingdOrEnding  is  1 for playing back starting sound effects
                           0 for playing back ending sound effects
                        
  returns  1 on success
           0 on error
*/
static char mx_restoreSoundEffectsTriggers( int   inStoreReadHandle,
                                            char  inStartingOrEnding ) {

    int   readInt;
    char  success;
    int   s;
    
    success = mx_readIntFromPersistData( inStoreReadHandle,
                                         &readInt );

    if( ! success ) {
        /* failed to read num sound effects */
        return 0;
        }

    if( readInt > 0 ) {
        /* some sound effects starting */

        int  num  =  readInt;
        
        for( s = 0;
             s < num;
             s ++ ) {

            success = mx_readIntFromPersistData( inStoreReadHandle,
                                                 &readInt );
            if( ! success ) {
                /* failed to read next sound effect handle */
                return 0;
                }
            
            /* only play them if we're not paused */
            if( ! mx_playbackPaused ) {

                /* decide whether to play based on direction and
                   what kinds of sounds we're playing */
                if( inStartingOrEnding
                    &&
                    mx_playbackDirection == 1
                    &&
                    ! mx_playbackBlockForwardSounds ) {

                    /* starting sounds only if we're playing forward */
                    maxigin_playSoundEffect( readInt );
                    }
                else if( ! inStartingOrEnding
                         &&
                         mx_playbackDirection == -1 ) {

                    /* ending sounds only if we're playing backward */
                    maxigin_playSoundEffect( readInt );
                    }
                }
            }
        }

    return 1;
    }


/* plays a sound effect
   but jumps instantly to inDataPos in that sound effect if inDataPos != -1 */
static void mx_playSoundEffectWithPos( int inSoundEffectHandle,
                                       int inDataPos );



/*                      
  returns  1 on success
           0 on error
*/
static char mx_restoreLiveSoundEffects( int   inStoreReadHandle ) {

    int   readInt;
    char  success;
    int   s;
    
    success = mx_readIntFromPersistData( inStoreReadHandle,
                                         &readInt );

    if( ! success ) {
        /* failed to read num sound effects */
        return 0;
        }

    if( mx_playbackPaused
        ||
        mx_playbackJumping ) {
        
        /* when restoring live effects, we auto-clear any playing effects
           because they're probably stale, or left over from a previously-played
           snapshot (if we're walking forward in time, trying to reach
           a desired snapshot */

        /* we are paused, so this is safe
           to do without hearing cut-off glitching */
        mx_endAllSoundEffectsNow();
        }

    if( readInt > 0 ) {
        /* some sound effects live */

        int  num  =  readInt;
        
        for( s = 0;
             s < num;
             s ++ ) {

            int  handle;
            int  dataPos;
            
            success = mx_readIntFromPersistData( inStoreReadHandle,
                                                 &handle );
            if( ! success ) {
                /* failed to read next sound effect handle */
                return 0;
                }

            success = mx_readIntFromPersistData( inStoreReadHandle,
                                                 &dataPos );
            if( ! success ) {
                /* failed to read next sound effect dataPos */
                return 0;
                }
            
            /* only trigger them if we're paused */
            if( mx_playbackPaused
                ||
                mx_playbackJumping ) {
                
                mx_playSoundEffectWithPos( handle,
                                           dataPos );
                }
            }
        }

    return 1;
    }


/*
  Restores a full memory snapshot from current position in a data store.

  Returns 1 on success, 0 on failure.
*/
static char mx_restoreFromFullMemorySnapshot( int  inStoreReadHandle ) {
    
    int   r;
    int   startPos;
    char  success;
    int   readInt;
    
    if( ! mx_checkHeader( inStoreReadHandle, 'F' ) ) {
        return 0;
        }

    success = mx_readIntFromPersistData( inStoreReadHandle,
                                         &readInt );

    if( ! success ) {
        /* failed to read step number */
        return 0;
        }

    mx_playbackCurrentStep = readInt;


    success = mx_readIntFromPersistData( inStoreReadHandle,
                                         &readInt );

    if( ! success ) {
        /* failed to read music position */
        return 0;
        }

    if( readInt != -1 ) {
        /* a meaningful music position
           set it... */

        /* but only if we're paused, otherwise there is a lot
           of sample discontinuity and popping */
        if( mx_playbackPaused
            ||
            mx_newPlaybackStarting
            ||
            mx_playbackJumping ) {
            
            mx_setMusicFilePos( readInt );
            }
        }

    success = mx_restoreSoundEffectsTriggers( inStoreReadHandle,
                                              1 );

    if( ! success ) {
        /* failed to read starting sound effects */
        return 0;
        }

    
    success = mx_restoreSoundEffectsTriggers( inStoreReadHandle,
                                              0 );

    if( ! success ) {
        /* failed to read ending sound effects */
        return 0;
        }

    
    success = mx_restoreLiveSoundEffects( inStoreReadHandle );

    if( ! success ) {
        /* failed to read live sound effects */
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
    int   musicPos;
    
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

    /* write step number */
    success = mx_writeIntToPeristentData( mx_recordingDataStoreHandle,
                                          mx_totalStepsRecorded );

    if( ! success ) {
        mingin_log( "Failed to write memory diff step number in recording\n" );
        
        mx_closeRecordingDataStores();
        return;
        }

    /* write music position */
    musicPos = mx_getMusicFilePos();

    success = mx_writeIntToPeristentData( mx_recordingDataStoreHandle,
                                          musicPos );

    if( ! success ) {
        mingin_log( "Failed to write music position in recording\n" );
        
        mx_closeRecordingDataStores();
        return;
        }


    success = mx_recordJustStartedSoundEffects();

    if( ! success ) {
        mingin_log( "Failed to write just-started sound effects "
                    "in recording\n" );
        
        mx_closeRecordingDataStores();
        return;
        }


    success = mx_recordJustEndedSoundEffects();

    if( ! success ) {
        mingin_log( "Failed to write just-ended sound effects "
                    "in recording\n" );
        
        mx_closeRecordingDataStores();
        return;
        }
    
    
    success = mx_recordLiveSoundEffects();

    if( ! success ) {
        mingin_log( "Failed to write live sound effects "
                    "in recording\n" );
        
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
            success = mx_writeIntToPeristentData( mx_recordingDataStoreHandle,
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
    
    success = mx_writeIntToPeristentData( mx_recordingDataStoreHandle,
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
        /* failed to read step number */
        return 0;
        }

    mx_playbackCurrentStep = readInt;


    success = mx_readIntFromPersistData( inStoreReadHandle,
                                         &readInt );

    if( ! success ) {
        /* failed to read music position */
        return 0;
        }

    if( readInt != -1 ) {
        /* a meaningful music position
           set it... */

        /* but only if we're paused, otherwise there is a lot
           of sample discontinuity and popping */
        if( mx_playbackPaused ) {
            mx_setMusicFilePos( readInt );
            }
        }

    
    success = mx_restoreSoundEffectsTriggers( inStoreReadHandle,
                                              1 );

    if( ! success ) {
        /* failed to read starting sound effects */
        return 0;
        }

    
    success = mx_restoreSoundEffectsTriggers( inStoreReadHandle,
                                              0 );

    if( ! success ) {
        /* failed to read ending sound effects */
        return 0;
        }

    
    success = mx_restoreLiveSoundEffects( inStoreReadHandle );

    if( ! success ) {
        /* failed to read live sound effects */
        return 0;
        }

    
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
    
    mx_recordingRunning    = 0;
    mx_totalStepsRecorded  = 0;
    
    if( ! MAXIGIN_ENABLE_RECORDING ) {
        return;
        }
    if( mx_numMemRecords == 0 ) {
        return;
        }
    if( MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES <
        mx_totalMemoryRecordsBytes ) {
        
        maxigin_logInt2( "Only have room for recording ",
                         MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES,
                         " bytes, but ",
                         mx_totalMemoryRecordsBytes,
                         " registered.  Disabling recording." );
        return;
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
        mx_totalStepsRecorded ++;
        }
    else {
        /* always record a diff right before our snapshot so that
           we can play the change recorded in our snapshot backwards */
        mx_recordMemoryDiff();
        
        mx_recordFullMemorySnapshot();
        mx_totalStepsRecorded ++;
        
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

        /* next add total step count, also padded */
        success = mx_writePaddedIntToPerisistentData(
            mx_recordingDataStoreHandle,
            mx_totalStepsRecorded );

        if( ! success ) {
            mingin_log( "Failed write total step count into end "
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
    const char  *settingName      =  "maxigin_nextRecoveryNumber.ini";

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



/* returns -1 on failure */
static int mx_getMaxStepNumber( int  inRecordingReadHandle,
                                int  inStartSeekPos ) {

    char  success;
    int   readInt;
    int   maxStepNumber  =  0;
    int   curPos;
    int   r;
    
    success = mingin_seekPersistData( inRecordingReadHandle,
                                      inStartSeekPos );
    
    if( ! success ) {
        return -1;
        }

    if( ! mx_checkHeader( inRecordingReadHandle, 'F' ) ) {
        return -1;
        }

    success = mx_readIntFromPersistData( inRecordingReadHandle,
                                         &readInt );

    if( ! success ) {
        /* failed to read step number */
        return -1;
        }

    maxStepNumber = readInt;


    /* now skip over the data from this full block */
    curPos = mingin_getPersistDataPosition( inRecordingReadHandle );
    
    for( r = 0;
         r < mx_numMemRecords;
         r ++ ) {
        
        int  recSize  =  mx_memRecords[r].numBytes;

        curPos += recSize;
        }

    success = mingin_seekPersistData( inRecordingReadHandle, curPos );

    if( ! success ) {
        /* failed to seek to end of full block position */
        return -1;
        }

    
    /* walk through diff blocks until the last one
       updating maxStepNumber as we go */

    while( success ) {
        
        if( ! mx_checkHeader( inRecordingReadHandle, 'D' ) ) {
            break;
            }

        success = mx_readIntFromPersistData( inRecordingReadHandle,
                                             &readInt );

        if( ! success ) {
            /* failed to read step number */
            break;
            }

        maxStepNumber = readInt;
        

        /* now skip over the data from this diff */
        success = mx_readIntFromPersistData( inRecordingReadHandle,
                                             &readInt );
        if( ! success ) {
            /* must have at least 1 int, at least the -1 at the end,
               even if diff is empty with no changes */
            break;
            }
        
        while( readInt != -1 ) {

            /* skip the xor value */
            unsigned char  xorValue;
            int            numRead;
        
            numRead = mingin_readPersistData( inRecordingReadHandle,
                                              1,
                                              &xorValue );

            if( numRead != 1 ) {
                success = 0;
                break;
                }

            /* read index of next diff xor */
            success = mx_readIntFromPersistData( inRecordingReadHandle,
                                                 &readInt );

            if( ! success ) {
                break;
                }
            }
        }
    

    return maxStepNumber;
    }



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



void mx_recordingCrashRecovery( void ) {

    int          recordingReadHandle;
    int          indexReadHandle;
    const char  *recoveryFileName;
    int          recoveryWriteHandle;
    int          recordingLength;
    int          indexLength;
    char         success;
    int          lastFullSnapshotPos;
    int          totalSteps;

    
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
            
    if( ! success ) {
        mingin_log( "Failed to copy recording data into recovery file.\n" );

        mingin_endReadPersistData( recordingReadHandle );
        
        mingin_endReadPersistData( indexReadHandle );
        
        mingin_endWritePersistData( recoveryWriteHandle );
            
        return;
        }

    success = mx_copyIntoDataStore( indexReadHandle,
                                    recoveryWriteHandle,
                                    indexLength );
            
    if( ! success ) {
        mingin_log( "Failed to copy recording index into recovery file.\n" );

        mingin_endReadPersistData( recordingReadHandle );
        
        mingin_endWritePersistData( recoveryWriteHandle );

        mingin_endReadPersistData( indexReadHandle ); 
            
        return;
        }


    /* read last full snapshot pos from end of index */
    success = mx_seekAndReadInt( indexReadHandle,
                                 indexLength - MAXIGIN_PADDED_INT_LENGTH,
                                 &lastFullSnapshotPos );

    mingin_endReadPersistData( indexReadHandle );
    
    if( ! success ) {
        mingin_log( "Failed to seek/read last offset in recording index "
                    "during recovery\n" );
        mingin_endReadPersistData( recordingReadHandle );
        
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

        mingin_endReadPersistData( recordingReadHandle );
        
        mingin_endWritePersistData( recoveryWriteHandle );
            
        return;
        }
    

    totalSteps = mx_getMaxStepNumber( recordingReadHandle,
                                      lastFullSnapshotPos );
    
    mingin_endReadPersistData( recordingReadHandle );

    if( totalSteps == -1 ) {
        mingin_log( "Failed to determine total step count during"
                    "recording recovery.\n" );
        mingin_endWritePersistData( recoveryWriteHandle );
            
        return;
        }

    success = mx_writePaddedIntToPerisistentData(
        recoveryWriteHandle,
        totalSteps );

    if( totalSteps == -1 ) {
        mingin_log( "Failed to write total step count during"
                    "recording recovery.\n" );
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











    


static char mx_initPlayback( void ) {
    
    char   success;
    int    indexLengthDataPos;
    int    totalStepsDataPos;
    int    indexLength;
    int    magicFooterDataPos;
    char   magicFooterBuffer[ 20 ];
    int    numRead;
    
    int    firstFullSnapshotDataPos;
    int    firstFullSnapshotStepNumber;

    mx_playbackRunning      =  0;
    mx_playbackSpeed        =  1;
    mx_playbackPaused       =  0;
    mx_playbackDirection    =  1;
    mx_playbackCurrentStep  =  0;
    mx_playbackTotalSteps   =  0;
        
    if( mx_numMemRecords == 0 ) {
        return 0;
        }

    if( MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES <
        mx_totalMemoryRecordsBytes ) {
        
        maxigin_logInt2( "Only have room for playing back ",
                         MAXIGIN_RECORDING_STATIC_MEMORY_MAX_BYTES,
                         " bytes, but ",
                         mx_totalMemoryRecordsBytes,
                         " registered.  Disabling playback." );
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

    
    /* jump back more and read padded int */
    totalStepsDataPos = magicFooterDataPos - MAXIGIN_PADDED_INT_LENGTH;

    if( totalStepsDataPos < 0 ) {
        mingin_log( "Playback file too short to even contain total steps.\n" );
        
        mingin_endReadPersistData( mx_playbackDataStoreHandle );
        return 0;
        }

    
    success = mx_seekAndReadInt( mx_playbackDataStoreHandle,
                                 totalStepsDataPos,
                                 & mx_playbackTotalSteps );
    
    if( ! success ) {
        maxigin_logInt( "Failed to seek to this position and read total steps "
                        "in playback data store: ",
                        totalStepsDataPos );
        
        mingin_endReadPersistData( mx_playbackDataStoreHandle );
        return 0;
        }
    
    
    /* jump back more and read another padded int */
    indexLengthDataPos = totalStepsDataPos - MAXIGIN_PADDED_INT_LENGTH;

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
                                 &firstFullSnapshotStepNumber );

    if( ! success ) {
        maxigin_logInt( "Failed to seek to this position and read first "
                        "step number in playback data store: ",
                        mx_playbackIndexStartPos );
        
        mingin_endReadPersistData( mx_playbackDataStoreHandle );
        return 0;
        }

    if( firstFullSnapshotStepNumber != 0 ) {
        maxigin_logInt( "Unexpected first full snapshot step number in "
                        "playback data store: ",
                        firstFullSnapshotStepNumber );
        
        mingin_endReadPersistData( mx_playbackDataStoreHandle );
        return 0;
        }

    success = mx_seekAndReadInt( mx_playbackDataStoreHandle,
                                 mx_playbackIndexStartPos
                                     + MAXIGIN_PADDED_INT_LENGTH,
                                 &firstFullSnapshotDataPos);

    if( ! success ) {
        mingin_log( "Failed to read first snapshot data position"
                    "playback data store\n" );
        
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

    
    /* fade in at start of fresh playback,
       but not if we're doing instant reverse, because we start
       playing that backward, and the music is already in the right place */
    if( ! mx_playbackInstantReverseRecording ) {

        mx_startSoundPauseRamp();
        
        mx_renewSoundStartFadeIn();

        /* also don't do this if interrupting, because it will
           jump our music file position,
           which we don't need to do if immediatly reversing direction */
        
        mx_newPlaybackStarting = 1;
        }
    
    success = mx_restoreFromFullMemorySnapshot( mx_playbackDataStoreHandle );

    mx_newPlaybackStarting = 0;

    
    if( ! success ) {

        maxigin_logInt( "Failed to restore first full memory snapshot from "
                        "playback data store at this position: ",
                        firstFullSnapshotDataPos );
        return 0;
        }

    
    mx_playbackFullSnapshotLastPlayed = 0;
    mx_playbackNumFullSnapshots =
        indexLength / ( MAXIGIN_PADDED_INT_LENGTH * 2 );
    
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

    mx_setSoundSpeedAndDirection( 1,
                                  1 );
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

        if( stepsSinceLastPlaybackStep >= stepsPerPlaybackStep - 1 ) {
            success = success && mx_playbackStep();
            stepsSinceLastPlaybackStep = 0;
            }
        else {
            stepsSinceLastPlaybackStep++;
            }
        }
    
    return success;
    }



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



static int mx_getSnapshotStepNumber( int inSnapshotNumber ) {

    int   indexJumpPos  =  mx_playbackIndexStartPos +
                           MAXIGIN_PADDED_INT_LENGTH * 2 * inSnapshotNumber;
    char  success;
    int   stepNumber;
    
    success = mingin_seekPersistData( mx_playbackDataStoreHandle,
                                      indexJumpPos );

    if( ! success ) {
        maxigin_logInt( "Failed to seek into index at pos: ",
                        indexJumpPos );
        return -1;
        }

    success = mx_readIntFromPersistData( mx_playbackDataStoreHandle,
                                         &stepNumber );
    
    if( ! success ) {
        mingin_log( "Failed to read step number from index\n" );
        return -1;
        }
    return stepNumber;
    }



static void mx_playbackJumpToStep( int inStepNumber ) {

    /* find closest snapshot before or at inStepNumber */
    int  snapshotGuess  =  inStepNumber / ( mx_diffsBetweenSnapshots + 1 );
    int  snapshotStepNumber;

    snapshotStepNumber = mx_getSnapshotStepNumber( snapshotGuess );

    while( snapshotStepNumber > inStepNumber ) {
        snapshotGuess --;
        snapshotStepNumber = mx_getSnapshotStepNumber( snapshotGuess );
        }

    if( snapshotStepNumber == -1 ) {
        maxigin_logInt( "Playback jump failed find full snapshot before step: ",
                        inStepNumber );
        mx_playbackEnd();
        return;
        }

    mx_playbackJumpToFullSnapshot( snapshotGuess );

    if( ! mx_playbackRunning ) {
        /* jumping to snapshot failed */
        return;
        }

    while( mx_playbackRunning
           &&
           mx_playbackCurrentStep < inStepNumber ) {
        
        mx_playbackStepForward();
        }

    if( ! mx_playbackRunning ) {
        maxigin_logInt( "Playback failed to step forward to step number "
                        "after jumping to full snapshot: ",
                        inStepNumber );
        }
    }



static void mx_playbackJumpToFullSnapshot( int inFullSnapshotIndex ) {

    int   indexJumpPos  =  mx_playbackIndexStartPos +
                           MAXIGIN_PADDED_INT_LENGTH * 2 * inFullSnapshotIndex;
    int   readPos;
    int   stepNumber;
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
                                         &stepNumber );
    
    if( ! success ) {
        mingin_log( "Playback jump failed to read step number from index\n" );
        mx_playbackEnd();
        return;
        }

    /* now jump to read full snapshot file position */
    indexJumpPos += MAXIGIN_PADDED_INT_LENGTH;
    
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
        mx_startSoundPauseRamp();
        
        mx_startSoundShortFadeIn();

        mx_playbackJumping = 1;
        
        mx_playbackJumpToFullSnapshot(
            mx_playbackFullSnapshotLastPlayed + numToJump );
        }
    }



static void mx_playbackJumpHalfBack( void ) {

    int destToJump =
        ( mx_playbackFullSnapshotLastPlayed ) / 2;

    if( destToJump < mx_playbackFullSnapshotLastPlayed ) {
        
        mx_startSoundPauseRamp();
        
        mx_startSoundShortFadeIn();

        mx_playbackJumping = 1;
        
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



void maxigin_logInt2( const char  *inStringA,
                      int          inValB,
                      const char  *inStringC,
                      int          inValD,
                      const char  *inStringE ) {

    const char  *logString;

    logString = maxigin_stringConcat6( inStringA,
                                       maxigin_intToString( inValB ),
                                       inStringC,
                                       maxigin_intToString( inValD ),
                                       inStringE,
                                       "\n" );
    mingin_log( logString );
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

    enum{  NUM_BUFFERS  =  10,
           BUFFER_LEN   =  20 };

    static  char  buffers[ NUM_BUFFERS ][ BUFFER_LEN ];
    static  int   nextBuffer                             =  0;

    unsigned int  c            =  0;
                  /* start with billions */
    int           divisor      =  1000000000;
    const char   *formatError  =  "[int_format_error]";
                  /* skip 0 digits until our first non-zero digit */
    int           qLowerLimit  =  1;
    char         *buffer;

    buffer = buffers[ nextBuffer ];
    
    
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
            if( c >= BUFFER_LEN - 1 ) {
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

    nextBuffer++;

    if( nextBuffer >= NUM_BUFFERS ) {
        nextBuffer = 0;
        }
    
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
        
        
            i = (unsigned char)( ( i + 1 ) & 0xFF );

        
            /* k increments much more slowly, roughly 1/64 as often as i
               but because we use n to decide when k increments, this happens
               on a very chaotic schedule that doesn't seem to have a pattern */
            if( n == 13 || n == 101 || n == 173  || n == 207 ) {
                k = (unsigned char)( ( k + 1 ) & 0xFF );
                }

            /* m increments roughly 1/256 as often as i */
            if( n == 67 ) {
                m = (unsigned char)( ( m + 1 ) & 0xFF );
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
    
    int          writeHandle;
    const char  *intAsString;
    
    writeHandle = mingin_startWritePersistData( inSettingName );

    if( writeHandle == -1 ) {
        /* failed */
        return;
        }

    /* on failure, writing a setting is a no-op */

    /* special case here:
       write lone int to setting without a terminating \0
       This makes these settings files end-user editable */

    intAsString = maxigin_intToString( inValue );

    mingin_writePersistData( writeHandle,
                             maxigin_stringLength( intAsString ),
                             (unsigned char*)intAsString );

    mingin_endWritePersistData( writeHandle );
    }



const char *maxigin_stringConcat( const char  *inStringA,
                                  const char  *inStringB ) {
    
    enum{  NUM_BUFFERS  =  10,
           BUFFER_LEN   =  128 };

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



const char *maxigin_stringConcat3( const char  *inStringA,
                                   const char  *inStringB,
                                   const char  *inStringC ) {
    
    return maxigin_stringConcat( maxigin_stringConcat( inStringA,
                                                       inStringB ),
                                 inStringC );
    }



const char *maxigin_stringConcat4( const char  *inStringA,
                                   const char  *inStringB,
                                   const char  *inStringC,
                                   const char  *inStringD ) {
    
    return maxigin_stringConcat( maxigin_stringConcat3( inStringA,
                                                        inStringB,
                                                        inStringC ),
                                 inStringD );
    }



const char *maxigin_stringConcat5( const char  *inStringA,
                                   const char  *inStringB,
                                   const char  *inStringC,
                                   const char  *inStringD,
                                   const char  *inStringE ) {
    
    return maxigin_stringConcat( maxigin_stringConcat4( inStringA,
                                                        inStringB,
                                                        inStringC,
                                                        inStringD ),
                                 inStringE );
    }



const char *maxigin_stringConcat6( const char  *inStringA,
                                   const char  *inStringB,
                                   const char  *inStringC,
                                   const char  *inStringD,
                                   const char  *inStringE,
                                   const char  *inStringF ) {
    
    return maxigin_stringConcat( maxigin_stringConcat5( inStringA,
                                                        inStringB,
                                                        inStringC,
                                                        inStringD,
                                                        inStringE ),
                                 inStringF );
    }



void maxigin_stringCopy( const char  *inString,
                         char        *inDest ) {

    int  i  =  0;

    while( inString[i] != '\0' ) {
        inDest[i] = inString[i];
        i++;
        }

    inDest[i] = '\0';
    }



typedef struct MaxiginWavFormat {

        int  bulkResourceHandle;
        
        int  numChannels;
        int  sampleRate;

        /* byte location of first sample in the bulk data store */
        int  firstSampleLocation;

        int  numSampleFrames;

        /* total length of the data, including the header */
        int  totalDataLength;
        
    } MaxiginWavFormat;



static char mx_openWavData( const char        *inBulkResourceName,
                            MaxiginWavFormat  *outFormat );



static  MaxiginWavFormat  mx_musicData;
static  char              mx_musicLoaded     =     0;
static  int               mx_soundDirection  =     1;

#define  MAXIGIN_AUDIO_MIXING_NUM_SAMPLES        256
#define  MAXIGIN_WAV_READING_BYTES              1024

static  int  mx_audioMixingBuffers[2][ MAXIGIN_AUDIO_MIXING_NUM_SAMPLES ];

static  unsigned char  mx_wavReadingBuffer[ MAXIGIN_WAV_READING_BYTES ];



typedef struct MaxiginSoundEffect {

    int  numSampleFrames;

    /* first sample first byte position in mx_soundBytes */
    int  startByte;
    
    } MaxiginSoundEffect;


static  unsigned char       mx_soundBytes  [ MAXIGIN_MAX_TOTAL_SOUND_BYTES ];
static  MaxiginSoundEffect  mx_soundEffects[ MAXIGIN_MAX_NUM_SOUND_EFFECTS ];

static  int                 mx_numSoundBytes   =    0;
static  int                 mx_numSoundEffect  =    0;


int maxigin_initSoundEffect( const char  *inBulkResourceName ) {

    char                 success;
    int                  newHandle;
    MaxiginWavFormat     wavFormat;
    int                  sampleBytes;
    MaxiginSoundEffect  *effect;
    int                  numRead;
    
    if( mx_numSoundEffect >= MAXIGIN_MAX_NUM_SOUND_EFFECTS ) {
        maxigin_logString( "Failed to load sound effect because too many "
                           "already loaded: ",
                           inBulkResourceName );
        return -1;
        }
    

    success = mx_openWavData( inBulkResourceName,
                              & wavFormat );

    if( ! success ) {
        maxigin_logString( "Failed to load sound effect because parsing "
                           "WAV data failed: ",
                           inBulkResourceName );
        return -1;
        }


    if( wavFormat.numChannels != 2 ) {
        mingin_endReadBulkData( wavFormat.bulkResourceHandle );

        maxigin_logString( "Failed to load sound effect because it doesn't "
                           "have two channels: ",
                           inBulkResourceName );
        return -1;
        }

    
    /* 16 bit stereo */
    sampleBytes = wavFormat.numSampleFrames * 4;

    if( sampleBytes + mx_numSoundBytes > MAXIGIN_MAX_TOTAL_SOUND_BYTES ) {

        mingin_endReadBulkData( wavFormat.bulkResourceHandle );

        maxigin_logString( "Failed to load sound effect because it would "
                           "overflow MAXIGIN_MAX_TOTAL_SOUND_BYTES: ",
                           inBulkResourceName );
        return -1;
        }

    newHandle = mx_numSoundEffect;

    

    effect = &( mx_soundEffects[ newHandle ] );
    
    effect->numSampleFrames = wavFormat.numSampleFrames;
    effect->startByte = mx_numSoundBytes;


    /* now read all sample bytes into memory buffer */

    numRead = mingin_readBulkData( wavFormat.bulkResourceHandle,
                                   sampleBytes,
                                   &( mx_soundBytes[ effect->startByte ] ) );

    if( numRead != sampleBytes ) {
        mingin_endReadBulkData( wavFormat.bulkResourceHandle );

        maxigin_logString( "Failed to read all sample bytes from WAV "
                           "data when trying to read sound effect: ",
                           inBulkResourceName );
        return -1;
        }

    mx_numSoundEffect ++;
    mx_numSoundBytes += sampleBytes;

    
    return newHandle;
    }






typedef struct MaxiginPlayingSoundEffect {
    
        int   soundHandle;

        int   dataPos;

        char  done;
        
    } MaxiginPlayingSoundEffect;



static  MaxiginPlayingSoundEffect  mx_playingSoundEffects[
                                       MAXIGIN_MAX_NUM_PLAYING_SOUND_EFFECTS ];

static  int                        mx_numPlayingSoundEffects      =  0;


static  int                        mx_justStartedSoundEffects[
                                       MAXIGIN_MAX_NUM_PLAYING_SOUND_EFFECTS ];

static  int                        mx_numJustStartedSoundEffects  =  0;


static  int                        mx_justEndedSoundEffects[
                                       MAXIGIN_MAX_NUM_PLAYING_SOUND_EFFECTS ];

static  int                        mx_numJustEndedSoundEffects  =  0;



static void mx_endAllSoundEffectsNow( void ) {
    mingin_lockAudio();

    mx_numPlayingSoundEffects = 0;

    mingin_unlockAudio();
    }


static void mx_clearJustStartedSoundEffects( void ) {
    mx_numJustStartedSoundEffects = 0;
    }



static int mx_getNumJustStartedSoundEffects( void ) {
    return mx_numJustStartedSoundEffects;
    }


static int mx_getNumJustEndedSoundEffects( void ) {
    return mx_numJustEndedSoundEffects;
    }



static int mx_getNextJustStartedSoundEffect( void ) {

    int  returnVal;

    if( mx_numJustStartedSoundEffects == 0 ) {
        return -1;
        }

    returnVal = mx_justStartedSoundEffects[ mx_numJustStartedSoundEffects - 1 ];

    mx_numJustStartedSoundEffects--;

    return returnVal;
    }


static int mx_getNextJustEndedSoundEffect( void ) {

    int  returnVal;

    if( mx_numJustEndedSoundEffects == 0 ) {
        return -1;
        }

    returnVal = mx_justEndedSoundEffects[ mx_numJustEndedSoundEffects - 1 ];

    mx_numJustEndedSoundEffects--;

    return returnVal;
    }



static int mx_getLiveSoundEffects( int *outHandleBuffer,
                                   int *outDataPosBuffer ) {
    int  num;
    int  i;
    
    mingin_lockAudio();

    num = 0;

    for( i = 0;
         i < mx_numPlayingSoundEffects;
         i ++ ) {

        if( ! mx_playingSoundEffects[i].done ) {
            outHandleBuffer[i] = mx_playingSoundEffects[i].soundHandle;
            outDataPosBuffer[i] = mx_playingSoundEffects[i].dataPos;

            num ++;
            }
        }
    mingin_unlockAudio();

    return num;
    }



static void mx_processDoneSoundEffects( void ) {

    int  i;
    
    /* clean up any that have become done */

    mingin_lockAudio();
    
    for( i = 0;
         i < mx_numPlayingSoundEffects;
         i ++ ) {

        if( mx_playingSoundEffects[ i ].done ) {

            int  handle  =  mx_playingSoundEffects[ i ].soundHandle;
            
            /* swap last into this spot and shrink */

            if( i != mx_numPlayingSoundEffects - 1 ) {

                mx_playingSoundEffects[ i ] =
                    mx_playingSoundEffects[ mx_numPlayingSoundEffects - 1 ];
                }
             
            mx_numPlayingSoundEffects --;

            /* now log it in our just done list */
            
            if( mx_numJustEndedSoundEffects <
                MAXIGIN_MAX_NUM_PLAYING_SOUND_EFFECTS ) {
        
                mx_justEndedSoundEffects[ mx_numJustEndedSoundEffects ] =
                    handle;
                
                mx_numJustEndedSoundEffects++;
                }
            }
        }

    mingin_unlockAudio();
    }




void maxigin_playSoundEffect( int inSoundEffectHandle ) {

    mx_playSoundEffectWithPos( inSoundEffectHandle,
                               -1 );
    }



void mx_playSoundEffectWithPos( int inSoundEffectHandle,
                                int inDataPos ) {
    
    if( inSoundEffectHandle == -1 ) {
        return;
        }

    mingin_lockAudio();


    if( mx_numPlayingSoundEffects >= MAXIGIN_MAX_NUM_PLAYING_SOUND_EFFECTS ) {

        mingin_unlockAudio();

        mingin_log( "Skipping sound effect because "
                    "too many already playing.\n" );
        return;
        }


    mx_playingSoundEffects[ mx_numPlayingSoundEffects ].soundHandle =
                                                            inSoundEffectHandle;

    mx_playingSoundEffects[ mx_numPlayingSoundEffects ].dataPos =
        mx_soundEffects[ inSoundEffectHandle ].startByte;

    if( mx_soundDirection == -1 ) {
        /* play sound backward, data pos points to first byte in last
           sample frame */
        mx_playingSoundEffects[ mx_numPlayingSoundEffects ].dataPos +=
            mx_soundEffects[ inSoundEffectHandle ].numSampleFrames * 4
            - 4;
        }

    if( inDataPos != -1 ) {
        /* a data pos override */
        
        if( inDataPos < mx_soundEffects[ inSoundEffectHandle ].startByte
            ||
            inDataPos >
                mx_soundEffects[ inSoundEffectHandle ].startByte +
                mx_soundEffects[ inSoundEffectHandle ].numSampleFrames * 4
                - 4 ) {
            
            maxigin_logInt( "Skipping playing sound effect because "
                            "supplied data position is out of range: ",
                            inDataPos );
            return;
            }
        
        mx_playingSoundEffects[ mx_numPlayingSoundEffects ].dataPos = inDataPos;
        }
    

    mx_playingSoundEffects[ mx_numPlayingSoundEffects ].done = 0;

    mx_numPlayingSoundEffects ++;
    
    mingin_unlockAudio();

    if( mx_numJustStartedSoundEffects <
        MAXIGIN_MAX_NUM_PLAYING_SOUND_EFFECTS ) {
        
        mx_justStartedSoundEffects[ mx_numJustStartedSoundEffects ] =
            inSoundEffectHandle;
        mx_numJustStartedSoundEffects++;
        }
    }



static void mx_mixInOneSoundEffectSamples( int  inPlayingSoundIndex,
                                           int  inNumSampleFrames ) {
    int                  dataPos;
    int                  f;
    int                  numFramesToMix       =  inNumSampleFrames;
    MaxiginSoundEffect  *effect;
    char                 consumingLastFrame   =  0;
    char                 consumingFirstFrame  =  0;
    int                  numFramesUsed;
    int                  numFramesLeft;
    int                  dataPosJump;

        
    if( mx_playingSoundEffects[ inPlayingSoundIndex ].done ) {
        return;
        }
       
    /* f is frame index in our mixing buffer */
    f = 0;

    /* truncate if there's not enough room in our mixing buffer */
    if( numFramesToMix > MAXIGIN_AUDIO_MIXING_NUM_SAMPLES ) {
        numFramesToMix = MAXIGIN_AUDIO_MIXING_NUM_SAMPLES;
        }

    effect = &( mx_soundEffects[
                       mx_playingSoundEffects[
                           inPlayingSoundIndex ].soundHandle ] );
    

    /* when playing forward OR backward dataPos points first byte of next sample
       frame to read..
    */

    dataPos = mx_playingSoundEffects[ inPlayingSoundIndex ].dataPos;

    if( dataPos < 0 ) {
        mingin_log( "Bad data position stored in playing sound effect.\n" );

        mx_playingSoundEffects[ inPlayingSoundIndex ].done = 1;
        return;
        }

    numFramesUsed = ( dataPos - effect->startByte ) / 4 ;
    
    numFramesLeft = effect->numSampleFrames - numFramesUsed;
        

    if( mx_soundDirection == 1 ) {     
        if( numFramesToMix > numFramesLeft ) {
            numFramesToMix = numFramesLeft;
            consumingLastFrame = 1;
            }
        dataPosJump = 4;
        }
    else {
        /* backwards direction */

        int  numFramesAvail  =  numFramesUsed + 1;

        if( numFramesToMix > numFramesAvail ) {
            numFramesToMix = numFramesAvail;
            consumingFirstFrame = 1;
            }
        dataPosJump = -4;
        }
 
    while( f < numFramesToMix ) {
                            
        unsigned short  uL;
        unsigned short  uR;
                            
        uL = (unsigned short)( 
            mx_soundBytes[ dataPos     ] |
            mx_soundBytes[ dataPos + 1 ] << 8 );
                            
        uR = (unsigned short)( 
            mx_soundBytes[ dataPos + 2 ] |
            mx_soundBytes[ dataPos + 3 ] << 8 );

        dataPos += dataPosJump;
        
        mx_audioMixingBuffers[0][ f ] += (short)uL;
        mx_audioMixingBuffers[1][ f ] += (short)uR;
                            
        f++;
        }
 
    if( consumingLastFrame ) {
        /* this mixing batch consumes last frame in WAV data */

        mx_playingSoundEffects[ inPlayingSoundIndex ].done = 1;
        return;
        }
    else if( consumingFirstFrame ) {
        /* this backwards mixing batch consumes first frame */

        mx_playingSoundEffects[ inPlayingSoundIndex ].done = 1;
        return;
        }

    mx_playingSoundEffects[ inPlayingSoundIndex ].dataPos = dataPos;
    }


        

/* mix playing sound effects into mx_audioMixingBuffers
   taking music direction into account

   inNumSampleFrames should be <= MAXIGIN_AUDIO_MIXING_NUM_SAMPLES

*/
static void mx_mixInAllSoundEffectsSamples( int  inNumSampleFrames ) {

    int  i;
    
    if( mx_numPlayingSoundEffects == 0 ) {
        return;
        }

    for( i = 0;
         i < mx_numPlayingSoundEffects;
         i ++ ) {

        mx_mixInOneSoundEffectSamples( i,
                                       inNumSampleFrames );
        }
    }




/* mix music samples into mx_audioMixingBuffers
   taking music direction into account.

   inNumSampleFrames should be <= MAXIGIN_AUDIO_MIXING_NUM_SAMPLES

*/
static void mx_mixInMusicSamples( int  inNumSampleFrames ) {

    
    int  dataPos;
    int  numFramesLeft;
    int  numFramesUsed;
    int  f;
    int  numFramesToMix   =  inNumSampleFrames;
    int  numFramesMixed   =  0;
    
    /* f is frame index in our mixing buffer */
    f = 0;

    /* truncate if there's not enough room in our mixing buffer */
    if( numFramesToMix > MAXIGIN_AUDIO_MIXING_NUM_SAMPLES ) {
        numFramesToMix = MAXIGIN_AUDIO_MIXING_NUM_SAMPLES;
        }

    if( mx_soundDirection == -1 ) {
        /* f starts at end and walks backward */
        f = numFramesToMix - 1;
        }

    /* when playing forward, dataPos points to NEXT byte to read
       when playing backward, dataPos points to last byte in next block
       to read backward (last byte should be included in next backward
       read) */
    
    dataPos = mingin_getBulkDataPosition( mx_musicData.bulkResourceHandle );

    if( dataPos < 0 ) {
        mingin_log( "Getting position in WAV bulk data failed\n" );
                            
        mingin_endReadBulkData( mx_musicData.bulkResourceHandle );
        mx_musicLoaded = 0;
        return;
        }

    
    while( numFramesMixed < numFramesToMix ) {

        int   numFramesToMixThisStep  =  numFramesToMix - numFramesMixed;
        char  consumingLastFrame      =  0;
        char  consumingFirstFrame     =  0;
        int   numBytesToRead;
        int   numBytesRead            =  0;

        numFramesUsed =
            ( dataPos - mx_musicData.firstSampleLocation )
            /
            ( mx_musicData.numChannels * 2 );
    
        numFramesLeft = mx_musicData.numSampleFrames - numFramesUsed;

        

        if( mx_soundDirection == 1 ) {
            if( numFramesToMixThisStep > numFramesLeft ) {
                numFramesToMixThisStep = numFramesLeft;
                consumingLastFrame = 1;
                }
            }
        else {
            /* backwards direction */

            int  numFramesAvail  =  numFramesUsed + 1;

            if( numFramesToMixThisStep > numFramesAvail ) {
                numFramesToMixThisStep = numFramesAvail;
                consumingFirstFrame = 1;
                }
            }
    
        numBytesToRead = numFramesToMixThisStep * 4;
                    
        while( numBytesRead < numBytesToRead ) {

            int  numBytesToReadNow    =  numBytesToRead
                                           - numBytesRead;
            int  numRead;
            int  wavB                 =  0;
                        
            if( numBytesToReadNow > MAXIGIN_WAV_READING_BYTES ) {

                numBytesToReadNow = MAXIGIN_WAV_READING_BYTES;
                }

            if( mx_soundDirection == -1 ) {
                /* walk backward in file and read block of data from there */

                /* when walking backward, dataPos points to last byte
                   in our backward block */
                
                dataPos -= numBytesToReadNow - 4;
                
                if( ! mingin_seekBulkData( mx_musicData.bulkResourceHandle,
                                           dataPos ) ) {
                    mingin_log(
                        "Seeking backwards in WAV bulk data failed\n" );
                            
                    mingin_endReadBulkData( mx_musicData.bulkResourceHandle );
                    mx_musicLoaded = 0;
                            
                    return;
                    }
                }
                
            numRead =
                mingin_readBulkData(
                    mx_musicData.bulkResourceHandle,
                    numBytesToReadNow,
                    mx_wavReadingBuffer );

            if( numRead != numBytesToReadNow ) {

                mingin_log(
                    "Reading from music WAV bulk data failed\n" );
                            
                mingin_endReadBulkData( mx_musicData.bulkResourceHandle );
                mx_musicLoaded = 0;
                            
                return;
                }

            /* keep dataPos updated as we read file */
            if( mx_soundDirection == 1 ) {
                dataPos += numBytesToReadNow;
                }
            else {
                /* data pos is at start of block we just read
                   move it backward 1 frame so it points to end of next
                   backwards block */
                dataPos -= 4;
                
                /* but rewind back there now */
                if( ! mingin_seekBulkData( mx_musicData.bulkResourceHandle,
                                           dataPos ) ) {
                    mingin_log(
                        "Seeking backwards in WAV bulk data failed\n" );
                            
                    mingin_endReadBulkData( mx_musicData.bulkResourceHandle );
                    mx_musicLoaded = 0;
                            
                    return;
                    }
                }
            

            while( wavB < numBytesToReadNow ) {
                            
                unsigned short  uL;
                unsigned short  uR;
                            
                uL = (unsigned short)( 
                    mx_wavReadingBuffer[ wavB     ] |
                    mx_wavReadingBuffer[ wavB + 1 ] << 8 );

                wavB += 2;
                            
                uR = (unsigned short)( 
                    mx_wavReadingBuffer[ wavB     ] |
                    mx_wavReadingBuffer[ wavB + 1 ] << 8 );

                wavB += 2;
                            
                mx_audioMixingBuffers[0][ f ] += (short)uL;
                mx_audioMixingBuffers[1][ f ] += (short)uR;
                            
                /* next (or previous, if backward) frame of 4 bytes
                   wavB has already been advanced by 4*/
                f += mx_soundDirection;
                }
            

            numBytesRead += numBytesToReadNow;
            }

        numFramesMixed += numFramesToMixThisStep;

        if( consumingLastFrame ) {
            /* this mixing batch consumes last frame in WAV data
               rewind so it will loop from here */
            if( ! mingin_seekBulkData( mx_musicData.bulkResourceHandle,
                                       mx_musicData.firstSampleLocation ) ) {

                mingin_log(
                    "Seeking back to start of music WAV bulk data failed\n" );
                            
                mingin_endReadBulkData( mx_musicData.bulkResourceHandle );
                mx_musicLoaded = 0;
                            
                return;
                }
            dataPos = mx_musicData.firstSampleLocation;
            }
        else if( consumingFirstFrame ) {
            /* this backwards mixing batch consumes first frame
               wrap back around to end */
            dataPos =
                mx_musicData.firstSampleLocation
                + mx_musicData.numSampleFrames * 4 - 4;
            
            if( ! mingin_seekBulkData( mx_musicData.bulkResourceHandle,
                                       dataPos ) ) {
                mingin_log(
                    "Seeking to end of music WAV bulk data failed\n" );
                            
                mingin_endReadBulkData( mx_musicData.bulkResourceHandle );
                mx_musicLoaded = 0;
                            
                return;
                }
            }                                
        }
    }






    

static  int   mx_numFramesPlayedTotal   =      0;
static  int   mx_globalVolume           =      0;
static  int   mx_globalVolumeScale      =  10000;
static  int   mx_globalVolumeError      =      0;

static  int   mx_msDefaultStartFadeIn   =   5000;
static  int   mx_msDefaultShortFadeIn   =    100;
static  int   mx_msStartFadeIn          =   5000;
static  int   mx_msEndFadeOut           =    100;
static  int   mx_startFadeInDone        =      0;
static  char  mx_endFadeOutDone         =      0;
static  char  mx_endFadeOutRunning      =      0;
static  int   mx_endFadeOutStartFrame   =     -1;

/* let some extra buffers go out to sound card after we fully
   fade out to avoid interrupting audio thread and causing a pop */
static  char  mx_endFadeOutAlmostDone   =      0;
static  int   mx_buffersPostEndFadeOut  =      0;
static  int   mx_soundSpeed             =      1;


/* for smooth ramping of last sample down to 0 when
   sound suddenly pauses, to avoid pop */
static  int   mx_lastSamplesPlayed[2];
static  int   mx_lastSamplesPlayedGlobalVolume  =  0;
static  int   mx_soundPauseRampRunning          =  0;



static void mx_startSoundPauseRamp( void ) {
    mingin_lockAudio();
    
    mx_soundPauseRampRunning = 1;
    
    mingin_unlockAudio();
    }



static void mx_renewSoundStartFadeIn( void ) {
    mx_globalVolume = 0;
    mx_startFadeInDone = 0;
    mx_msStartFadeIn = mx_msDefaultStartFadeIn;
    }



static void mx_startSoundShortFadeIn( void ) {
    mx_globalVolume = 0;
    mx_startFadeInDone = 0;
    mx_msStartFadeIn = mx_msDefaultShortFadeIn;
    }



static void mx_setSoundSpeedAndDirection( int  inSpeed,
                                          int  inDirection ) {
    char  wasPaused  =  0;
    
    mingin_lockAudio();

    if( inDirection > 0 ) {
        mx_soundDirection = 1;
        }
    else {
        mx_soundDirection = -1;
        }

    if( mx_soundSpeed == 0 ) {
        wasPaused = 1;
        }
    
    mx_soundSpeed = inSpeed;

    if( wasPaused
        &&
        mx_soundSpeed != 0 ) {
        
        /* coming out of a pause */
        /* ramp up volume to hide pop */

        /* re-use start fade-in params for this, but make
           it much shorter than our startup fade-in */
        mx_globalVolume    =   0;
        mx_startFadeInDone =   0;
        mx_msStartFadeIn   = 100;
        }
    else if( ! wasPaused
             &&
             mx_soundSpeed == 0 ) {
        /* going into a pause
           avoid pop at end of audio that was paused */
        mx_soundPauseRampRunning = 1;
        }
    
    mingin_unlockAudio();
    }



static int mx_getMusicFilePos( void ) {

    int  pos;

    if( ! mingin_isSoundPlaying() ) {
        /* no sound playing  */
        return -1;
        }
    
    mingin_lockAudio();

    if( ! mx_musicLoaded ) {
        
        mingin_unlockAudio();
        
        return -1;
        }

    pos = mingin_getBulkDataPosition( mx_musicData.bulkResourceHandle );

    mingin_unlockAudio();

    return pos;
    }

    

static void mx_setMusicFilePos( int  inPos ) {

    if( inPos < 0 ) {
        return;
        }

    if( ! mingin_isSoundPlaying() ) {
        /* no sound playing  */
        return;
        }
    
    mingin_lockAudio();

    if( ! mx_musicLoaded
        ||
        inPos < mx_musicData.firstSampleLocation
        ||
        inPos > mx_musicData.firstSampleLocation
              + mx_musicData.numSampleFrames
                * 2 * mx_musicData.numChannels ) {

        /* inPos out of range */
        mingin_unlockAudio();
        return;
        }

    mingin_seekBulkData( mx_musicData.bulkResourceHandle,
                         inPos );
        
    mingin_unlockAudio();
    }



static char mx_stepSoundFadeOut( void ) {

    if( ! mingin_isSoundPlaying() ) {
        /* no sound playing, nothing to fade out, done */
        return 1;
        }
    
    mingin_lockAudio();


    if( mx_endFadeOutDone ) {

        mingin_unlockAudio();

        return 1;
        }

    if( ! mx_endFadeOutRunning ) {
        /* start fade out */

        /* stop any fade-in that is still not done */
        mx_startFadeInDone = 1;
        

        mx_endFadeOutStartFrame = mx_numFramesPlayedTotal;
        mx_endFadeOutRunning = 1;
        }

    mingin_unlockAudio();

    return 0;
    }


/* allow all sound effects PLUS music to mix without clipping */
static  int  mx_soundNormalizationFactor  = 3;


void minginGame_getAudioSamples( int             inNumSampleFrames,
                                 int             inSamplesPerSecond,
                                 unsigned char  *inSampleBuffer ) {
    int   f;
    int   b                      =  0;
    int   numBufferFramesFilled  =  0;
    int   fIncr                  =  1;
    int   fAccum                 =  0;
    int   fAccumThreshold        =  0;
    
    while( numBufferFramesFilled < inNumSampleFrames ) {

        int   numFramesToMix   =  inNumSampleFrames - numBufferFramesFilled;

        if( mx_soundSpeed > 1 ) {
            numFramesToMix *= mx_soundSpeed;
            }
        else if( mx_soundSpeed < 0 ) {
            numFramesToMix /= (- mx_soundSpeed );
            }
        
        if( numFramesToMix == 0 ) {
            /* we only need to fill a few more frames
               but our playback is in slow mode
               Get one more frame of audio to cover this case */
            numFramesToMix = 1;
            }
            


        /* zero out our mixing buffer before mixing anything into it */
        
        if( numFramesToMix > MAXIGIN_AUDIO_MIXING_NUM_SAMPLES ) {
            numFramesToMix = MAXIGIN_AUDIO_MIXING_NUM_SAMPLES;
            }

        for( f = 0;
             f < numFramesToMix;
             f ++ ) {

            mx_audioMixingBuffers[0][ f ] = 0;
            mx_audioMixingBuffers[1][ f ] = 0;
            }

        
            
        if( mx_musicLoaded
            &&
            mx_soundSpeed != 0 ) {


            mx_mixInMusicSamples( numFramesToMix );
            }


        if( mx_soundSpeed != 0 ) {

            mx_mixInAllSoundEffectsSamples( numFramesToMix );
            
            }
        
        /* now normalize to prevent clipping */
        for( f = 0;
             f < numFramesToMix;
             f ++ ) {

            mx_audioMixingBuffers[0][ f ] /= mx_soundNormalizationFactor;
            mx_audioMixingBuffers[1][ f ] /= mx_soundNormalizationFactor;
            }
        
        
        
        /* now our mixing buffer contains mixed frames
           apply any global dynamic volume fades or levels */

        if( ! mx_startFadeInDone ) {

            int  volStepAmount       =  1;
            int  samplesTotalFadeIn  =  ( inSamplesPerSecond / 1000 )
                                        * mx_msStartFadeIn;
            int  samplesPerVolStep;

            if( mx_soundSpeed > 1 ) {
                samplesTotalFadeIn *= mx_soundSpeed;
                }
            else if( mx_soundSpeed < 0 ) {
                samplesTotalFadeIn /= (- mx_soundSpeed );
                }
            
            samplesPerVolStep = samplesTotalFadeIn / mx_globalVolumeScale;
           
            if( samplesPerVolStep == 0 ) {
                /* not enough fade-in samples to cover our full
                   volume scale */

                /* maybe we need to step more than one vol step per
                   frame? */
                volStepAmount = mx_globalVolumeScale / samplesTotalFadeIn;
                }
            
            for( f = 0;
                 f < numFramesToMix;
                 f ++ ) {

                if( mx_globalVolume < mx_globalVolumeScale ) {

                    /* by ticking up a volume error, and then ticking
                       up volume periodically, we can handle many
                       volume steps for smooth fades and very long
                       fade times */
                        
                    mx_globalVolumeError += 1;

                    if( mx_globalVolumeError >= samplesPerVolStep ) {
                        mx_globalVolume += volStepAmount;
                        if( mx_globalVolume > mx_globalVolumeScale ) {
                            mx_globalVolume = mx_globalVolumeScale;
                            }
                        mx_globalVolumeError = 0;
                        }
                    
                    mx_audioMixingBuffers[0][ f ] *= mx_globalVolume;
                    mx_audioMixingBuffers[1][ f ] *= mx_globalVolume;

                    mx_audioMixingBuffers[0][ f ] /= mx_globalVolumeScale;
                    mx_audioMixingBuffers[1][ f ] /= mx_globalVolumeScale;
                    }
                else {
                    /* full volume, do nothing to our mixing buffer */
                    }
                }

            if( mx_globalVolume == mx_globalVolumeScale ) {
                    
                mx_startFadeInDone = 1;
                mx_globalVolumeError = 0;
                }
            }
        else if( mx_endFadeOutRunning
                 &&
                 ! mx_endFadeOutAlmostDone ) {

            int  volStepAmount        =  1;
            int  samplesTotalFadeOut  =  ( inSamplesPerSecond / 1000 )
                * mx_msEndFadeOut;
            int  samplesPerVolStep;
            
            if( mx_soundSpeed > 1 ) {
                samplesTotalFadeOut *= mx_soundSpeed;
                }
            else if( mx_soundSpeed < 0 ) {
                samplesTotalFadeOut /= (- mx_soundSpeed );
                }
            
            samplesPerVolStep = samplesTotalFadeOut / mx_globalVolumeScale;

            if( samplesPerVolStep == 0 ) {
                /* not enough fade-out samples to cover our full
                   volume scale */

                /* maybe we need to step more than one vol step per
                   frame? */
                volStepAmount = mx_globalVolumeScale / samplesTotalFadeOut;
                }
            
            for( f = 0;
                 f < numFramesToMix;
                 f ++ ) {

                if( mx_globalVolume > 0 ) {

                    /* by ticking up a volume error, and then ticking
                       up volume periodically, we can handle many
                       volume steps for smooth fades and very long
                       fade times */
                        
                    mx_globalVolumeError += 1;

                    if( mx_globalVolumeError >= samplesPerVolStep ) {
                        mx_globalVolume -= volStepAmount;
                        if( mx_globalVolume < 0 ) {
                            mx_globalVolume = 0;
                            }
                        mx_globalVolumeError = 0;
                        }
                    
                    mx_audioMixingBuffers[0][ f ] *= mx_globalVolume;
                    mx_audioMixingBuffers[1][ f ] *= mx_globalVolume;

                    mx_audioMixingBuffers[0][ f ] /= mx_globalVolumeScale;
                    mx_audioMixingBuffers[1][ f ] /= mx_globalVolumeScale;
                    }
                else {
                    mx_audioMixingBuffers[0][ f ] = 0;
                    mx_audioMixingBuffers[1][ f ] = 0;
                    }
                }

            if( mx_globalVolume == 0 ) {
                    
                mx_endFadeOutAlmostDone = 1;
                mx_globalVolumeError = 0;
                mx_buffersPostEndFadeOut = 0;
                }

            }
        else {
            /* just apply a static global volume to our mixing buffer */
            if( mx_globalVolume <= mx_globalVolumeScale ) {
                /*  not at max volume, there's something to adjust */
                for( f = 0;
                     f < numFramesToMix;
                     f ++ ) {
                    
                    if( mx_globalVolume > 0 ) {
                        
                        mx_audioMixingBuffers[0][ f ] *= mx_globalVolume;
                        mx_audioMixingBuffers[1][ f ] *= mx_globalVolume;

                        mx_audioMixingBuffers[0][ f ] /= mx_globalVolumeScale;
                        mx_audioMixingBuffers[1][ f ] /= mx_globalVolumeScale;
                        }
                    else {
                        mx_audioMixingBuffers[0][ f ] = 0;
                        mx_audioMixingBuffers[1][ f ] = 0;
                        }
                    }
                }
            }


        if( mx_soundPauseRampRunning ) {

            /* apply ramp across numFramesToMix
               from our last-played sample to prevent a pop
               when sound stops suddenly */

            for( f = 0;
                 f < numFramesToMix;
                 f ++ ) {

                int  rampPos  =  numFramesToMix - f;
                    
                mx_audioMixingBuffers[0][ f ] +=
                    ( mx_lastSamplesPlayedGlobalVolume *
                      ( ( rampPos * mx_lastSamplesPlayed[0] )
                        / numFramesToMix ) )
                    / mx_globalVolumeScale;
                
                mx_audioMixingBuffers[1][ f ] +=
                    ( mx_lastSamplesPlayedGlobalVolume *
                      ( ( rampPos * mx_lastSamplesPlayed[1] )
                        / numFramesToMix ) )
                    / mx_globalVolumeScale;
                }
            mx_soundPauseRampRunning = 0;
            }

        /* now convert our int mixing buffers to interleaved signed
           short samples in our audio buffer */


        /* account for speed by either skipping samples or
           repeating samples */

        
        if( mx_soundSpeed > 1 ) {
            
            /* at increased speed, we skip samples
               in our mixing buffer by incrementing f with
               bigger steps */
            
            fIncr = mx_soundSpeed;
            fAccumThreshold = 0;
            }
        else if( mx_soundSpeed < 0 ) {
            /* don't increment f in each iteration */
            fIncr = 0;

            /* instead, wait for acculator to fill and then increment f */
            fAccum = 0;
            fAccumThreshold =  - mx_soundSpeed;
            }
            
        for( f = 0;
             f < numFramesToMix
                 &&
                 numBufferFramesFilled < inNumSampleFrames;
             f += fIncr ) {

            short           left;
            short           right;
            unsigned short  uL;
            unsigned short  uR;
                        
            left   =  (short)( mx_audioMixingBuffers[0][ f ] );
            right  =  (short)( mx_audioMixingBuffers[1][ f ] );

            uL = (unsigned short)left;
            uR = (unsigned short)right;
                        

            inSampleBuffer[ b++ ] =
                (unsigned char)(  uL         & 0xFF );
            inSampleBuffer[ b++ ] =
                (unsigned char)( ( uL >> 8 ) & 0xFF );

            inSampleBuffer[ b++ ] =
                (unsigned char)(  uR         & 0xFF );
            inSampleBuffer[ b++ ] =
                (unsigned char)( ( uR >> 8 ) & 0xFF );

            numBufferFramesFilled ++;

            if( fIncr == 0 ) {

                fAccum ++;

                if( fAccum >= fAccumThreshold ) {
                    fAccum = 0;
                    f++;
                    }
                }
            }

        if( mx_soundSpeed != 0 ) {
            /* save the last sample played, in case we pause sound later */
            f = numFramesToMix - 1;
            
            mx_lastSamplesPlayed[0] = mx_audioMixingBuffers[0][ f ];
            mx_lastSamplesPlayed[1] = mx_audioMixingBuffers[1][ f ];

            mx_lastSamplesPlayedGlobalVolume = mx_globalVolume;
            }    
        
        }
    

    mx_numFramesPlayedTotal += numBufferFramesFilled;


    if( mx_endFadeOutAlmostDone ) {
        mx_buffersPostEndFadeOut ++;

        if( mx_buffersPostEndFadeOut > 5 ) {
            mx_endFadeOutDone = 1;
            }
        }
       
    }



void maxigin_initMusicLoop( const char  *inBulkResourceName ) {
    mx_startPlayingMusic( inBulkResourceName );
    }



static char mn_nextBytesEqualString( int          inBulkDataHandle,
                                     const char  *inString ) {
    unsigned char  b;
    int            i         =  0;
    int            numRead;
    char           match     =  1;

    /* keep reading number of bytes from file equal to length of inString
       so that this function will "skip" 4-byte tags in data that don't match */
    
    while( inString[i] != '\0' ) {

        numRead = mingin_readBulkData( inBulkDataHandle,
                                       1,
                                       & b );

        if( numRead != 1 ) {
            return 0;
            }
        
        if( b != inString[i] ) {
            match = 0;
            }

        i ++;
        }
    

    /* got to end of string with all bytes matching */

    return match;
    }

    

/*
  Opens wav data from a bulk resource name for reading.

  Only succeeds if WAV is S16LE PCM.

  On success, puts open wav parameters in structure pointed to by outFormat
  and returns 1.

  Returns 0 on failure.
*/
static char mx_openWavData( const char        *inBulkResourceName,
                            MaxiginWavFormat  *outFormat ) {

    int            bulkSize;
    int            bulkHandle;
    unsigned char  readBuffer[ 4 ];
    int            numRead;
    int            bytesPerBlock;
    int            bitsPerSample;
    char           foundDataChunk;
    int            chunkSize;
    int            pos;
    
    bulkHandle = mingin_startReadBulkData( inBulkResourceName,
                                           & bulkSize );

    if( bulkHandle == -1 ) {
        maxigin_logString( "Failed to open WAV bulk data for reading: ",
                           inBulkResourceName );
        return 0;
        }

    if( ! mn_nextBytesEqualString( bulkHandle,
                                   "RIFF" ) ) {

        maxigin_logString( "Failed to find RIFF header in bulk WAV data: ",
                           inBulkResourceName );
        
        mingin_endReadBulkData( bulkHandle );
        return 0;
        }
    

    /* read file size - 8 bytes, and ignore */
    numRead = mingin_readBulkData( bulkHandle,
                                   4,
                                   readBuffer );

    if( numRead != 4 ) {

        maxigin_logString( "Failed to read RIFF file size in bulk WAV data: ",
                           inBulkResourceName );
        
        mingin_endReadBulkData( bulkHandle );
        return 0;
        }

    if( ! mn_nextBytesEqualString( bulkHandle,
                                   "WAVE" ) ) {

        maxigin_logString( "Failed to find RIFF 'WAVE' tag in bulk WAV data: ",
                           inBulkResourceName );
        
        mingin_endReadBulkData( bulkHandle );
        return 0;
        }

    if( ! mn_nextBytesEqualString( bulkHandle,
                                   "fmt " ) ) {
        maxigin_logString( "Failed to find 'fmt ' chunk in bulk WAV data: ",
                           inBulkResourceName );
        
        mingin_endReadBulkData( bulkHandle );
         
        }

    /* read fmt chunk size */
    numRead = mingin_readBulkData( bulkHandle,
                                   4,
                                   readBuffer );

    if( numRead != 4 ) {

        maxigin_logString( "Failed to read fmt chunk size in bulk WAV data: ",
                           inBulkResourceName );
        
        mingin_endReadBulkData( bulkHandle );
        return 0;
        }

    if( readBuffer[0] != 16 ) {
        /* fmt chunk must have 16 bytes for PCM format */

        maxigin_logString( "fmt chunk not correct size in WAV data: ",
                           inBulkResourceName );
        
        mingin_endReadBulkData( bulkHandle );
        return 0;
        }

    /* read audio format tag */
    numRead = mingin_readBulkData( bulkHandle,
                                   2,
                                   readBuffer );

    if( numRead != 2 ) {

        maxigin_logString( "Failed to read audio format tag in bulk WAV data: ",
                           inBulkResourceName );
        
        mingin_endReadBulkData( bulkHandle );
        return 0;
        }

    if( readBuffer[0] != 1 ) {
        /* non-PCM WAV data encountered */
        
        maxigin_logString( "Only found unsupported non-PCM WAV data: ",
                           inBulkResourceName );

        mingin_endReadBulkData( bulkHandle );
        return 0;
        }


    /* read number of channels */
    numRead = mingin_readBulkData( bulkHandle,
                                   2,
                                   readBuffer );

    if( numRead != 2 ) {

        maxigin_logString( "Failed to read channel count from bulk WAV data: ",
                           inBulkResourceName );
        
        mingin_endReadBulkData( bulkHandle );
        return 0;
        }

    outFormat->numChannels = ( readBuffer[1] << 8 ) | readBuffer[0];


    /* read sample rate */
    numRead = mingin_readBulkData( bulkHandle,
                                   4,
                                   readBuffer );

    if( numRead != 4 ) {

        maxigin_logString( "Failed to read sample rate from bulk WAV data: ",
                           inBulkResourceName );
        
        mingin_endReadBulkData( bulkHandle );
        return 0;
        }

    outFormat->sampleRate =
        ( readBuffer[3] << 24 ) |
        ( readBuffer[2] << 16 ) |
        ( readBuffer[1] <<  8 ) |
        ( readBuffer[0]       );

    
    /* skip bytes per second */
    numRead = mingin_readBulkData( bulkHandle,
                                   4,
                                   readBuffer );

    if( numRead != 4 ) {

        maxigin_logString( "Failed to read bytes per sec from bulk WAV data: ",
                           inBulkResourceName );
        
        mingin_endReadBulkData( bulkHandle );
        return 0;
        }

    /* read bytes per block */
    numRead = mingin_readBulkData( bulkHandle,
                                   2,
                                   readBuffer );

    if( numRead != 2 ) {

        maxigin_logString( "Failed to read bytes per block from bulk WAV data: ",
                           inBulkResourceName );
        
        mingin_endReadBulkData( bulkHandle );
        return 0;
        }

    bytesPerBlock = ( readBuffer[1] << 8 ) | readBuffer[0];


    /* read bits per sample */
    numRead = mingin_readBulkData( bulkHandle,
                                   2,
                                   readBuffer );

    if( numRead != 2 ) {

        maxigin_logString( "Failed to read bits per sample from bulk WAV data: ",
                           inBulkResourceName );
        
        mingin_endReadBulkData( bulkHandle );
        return 0;
        }

    bitsPerSample = ( readBuffer[1] << 8 ) | readBuffer[0];
    
    if( bitsPerSample != 16 ) {
        maxigin_logString( "Only 16-bit samples supported in bulk WAV data: ",
                           inBulkResourceName );
        
        mingin_endReadBulkData( bulkHandle );
        return 0;
        }

    /* sanity check */
    if( bytesPerBlock
        !=
        ( outFormat->numChannels * bitsPerSample ) / 8 ) {

        maxigin_logString( "Bytes-per-block mismatch with bits-per-sample "
                           "in bulk WAV data: ",
                           inBulkResourceName );
        
        mingin_endReadBulkData( bulkHandle );
        return 0;
        }


    /* skip other RIFF chunks until we encounter 'data' chunk */
    foundDataChunk = 0;

    while( !foundDataChunk ) {
        
        char  result;
        
        if( mn_nextBytesEqualString( bulkHandle,
                                     "data" ) ) {
            foundDataChunk = 1;
            break;
            }

        /* read chunk size */
        numRead = mingin_readBulkData( bulkHandle,
                                       4,
                                       readBuffer );

        if( numRead != 4 ) {

            maxigin_logString( "Failed to read chunk size in bulk WAV data: ",
                               inBulkResourceName );
        
            mingin_endReadBulkData( bulkHandle );
            return 0;
            }
        
        chunkSize =
            ( readBuffer[3] << 24 ) |
            ( readBuffer[2] << 16 ) |
            ( readBuffer[1] <<  8 ) |
            ( readBuffer[0]       );

        pos = mingin_getBulkDataPosition( bulkHandle );

        if( pos == -1 ) {
            maxigin_logString( "Failed to get position in bulk WAV data: ",
                               inBulkResourceName );
        
            mingin_endReadBulkData( bulkHandle );
            return 0;
            }

        /* skip chunk */
        pos = pos + chunkSize;

        if( ( chunkSize & 1 ) == 1 ) {
            /* odd chunk size, pad with 1 extra byte according to RIFF
               chunk format */
            pos ++;
            }   

        result = mingin_seekBulkData( bulkHandle,
                                      pos );

        if( ! result ) {
            maxigin_logString( "Failed to seek in bulk WAV data: ",
                               inBulkResourceName );
        
            mingin_endReadBulkData( bulkHandle );
            return 0;
            }
        /* ready to examine next chunk */
        }

    /* finally found data chunk */

    /* read chunk size */
    numRead = mingin_readBulkData( bulkHandle,
                                   4,
                                   readBuffer );

    if( numRead != 4 ) {

        maxigin_logString( "Failed to read data chunk size in bulk WAV data: ",
                           inBulkResourceName );
        
        mingin_endReadBulkData( bulkHandle );
        return 0;
        }
        
    chunkSize =
        ( readBuffer[3] << 24 ) |
        ( readBuffer[2] << 16 ) |
        ( readBuffer[1] <<  8 ) |
        ( readBuffer[0]       );

    pos = mingin_getBulkDataPosition( bulkHandle );

    if( bulkSize - pos < chunkSize ) {
        maxigin_logString( "WAV data too short to contain full data chunk: ",
                           inBulkResourceName );
        
        mingin_endReadBulkData( bulkHandle );
        return 0;
        }

    outFormat->firstSampleLocation = pos;

    outFormat->numSampleFrames = chunkSize / ( bytesPerBlock );

    /* sanity check */

    if( outFormat->numSampleFrames * bytesPerBlock != chunkSize ) {
        maxigin_logString( "WAV data chunk does not contain a whole number "
                           "of sample frames: ",
                           inBulkResourceName );
        
        mingin_endReadBulkData( bulkHandle );
        return 0;
        }
    

    outFormat->bulkResourceHandle = bulkHandle;
    outFormat->totalDataLength = bulkSize;
    
    return 1;
    }



static void mx_startPlayingMusic( const char  *inMusicBulkResourceName ) {

    char  success;

    success = mx_openWavData( inMusicBulkResourceName,
                              & mx_musicData );

    if( success ) {

        /* half a second of stereo 16 44100 music */
        enum{  BUFFER_SIZE  =  88200  };

        static  unsigned char  buffer[ BUFFER_SIZE ];

        if( mx_musicData.numChannels != 2 ) {
            maxigin_logString( "Can only play 2-channel WAV data as music: ",
                               inMusicBulkResourceName );

            mingin_endReadBulkData( mx_musicData.bulkResourceHandle );

            mingin_lockAudio();

            mx_musicLoaded = 0;

            mingin_unlockAudio();
        
            return;
            }

        mingin_setBulkDataReadBuffer( mx_musicData.bulkResourceHandle,
                                      BUFFER_SIZE,
                                      buffer );

        mingin_lockAudio();

        mx_musicLoaded = 1;

        mingin_unlockAudio();
        }
    }


static void mx_stopPlayingMusic( void ) {
    
    char  loaded  =  0;
    
    mingin_lockAudio();

    if( mx_musicLoaded ) {
        loaded = 1;
        mx_musicLoaded = 0;
        }
    mingin_unlockAudio();

    if( loaded ) {
        mingin_endReadBulkData( mx_musicData.bulkResourceHandle );
        }
    }




#define               MAXIGIN_MAX_NUM_HINT_SPRITES     128
static  int           mx_buttonHintStripHandle      =   -1;
static  MinginButton  mx_buttonHintMapping[ MAXIGIN_MAX_NUM_HINT_SPRITES ];

void maxigin_initKeyAndButtonHintSprites( int           inSpriteStripHandle,
                                          MinginButton  inMapping[] ) {

    int  i;
    
    mx_buttonHintStripHandle = inSpriteStripHandle;
    
    i = 0;

    while( inMapping[i] != MGN_MAP_END
           &&
           /* leave room for termination, even if list full */
           i < MAXIGIN_MAX_NUM_HINT_SPRITES - 1 ) {

        mx_buttonHintMapping[i] = inMapping[i];

        i ++;
        }

    if( inMapping[i] != MGN_MAP_END ) {
        maxigin_logInt(
            "inMapping passed into maxigin_initKeyAndButtonHintSprites "
            "has too many buttons in it.  Max is ",
            MAXIGIN_MAX_NUM_HINT_SPRITES );
        }
    
    /* terminate list */
    mx_buttonHintMapping[i] = MGN_MAP_END;

    if( maxigin_getNumSpritesInStrip( mx_buttonHintStripHandle )
        < i ) {

        /* if we have more sprites than mapping slots, that's okay */
        maxigin_logInt2(
            "inMapping contains ",
            i,
            "elements, but sprite strip contains ",
            maxigin_getNumSpritesInStrip( mx_buttonHintStripHandle ),
            "elements in maxigin_initKeyAndButtonHintSprites" );
        
        mx_buttonHintStripHandle = -1;
        }
    }


static  char         mx_buttonToNameMapReady                 =  0;
static  const char  *mx_buttonToNameMap[ MGN_NUM_BUTTONS ];

static void mx_setupButtonToNameMap( void ) {

    int  i;

    for( i = 0;
         i < MGN_NUM_BUTTONS;
         i ++ ) {
        mx_buttonToNameMap[ i ] = "";
        }
    
    mx_buttonToNameMap[ MGN_KEY_BACKSPACE ]     =  "Backspace";
    mx_buttonToNameMap[ MGN_KEY_TAB ]           =  "Tab";
    mx_buttonToNameMap[ MGN_KEY_RETURN ]        =  "Return";
    mx_buttonToNameMap[ MGN_KEY_ESCAPE ]        =  "Escape";
    mx_buttonToNameMap[ MGN_KEY_DELETE ]        =  "Delete";
    mx_buttonToNameMap[ MGN_KEY_HOME ]          =  "Home";
    mx_buttonToNameMap[ MGN_KEY_LEFT ]          =  "Left";
    mx_buttonToNameMap[ MGN_KEY_UP ]            =  "Up";
    mx_buttonToNameMap[ MGN_KEY_RIGHT ]         =  "Right";
    mx_buttonToNameMap[ MGN_KEY_DOWN ]          =  "Down";
    mx_buttonToNameMap[ MGN_KEY_PAGE_UP ]       =  "Page Up";
    mx_buttonToNameMap[ MGN_KEY_PAGE_DOWN ]     =  "Page Down";
    mx_buttonToNameMap[ MGN_KEY_END ]           =  "End";
    mx_buttonToNameMap[ MGN_KEY_NUM_LOCK ]      =  "Num Lock";
    mx_buttonToNameMap[ MGN_KEY_F1 ]            =  "F1";
    mx_buttonToNameMap[ MGN_KEY_F2 ]            =  "F2";
    mx_buttonToNameMap[ MGN_KEY_F3 ]            =  "F3";
    mx_buttonToNameMap[ MGN_KEY_F4 ]            =  "F4";
    mx_buttonToNameMap[ MGN_KEY_F5 ]            =  "F5";
    mx_buttonToNameMap[ MGN_KEY_F6 ]            =  "F6";
    mx_buttonToNameMap[ MGN_KEY_F7 ]            =  "F7";
    mx_buttonToNameMap[ MGN_KEY_F8 ]            =  "F8";
    mx_buttonToNameMap[ MGN_KEY_F9 ]            =  "F9";
    mx_buttonToNameMap[ MGN_KEY_F10 ]           =  "F10";
    mx_buttonToNameMap[ MGN_KEY_F11 ]           =  "F11";
    mx_buttonToNameMap[ MGN_KEY_F12 ]           =  "F12";
    mx_buttonToNameMap[ MGN_KEY_F13 ]           =  "F13";
    mx_buttonToNameMap[ MGN_KEY_F14 ]           =  "F14";
    mx_buttonToNameMap[ MGN_KEY_F15 ]           =  "F15";
    mx_buttonToNameMap[ MGN_KEY_F16 ]           =  "F16";
    mx_buttonToNameMap[ MGN_KEY_F17 ]           =  "F17";
    mx_buttonToNameMap[ MGN_KEY_F18 ]           =  "F18";
    mx_buttonToNameMap[ MGN_KEY_F19 ]           =  "F19";
    mx_buttonToNameMap[ MGN_KEY_F20 ]           =  "F20";
    mx_buttonToNameMap[ MGN_KEY_F21 ]           =  "F21";
    mx_buttonToNameMap[ MGN_KEY_F22 ]           =  "F22";
    mx_buttonToNameMap[ MGN_KEY_F23 ]           =  "F23";
    mx_buttonToNameMap[ MGN_KEY_F24 ]           =  "F24";
    mx_buttonToNameMap[ MGN_KEY_F25 ]           =  "F25";
    mx_buttonToNameMap[ MGN_KEY_F26 ]           =  "F26";
    mx_buttonToNameMap[ MGN_KEY_F27 ]           =  "F27";
    mx_buttonToNameMap[ MGN_KEY_F28 ]           =  "F28";
    mx_buttonToNameMap[ MGN_KEY_F29 ]           =  "F29";
    mx_buttonToNameMap[ MGN_KEY_F30 ]           =  "F30";
    mx_buttonToNameMap[ MGN_KEY_F31 ]           =  "F31";
    mx_buttonToNameMap[ MGN_KEY_F32 ]           =  "F32";
    mx_buttonToNameMap[ MGN_KEY_F33 ]           =  "F33";
    mx_buttonToNameMap[ MGN_KEY_F34 ]           =  "F34";
    mx_buttonToNameMap[ MGN_KEY_F35 ]           =  "F35";
    mx_buttonToNameMap[ MGN_KEY_SHIFT_L ]       =  "Shift L";
    mx_buttonToNameMap[ MGN_KEY_SHIFT_R ]       =  "Shift R";
    mx_buttonToNameMap[ MGN_KEY_CONTROL_L ]     =  "Control L";
    mx_buttonToNameMap[ MGN_KEY_CONTROL_R ]     =  "Control R";
    mx_buttonToNameMap[ MGN_KEY_CAPS_LOCK ]     =  "Caps Lock";
    mx_buttonToNameMap[ MGN_KEY_META_L ]        =  "Meta L";
    mx_buttonToNameMap[ MGN_KEY_META_R ]        =  "Meta R";
    mx_buttonToNameMap[ MGN_KEY_ALT_L ]         =  "Alt L";
    mx_buttonToNameMap[ MGN_KEY_ALT_R ]         =  "Alt R";
    mx_buttonToNameMap[ MGN_KEY_SUPER_L ]       =  "Super L";
    mx_buttonToNameMap[ MGN_KEY_SUPER_R ]       =  "Super R";
    mx_buttonToNameMap[ MGN_KEY_SPACE ]         =  "Space";

    mx_buttonToNameMap[ MGN_KEY_EXCLAMATION ]   =  "!";
    mx_buttonToNameMap[ MGN_KEY_DOUBLE_QUOTE ]  =  "\"";
    mx_buttonToNameMap[ MGN_KEY_NUMBER_SIGN ]   =  "#";
    mx_buttonToNameMap[ MGN_KEY_DOLLAR ]        =  "$";
    mx_buttonToNameMap[ MGN_KEY_PERCENT ]       =  "%";
    mx_buttonToNameMap[ MGN_KEY_AMPERSAND ]     =  "&";
    mx_buttonToNameMap[ MGN_KEY_APOSTROPHE ]    =  "'";
    mx_buttonToNameMap[ MGN_KEY_PAREN_L ]       =  "(";
    mx_buttonToNameMap[ MGN_KEY_PAREN_R ]       =  ")";
    mx_buttonToNameMap[ MGN_KEY_ASTERISK ]      =  "*";
    mx_buttonToNameMap[ MGN_KEY_PLUS ]          =  "+";
    mx_buttonToNameMap[ MGN_KEY_COMMA ]         =  ",";
    mx_buttonToNameMap[ MGN_KEY_MINUS ]         =  "-";
    mx_buttonToNameMap[ MGN_KEY_PERIOD ]        =  ".";
    mx_buttonToNameMap[ MGN_KEY_SLASH ]         =  "/";
    mx_buttonToNameMap[ MGN_KEY_0 ]             =  "0";
    mx_buttonToNameMap[ MGN_KEY_1 ]             =  "1";
    mx_buttonToNameMap[ MGN_KEY_2 ]             =  "2";
    mx_buttonToNameMap[ MGN_KEY_3 ]             =  "3";
    mx_buttonToNameMap[ MGN_KEY_4 ]             =  "4";
    mx_buttonToNameMap[ MGN_KEY_5 ]             =  "5";
    mx_buttonToNameMap[ MGN_KEY_6 ]             =  "6";
    mx_buttonToNameMap[ MGN_KEY_7 ]             =  "7";
    mx_buttonToNameMap[ MGN_KEY_8 ]             =  "8";
    mx_buttonToNameMap[ MGN_KEY_9 ]             =  "9";
    mx_buttonToNameMap[ MGN_KEY_COLON ]         =  ":";
    mx_buttonToNameMap[ MGN_KEY_SEMICOLON ]     =  ";";
    mx_buttonToNameMap[ MGN_KEY_LESS ]          =  "<";
    mx_buttonToNameMap[ MGN_KEY_EQUAL ]         =  "=";
    mx_buttonToNameMap[ MGN_KEY_GREATER ]       =  ">";
    mx_buttonToNameMap[ MGN_KEY_QUESTION ]      =  "?";
    mx_buttonToNameMap[ MGN_KEY_AT_SIGN ]       =  "@";
    mx_buttonToNameMap[ MGN_KEY_BRACKET_L ]     =  "[";
    mx_buttonToNameMap[ MGN_KEY_BACKSLASH ]     =  "\\";
    mx_buttonToNameMap[ MGN_KEY_BRACKET_R ]     =  "]";
    mx_buttonToNameMap[ MGN_KEY_CIRCUMFLEX ]    =  "^";
    mx_buttonToNameMap[ MGN_KEY_UNDERSCORE ]    =  "_";
    mx_buttonToNameMap[ MGN_KEY_BACK_TICK ]     =  "`";
    mx_buttonToNameMap[ MGN_KEY_A ]             =  "A";
    mx_buttonToNameMap[ MGN_KEY_B ]             =  "B";
    mx_buttonToNameMap[ MGN_KEY_C ]             =  "C";
    mx_buttonToNameMap[ MGN_KEY_D ]             =  "D";
    mx_buttonToNameMap[ MGN_KEY_E ]             =  "E";
    mx_buttonToNameMap[ MGN_KEY_F ]             =  "F";
    mx_buttonToNameMap[ MGN_KEY_G ]             =  "G";
    mx_buttonToNameMap[ MGN_KEY_H ]             =  "H";
    mx_buttonToNameMap[ MGN_KEY_I ]             =  "I";
    mx_buttonToNameMap[ MGN_KEY_J ]             =  "J";
    mx_buttonToNameMap[ MGN_KEY_K ]             =  "K";
    mx_buttonToNameMap[ MGN_KEY_L ]             =  "L";
    mx_buttonToNameMap[ MGN_KEY_M ]             =  "M";
    mx_buttonToNameMap[ MGN_KEY_N ]             =  "N";
    mx_buttonToNameMap[ MGN_KEY_O ]             =  "O";
    mx_buttonToNameMap[ MGN_KEY_P ]             =  "P";
    mx_buttonToNameMap[ MGN_KEY_Q ]             =  "Q";
    mx_buttonToNameMap[ MGN_KEY_R ]             =  "R";
    mx_buttonToNameMap[ MGN_KEY_S ]             =  "S";
    mx_buttonToNameMap[ MGN_KEY_T ]             =  "T";
    mx_buttonToNameMap[ MGN_KEY_U ]             =  "U";
    mx_buttonToNameMap[ MGN_KEY_V ]             =  "V";
    mx_buttonToNameMap[ MGN_KEY_W ]             =  "W";
    mx_buttonToNameMap[ MGN_KEY_X ]             =  "X";
    mx_buttonToNameMap[ MGN_KEY_Y ]             =  "Y";
    mx_buttonToNameMap[ MGN_KEY_Z ]             =  "Z";
    mx_buttonToNameMap[ MGN_KEY_BRACE_L ]       =  "{";
    mx_buttonToNameMap[ MGN_KEY_VERTICAL_BAR ]  =  "|";
    mx_buttonToNameMap[ MGN_KEY_BRACE_R ]       =  "}";
    mx_buttonToNameMap[ MGN_KEY_TILDE ]         =  "~";
    
    mx_buttonToNameMapReady = 1;
    }



void maxigin_drawButtonHintSprite( int  inButtonHandle,
                                   int  inCenterX,
                                   int  inCenterY ) {

    MinginButton  primaryButton;
    int           spriteHandle    =  -1;
    int           i;
    const char   *spelledOut      =   0;
    
    if( mx_buttonHintStripHandle == -1 ) {
        return;
        }

    if( ! mx_buttonToNameMapReady ) {
        mx_setupButtonToNameMap();
        }

    /* push out beyond maxigin internal button mappings */
    inButtonHandle += LAST_MAXIGIN_USER_ACTION;
    
    
    primaryButton = mingin_getPlatformPrimaryButton( inButtonHandle );

    i = 0;

    while( mx_buttonHintMapping[i] != primaryButton
           &&
           mx_buttonHintMapping[i] != MGN_MAP_END ) {
        
        i ++;
        }

    if( mx_buttonHintMapping[i] == primaryButton ) {
        /* found it */

        spriteHandle = maxigin_getSpriteFromStrip( mx_buttonHintStripHandle,
                                                   i );
        }

    if( spriteHandle != -1 ) {

        maxigin_drawSprite( spriteHandle,
                            inCenterX,
                            inCenterY );
        return;
        }
    
    if( primaryButton >= MGN_FIRST_PRINTABLE_KEY
        &&
        primaryButton <= MGN_LAST_PRINTABLE_KEY ) {

        i = 0;

        while( mx_buttonHintMapping[i] != MGN_ANY_KEY
               &&
               mx_buttonHintMapping[i] != MGN_MAP_END ) {
        
            i ++;
            }

        if( mx_buttonHintMapping[i] == MGN_ANY_KEY ) {
            /* found the blank key cap sprite */

            spriteHandle = maxigin_getSpriteFromStrip( mx_buttonHintStripHandle,
                                                       i );

            maxigin_drawSprite( spriteHandle,
                                inCenterX,
                                inCenterY );

            /* fixme:  use font to draw printable character on key cap */
            return;
            }
        }

    spelledOut = mx_buttonToNameMap[ primaryButton ];

    if( spelledOut[0] != '\0' ) {
        /* we have a name for this key */

        /* fixme:  draw the name with a font */

        }
    
    }



typedef struct MaxiginCharacterPair {

        /* will be 0 as a marker for an empty hash table entry */
        unsigned long  codePoint;

        int            spriteHandle;
        
    } MaxiginCharacterPair;


/* hash table has room for loading at 50%
   each font uses a sub-section of this, depending on how many characters
   it needs to support */
#define  MAXIGIN_FONT_HASH_TABLE_SIZE  ( MAXIGIN_MAX_TOTAL_FONT_CHARACTERS * 2 )

static  MaxiginCharacterPair  mx_fontHashTable[ MAXIGIN_FONT_HASH_TABLE_SIZE ];

static  int                   mx_numCharHashEntries      =  0;


/*
  each character has an index i into this table, for pixel row r:

   [ i ][ 0 ][ r ]   is the leftmost non-transparent pixel for row r
   [ i ][ 1 ][ r ]   is the rightmost non-transparent pixel for row r
*/
static  int                   mx_fontKerningTable
                                  [ MAXIGIN_MAX_TOTAL_FONT_CHARACTERS ]
                                  [2]
                                  [ MAXIGIN_MAX_FONT_SPRITE_HEIGHT ];

static  int                   mx_numKerningTableEntries  =  0;



typedef struct MaxiginFont {

        /* maps one-byte UTF-8 characters directly to sprite handles,
           or -1 if that one-byte character is not supported */
        int            oneByteMap[128];

        /* index to first position in mx_fontHashTable */
        int            hashTableStart;

        /* a power of 2 bigger than 2x character count */
        int            hashTableNumEntries;

        /* bitmask to apply to 32-bit code points to map
           them into hash table */
        unsigned long  hashMask;

        
        int            spacing;
        int            spaceWidth;
        int            fixedWidth;
        
    } MaxiginFont;



static int mx_getHashTableSize( int  inNumCharactes ) {

    int  powerOfTwo  =  1;
    int  target      =  inNumCharactes * 2;

    while( powerOfTwo < target ) {
        powerOfTwo *= 2;
        }
    
    return powerOfTwo;
    }



static  MaxiginFont  mx_fonts[ MAXIGIN_MAX_NUM_FONTS ];
static  int          mx_numFonts                         =  0;



/* auto-skips any space characters (0x20) before next code point
   returns -1 on failure */
static long mx_readNextCodePoint( int  inBulkResourceHandle ) {

    unsigned char  c0;
    unsigned char  c1;
    unsigned char  c2;
    unsigned char  c3;

    c0 = 0x20;

    /* keep reading until we get past any white space characters */

    while( c0 == 0x20
           ||
           c0 == '\n'
           ||
           c0 == '\r'
           ||
           c0 == '\t' ) {
        
        if( 1 != mingin_readBulkData( inBulkResourceHandle,
                                      1,
                                      &c0 ) ) {
            return -1;
            }
        }
    

    if( c0 < 128 ) {
        /* single byte case */
        return (long)( c0 );
        }
    else if( ( c0 & 0xE0 ) == 0xC0 ) {
        /* first byte is 110xxxxx
           2-byte case */
        if( 1 != mingin_readBulkData( inBulkResourceHandle,
                                      1,
                                      &c1 ) ) {
            return -1;
            }
        if( ( c1 & 0xC0 ) != 0x80 ) {
            /* invalid continuation byte */
            return -1;
            }
        /* take 5 bits from first byte, and 6 bits from continuation */
        return (long)( ( c0 & 0x1F ) << 6
                       |
                       ( c1 & 0x3F ) );
        }
    else if( ( c0 & 0xF0 ) == 0xE0 ) {
        /* first byte is 1110xxxx
           3-byte case */
        if( 1 != mingin_readBulkData( inBulkResourceHandle,
                                      1,
                                      &c1 ) ) {
            return -1;
            }
        if( ( c1 & 0xC0 ) != 0x80 ) {
            /* invalid continuation byte */
            return -1;
            }
        if( 1 != mingin_readBulkData( inBulkResourceHandle,
                                      1,
                                      &c2 ) ) {
            return -1;
            }
        if( ( c2 & 0xC0 ) != 0x80 ) {
            /* invalid continuation byte */
            return -1;
            }
        /* take 4 bits from first byte, and 6 bits from each continuation */
        return (long)( ( c0 & 0x0F ) << 12
                       |
                       ( c1 & 0x3F ) << 6
                       |
                       ( c2 & 0x3F ) );
        }
    else if( ( c0 & 0xF8 ) == 0xF0 ) {
        /* first byte is 11110xxx
           4-byte case */
        if( 1 != mingin_readBulkData( inBulkResourceHandle,
                                      1,
                                      &c1 ) ) {
            return -1;
            }
        if( ( c1 & 0xC0 ) != 0x80 ) {
            /* invalid continuation byte */
            return -1;
            }
        if( 1 != mingin_readBulkData( inBulkResourceHandle,
                                      1,
                                      &c2 ) ) {
            return -1;
            }
        if( ( c2 & 0xC0 ) != 0x80 ) {
            /* invalid continuation byte */
            return -1;
            }
        if( 1 != mingin_readBulkData( inBulkResourceHandle,
                                      1,
                                      &c3 ) ) {
            return -1;
            }
        if( ( c3 & 0xC0 ) != 0x80 ) {
            /* invalid continuation byte */
            return -1;
            }
        /* take 3 bits from first byte, and 6 bits from each continuation */
        return (long)( ( c0 & 0x07 ) << 18
                       |
                       ( c1 & 0x3F ) << 12
                       |
                       ( c2 & 0x3F ) << 6
                       |
                       ( c3 & 0x3F ) );
        }
    else {
        /* we should never get here */
        return -1;
        }
    }




/* outPoint set to the scanned code point, or -1 on failure

   returns pointer to next byte after bytes used up by scan */
static char *mx_scanNextCodePoint( char  *inBuffer,
                                   long  *outPoint ) {

    unsigned char  c0;
    unsigned char  c1;
    unsigned char  c2;
    unsigned char  c3;

    c0 = (unsigned char)( inBuffer[ 0 ] );

    if( c0 == '\0' ) {
        *outPoint = -1;
        return inBuffer;
        }
    

    if( c0 < 128 ) {
        /* single byte case */
        *outPoint = (long)( c0 );
        return &( inBuffer[ 1 ] );
        }
    else if( ( c0 & 0xE0 ) == 0xC0 ) {
        /* first byte is 110xxxxx
           2-byte case */

        c1 = (unsigned char)( inBuffer[ 1 ] );

        if( c1 == '\0' ) {
            *outPoint = -1;
            return inBuffer;
            }

        if( ( c1 & 0xC0 ) != 0x80 ) {
            /* invalid continuation byte */
            *outPoint = -1;
            return inBuffer;
            }
        
        /* take 5 bits from first byte, and 6 bits from continuation */
        *outPoint = (long)( ( c0 & 0x1F ) << 6
                            |
                            ( c1 & 0x3F ) );
        
        return &( inBuffer[ 2 ] );
        }
    else if( ( c0 & 0xF0 ) == 0xE0 ) {
        /* first byte is 1110xxxx
           3-byte case */

        c1 = (unsigned char)( inBuffer[ 1 ] );

        if( c1 == '\0' ) {
            *outPoint = -1;
            return inBuffer;
            }

        if( ( c1 & 0xC0 ) != 0x80 ) {
            /* invalid continuation byte */
            *outPoint = -1;
            return inBuffer;
            }

        c2 = (unsigned char)( inBuffer[ 2 ] );

        if( c2 == '\0' ) {
            *outPoint = -1;
            return inBuffer;
            }

        if( ( c2 & 0xC0 ) != 0x80 ) {
            /* invalid continuation byte */
            *outPoint = -1;
            return inBuffer;
            }

        /* take 4 bits from first byte, and 6 bits from each continuation */
        *outPoint =  (long)( ( c0 & 0x0F ) << 12
                             |
                             ( c1 & 0x3F ) << 6
                             |
                             ( c2 & 0x3F ) );
        
        return &( inBuffer[ 3 ] );
        }
    else if( ( c0 & 0xF8 ) == 0xF0 ) {
        /* first byte is 11110xxx
           4-byte case */
        
        c1 = (unsigned char)( inBuffer[ 1 ] );

        if( c1 == '\0' ) {
            *outPoint = -1;
            return inBuffer;
            }

        if( ( c1 & 0xC0 ) != 0x80 ) {
            /* invalid continuation byte */
            *outPoint = -1;
            return inBuffer;
            }

        c2 = (unsigned char)( inBuffer[ 2 ] );

        if( c2 == '\0' ) {
            *outPoint = -1;
            return inBuffer;
            }

        if( ( c2 & 0xC0 ) != 0x80 ) {
            /* invalid continuation byte */
            *outPoint = -1;
            return inBuffer;
            }
        
        c3 = (unsigned char)( inBuffer[ 3 ] );

        if( c3 == '\0' ) {
            *outPoint = -1;
            return inBuffer;
            }

        if( ( c3 & 0xC0 ) != 0x80 ) {
            /* invalid continuation byte */
            *outPoint = -1;
            return inBuffer;
            }
        
        /* take 3 bits from first byte, and 6 bits from each continuation */
        *outPoint = (long)( ( c0 & 0x07 ) << 18
                            |
                            ( c1 & 0x3F ) << 12
                            |
                            ( c2 & 0x3F ) << 6
                            |
                            ( c3 & 0x3F ) );
        
        return &( inBuffer[ 4 ] );
        }
    else {
        /* we should never get here */
        *outPoint = -1;
        return inBuffer;
        }
    }



/* returns index to mx_fontHashTable
   will return an index pointing to an empty entry if inCodePoint not found

   returns -1   if hash table totally full and code point not found
                (note that hash table 2x sizing should prevent this case)
*/
static int mx_fontHashLookup( MaxiginFont   *inFont,
                              unsigned long  inCodePoint ) {
    
    int            start      =  inFont->hashTableStart;
    int            size       =  inFont->hashTableNumEntries;
    unsigned long  mask       =  inFont->hashMask;

    int            next       =  (int)( inCodePoint & mask ) + start;
    int            wrapCount  =  0;
    
    while( mx_fontHashTable[ next ].codePoint != inCodePoint
           &&
           mx_fontHashTable[ next ].codePoint != 0 ) {

        next ++;

        if( next - start >= size ) {
            /* wrap around */
            next = start;
            wrapCount ++;

            if( wrapCount >= 2 ) {
                /* entire hash table is full ??
                   this should never happen */
                return -1;
                }
            }
        }

    /* found match, or found empty */

    return next;
    }


/* returns sprite handle

   returns -1   if sprite doesn't exist for code point in font
*/
static int mx_fontSpriteLookup( MaxiginFont   *inFont,
                                unsigned long  inCodePoint ) {

    if( inCodePoint < 128 ) {
        return inFont->oneByteMap[ inCodePoint ];
        }
    else {
        
        int  hashLoc  =  mx_fontHashLookup( inFont,
                                            inCodePoint );
        
        if( hashLoc == -1 ) {
            return -1;
            }
    
        if( mx_fontHashTable[ hashLoc ].codePoint == 0 ) {
            /* found empty entry, code point does not exist in table */
            return -1;
            }

        return mx_fontHashTable[ hashLoc ].spriteHandle;
        }
    }



int maxigin_initFont( int          inSpriteStripHandle,
                      const char  *inMapBulkResourceName,
                      int          inCharSpacing,
                      int          inSpaceWidth,
                      int          inFixedWidth ) {

    int           bulkHandle;
    int           bulkSize;
    long          codePoint;
    int           numFontChars;
    int           hashTableSize;
    int           numCodePointsRead  =  0;
    int           newFontHandle;
    MaxiginFont  *f;
    int           h;
    int           b;
    
    
    if( mx_numFonts >= MAXIGIN_MAX_NUM_FONTS ) {
        maxigin_logString( "Too many fonts already loaded, "
                           "loading a new font failed: ",
                           inMapBulkResourceName );
        return -1;
        }

    newFontHandle = mx_numFonts;
    f = &( mx_fonts[ newFontHandle ] );
    
    numFontChars = maxigin_getNumSpritesInStrip( inSpriteStripHandle );;
    
    hashTableSize = mx_getHashTableSize( numFontChars );

    if( hashTableSize + mx_numCharHashEntries
        >
        MAXIGIN_FONT_HASH_TABLE_SIZE ) {
        
        maxigin_logString( "Not enough extgra room in UTF-8 lookup hash table, "
                           "loading a new font failed: ",
                           inMapBulkResourceName  );
        return -1;
        }

    
    f->hashTableStart = mx_numCharHashEntries;
    f->hashTableNumEntries = hashTableSize;
    f->hashMask = (unsigned long)( hashTableSize - 1 );

    f->spacing = inCharSpacing;
    f->spaceWidth = inSpaceWidth;
    f->fixedWidth = inFixedWidth;

    /* clear one-byte map */
    for( b = 0;
         b < 128;
         b ++ ) {
        
        f->oneByteMap[ b ] = -1;
        }
    
    
    /* zero out hash table */
    for( h = f->hashTableStart;
         h < f->hashTableStart + hashTableSize;
         h ++ ) {

        mx_fontHashTable[ h ].codePoint = 0;
        }
    
    
    bulkHandle = mingin_startReadBulkData( inMapBulkResourceName,
                                           & bulkSize );

    if( bulkHandle == -1 ) {
        maxigin_logString( "Failed to open font UTF-8 character map: ",
                           inMapBulkResourceName );
        return -1;
        }

    codePoint = mx_readNextCodePoint( bulkHandle );

    while( codePoint != -1
           &&
           numCodePointsRead < numFontChars ) {

        if( codePoint < 128 ) {
            /* a one-byte point
               don't even bother storing this in the hash table */
            f->oneByteMap[ codePoint ] =
                maxigin_getSpriteFromStrip( inSpriteStripHandle,
                                            numCodePointsRead );
            
            mx_regenerateSpriteKerning( f->oneByteMap[ codePoint ]  );
            }
        else {
            /* a multi-byte point, put in hash table */
            
            int  hashLoc  =  mx_fontHashLookup( f,
                                                (unsigned long)codePoint );

            if( hashLoc == -1 ) {
                /* this should never happen, since we have already
                   confirmed that there's 2x the needed space
                   in the table */
                maxigin_logString( "UTF-8 lookup hash table full (?), "
                                   "loading a new font failed: ",
                                   inMapBulkResourceName  );
                mingin_endReadBulkData( bulkHandle );
                return -1;
                }

            mx_fontHashTable[ hashLoc ].codePoint = (unsigned long)codePoint;
        
            mx_fontHashTable[ hashLoc ].spriteHandle =
                maxigin_getSpriteFromStrip( inSpriteStripHandle,
                                            numCodePointsRead );

            mx_regenerateSpriteKerning(
                mx_fontHashTable[ hashLoc ].spriteHandle );
            }
        
        numCodePointsRead ++;
        codePoint = mx_readNextCodePoint( bulkHandle );
        }

    if( codePoint != -1 ) {
        maxigin_logString( "Font UTF-8 character map contains more entries "
                           "than there are font sprites in the sprite strip: ",
                           inMapBulkResourceName  );
        }
    else if( numCodePointsRead < numFontChars ) {
        maxigin_logString( "Font UTF-8 character map contains fewer entries "
                           "than there are font sprites in the sprite strip: ",
                           inMapBulkResourceName  );
        }

    mingin_endReadBulkData( bulkHandle );

    /* success */

    mx_numFonts ++;

    mx_numCharHashEntries += hashTableSize;
    
    
    return newFontHandle;
    }



/* must be a power of 2 */
#define  MAXIGIN_NUM_KERNING_CACHE_ENTRIES  2048

static  unsigned long  mx_kerningCacheHashMask  =
                           MAXIGIN_NUM_KERNING_CACHE_ENTRIES - 1;


typedef struct MaxiginKerningRecord {
        
        int  prevSpriteHandle;
        int  nextSpriteHandle;

        int            sep;
        
    } MaxiginKerningRecord;


static  MaxiginKerningRecord  mx_kerningCache[
                                  MAXIGIN_NUM_KERNING_CACHE_ENTRIES ];

static  char                  mx_kerningCacheInitialized  =  0;


static void mx_clearKerningCache( void ) {

    int  i;

    for( i = 0;
         i < MAXIGIN_NUM_KERNING_CACHE_ENTRIES;
         i ++ ) {

        MaxiginKerningRecord  *r  =  &( mx_kerningCache[ i ] );

        r->prevSpriteHandle = 0;
        r->nextSpriteHandle = 0;
        r->sep              = 0;
        }

    mx_kerningCacheInitialized = 1;
    }



/* returns pointer to bin in kerning cache for a given sprite pair
   bin may already contain values for a different pair */
static MaxiginKerningRecord *mx_kerningCacheGetBin( int  inPrevSpriteHandle,
                                                    int  inNextSpriteHandle ) {
    unsigned long  key;

    /* from Knuth */
    key = (unsigned long)inPrevSpriteHandle * 2654435761;
    

    key = ( key
            ^
            (unsigned long)inNextSpriteHandle )
        & mx_kerningCacheHashMask;

    return &( mx_kerningCache[ key ] );
    }



#define  MAXIGIN_KERNING_CACHE_MISS   -9999

/* returns sep, or MAXIGIN_KERNING_CACHE_MISS if not found */
static int mx_kerningCacheLookup( int  inPrevSpriteHandle,
                                  int  inNextSpriteHandle ) {

    MaxiginKerningRecord  *r  =  mx_kerningCacheGetBin( inPrevSpriteHandle,
                                                        inNextSpriteHandle );

    if( r->prevSpriteHandle == inPrevSpriteHandle
        &&
        r->nextSpriteHandle == inNextSpriteHandle ) {
        
        return r->sep;
        }
    
    return MAXIGIN_KERNING_CACHE_MISS;
    }


static void mx_kerningCacheInsert( int  inPrevSpriteHandle,
                                   int  inNextSpriteHandle,
                                   int  sep ) {

    MaxiginKerningRecord  *r  =  mx_kerningCacheGetBin( inPrevSpriteHandle,
                                                        inNextSpriteHandle );

    r->prevSpriteHandle = inPrevSpriteHandle;
    r->nextSpriteHandle = inNextSpriteHandle;
    r->sep              = sep;
    }



void maxigin_drawText( int           inFontHandle,
                       const char   *inText,
                       int           inLocationX,
                       int           inLocationY,
                       MaxiginAlign  inAlign ) {

    MaxiginFont  *f              =  &( mx_fonts[ inFontHandle ] );
    char         *nextText       =  (char*)inText;
    int           numSprites     =  0;
    int           totalPixWidth  =  0;
    int           startX;
    int           s;
    char          fixed          =  ( f->fixedWidth > 0 );
    int           spaceW         =  f->spaceWidth;
    int           halfSpaceW     =  spaceW / 2;
    int           charSpaceW     =  f->spacing;
    enum{         BUFFER_LEN     =  256 };
    
    static  int  spriteHandles[ BUFFER_LEN ];
    static  int  charCenterOffsetFromPrev[ BUFFER_LEN ];

    if( ! mx_kerningCacheInitialized ) {
        mx_clearKerningCache();
        }
    
    /* first, convert our string into sprite handles */
    while( nextText[0] != '\0' ) {

        long  codePoint;
        int   spriteHandle;
        
        nextText = mx_scanNextCodePoint( nextText,
                                         &codePoint );

        if( codePoint == -1 ) {
            break;
            }
        spriteHandle = mx_fontSpriteLookup( f,
                                            (unsigned long)codePoint );

        
        spriteHandles[ numSprites ] = spriteHandle;

        if( ! fixed ) {
            /* variable width chars */

            if( numSprites == 0 ) {
                /* the first sprite */
                
                if( spriteHandle >= 0 ) {
                
                    charCenterOffsetFromPrev[ numSprites ] =
                        mx_sprites[ spriteHandle ].leftVisibleRadius;
                
                    totalPixWidth +=
                        mx_sprites[ spriteHandle ].leftVisibleRadius
                        +
                        mx_sprites[ spriteHandle ].rightVisibleRadius;  
                    }
                else {
                    /* no sprite for this char, count as a space */
                    charCenterOffsetFromPrev[ numSprites ] = halfSpaceW;
                    totalPixWidth += spaceW;
                    }
                }
            else {
                /* a subsequent sprite */

                int  prevHandle  = spriteHandles[ numSprites - 1 ];
                
                if( prevHandle == -1 ) {
                    /* prev was a space */
                    charCenterOffsetFromPrev[ numSprites ] = halfSpaceW;

                    if( spriteHandle >= 0 ) {
                        /* not space for this */
                        charCenterOffsetFromPrev[ numSprites ] +=
                            mx_sprites[ spriteHandle ].leftVisibleRadius;
                        
                        totalPixWidth +=
                            mx_sprites[ spriteHandle ].leftVisibleRadius
                            +
                            mx_sprites[ spriteHandle ].rightVisibleRadius;
                        }
                    else {
                        /* two spaces in a row */
                        charCenterOffsetFromPrev[ numSprites ] = spaceW;
                        totalPixWidth += spaceW;
                        }
                    }
                else {
                    /* prev was a non-space */

                    if( spriteHandle == -1 ) {
                        /* non-space followed by a space */
                        charCenterOffsetFromPrev[ numSprites ] =
                            mx_sprites[ prevHandle ].rightVisibleRadius;
                        
                        charCenterOffsetFromPrev[ numSprites ] += halfSpaceW;
                                                
                        totalPixWidth += spaceW;
                        }
                    else {
                        /* non-space followed by a non-space
                           kerning per pixel row! */

                        /* cached ? */

                        int  sep  = mx_kerningCacheLookup( prevHandle,
                                                           spriteHandle );

                        if( sep == MAXIGIN_KERNING_CACHE_MISS ) {

                            /* recompute it */
                            
                        
                            int   w          =  mx_sprites[ spriteHandle ].w;
                            int   h          =  mx_sprites[ spriteHandle ].h;
                            int   prevI      =
                                    mx_sprites[ prevHandle   ].kerningTableIndex;
                            int   thisI      =
                                    mx_sprites[ spriteHandle ].kerningTableIndex;
                            int  *prevRight  =
                                    mx_fontKerningTable[ prevI ][ 1 ];
                            int  *thisLeft   =
                                    mx_fontKerningTable[ thisI ][ 0 ];
                        
                            int  y;
                        
                            sep = -w;

                            for( y = 0;
                                 y < h;
                                 y ++ ) {

                                int  rowSep  = prevRight[y] - thisLeft[y];

                                if( rowSep > sep ) {
                                    sep = rowSep;
                                    }
                                }
                            
                            sep += 1;
                            
                            mx_kerningCacheInsert( prevHandle,
                                                   spriteHandle,
                                                   sep );
                            }

                        sep += charSpaceW;
                        
                        charCenterOffsetFromPrev[ numSprites ] = sep;

                        totalPixWidth += sep;

                        totalPixWidth -=
                            mx_sprites[ prevHandle ].rightVisibleRadius;
                        
                        totalPixWidth +=
                            mx_sprites[ spriteHandle ].rightVisibleRadius;
                        }
                    }
                }
            }
        else {
            /* fixed width for all chars */
            charCenterOffsetFromPrev[ numSprites ] = f->fixedWidth;
            totalPixWidth += f->fixedWidth;

            if( numSprites > 0 ) {
                /* not first sprite */
                charCenterOffsetFromPrev[ numSprites ] += charSpaceW;
                totalPixWidth += charSpaceW;
                } 
            }

        
        numSprites ++;

        if( numSprites >= BUFFER_LEN ) {
            /* cut off at the max string length limit */
            break;
            }
        }

    
    if( numSprites == 0
        ||
        totalPixWidth == 0 ) {
        
        return;
        }

    
    switch( inAlign ) {
        case MAXIGIN_LEFT:
            startX = inLocationX;
            break;
        case MAXIGIN_RIGHT:
            startX = inLocationX - totalPixWidth;
            break;
        default:
            /* center */
            startX = inLocationX - totalPixWidth / 2;
            break;
        }

    for( s = 0;
         s < numSprites;
         s ++ ) {

        startX += charCenterOffsetFromPrev[ s ];
        
        if( spriteHandles[ s ] != -1 ) {
            maxigin_drawSprite( spriteHandles[ s ],
                                startX,
                                inLocationY );
            }
                          
        }
    }



static  char  mx_fullKerningTableWarningPrinted  =  0;
static  char  mx_tooTallKerningWarningPrinted  =  0;


static void mx_regenerateSpriteKerning( int  inSpriteHandle ) {
    
    MaxiginSprite  *s            =  &( mx_sprites[ inSpriteHandle ] );
    
    int             w               =  s->w;
    int             h               =  s->h;
    int             y;
    int             startByte       =  s->startByte;
    int             kerningIndex    =  s->kerningTableIndex;


    if( h > MAXIGIN_MAX_FONT_SPRITE_HEIGHT ) {
        
        if( ! mx_tooTallKerningWarningPrinted ) {
            
            maxigin_logInt2( "Font character is ",
                             h,
                             " pixels tall, maxium height for kerning is ",
                             MAXIGIN_MAX_FONT_SPRITE_HEIGHT,
                             " font will not be kerned" );
            mx_tooTallKerningWarningPrinted = 1;
            }
        
        return;
        }
    
    if( kerningIndex == -1 ) {
        /* computing new entry for first time */

        kerningIndex = mx_numKerningTableEntries;

        if( kerningIndex >= MAXIGIN_MAX_TOTAL_FONT_CHARACTERS ) {
            /* kerning table is full */

            if( ! mx_fullKerningTableWarningPrinted ) {
                mingin_log( "Kerning table is full, additional font characters "
                            "will not be kerned.\n" );
                mx_fullKerningTableWarningPrinted = 1;
                }
            
            return;
            }

        mx_numKerningTableEntries ++;
        }

    s->kerningTableIndex = kerningIndex;


    /* any time we regenerate kerning for any sprite
       we must clear our kerning cache */
    mx_kerningCacheInitialized = 0;
    
    
    /* find left/right visible extents per row */
    
    for( y = 0;
         y < h;
         y ++ ) {

        int   curByte      =  startByte + y * w * 4;
        int   x;
        int  *leftmost   = &( mx_fontKerningTable[ kerningIndex ][ 0 ][ y ] );
        int  *rightmost  = &( mx_fontKerningTable[ kerningIndex ][ 1 ][ y ] );

        *leftmost  = w - 1;
        *rightmost = 0;
        
        for( x = 0;
             x < w;
             x ++ ) {

            unsigned char  a  =  mx_spriteBytes[ curByte + 3 ];

            if( a > 0 ) {

                if( x > *rightmost ) {
                    *rightmost = x;
                    }
                if( x < *leftmost ) {
                    *leftmost = x;
                    }
                }
            
            /* on to next pixel */
            curByte += 4;
            }
        }

    }







#define  MAXIGIN_LANGUAGE_FONT_MAX_NAME_LENGTH  64

static   int           mx_languageFontHandles[ MAXIGIN_MAX_NUM_LANGUAGE_FONTS ];

static   char          mx_languageFontBulkResourceNames
                           [ MAXIGIN_MAX_NUM_LANGUAGE_FONTS            ]
                           [ MAXIGIN_LANGUAGE_FONT_MAX_NAME_LENGTH + 1 ];

static   int           mx_numLanguageFonts  =  0;


#define  MAXIGIN_MAX_TRANSLATION_KEY_LENGTH  32

static   char          mx_translationKeys
                           [ MAXIGIN_MAX_NUM_TRANSLATION_KEYS ]
                           [ MAXIGIN_MAX_TRANSLATION_KEY_LENGTH + 1 ];

static   char          mx_anyTranslationKeysSet  = 0;


static   char          mx_translationStringBytes
                           [ MAXIGIN_MAX_TOTAL_TRANSLATION_STRING_BYTES ];

static   int           mx_numTranslationStringBytes = 0;



#define  MAXIGIN_LANGUAGE_NAME_MAX_LENGTH  64


typedef struct MaxiginLanguage {

        char          displayName     [ MAXIGIN_LANGUAGE_NAME_MAX_LENGTH + 1 ];
        char          bulkResourceName[ MAXIGIN_LANGUAGE_NAME_MAX_LENGTH + 1 ];
        
        int           fontHandle;

        /* pointers to starts of strings in mx_translationStringBytes, or 0 of
           not present */
        int           stringStartBytes[ MAXIGIN_MAX_NUM_TRANSLATION_KEYS ];
        
    } MaxiginLanguage;



static  MaxiginLanguage  mx_languages[ MAXIGIN_MAX_NUM_LANGUAGES ];
static  int              mx_numLanguages     =  0;
static  int              mx_currentLanguage  =  0;


static void mx_nextLang( void ) {
    if( mx_numLanguages == 0 ) {
        return;
        }
    
    mx_currentLanguage ++;

    if( mx_currentLanguage >= mx_numLanguages ) {
        mx_currentLanguage = 0;
        }
    }


static void mx_clearTranslationKeys( void ) {
    
    int  t;
    
    for( t = 0;
         t < MAXIGIN_MAX_NUM_TRANSLATION_KEYS;
         t ++ ) {
        
        mx_translationKeys[ t ][ 0 ] = '\0';
        }
    }



/* removes a translation string from mx_translationStringBytes, slides
   later strings back, and updates languages that point to those later strings
*/
static void mx_removeTranslationString( int  inStartByte ) {

    char  *s         =  &( mx_translationStringBytes[ inStartByte ] );
    int    len       =  maxigin_stringLength( s ) + 1;
    int    b;
    int    afterPos  =  inStartByte + len;
    int    ln;
    
    for( b = afterPos;
         b < MAXIGIN_MAX_TOTAL_TRANSLATION_STRING_BYTES;
         b ++ ) {

        mx_translationStringBytes[ b - len ] =
            mx_translationStringBytes[ b ];
        }

    for( ln = 0;
         ln < mx_numLanguages;
         ln ++ ) {

        MaxiginLanguage  *lang  =  &( mx_languages[ ln ] );
        int               i;

        for( i = 0;
             i < MAXIGIN_MAX_NUM_TRANSLATION_KEYS;
             i ++ ) {

            if( lang->stringStartBytes[ i ] > inStartByte ) {
                lang->stringStartBytes[ i ] -= len;
                }
            }
        
        }
    }


    


/* returns -1 if no matching language font found */
static int mx_findLanguageFont( const char  *inBulkResourceName ) {

    int  i;


    for( i = 0;
         i < mx_numLanguageFonts;
         i ++ ) {

        if( maxigin_stringsEqual( inBulkResourceName,
                                  mx_languageFontBulkResourceNames[i] ) ) {

            return mx_languageFontHandles[ i ];
            }
        }

    return -1;
    }




char maxigin_initTranslationKey( int          inPhraseKey,
                                 const char  *inPhraseKeyString ) {

    if( ! mx_areWeInMaxiginGameInitFunction ) {
        mingin_log( "Game tried to call "
                    "maxigin_initTranslationKey "
                    "from outside of maxiginGame_init\n" );
        return 0;
        }
    
    if( inPhraseKey < 0
        ||
        inPhraseKey >= MAXIGIN_MAX_NUM_TRANSLATION_KEYS ) {

        maxigin_logInt( "Translation key out of range: ",
                        inPhraseKey );
        
        maxigin_logString( "  Corresponding key string = ",
                           inPhraseKeyString );
        return 0;
        }

    if( maxigin_stringLength( inPhraseKeyString )
        >
        MAXIGIN_MAX_TRANSLATION_KEY_LENGTH ) {
        
        maxigin_logString( "Translation key too long, skipping: ",
                           inPhraseKeyString );
        return 0;
        }

    maxigin_stringCopy( inPhraseKeyString,
                        mx_translationKeys[ inPhraseKey ] );

    mx_anyTranslationKeysSet = 1;

    return 1;
    }



/* returns index into mx_translationKeys or -1 if not found */
static int mx_findTranslationKey( const char *inPhraseKeyString ) {

    int  i;

    for( i = 0;
         i < MAXIGIN_MAX_NUM_TRANSLATION_KEYS;
         i ++ ) {

        if( maxigin_stringsEqual( inPhraseKeyString,
                                  mx_translationKeys[ i ] ) ) {
            return i;
            }
        }
    
    return -1;
    }



/* inMaxStringLength is max length of string to parse
   will be truncated if longer

   inDestBuffer  must have room for inMaxStringLength + 1 bytes (for \0 end)

   closing quote is skipped in

   returns 1 on success
           0 on failure
*/
static char mx_readQuotedString( int    inBulkReadHandle,
                                 int    inMaxStringLength,
                                 char  *inDestBuffer ) {

    int           i           =   0;
    int           readNum;
    char          c;
    

    /* consume up past first " mark */
    
    readNum = mingin_readBulkData( inBulkReadHandle,
                                   1,
                                   (unsigned char *)&( c ) );
    
    while( readNum == 1
           &&
           c != '"' ) {
        readNum = mingin_readBulkData( inBulkReadHandle,
                                       1,
                                       (unsigned char *)&( c ) );
        }

    if( readNum != 1 ) {
        mingin_log( "Failed to find first quote mark when scanning "
                    "for quoted string in bulk resource\n" );
        return 0;
        }

    
    /* now consume until closing " mark and fill buffer with it */
    
    readNum = mingin_readBulkData( inBulkReadHandle,
                                   1,
                                   (unsigned char *)&( c ) );
    
    while( readNum == 1
           &&
           c != '"'
           &&
           i < inMaxStringLength ) {

        inDestBuffer[i] =  c;
        i++;
        
        readNum = mingin_readBulkData( inBulkReadHandle,
                                   1,
                                   (unsigned char *)&( c ) );
        }

    /* any of the cases where we fall out of loop are okay
       either we consumed closing quote mark, or we reached the length limit,
       or we got to the end of the data resource
       In all cases, terminate whatever string we have and return */

    inDestBuffer[i] = '\0';

    return 1;
    }



/* inits language in mx_languages in inSlot, or adds to end if inSlot is -1 */
static void mx_initLanguage( const char  *inLanguageBulkResourceName,
                             int          inSlot ) {

    MaxiginLanguage  *lang;
    int               languageReadHandle;
    int               dataLen;
    const char       *langFontName;
    const char       *langFontTextName;
    int               fontHeight;
    int               fontCharSpacing;
    int               fontSpaceWidth;
    int               fontFixedWidth;
    char              success;
    int               i;
    const char       *nextKey;
    char              isNewLangSlot;
    
    if( mx_numLanguages >= MAXIGIN_MAX_NUM_LANGUAGES ) {
        maxigin_logString( "Too many languages already loaded, skipping:  ",
                           inLanguageBulkResourceName );
        return;
        }


    if( maxigin_stringLength( inLanguageBulkResourceName )
        >
        MAXIGIN_LANGUAGE_NAME_MAX_LENGTH ) {

        maxigin_logString( "Failed to loadlanguage bulk resource with name "
                           "that is too long: ",
                           inLanguageBulkResourceName );
        return;
        }
        
    maxigin_logString( "Loading language:  ",
                       inLanguageBulkResourceName );

    isNewLangSlot = 0;
    
    if( inSlot == -1 ) {
        isNewLangSlot = 1;
        inSlot = mx_numLanguages;
        }
    
    lang = &( mx_languages[ inSlot ] );

    
    maxigin_stringCopy( inLanguageBulkResourceName,
                        lang->bulkResourceName );
    

    languageReadHandle = mingin_startReadBulkData( inLanguageBulkResourceName,
                                                   &dataLen );

    if( languageReadHandle == -1 ) {
        maxigin_logString( "Failed to open language bulk resource: ",
                           inLanguageBulkResourceName );
        return;
        }

    success = mx_readQuotedString( languageReadHandle,
                                   MAXIGIN_LANGUAGE_NAME_MAX_LENGTH,
                                   lang->displayName );

    if( ! success ) {
        maxigin_logString( "Failed to read display name from language "
                           "bulk resource: ",
                           inLanguageBulkResourceName );
        
        mingin_endReadBulkData( languageReadHandle );
        return;
        }
        

    langFontName = mx_readShortTokenFromBulkData( languageReadHandle );

    if( langFontName == 0 ) {
        maxigin_logString( "Failed to read font TGA name from language "
                           "bulk resource: ",
                           inLanguageBulkResourceName );
        
        mingin_endReadBulkData( languageReadHandle );
        return;
        }

    if( maxigin_stringLength( langFontName ) >
        MAXIGIN_LANGUAGE_FONT_MAX_NAME_LENGTH ) {

        maxigin_logString( "Font TGA name too long in language "
                           "bulk resource: ",
                           inLanguageBulkResourceName );
        
        mingin_endReadBulkData( languageReadHandle );
        return;
        }
    

    success = mx_readIntTokenFromBulkData( languageReadHandle,
                                           &fontHeight );

    if( ! success ) {
        maxigin_logString( "Failed to read font character height from language "
                           "bulk resource: ",
                           inLanguageBulkResourceName );
        
        mingin_endReadBulkData( languageReadHandle );
        return;
        }

    success = mx_readIntTokenFromBulkData( languageReadHandle,
                                           &fontCharSpacing );

    if( ! success ) {
        maxigin_logString( "Failed to read font character spacing from language "
                           "bulk resource: ",
                           inLanguageBulkResourceName );
        
        mingin_endReadBulkData( languageReadHandle );
        return;
        }

    success = mx_readIntTokenFromBulkData( languageReadHandle,
                                           &fontSpaceWidth );

    if( ! success ) {
        maxigin_logString( "Failed to read font space width from language "
                           "bulk resource: ",
                           inLanguageBulkResourceName );
        
        mingin_endReadBulkData( languageReadHandle );
        return;
        }

    success = mx_readIntTokenFromBulkData( languageReadHandle,
                                           &fontFixedWidth );

    if( ! success ) {
        maxigin_logString( "Failed to read font fixed width from language "
                           "bulk resource: ",
                           inLanguageBulkResourceName );
        
        mingin_endReadBulkData( languageReadHandle );
        return;
        }

    langFontTextName = mx_readShortTokenFromBulkData( languageReadHandle );

    if( langFontTextName == 0 ) {
        maxigin_logString( "Failed to read font TXT name from language "
                           "bulk resource: ",
                           inLanguageBulkResourceName );
        
        mingin_endReadBulkData( languageReadHandle );
        return;
        }

    
    lang->fontHandle = mx_findLanguageFont( langFontName );

    if( lang->fontHandle >= 0 ) {
        /* found one
           Update it's spacing settings.
           Note that if multiple languages use the same font,
           the spacing from the last one loaded (or hot-reloaded) will win.
           This enables us to hot reload and tweak these settings when
           a language file changes without completely re-making the font */
        MaxiginFont  *font  =  &( mx_fonts[ lang->fontHandle ] );

        font->spacing    = fontCharSpacing;
        font->spaceWidth = fontSpaceWidth;
        font->fixedWidth = fontFixedWidth;
        }
        
    if( lang->fontHandle == -1 ) {
        /* no matching font found, build a new one */

        int  stripHandle;
        int  fontHandle;

        if( mx_numLanguageFonts >= MAXIGIN_MAX_NUM_LANGUAGE_FONTS ) {
            maxigin_logString( "Too many language fonts already when trying "
                               "to create new one for language: ",
                               inLanguageBulkResourceName );
            
            mingin_endReadBulkData( languageReadHandle );
            return;
            }
        
        stripHandle = maxigin_initSpriteStrip( langFontName,
                                               fontHeight );

        if( stripHandle == -1 ) {
            mingin_log(
                maxigin_stringConcat5(
                    "Failed to read font strip ",
                    langFontName,
                    " specified in language bulk resource: ",
                    inLanguageBulkResourceName,
                    "\n" ) );
            
            mingin_endReadBulkData( languageReadHandle );
            return;
            }
        
        
        fontHandle = maxigin_initFont( stripHandle,
                                       langFontTextName,
                                       fontCharSpacing,
                                       fontSpaceWidth,
                                       fontFixedWidth );

        if( fontHandle == -1 ) {
            mingin_log(
                maxigin_stringConcat5(
                    "Failed to read font mapping ",
                    langFontName,
                    " specified in language bulk resource: ",
                    inLanguageBulkResourceName,
                    "\n" ) );
            
            mingin_endReadBulkData( languageReadHandle );
            return;
            }

        lang->fontHandle = fontHandle;
        
        mx_languageFontHandles[ mx_numLanguageFonts ] = fontHandle;

        maxigin_stringCopy( langFontName,
                            mx_languageFontBulkResourceNames
                                [ mx_numLanguageFonts ] );
        
        mx_numLanguageFonts ++;
        }


    /* clear all translation keys */
    for( i = 0;
         i < MAXIGIN_MAX_NUM_TRANSLATION_KEYS;
         i ++ ) {
        lang->stringStartBytes[ i ] = -1;
        }


    nextKey = mx_readShortTokenFromBulkData( languageReadHandle );

    while( nextKey != 0 ) {

        int  key  =  mx_findTranslationKey( nextKey );

        if( key == -1 ) {
            mingin_log(
                maxigin_stringConcat5(
                    "Failed to find translation key ",
                    nextKey,
                    " specified in language bulk resource: ",
                    inLanguageBulkResourceName,
                    " (game needs to call maxigin_initTranslationKey for "
                    "each valid key during maxiginGame_init() )\n" ) );

            mingin_endReadBulkData( languageReadHandle );
            return;
            }
        
        if( mx_numTranslationStringBytes ==
            MAXIGIN_MAX_TOTAL_TRANSLATION_STRING_BYTES ) {

            maxigin_logString( "Ran out of room for more translation strings "
                               "when loading language: ",
                               inLanguageBulkResourceName );
            
            mingin_endReadBulkData( languageReadHandle );
            return;
            }
        
        success = mx_readQuotedString(
            languageReadHandle,
            MAXIGIN_MAX_TOTAL_TRANSLATION_STRING_BYTES
            -
            mx_numTranslationStringBytes
            - 1,
            &( mx_translationStringBytes[
                   mx_numTranslationStringBytes ] ) );

        if( ! success ) {
            maxigin_logString( "Failed to read translation string "
                               "when loading language: ",
                               inLanguageBulkResourceName );
            
            mingin_endReadBulkData( languageReadHandle );
            return;
            }

        lang->stringStartBytes[ key ] = mx_numTranslationStringBytes;

        mx_numTranslationStringBytes +=
            maxigin_stringLength(
                &( mx_translationStringBytes[ mx_numTranslationStringBytes ] ) );

        /* also advance past termination byte */
        mx_numTranslationStringBytes += 1;

        nextKey = mx_readShortTokenFromBulkData( languageReadHandle );
        }
    
    
    mingin_endReadBulkData( languageReadHandle );


    /* language successfully loaded */
    if( isNewLangSlot ) {
        mx_numLanguages ++;
        }
    }



static void mx_initLanguages( void ) {

    int          bulkHandle;
    int          len;
    const char  *token;

    
    if( ! mx_anyTranslationKeysSet ) {
        mingin_log( "No translation keys set by game, so skipping loading "
                    "languages" );
        return;
        }

    bulkHandle = mingin_startReadBulkData( "languages.txt",
                                           &len );

    if( bulkHandle == -1 ) {
        mingin_log( "Failed to open bulk data resource:  languages.txt\n" );
        return;
        }

    token = mx_readShortTokenFromBulkData( bulkHandle );


    while( token != 0 ) {

        mx_initLanguage( token,
                         -1 );

        token = mx_readShortTokenFromBulkData( bulkHandle );
        }
    }



static  char  mx_drawLangFailureShown  =  0;


void maxigin_drawLangText( int           inPhraseKey,
                           int           inLocationX,
                           int           inLocationY,
                           MaxiginAlign  inAlign ) {

    MaxiginLanguage  *lang;
    char             *langString;
    
    if( mx_currentLanguage >= mx_numLanguages ) {

        if( ! mx_drawLangFailureShown ) {
            mingin_log( "maxigin_drawLangText called when no "
                        "languages loaded\n" );
            mx_drawLangFailureShown = 1;
            }
        return;
        }

    lang = &( mx_languages[ mx_currentLanguage ] );

    if( inPhraseKey < 0
        ||
        inPhraseKey >= MAXIGIN_MAX_NUM_TRANSLATION_KEYS ) {

        if( ! mx_drawLangFailureShown ) {
            maxigin_logInt( "maxigin_drawLangText called with phrase key "
                            "out of range: ",
                            inPhraseKey );
            mx_drawLangFailureShown = 1;
            }
        return;
        }


    langString = &( mx_translationStringBytes[
                        lang->stringStartBytes[ inPhraseKey ] ] );

    if( langString == 0 ) {
        if( ! mx_drawLangFailureShown ) {
            maxigin_logString( "Translation string for phrase key "
                               "not found for current language: ",
                               mx_translationKeys[ inPhraseKey ] );
            mx_drawLangFailureShown = 1;
            }
        return;
        }
        
    maxigin_drawText( lang->fontHandle,
                      langString,
                      inLocationX,
                      inLocationY,
                      inAlign ); 
    }



static void mx_checkLangNeedsReload( void ) {
    int  ln;

    for( ln = 0;
         ln < mx_numLanguages;
         ln ++ ) {

        MaxiginLanguage  *lang  =  &( mx_languages[ ln ] );
        
        if( mingin_getBulkDataChanged( lang->bulkResourceName ) ) {

            int  k;

            char  bulkName[ MAXIGIN_LANGUAGE_NAME_MAX_LENGTH + 1 ];

            maxigin_stringCopy( lang->bulkResourceName,
                                bulkName );
            
            for( k = 0;
                 k < MAXIGIN_MAX_NUM_TRANSLATION_KEYS;
                 k ++ ) {

                if( lang->stringStartBytes[ k ] != -1 ) {

                    mx_removeTranslationString( lang->stringStartBytes[ k ] );

                    lang->stringStartBytes[ k ] = -1;
                    }
                }

            /* re-init in same spot */
            mx_initLanguage( lang->bulkResourceName,
                             ln );
            }
        }
    }






/* end #ifdef MAXIGIN_IMPLEMENTATION */
#endif





/* end of #ifndef MAXIGIN_H_INCLUDED */
#endif
