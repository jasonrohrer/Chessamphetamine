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



void boardInit( void ) {

    
    borderSpriteH = maxigin_initSprite( "boardEdgeH.tga" );

    maxigin_initMakeGlowSprite( borderSpriteH,
                                4,
                                2 );

    borderSpriteV = maxigin_initSprite( "boardEdgeV.tga" );

    maxigin_initMakeGlowSprite( borderSpriteV,
                                4,
                                2 );
    }


void boardDraw( int  inCenterX,
                int  inCenterY ) {

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
    }



#endif




#endif
