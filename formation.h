/*
  Include in your C code wherever like so:

      #include "formation.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define FORMATION_IMPLEMENTATION
      #include "formation.h"

*/

#ifndef FORMATION_H_INCLUDED
#define FORMATION_H_INCLUDED


void formationInit( int  inPointerActionHandle );


void formationDraw( int  inBoardCenterX,
                    int  inBoardCenterY );


/* returns 1 if done adjusting formation */
char formationStep( int  inBoardCenterX,
                    int  inBoardCenterY,
                    int  inPieceLiftSound );


/* returns 0 for empty, 1 for regular piece, 2 for king */
char formationGet( int  inRow,
                   int  inCol );

void formationAddNewSpot( void );


/* back to starting formation */
void formationBackToStart( void );


char formationHasRoomForNewSpot( void );


int formationGetSpotSprite( void );

int formationGetNewSpotLangHandle( void );



#endif



#ifdef  FORMATION_IMPLEMENTATION

#ifndef FORMATION_IMPLEMENTATION_INCLUDED
#define FORMATION_IMPLEMENTATION_INCLUDED


#include "chess.h"
#include "moveAnim.h"

#include "memoryRegister.h"

#include "button.h"


/* 0 empty, 1 regular piece, 2 king */
static  char           formation             [BH][BW];
static  unsigned char  formationHighlightFade[BH][BW];

static  int            formationPickedY;
static  int            formationPickedX;

static  int            fmPointerActionHandle   =  -1;

static  int            fmSpotSprite            =  -1;
static  int            fmSpotPickedSprite      =  -1;
static  int            fmSpotKingSprite        =  -1;
static  int            fmSpotKingPickedSprite  =  -1;

static  char           fmActionDown            =   0;
static  int            fmDoneButton            =  -1;

static  char           fmNewSpotWaiting        =   0;
static  int            formationSize           =   0;

static  int            lang_newSpot;



void formationInit( int  inPointerActionHandle ) {

    fmSpotSprite           = maxigin_initSprite( "formationSpot.tga"           );
    fmSpotPickedSprite     = maxigin_initSprite( "formationSpotPicked.tga"     );
    fmSpotKingSprite       = maxigin_initSprite( "formationKingSpot.tga"       );
    fmSpotKingPickedSprite = maxigin_initSprite( "formationKingSpotPicked.tga" );

    maxigin_initMakeGlowSprite( fmSpotSprite,
                                4,
                                2 );
    maxigin_initMakeGlowSprite( fmSpotPickedSprite,
                                4,
                                2 );
    maxigin_initMakeGlowSprite( fmSpotKingSprite,
                                4,
                                2 );
    maxigin_initMakeGlowSprite( fmSpotKingPickedSprite,
                                4,
                                2 );

    fmPointerActionHandle = inPointerActionHandle;

    formationBackToStart();


    fmDoneButton = buttonInit( maxigin_initSprite( "doneButton.tga" ),
                               -1,
                               maxigin_initSprite( "doneButtonPressed.tga" ),
                               50,
                               MAXIGIN_GAME_NATIVE_H - 10,
                               0,
                               fmPointerActionHandle,
                               /* fixme... need controller mapping for this */
                               -1 );

    lang_newSpot = maxigin_initTranslationKey( "newFormationSpot" );

    REGISTER_ARRAY_MEM( formation );
    REGISTER_ARRAY_MEM( formationHighlightFade );
    REGISTER_VAL_MEM  ( formationPickedY );
    REGISTER_VAL_MEM  ( formationPickedX );

    REGISTER_VAL_MEM  ( fmNewSpotWaiting );
    }



void formationDraw( int  inBoardCenterX,
                    int  inBoardCenterY ) {

    int  y;
    int  x;

    /* black box behind to cover deck when board slides up */
    maxigin_drawSetColor( 0,
                          0,
                          0,
                          255 );
    maxigin_drawFillRect( inBoardCenterX - ( BW * BOARD_SQUARE_SIZE ) / 2,
                          inBoardCenterY - ( BH * BOARD_SQUARE_SIZE ) / 2,
                          inBoardCenterX + ( BW * BOARD_SQUARE_SIZE ) / 2,
                          inBoardCenterY + ( BH * BOARD_SQUARE_SIZE ) / 2 );
                          
    /* last 3 rows on white's side */
    maxigin_drawResetColor();
    boardDrawPortion( inBoardCenterX,
                      inBoardCenterY,
                      5 );

    for( y = 0;
         y < BH;
         y ++ ) {
        for( x = 0;
             x < BW;
             x ++ ) {

            int  f  =  formation[ y ][ x ];
            

            if( f  > 0 ) {

                int  cX;
                int  cY;
                int  s;
                
                boardGetSquareCenter( inBoardCenterX,
                                      inBoardCenterY,
                                      y,
                                      x,
                                      &cX,
                                      &cY );

                if( f == 1 ) {
                    s = fmSpotSprite;

                    if( formationPickedY == y
                        &&
                        formationPickedX == x ) {
                        s = fmSpotPickedSprite;
                        }
                    }
                if( f == 2 ) {
                    s = fmSpotKingSprite;

                    if( formationPickedY == y
                        &&
                        formationPickedX == x ) {
                        s = fmSpotKingPickedSprite;
                        }
                    }

                maxigin_drawResetColor();
                
                maxigin_drawSprite( s,
                                    cX,
                                    cY );

                if( formationHighlightFade[ y ][ x ] > 0 ) {


                    maxigin_drawSetAlpha( formationHighlightFade[ y ][ x ] );
                    
                    maxigin_drawSpriteGlowOnly( s,
                                                cX,
                                                cY );
                    }  
                }
            }
        }

    if( fmNewSpotWaiting ) {
        maxigin_drawResetColor();

        maxigin_setLanguageFontIndex( 1 );
        
        maxigin_drawLangText(
                lang_newSpot,
                inBoardCenterX,
                inBoardCenterY - 70,
                MAXIGIN_CENTER );
        maxigin_setLanguageFontIndex( 0 );
        }
    else {
        /* hide done button if new spot waiting */

        buttonDraw( fmDoneButton );
        }
    }



char formationStep( int  inBoardCenterX,
                    int  inBoardCenterY,
                    int  inPieceLiftSound ) {
    

    int  pointerX;
    int  pointerY;
    int  y;
    int  x;
    int  r              =  mingin_getStepsPerSecond();
    int  deltaFade      =  ( 20 * 60 ) / r;
    int  overSlotX      =  -1;
    int  overSlotY      =  -1;

    int  squareR        =  BOARD_SQUARE_SIZE / 2;
    
    if( maxigin_getPointerLocation( &pointerX,
                                    &pointerY ) ) {

        for( y = 0;
             y < BH;
             y ++ ) {
            for( x = 0;
                 x < BW;
                 x ++ ) {

                int  cX;
                int  cY;
                
                boardGetSquareCenter( inBoardCenterX,
                                      inBoardCenterY,
                                      y,
                                      x,
                                      &cX,
                                      &cY );

                if( pointerX > cX - squareR
                    &&
                    pointerX < cX + squareR
                    &&
                    pointerY > cY - squareR
                    &&
                    pointerY < cY + squareR ) {

                    if( formation[ y ][ x ] != 0
                        &&
                        formationHighlightFade[ y ][ x ] < 255 ) {
                        
                        maxigin_playSoundEffect( inPieceLiftSound,
                                                 256 );
                        }
                    formationHighlightFade[ y ][ x ] = 255;

                    overSlotX = x;
                    overSlotY = y;
                    }
                }
            }
        }
    
    for( y = 0;
         y < BH;
         y ++ ) {
        for( x = 0;
             x < BW;
             x ++ ) {


            if( ( y != overSlotY
                  ||
                  x != overSlotX )
                &&
                formationHighlightFade[ y ][ x ] > 0 ) {

                int  newHighlight =
                    formationHighlightFade[ y ][ x ] - deltaFade;

                if( newHighlight > 0 ) {
                    formationHighlightFade[ y ][ x ] =
                        (unsigned char)newHighlight;
                    }
                else {
                    formationHighlightFade[ y ][ x ] = 0;
                    }
                }
            }

        }


    if( ! fmActionDown
        &&
        maxigin_isButtonDown( fmPointerActionHandle ) ) {

        if( overSlotX != -1
            &&
            overSlotY != -1
            &&
            overSlotY >= 5 ) {

            if( formationPickedX == overSlotX
                &&
                formationPickedY == overSlotY ) {
                
                formationPickedX = -1;
                formationPickedY = -1;
                playBeepDownSound();
                }
            else if( formationPickedX == -1
                     &&
                     formationPickedY == -1 ) {

                if( formation[ overSlotY ][ overSlotX ] != 0 ) {
                    formationPickedX = overSlotX;
                    formationPickedY = overSlotY;
                    playBeepUpSound();
                    }
                }
            else if( ! fmNewSpotWaiting
                     ||
                     formation[ overSlotY ][ overSlotX ] == 0 ) {
                
                /* swap */

                char  temp  =  formation[ formationPickedY ][ formationPickedX ];

                formation[ formationPickedY ][ formationPickedX ] =
                    formation[ overSlotY ][ overSlotX ];

                formation[ overSlotY ][ overSlotX ] = temp;

                formationPickedX = -1;
                formationPickedY = -1;
                playBeepDownSound();

                fmNewSpotWaiting = 0;
                }

            }

        fmActionDown = 1;
        }

    if( ! maxigin_isButtonDown( fmPointerActionHandle ) ) {
        fmActionDown = 0;
        }
    

    if( ! fmNewSpotWaiting
        &&
        buttonIsNewPressed( fmDoneButton ) ) {
        formationPickedX = -1;
        formationPickedY = -1;
        return 1;
        }
    return 0;
    }



char formationGet( int  inRow,
                   int  inCol ) {
    return formation[ inRow ][ inCol ];
    }


void formationBackToStart( void ) {
    
    int  y;
    int  x;
    
    for( y = 0;
         y < BH;
         y ++ ) {
        for( x = 0;
             x < BW;
             x ++ ) {

            formation             [ y ][ x ] = 0;
            formationHighlightFade[ y ][ x ] = 0;
            }
        }
    
    formationPickedY = -1;
    formationPickedX = -1;

    /* default starting formation
       k in back, 2 pieces in front */
    
    /* king */
    formation[ 7 ][ 5 ] = 2;
    
    formation[ 6 ][ 4 ] = 1;
    formation[ 6 ][ 5 ] = 1;

    formationSize = 3;
    }


void formationAddNewSpot( void ) {
    
    formation[ 3 ][ 4 ] = 1;

    /* force it to be picked so they have to move it next time */
    formationPickedY = 3;
    formationPickedX = 4;

    fmNewSpotWaiting = 1;

    formationSize ++;
    }



char formationHasRoomForNewSpot( void ) {

    /* can fill first 3 rows */
    if( formationSize < BW * 3 ) {
        return 1;
        }
    else {
        return 0;
        }
    }



int formationGetSpotSprite( void ) {
    return fmSpotSprite;
    }


int formationGetNewSpotLangHandle( void ) {
    return lang_newSpot;
    }



#endif

#endif
