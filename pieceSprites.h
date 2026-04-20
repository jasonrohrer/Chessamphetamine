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
#include "particleSprite.h"



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
                     char         inCheckmate,
                     char         inStalemate,
                     char         inDrawGame,
                     int          inLoserColor,
                     Move        *inMove,
                     int          inMoveProgress,
                     int          inMoveProgressMax,
                     int          inBoardCenterX,
                     int          inBoardCenterY );



/* sets maxigin draw color to piece color for future draw calls
   inPieceColor  is CHESS_WHITE or CHESS_BLACK */
void drawSetPieceColor( int  inPieceColor );




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
                                                                "king.tga",
                                                                "rook.tga" };

/* extra sprite to be drawn on top of sprite with
   no piece-color modification */

static  int          pieceSpriteExtraHandles[ NUM_CHESS_PIECES ];
static  int          pieceExtraOffsetY      [ NUM_CHESS_PIECES ] = { 0,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     8 };
static  const char  *pieceSpriteExtraFiles  [ NUM_CHESS_PIECES ] =
                                              { "",
                                                "",
                                                "",
                                                "",
                                                "",
                                                "",
                                                "",
                                                "rookLasers.tga" };


static  int          slashSpriteHandle      =  -1;
static  int          whiteFlagSpriteHandle  =  -1;
static  int          noMoveSpriteHandle     =  -1;


void pieceSpritesInit( void ) {

    int  i;


    maxigin_logInt( "Size of enum: ",
                    sizeof( ChessPiece ) );
    
    for( i = FIRST_CHESS_PIECE;
         i < NUM_CHESS_PIECES;
         i ++ ) {

        int   j;
        char  foundMatch  =  0;
        
        for( j = FIRST_CHESS_PIECE;
             j < i;
             j ++ ) {

            if( maxigin_stringsEqual( pieceSpriteFiles[i],
                                      pieceSpriteFiles[j] ) ) {

                /* we already loaded this file for a previous piece,
                   re-use it */
                
                foundMatch = 1;

                pieceSpriteHandles[ i ] = pieceSpriteHandles[ j ];
                pieceOffsetY      [ i ] = pieceOffsetY      [ j ];
                }
            }

        if( foundMatch ) {
            continue;
            }
        
        /* else a different sprite data file from what we've already processed */

        pieceSpriteHandles[ i ] = -1;

        if( maxigin_stringsEqual( pieceSpriteFiles[i],
                                  "" ) ) {
            /* skip blank file names */
            continue;
            }
        
        pieceSpriteHandles[ i ] = maxigin_initSprite( pieceSpriteFiles[i] );

        pieceOffsetY[ i ] = 0;
        
        if( pieceSpriteHandles[i] != -1 ) {

            int  w;
            int  h;
                
            /* hazy black border around top only */
            maxigin_initMakeDropShadowSprite( pieceSpriteHandles[i],
                                              5,
                                              2,
                                              0,
                                              255,
                                              60,
                                              30,
                                              100,
                                              0 );

            
            maxigin_initMakeGlowSprite( pieceSpriteHandles[i],
                                        4,
                                        2 );

            maxigin_getSpriteDimensions( pieceSpriteHandles[i],
                                         &w,
                                         &h );

            pieceOffsetY[ i ] = -( h / 2 - pieceBottomHeight );
            }
        }


    
    for( i = FIRST_CHESS_PIECE;
         i < NUM_CHESS_PIECES;
         i ++ ) {

        int   j;
        char  foundMatch  =  0;
        
        for( j = FIRST_CHESS_PIECE;
             j < i;
             j ++ ) {

            if( maxigin_stringsEqual( pieceSpriteExtraFiles[i],
                                      pieceSpriteExtraFiles[j] ) ) {

                /* we already loaded this file for a previous piece,
                   re-use it */
                
                foundMatch = 1;

                pieceSpriteExtraHandles[ i ] = pieceSpriteExtraHandles[ j ];
                }
            }

        if( foundMatch ) {
            continue;
            }
        
        /* else a different sprite data file from what we've already processed */

        pieceSpriteExtraHandles[ i ] = -1;

        if( maxigin_stringsEqual( pieceSpriteExtraFiles[i],
                                  "" ) ) {
            /* skip blank file names */
            continue;
            }
        
        pieceSpriteExtraHandles[ i ] = maxigin_initSprite(
                                           pieceSpriteExtraFiles[i] );

        if( pieceSpriteExtraHandles[i] != -1 ) {

            /* hazy black border around top only */
            maxigin_initMakeDropShadowSprite( pieceSpriteExtraHandles[i],
                                              5,
                                              2,
                                              0,
                                              255,
                                              60,
                                              30,
                                              100,
                                              0 );

            
            maxigin_initMakeGlowSprite( pieceSpriteExtraHandles[i],
                                        4,
                                        2 );
            }
        }
    


    slashSpriteHandle = maxigin_initSprite( "checkmateSlash.tga" );

    if( slashSpriteHandle != -1 ) {
        maxigin_initMakeGlowSprite( slashSpriteHandle,
                                    4,
                                    2 );

        /* hazy drop shadow top to bottom */
        maxigin_initMakeDropShadowSprite( slashSpriteHandle,
                                          5,
                                          2,
                                          255,
                                          255,
                                          100,
                                          0,
                                          100,
                                          0 );
        }

    whiteFlagSpriteHandle = maxigin_initSprite( "whiteFlag.tga" );

    if( whiteFlagSpriteHandle != -1 ) {
        maxigin_initMakeGlowSprite( whiteFlagSpriteHandle,
                                    4,
                                    2 );

        /* hazy drop shadow top to bottom */
        maxigin_initMakeDropShadowSprite( whiteFlagSpriteHandle,
                                          5,
                                          2,
                                          255,
                                          255,
                                          100,
                                          0,
                                          100,
                                          0 );
        }

    noMoveSpriteHandle = maxigin_initSprite( "noMoveSquare.tga" );

    if( noMoveSpriteHandle != -1 ) {
        maxigin_initMakeGlowSprite( noMoveSpriteHandle,
                                    4,
                                    2 );

        /* hazy drop shadow top to bottom */
        maxigin_initMakeDropShadowSprite( noMoveSpriteHandle,
                                          5,
                                          2,
                                          255,
                                          255,
                                          100,
                                          0,
                                          100,
                                          0 );
        }
    }



void drawSetPieceColor( int  inPieceColor ) {
    if( inPieceColor == CHESS_BLACK ) {
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
    }



void drawPiece( ChessPiece  inPiece,
                int  inBaseCenterX,
                int  inBaseCenterY ) {

    ChessPiece  rawP  =  inPiece & CHESS_TYPE_MASK;

    drawSetPieceColor( inPiece & CHESS_COLOR_MASK );
    
    maxigin_drawSprite( pieceSpriteHandles[ rawP ],
                        inBaseCenterX,
                        inBaseCenterY + pieceOffsetY[ rawP ] );

    maxigin_drawResetColor();

    if( pieceSpriteExtraHandles[ rawP ] != -1 ) {
        maxigin_drawSprite( pieceSpriteExtraHandles[ rawP ],
                            inBaseCenterX,
                            inBaseCenterY
                            + pieceOffsetY[ rawP ]
                            + pieceExtraOffsetY[ rawP ] );
        }
    }


static  int  explodeMax  =  512;


int stepExplodingPiece( int  inProgress ) {

    if( getParticleSprite() == -1 ) {
        return -1;
        }
    
    inProgress += ( 30 * 60 ) / mingin_getStepsPerSecond();
    

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

    ChessPiece     rawP        =  inPiece & CHESS_TYPE_MASK;

    unsigned char  a;
    int            x;
    int            y;
    int            partSprite  =  getParticleSprite();
    
    if( partSprite == -1 ) {
        return;
        }

    boardGetSquareCenter( inBoardCenterX,
                          inBoardCenterY,
                          inRow,
                          inCol,
                          &x,
                          &y );

    drawSetPieceColor( inPiece & CHESS_COLOR_MASK );

    
    if( ( inPiece & CHESS_COLOR_MASK ) == CHESS_WHITE ) {

        /* desaturate gold color to make it clip white */
        maxigin_drawSetColorSaturation( 8500 );
        }


    a = (unsigned char)( ( (long)( explodeMax - inProgress ) * 255 )
                         / explodeMax );

    maxigin_drawToggleAdditive( 1 );

    maxigin_drawExplodingSprite( pieceSpriteHandles[ rawP ],
                                 partSprite,
                                 x,
                                 y + pieceOffsetY[ rawP ],
                                 BOARD_SQUARE_SIZE / 4,
                                 inProgress,
                                 explodeMax,
                                 a );
    
    maxigin_drawToggleAdditive( 0 );
    
    maxigin_drawResetColor();
    }



void drawBoardState( BoardState  *inState,
                     char         inCheckmate,
                     char         inStalemate,
                     char         inDrawGame,
                     int          inLoserColor,
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


    if( inStalemate ) {
        /* draw no move x's around loser's king, under other pieces */

        for( y = 0;
             y < BH;
             y ++ ) {
        
            for( x = 0;
                 x < BW;
                 x ++ ) {

                ChessPiece  p    =  inState->grid[y][x];
                ChessPiece  t    =  p & CHESS_TYPE_MASK;
                ChessPiece  c    =  p & CHESS_COLOR_MASK;

                if( t == king
                    &&
                    c == inLoserColor ) {

                    int dx;
                    int dy;

                    for( dy = -1;
                         dy <= 1;
                         dy ++ ) {

                        int  oy  =  y + dy;

                        if( oy >= 0
                            &&
                            oy < BH ) {
                            /* oy in bounds  */
                        
                            for( dx = -1;
                                 dx <= 1;
                                 dx ++ ) {

                                int  ox  =  x + dx;

                                if( ox >= 0
                                    &&
                                    ox < BW ) {

                                    /* ox in bounds */

                                    if( ox != x
                                        ||
                                        oy != y ) {
                                        /* no x on the king's pos itself */
                            
                                        int  pX;
                                        int  pY;

                                        boardGetSquareCenter( inBoardCenterX,
                                                              inBoardCenterY,
                                                              oy,
                                                              ox,
                                                              &pX,
                                                              &pY );
                                        maxigin_drawSprite( noMoveSpriteHandle,
                                                            pX,
                                                            pY );
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    
    

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
         y < BH;
         y ++ ) {
        
        for( x = 0;
             x < BW;
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

                    if( inCheckmate
                        ||
                        inDrawGame ) {
                        
                        ChessPiece  c  =  p & CHESS_COLOR_MASK;
                        ChessPiece  t  =  p & CHESS_TYPE_MASK;

                        if( t == king
                            &&
                            c == inLoserColor ) {

                            if( inCheckmate ) {
                                /* draw checkmate slash */
                                maxigin_drawSprite( slashSpriteHandle,
                                                    pX,
                                                    pY + pieceOffsetY[ t ] );
                                }
                            else {
                                /* draw white flag  */
                                maxigin_drawSprite( whiteFlagSpriteHandle,
                                                    pX,
                                                    pY + pieceOffsetY[ t ] );
                                }
                            }
                        }  
                    }
                }
            }
        }
    }
    

#endif


#endif
