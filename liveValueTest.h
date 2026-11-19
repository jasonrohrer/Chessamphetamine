/*
  Include in your C code wherever like so:

      #include "liveValueTest.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define PARTICLE_SPRITES_IMPLEMENTATION
      #include "liveValueTest.h"

*/

#ifndef LIVE_VALUE_TEST_H_INCLUDED
#define LIVE_VALUE_TEST_H_INCLUDED


#define  MAX_NUM_LIVE_VALUES  6



/* returns the current tweaked value of the live value */
int liveValue( int  inIndex,
               int  inDefaultValue );

void liveValueStep( int  inUpActionHandle,
                    int  inDownActionHandle,
                    int  inNextActionHandle );

void liveValueDraw( int  inPosX,
                    int  inPosY );




#endif



#ifdef  LIVE_VALUE_TEST_IMPLEMENTATION

#ifndef LIVE_VALUE_TEST_IMPLEMENTATION_INCLUDED
#define LIVE_VALUE_TEST_IMPLEMENTATION_INCLUDED


#include "numbers.h"
#include "maxigin.h"

static  int   liveIndex                             =  -1;
static  char  isLive      [ MAX_NUM_LIVE_VALUES ]   =   { 0, 0, 0, 0, 0, 0};
static  int   liveValueMap[ MAX_NUM_LIVE_VALUES ];

static  char  liveButtonHeld  =  0;



int liveValue( int  inIndex,
               int  inDefaultValue ) {
    if( inIndex >= MAX_NUM_LIVE_VALUES ) {
        return inDefaultValue;
        }
    
    if( ! isLive[ inIndex ] ) {
        liveValueMap[ inIndex ] = inDefaultValue;
        }
    isLive[ inIndex ] = 1;

    if( liveIndex == -1 ) {
        /* never been set before, make first one live */ 
        liveIndex = inIndex;
        }

    return liveValueMap[ inIndex ];
    }



void liveValueStep( int  inUpActionHandle,
                    int  inDownActionHandle,
                    int  inNextActionHandle  ) {

    if( liveIndex == -1 ) {
        return;
        }

    if( liveButtonHeld
        &&
        ! maxigin_isButtonDown( inUpActionHandle )
        &&
        ! maxigin_isButtonDown( inDownActionHandle )
        &&
        ! maxigin_isButtonDown( inNextActionHandle ) ) {
        
        liveButtonHeld = 0;
        }
    

    if( maxigin_isButtonDown( inUpActionHandle ) ) {
        liveButtonHeld = 1;
        liveValueMap[ liveIndex ] ++;
        }
    else if( maxigin_isButtonDown( inDownActionHandle ) ) {
        liveButtonHeld = 1;
        liveValueMap[ liveIndex ] --;
        }
    else if( maxigin_isButtonDown( inNextActionHandle ) ) {
        liveButtonHeld = 1;

        liveIndex ++;
        if( liveIndex >= MAX_NUM_LIVE_VALUES ) {
            liveIndex = 0;
            }
        while( ! isLive[ liveIndex ] ) {
            liveIndex ++;
            if( liveIndex >= MAX_NUM_LIVE_VALUES ) {
                liveIndex = 0;
                }
            }
        }
    }


void liveValueDraw( int  inPosX,
                    int  inPosY ) {
    int  i;

    if( liveIndex == -1 ) {
        return;
        }

    

    for( i = 0;
         i < MAX_NUM_LIVE_VALUES;
         i   ++ ) {
        if( isLive[ i ] ) {

            maxigin_drawResetColor();

            if( i == liveIndex ) {
                maxigin_drawSetColor( 255,
                                      255,
                                      0,
                                      255 );
                }
            
            numberDraw( liveValueMap[ i ],
                        inPosX,
                        inPosY + 10 * i,
                        1 );
            }
        }
    }

#endif



#endif

