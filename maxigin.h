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
  How many sprites are supported?
  
  To make room for 256 sprites, do this:

      #define  MAXIGIN_MAX_NUM_SPRITES  256

  [jumpSettings]
*/
#ifndef  MAXIGIN_MAX_NUM_SPRITES 
    #define  MAXIGIN_MAX_NUM_SPRITES  1024
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

  The rate at which maxigin_gameStep is called can be found by calling

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
  Loads a TGA-formatted sprite from the platform's bulk data store, and
  generates an internal glow sprite to go along with it.  Calls to
  maxigin_drawSprite for the resulting sprite handle will draw the underlying
  sprite and additively blend in the glow sprite.
  
  Sprites must be in RGBA 32-bit uncompressed TGA format.

  Parameters:

      inBulkResourceName   the name of the bulk data resource to load the sprite
                           from

      inBlurRadius         the blur radius for the glow, in pixels

      inBlurIterations     the number of iterations of the blur to apply
      
  Returns:

      sprite handle   on load success

      -1              on failure;

  [jumpMaxiginInit]      
*/
int maxigin_initGlowSprite( const char  *inBulkResourceName,
                            int          inBlurRadius,
                            int          inBlurIterations );


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

  Sprites must be in RGBA 32-bit uncompressed TGA format.

  
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



/*
  Sets the active draw color for future calls to drawing functions.
  
  Defaults to opaque white (255, 255, 255, 255).

  The color components affect geometric shapes and do not change the colors
  of sprites.

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




/* all of the button actions that Maxigin registers internally */
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
        
        unsigned char  hash[ MAXIGIN_SPRITE_HASH_LENGTH ];
        
    } MaxiginSprite;



static  unsigned char  mx_spriteBytes  [ MAXIGIN_MAX_TOTAL_SPRITE_BYTES ];
static  MaxiginSprite  mx_sprites      [ MAXIGIN_MAX_NUM_SPRITES        ];

static  int            mx_numSpriteBytesUsed  =  0;
static  int            mx_numSprites          =  0;



        
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
    int   y;
    int   leftRadius;
    int   rightRadius;
    int   xCenter;
    
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



    
    if( newSpriteHandle == mx_numSprites ) {

        /* leave old glow handle in place if this isn't a new sprite record */
        
        mx_sprites[ newSpriteHandle ].glowSpriteHandle    = -1;
            
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

    /* now find left/right visible extents */
    leftRadius  = 0;
    rightRadius = 0;
    xCenter     = w / 2;
    
    for( y = 0;
         y < h;
         y ++ ) {

        int  curByte  =  startByte + y * w * 4;
        int  x;
        
        for( x = 0;
             x < w;
             x ++ ) {

            unsigned char  a  =  mx_spriteBytes[ curByte ];

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

    mx_sprites[ newSpriteHandle ].leftVisibleRadius  = leftRadius;

    /* center position is actually off by 1 in favor of left radius */
    mx_sprites[ newSpriteHandle ].rightVisibleRadius = rightRadius + 1;
    
    


    /* hash bytes after origin flip and BGRA conversion */

    maxigin_flexHash( neededSpriteBytes,
                      &( mx_spriteBytes[ startByte ] ),
                      MAXIGIN_SPRITE_HASH_LENGTH,
                      mx_sprites[ newSpriteHandle ].hash );
     
    
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

    const char    *glowSpriteDataName;
    int            persistReadHandle;
    int            numBytes;
    int            numRead;
    int            b;
    int            glowW;
    int            glowH;
    int            glowBorder;
    
    char           readGlowFromFile;
    unsigned char  hashBuffer[ MAXIGIN_SPRITE_HASH_LENGTH ];



    glowBorder = inBlurRadius * inBlurIterations * 2;
    
    glowW = mx_sprites[ inMainSpriteHandle ].w
            +
            2 * glowBorder;
        
    glowH = mx_sprites[ inMainSpriteHandle ].h
            +
            2 * glowBorder;
    
    glowSpriteDataName = maxigin_stringConcat(
                             mx_sprites[ inMainSpriteHandle ].bulkResourceName,
                             ".glow" );

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
                        mx_sprites[ inMainSpriteHandle ].hash[ b ] ) {
                        
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
                    if( mx_sprites[ inMainSpriteHandle ].glowSpriteHandle ==
                        -1 ) {

                        MinginOpenData  openData;
                        
                        /* load from glow file */
                        openData.readHandle = persistReadHandle;
                        openData.isBulk = 0;

                        /* set a non-file-name (blank string )
                           as inBulkResourceName
                           since this glow sprite isn't actually in
                           our bulk data store */
                        mx_sprites[ inMainSpriteHandle ].glowSpriteHandle =
                            mx_reloadSpriteFromOpenData(
                                "",
                                -1,
                                & openData,
                                /* TGA data comes right after hash bytes */
                                numBytes - MAXIGIN_SPRITE_HASH_LENGTH );

                        if( mx_sprites[ inMainSpriteHandle ].glowSpriteHandle !=
                            -1 ) {
                            /* successfully loaded from file */
                            readGlowFromFile = 1;
                            
                            maxigin_logString( "Successfully read cached glow "
                                               "sprite from perisistent data "
                                               "store for ",
                                               mx_sprites[ inMainSpriteHandle ].
                                                   bulkResourceName );
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
        
        glowSpriteHandle = mx_sprites[ inMainSpriteHandle ].glowSpriteHandle;


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
                    mx_sprites[ inMainSpriteHandle ].bulkResourceName );
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
                mx_sprites[ inMainSpriteHandle ].bulkResourceName );
            
            return;
            }

        
        mx_sprites[ inMainSpriteHandle ].glowSpriteHandle = glowSpriteHandle;
        mx_sprites[ inMainSpriteHandle ].glowRadius       = inBlurRadius;
        mx_sprites[ inMainSpriteHandle ].glowIterations   = inBlurIterations;

        glowStartByte  =  mx_numSpriteBytesUsed;
        
        mainW          =  mx_sprites[ inMainSpriteHandle ].w;
        mainH          =  mx_sprites[ inMainSpriteHandle ].h;
        mainStartByte  =  mx_sprites[ inMainSpriteHandle ].startByte;
        
        mx_sprites[ glowSpriteHandle ].w                = glowW;
        mx_sprites[ glowSpriteHandle ].h                = glowH;
        mx_sprites[ glowSpriteHandle ].startByte        = glowStartByte;
        mx_sprites[ glowSpriteHandle ].glowSpriteHandle = -1;

        mx_sprites[ glowSpriteHandle ].bulkResourceName[0] = '\0';

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
                                 mx_sprites[ inMainSpriteHandle ].hash );

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

    


int maxigin_initGlowSprite( const char  *inBulkResourceName,
                            int          inBlurRadius,
                            int          inBlurIterations ) {

    int  spriteHandle;
    
    if( ! mx_areWeInMaxiginGameInitFunction ) {
        mingin_log( "Game tried to call maxigin_initGlowSprite "
                    "from outside of maxiginGame_init\n" );
        return -1;
        }

    spriteHandle = mx_reloadSprite( inBulkResourceName,
                                    -1 );

    if( spriteHandle != -1 ) {
        mx_regenerateGlowSprite( spriteHandle,
                                 inBlurRadius,
                                 inBlurIterations );
        }

    return spriteHandle;
    }



typedef struct MaxiginSliderSprites {

        /* index 0 for empty, 1 for full */
        int  left  [2];
        int  right [2];
        int  bar[2];
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
        ( mx_sliderSprites.left[ 0 ] > 0 )
        &&
        ( mx_sliderSprites.left[ 1 ] > 0 )
        &&
        ( mx_sliderSprites.right[ 0 ] > 0 )
        &&
        ( mx_sliderSprites.right[ 1 ] > 0 )
        &&
        ( mx_sliderSprites.bar[ 0 ] > 0 )
        &&
        ( mx_sliderSprites.bar[ 1 ] > 0 )
        &&
        ( mx_sliderSprites.sliver[ 0 ] > 0 )
        &&
        ( mx_sliderSprites.sliver[ 1 ] > 0 )
        &&
        ( mx_sliderSprites.thumb[ 0 ] > 0 )
        &&
        ( mx_sliderSprites.thumb[ 1 ] > 0 )
        &&
        ( mx_sliderSprites.thumb[ 2 ] > 0 );
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
                    
                    mx_sprites[ s ].pendingChange       = 0;
                    mx_sprites[ s ].retryCount          = 0;
                    mx_sprites[ s ].stepsUntilNextRetry = 0;

                    if( mx_sprites[ s ].glowSpriteHandle != -1 ) {

                        mx_regenerateGlowSprite(
                            s,
                            mx_sprites[ s ].glowRadius,
                            mx_sprites[ s ].glowIterations );
                        }
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
                if( mx_sprites[ s ].glowSpriteHandle != -1 ) {

                    mx_regenerateGlowSprite(
                        s,
                        mx_sprites[ s ].glowRadius,
                        mx_sprites[ s ].glowIterations );
                    }
                }
            }
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

    
    if( ! mx_areWeInMaxiginGameDrawFunction ) {

        mingin_log( "Game tried to call maxigin_drawSprite "
                    "from outside of maxiginGame_getNativePixels\n" );
        return;
        }

    if( inSpriteHandle == -1 ) {
        /* trying to draw a sprite that failed to load
           ignore it */
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
                
                unsigned char  a  =  mx_spriteBytes[ spriteByte + 3 ];

                if( drawAlphaSet ) {
                    a = (unsigned char)( ( a * mx_drawColor.comp.alpha ) / 255 );
                    }
                    
                if( a == 255 ) {

                    int  v;

                    /* red */
                    v = mx_gameImageBuffer[ imageByte  ]
                        +
                        mx_spriteBytes    [ spriteByte ];

                    if( v > 255 ) {
                        v = 255;
                        }
                    mx_gameImageBuffer[ imageByte ] = (unsigned char)v;

                    imageByte  ++;
                    spriteByte ++;

                    /* green */
                    v = mx_gameImageBuffer[ imageByte  ]
                        +
                        mx_spriteBytes    [ spriteByte ];

                    if( v > 255 ) {
                        v = 255;
                        }
                    mx_gameImageBuffer[ imageByte ] = (unsigned char)v;

                    imageByte  ++;
                    spriteByte ++;

                    /* blue */
                    v = mx_gameImageBuffer[ imageByte  ]
                        +
                        mx_spriteBytes    [ spriteByte ];

                    if( v > 255 ) {
                        v = 255;
                        }
                    mx_gameImageBuffer[ imageByte ] = (unsigned char)v;

                    imageByte  ++;
                    spriteByte ++;
                    }
                else {
                    /* alpha blending */

                    int  v;
                    
                    /* red */
                    v = mx_gameImageBuffer[ imageByte ]
                        +
                        ( mx_spriteBytes[ spriteByte ] * a ) / 255;
                    
                    if( v > 255 ) {
                        v = 255;
                        }
                    mx_gameImageBuffer[ imageByte ] = (unsigned char)v;
                
                    imageByte  ++;
                    spriteByte ++;

                    /* green */
                    v = mx_gameImageBuffer[ imageByte ]
                        +
                        ( mx_spriteBytes[ spriteByte ] * a ) / 255;
                    
                    if( v > 255 ) {
                        v = 255;
                        }
                    mx_gameImageBuffer[ imageByte ] = (unsigned char)v;
                
                    imageByte  ++;
                    spriteByte ++;

                    /* blue */
                    v = mx_gameImageBuffer[ imageByte ]
                        +
                        ( mx_spriteBytes[ spriteByte ] * a ) / 255;
                    
                    if( v > 255 ) {
                        v = 255;
                        }
                    mx_gameImageBuffer[ imageByte ] = (unsigned char)v;
                
                    imageByte  ++;
                    spriteByte ++;
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
            
                unsigned char  a  =  mx_spriteBytes[ spriteByte + 3 ];

                if( drawAlphaSet ) {
                    a = (unsigned char)( ( a * mx_drawColor.comp.alpha ) / 255 );
                    }
                
                if( a == 255 ) {
                    /* RGBA bytes */
                    mx_gameImageBuffer[ imageByte  ++ ] =
                        mx_spriteBytes[ spriteByte ++ ];

                    mx_gameImageBuffer[ imageByte  ++ ] =
                        mx_spriteBytes[ spriteByte ++ ];

                    mx_gameImageBuffer[ imageByte  ++ ] =
                        mx_spriteBytes[ spriteByte ++ ];
                    }
                else {
                    /* alpha blending */

                    /* red */
                    mx_gameImageBuffer[ imageByte ] =
                        (unsigned char)( 
                            ( mx_gameImageBuffer[ imageByte ] * ( 255 - a )
                              +
                              mx_spriteBytes[ spriteByte ]    * a )
                            /
                            255 );
                
                    imageByte  ++;
                    spriteByte ++;

                    /* green */
                    mx_gameImageBuffer[ imageByte ] =
                        (unsigned char)( 
                            ( mx_gameImageBuffer[ imageByte ] * ( 255 - a )
                              +
                              mx_spriteBytes[ spriteByte ]    * a )
                            /
                            255 );
                
                    imageByte  ++;
                    spriteByte ++;

                    /* blue */
                    mx_gameImageBuffer[ imageByte ] =
                        (unsigned char)( 
                            ( mx_gameImageBuffer[ imageByte ] * ( 255 - a )
                              +
                              mx_spriteBytes[ spriteByte ]    * a )
                            /
                            255 );
                
                    imageByte  ++;
                    spriteByte ++;
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
                    inGUI->drawComponents[i].drawParams.line.startX,
                    inGUI->drawComponents[i].drawParams.line.startY,
                    inGUI->drawComponents[i].drawParams.line.endX,
                    inGUI->drawComponents[i].drawParams.line.endY );
                break;
                
            case MX_GUI_DRAW_RECT:
                maxigin_drawRect(
                    inGUI->drawComponents[i].drawParams.rect.startX,
                    inGUI->drawComponents[i].drawParams.rect.startY,
                    inGUI->drawComponents[i].drawParams.rect.endX,
                    inGUI->drawComponents[i].drawParams.rect.endY );
                break;
                
            case MX_GUI_FILL_RECT:
                maxigin_drawFillRect(
                    inGUI->drawComponents[i].drawParams.rect.startX,
                    inGUI->drawComponents[i].drawParams.rect.startY,
                    inGUI->drawComponents[i].drawParams.rect.endX,
                    inGUI->drawComponents[i].drawParams.rect.endY );
                break;
                
            case MX_GUI_DRAW_SPRITE:
                maxigin_drawSprite(
                    inGUI->drawComponents[i].drawParams.sprite.spriteHandle,
                    inGUI->drawComponents[i].drawParams.sprite.centerX,
                    inGUI->drawComponents[i].drawParams.sprite.centerY );
                break;
            case MX_GUI_DRAW_SPRITE_SEQUENCE: {

                int  s;
                int  x   =  inGUI->drawComponents[i].
                                drawParams.spriteSequence.startCenterX;
                int  y   =  inGUI->drawComponents[i].
                                drawParams.spriteSequence.startCenterY;
                
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

                v = ( x - inStartX  + 1 ) * ( inMaxValue - inMinValue )
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

            int  pos;
            int  lower;
            int  upper;
            
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

    maxigin_startGUI( &mx_internalGUI );

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

        int   oldPlaybackFrame;
        int   newPlaybackFrame;
        char  playbackSliderActive;
        
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
                    mx_playbackInterruptedRecording = 1;
                    mx_initPlayback();

                    /* jump to last step */
                    mx_playbackJumpToStep( mx_playbackTotalSteps - 1 );

                    mx_playbackDirection = -1;
                    mx_playbackSpeed     = 1;
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

    


static void mx_gameInit( void ) {

    int  p;
    
    mingin_registerButtonMapping( QUIT,
                                  mx_quitMapping );
    
    mingin_registerButtonMapping( FULLSCREEN_TOGGLE,
                                  mx_fullscreenMapping );

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
                              
    
    
    mx_areWeInMaxiginGameInitFunction = 1;
    
    maxigin_initGUI( &mx_internalGUI );
    
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

    /* write step number */
    success = mx_writeIntToPeristentData( mx_recordingDataStoreHandle,
                                          mx_totalStepsRecorded );

    if( ! success ) {
        mingin_log( "Failed to write memory diff step number in recording\n" );
        
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

    success = mx_restoreFromFullMemorySnapshot( mx_playbackDataStoreHandle );

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




/* end #ifdef MAXIGIN_IMPLEMENTATION */
#endif





/* end of #ifndef MAXIGIN_H_INCLUDED */
#endif
