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

   If a step goes by without message being set again, it will start to fade

   A higher priority tip will block other tips from overriding it. */
void tipDisplaySet( int   inMessageKeyA,
                    int   inNumber,
                    int   inMessageKeyB,
                    char  inPriority,
                    char  inVertPosToggle );

/* returns 1 if a message is set and higher than 0 priority.
   returns 0 as soon as the message expires and starts to fade,
   or if a 0-priority message is showing.  */
char tipDisplayIsShowing( void );


void tipDisplayDraw( int  inCenterX,
                     int  inCenterY );


void tipDisplayStep( void );



#endif



#ifdef  TIP_DISPLAY_IMPLEMENTATION

#ifndef TIP_DISPLAY_IMPLEMENTATION_INCLUDED
#define TIP_DISPLAY_IMPLEMENTATION_INCLUDED

#include "memoryRegister.h"


#define  TD_MAX_NUM_LINES     5
#define  TD_MAX_LINE_LENGTH  20


static  int            tdLineSpace      =  10;

static  int            tdMessageKeyA    =  -1;
static  int            tdMessageKeyB    =  -1;
static  int            tdNumber         =  -1;
static  char           tdPriority       =  0;
static  unsigned char  tdFade           =  0;
static  int            tdStepsSinceSet  =  0;

static  int            tdFadePoint      =  255;

/* tips can be in two different vertical positions */
static  char           tdVertPosToggle  =  0;

static  int            tdNumLines;
static  char           tdLines[ TD_MAX_NUM_LINES ][ TD_MAX_LINE_LENGTH ];




void tipDisplayInit( void ) {

    REGISTER_VAL_MEM( tdMessageKeyA );
    REGISTER_VAL_MEM( tdMessageKeyB );
    REGISTER_VAL_MEM( tdNumber );
    REGISTER_VAL_MEM( tdPriority );
    REGISTER_VAL_MEM( tdFade );
    REGISTER_VAL_MEM( tdStepsSinceSet );
    REGISTER_VAL_MEM( tdVertPosToggle );
    }



void tipDisplaySet( int   inMessageKeyA,
                    int   inNumber,
                    int   inMessageKeyB,
                    char  inPriority,
                    char  inVertPosToggle ) {

    if( tdFade >= 255
        &&
        inPriority < tdPriority ) {
        /* higher priority tip already set, don't override it with this
           lower-priority tip */
        return;
        }
    
    tdMessageKeyA   = inMessageKeyA;
    tdMessageKeyB   = inMessageKeyB;
    tdNumber        = inNumber;
    tdPriority      = inPriority;
    tdVertPosToggle = inVertPosToggle;
    

    if( tdMessageKeyA != -1
        ||
        tdMessageKeyB != -1
        ||
        tdNumber      != -1 ) {

        tdStepsSinceSet = 0;
        tdFade = 255;
        }
    }



char tipDisplayIsShowing( void ) {
    if( tdPriority > 0
        &&
        tdFade >= tdFadePoint ) {
        
        return 1;
        }
    return 0;
    }



#define  TD_BUFFER_LEN  128

static  char  tdBuffer[ TD_BUFFER_LEN ];


static void tdBufferString( const char  *inString ) {
    int  i  =  0;

    while( inString[ i ] != '\0'
           &&
           i < TD_BUFFER_LEN ) {

        tdBuffer[ i ] = inString[ i ];

        i ++;
        }

    if( i < TD_BUFFER_LEN ) {
        tdBuffer[ i ] = '\0';
        }
    else {
        tdBuffer[ TD_BUFFER_LEN - 1 ] = '\0';
        }
    
    }



static void tdSplitLines( int  inMessageKey ) {

    const char  *text   =  maxigin_getLangText( inMessageKey );
    int          i      =  0;
    int          lineI  =  0;
    
    tdBufferString( text );


    tdNumLines = 0;

    while( tdBuffer[ i ] != '\0' ) {

        if( tdBuffer[ i ]     == '#'
            &&
            tdBuffer[ i + 1 ] == '#' ) {

            tdLines[ tdNumLines ][ lineI ] = '\0';

            /* remove trailing spaces from line that just ended */
            lineI --;
            
            while( tdLines[ tdNumLines ][ lineI ] == ' ' ) {
                tdLines[ tdNumLines ][ lineI ] = '\0';
                lineI --;
                }
            
            tdNumLines ++;
            
            lineI = 0;

            /* skip ## marker */
            i += 2;

            /* skip any leading spaces on next line */
            while( tdBuffer[ i ] == ' ' ) {
                i ++;
                }
            }
        else {
            tdLines[ tdNumLines ][ lineI ] = tdBuffer[ i ];

            lineI ++;
            i     ++;

            if( lineI >= TD_MAX_LINE_LENGTH ) {
                /* line too long, terminate it early */
                tdLines[ tdNumLines ][ lineI - 1 ] = '\0';

                tdNumLines ++;
                lineI = 0;
                }
            }
        
        if( tdNumLines >= TD_MAX_NUM_LINES ) {
            /* ran out of lines */
            return;
            }
        
        }

    if( lineI != 0 ) {
        tdLines[ tdNumLines ][ lineI ] = '\0';
        
        tdNumLines ++;
        }
    }


static void tdDrawLines( int  inCenterX,
                         int  inBottomLineCenterY ) {

    int  i;


    int  curY       =  inBottomLineCenterY - tdLineSpace * ( tdNumLines - 1 );
    

    for( i = 0;
         i < tdNumLines;
         i   ++ ) {

        maxigin_drawLangTextString( tdLines[i],
                                    inCenterX,
                                    curY,
                                    MAXIGIN_CENTER );
        
        curY += tdLineSpace;
        }

    }



void tipDisplayDraw( int  inCenterX,
                     int  inCenterY ) {
    
    if( tdFade > 0 ) {

        if( tdVertPosToggle ) {
            inCenterY += 30;
            }

        colorsApply( COLOR_LCD_TEXT );

        maxigin_drawSetAlpha( tdFade );

        maxigin_setLanguageFontIndex( 1 );

        if( tdNumber != -1 ) {
            inCenterY -= tdLineSpace + tdLineSpace / 2;
            }

        if( tdMessageKeyA != -1 ) {


            tdSplitLines( tdMessageKeyA );

            tdDrawLines( inCenterX,
                         inCenterY );

            inCenterY += tdLineSpace;
            }

        if( tdNumber != -1 ) {

            inCenterY += tdLineSpace / 2;
            
            numbersToggleBorder( 0 );

            numberDrawCenter( tdNumber,
                              inCenterX,
                              inCenterY,
                              1 );

            numbersToggleBorder( 1 );

            inCenterY += tdLineSpace + tdLineSpace / 2;
            }

        if( tdMessageKeyB != -1 ) {
            
            tdSplitLines( tdMessageKeyB );

            inCenterY += tdLineSpace * ( tdNumLines - 1 );

            tdDrawLines( inCenterX,
                         inCenterY );
            }
        

        maxigin_setLanguageFontIndex( 0 );
        }
    }



void tipDisplayStep( void ) {

    tdStepsSinceSet ++;

    if( tdStepsSinceSet > 3 ) {

        /* cap it so it doesn't keep incrementing and overflow */

        tdStepsSinceSet = 4;

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
