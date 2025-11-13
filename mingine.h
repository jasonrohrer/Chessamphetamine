
/*
  Include wherever like so:

  #include "mingine.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

  #define MINGINE_IMPLEMENTATION
  #include "mingine.h"

*/



/*

  mingine provides a simple wrapper for the platform-specific infrastructure
  necessary to make a single-player computer game.

  In order to achieve maximum portability across past and future platforms
  that support compiling C89 code, we make no assumptions about the plaforms
  themselves and what they provide.  For example:

  --This code makes no includes that are assumed to be available on every
    plaform.  Any includes present are strictly platform-specific and surrounded
    by #ifdef's that will remove them from compilation on other platforms

  --This code makes no assumptions about any particular metaphors or program
    structures, and works just fine on platforms without "filesystems" or
    "heap allocation" or "threads"

  --This code makes no assumptions about entry points (like main) that must
    be present on every platform.  Platform-specific entry points are possible.

  --The code makes no assumptions about what hardware is present, and can
    be used to run games on platforms that can only provide a subset of
    the required infrastructure.
    

  Such games need four things to function, which they can't do on their own
  without the help of the underlying platform:

  1.  A way to draw to a rectangular window or screen.
  
  2.  A way to play a stream of audio to some kind of audio output.

  3.  A way to receive input from the user (mouse, keyboard, controller, etc.).
  
  4.  A way to read and write data that persists between runs of the game.
  
  5.  A way to read bulk data resources, like graphics and sounds, that
      aren't practical to compile into the program code itself.  For example,
      a game might stream music audio data that is too big to fit in RAM.
*/



/*
  First, the game itself must implement these functions.
  These are called by the plaform, and the platform is in charge
  of doing any necessary color or sample format conversions with the
  data provided by the game.

  For example, this interface is in 24-bit color, so a platform that
  only supports 8-bit color must do the color conversion.
*/


/*
  Get the next screen full of pixels
  Each R, G, and B color component is 8 bytes, interleaved in
  RGBRGBRGB... in row-major order, starting from the top left corner
  of the screen and going left-to-right and top-to-bottom.
*/
void mingin_gameGetScreenPixels( int inWide, int inHigh,
                                 unsigned char *inRGBBuffer );

/*
  Get the next buffer full of audio samples.
  Samples are in Signed 16-bit little-endian, with channels interleaved

  inSampleBuffer will have inNumSamples * inNumChannels * 2 bytes
*/
void mingin_gameGetAudioSamples( int inNumSamples,
                                 int inNumChannels,
                                 int inSamplesPerSecond,
                                 unsigned char *inSampleBuffer );




#ifdef MINGINE_IMPLEMENTATION




#ifdef __linux__


int main( void );

int main( void ) {
    
    return 1;
    }


/* end #ifdef __linux__ */
#endif




/* end #ifdef MINGINE_IMPLEMENTATION */
#endif