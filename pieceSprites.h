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



typedef enum{
    pawn = 0,
    bishop,
    knight,
    rook,
    queen,
    king } ChessPiece;

#define NUM_CHESS_PIECES  6



void pieceSpritesInit( void );


void drawPiece( ChessPiece  inPiece,
                int  inBaseCenterX,
                int  inBaseCenterY );



#ifdef PIECE_SPRITES_IMPLEMENTATION


static  int  pieceBottomHeight  =  5;

static  int          pieceSpriteHandles[ NUM_CHESS_PIECES ];
static  int          pieceOffsetY      [ NUM_CHESS_PIECES ];
static  const char  *pieceSpriteFiles  [ NUM_CHESS_PIECES ] = { "pawn.tga",
                                                                "bishop.tga",
                                                                "knight.tga",
                                                                "rook.tga",
                                                                "queen.tga",
                                                                "king.tga" };

void pieceSpritesInit( void ) {

    int  i;


    for( i = 0;
         i < NUM_CHESS_PIECES;
         i ++ ) {
        
        pieceSpriteHandles[ i ] = maxigin_initSprite( pieceSpriteFiles[i] );

        pieceOffsetY[ i ] = 0;
        
        if( pieceSpriteHandles[i] != -1 ) {

            int  w;
            int  h;
            
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

    /* fixme:  need to offset center */
    maxigin_drawSetColor( 192,
                          128,
                          0,
                          255 );
    maxigin_drawSprite( pieceSpriteHandles[ inPiece ],
                        inBaseCenterX,
                        inBaseCenterY + pieceOffsetY[ inPiece ] );

    maxigin_drawResetColor();
    }

    

#endif


#endif
