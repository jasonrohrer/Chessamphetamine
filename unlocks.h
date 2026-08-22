/*
  Include in your C code wherever like so:

      #include "unlocks.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define UNLOCKS_IMPLEMENTATION
      #include "unlocks.h"

*/

#ifndef UNLOCKS_H_INCLUDED
#define UNLOCKS_H_INCLUDED


void unlocksInit( void );


void unlocksDraw( int  inPosX,
                  int  inPosY );


/* returns lang translation key for live unlock currently moused over
   or -1 if none moused over */
int unlocksStep( int  inPosX,
                 int  inPosY  );




#endif



#ifdef  UNLOCKS_IMPLEMENTATION

#ifndef UNLOCKS_IMPLEMENTATION_INCLUDED
#define UNLOCKS_IMPLEMENTATION_INCLUDED


#define  NUM_UNLOCKS  3

static  char           unlockEnabled      [ NUM_UNLOCKS ];

static  int            unlockPosX         [ NUM_UNLOCKS ];
static  int            unlockPosY         [ NUM_UNLOCKS ];

static  int            unlockSprites      [ NUM_UNLOCKS ];

static  unsigned char  unlockExtraGlowFade[ NUM_UNLOCKS ];
static  int            unlockFrameSprite;
static  int            unlockGlintSprite;


void unlocksInit( void ) {

    int  i;

    int  offsetX  =  15;
    int  curX     =  - ( ( NUM_UNLOCKS - 1 ) * offsetX ) / 2;

    unlockFrameSprite = maxigin_initSprite( "unlockFrame.tga" );
    unlockGlintSprite = maxigin_initSprite( "unlockGlint.tga" );

    unlockSprites[0] = maxigin_initSprite( "unlockSideboard.tga" );
    unlockSprites[1] = maxigin_initSprite( "unlockShopSlot.tga"  );
    unlockSprites[2] = maxigin_initSprite( "unlockSale.tga"      );

    for( i = 0;
         i < NUM_UNLOCKS;
         i ++ ) {
        maxigin_initMakeGlowSprite( unlockSprites[ i ],
                                    4,
                                    2 );
        unlockPosX[ i ] = curX;
        unlockPosY[ i ] = 0;

        curX += offsetX;

        unlockEnabled[ i ] = 1;

        unlockExtraGlowFade[ i ] = 0;
        }

    unlockEnabled[ 0 ] = 1;
    }


void unlocksDraw( int  inPosX,
                  int  inPosY ) {

    int  i;
    int  j;
    int  glowRep  =  3;
    

    maxigin_drawResetColor();
    
    for( i = 0;
         i < NUM_UNLOCKS;
         i ++ ) {
        int  x  =  unlockPosX[ i ] + inPosX;
        int  y  =  unlockPosY[ i ] + inPosY;
        
        maxigin_drawSprite( unlockFrameSprite,
                            x,
                            y );
        
        if( ! unlockEnabled[ i ] ) {

            maxigin_drawSprite( unlockGlintSprite,
                                x,
                                y );
            }
        else {
            maxigin_drawSprite( unlockSprites[ i ],
                                x,
                                y );

            
            
            for( j = 0;
                 j < glowRep - 1;
                 j ++ ) {
                maxigin_drawSpriteGlowOnly( unlockSprites[ i ],
                                            x,
                                            y );
                }

            if( unlockExtraGlowFade[ i ] > 0 ) {
                maxigin_drawSetAlpha( unlockExtraGlowFade[ i ] );
                
                for( j = 0;
                     j < glowRep - 1;
                     j ++ ) {
                    maxigin_drawSpriteGlowOnly( unlockSprites[ i ],
                                                x,
                                                y );
                    }
                
                maxigin_drawResetColor();
                }
            }
        }
    }



int unlocksStep( int  inPosX,
                 int  inPosY  ) {

    int  i;

    int  pointerX;
    int  pointerY;
    int  w;
    int  h;

    int  overI      =  -1;
    int  r          =  mingin_getStepsPerSecond();
    int  deltaFade  =  ( 20 * 60 ) / r;

    maxigin_getSpriteDimensions( unlockFrameSprite,
                                 &w,
                                 &h );

    w /= 2;
    h /= 2;

    if( maxigin_getPointerLocation( &pointerX,
                                    &pointerY ) ) {
        
        for( i = 0;
             i < NUM_UNLOCKS;
             i ++ ) {
            
            int  x   =  unlockPosX[ i ] + inPosX;
            int  y   =  unlockPosY[ i ] + inPosY;

            int  dx  =  pointerX - x;
            int  dy  =  pointerY - y;

            if( dx >= -w
                &&
                dx <=  w
                &&
                dy >= -h
                &&
                dy <=  h ) {

                unlockExtraGlowFade[ i ] = 255;

                overI = i;
                break;
                }
            }
        }

    
    for( i = 0;
         i < NUM_UNLOCKS;
         i ++ ) {

        if( i != overI
            &&
            unlockExtraGlowFade[ i ] > 0 ) {

            int  newHighlight = unlockExtraGlowFade[i] - deltaFade;

            if( newHighlight > 0 ) {
                unlockExtraGlowFade[i] = (unsigned char)newHighlight;
                }
            else {
                unlockExtraGlowFade[i] = 0;
                }
            }
        }

    /* fixme */
    return -1;
    }

    

#endif



#endif
