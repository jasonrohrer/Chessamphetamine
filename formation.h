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


void formationInit( int  inBoardCenterX,
                    int  inBoardCenterY );


void formationDraw( void );


/* returns 1 if done adjusting formation */
char formationStep( void );



#endif



#ifdef  FORMATION_IMPLEMENTATION

#ifndef FORMATION_IMPLEMENTATION_INCLUDED
#define FORMATION_IMPLEMENTATION_INCLUDED


#include "chess.h"

#include "memoryRegister.h"


/* 0 empty, 1 regular piece, 2 king */
static  char  formation             [BH][BW];
static  int   formationHighlightFade[BH][BW];

static  int   formationPickedY;
static  int   formationPickedX;

static  int   fmBoardCenterX;
static  int   fmBoardCenterY;

static  int   fmSpotSprite            =  -1;
static  int   fmSpotPickedSprite      =  -1;
static  int   fmSpotKingSprite        =  -1;
static  int   fmSpotKingPickedSprite  =  -1;



void formationInit( int  inBoardCenterX,
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
                    maxigin_drawSprite( fmSpotSprite,
                                        cX,
                                        cY );
                    }
                if( f == 2 ) {
                    s = fmSpotKingSprite;

                    if( formationPickedY == y
                        &&
                        formationPickedX == x ) {
                        s = fmSpotKingPickedSprite;
                        }
                    }

                maxigin_drawSprite( s,
                                    cX,
                                    cY );
                }
            }
        }
    }



#endif

#endif
