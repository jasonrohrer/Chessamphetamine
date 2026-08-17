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


void formationInit( int  inPointerActionHandle,
                    int  inBoardCenterX,
                    int  inBoardCenterY );


void formationDraw( void );


/* returns 1 if done adjusting formation */
char formationStep( void );



#endif



#ifdef  FORMATION_IMPLEMENTATION

#ifndef FORMATION_IMPLEMENTATION_INCLUDED
#define FORMATION_IMPLEMENTATION_INCLUDED


#include "chess.h"
#include "moveAnim.h"

#include "memoryRegister.h"


/* 0 empty, 1 regular piece, 2 king */
static  char  formation             [BH][BW];
static  int   formationHighlightFade[BH][BW];

static  int   formationPickedY;
static  int   formationPickedX;

static  int   fmBoardCenterX;
static  int   fmBoardCenterY;
static  int   fmPointerActionHandle   =  -1;

static  int   fmSpotSprite            =  -1;
static  int   fmSpotPickedSprite      =  -1;
static  int   fmSpotKingSprite        =  -1;
static  int   fmSpotKingPickedSprite  =  -1;

static  char  fmActionDown            =   0;



void formationInit( int  inPointerActionHandle,
                    int  inBoardCenterX,
                    int  inBoardCenterY ) {

    int  y;
    int  x;

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
    
    
    fmBoardCenterX = inBoardCenterX;
    fmBoardCenterY = inBoardCenterY;

    fmPointerActionHandle = inPointerActionHandle;

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
       k in back, 3 pieces in front */
    
    /* king */
    formation[ 7 ][ 4 ] = 2;

    formation[ 6 ][ 3 ] = 1;
    formation[ 6 ][ 4 ] = 1;
    formation[ 6 ][ 5 ] = 1;
    

    REGISTER_ARRAY_MEM( formation );
    REGISTER_ARRAY_MEM( formationHighlightFade );
    REGISTER_VAL_MEM  ( formationPickedY );
    REGISTER_VAL_MEM  ( formationPickedX );
    }



void formationDraw( void ) {

    int  y;
    int  x;
    
    /* last 3 rows on white's side */
    maxigin_drawResetColor();
    boardDrawPortion( fmBoardCenterX,
                      fmBoardCenterY,
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
                
                boardGetSquareCenter( fmBoardCenterX,
                                      fmBoardCenterY,
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


                    maxigin_drawSetAlpha(
                        (unsigned char)( formationHighlightFade[ y ][ x ] ) );
                    
                    maxigin_drawSpriteGlowOnly( s,
                                                cX,
                                                cY );
                    }  
                }
            }
        }
    }



char formationStep( void ) {
    

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
                
                boardGetSquareCenter( fmBoardCenterX,
                                      fmBoardCenterY,
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

                    formationHighlightFade[ y ][ x ] = 255;

                    overSlotX = x;
                    overSlotY = y;

                    maxigin_logInt2( "Over ",
                                     overSlotX,
                                     ", ",
                                     overSlotY,
                                     " " );
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
                
                formationPickedX = overSlotX;
                formationPickedY = overSlotY;
                playBeepUpSound();
                }
            else {
                /* swap */

                char  temp  =  formation[ formationPickedY ][ formationPickedX ];

                formation[ formationPickedY ][ formationPickedX ] =
                    formation[ overSlotY ][ overSlotX ];

                formation[ overSlotY ][ overSlotX ] = temp;

                formationPickedX = -1;
                formationPickedY = -1;
                playBeepDownSound();
                }

            }

        fmActionDown = 1;
        }

    if( ! maxigin_isButtonDown( fmPointerActionHandle ) ) {
        fmActionDown = 0;
        }
    

    return 0;
    }


#endif

#endif
