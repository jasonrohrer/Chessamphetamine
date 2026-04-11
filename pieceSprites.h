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


/* start with inProgress = 0
   returns new progress after one explosion step
   returns -1 when explosion done
*/
int stepExplodingPiece( int  inProgress );

void drawExplodingPiece( ChessPiece  inPiece,
                         int         inBoardCenterX,
                         int         inBoardCenterY,
                         int         inRow,
                         int         inCol,
                         int         inProgress );



/* inMoveProgress goes from 0 to inMoveProgressMax
   if inMoveProgressMax is 0, inMove is ignored */
void drawBoardState( BoardState  *inState,
                     Move        *inMove,
                     int          inMoveProgress,
                     int          inMoveProgressMax,
                     int          inBoardCenterX,
                     int          inBoardCenterY );



#ifdef PIECE_SPRITES_IMPLEMENTATION


static  int  pieceBottomHeight  =  6;

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

                /* light outline behind black border */
                if(0)maxigin_initMakeDropShadowSprite( pieceSpriteHandles[i],
                                                  1,
                                                  3,
                                                  0,
                                                  192,
                                                  50,
                                                  20,
                                                  3400,
                                                  255 );
                /* sharper black borders */
                if(0)maxigin_initMakeDropShadowSprite( pieceSpriteHandles[i],
                                                  1,
                                                  2,
                                                  255,
                                                  255,
                                                  100,
                                                  0,
                                                  700,
                                                  0 );
                
                /* hazy black border around top only */
                if(1)maxigin_initMakeDropShadowSprite( pieceSpriteHandles[i],
                                                  5,
                                                  2,
                                                  0,
                                                  255,
                                                  60,
                                                  30,
                                                  100,
                                                  0 );

                /* haze at bottom */
                if(0)maxigin_initMakeDropShadowSprite( pieceSpriteHandles[i],
                                                  4,
                                                  3,
                                                  255,
                                                  0,
                                                  70,
                                                  50,
                                                  100,
                                                  0 );

                }
            else {
                /* hazier black borders */
                maxigin_initMakeDropShadowSprite( pieceSpriteHandles[i],
                                                  4,
                                                  2,
                                                  64,
                                                  255,
                                                  100,
                                                  0,
                                                  200,
                                                  0 );
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

    ChessPiece  rawP  =  inPiece & CHESS_TYPE_MASK;

    if( ( inPiece & CHESS_COLOR_MASK ) == CHESS_BLACK ) {
        maxigin_drawSetColor( 128,
                              64,
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


static  int  explodeMax  =  512;


int stepExplodingPiece( int  inProgress ) {
    
    inProgress += ( 2 * mingin_getStepsPerSecond() ) / 60;
    

    if( inProgress >= explodeMax ) {
        inProgress = -1;
        }

    return inProgress;
    }


    
void drawExplodingPiece( ChessPiece  inPiece,
                         int         inBoardCenterX,
                         int         inBoardCenterY,
                         int         inRow,
                         int         inCol,
                         int         inProgress ) {

    ChessPiece     rawP  =  inPiece & CHESS_TYPE_MASK;

    unsigned char  a;
    int            x;
    int            y;

    boardGetSquareCenter( inBoardCenterX,
                          inBoardCenterY,
                          inRow,
                          inCol,
                          &x,
                          &y );
    
    if( ( inPiece & CHESS_COLOR_MASK ) == CHESS_BLACK ) {
        maxigin_drawSetColor( 128,
                              64,
                              128,
                              255 );
        }
    else {
        maxigin_drawSetColor( 192,
                              128,
                              0,
                              255 );
        }

    a = (unsigned char)( ( (long)( explodeMax - inProgress ) * 255 )
                         / explodeMax );
    
    maxigin_drawExplodingSprite( pieceSpriteHandles[ rawP ],
                                 x,
                                 y + pieceOffsetY[ rawP ],
                                 BOARD_SQUARE_SIZE / 2,
                                 inProgress,
                                 explodeMax,
                                 a );

    maxigin_drawResetColor();
    }



void drawBoardState( BoardState  *inState,
                     Move        *inMove,
                     int          inMoveProgress,
                     int          inMoveProgressMax,
                     int          inBoardCenterX,
                     int          inBoardCenterY ) {

    int         x;
    int         y;
    char        drawMoving  =  0;
    int         mX;
    int         mY;
    ChessPiece  mP;
    

    if( inMoveProgressMax != 0 ) {
        /* a move is in progress  */
        int  sY;
        int  sX;
        
        int  dX;
        int  dY;

        long  workingX;
        long  workingY;


        boardGetSquareCenter( inBoardCenterX,
                              inBoardCenterY,
                              inMove->startPos[0],
                              inMove->startPos[1],
                              &sX,
                              &sY );
        
        boardGetSquareCenter( inBoardCenterX,
                              inBoardCenterY,
                              inMove->endPos[0],
                              inMove->endPos[1],
                              &dX,
                              &dY );

        workingX = dX - sX;
        workingY = dY - sY;

        workingX *= inMoveProgress;
        workingY *= inMoveProgress;

        mX = (int)( workingX / inMoveProgressMax ) + sX;
        mY = (int)( workingY / inMoveProgressMax ) + sY;

        mP = inState->grid[ inMove->startPos[0] ][ inMove->startPos[1] ];
        
        drawMoving = 1;
        }
    

    for( y = 0;
         y < 8;
         y ++ ) {
        
        for( x = 0;
             x < 8;
             x ++ ) {

            ChessPiece  p    =  inState->grid[y][x];
            int         pX;
            int         pY;

            boardGetSquareCenter( inBoardCenterX,
                                  inBoardCenterY,
                                  y,
                                  x,
                                  &pX,
                                  &pY );

            if( drawMoving ) {
                /* see if we're at a good order spot where we should
                   finall draw this piece behind this next piece */

                if( pY >= mY ) {
                    /* next piece is first spot (which may contain a piece)
                       that should be in
                       front of our moving piece,
                       draw our moving piece now, just behind it */
                    drawPiece( mP,
                               mX,
                               mY );
                    drawMoving = 0;
                    }
                }

            if( p != noPiece ) {

                if( inMoveProgressMax != 0
                    &&
                    inMove->startPos[0] == y
                    &&
                    inMove->startPos[1] == x ) {

                    /* skip this piece, because it's moving
                       draw it separately */
                    }
                else {
                    
                    drawPiece( p,
                               pX,
                               pY );
                    }
                }
            }
        }
    }
    

#endif


#endif
