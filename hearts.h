/*
  Include in your C code wherever like so:

      #include "hearts.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define HEARTS_IMPLEMENTATION
      #include "hearts.h"

*/

#ifndef HEARTS_H_INCLUDED
#define HEARTS_H_INCLUDED


void heartsInit( void );


void heartsReset( void );

void heartsGain( void );

void heartsLose( void );


char heartsIsDead( void );


void heartsDraw( int  inPosX,
                 int  inPosY );


void heartsStep( void );



#ifdef HEARTS_IMPLEMENTATION


#include "maxigin.h"
#include "memoryRegister.h"
#include "util.h"


#define                HEARTS_MAX                            8

static  int            heartsHousingSprite               =  -1;
static  int            heartsGlintSprite                 =  -1;
static  int            heartsShineSprite                 =  -1;

static  int            heartsSkullHousingSprite          =  -1;
static  int            heartsSkullGlintSprite            =  -1;
static  int            heartsSkullShineSprite            =  -1;
static  int            heartsSkullDarkEyesSprite         =  -1;

/* no sound for losing a hear, b/c that happens during checkmate,
   which already has a sound */
static  int            heartsGainSound                   =  -1;

static  int            heartsCount                       =   0;
static  int            heartsStarting                    =   3;

static  unsigned char  heartsFlashCount[ HEARTS_MAX ];
static  unsigned char  heartsShineFade [ HEARTS_MAX ];
static  unsigned char  heartsSkullShineFade              =   0;
static  unsigned char  heartsSkullFlashCount             =   0;


static void heartsResetFade( void ) {

    int  i;

    for( i = 0;
         i < heartsCount;
         i ++ ) {

        heartsShineFade [ i ] = 255;
        heartsFlashCount[ i ] = 0;
        }
    for( i = heartsCount;
         i < HEARTS_MAX;
         i ++ ) {

        heartsShineFade [ i ] = 0;
        heartsFlashCount[ i ] = 0;
        }
    heartsSkullShineFade  = 0;
    heartsSkullFlashCount = 0;
    }


void heartsInit( void ) {
    heartsHousingSprite = maxigin_initSprite( "heartLightHousing.tga" );

    heartsGlintSprite = maxigin_initSprite( "heartLightGlint.tga" );

    heartsShineSprite = maxigin_initSprite( "heartLightShine.tga" );

    maxigin_initMakeGlowSprite( heartsShineSprite,
                                4,
                                2 );

    heartsSkullHousingSprite = maxigin_initSprite( "skullLightHousing.tga" );

    heartsSkullGlintSprite = maxigin_initSprite( "skullLightGlint.tga" );

    heartsSkullShineSprite = maxigin_initSprite( "skullLightShine.tga" );

    maxigin_initMakeGlowSprite( heartsSkullShineSprite,
                                4,
                                2 );

    heartsSkullDarkEyesSprite = maxigin_initSprite( "skullLightDarkEyes.tga" );

    heartsGainSound = maxigin_initSoundEffect( "heartGain_sd_2.wav" );

    heartsReset();
    heartsResetFade();
    
    REGISTER_VAL_MEM  ( heartsCount           );
    REGISTER_ARRAY_MEM( heartsShineFade       );
    REGISTER_VAL_MEM  ( heartsSkullShineFade  );

    REGISTER_ARRAY_MEM( heartsFlashCount      );
    REGISTER_VAL_MEM  ( heartsSkullFlashCount );
    }



void heartsReset( void ) {
    while( heartsCount < heartsStarting ) {
        heartsFlashCount[ heartsCount ] = 3;
        
        heartsCount ++;
        }
    while( heartsCount > heartsStarting ) {
        heartsFlashCount[ heartsCount - 1 ] = 3;
        
        heartsCount --;
        }
    }



void heartsGain( void ) {
    if( heartsCount < HEARTS_MAX ) {

        maxigin_playSoundEffect( heartsGainSound,
                                 256 );

        heartsFlashCount[ heartsCount ] = 3;
        
        heartsCount ++;
        }
    }



void heartsLose( void ) {
    if( heartsCount > 0 ) {

        if( heartsCount > 1 ) {
            heartsFlashCount[ heartsCount - 1 ] = 11;
            }
        else {
            /* going to 0, don't make last heart flash, but make skull
               flash as it comes on */
            heartsSkullFlashCount = 11;
            }
        
        heartsCount --;
        }
    }


char heartsIsDead( void ) {
    return heartsCount == 0;
    }



void heartsDraw( int  inPosX,
                 int  inPosY ) {

    int  i;
    int  j;
    int  offset     =  15;
    int  glowRep    =   3;
    int  glintFade  = 255;

    maxigin_drawResetColor();

    maxigin_drawSprite( heartsSkullHousingSprite,
                            inPosX,
                            inPosY );

    if( heartsSkullShineFade >  0 ) {

        maxigin_drawSetAlpha( heartsSkullShineFade );
        
        maxigin_drawSprite( heartsSkullShineSprite,
                            inPosX,
                            inPosY );
        maxigin_drawSprite( heartsSkullDarkEyesSprite,
                            inPosX,
                            inPosY );

        for( j = 0;
             j < glowRep - 1;
             j ++ ) {
            maxigin_drawSpriteGlowOnly( heartsSkullShineSprite,
                                        inPosX,
                                        inPosY );
            }
        
        maxigin_drawSpriteGlowOnly( heartsSkullShineSprite,
                                    inPosX,
                                    inPosY );

        if( heartsSkullShineFade > 127 ) {
            glintFade = ( ( 255 - heartsSkullShineFade ) * 255 ) / 127;
            }
        }

    
    if( glintFade > 0 ) {
        maxigin_drawSetAlpha( (unsigned char)glintFade );
            
        maxigin_drawSprite( heartsSkullGlintSprite,
                            inPosX,
                            inPosY );
        }
    

    
    inPosY -= offset;

    for( i = 0;
         i < HEARTS_MAX;
         i ++ ) {

        glintFade = 255;
        
        maxigin_drawResetColor();

        maxigin_drawSprite( heartsHousingSprite,
                            inPosX,
                            inPosY );

        if( heartsShineFade[ i ] > 0 ) {

            maxigin_drawSetAlpha( heartsShineFade[i] );
            
            maxigin_drawSprite( heartsShineSprite,
                                inPosX,
                                inPosY );

            for( j = 0;
                 j < glowRep;
                 j ++ ) {
                
                maxigin_drawSpriteGlowOnly( heartsShineSprite,
                                            inPosX,
                                            inPosY );
                }

            if( heartsShineFade[i] > 127 ) {

                glintFade = ( ( 255 - heartsShineFade[i] ) * 255 ) / 127;
                }
            }

        if( glintFade > 0 ) {
            maxigin_drawSetAlpha( (unsigned char)glintFade );
            
            maxigin_drawSprite( heartsGlintSprite,
                                inPosX,
                                inPosY );
            }

        inPosY -= offset;
        }
    

    }

    

void heartsStep( void ) {

    static  unsigned char  targetFade[ HEARTS_MAX ];
    
    int            i;
    unsigned char  targetSkullFade;
    int            r;
    int            d;
    
    /* fixme:  animate gain and loss */

    for( i = 0;
         i < HEARTS_MAX;
         i ++ ) {
        if( i < heartsCount ) {
            targetFade[ i ] = 255;
            }
        else {
            targetFade[ i ] = 0;
            }
        if( heartsFlashCount[i] > 0 ) {
            /* invert dir on even flashes */
            if( heartsFlashCount[i] % 2 == 0 ) {
                if( targetFade[i] == 0 ) {
                    targetFade[i] = 255;
                    }
                else {
                    targetFade[i] = 0;
                    }
                }
            }
        }
    if( heartsCount == 0 ) {
        targetSkullFade = 255;
        }
    else {
        targetSkullFade = 0;
        }

    if( heartsSkullFlashCount > 0 ) {
        /* invert dir on even flashes */
        if( heartsSkullFlashCount % 2 == 0 ) {
            if( targetSkullFade == 0 ) {
                targetSkullFade = 255;
                }
            else {
                targetSkullFade = 0;
                }
            }
        }

    r = mingin_getStepsPerSecond();

    d = ( 20 * 60 ) / r;
    

    for( i = 0;
         i < HEARTS_MAX;
         i ++ ) {

        heartsShineFade[i] = tweenToByte( heartsShineFade[i],
                                          targetFade[i],
                                          d );

        if( heartsFlashCount[i] > 0 ) {
            if( heartsShineFade[i] == targetFade[i] ) {
                heartsFlashCount[i] --;
                }
            }
            
        }

    heartsSkullShineFade = tweenToByte( heartsSkullShineFade,
                                        targetSkullFade,
                                        d );
    if( heartsSkullFlashCount > 0
        &&
        heartsSkullShineFade == targetSkullFade ) {
        heartsSkullFlashCount --;
        }

    return;     
    }


#endif



#endif
