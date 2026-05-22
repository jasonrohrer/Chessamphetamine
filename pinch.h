/*
  Include in your C code wherever like so:

      #include "pinch.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define PINCH_IMPLEMENTATION
      #include "pinch.h"

*/

#ifndef PINCH_H_INCLUDED
#define PINCH_H_INCLUDED


void pinchInit( void );


void pinchSet( int  inX,
               int  inY,
               int  inStrength,
               int  inRadius );

void pinchClear( void );


/* applies pinch effect to native pixel buffer with size
   MAXIGIN_GAME_NATIVE_W  by  MAXIGIN_GAME_NATIVE_H
   
   does nothing if pinch has been cleared or has 0 strength value */
void pinchApply( unsigned char *inRGBBuffer );



#ifdef PINCH_IMPLEMENTATION

#include "memoryRegister.h"
#include "gameSize.h"



static  int  pinchX;
static  int  pinchY;
static  int  pinchStrength;
static  int  pinchRadius;


void pinchInit( void ) {

    pinchClear();

    REGISTER_VAL_MEM( pinchX );
    REGISTER_VAL_MEM( pinchY );
    REGISTER_VAL_MEM( pinchStrength );
    REGISTER_VAL_MEM( pinchRadius );
    }



void pinchClear( void ) {
    pinchSet( 0,
              0,
              0,
              0 );
    }



void pinchSet( int  inX,
               int  inY,
               int  inStrength,
               int  inRadius ) {
    
    pinchX        = inX;
    pinchY        = inY;
    pinchStrength = inStrength;
    pinchRadius   = inRadius;
    }



static  unsigned char  pinchBuffer[ MAXIGIN_GAME_NATIVE_W *
                                    MAXIGIN_GAME_NATIVE_H *
                                    3 ];


void pinchApply( unsigned char *inRGBBuffer ) {
    if( pinchStrength == 0
        ||
        pinchRadius == 0 ) {
        return;
        }

    (void)inRGBBuffer;
    (void)pinchBuffer;
    }






#endif
#endif
