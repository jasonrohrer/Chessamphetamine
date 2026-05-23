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


/* strength in range 0...1024 */
void pinchSet( int  inX,
               int  inY,
               int  inStrength,
               int  inRadius );


/* leaves other params along from what was last set with pinchSet */
void pinchSetStrength( int  inStrength );



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



void pinchSetStrength( int  inStrength ) {
    pinchStrength = inStrength;
    }



static  unsigned char  pinchBuffer[ MAXIGIN_GAME_NATIVE_W *
                                    MAXIGIN_GAME_NATIVE_H *
                                    3 ];


void pinchApply( unsigned char *inRGBBuffer ) {

    int   y;
    int   x;
    long  b;
    long  numPixels;
    long  numBytes;
    long  p           =  0;
    int   minX;
    int   maxX;
    int   minY;
    int   maxY;
    
    if( pinchStrength == 0
        ||
        pinchRadius == 0 ) {
        return;
        }

    numPixels = MAXIGIN_GAME_NATIVE_H * MAXIGIN_GAME_NATIVE_W;
    numBytes  = numPixels * 3;

    
    /* copy into working buffer
       then we can pinch and pull results into live buffer
       and we only have to process part that is inside radius */
    for( b = 0;
         b < numBytes;
         b ++ ) {

        pinchBuffer[b] = inRGBBuffer[b];
        }

    minX = pinchX - pinchRadius;
    maxX = pinchX + pinchRadius;
    minY = pinchY - pinchRadius;
    maxY = pinchY + pinchRadius;

    if( minX < 0 ) {
        minX = 0;
        }
    if( minX >= MAXIGIN_GAME_NATIVE_W ) {
        minX = MAXIGIN_GAME_NATIVE_W - 1;
        }
    if( maxX < 0 ) {
        maxX = 0;
        }
    if( maxX >= MAXIGIN_GAME_NATIVE_W ) {
        maxX  = MAXIGIN_GAME_NATIVE_W - 1;
        }

    if( minY < 0 ) {
        minY = 0;
        }
    if( minY >= MAXIGIN_GAME_NATIVE_H ) {
        minY  = MAXIGIN_GAME_NATIVE_H - 1;
        }
    if( maxY < 0 ) {
        maxY = 0;
        }
    if( maxY >= MAXIGIN_GAME_NATIVE_H ) {
        maxY  = MAXIGIN_GAME_NATIVE_H - 1;
        }
    

    for( y = minY;
         y <= maxY;
         y ++ ) {

        long  dy     =  y - pinchY;
        long  dy2    =  dy * dy;
        long  yPart  =  y * MAXIGIN_GAME_NATIVE_W * 3;

        for( x = minX;
             x <= maxX;
             x ++ ) {

            /* where should x and y pixel values come from in source image? */
            int  sy;
            int  sx;
            
            long  dx = x - pinchX;

            /* compute sqrt with more precision */
            long r_16 =
                (long)longSquareRoot(
                    (unsigned long)( dy2 + dx * dx ) * 256 );

            if( r_16 > 0
                &&
                r_16 < pinchRadius * 16 ) {
                
                /* t^2 is a strong pinch, without a pow funciton call
                   we vary strength by mixing below */
                long  t_1024_16   =  ( r_16 * 1024 ) / pinchRadius;
                long  t2_1024_16  =  ( t_1024_16 * t_1024_16 ) / ( 1024 * 16 );
                long  rPinch_16   =  pinchRadius * t2_1024_16 / 1024;

                /* we're scaling by pinch strength here */
                long  rSource_16  =
                    r_16 + ( pinchStrength * ( r_16 - rPinch_16 ) ) / 1024;
                
                sy = (int)( pinchY + ( dy * rSource_16 ) / r_16 );
                sx = (int)( pinchX + ( dx * rSource_16 ) / r_16 );
                }
            else {
                sy = y;
                sx = x;
                }

            p = yPart + x * 3;
            
            if( sy < 0
                ||
                sy >= MAXIGIN_GAME_NATIVE_H
                ||
                sx < 0
                ||
                sx >= MAXIGIN_GAME_NATIVE_W ) {

                /* out of bounds, black */
                inRGBBuffer[ p++ ] = 0;
                inRGBBuffer[ p++ ] = 0;
                inRGBBuffer[ p++ ] = 0;
                }
            else {
                /* landed in image, take color from there */
                long sp = (long)sy * MAXIGIN_GAME_NATIVE_W * 3 + sx * 3;

                inRGBBuffer[ p++ ] = pinchBuffer[ sp++ ];
                inRGBBuffer[ p++ ] = pinchBuffer[ sp++ ];
                inRGBBuffer[ p++ ] = pinchBuffer[ sp++ ];
                }
            }
        }

    
    
    }






#endif
#endif
