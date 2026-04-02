/*
  Include in your C code wherever like so:

      #include "board.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define BOARD_IMPLEMENTATION
      #include "board.h"

*/

#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED


void boardInit( void );


void boardDraw( int  inCenterX,
                int  inCenterY );



#ifdef BOARD_IMPLEMENTATION


static  int  borderSpriteH;
static  int  borderSpriteV;

static  int  squareSpriteWhite;
static  int  squareSpriteBlack;

static  int  squareSize          =  25;


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
    
    int  w   =  1;
    
    maxigin_drawSetColor( 0,
                          255,
                          255,
                          255 );

    /* draw black squares in first pass, then white on top */
    for( pass = 0;
         pass < 2;
         pass ++ ) {
        
        for( y = 0;
             y < 8;
             y ++ ) {

            int  yPos  =  inCenterY - 100 + y * squareSize + squareSize / 2;

            for( x = 0;
                 x < 8;
                 x ++  ) {

                int  xPos  =  inCenterX - 100 + x * squareSize + squareSize / 2;

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
                        inCenterY - 100 );

    maxigin_drawSprite( borderSpriteH,
                        inCenterX,
                        inCenterY + 99 );
    
    maxigin_drawSprite( borderSpriteV,
                        inCenterX - 100,
                        inCenterY );

    maxigin_drawSprite( borderSpriteV,
                        inCenterX + 99,
                        inCenterY );

    maxigin_drawResetColor();
    }



#endif




#endif
