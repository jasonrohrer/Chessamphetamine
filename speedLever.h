/*
  Include in your C code wherever like so:

      #include "speedLever.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define SPEED_LEVER_H_INCLUDED
      #include "speedLever.h"

*/

#ifndef SPEED_LEVER_H_INCLUDED
#define SPEED_LEVER_H_INCLUDED


void speedLeverInit( int  inPosX,
                     int  inPosY,
                     int  inPointerActionHandle,
                     int  inSpeedActionHandle );

void speedLeverDraw( void );


void speedLeverStep( int  inChangeSound );

int speedLeverGetSlowDownFactor( void );


#endif



#ifdef  SPEED_LEVER_IMPLEMENTATION

#ifndef SPEED_LEVER_IMPLEMENTATION_INCLUDED
#define SPEED_LEVER_IMPLEMENTATION_INCLUDED


#include "liveValueTest.h"

#include "memoryRegister.h"


static  int   leverSlotSprite            =  -1;
static  int   leverSlotBottomSprite      =  -1;
static  int   leverSprite                =  -1;
static  int   leverStemSprite            =  -1;

static  int   leverPosX;
static  int   leverPosY;
static  int   leverPointerActionHandle;
static  int   leverSpeedActionHandle;

static  int   slowDownFactor             =   1;
static  int   maxSlowDownFactor          =   4;

static  char  leverButtonDown            =   0;
static  char  leverButtonDir             =   1;

static  int   lang_speedTip;


void speedLeverInit( int  inPosX,
                     int  inPosY,
                     int  inPointerActionHandle,
                     int  inSpeedActionHandle ) {

    leverPosX = inPosX;
    leverPosY = inPosY;
    
    leverPointerActionHandle = inPointerActionHandle;
    leverSpeedActionHandle = inSpeedActionHandle;

    leverSlotSprite = maxigin_initSprite( "speedLeverSlot.tga" );
    leverSlotBottomSprite = maxigin_initSprite( "speedLeverSlotBottom.tga" );

    leverSprite = maxigin_initSprite( "speedLever.tga" );
    leverStemSprite = maxigin_initSprite( "speedLeverStem.tga" );

    lang_speedTip = maxigin_initTranslationKey( "speedTip" );
    
    REGISTER_VAL_MEM( slowDownFactor );
    REGISTER_VAL_MEM( leverButtonDown );
    REGISTER_VAL_MEM( leverButtonDir );
    }



static int getKnobY( void ) {
    
    int  knobY  =  leverPosY - 11;
    
    knobY += 19 * ( slowDownFactor - 1 ) / ( maxSlowDownFactor - 1 );

    return knobY;
    }



void speedLeverStep( int  inChangeSound ) {

    int  pointerX;
    int  pointerY;

    tipDisplaySet( lang_questPick,
                           -1,
                           -1,
                           1,
                           0 );

    if( maxigin_getActivePointerLocation( &pointerX,
                                          &pointerY ) ) {

        if( pointerX < leverPosX + 10
            &&
            pointerX > leverPosX - 10
            &&
            pointerY < leverPosY + 12
            &&
            pointerY > leverPosY - 17 ) {

            
            tipDisplaySet( lang_speedTip,
                           100 / slowDownFactor,
                           -1,
                           1,
                           0 );
            
            if( maxigin_isButtonDown( leverPointerActionHandle ) ) {
                
                int  knobY     =  getKnobY();
                int  i;
                int  bestI;
                int  minYDist  =  1000;
                int  dist;
                int  oldVal    =  slowDownFactor;
                
                for( i =  1;
                     i <= maxSlowDownFactor;
                     i    ++ ) {

                    slowDownFactor = i;

                    knobY = getKnobY();

                    dist = knobY - pointerY;

                    if( dist < 0 ) {
                        dist *= -1;
                        }
                    if( dist < minYDist ) {
                        bestI = i;
                        minYDist = dist;
                        }
                    }

                slowDownFactor = bestI;
                 

                if( slowDownFactor != oldVal ) {

                    maxigin_playSoundEffect( inChangeSound,
                                             256 );
                    }
                }
            }
        }
    else if( maxigin_isButtonDown( leverSpeedActionHandle ) ) {

        if( ! leverButtonDown ) {

            slowDownFactor += leverButtonDir;

            maxigin_playSoundEffect( inChangeSound,
                                     256 );

            if( leverButtonDir > 0
                &&
                slowDownFactor > maxSlowDownFactor ) {
                leverButtonDir = -1;
                slowDownFactor = maxSlowDownFactor - 1;
                }
            if( leverButtonDir < 0
                &&
                slowDownFactor < 1 ) {
                leverButtonDir = 1;
                slowDownFactor = 2;
                }
            }

        leverButtonDown = 1;
        }
    else {
        leverButtonDown = 0;
        }
    }


void speedLeverDraw( void ) {

    int  knobY  =  getKnobY();
    
    maxigin_drawResetColor();

    maxigin_drawSprite( leverSlotSprite,
                        leverPosX,
                        leverPosY );

    maxigin_drawSprite( leverStemSprite,
                        leverPosX - 4,
                        knobY + 2 );

    maxigin_drawSprite( leverSlotBottomSprite,
                        leverPosX - 4,
                        leverPosY + 10 );
    
    maxigin_drawSprite( leverSprite,
                        leverPosX - 5,
                        knobY );

    maxigin_drawAddButtonHint(
        leverSpeedActionHandle,
                    leverPosX - 11,
                    leverPosY - 10 );
    }



int speedLeverGetSlowDownFactor( void ) {
    return slowDownFactor;
    }



#endif



#endif

