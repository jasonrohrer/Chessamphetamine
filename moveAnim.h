/*
  Include in your C code wherever like so:

      #include "moveAnim.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define MOVE_ANIM_IMPLEMENTATION
      #include "moveAnim.h"

*/

#ifndef MOVE_ANIM_H_INCLUDED
#define MOVE_ANIM_H_INCLUDED


#include "board.h"
#include "pieceSprites.h"


void moveAnimInit( void );


/* inMoveProgress should start at 0

   inMoveProgress is updated with values that have meaning internally
   (range may vary depending on piece type that is moving)

   returns  0 when not done yet
            1 when done
*/
char stepMoveAnimation( BoardState  *inState,
                        Move        *inMove,
                        Captured    *inCaptured,
                        BoardState  *inNewState,
                        int         *inMoveProgress );



/* draws entire board with move animation according to inMoveProgress */
void drawMoveAnimation( int          inBoardCenterX,
                        int          inBoardCenterY,
                        BoardState  *inState,
                        Move        *inMove,
                        Captured    *inCaptured,
                        BoardState  *inNewState,
                        int          inMoveProgress );



#ifdef MOVE_ANIM_IMPLEMENTATION


static  int  beepUp       =  -1;
static  int  beepDown     =  -1;
static  int  shooshGood   =  -1;
static  int  splatterBad  =  -1;



void moveAnimInit( void ) {

    beepUp = maxigin_initSoundEffect( "beepUp.wav" );
    beepDown = maxigin_initSoundEffect( "beepDown.wav" );
    shooshGood = maxigin_initSoundEffect( "shooshGood.wav" );
    splatterBad = maxigin_initSoundEffect( "splatterBad.wav" );
    }




/* The signature for a move animation step function.
   We define one of these for each piece type in the enum in chess.h.

   Updates inMoveProgress and returns 1 when animation is done.
*/
typedef char (*MoveAnimStepFunction)( BoardState  *inState,
                                      Move        *inMove,
                                      Captured    *inCaptured,
                                      BoardState  *inNewState,
                                      int         *inMoveProgress );

/* The signature for a move animation draw function.
   We define one of these for each piece type in the enum in chess.h.
*/
typedef void (*MoveAnimDrawFunction)( int          inBoardCenterX,
                                      int          inBoardCenterY,
                                      BoardState  *inState,
                                      Move        *inMove,
                                      Captured    *inCaptured,
                                      BoardState  *inNewState,
                                      int          inMoveProgress );



static char noPieceStep( BoardState  *inState,
                         Move        *inMove,
                         Captured    *inCaptured,
                         BoardState  *inNewState,
                         int         *inMoveProgress ) {
    (void)inState;
    (void)inMove;
    (void)inCaptured;
    (void)inNewState;
    (void)inMoveProgress;
    return 1;
    }



/* draws entire board with move animation according to inMoveProgress */
static void noPieceDraw( int          inBoardCenterX,
                         int          inBoardCenterY,
                         BoardState  *inState,
                         Move        *inMove,
                         Captured    *inCaptured,
                         BoardState  *inNewState,
                         int          inMoveProgress ) {
    
    (void)inBoardCenterX;
    (void)inBoardCenterY;
    (void)inState;
    (void)inMove;
    (void)inCaptured;
    (void)inNewState;
    (void)inMoveProgress;
    }




static char defaultPieceStep( BoardState  *inState,
                              Move        *inMove,
                              Captured    *inCaptured,
                              BoardState  *inNewState,
                              int         *inMoveProgress ) {
    
    int  pixDist  =  boardGetPixelDistance( inMove->startPos[0],
                                            inMove->startPos[1],
                                            inMove->endPos[0],
                                            inMove->endPos[1] );
    int  r        =  mingin_getStepsPerSecond();

    
    (void)inState;
    (void)inNewState;
    
    
    if( *inMoveProgress < pixDist ) {

        /* still on piece movement portion of animtion */
        
        *inMoveProgress += ( 4 * 60 ) / r;

        if( *inMoveProgress > pixDist ) {
            *inMoveProgress = pixDist;
            
            if( inCaptured->num == 0 ) {
                /* nothing to explode */

                /* play sound at end of piece move */

                if( inState->nextToMove == CHESS_WHITE ) {
                    maxigin_playSoundEffect( beepUp,
                                             256 );
                    }
                else {
                    maxigin_playSoundEffect( beepDown,
                                             256 );
                    }

                /* animation done */
                return 1;
                }
            else {
                
                /* play sound effect at start of capture explosions */
                
                int   oldScore  =  getScore( inState );
                int   newScore  =  getScore( inNewState );

                if( oldScore <= newScore ) {
                    maxigin_playSoundEffect( shooshGood,
                                             512 );
                    }
                else {
                    maxigin_playSoundEffect( splatterBad,
                                             512 );
                    }
                }
            }
        }
    else {
        /* onto explosion of captured pieces */

        int  explodeProgress  =  *inMoveProgress - pixDist;
        
        explodeProgress = stepExplodingPiece( explodeProgress );

        if( explodeProgress == -1 ) {
            /* done exploding */
            return 1;
            }

        *inMoveProgress = pixDist + explodeProgress;
        }

    return 0;
    }



/* draws entire board with move animation according to inMoveProgress */
static void defaultPieceDraw( int          inBoardCenterX,
                              int          inBoardCenterY,
                              BoardState  *inState,
                              Move        *inMove,
                              Captured    *inCaptured,
                              BoardState  *inNewState,
                              int          inMoveProgress ) {

    int  pixDist  =  boardGetPixelDistance( inMove->startPos[0],
                                            inMove->startPos[1],
                                            inMove->endPos[0],
                                            inMove->endPos[1] );
    boardDraw( inBoardCenterX,
               inBoardCenterY );

    if( inMoveProgress < pixDist ) {
        /* draw move in progress */

        drawBoardState( inState,
                        0,
                        0,
                        0,
                        0,
                        inMove,
                        inMoveProgress,
                        pixDist,
                        inBoardCenterX,
                        inBoardCenterY );
        }
    else {
        /* move done, draw end state */
        drawBoardState( inNewState,
                        0,
                        0,
                        0,
                        0,
                        inMove,
                        0,
                        0,
                        inBoardCenterX,
                        inBoardCenterY );

        if( inCaptured->num > 0 ) {

            /* there are captured pieces, draw them */

            int  pn;
            int  explodingProgress  =  inMoveProgress - pixDist;
            
            for( pn = 0;
                 pn < inCaptured->num;
                 pn ++ ) {

                BoardPiece  *bp  =  &( inCaptured->pieces[ pn ] );
            
                drawExplodingPiece( bp->p,
                                    inBoardCenterX,
                                    inBoardCenterY,
                                    bp->row,
                                    bp->col,
                                    explodingProgress );
                }
            }
        }
    }




static MoveAnimStepFunction stepFunctions[ NUM_CHESS_PIECES ] =
                                              { noPieceStep,
                                                defaultPieceStep,
                                                defaultPieceStep,
                                                defaultPieceStep,
                                                defaultPieceStep,
                                                defaultPieceStep,
                                                defaultPieceStep,
                                                defaultPieceStep,
                                                defaultPieceStep };


static MoveAnimDrawFunction drawFunctions[ NUM_CHESS_PIECES ] =
                                              { noPieceDraw,
                                                defaultPieceDraw,
                                                defaultPieceDraw,
                                                defaultPieceDraw,
                                                defaultPieceDraw,
                                                defaultPieceDraw,
                                                defaultPieceDraw,
                                                defaultPieceDraw,
                                                defaultPieceDraw };



char stepMoveAnimation( BoardState  *inState,
                        Move        *inMove,
                        Captured    *inCaptured,
                        BoardState  *inNewState,
                        int         *inMoveProgress ) {

    ChessPiece  p  =  inState->grid[ inMove->startPos[0] ]
                                   [ inMove->startPos[1] ];
    ChessPiece  t  =  p & CHESS_TYPE_MASK;
    
    return stepFunctions[ t ]( inState,
                               inMove,
                               inCaptured,
                               inNewState,
                               inMoveProgress );
    }




/* draws entire board with move animation according to inMoveProgress */
void drawMoveAnimation( int          inBoardCenterX,
                        int          inBoardCenterY,
                        BoardState  *inState,
                        Move        *inMove,
                        Captured    *inCaptured,
                        BoardState  *inNewState,
                        int          inMoveProgress ) {

    ChessPiece  p  =  inState->grid[ inMove->startPos[0] ]
                                   [ inMove->startPos[1] ];
    ChessPiece  t  =  p & CHESS_TYPE_MASK;
    
    drawFunctions[ t ]( inBoardCenterX,
                        inBoardCenterY,
                        inState,
                        inMove,
                        inCaptured,
                        inNewState,
                        inMoveProgress );
    }



#endif


#endif
