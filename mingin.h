/*
  Mingin: a minimal single-header pure C89 single-player video game engine
          by Jason Rohrer
          
  This work is not copyrighted.  I place it into the public domain.

  
  ===================================================
  Table of Contents                    [jumpContents]
  ===================================================

  Jump to a section by searching for the corresponding [keyString]
  

  -- How to compile                    [jumpCompile]

  -- Why Mingin?                       [jumpWhy]

  -- How to make a Mingin game         [jumpGame]

  -- What Mingin provides              [jumpMingin]

  -- How to make a Mingin platform     [jumpPlatform]

  -- Mingin internal code              [jumpInternal]

  -- Mingin platform implementations   [jumpSystems]

     -- Linux implementation           [jumpLinux]

     -- Windows implementation         [jumpWindows]

     -- Dummy example implementation   [jumpExample]
*/



/*
  ==================================================
  How to compile                       [jumpCompile]
  ==================================================
  
  Include in your C code wherever like so:

  #include "mingin.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

  #define MINGIN_IMPLEMENTATION
  #include "mingin.h"

*/



/*
  ==============================================
  Why Mingin?                          [jumpWhy]
  ==============================================
  
  Mingin is a single C89 include file that provides the platform-specific
  infrastructure necessary to make a single-player video game.  Mingin aims to
  outlive any specific OS, SDK, library, or hardware system.  A Mingin game
  should be compilable and runnable forever, since it should be easy to get
  Mingin running on any platform.
  
  Doom is widely celebrated for its portability.  Doom will compile and run
  on your toaster.  Mingin aims to make games just as portable, if not more
  portable, than Doom.

  In order to achieve maximum portability across past and future platforms
  that support compiling C89 code, we make no assumptions about the platforms
  themselves and what features they have, beyond providing a C89 compiler.

  For example:

  --This code makes no includes that are assumed to be available on every
    platform.  Any includes present are strictly platform-specific for
    accessing operating system resources and are surrounded
    by #ifdef's that will remove them from compilation on other platforms.
    None of the C Standard Library includes are used, not even the
    "freestanding" subset.  Mingin never calls malloc, free, fopen, printf,
    assert, etc.  If you want to use these in your game, you're free to do so,
    but Mingin doesn't force your game to depend on these.

  --This code makes no assumptions about any particular metaphors or program
    structures, and works just fine on platforms without "filesystems" or
    "heap allocation" or "threads"

  --This code makes no assumptions about availability of specialized hardware
    across platforms, so it works just fine on platforms without floating
    point units or clocks or persistent storage or 3D graphics cards.

  --This code makes no assumptions about entry points (like main) that must
    be present on every platform.  Platform-specific entry points are possible.

  --The code makes no assumptions about what platform features are present,
    and Mingin can be used to run games on platforms that can only provide a
    subset of the suggested infrastructure.


    

  Single-player video games need seven things to function, which they
  can't do on their own in pure C code without the help of the underlying
  platform, in order of importance:

  1.  A way to have regular time steps pass so that game logic can update.

  2.  A way to receive input from the user (mouse, keyboard, controller,
      buttons, joystick, knob, tilt-sensor, treadmill, etc.).
  
  3.  A way to draw pixels to a rectangular window or screen.
  
  4.  A way to play a stream of audio samples to some kind of audio output.
  
  5.  A way to read bulk data resources, like graphics and sounds, that
      aren't practical to compile into the program code itself.  For example,
      a game might stream music audio data that is too big to fit in RAM.
      
  6.  A way to read and write data that persists between runs of the game,
      for saved games, settings, caches, and so on.

  7.  A way to log messages for the purposes of troubleshooting.


  
  Note that a given platform might not provide all of these things for real,
  yet the game can still function on that platform.

  For example, a platform without speakers can run a silent version of the game,
  or a platform with no persistent data store can still run a memoryless
  version of the game that starts fresh each time it is launched, or a platform
  with no bulk data device at all might compile needed bulk data resources
  directly into the program code for access by the game---remember Resource
  Forks on ancient versions of Mac OS?

  As long as regular time steps pass, the game can still function in some
  capacity.
*/




/*
  ===============================================
  How to make a Mingin game            [jumpGame]
  ===============================================
  
  The game itself must implement these four functions:
  
      void minginGame_step( char  inFinalStep );
      
      
      void minginGame_getMinimumViableScreenSize( int  *outWide,
                                                  int  *outHigh );

                                                  
      void minginGame_getScreenPixels( int             inWide,
                                       int             inHigh,
                                       unsigned char  *inRGBBuffer );

                                       
      void minginGame_getAudioSamples( int             inNumSamples,
                                       int             inNumChannels,
                                       int             inSamplesPerSecond,
                                       unsigned char  *inSampleBuffer );
  
  Each function is tagged below with   [jumpGameRequired]
  
  These are called by the engine, and the engine is in charge
  of doing any necessary color or sample format conversions with the
  data provided by the game.

  For example, this interface is in 24-bit color, so a platform that
  only supports 8-bit color must do the color conversion.  The game code
  itself can assume 24-bit color on every platform.

  Only minginGame_step is guaranteed to be called.

  The other three minginGame_ functions might not be called on every platform
  (for example, a headless test mode, or a platform without speakers).
  
  minginGame_step is guaranteed to be called at least once, before
  any other minginGame_ functions are called.
  
  The order and frequency of these function calls are not guaranteed.
  minginGame_getScreenPixels might be called after multiple calls to
  minginGame_step, or multiple times between each call to minginGame_step, etc.

  The only guarantee is that these functions will never be called concurrently.
*/




#ifndef MINGIN_H_INCLUDED
#define MINGIN_H_INCLUDED


/*
  Steps to the next time frame of the game.
  
  Note that this is the ONLY function of a game that a platform must
  actually call, so all game logic must be executed by this function.
  
  The game must not depend on being asked for screens of pixels
  or buffers of audio to advance things.

  The rate at which mingin_gameStep is called can be found by calling

      mingin_getStepsPerSecond()

  Note that there is no "init" function that the platform will call.

  A game that needs special init code can do that the first time its
  minginGame_step is called (and minginGame_step will be called at least
  once before the other minginGame_ functions are called).

  On platforms that can force the game to quit against its will,
  inFinalStep will be set to 1 on the final step call.

  Otherwise, a game is expected to do its own shutdown steps *before* calling
  mingin_quit().  minginGame_step will never be called after mingin_quit()
  has been called.

  ****
  This is the ONLY minginGame_function where you can call
  mingin-provided "mingin_" functions.
  ****
  
  Will be called at least once, and before any other minginGame_ calls.

  Parameters:
  
      inFinalStep   1 if this is the final step of the game before exit
                    0 otherwise
                    
  [jumpGameRequired]
*/
void minginGame_step( char  inFinalStep );



/*
  Gets this minimum screen size that would allow the game graphics
  to be legible.  For a pixel art game, this would be the minimum pixel
  resolution with no upscaling.  For a smoothly-scalable game, this
  might be the minimum resolution at which text/icons are legible.

  Platforms can assume that this value does not change over time, so this
  function should return the same static value every time it is called.

  Note that this minium screen size is not guaranteed by a given platform,
  and getScreenPixels may be called with smaller sizes.  However,
  the game doesn't need to produce legible output in those cases (it could
  downscale in a crude way, or crop its display, perhaps).

  On platforms that operate with a flexible window size, windows with
  interger multiples of this miniumum viable size might be used.

  ****
  Do not call mingin-provided "mingin_" functions from this function.
  ****
  
  Will not necessarily be called.

  If called, it will be called after at least one call to minginGame_step().

  Parameters:
 
      outWide   pointer to where minum viable width should be returned

      outHigh   pointer to where minum viable height should be returned
      
  [jumpGameRequired]
*/
void minginGame_getMinimumViableScreenSize( int  *outWide,
                                            int  *outHigh );



/*
  Get the next screen full of pixels
  Each R, G, and B color component is 8 bytes, interleaved in
  RGBRGBRGB... in row-major order, starting from the top left corner
  of the screen and going left-to-right and top-to-bottom.

  Note that inWide and inHigh may change from call to call and aren't
  necessarily fixed across the entire run of a game (for example,
  the game may be switched between windowed and fullscreen mode mid-run,
  and the platform may want to use a different window size after the switch).

  ****
  Do not call mingin-provided "mingin_" functions from this function.
  ****
  
  Will not necessarily be called.

  If called, it will be called after at least one call to minginGame_step().

  Parameters:

      inWide        width of the image to fill in pixels

      inHigh        height of the image to fill in pixels

      inRGBBuffer   buffer of (inWide * inHigh * 3) bytes to fill in
                    RGBRGB... row major order starting at the top left corner.
                    
  [jumpGameRequired]
*/
void minginGame_getScreenPixels( int             inWide,
                                 int             inHigh,
                                 unsigned char  *inRGBBuffer );



/*
  Get the next buffer full of audio samples.
  Samples are in Signed 16-bit little-endian, with channels interleaved

  inSampleBuffer will have inNumSampleFrames * inNumChannels * 2 bytes

  inNumSamples, inNumChannels, and inSamplesPerSecond may all change from
  call to call and aren't necessarily fixed across the entire run of the game.

  ****
  Do not call mingin-provided "mingin_" functions from this function.
  ****
  
  Will not necessarily be called.

  If called, it will be called after at least one call to minginGame_step().

  Parameters:
  
      inNumSampleFrames         number of sample frames to write to the buffer
      
      inNumChannels             number of channels per sample frame

      inSampleFramesPerSecond   number of sample frames per second (also known
                                as the sound sample rate)
                                
      inSampleBuffer            buffer of
                                (inNumSampleFrames * inNumChannels * 2) bytes
                                to fill.  Each sample is in signed 16-bit
                                little-endian PCM.  Channels in each
                                frame are interleaved.  For stereo, channels
                                are interleaved LRLRLR...
      
  [jumpGameRequired]
*/
void minginGame_getAudioSamples( int             inNumSampleFrames,
                                 int             inNumChannels,
                                 int             inSamplesPerSecond,
                                 unsigned char  *inSampleBuffer );



/*
  This is the end of the requirements for what a game must implement
  to run on Mingin.

  [jumpGameRequired]
*/






/*
  =================================================
  What Mingin provides                 [jumpMingin]
  =================================================
  
  Mingin provides these functions, which the game can call from inside
  its minginGame_step function.

  ****
  If you call these functions from outside of your minginGame_step function,
  platforms are permitted to ignore the function calls, and return unexpected
  (though safe) results.  Platforms are also permitted to write error messages
  to the log in this case.
  ****

  Each function is tagged with   [jumpMinginProvides]

  All of these functions are available on all platforms, though some of them
  might not do anything real on certain platforms.  For maximum portability,
  the game should boldly march on regardless of what these functions do.
*/



/*
  What's the step rate, in steps per second, that the platform
  is running the game at?

  Every platform will return a positive value here, even if it's not accurate.

  This value may change over time as the platform adjusts the game step time
  for any variety of reasons.

  Returns:

      the number of steps per second
      
  [jumpMinginProvides]
*/
int mingin_getStepsPerSecond( void );



/*
  How many milliseconds are left in the current step?

  Returns:

      the number of milliseconds left

          or

      -1  on platforms where this measurement is not supported.
            
  [jumpMinginProvides]
*/
int mingin_getMillisecondsLeftInStep( void );



/*
  Used to end button mapping arrays in calls to:
     mingin_registerButtonMapping
       (see below)

  [jumpMinginProvides]
*/  
#define  MGN_MAP_END  0



/*
  This enum lists all the buttons and keys that can possibly be checked
  on some potential platforms.

  These values are used in calls:
  
     mingin_registerButtonMapping
         and
     mingin_getLastButtonPressed
         (see below)

  For a game to function on as many platforms as possible in an intelligent way,
  the game should cast a wide net for each call to mingin_registerButtonMapping
  below.  Platforms will auto-map their actual controls to the symbols
  in this enum, but they won't automatically cross-map.

  For example, a platform with a weird "star button" might map that to
  MGN_BUTTON_SQUARE, but a Playstation won't auto map MGN_BUTTON_SQUARE to
  MGN_BUTTON_A.  If a game wants a particular control to be triggered by *both*
  "square" on Playstation and "A" on XBox, it needs to say so.

  Games can map MGN_ANY_KEY_OR_BUTTON if they are interested in asking about
  any button at all, like for one-button games, or for advancing through
  opening title screens.
  
  Note that not all keys are pressable, even on platforms with keyboards
  that show those symbols.
  
  For example, on US keyboards, it's impossible to hit the % key, since
  that's just the 5 key while SHIFT is held down.
  
  Platforms are generally expected to report only raw button presses and
  will not automatically map multi-key combos like SHIFT-5 to %

  [jumpMinginProvides]
*/
typedef enum MinginButton {
    MGN_BUTTON_NONE = MGN_MAP_END,
    MGN_ANY_KEY_OR_BUTTON,   /* maps to any and every key or button pressed */
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
    
    MGN_BUTTON_PS_X,
    MGN_BUTTON_PS_CIRCLE,
    MGN_BUTTON_PS_TRIANGLE,
    MGN_BUTTON_PS_SQUARE,
    MGN_BUTTON_L1,
    MGN_BUTTON_R1,
    MGN_BUTTON_L2,
    MGN_BUTTON_R2,
    MGN_BUTTON_PS_SHARE,
    MGN_BUTTON_PS_OPTIONS,
    MGN_BUTTON_PS_PS,
    MGN_BUTTON_STICK_LEFT_PRESS,
    MGN_BUTTON_STICK_RIGHT_PRESS,

    MGN_BUTTON_DPAD_LEFT,
    MGN_BUTTON_DPAD_RIGHT,
    MGN_BUTTON_DPAD_UP,
    MGN_BUTTON_DPAD_DOWN,
    
    MGN_BUTTON_XBOX_A,
    MGN_BUTTON_XBOX_B,
    MGN_BUTTON_XBOX_X,
    MGN_BUTTON_XBOX_Y,
    MGN_BUTTON_XBOX_BACK,
    MGN_BUTTON_XBOX_START,
    MGN_BUTTON_XBOX_GUIDE,
    
    MGN_BUTTON_MOUSE_LEFT,
    MGN_BUTTON_MOUSE_MIDDLE,
    MGN_BUTTON_MOUSE_RIGHT,
    MGN_DUMMY_LAST_BUTTON
    } MinginButton;


#define  MGN_NUM_BUTTONS  MGN_DUMMY_LAST_BUTTON

    

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

  The above examples could be accomplished with the following code:

      const int JUMP = 1;
      const int FIRE = 2;
  
      const MinginButton jumpMapping[] = { MGN_KEY_SPACE,
                                           MGN_BUTTON_SQUARE,
                                           MGN_BUTTON_A,
                                           MGN_MAP_END };
                                
      const MinginButton fireMapping[] = { MGN_BUTTON_MOUSE_LEFT,
                                           MGN_KEY_X,
                                           MGN_MAP_END };
                            
      mingin_registerButtonMapping( JUMP,
                                    jumpMapping );
                                    
      mingin_registerButtonMapping( FIRE,
                                    fireMapping );

  In for more complex control schemes, an enum can be used, like so:
  
      enum UserAction {
          QUIT,
          JUMP,
          FIRE,
          ROLL,
          BOMB,
          FULLSCREEN_TOGGLE };
  
  inButtonHandle can be in the range 0..255, which means 256 distinct
  user actions can be mapped to buttons.

  However, these can be remapped as many times as needed, allowing for
  modal changes to controls, giving way more than 256 user actions if needed.

  inMapping can contain at most 32 elements, including the final MGN_MAP_END

  If there are more than 31 non-END elements in a mapping, the extra ones
  will be ignored.
  
  Parameters:

      inButtonHandle   the game-defined button action to map

      inMapping        an array of MinginButton values, ending with MGN_MAP_END,
                       that should trigger this game-defined action
                       
  Returns:

      1   on success
      
      0   on failure (if inButtonHandle is out of the supported range)

  [jumpMinginProvides]
*/
char mingin_registerButtonMapping( int                 inButtonHandle,
                                   const MinginButton  inMapping[] );



/*
  Check whether a previously-mapped button handle is currently held down.

  Continuing the above examples with JUMP and FIRE, we might check these
  in our minginGame_step() function with the following code:

      if( mingin_isButtonDown( JUMP ) ) {
          ... do jump logic ...
          }
      else if( mingin_isButtonDown( FIRE ) ) {
          ... do fire logic ...
          }

  Parameters:

      inButtonHandle   the game-defined button action to check

  Returns:

      1   if pressed

      0   if not pressed.

  [jumpMinginProvides]
*/
char mingin_isButtonDown( int  inButtonHandle );



/*
  Reports the primary button for triggering a previously registered button
  handle on this platform.

  A game might map JUMP to SPACE on a keyboard, TRIANGLE on a PS4, and
  the Y button on an XBox.  This function would report which of these is
  actually available on the current platform, and if multiple are available
  (like a Linux machine with a keyboard AND a PS4 controller plugged in),
  which on is preferred.

  The results of this function can be used to display on-screen control hints
  that vary by platform, and also for control remapping screens.

  Parameters:

      inButtonHandle   the game-defined button action to check

  Returns:

      the primary assigned button   if one is available

      MGN_BUTTON_NONE               if none of the assigned buttons are
                                    available on this platform.
                        
  [jumpMinginProvides]
*/
MinginButton mingin_getPlatformPrimaryButton( int  inButtonHandle );



/*
  Gets the last button pressed before this function was called, and clears
  the memory of the last button pressed.

  This is useful for games that have control settings panels and want to let
  the user "live poke" a key or button to change an assigned button.

  In that case, this function would normally be called once to clear the memory
  (which would return whatever button was pressed last, like the mouse
  button for clicking the setting),
  and then repeatedly every minginGame_step until a value is returned.

  Returns:

      the last button pressed   since the memory has been cleared

      MGN_BUTTON_NONE           if no button has been pressed since the last
                                time the memory was cleared

  [jumpMinginProvides]
*/
MinginButton mingin_getLastButtonPressed( void );



/*
  Gets the current on-screen pointer location, if any.
  
  Pointer location spans the game screen, with (0,0) at the top left corner
  of the screen, and (outMaxX,outMaxY) at the bottom right corner of the screen.

  Parameters:

      outX      pointer to where the x location should be returned

      outY      pointer to where the y location should be returned

      outMaxX   pointer to where the max x range value should be returned

      outMaxY   pointer to where the max y range value should be returned

  Returns:

      1   if pointer location is available

      0   if not

  [jumpMinginProvides]
*/
char mingin_getPointerLocation( int  *outX,
                                int  *outY,
                                int  *outMaxX,
                                int  *outMaxY );



/*
  This enum lists all the joystick axes that can possibly be checked
  on some potential platforms.

  These values are used in call:
     mingin_registerStickAxis
       (see below)

  [jumpMinginProvides]
*/
typedef enum MinginStick {
    MGN_STICK_NONE = MGN_MAP_END,
    MGN_STICK_LEFT_X,
    MGN_STICK_LEFT_Y,
    MGN_STICK_RIGHT_X,
    MGN_STICK_RIGHT_Y,
    MGN_STICK_LEFT_TRIGGER,
    MGN_STICK_RIGHT_TRIGGER,
    MGN_STICK_DPAD_X,
    MGN_STICK_DPAD_Y,
    MGN_DUMMY_LAST_STICK
    } MinginStick;

#define MGN_NUM_STICKS  MGN_DUMMY_LAST_STICK



/*
  Similar to registering a button mapping, game can define constants as-needed
  to refer to the stick axes that it is interested in.  For example,
  it might define AIM_UP and then map it to
  { MGN_LEFT_STICK_Y, MGN_RIGHT_STICK_Y, MGN_MAP_END }

  The above examples could be accomplished with the following code:

      const int AIM_UP = 1;
  
      const MinginButton aimUpMapping[] = { MGN_LEFT_STICK_Y,
                                            MGN_RIGHT_STICK_Y,
                                            MGN_MAP_END };
                            
      mingin_registerStickAxis( AIM_UP,
                                aimUpMapping );

  inStickAxisHandle can be in the range 0..255, which means 256 distinct
  user actions can be mapped to sticks.

  However, these can be remapped as many times as needed, allowing for
  modal changes to controls, giving way more than 256 user actions if needed.

  inMapping can contain at most 32 elements, including the final MGN_MAP_END

  If there are more than 31 non-END elements in a mapping, the extra ones
  will be ignored.
  
  Parameters:

      inStickAxisHandle   the game-defined stick axis to map

      inMapping           an array of MinginStick values, ending with
                          MGN_MAP_END, that should map to the game-defined
                          stick axis.
                          
  Returns:

      1   on success
      
      0   on failure (if inStickAxisHandle is out of the supported range)
                       
  [jumpMinginProvides]
*/
char mingin_registerStickAxis( int                inStickAxisHandle,
                               const MinginStick  inMapping[] );



/*
  Checks the position of a previously-mapped joystick axis handle.

  The returned value in outPosition will be in the range
      [outLowerLimit, outUpperLimit]

  Note that if multiple mapped sticks are present on the active joystick
  (for example, AIM_UP is mapped to both MGN_STICK_LEFT_Y and MGN_STICK_RIGHT_Y,
  and both are present), the position of the stick with the largest current
  magnitude value is returned.  This allows the player to switch back and
  forth between the sticks to control aiming, for example.
  
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
                          
  [jumpMinginProvides]
*/
char mingin_getStickPosition( int   inStickAxisHandle,
                              int  *outPosition,
                              int  *outLowerLimit,
                              int  *outUpperLimit );



/*
  Toggle between fullscreen and windowed mode on platforms that support
  this.

  Note that some platforms might recall the last setting on future startups.

  Note that you can check if toggling is supported before toggling
  by calling:

      mingin_toggleFullscreen( mingin_isFullscreen() )

  Parameters:

      inFullscreen   1 to switch to full screen, 0 to switch to windowed mode

  Returns:

      1   if toggling supported

      0   if toggling not supported

  [jumpMinginProvides]
*/
char mingin_toggleFullscreen( char  inFullscreen );



/*
  Checks current fullscreen status.
  
  Returns:

      1   if fullscreen

      0   if windowed

  [jumpMinginProvides]
*/
char mingin_isFullscreen( void );



/*
  Writes a string to the log.

  Parameters:

      inString   the \0-terminated string to write to the log

  [jumpMinginProvides]
*/
void mingin_log( const char  *inString );



/*
  Exits the game program.

  Guarantees that no additional calls to minginGame_step or other minginGame_
  functions will be made.

  [jumpMinginProvides]
*/
void mingin_quit( void );



/*
  Opens a named persistent data store for writing.

  Parameters:

      inStoreName   the name of the store as a \0-terminated string.

  Returns:

      store handle   on success

      -1             on failure
  
  [jumpMinginProvides]
*/
int mingin_startWritePersistData( const char  *inStoreName );



/*
  Opens a named persistent data store for reading.

  Parameters:

      inStoreName     the name of the store as a \0-terminated string.

      outTotalBytes   pointer to where the number of bytes in the data
                      store should be returned
  Returns:

      store handle   on success

      -1             on failure
  
  [jumpMinginProvides]
*/
int mingin_startReadPersistData( const char  *inStoreName,
                                 int         *outTotalBytes );



/*
  Writes more data to an open persistent data store.

  Parameters:

      inStoreWriteHandle   handle of the store to write to

      inNumBytesToWrite    the number of bytes to write

      inByteBuffer         the buffer of bytes to write
      
  Returns:

      1   on success
      
      0   on failure
  
  [jumpMinginProvides]
*/
char mingin_writePersistData( int                   inStoreWriteHandle,
                              int                   inNumBytesToWrite,
                              const unsigned char  *inByteBuffer );



/*
  Reads more data from an open persistent data store.

  Parameters:

      inStoreReadHandle   handle of the store to read from

      inNumBytesToRead    the number of bytes to read

      inByteBuffer        the buffer where read bytes should be returned
      
  Returns:

      number of bytes read   on success

      -1                     on failure
      
      returns a non-negative number < inNumBytesToRead ONLY if we've reached
      the end of the data store and no more bytes can be read.
  
  [jumpMinginProvides]
*/
int mingin_readPersistData( int             inStoreReadHandle,
                            int             inNumBytesToRead,
                            unsigned char  *inByteBuffer );



/*
  Seeks to a byte position in a persistent data store that is being read.

  Parameters:

      inStoreReadHandle        handle of the read store to seek in

      inAbsoluteBytePosition   position relative to 0 at the start of the
                               data store.
  Returns:

      1   on success

      0   on failure
  
  [jumpMinginProvides]
*/
char mingin_seekPersistData( int  inStoreReadHandle,
                             int  inAbsoluteBytePosition );



/*
  Gets the current byte position in a data store that is being read.

  Parameters:

      inStoreReadHandle        handle of the read store to seek in

  Returns:

      position   relative to 0 at the start of the store

      -1         on failure
  
  [jumpMinginProvides]
*/
int mingin_getPersistDataPosition( int  inStoreReadHandle );



/*
  Ends writing persistent data store.
  
  Parameters:

      inStoreWriteHandle   handle of the write store to end writing
  
  [jumpMinginProvides]
*/
void mingin_endWritePersistData( int  inStoreWriteHandle );



/*
  Ends reading persistent data store.
   
  Parameters:

      inStoreReadHandle   handle of the read store to end reading 
  
  [jumpMinginProvides]
*/
void mingin_endReadPersistData( int  inStoreReadHandle );



/*
  Deletes a persistent data store by name, if it exists.

  Parameters:

      inStoreName   the name of the store to delete as a \0-terminated string.

  [jumpMinginProvides]
*/
void mingin_deletePersistData( const char  *inStoreName );



/*
  Opens a named bulk data resource for reading.

  
  Parameters:

      inBulkName      the name of the bulk resource as a \0-terminated string.

      outTotalBytes   pointer to where the number of bytes in the data
                      resource should be returned
  Returns:

      resource handle   on success

      -1                on failure
  
  [jumpMinginProvides]
*/
int mingin_startReadBulkData( const char  *inBulkName,
                              int         *outTotalBytes );



/*
  Reads more data from an open bulk data resource.

  Parameters:

      inBulkDataHandle    handle of the bulk data resource to read from

      inNumBytesToRead    the number of bytes to read

      inByteBuffer        the buffer where read bytes should be returned
      
  Returns:

      number of bytes read   on success

      -1                     on failure
      
      returns a non-negative number < inNumBytesToRead ONLY if we've reached
      the end of the bulk data resource and no more bytes can be read.
  
  [jumpMinginProvides]
*/
int mingin_readBulkData( int             inBulkDataHandle,
                         int             inNumBytesToRead,
                         unsigned char  *inByteBuffer );



/*
  Seeks to a byte position in a bulk data resource.
  
  Parameters:

      inBulkDataHandle         handle of the bulk data resource to seek in

      inAbsoluteBytePosition   position relative to 0 at the start of the
                               bulk data resource.
  Returns:

      1   on success
      
      0   on failure
  
  [jumpMinginProvides]
*/
char mingin_seekBulkData( int  inBulkDataHandle,
                          int  inAbsoluteBytePosition );



/*
  Gets the current byte position in a bulk data resource.

  Parameters:

      inStoreReadHandle   handle of the bulk resource to seek in

  Returns:

      position   relative to 0 at the start of the bulk data resource

      -1         on failure
  
  [jumpMinginProvides]
*/
int mingin_getBulkDataPosition( int  inStoreReadHandle );




/*
  Ends reading bulk data resource.
  
  Parameters:

      inBulkDataHandle   handle of the bulk data resource to end reading 
  
  [jumpMinginProvides]
*/
void mingin_endReadBulkData( int  inBulkDataHandle );



/*
  This is the end of Mingin functions that a game can call.

  [jumpMinginProvides]
*/








#ifdef MINGIN_IMPLEMENTATION


/*
  ===================================================
  How to make a Mingin platform        [jumpPlatform]
  ===================================================

  Porting Mingin to a new platform involves four aspects:

  1.  Providing a program entry point (main or whatever the platform needs).
      All platform-specific initialization can be done in this entry point,
      and the platform can also potentially use this to "run the show" in terms
      of calling the game's minginGame_step( 0 ) function in a loop
      along with the other various minginGame_ functions, etc., though some
      platforms may trigger minginGame_step on some kind of system interrupt
      callback or some other mechanism.

      
  2.  Initializing mingin itself at startup with a call to ONE function,

          minginInternal_init
  
      This is tagged below with       [jumpPlatformCalls]

      
  3.  Implementing TWO minginPlatform_ functions,

          minginPlatform_isButtonDown

          minginPlatform_getStickPosition
      
      These are tagged below with      [jumpPlatformRequired]

      
  4.  Implementing all but two of the mingin_ functions above.

      Explanation tagged below with   [jumpPlatformRequired]

      
   = Design Notes =

   
   == Button handling and callbacks ==
   
   We don't ask for a callback from the platform when any button is pressed or
   released, to support platforms that don't have button events.  Instead,
   we specifically poll for the DOWN state of a given button of interest.
   
   This works on platforms that DO have button events (since they can
   track press/release events internally and report the DOWN state when polled),
   but it also works on platforms that only support button state polling.

   This also places the platform in charge of remembering the last button
   for calls to minginPlatform_getLastButtonPressed(), if the platform supports
   that, since Mingin is not going to poll for every possible button's DOWN
   state to track this itself.

   
   == Separation of persistent data and bulk data and logging ==

   There are separate functions that must be implemented for read-only bulk
   data (like graphics and sounds) and read/write persistent data (like saved
   games).  On many plaforms, these might be implemented internally with
   the same function for reading/writing files.

   However, separating them makes it possible to support platforms where
   they really are separate (like catridges that load graphics from a ROM
   area, and keep a small flash area for saved games).

   Keeping the logging separate also allows platforms that log remotely
   through a network or devkit cable.

*/



/*
  Platform must call this function once at startup.

  [jumpPlatformCalls]
*/
static void minginInternal_init( void );



/*
  Returns 1 if a given button currently held down on the platform, 0 if not.

  If inButton is MGN_ANY_KEY_OR_BUTTON, then this function should return 1
  if any button on the platform is currently held down, and 0 if no buttons
  are held down.

  Parameters:

      inButton   the platform button to check

  Returns:

      1   if pressed

      0   if not pressed.
  
  [jumpPlatformRequired]
*/
static char minginPlatform_isButtonDown( MinginButton  inButton );



/*
  Checks the position of a joystick axis.

  The returned value in outPosition will be in the range
      [outLowerLimit, outUpperLimit]

  Parameters:

      inStick         the stick to check   

      outPosition     pointer to where the stick position should be returned

      outLowerLimit   pointer to where the lower limit for that stick
                      should be returned

      outUpperLimit   pointer to where the upper limit for that stick
                      should be returned                    
  Returns:

      1   if stick is available on the active gamepad

      0   if not
                          
  [jumpMinginProvides]
*/
char minginPlatform_getStickPosition( MinginStick   inStick,
                                      int          *outPosition,
                                      int          *outLowerLimit,
                                      int          *outUpperLimit );



/*
  NOTE:

  Platform must implement all mingin_ functions above, except for these,
  which Mingin implements internally:

    mingin_registerButtonMapping
    
    mingin_isButtonDown

    mingin_registerStickAxis
    
    mingin_getStickPosition
    
  [jumpPlatformRequired]
*/




/*
  This is the end of the requirements for what a platform must implement
  to run Mingin.

  [jumpPlatformRequired]
*/






/*
  ===================================================
  Mingin internal code                 [jumpInternal]
  ===================================================

  The following code is Minin's internal platform-independent implementation.

*/








#define MINGIN_NUM_BUTTON_MAPPINGS          256
#define MINGIN_MAX_BUTTON_MAPPING_ELEMENTS   32

static MinginButton minginButtonMappings
                        [ MINGIN_NUM_BUTTON_MAPPINGS         ]
                        [ MINGIN_MAX_BUTTON_MAPPING_ELEMENTS ];


#define MINGIN_NUM_STICK_MAPPINGS           256
#define MINGIN_MAX_STICK_MAPPING_ELEMENTS    32

static MinginStick minginStickMappings
                        [ MINGIN_NUM_STICK_MAPPINGS         ]
                        [ MINGIN_MAX_STICK_MAPPING_ELEMENTS ];



static void minginInternal_init( void ) {

    int i = 0;

    for( i = 0;
         i < MINGIN_NUM_BUTTON_MAPPINGS;
         i ++ ) {
        
        minginButtonMappings[ i ][ 0 ] = MGN_MAP_END;
        }

    for( i = 0;
         i < MINGIN_NUM_STICK_MAPPINGS;
         i ++ ) {
        
        minginStickMappings[ i ][ 0 ] = MGN_MAP_END;
        }
    
    }



char mingin_registerButtonMapping( int                 inButtonHandle,
                                   const MinginButton  inMapping[] ) {
    int i = 0;
    
    if( inButtonHandle < 0
        ||
        inButtonHandle >= MINGIN_NUM_BUTTON_MAPPINGS ) {
        
        return 0;
        }

    /* leave room at end of our internal mapping for MGN_MAP_END */
    while( i < MINGIN_MAX_BUTTON_MAPPING_ELEMENTS - 1
           &&
           inMapping[i] != MGN_MAP_END ) {

        if( inMapping[i] <= MGN_BUTTON_NONE
            ||
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



char mingin_isButtonDown( int  inButtonHandle ) {

    int i = 0;
    
    if( inButtonHandle < 0
        ||
        inButtonHandle >= MINGIN_NUM_BUTTON_MAPPINGS ) {
        
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



char mingin_registerStickAxis( int                inStickAxisHandle,
                               const MinginStick  inMapping[] ) {
    int i = 0;
    
    if( inStickAxisHandle < 0
        ||
        inStickAxisHandle >= MINGIN_NUM_STICK_MAPPINGS ) {
        
        return 0;
        }

    /* leave room at end of our internal mapping for MGN_MAP_END */
    while( i < MINGIN_MAX_STICK_MAPPING_ELEMENTS - 1
           &&
           inMapping[i] != MGN_MAP_END ) {

        if( inMapping[i] <= MGN_STICK_NONE
            ||
            inMapping[i] >= MGN_DUMMY_LAST_STICK ) {
            /* out of enum range
               end the mapping now */
            break;
            }
        minginStickMappings[ inStickAxisHandle ][i] = inMapping[i];
        i++;
        }
    
    /* terminate */
    minginStickMappings[ inStickAxisHandle ][i] = MGN_MAP_END;
    
    return 1;
    }



static int  mn_abs( int inVal ) {
    if( inVal < 0 ) {
        inVal = - inVal;
        }
    return inVal;
    }



char mingin_getStickPosition( int   inStickAxisHandle,
                              int  *outPosition,
                              int  *outLowerLimit,
                              int  *outUpperLimit ) {

    int  i                =   0;
    int  maxPosMagnitude  =  -1;
    int  maxPosIndex      =  -1;
    
    if( inStickAxisHandle < 0
        ||
        inStickAxisHandle >= MINGIN_NUM_STICK_MAPPINGS ) {
        
        return 0;
        }

    /* find present mapped stick axis with largest magnitude position */
    while( minginStickMappings[ inStickAxisHandle ][i] != MGN_MAP_END ) {

        int   pos;
        int   lowerLimit;
        int   upperLimit;
        char  present;

        present = minginPlatform_getStickPosition(
                      minginStickMappings[ inStickAxisHandle ][ i ],
                      & pos,
                      & lowerLimit,
                      & upperLimit );

        if( present ) {
            int  absPos = mn_abs( pos );

            if( absPos > maxPosMagnitude ) {
                
                maxPosMagnitude = absPos;
                maxPosIndex     = i;
                }
            }
        i++;
        }

    if( maxPosIndex == -1 ) {
        return 0;
        }

    /* now that we've found the stick with the maximum magnitude,
       return its values */
    
    minginPlatform_getStickPosition(
        minginStickMappings[ inStickAxisHandle ][ maxPosIndex ],
        outPosition,
        outLowerLimit,
        outUpperLimit );
    
    return 1;
    }


/*
  This is the end of Mingin's internal code.

  [jumpInternal]
*/




/*
  ==================================================
  Mingin platform implementations      [jumpSystems]
  ==================================================

  The following #ifdefs selectively compile platform-specific implmentation
  code.

*/




#ifdef __linux__


/*
  ================================================
  Linux implementation                 [jumpLinux]
  ================================================
  
  Linux platform with X11 windows
  Supports only 24-bit color
*/




#define MINGIN_LINUX_MAX_WIN_W   4096
#define MINGIN_LINUX_MAX_WIN_H   2160


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <X11/extensions/Xrandr.h>

#include <GL/glx.h>

#include <unistd.h>
#include <errno.h>
#include <sys/time.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>



/* game's expected buffer is RGB */
static unsigned char mn_gameScreenBuffer[ MINGIN_LINUX_MAX_WIN_W *
                                          MINGIN_LINUX_MAX_WIN_H * 3 ];

static  char            mn_shouldQuit           =  0;
static  int             mn_windowW              =  0;
static  int             mn_windowH              =  0;
static  char            mn_areWeInStepFunction  =  0;
static  char            mn_xFullscreen          =  0;
static  int             mn_screenRefreshRate    =  0;
static  struct timeval  mn_lastRedrawTime;


static void mn_getMonitorSize( Display  *inXDisplay,
                               int      *outW,
                               int      *outH ) {
    
    Window             win      =  DefaultRootWindow( inXDisplay );
    XWindowAttributes  winAttr;
    
    XGetWindowAttributes( inXDisplay, win, &winAttr );

    *outW = winAttr.width;
    *outH = winAttr.height;
    }



int mingin_getStepsPerSecond( void ) {
    return mn_screenRefreshRate;
    }



int mingin_getMillisecondsLeftInStep( void ) {

    static  struct timeval  currentTime;

    int  msPassed;
    int  msPerStep  =  1000 / mn_screenRefreshRate;
    
    gettimeofday( & currentTime,
                  NULL );

    msPassed =
        (int)(
            1000 * ( currentTime.tv_sec - mn_lastRedrawTime.tv_sec )
            +
            ( currentTime.tv_usec - mn_lastRedrawTime.tv_usec ) / 1000 );


    if( msPassed > msPerStep ) {
        return 0;
        }
    else {
        return msPerStep - msPassed;
        }
    }



void mingin_quit( void ) {
    if( ! mn_areWeInStepFunction ) {
        mingin_log( "Error:  calling mingin_quit from "
                    "outside minginGame_step function\n" );
        return;
        }
    mn_shouldQuit = 1;
    }



char mingin_toggleFullscreen( char  inFullscreen ) {
    if( ! mn_areWeInStepFunction ) {
        mingin_log( "Error:  calling mingin_toggleFullscreen from "
                    "outside minginGame_step function\n" );
        return 1;
        }
    mn_xFullscreen = inFullscreen;
    return 1;
    }



char mingin_isFullscreen( void ) {
    return mn_xFullscreen;
    }

    

/* memory for getLastButtonPressed call */
static  MinginButton  mn_lastButtonPressed  =  MGN_BUTTON_NONE;

/* status tracking pressed/released state */
static  char          mn_buttonDown[ MGN_NUM_BUTTONS ];

/* maps each Mingin key to an X11 XK_ keysym */
static  KeySym        mn_buttonToXKeyMap[ MGN_NUM_BUTTONS ];




#define  MGN_FIRST_GAMEPAD   0

typedef enum MinginGamepad {
    MGN_NO_GAMEPAD          =  -1,
    MGN_PS_DUALSHOCK_4      =  MGN_FIRST_GAMEPAD,
    MGN_XBOX_360,
    MGN_NUM_GAMEPADS
    } MinginGamepad;


static  const char   *mn_gamepadIDStrings[ MGN_NUM_GAMEPADS ] = {
    "Sony Interactive Entertainment Wireless Controller",
    "Microsoft X-Box 360 pad"
    };


#define  MINGIN_MAX_NUM_GAMEPAD_BUTTONS  32

#define  MINGIN_MAX_NUM_GAMEPAD_STICKS   32


/*
  Maps /dev/input/js  button numbers (0, 1, 2, etc.) to MGN_BUTTON_ symbols
  for a specific gamepad.
   
  Each list of buttons is padded with MGN_MAP_END
*/
static  MinginButton        mn_jsButtonToButtonMap
                                [ MGN_NUM_GAMEPADS ]
                                [ MINGIN_MAX_NUM_GAMEPAD_BUTTONS ];


/*
  Maps /dev/input/js  stick numbers (0, 1, 2, etc.) to MGN_BUTTON_ symbols
  becaus some "sticks", like D-pads, are actually behaving line binary buttons
  on certain gamepads.

  2 indices at end are 0 for negative direction and 1 for positive direction
  on the stick that maps to binary button presses
   
  Each list of buttons is padded with MGN_MAP_END
*/
static  MinginButton        mn_jsStickToButtonMap
                                [ MGN_NUM_GAMEPADS ]
                                [ MINGIN_MAX_NUM_GAMEPAD_BUTTONS ]
                                [ 2 ];


/*
  Maps /dev/input/js  stick numbers (0, 1, 2, etc.) to MGN_STICK_ symbols
  for a specific gamepad.
   
  Each list of sticks is padded with MGN_MAP_END
*/
static  MinginStick         mn_jsStickToStickMap
                                [ MGN_NUM_GAMEPADS ]
                                [ MINGIN_MAX_NUM_GAMEPAD_STICKS ];

/*
  Which sticks are present on each gamepad.
*/
static  char                mn_stickPresent
                                [ MGN_NUM_GAMEPADS ]
                                [ MGN_NUM_STICKS ];

/*
  Tracks range of each stick for each gamepad.

  2 indices at end are 0 for lower limit and 1 for upper limit.

  If a gamepad doesn't have a stick, both values will be 0.
*/
static  int                 mn_stickRange
                                [ MGN_NUM_GAMEPADS ]
                                [ MGN_NUM_STICKS ]
                                [ 2 ];




static  MinginGamepad  mn_activeGamepad  =  MGN_NO_GAMEPAD;


/*
  Tracks each stick position for our active gamepad.

  If active gamepad doesn't have this stick, value will be 0.
*/
static  int                 mn_stickPosition
                                [ MGN_NUM_STICKS ];



/*
  Looks through  /dev/input/js  devices for the first gamepad that matches
  one of our mn_gamepadIDStrings.

  Side Effects:

      mn_activeGamepad   set to an index in MinginGamepad if found
      
      mn_activeGamepad   set to MGN_NO_GAMEPAD of not found

  Returns:

      file descriptor   for open gamepad if found

      -1                if not found
*/
static int mn_openActiveGamepad( void );




static char minginPlatform_isButtonDown( MinginButton  inButton ) {
    
    if( inButton <= MGN_BUTTON_NONE
        ||
        inButton >= MGN_DUMMY_LAST_BUTTON ) {
        return 0;
        }
    
    if( mn_buttonDown[ inButton ] ) {
        return 1;
        }

    if( inButton == MGN_ANY_KEY_OR_BUTTON ) {
        /* loop through entire list and see if anything is currently down */

        int i;

        for( i = 0;
             i < MGN_NUM_BUTTONS;
             i ++ ) {
            
            if( mn_buttonDown[i] ) {
                return 1;
                }
            }
        }
    
    return 0;
    }



char minginPlatform_getStickPosition( MinginStick   inStick,
                                      int          *outPosition,
                                      int          *outLowerLimit,
                                      int          *outUpperLimit ) {

    if( mn_activeGamepad == MGN_NO_GAMEPAD ) {
        return 0;
        }

    if( ! mn_stickPresent[ mn_activeGamepad ][ inStick ] ) {
        return 0;
        }

    *outPosition    =  mn_stickPosition[ inStick ];

    *outLowerLimit  =  mn_stickRange[ mn_activeGamepad ][ inStick ][ 0 ];
    *outUpperLimit  =  mn_stickRange[ mn_activeGamepad ][ inStick ][ 1 ];

    return 1;
    }



static char mn_doesActiveGamepadHaveButton( MinginButton inButton ) {

    int i;
        
    if( mn_activeGamepad == MGN_NO_GAMEPAD ) {
        return 0;
        }

    i = 0;

    while( mn_jsButtonToButtonMap[ mn_activeGamepad ][ i ]
           !=
           MGN_MAP_END ) {

        if( mn_jsButtonToButtonMap[ mn_activeGamepad ][ i ] == inButton ) {
            return 1;
            }
        i ++;
        }
    
    i = 0;

    while( mn_jsStickToButtonMap[ mn_activeGamepad ][ i ][ 0 ]
           !=
           MGN_MAP_END ) {

        if( mn_jsStickToButtonMap[ mn_activeGamepad ][ i ][ 0 ] == inButton ) {
            return 1;
            }
        i ++;
        }

    i = 0;
    
    while( mn_jsStickToButtonMap[ mn_activeGamepad ][ i ][ 1 ]
           !=
           MGN_MAP_END ) {

        if( mn_jsStickToButtonMap[ mn_activeGamepad ][ i ][ 1 ] == inButton ) {
            return 1;
            }
        i ++;
        }
    
    return 0;
    }



MinginButton mingin_getPlatformPrimaryButton( int inButtonHandle ) {

    int i;
    
    if( mn_activeGamepad != MGN_NO_GAMEPAD ) {
        /* gamepad active */

        i = 0;

        while( minginButtonMappings[ inButtonHandle ][ i ]
               !=
               MGN_MAP_END ) {
            
            MinginButton  b  =  minginButtonMappings[ inButtonHandle ][ i ];

            if( b > MGN_KEY_TILDE
                &&
                b < MGN_BUTTON_MOUSE_LEFT ) {

                /* gamepad buttons are above keyboard and below
                   mouse in our button enum */
            
                if( mn_doesActiveGamepadHaveButton( b ) ) {
                    return b;
                    }
                }         
            i ++;
            }
        }

    /* didn't find a mapped gamepad button, or gamepad not active
       consider keyboard */

    i = 0;

    while( minginButtonMappings[ inButtonHandle ][ i ]
           !=
           MGN_MAP_END ) {
        
        MinginButton  b  =  minginButtonMappings[ inButtonHandle ][ i ];

        if( ( b >= MGN_KEY_BACKSPACE
              &&
              b <= MGN_KEY_TILDE )
            ||
            ( b >= MGN_BUTTON_MOUSE_LEFT
              &&
              b <= MGN_BUTTON_MOUSE_RIGHT ) ) {

            /* all keys are possible on Linux keyboard, along with
               all mouse buttons.

               return first mapped key or mouse button found */

            return b;
            }
        i++;
        }

    return MGN_BUTTON_NONE;
    }



static MinginButton mn_mapXKeyToButton( KeySym  inXKey ) {

    int i;

    for( i = 0;
         i < MGN_NUM_BUTTONS;
         i ++ ) {
        
        if( mn_buttonToXKeyMap[ i ] == inXKey ) {
            return i;
            }
        }
    
    return MGN_BUTTON_NONE;
    }



static MinginButton mn_mapXButtonToButton( unsigned int inButton ) {

    switch( inButton ) {
        case 1:
            return MGN_BUTTON_MOUSE_LEFT;
            break;
        case 2:
            return MGN_BUTTON_MOUSE_MIDDLE;
            break;
        case 3:
            return MGN_BUTTON_MOUSE_RIGHT;
            break;  
        }

    return MGN_BUTTON_NONE;
    }



/*
  Maps button index read from a /dev/input/js event to a MinginButton,
  based on the current active gamepad.

  Returns MGN_BUTTON_NONE on failure.
*/
static MinginButton mn_mapJSButtonToButton( int inJSButton ) {
    
    MinginButton  b;

    if( inJSButton >= MINGIN_MAX_NUM_GAMEPAD_BUTTONS ) {
        return MGN_BUTTON_NONE;
        }
    
    if( mn_activeGamepad == MGN_NO_GAMEPAD ) {
        return MGN_BUTTON_NONE;
        }

    b = mn_jsButtonToButtonMap[ mn_activeGamepad ][ inJSButton ];

    if( b == MGN_MAP_END ) {
        return MGN_BUTTON_NONE;
        }

    return b;
    }



/*
  Maps stick index and position read from a /dev/input/js event to a
  MinginButton, based on the current active gamepad.

  (Some "sticks" are actually binary on/off buttons, like the d-pad)

  Returns MGN_BUTTON_NONE on failure.
*/
static MinginButton mn_mapJSStickToButton( int inJSButton,
                                           int inStickPosition ) {
    
    MinginButton  b;
    int           posIndex;
    
    if( inJSButton >= MINGIN_MAX_NUM_GAMEPAD_STICKS
        ||
        inJSButton < 0  ) {
        
        return MGN_BUTTON_NONE;
        }

    
    if( mn_activeGamepad == MGN_NO_GAMEPAD ) {
        return MGN_BUTTON_NONE;
        }

    if( inStickPosition < 0 ) {
        posIndex = 0;
        }
    else {
        posIndex = 1;
        }
    
    b = mn_jsStickToButtonMap[ mn_activeGamepad ][ inJSButton ][ posIndex ];

    if( b == MGN_MAP_END ) {
        return MGN_BUTTON_NONE;
        }

    return b;
    }



/*
  Maps a stick index position from /dev/input/js event and records
  that stick position in the appropriate slot in mn_stickPosition
  for the active gamepad.
*/
static void mn_registerJSStickPosition( int inJSButton,
                                        int inStickPosition ) {
    
    MinginStick  s;
    
    if( inJSButton >= MINGIN_MAX_NUM_GAMEPAD_STICKS
        ||
        inJSButton < 0 ) {
        
        return;
        }
    
    if( mn_activeGamepad == MGN_NO_GAMEPAD ) {
        return;
        }

    s = mn_jsStickToStickMap[ mn_activeGamepad ][ inJSButton ];

    
    if( s == MGN_MAP_END ) {
        return;
        }

    mn_stickPosition[ s ] = inStickPosition;
    }
    



MinginButton mingin_getLastButtonPressed( void ) {
    
    MinginButton last = mn_lastButtonPressed;
    
    mn_lastButtonPressed = MGN_BUTTON_NONE;
    return last;
    }



static int mn_stringLength( const char  *inString ) {
    
    int i = 0;
    
    while( inString[i] != '\0' ) {
        i++;
        }
    
    return i;
    }



static char mn_stringsEqual( const char  *inStringA,
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
        

void mingin_log( const char  *inString ) {
    write( STDOUT_FILENO,
           inString,
           (unsigned int)mn_stringLength( inString ) );
    }




/*
  Returns a static buffer that must be used before next call to mn_intToString
*/
static const char *mn_intToString( int  inInt ) {
    
    static  char           buffer[20];
            const char    *formatError  =  "[int_format_error]";
            unsigned int   c            =  0;         
            int            divisor      =  1000000000;  /* start w/ billions */
            int            qLowerLimit  =  1;           /* skip 0 digits until
                                                           our first non-zero
                                                           digit */
    if( inInt == 0 ) {
        return "0";
        }
    if( inInt < 0 ) {
        buffer[c] = '-';
        c++;
        inInt *= -1;
        }
    while( divisor >= 1 ) {
        
        int q = inInt / divisor;
        
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



static void mn_setupX11KeyMap( void );


static void mn_setupLinuxGamepadMaps( void );


/*
  Maps an id string to an index in MinginGamepad, or -1 if no match
*/
static int mn_getGamepadIndex( const char  *inIDString );




static void mn_xSetFullscreen( Display  *inXDisplay,
                               Window    inXWindow,
                               char      inToggle ) {
    XEvent  ev;
    Atom    atom;

    ev.type                  =  ClientMessage;
    ev.xclient.window        =  inXWindow;
    ev.xclient.message_type  =  XInternAtom( inXDisplay,
                                             "_NET_WM_STATE",
                                             False );
    ev.xclient.format        =  32;
    ev.xclient.data.l[0]     =  inToggle;
    atom                     =  XInternAtom( inXDisplay,
                                             "_NET_WM_STATE_FULLSCREEN",
                                             False );
    ev.xclient.data.l[1]     =  (long int)atom;
    ev.xclient.data.l[2]     =  (long int)atom;
    
    XSendEvent( inXDisplay,
                DefaultRootWindow( inXDisplay ),
                False,
                ClientMessage,
                &ev );
    }




/*
  reconfigures based on mn_xFullscreen and game's minimum viable screen size
*/
static void mn_reconfigureWindowSize( Display  *inXDisplay ) {
    
    if( mn_xFullscreen ) {
        mn_getMonitorSize( inXDisplay,
                           &mn_windowW,
                           &mn_windowH );
        }
    else {
        
        int  monW;
        int  monH;
        int  gameW;
        int  gameH;
        int  smallestMult;
        
        mn_getMonitorSize( inXDisplay,
                           &monW,
                           &monH );
        
        minginGame_getMinimumViableScreenSize( &gameW,
                                               &gameH );

        if( monW < gameW
            ||
            monH < gameH ) {
            /* physical monitor too small for game
               have window fill monitor */
            mn_windowW = monW;
            mn_windowH = monH;
            }
        else {
            /* monitor big enough for game */
            int  wMult  =  monW / gameW;
            int  hMult  =  monH / gameH;

            smallestMult = wMult;
            
            if( hMult < smallestMult ) {
                smallestMult = hMult;
                }
            
            mn_windowW = smallestMult * gameW;
            mn_windowH = smallestMult * gameH;

            while( smallestMult > 1
                   &&
                   ( mn_windowW > ( 9 * monW ) / 10
                     ||
                     mn_windowH > ( 9 * monH ) / 10 ) ) {

                /* window filling more than 90% of monitor, too big */
                smallestMult --;
                
                mn_windowW = smallestMult * gameW;
                mn_windowH = smallestMult * gameH;
                }
            }
        }
    
    /* make sure we're never bigger than our statically allocated
       framebuffer */
    if( mn_windowW > MINGIN_LINUX_MAX_WIN_W ) {
        mn_windowW = MINGIN_LINUX_MAX_WIN_W;
        }
    if( mn_windowH > MINGIN_LINUX_MAX_WIN_H ) {
        mn_windowH = MINGIN_LINUX_MAX_WIN_H;
        }
    mingin_log( "Window = " );
    mingin_log( mn_intToString( mn_windowW ) );
    mingin_log( "," );
    mingin_log( mn_intToString( mn_windowH ) );
    mingin_log( "\n" );
    }



typedef struct MinginXWindowSetup {
        
        Display      *xDisplay;
        Window        xWindow;
        Window        xRoot;
        int           xScreen;
        XVisualInfo  *xVisual;
        GLXContext    glxContext;
        
    } MinginXWindowSetup;



static  char                 mn_XSetupLive  =  0;
static  MinginXWindowSetup   mn_XSetup;



/* returns 1 on success, 0 on failure */
static char mn_openXWindow( MinginXWindowSetup  *inSetup ) {
    
    MinginXWindowSetup      *s                =  inSetup;
    int                      glxAttributes[]  = { GLX_RGBA,
                                                  GLX_DOUBLEBUFFER,
                                                  None };
    unsigned long            xBlackColor;
    XRRScreenConfiguration  *conf;
        
    s->xDisplay = XOpenDisplay( NULL );

    mn_reconfigureWindowSize( s->xDisplay );

    
    s->xRoot = DefaultRootWindow( s->xDisplay );

    conf = XRRGetScreenInfo( s->xDisplay, s->xRoot );
    mn_screenRefreshRate = XRRConfigCurrentRate( conf );

    mingin_log( "Found monitor refresh reate " );
    mingin_log( mn_intToString( mn_screenRefreshRate ) );
    mingin_log( "\n" );
    
    s->xScreen = DefaultScreen( s->xDisplay );

    
    s->xVisual = glXChooseVisual( s->xDisplay,
                                  s->xScreen,
                                  glxAttributes );

    if( ! s->xVisual ) {
        mingin_log( "No Visual found for GLX\n" );
        XCloseDisplay( s->xDisplay );
        return 0;
        }

    xBlackColor = BlackPixel( s->xDisplay,
                              s->xScreen );
    

    s->xWindow = XCreateSimpleWindow(
        s->xDisplay,
        s->xRoot,
        0, 0,
        (unsigned int)mn_windowW,
        (unsigned int)mn_windowH,
        0,
        xBlackColor,
        xBlackColor );

    
    XSelectInput( s->xDisplay,
                  s->xWindow,
                  StructureNotifyMask |
                  KeyPressMask        |
                  KeyReleaseMask      |
                  ButtonPressMask     |
                  ButtonReleaseMask );

    XMapWindow( s->xDisplay,
                s->xWindow );

    s->glxContext = glXCreateContext( s->xDisplay,
                                      s->xVisual,
                                      NULL,
                                      GL_TRUE );
    
    if( ! s->glxContext ) {
        mingin_log( "Failed to create GLX context\n" );
        XDestroyWindow( s->xDisplay,
                        s->xWindow );
        XFree( s->xVisual );
        XCloseDisplay( s->xDisplay );
        
        return 0;
        }


    /* wait for MapNotify */
    while( 1 ) {
        XEvent e;
        XNextEvent( s->xDisplay,
                    &e);
        if( e.type == MapNotify ) {
            break;
            }
        }

    
    mn_xSetFullscreen( s->xDisplay,
                       s->xWindow,
                       mn_xFullscreen );

    
    glXMakeCurrent( s->xDisplay,
                    s->xWindow,
                    s->glxContext );

    return 1;
    }



static void mn_closeXWindow( MinginXWindowSetup  *inSetup ) {
    
    MinginXWindowSetup  *s  =  inSetup;

    XFree( s->xVisual );
    
    glXMakeCurrent( s->xDisplay,
                    None,
                    NULL );

    glXDestroyContext( s->xDisplay,
                       s->glxContext );

    
    XDestroyWindow( s->xDisplay,
                    s->xWindow );

    XCloseDisplay( s->xDisplay );
    }



char mingin_getPointerLocation( int  *outX,
                                int  *outY,
                                int  *outMaxX,
                                int  *outMaxY ) {

    Window        currentRoot;
    Window        currentChild;
    int           rootX;
    int           rootY;
    int           winX;
    int           winY;
    unsigned int  mask;
    Bool          result;
    
    if( ! mn_XSetupLive ) {
        return 0;
        }
    
    result = XQueryPointer( mn_XSetup.xDisplay,
                            mn_XSetup.xWindow,
                            & currentRoot,
                            & currentChild,
                            & rootX,
                            & rootY,
                            & winX,
                            & winY,
                            & mask );

    if( result != True ) {
        return 0;
        }
        
    if( currentRoot != mn_XSetup.xRoot ) {
        return 0;
        }
    
    if( winX < 0
        ||
        winX > mn_windowW
        ||
        winY < 0
        ||
        winY > mn_windowW ) {

        return 0;
        }

    *outX    = winX;
    *outY    = winY;
    *outMaxX = mn_windowW;
    *outMaxY = mn_windowH;

    return 1;
    }



        
int main( void ) {

    int   b;
    char  currentlyFullscreen  =  0;
    int   gamepadFD            =  -1;
    
    mn_xFullscreen = currentlyFullscreen;

    mingin_log( "Linux mingin platform starting up\n" );
    
    minginInternal_init();

    for( b = 0;
         b < MGN_NUM_BUTTONS;
         b ++ ) {
        
        mn_buttonDown[b] = 0;
        mn_buttonToXKeyMap[b] = 0;
        }

    mn_setupX11KeyMap();

    mn_setupLinuxGamepadMaps();
    

    if( ! mn_openXWindow( & mn_XSetup ) ) {
        mingin_log( "Opening X Window failed\n" );
        return 1;
        }

    mn_XSetupLive = 1;

    
    gamepadFD = mn_openActiveGamepad();

    
    while( ! mn_shouldQuit ) {
        
        /* pump all X11 events */
        while( XPending( mn_XSetup.xDisplay ) > 0 ) {
            
            XEvent  e;
            
            XNextEvent( mn_XSetup.xDisplay,
                        &e );

            if( e.type == KeyPress ) {
                KeySym        ks      =  XLookupKeysym( &( e.xkey ),
                                                        0 );
                MinginButton  button  =  mn_mapXKeyToButton( ks );
                
                if( button > MGN_BUTTON_NONE ) {
                    mn_buttonDown[ button ] = 1;
                    /* a new press to remember */
                    mn_lastButtonPressed = button;
                    }
                }
            else if( e.type == KeyRelease ) {
                KeySym        ks      =  XLookupKeysym( &(e.xkey),
                                                        0 );
                MinginButton  button  =  mn_mapXKeyToButton( ks );
                
                if( button > MGN_BUTTON_NONE ) {
                    mn_buttonDown[ button ] = 0;
                    }
                }
            else if( e.type == ButtonPress ) {
                MinginButton  button  =  mn_mapXButtonToButton(
                                             e.xbutton.button );
                
                if( button > MGN_BUTTON_NONE ) {
                    mn_buttonDown[ button ] = 1;
                    /* a new press to remember */
                    mn_lastButtonPressed = button;
                    }
                }
            else if( e.type == ButtonRelease ) {
                MinginButton  button  =  mn_mapXButtonToButton(
                                             e.xbutton.button );
                
                if( button > MGN_BUTTON_NONE ) {
                    mn_buttonDown[ button ] = 0;
                    }
                }
            }

        /* pump all events for open joystick */
        if( gamepadFD != -1 ) {

            char             readMore  =  1;
            ssize_t          numRead;
            struct js_event  e;
            
            while( readMore ) {
                numRead = read( gamepadFD,
                                & e,
                                sizeof( struct js_event ) );
                
                if( numRead == sizeof( struct js_event ) ) {

                    MinginButton  button;

                    /* for stick axes that act like binary button
                       presses, the axis will return 0 when either
                       directional button is released, so we need
                       to release both of them */
                    MinginButton  buttonMinus;
                    MinginButton  buttonPlus;
                    
                    switch (e.type) {
                        case JS_EVENT_BUTTON:
                            button = mn_mapJSButtonToButton( e.number );

                            if( button > MGN_BUTTON_NONE ) {
                                if( e.value ) {
                                    mn_buttonDown[ button ] = 1;
                                    /* a new press to remember */
                                    mn_lastButtonPressed = button;
                                    }
                                else {
                                    /* release */
                                    mn_buttonDown[ button ] = 0;
                                    }
                                }
                            break;
                        case JS_EVENT_AXIS:
                            if( e.value == 0 ) {
                                /* might be a release of a stick
                                   that behaves like a binary button */
                                buttonPlus = mn_mapJSStickToButton( e.number,
                                                                    1 );
                                buttonMinus = mn_mapJSStickToButton( e.number,
                                                                     -1 );

                                if( buttonPlus > MGN_BUTTON_NONE ) {
                                    /* release */
                                    mn_buttonDown[ buttonPlus ] = 0;
                                    }
                                if( buttonMinus > MGN_BUTTON_NONE ) {
                                    /* release */
                                    mn_buttonDown[ buttonMinus ] = 0;
                                    }
                                }
                            else {
                                button = mn_mapJSStickToButton( e.number,
                                                                e.value );
                                /* negate the value to see
                                   opposite side that might need to be
                                   released */
                                buttonMinus = mn_mapJSStickToButton( e.number,
                                                                     -e.value );

                                if( button > MGN_BUTTON_NONE ) {
                                    mn_buttonDown[ button ] = 1;
                                    /* a new press to remember */
                                    mn_lastButtonPressed = button;
                                    }
                                if( buttonMinus > MGN_BUTTON_NONE ) {
                                    /* release opposite side */
                                    mn_buttonDown[ buttonMinus ] = 0;
                                    }
                                }
                            
                            mn_registerJSStickPosition( e.number,
                                                        e.value );

                            break;
                        }
                    }
                else if( numRead == -1 ) {
                    readMore = 0;
                    
                    if( errno == EAGAIN
                        ||
                        errno == EINTR
                        ||
                        errno == EWOULDBLOCK ) {

                        
               
                        /* no event ready, we've read all available */
                        }
                    else {
                        /* error on reading... maybe joystick unplugged */
                        close( gamepadFD );
                        
                        /* try to re-open, maybe switch joysticks */
                        gamepadFD = mn_openActiveGamepad();
                        }
                    }
                }
            }
        
            

        mn_areWeInStepFunction = 1;

        minginGame_step( 0 );

        mn_areWeInStepFunction = 0;
        
        
        if( mn_shouldQuit ) {
            /* mingin_quit called in the most recent step,
               so we can't call any minginGame_ functions again */
            break;
            }
        
        
        minginGame_getScreenPixels( mn_windowW,
                                    mn_windowH,
                                    mn_gameScreenBuffer );

        glRasterPos2f( -1, 1 );
        
        glPixelZoom( 1, -1 );
 
        glDrawPixels( (GLsizei)mn_windowW,
                      (GLsizei)mn_windowH,
                      GL_RGB,
                      GL_UNSIGNED_BYTE,
                      mn_gameScreenBuffer );

        glXSwapBuffers( mn_XSetup.xDisplay,
                        mn_XSetup.xWindow ); 

        if( currentlyFullscreen != mn_xFullscreen ) {
            int  oldW  =  mn_windowW;
            int  oldH  =  mn_windowH;

            mn_reconfigureWindowSize( mn_XSetup.xDisplay );

            if( oldW != mn_windowW
                ||
                oldH != mn_windowH ) {
                /* need to destroy and re-make window */

                mingin_log( "Calling resize window with " );
                mingin_log( mn_intToString( mn_windowW ) );
                mingin_log( "," );
                mingin_log( mn_intToString( mn_windowH ) );
                mingin_log( "\n" );

                mn_closeXWindow( & mn_XSetup );

                if( ! mn_openXWindow( & mn_XSetup ) ) {
                    mingin_log( "Failed to re-open X Window after toggling "
                                "fullscreen mode\n" );
                    return 1;
                    }
                }
            else {
                /* same window size after fullscreen toggle,
                   no need to remake it */
                mn_xSetFullscreen( mn_XSetup.xDisplay,
                                   mn_XSetup.xWindow,
                                   mn_xFullscreen );
                }


            
            currentlyFullscreen = mn_xFullscreen;

            }

        /* do this after resizing window (if we do that)
           to give us a fresh start on our next step timing */
        gettimeofday( & mn_lastRedrawTime,
                      NULL );
        
        } /* end of  while( ! mn_shouldQuit )  */

    
    mn_closeXWindow( & mn_XSetup );

    if( gamepadFD != -1 ) {
        close( gamepadFD );
        gamepadFD = -1;
        }
    
    return 1;
    }



static void mn_setupX11KeyMap( void ) {
    mn_buttonToXKeyMap[ MGN_KEY_BACKSPACE ]     =  XK_BackSpace;
    mn_buttonToXKeyMap[ MGN_KEY_TAB ]           =  XK_Tab;
    mn_buttonToXKeyMap[ MGN_KEY_RETURN ]        =  XK_Return;
    mn_buttonToXKeyMap[ MGN_KEY_ESCAPE ]        =  XK_Escape;
    mn_buttonToXKeyMap[ MGN_KEY_DELETE ]        =  XK_Delete;
    mn_buttonToXKeyMap[ MGN_KEY_HOME ]          =  XK_Home;
    mn_buttonToXKeyMap[ MGN_KEY_LEFT ]          =  XK_Left;
    mn_buttonToXKeyMap[ MGN_KEY_UP ]            =  XK_Up;
    mn_buttonToXKeyMap[ MGN_KEY_RIGHT ]         =  XK_Right;
    mn_buttonToXKeyMap[ MGN_KEY_DOWN ]          =  XK_Down;
    mn_buttonToXKeyMap[ MGN_KEY_PAGE_UP ]       =  XK_Page_Up;
    mn_buttonToXKeyMap[ MGN_KEY_PAGE_DOWN ]     =  XK_Page_Down;
    mn_buttonToXKeyMap[ MGN_KEY_END ]           =  XK_End;
    mn_buttonToXKeyMap[ MGN_KEY_NUM_LOCK ]      =  XK_Num_Lock;
    mn_buttonToXKeyMap[ MGN_KEY_F1 ]            =  XK_F1;
    mn_buttonToXKeyMap[ MGN_KEY_F2 ]            =  XK_F2;
    mn_buttonToXKeyMap[ MGN_KEY_F3 ]            =  XK_F3;
    mn_buttonToXKeyMap[ MGN_KEY_F4 ]            =  XK_F4;
    mn_buttonToXKeyMap[ MGN_KEY_F5 ]            =  XK_F5;
    mn_buttonToXKeyMap[ MGN_KEY_F6 ]            =  XK_F6;
    mn_buttonToXKeyMap[ MGN_KEY_F7 ]            =  XK_F7;
    mn_buttonToXKeyMap[ MGN_KEY_F8 ]            =  XK_F8;
    mn_buttonToXKeyMap[ MGN_KEY_F9 ]            =  XK_F9;
    mn_buttonToXKeyMap[ MGN_KEY_F10 ]           =  XK_F10;
    mn_buttonToXKeyMap[ MGN_KEY_F11 ]           =  XK_F11;
    mn_buttonToXKeyMap[ MGN_KEY_F12 ]           =  XK_F12;
    mn_buttonToXKeyMap[ MGN_KEY_F13 ]           =  XK_F13;
    mn_buttonToXKeyMap[ MGN_KEY_F14 ]           =  XK_F14;
    mn_buttonToXKeyMap[ MGN_KEY_F15 ]           =  XK_F15;
    mn_buttonToXKeyMap[ MGN_KEY_F16 ]           =  XK_F16;
    mn_buttonToXKeyMap[ MGN_KEY_F17 ]           =  XK_F17;
    mn_buttonToXKeyMap[ MGN_KEY_F18 ]           =  XK_F18;
    mn_buttonToXKeyMap[ MGN_KEY_F19 ]           =  XK_F19;
    mn_buttonToXKeyMap[ MGN_KEY_F20 ]           =  XK_F20;
    mn_buttonToXKeyMap[ MGN_KEY_F21 ]           =  XK_F21;
    mn_buttonToXKeyMap[ MGN_KEY_F22 ]           =  XK_F22;
    mn_buttonToXKeyMap[ MGN_KEY_F23 ]           =  XK_F23;
    mn_buttonToXKeyMap[ MGN_KEY_F24 ]           =  XK_F24;
    mn_buttonToXKeyMap[ MGN_KEY_F25 ]           =  XK_F25;
    mn_buttonToXKeyMap[ MGN_KEY_F26 ]           =  XK_F26;
    mn_buttonToXKeyMap[ MGN_KEY_F27 ]           =  XK_F27;
    mn_buttonToXKeyMap[ MGN_KEY_F28 ]           =  XK_F28;
    mn_buttonToXKeyMap[ MGN_KEY_F29 ]           =  XK_F29;
    mn_buttonToXKeyMap[ MGN_KEY_F30 ]           =  XK_F30;
    mn_buttonToXKeyMap[ MGN_KEY_F31 ]           =  XK_F31;
    mn_buttonToXKeyMap[ MGN_KEY_F32 ]           =  XK_F32;
    mn_buttonToXKeyMap[ MGN_KEY_F33 ]           =  XK_F33;
    mn_buttonToXKeyMap[ MGN_KEY_F34 ]           =  XK_F34;
    mn_buttonToXKeyMap[ MGN_KEY_F35 ]           =  XK_F35;
    mn_buttonToXKeyMap[ MGN_KEY_SHIFT_L ]       =  XK_Shift_L;
    mn_buttonToXKeyMap[ MGN_KEY_SHIFT_R ]       =  XK_Shift_R;
    mn_buttonToXKeyMap[ MGN_KEY_CONTROL_L ]     =  XK_Control_L;
    mn_buttonToXKeyMap[ MGN_KEY_CONTROL_R ]     =  XK_Control_R;
    mn_buttonToXKeyMap[ MGN_KEY_CAPS_LOCK ]     =  XK_Caps_Lock;
    mn_buttonToXKeyMap[ MGN_KEY_META_L ]        =  XK_Meta_L;
    mn_buttonToXKeyMap[ MGN_KEY_META_R ]        =  XK_Meta_R;
    mn_buttonToXKeyMap[ MGN_KEY_ALT_L ]         =  XK_Alt_L;
    mn_buttonToXKeyMap[ MGN_KEY_ALT_R ]         =  XK_Alt_R;
    mn_buttonToXKeyMap[ MGN_KEY_SUPER_L ]       =  XK_Super_L;
    mn_buttonToXKeyMap[ MGN_KEY_SUPER_R ]       =  XK_Super_R;
    mn_buttonToXKeyMap[ MGN_KEY_SPACE ]         =  XK_space;
    mn_buttonToXKeyMap[ MGN_KEY_EXCLAMATION ]   =  XK_exclam;
    mn_buttonToXKeyMap[ MGN_KEY_DOUBLE_QUOTE ]  =  XK_quotedbl;
    mn_buttonToXKeyMap[ MGN_KEY_NUMBER_SIGN ]   =  XK_numbersign;
    mn_buttonToXKeyMap[ MGN_KEY_DOLLAR ]        =  XK_dollar;
    mn_buttonToXKeyMap[ MGN_KEY_PERCENT ]       =  XK_percent;
    mn_buttonToXKeyMap[ MGN_KEY_AMPERSAND ]     =  XK_ampersand;
    mn_buttonToXKeyMap[ MGN_KEY_APOSTROPHE ]    =  XK_apostrophe;
    mn_buttonToXKeyMap[ MGN_KEY_PAREN_L ]       =  XK_parenleft;
    mn_buttonToXKeyMap[ MGN_KEY_PAREN_R ]       =  XK_parenright;
    mn_buttonToXKeyMap[ MGN_KEY_ASTERISK ]      =  XK_asterisk;
    mn_buttonToXKeyMap[ MGN_KEY_PLUS ]          =  XK_plus;
    mn_buttonToXKeyMap[ MGN_KEY_COMMA ]         =  XK_comma;
    mn_buttonToXKeyMap[ MGN_KEY_MINUS ]         =  XK_minus;
    mn_buttonToXKeyMap[ MGN_KEY_PERIOD ]        =  XK_period;
    mn_buttonToXKeyMap[ MGN_KEY_SLASH ]         =  XK_slash;
    mn_buttonToXKeyMap[ MGN_KEY_0 ]             =  XK_0;
    mn_buttonToXKeyMap[ MGN_KEY_1 ]             =  XK_1;
    mn_buttonToXKeyMap[ MGN_KEY_2 ]             =  XK_2;
    mn_buttonToXKeyMap[ MGN_KEY_3 ]             =  XK_3;
    mn_buttonToXKeyMap[ MGN_KEY_4 ]             =  XK_4;
    mn_buttonToXKeyMap[ MGN_KEY_5 ]             =  XK_5;
    mn_buttonToXKeyMap[ MGN_KEY_6 ]             =  XK_6;
    mn_buttonToXKeyMap[ MGN_KEY_7 ]             =  XK_7;
    mn_buttonToXKeyMap[ MGN_KEY_8 ]             =  XK_8;
    mn_buttonToXKeyMap[ MGN_KEY_9 ]             =  XK_9;
    mn_buttonToXKeyMap[ MGN_KEY_COLON ]         =  XK_colon;
    mn_buttonToXKeyMap[ MGN_KEY_SEMICOLON ]     =  XK_semicolon;
    mn_buttonToXKeyMap[ MGN_KEY_LESS ]          =  XK_less;
    mn_buttonToXKeyMap[ MGN_KEY_EQUAL ]         =  XK_equal;
    mn_buttonToXKeyMap[ MGN_KEY_GREATER ]       =  XK_greater;
    mn_buttonToXKeyMap[ MGN_KEY_QUESTION ]      =  XK_question;
    mn_buttonToXKeyMap[ MGN_KEY_AT_SIGN ]       =  XK_at;
    mn_buttonToXKeyMap[ MGN_KEY_BRACKET_L ]     =  XK_bracketleft;
    mn_buttonToXKeyMap[ MGN_KEY_BACKSLASH ]     =  XK_backslash;
    mn_buttonToXKeyMap[ MGN_KEY_BRACKET_R ]     =  XK_bracketright;
    mn_buttonToXKeyMap[ MGN_KEY_CIRCUMFLEX ]    =  XK_asciicircum;
    mn_buttonToXKeyMap[ MGN_KEY_UNDERSCORE ]    =  XK_underscore;
    mn_buttonToXKeyMap[ MGN_KEY_BACK_TICK ]     =  XK_grave;
    mn_buttonToXKeyMap[ MGN_KEY_A ]             =  XK_a;
    mn_buttonToXKeyMap[ MGN_KEY_B ]             =  XK_b;
    mn_buttonToXKeyMap[ MGN_KEY_C ]             =  XK_c;
    mn_buttonToXKeyMap[ MGN_KEY_D ]             =  XK_d;
    mn_buttonToXKeyMap[ MGN_KEY_E ]             =  XK_e;
    mn_buttonToXKeyMap[ MGN_KEY_F ]             =  XK_f;
    mn_buttonToXKeyMap[ MGN_KEY_G ]             =  XK_g;
    mn_buttonToXKeyMap[ MGN_KEY_H ]             =  XK_h;
    mn_buttonToXKeyMap[ MGN_KEY_I ]             =  XK_i;
    mn_buttonToXKeyMap[ MGN_KEY_J ]             =  XK_j;
    mn_buttonToXKeyMap[ MGN_KEY_K ]             =  XK_k;
    mn_buttonToXKeyMap[ MGN_KEY_L ]             =  XK_l;
    mn_buttonToXKeyMap[ MGN_KEY_M ]             =  XK_m;
    mn_buttonToXKeyMap[ MGN_KEY_N ]             =  XK_n;
    mn_buttonToXKeyMap[ MGN_KEY_O ]             =  XK_o;
    mn_buttonToXKeyMap[ MGN_KEY_P ]             =  XK_p;
    mn_buttonToXKeyMap[ MGN_KEY_Q ]             =  XK_q;
    mn_buttonToXKeyMap[ MGN_KEY_R ]             =  XK_r;
    mn_buttonToXKeyMap[ MGN_KEY_S ]             =  XK_s;
    mn_buttonToXKeyMap[ MGN_KEY_T ]             =  XK_t;
    mn_buttonToXKeyMap[ MGN_KEY_U ]             =  XK_u;
    mn_buttonToXKeyMap[ MGN_KEY_V ]             =  XK_v;
    mn_buttonToXKeyMap[ MGN_KEY_W ]             =  XK_w;
    mn_buttonToXKeyMap[ MGN_KEY_X ]             =  XK_x;
    mn_buttonToXKeyMap[ MGN_KEY_Y ]             =  XK_y;
    mn_buttonToXKeyMap[ MGN_KEY_Z ]             =  XK_z;
    mn_buttonToXKeyMap[ MGN_KEY_BRACE_L ]       =  XK_braceleft;
    mn_buttonToXKeyMap[ MGN_KEY_VERTICAL_BAR ]  =  XK_bar;
    mn_buttonToXKeyMap[ MGN_KEY_BRACE_R ]       =  XK_braceright;
    mn_buttonToXKeyMap[ MGN_KEY_TILDE ]         =  XK_asciitilde;
    }



static void mn_setupLinuxGamepadMaps( void ) {

    int            i;
    int            j;
    MinginGamepad  ps4Gamepad    =  MGN_PS_DUALSHOCK_4;
    /*MinginGamepad  xboxGamepad  =  MGN_XBOX_360;*/
    
    /* start by filling all with MGN_MAP_END padding */
    for( i = MGN_FIRST_GAMEPAD;
         i < MGN_NUM_GAMEPADS;
         i ++ ) {

        for( j = 0;
             j < MINGIN_MAX_NUM_GAMEPAD_BUTTONS;
             j ++ ) {
            
            mn_jsButtonToButtonMap[ i ][ j ] = MGN_MAP_END;
            }
        
        for( j = 0;
             j < MINGIN_MAX_NUM_GAMEPAD_STICKS;
             j ++ ) {
            
            mn_jsStickToButtonMap[ i ][ j ][0] = MGN_MAP_END;
            mn_jsStickToButtonMap[ i ][ j ][1] = MGN_MAP_END;
            
            mn_jsStickToStickMap [ i ][ j ]    = MGN_MAP_END;

            /* default to no sticks being present */
            mn_stickPresent[ i ][ j ] = 0;
                
            /* default range of 0 for all sticks on all gamepads */
            mn_stickRange[ i ][ j ][ 0 ] = 0;
            mn_stickRange[ i ][ j ][ 1 ] = 0;
            }
        
        }


    
    /* First, setup PS Dualshock 4 controller */
    
    mn_jsButtonToButtonMap[ ps4Gamepad ][  0 ]  =  MGN_BUTTON_PS_X,
    mn_jsButtonToButtonMap[ ps4Gamepad ][  1 ]  =  MGN_BUTTON_PS_CIRCLE,
    mn_jsButtonToButtonMap[ ps4Gamepad ][  2 ]  =  MGN_BUTTON_PS_TRIANGLE,
    mn_jsButtonToButtonMap[ ps4Gamepad ][  3 ]  =  MGN_BUTTON_PS_SQUARE,
    mn_jsButtonToButtonMap[ ps4Gamepad ][  4 ]  =  MGN_BUTTON_L1,
    mn_jsButtonToButtonMap[ ps4Gamepad ][  5 ]  =  MGN_BUTTON_R1,
    mn_jsButtonToButtonMap[ ps4Gamepad ][  6 ]  =  MGN_BUTTON_L2,
    mn_jsButtonToButtonMap[ ps4Gamepad ][  7 ]  =  MGN_BUTTON_R2,
    mn_jsButtonToButtonMap[ ps4Gamepad ][  8 ]  =  MGN_BUTTON_PS_SHARE,
    mn_jsButtonToButtonMap[ ps4Gamepad ][  9 ]  =  MGN_BUTTON_PS_OPTIONS,
    mn_jsButtonToButtonMap[ ps4Gamepad ][ 10 ]  =  MGN_BUTTON_PS_PS,
    mn_jsButtonToButtonMap[ ps4Gamepad ][ 11 ]  =  MGN_BUTTON_STICK_LEFT_PRESS,
    mn_jsButtonToButtonMap[ ps4Gamepad ][ 12 ]  =  MGN_BUTTON_STICK_RIGHT_PRESS,
        
    mn_jsStickToButtonMap[ ps4Gamepad ][ 6 ][ 0 ]  =  MGN_BUTTON_DPAD_LEFT;
    mn_jsStickToButtonMap[ ps4Gamepad ][ 6 ][ 1 ]  =  MGN_BUTTON_DPAD_RIGHT;
    mn_jsStickToButtonMap[ ps4Gamepad ][ 7 ][ 0 ]  =  MGN_BUTTON_DPAD_UP;
    mn_jsStickToButtonMap[ ps4Gamepad ][ 7 ][ 1 ]  =  MGN_BUTTON_DPAD_DOWN;

    mn_jsStickToStickMap[ ps4Gamepad ][ 0 ]  =  MGN_STICK_LEFT_X;
    mn_jsStickToStickMap[ ps4Gamepad ][ 1 ]  =  MGN_STICK_LEFT_Y;
    mn_jsStickToStickMap[ ps4Gamepad ][ 2 ]  =  MGN_STICK_LEFT_TRIGGER;
    mn_jsStickToStickMap[ ps4Gamepad ][ 3 ]  =  MGN_STICK_RIGHT_X;
    mn_jsStickToStickMap[ ps4Gamepad ][ 4 ]  =  MGN_STICK_RIGHT_Y;
    mn_jsStickToStickMap[ ps4Gamepad ][ 5 ]  =  MGN_STICK_RIGHT_TRIGGER;
    mn_jsStickToStickMap[ ps4Gamepad ][ 6 ]  =  MGN_STICK_DPAD_X;
    mn_jsStickToStickMap[ ps4Gamepad ][ 7 ]  =  MGN_STICK_DPAD_Y;

    /* all 8 sticks on PS4 controller have same range */
    for( j =  0;
         j <= 7;
         j ++ ) {
        
        MinginStick s  =  mn_jsStickToStickMap[ ps4Gamepad ][ j ];

        mn_stickPresent[ ps4Gamepad ][ s ]       =       1;
        mn_stickRange  [ ps4Gamepad ][ s ][ 0 ]  =  -32767;
        mn_stickRange  [ ps4Gamepad ][ s ][ 1 ]  =   32767;
        }


    /* Next, setup XBox 360 controller */
    }



static int mn_getGamepadIndex( const char  *inIDString ) {

    int i  =  MGN_FIRST_GAMEPAD;
    
    while( i < MGN_NUM_GAMEPADS ) {
        if(  mn_stringsEqual( inIDString,
                              mn_gamepadIDStrings[ i ] ) ) {
            return i;
            }
        i ++;
        }
    
    return -1;
    }



/*
  Returns static string.

  Only indexes 0..9 supported.
*/
static const char *mn_getJSPath( int  inIndex ) {

    static  char  pathString[15]  = "/dev/input/js0";

    int  digitPos  =  13;

    
    pathString[ digitPos ] = (char)( '0' + inIndex );
    
    return pathString;
    }



static int mn_openActiveGamepad( void ) {

    int   fd;
    char  name[ 128 ];
    int   i             =  0;
    int   result;

    /* whenever we open or re-open a gamepad, zero out our stick positions */
    for( i = 0;
         i < MGN_NUM_STICKS;
         i ++ ) {
        
        mn_stickPosition[ i ] = 0;
        }

    for( i =  0;
         i <= 9;
         i ++ ) {
        
        fd = open( mn_getJSPath( i ),
                   O_RDONLY | O_NONBLOCK );

        if( fd != -1 ) {
            /* opened a joystick device */

            /* does it match one of our strings? */

            result = ioctl( fd,
                            JSIOCGNAME( sizeof( name ) ),
                            name );

            if( result >= 0 ) {
                
                int gamepadIndex  =
                    mn_getGamepadIndex( name );

                if( gamepadIndex != -1 ) {

                    mingin_log( "Active gamepad opened: " );
                    mingin_log( name );
                    mingin_log( "\n" );
                    
                    mn_activeGamepad = gamepadIndex;

                    return fd;
                    }
                }

            /* not a match */
            close( fd );
            }
        }

    /* got through all of /dev/input/js0 through /dev/input/js9
       without finding one */

    mn_activeGamepad = MGN_NO_GAMEPAD;

    return -1;
    }


    



static char *mn_linuxGetFilePath( const char  *inFolderName,
                                  const char  *inFileName ) {
    
    enum{  MAX_PATH_LEN  =  255  };
    
    static  char  buffer[ MAX_PATH_LEN + 1 ];

    int  i  =  0;
    int  j  =  0;
    
    
    while( i < MAX_PATH_LEN
           &&
           inFolderName[i] != '\0' ) {
        
        buffer[i] = inFolderName[i];
        i++;
        }
    
    /* now separator */
    if( i < MAX_PATH_LEN ) {
        buffer[i] = '/';
        i++;
        }
    
    while( i < MAX_PATH_LEN
           &&
           inFileName[j] != '\0' ) {
        
        buffer[i] = inFileName[j];
        i++;
        j++;
        }

    /* now terminate */
    if( i < MAX_PATH_LEN ) {
        buffer[i] = '\0';
        }
    
    return buffer;
    }



static int mn_linuxFileOpenRead( const char  *inFolderName,
                                 const char  *inFileName,
                                 int         *outTotalBytes ) {
    struct stat   statStruct;
    int           fd;
    char         *path         =  mn_linuxGetFilePath( inFolderName,
                                                       inFileName );

    *outTotalBytes = 0;
    
    if( stat( path, &statStruct ) == 0 ) {
        
        *outTotalBytes = (int)( statStruct.st_size );
        }
    else {
        /* stat failed, file does not exist */
        return -1;
        }
    
    fd = open( path,
               O_RDONLY );

    return fd;
    }



static int mn_linuxFileOpenWrite( const char  *inFolderName,
                                  const char  *inFileName ) {
    struct stat  statStruct;
    char         folderExists  =  0;

    /* make sure folder exists */
    
    /* Check if the path exists and retrieve its stat */
    if( stat( inFolderName, &statStruct ) == 0 ) {
        /* use macro to check if it's a dir */
        folderExists = S_ISDIR( statStruct.st_mode );
        }
    if( ! folderExists ) {
        if( mkdir( inFolderName, 0755 ) == 0 ) {
            folderExists = 1;
            }
        }
    if( folderExists ) {
        
        char  *path  =  mn_linuxGetFilePath( inFolderName,
                                             inFileName );
        int    fd    =  open( path,
                              O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU );

        return fd;
        }
    else {
        return -1;
        }
    }



static char mn_linuxFileWrite( int                   inFD,
                               int                   inNumBytesToWrite,
                               const unsigned char  *inByteBuffer ) {
    
    int  numWritten  =  0;
    
    while( numWritten < inNumBytesToWrite ) {
        size_t   numLeftToWrite      =  (size_t)( inNumBytesToWrite -
                                                  numWritten );
        ssize_t  numWrittenThisTime  =  write( inFD,
                                               &( inByteBuffer[ numWritten ] ),
                                               numLeftToWrite );
        if( numWrittenThisTime == -1 ) {
            return 0;
            }
        
        numWritten += (int)numWrittenThisTime;
        }

    return 1;
    }



static int mn_linuxFileRead(           int    inFD,
                                       int    inNumBytesToRead,
                             unsigned  char  *inByteBuffer ) {
    int  numRead  =  0;
    
    while( numRead < inNumBytesToRead ) {
        size_t   numLeftToRead    =  (size_t)( inNumBytesToRead - numRead );
        
        ssize_t  numReadThisTime  =  read( inFD,
                                           &( inByteBuffer[numRead] ),
                                           numLeftToRead );

        if( numReadThisTime == 0 ) {
            /* truly reached end of file */
            return numRead;
            }
        else if( numReadThisTime == -1 ) {
            /* error on read */
            return 0;
            }
        numRead += (int)numReadThisTime;
        }

    return numRead;
    }



static char mn_linuxFileSeek( int  inFD,
                              int  inAbsoluteBytePosition ) {

    off_t  offset  =  lseek( inFD,
                             inAbsoluteBytePosition,
                             SEEK_SET );
    
    if( offset == (off_t)( -1 ) ) {
        return 0;
        }
    return 1;
    }



static int mn_linuxFileGetPos( int  inFD ) {
    
    off_t  offset  =  lseek( inFD,
                             0,
                             SEEK_CUR );
    
    if( offset == (off_t)( -1 ) ) {
        return -1;
        }
    return (int)offset;
    }



int mingin_startWritePersistData( const char  *inStoreName ) {
    return mn_linuxFileOpenWrite( "settings",
                                  inStoreName );
    }



int mingin_startReadPersistData( const char  *inStoreName,
                                 int         *outTotalBytes ) {
    return mn_linuxFileOpenRead( "settings",
                                 inStoreName,
                                 outTotalBytes );
    }



char mingin_writePersistData( int                   inStoreWriteHandle,
                              int                   inNumBytesToWrite,
                              const unsigned char  *inByteBuffer ) {
    
    return mn_linuxFileWrite( inStoreWriteHandle,
                              inNumBytesToWrite,
                              inByteBuffer );
    }



int mingin_readPersistData( int             inStoreReadHandle,
                            int             inNumBytesToRead,
                            unsigned char  *inByteBuffer ) {

    return mn_linuxFileRead( inStoreReadHandle,
                             inNumBytesToRead,
                             inByteBuffer );
    }



char mingin_seekPersistData( int  inStoreReadHandle,
                             int  inAbsoluteBytePosition ) {
    return mn_linuxFileSeek( inStoreReadHandle,
                             inAbsoluteBytePosition );
    }



int mingin_getPersistDataPosition( int  inStoreReadHandle ) {
    return mn_linuxFileGetPos( inStoreReadHandle );
    }



void mingin_endWritePersistData( int  inStoreWriteHandle ) {
    close( inStoreWriteHandle );
    }



void mingin_endReadPersistData( int  inStoreReadHandle ) {
    close( inStoreReadHandle );
    }


void mingin_deletePersistData( const char  *inStoreName ) {
    
    char  *path  =  mn_linuxGetFilePath( "settings",
                                         inStoreName );
    
    unlink( path );
    }




int mingin_startReadBulkData( const char  *inBulkName,
                              int         *outTotalBytes ) {
    return mn_linuxFileOpenRead( "data",
                                 inBulkName,
                                 outTotalBytes );
    }



int mingin_readBulkData( int             inBulkDataHandle,
                         int             inNumBytesToRead,
                         unsigned char  *inByteBuffer ) {
    
    return mn_linuxFileRead( inBulkDataHandle,
                             inNumBytesToRead,
                             inByteBuffer );
    }




char mingin_seekBulkData( int  inBulkDataHandle,
                          int  inAbsoluteBytePosition ) {
    return mn_linuxFileSeek( inBulkDataHandle,
                             inAbsoluteBytePosition );
    }



int mingin_getBulkDataPosition( int  inBulkDataHandle ) {
    return mn_linuxFileGetPos( inBulkDataHandle );
    }



void mingin_endReadBulkData( int  inBulkDataHandle ) {
    close( inBulkDataHandle );
    }










/* end #ifdef __linux__ */
#elif defined(WIN32)

/*
  ==================================================
  Windows implementation               [jumpWindows]
  ==================================================
*/

/* empty for now */



#else

/*
  ==================================================
  Dummy example implementation         [jumpExample]
  ==================================================

  If we fall through the above platform-detecting #ifdefs and match
  nothing, this platform-independent implementation will compile instead.

  This code only works on platforms that support int main( void ) as an
  entrypoint.

  You can use this as a skeleton when making a Mingin implementation for
  a new platform.
*/

#define  MINGIN_DUMMY_SCREEN_W  640
#define  MINGIN_DUMMY_SCREEN_H  480

static  unsigned char  mn_dummyScreenBuffer[ MINGIN_DUMMY_SCREEN_W *
                                             MINGIN_DUMMY_SCREEN_H * 3 ];

static  int            mn_gotQuit  =  0;



int main( void ) {

    minginInternal_init();
    
    while( ! mn_gotQuit ) {
        
        /* don't ask about the minimum screen size */
        
        minginGame_step( 0 );
        
        /* ask for screen pixels and do nothing with them */
        minginGame_getScreenPixels( MINGIN_DUMMY_SCREEN_W,
                                    MINGIN_DUMMY_SCREEN_H,
                                    mn_dummyScreenBuffer );

        /* don't even bother asking for game audio samples */
        }

    /* game asked to quit ! */
    return 0;
    }



int mingin_getStepsPerSecond( void ) {
    /* dummy value */
    return 1;
    }



int mingin_getMillisecondsLeftInStep( void ) {
    return -1;
    }



void mingin_quit( void ) {
    mn_gotQuit = 1;
    }




static char minginPlatform_isButtonDown( MinginButton  inButton ) {
    /* suppress warning */
    if( inButton == MGN_BUTTON_NONE ) {
        }
    return 0;
    }


MinginButton mingin_getPlatformPrimaryButton( int inButtonHandle ) {
    /* suppress warning */
    if( inButtonHandle ) {
        }
    return MGN_BUTTON_NONE;
    }



char mingin_getPointerLocation( int  *outX,
                                int  *outY,
                                int  *outMaxX,
                                int  *outMaxY ) {
    /* suppress warning */
    if( *outX ||
        *outY ||
        *outMaxX ||
        *outMaxY ) {
        return 0;
        }
    return 0;
    }




void mingin_log( const char  *inString ) {
    /* suppress compiler warning */
    if( inString[0] == '\0' ) {
        }
    return;
    }




char mingin_toggleFullscreen( char  inFullscreen ) {
    /* suppress warning */
    if( inFullscreen ) {
        }
    return 0;
    }



char mingin_isFullscreen( void ) {
    return 0;
    }



MinginButton mingin_getLastButtonPressed( void ) {
    return MGN_BUTTON_NONE;
    }



int mingin_startWritePersistData( const char  *inStoreName ) {
    /* suppress  warning */
    if( inStoreName[0] == '\0' ) {
        }
    return -1;
    }



int mingin_startReadPersistData( const char  *inStoreName,
                                 int         *outTotalBytes ) {
    /* suppress  warning */
    if( inStoreName[0] == '\0' ) {
        }
    *outTotalBytes = 0;
    
    return -1;
    }



char mingin_writePersistData( int                   inStoreWriteHandle,
                              int                   inNumBytesToWrite,
                              const unsigned char  *inByteBuffer ) {
    /* suppress warning */
    if( inStoreWriteHandle > 0
        ||
        inNumBytesToWrite > 0
        ||
        inByteBuffer != 0 ) {
        }
    return 0;
    }



int mingin_readPersistData( int             inStoreReadHandle,
                            int             inNumBytesToRead,
                            unsigned char  *inByteBuffer ) {
    /* suppress warning */
    if( inStoreReadHandle > 0
        ||
        inNumBytesToRead > 0
        ||
        inByteBuffer != 0 ) {
        }
    return -1;
    }



char mingin_seekPersistData( int  inStoreReadHandle,
                             int  inAbsoluteBytePosition ) {
    /* suppress warning */
    if( inStoreReadHandle > 0
        ||
        inAbsoluteBytePosition > 0 ) {
        }
    return 0;
    }


int mingin_getPersistDataPosition( int  inStoreReadHandle ) {
    /* suppress warning */
    if( inStoreReadHandle > 0 ) {
        }
    return 0;
    }




void mingin_endWritePersistData( int  inStoreWriteHandle ) {
    /* suppress warning */
    if( inStoreWriteHandle > 0 ) {
        }
    }



void mingin_endReadPersistData( int  inStoreReadHandle ) {
    /* suppress warning */
    if( inStoreReadHandle > 0 ) {
        }
    }



int mingin_startReadBulkData( const char  *inBulkName,
                              int         *outTotalBytes ) {
    /* suppress warning */
    if( inBulkName[0] != '\0' ) {
        }
    *outTotalBytes = 0;
    return -1;
    }



int mingin_readBulkData( int             inBulkDataHandle,
                         int             inNumBytesToRead,
                         unsigned char  *inByteBuffer ) {
    /* suppress warning */
    if( inBulkDataHandle > 0
        ||
        inNumBytesToRead > 0
        ||
        inByteBuffer != 0 ) {
        }
    return -1;
    }




char mingin_seekBulkData( int  inBulkDataHandle,
                          int  inAbsoluteBytePosition ) {
    /* suppress warning */
    if( inBulkDataHandle > 0
        ||
        inAbsoluteBytePosition > 0 ) {
        }
    return 0;
    }


int mingin_getBulkDataPosition( int  inBulkDataHandle ) {
    /* suppress warning */
    if( inBulkDataHandle > 0 ) {
        }
    return 0;
    }


void mingin_endReadBulkData( int  inBulkDataHandle ) {
    /* suppress warning */
    if( inBulkDataHandle > 0 ) {
        }
    }


void mingin_deletePersistData( const char  *inStoreName ) {
    /* suppress warning */
    if( inStoreName[0] != '\0' ) {
        }
    }





/* end of platform-specific #ifdef  */
#endif




/* end #ifdef MINGIN_IMPLEMENTATION */
#endif



/* end of #ifndef MINGIN_H_INCLUDED */
#endif
