/*
  Include in your C code wherever like so:

      #include "tipDisplay.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define TIP_DISPLAY_IMPLEMENTATION
      #include "tipDisplay.h"

*/

#ifndef TIP_DISPLAY_H_INCLUDED
#define TIP_DISPLAY_H_INCLUDED


void tipDisplayInit( void );


/* message keys can be -1 to skip
   number will be skipped if it is -1

   Should be set every step to renew it, even if mouse is over
   the same thing.

   If a step goes by without message being set again, it will start to fade */
void tipDisplaySet( int  inMessageKeyA,
                    int  inNumber,
                    int  inMessageKeyB );

/* returns 1 if a message is set.
   returns 0 as soon as the message expires and starts to fade */
char tipDisplayIsShowing( void );


void tipDisplayDraw( int  inCenterX,
                     int  inCenterY );


void tipDisplayStep( void );



#endif



#ifdef  TIP_DISPLAY_IMPLEMENTATION

#ifndef TIP_DISPLAY_IMPLEMENTATION_INCLUDED
#define TIP_DISPLAY_IMPLEMENTATION_INCLUDED

#include "memoryRegister.h"


int            tdMessageKeyA   =  -1;
int            tdMessageKeyB   =  -1;
int            tdNumber        =  -1;
unsigned char  tdFade          =   0;
int            tdStepsSiceSet  =   0;



void tipDisplayInit( void ) {

    REGISTER_VAL_MEM( tdMessageKeyA );
    REGISTER_VAL_MEM( tdMessageKeyB );
    REGISTER_VAL_MEM( tdNumber );
    REGISTER_VAL_MEM( tdFade );
    REGISTER_VAL_MEM( tdStepsSiceSet );
    }



void tipDisplaySet( int  inMessageKeyA,
                    int  inNumber,
                    int  inMessageKeyB ) {
    
    tdMessageKeyA = inMessageKeyA;
    tdMessageKeyB = inMessageKeyB;
    tdNumber      = inNumber;

    if( tdMessageKeyA != -1
        ||
        tdMessageKeyB != -1
        ||
        tdNumber      != -1 ) {

        tdStepsSiceSet = 0;
        tdFade = 255;
        }
    }



char tipDisplayIsShowing( void ) {
    if( tdFade == 255 ) {
        return 1;
        }
    return 0;
    }


void tipDisplayDraw( int  inCenterX,
                     int  inCenterY ) {
    
    if( tdFade > 0 ) {

        colorsApply( COLOR_LCD_TEXT );

        maxigin_drawSetAlpha( tdFade );

        maxigin_setLanguageFontIndex( 1 );

        if( tdMessageKeyA != -1 ) {
            maxigin_drawLangText( tdMessageKeyA,
                                  inCenterX,
                                  inCenterY,
                                  MAXIGIN_CENTER );
            }

        maxigin_setLanguageFontIndex( 0 );
        }
    }


void tipDisplayStep( void ) {

    tdStepsSiceSet ++;

    if( tdStepsSiceSet > 2 ) {

        /* cap it so it doesn't keep incrementing and overflow */

        tdStepsSiceSet = 2;

        if( tdFade > 0 ) {

            int  r        =  mingin_getStepsPerSecond();

            int  newFade  =  tdFade - 20 * 60 / r;

            if( newFade < 0 ) {
                newFade = 0;
                }
            
            tdFade = (unsigned char)newFade;
            }
        }
    }


#endif



#endif
