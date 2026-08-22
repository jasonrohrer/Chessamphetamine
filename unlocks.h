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


/* register that player checkmated a level.
   Checkmating certain levels may unlock things */
void unlockBeatLevel( int  inLevel );



void unlocksDraw( int  inPosX,
                  int  inPosY );


void unlocksStep( int  inPosX,
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

static  int            unlockAtLevel      [ NUM_UNLOCKS ];

static  int            unlockFrameSprite;
static  int            unlockGlintSprite;
static  int            unlockSound                           =  -1;
static  int            unlockLastOver                        =  -1;


static  int            lang_unlockDes;
static  int            lang_unlocks       [ NUM_UNLOCKS ];
static  int            lang_lockedA;
static  int            lang_lockedB;


static void unlocksWrite( void ) {}


static void unlocksRead( void ) {}



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

        unlockEnabled[ i ] = 0;

        unlockExtraGlowFade[ i ] = 0;
        }

    unlockAtLevel[ 0 ] = 8;
    unlockAtLevel[ 1 ] = 16;
    unlockAtLevel[ 2 ] = 32;

    unlockSound = maxigin_initSoundEffect( "unlock_sd_24.wav" );


    lang_unlockDes    = maxigin_initTranslationKey( "unlockDes" );
    lang_unlocks[ 0 ] = maxigin_initTranslationKey( "unlock0"   );
    lang_unlocks[ 1 ] = maxigin_initTranslationKey( "unlock1"   );
    lang_unlocks[ 2 ] = maxigin_initTranslationKey( "unlock2"   );

    lang_lockedA      = maxigin_initTranslationKey( "unlockLockedA" );
    lang_lockedB      = maxigin_initTranslationKey( "unlockLockedB" );
                                        
    
    unlocksRead();

    REGISTER_ARRAY_MEM( unlockEnabled       );
    REGISTER_ARRAY_MEM( unlockExtraGlowFade );
    REGISTER_VAL_MEM( unlockLastOver );
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

    if( unlockLastOver != -1
        &&
        unlockExtraGlowFade[ unlockLastOver ] > 0 ) {

        int  centX  =  MAXIGIN_GAME_NATIVE_W - 42;
        int  centY  =  MAXIGIN_GAME_NATIVE_H / 2;

        
        if( unlockEnabled[ unlockLastOver ] ) {

            drawDescriptionText( lang_unlockDes,
                                 lang_unlocks[ unlockLastOver ],
                                 centX,
                                 centY,
                                 unlockExtraGlowFade[ unlockLastOver ] );

            maxigin_drawSetColor( 70,
                                  198,
                                  87,
                                  unlockExtraGlowFade[ unlockLastOver ] );
            
            drawDescriptionFrame( centX,
                                  centY );
            }
        else {

            maxigin_drawResetColor();

            maxigin_setLanguageFontIndex( 1 );

            maxigin_drawSetAlpha( unlockExtraGlowFade[ unlockLastOver ] );

            maxigin_drawLangText( lang_lockedA,
                                  centX,
                                  centY - 15,
                                  MAXIGIN_CENTER );

            numberDrawCenter( unlockAtLevel[ unlockLastOver ],
                              centX,
                              centY,
                              0 );

            maxigin_setLanguageFontIndex( 1 );

            maxigin_drawLangText( lang_lockedB,
                                  centX,
                                  centY + 15,
                                  MAXIGIN_CENTER );
            
            maxigin_setLanguageFontIndex( 0 );
            }
        
        }
    }



void unlocksStep( int  inPosX,
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
                unlockLastOver = i;
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
    }



void unlockBeatLevel( int  inLevel ) {
    int  i;
    
    for( i = 0;
         i < NUM_UNLOCKS;
         i ++ ) {

        if( ! unlockEnabled[ i ]
            &&
            unlockAtLevel[ i ] <= inLevel ) {

            maxigin_playSoundEffect( unlockSound,
                                     512 );

            unlockEnabled[ i ] = 1;

            unlockExtraGlowFade[ i ] = 255;

            unlocksWrite();

            /* at most one unlock per level beaten */
            
            return;
            }
        }
    
    }



    

#endif



#endif
