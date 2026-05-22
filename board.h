/*
  Include in your C code wherever like so:

      #include "board.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define BOARD_IMPLEMENTATION
      #include "board.h"

*/

#ifndef  BOARD_H_INCLUDED
#define  BOARD_H_INCLUDED

#include "chess.h"


#define  BOARD_SQUARE_SIZE  25


void boardInit( void );


void boardDraw( int  inCenterX,
                int  inCenterY );


/* row and col are 0,0 at top left corner (a8) */
void boardGetSquareCenter( int   inBoardCenterX,
                           int   inBoardCenterY,
                           int   inRow,
                           int   inCol,
                           int  *outSquareCenterX,
                           int  *outSquareCenterY );



int boardGetPixelDistance( int  inRowA,
                           int  inColA,
                           int  inRowB,
                           int  inColB );



#ifdef BOARD_IMPLEMENTATION

#include "colors.h"

#include "fixedMath.h"


static  int  borderSpriteH;
static  int  borderSpriteV;

static  int  squareSpriteWhite;
static  int  squareSpriteBlack;

static  int  squareSize          =  BOARD_SQUARE_SIZE;







void boardInit( void ) {

    
    borderSpriteH = maxigin_initSprite( "boardEdgeH.tga" );

    maxigin_initMakeGlowSprite( borderSpriteH,
                                4,
                                2 );

    borderSpriteV = maxigin_initSprite( "boardEdgeV.tga" );

    maxigin_initMakeGlowSprite( borderSpriteV,
                                4,
                                2 );

    squareSpriteWhite = maxigin_initSprite( "squareWhite.tga" );

    maxigin_initMakeGlowSprite( squareSpriteWhite,
                                4,
                                2 );

    squareSpriteBlack = maxigin_initSprite( "squareBlack.tga" );

    maxigin_initMakeGlowSprite( squareSpriteBlack,
                                4,
                                2 );
    }


void boardDraw( int  inCenterX,
                int  inCenterY ) {

    int  y;
    int  x;
    int  pass;
    
    int  w;
    int  yOff  =  ( squareSize * BH ) / 2;
    int  xOff  =  ( squareSize * BW ) / 2;

    colorsApplyBoardColor();

    /* draw black squares in first pass, then white on top */
    for( pass = 0;
         pass < 2;
         pass ++ ) {

        w = 1;
        
        for( y = 0;
             y < BH;
             y ++ ) {

            int  yPos  =  inCenterY - yOff + y * squareSize + squareSize / 2;

            for( x = 0;
                 x < BW;
                 x ++  ) {

                int  xPos  =  inCenterX - xOff + x * squareSize + squareSize / 2;

                if( w ) {
                    if( pass == 1 ) {
                        maxigin_drawSprite( squareSpriteWhite,
                                            xPos,
                                            yPos );
                        }
                    }
                else {
                    if( pass == 0 ) {
                        maxigin_drawSprite( squareSpriteBlack,
                                            xPos,
                                            yPos );
                        }
                    }
                w = ! w;
                }
            /* after each row, re-toggle */
            w = ! w;
            }
        }
    

    maxigin_drawSprite( borderSpriteH,
                        inCenterX,
                        inCenterY - yOff );

    maxigin_drawSprite( borderSpriteH,
                        inCenterX,
                        inCenterY + yOff - 1 );
    
    maxigin_drawSprite( borderSpriteV,
                        inCenterX - xOff,
                        inCenterY );

    maxigin_drawSprite( borderSpriteV,
                        inCenterX + xOff - 1,
                        inCenterY );

    maxigin_drawResetColor();
    }



void boardGetSquareCenter( int   inBoardCenterX,
                           int   inBoardCenterY,
                           int   inRow,
                           int   inCol,
                           int  *outSquareCenterX,
                           int  *outSquareCenterY ) {

    *outSquareCenterY  =
        inBoardCenterY -  ( squareSize * BH ) / 2
        + inRow * squareSize + squareSize / 2;

    *outSquareCenterX  =
        inBoardCenterX - ( squareSize * BW ) / 2
        + inCol * squareSize + squareSize / 2;
    }







int boardGetPixelDistance( int  inRowA,
                           int  inColA,
                           int  inRowB,
                           int  inColB ) {
 
    int  dRow       =  inRowA - inRowB;
    int  dCol       =  inColA - inColB;
    
    if( dRow < 0 ) {
        dRow = - dRow;
        }
    if( dCol < 0 ) {
        dCol = - dCol;
        }
    
    if( dRow == 0 ) {
        return dCol * squareSize;
        }
    
    if( dCol == 0 ) {
        return dRow * squareSize;
        }

    if( dRow == dCol ) {

        /* diag move */

        /* fixed point approx of square root of 2 */
        long  working  =  dRow * 1414;

        return (int)( (working * squareSize ) / 1000 );
        }

    if( ( dRow == 1
          &&
          dCol == 2 )
        ||
        ( dRow == 2
          &&
          dCol == 1 ) ) {

        /* knight's move, sqrt 5 distance */
        long  working  =  2236;

        return (int)( (working * squareSize ) / 1000 );
        }


    /* if we got here, it's some other non-standard chess move
       compute actual square root  */

    return (int)( longSquareRoot( (long)( dRow * dRow + dCol * dCol )
                                  * squareSize * squareSize ) );
    }


    


#endif




#endif
