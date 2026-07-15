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




static  int  heartsHousingSprite        =  -1;
static  int  heartsGlintSprite          =  -1;
static  int  heartsShineSprite          =  -1;

static  int  heartsSkullHousingSprite   =  -1;
static  int  heartsSkullGlintSprite     =  -1;
static  int  heartsSkullShineSprite     =  -1;
static  int  heartsSkullDarkEyesSprite  =  -1;

/* no sound for losing a hear, b/c that happens during checkmate,
   which already has a sound */
static  int  heartsGainSound       =  -1;


static  int  heartsCount           =   0;
static  int  heartsStarting        =   3;
static  int  heartsMax             =   8;

static  int  stepCount             =   0;



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
    }



void heartsReset( void ) {
    heartsCount = heartsStarting;
    }


void heartsGain( void ) {
    if( heartsCount < heartsMax ) {

        maxigin_playSoundEffect( heartsGainSound,
                                 256 );

        heartsCount ++;
        }
    }

void heartsLose( void ) {
    if( heartsCount > 0 ) {
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
    int  offset   = 15;
    int  glowRep  =  3;
    

    maxigin_drawResetColor();

    maxigin_drawSprite( heartsSkullHousingSprite,
                            inPosX,
                            inPosY );

    if( heartsCount == 0 ) {
        maxigin_drawSprite( heartsSkullShineSprite,
                            inPosX,
                            inPosY );
maxigin_drawSprite( heartsSkullDarkEyesSprite,
                            inPosX,
                            inPosY );

        for( j = 0;
             j < glowRep - 1;
             j ++ )
            maxigin_drawSpriteGlowOnly( heartsSkullShineSprite,
                                        inPosX,
                                        inPosY );
        
        maxigin_drawSpriteGlowOnly( heartsSkullShineSprite,
                                        inPosX,
                                        inPosY );
        }
    else {
        maxigin_drawSprite( heartsSkullGlintSprite,
                            inPosX,
                            inPosY );
        }
    

    
    inPosY -= offset;

    for( i = 0;
         i < heartsMax;
         i ++ ) {

        maxigin_drawSprite( heartsHousingSprite,
                            inPosX,
                            inPosY );

        if( i < heartsCount ) {
            
            maxigin_drawSprite( heartsShineSprite,
                                inPosX,
                                inPosY );

            for( j = 0;
                 j < glowRep;
                 j ++ )
            maxigin_drawSpriteGlowOnly( heartsShineSprite,
                                        inPosX,
                                        inPosY );

            }
        else {
            maxigin_drawSprite( heartsGlintSprite,
                                inPosX,
                                inPosY );
            }

        inPosY -= offset;
        }
    

    }


static  int  testDir  = 1;


void heartsStep( void ) {

    /* fixme:  animate gain and loss */

    stepCount ++;

    if( stepCount % 15 == 0 ) {

        heartsCount += testDir;

        if( heartsCount > heartsMax ) {
            testDir = - testDir;

            heartsCount = heartsMax - 1;
            }
        else if( heartsCount < 0 ) {
            testDir = - testDir;

            heartsCount = 1;
            }
        }
        
    }


#endif



#endif
