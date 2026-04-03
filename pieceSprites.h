/*
  Include in your C code wherever like so:

      #include "pieceSprites.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define PIECE_SPRITES_IMPLEMENTATION
      #include "pieceSprites.h"

*/

#ifndef PIECE_SPRITES_H_INCLUDED
#define PIECE_SPRITES_H_INCLUDED



#include "chess.h"



void pieceSpritesInit( void );


void drawPiece( ChessPiece  inPiece,
                int  inBaseCenterX,
                int  inBaseCenterY );


void drawBoardState( BoardState  *inState,
                     int          inBoardCenterX,
                     int          inBoardCenterY );



#ifdef PIECE_SPRITES_IMPLEMENTATION


static  int  pieceBottomHeight  =  5;

static  int          pieceSpriteHandles[ NUM_CHESS_PIECES ];
static  int          pieceOffsetY      [ NUM_CHESS_PIECES ];
static  const char  *pieceSpriteFiles  [ NUM_CHESS_PIECES ] = { "",
                                                                "pawn.tga",
                                                                "bishop.tga",
                                                                "knight.tga",
                                                                "rook.tga",
                                                                "queen.tga",
                                                                "king.tga" };

void pieceSpritesInit( void ) {

    int  i;


    maxigin_logInt( "Size of enum: ",
                    sizeof( ChessPiece ) );

    /* skip noPiece */
    
    for( i = pawn;
         i < NUM_CHESS_PIECES;
         i ++ ) {
        
        pieceSpriteHandles[ i ] = maxigin_initSprite( pieceSpriteFiles[i] );

        pieceOffsetY[ i ] = 0;
        
        if( pieceSpriteHandles[i] != -1 ) {

            int  w;
            int  h;

            if( 1 ) {
                /* sharper black borders */
                maxigin_initMakeDropShadowSprite( pieceSpriteHandles[i],
                                                  1,
                                                  2,
                                                  0,
                                                  255,
                                                  700 );
                }
            else {
                /* hazier black borders */
                maxigin_initMakeDropShadowSprite( pieceSpriteHandles[i],
                                                  4,
                                                  2,
                                                  64,
                                                  255,
                                                  200 );
                }
            
            maxigin_initMakeGlowSprite( pieceSpriteHandles[i],
                                        4,
                                        2 );

            maxigin_getSpriteDimensions( pieceSpriteHandles[i],
                                         &w,
                                         &h );

            pieceOffsetY[ i ] = -( h / 2 - pieceBottomHeight );
            }
        }
    }



void drawPiece( ChessPiece  inPiece,
                int  inBaseCenterX,
                int  inBaseCenterY ) {

    ChessPiece  rawP  =  inPiece & CHESS_PIECE_MASK;

    if( ( inPiece & CHESS_COLOR_MASK ) == CHESS_BLACK ) {
        maxigin_drawSetColor( 128,
                              128,
                              128,
                              255 );
        }
    else {
        maxigin_drawSetColor( 192,
                              128,
                              0,
                              255 );
        }
    
    maxigin_drawSprite( pieceSpriteHandles[ rawP ],
                        inBaseCenterX,
                        inBaseCenterY + pieceOffsetY[ rawP ] );

    maxigin_drawResetColor();
    }



void drawBoardState( BoardState  *inState,
                     int          inBoardCenterX,
                     int          inBoardCenterY ) {

    int  x;
    int  y;

    for( y = 0;
         y < 8;
         y ++ ) {
        
        for( x = 0;
             x < 8;
             x ++ ) {

            ChessPiece  p  =  inState->squareStates[y][x];
            

            if( p != noPiece ) {

                int  pX;
                int  pY;

                boardGetSquareCenter( inBoardCenterX,
                                      inBoardCenterY,
                                      y,
                                      x,
                                      &pX,
                                      &pY );
                
                drawPiece( p,
                           pX,
                           pY );
                }
            }
        }
    }
    

#endif


#endif
