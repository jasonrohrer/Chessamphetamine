/*
  Include in your C code wherever like so:

      #include "pieceAnim.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define PIECE_ANIM_IMPLEMENTATION
      #include "pieceAnim.h"

*/

#ifndef PIECE_ANIM_H_INCLUDED
#define PIECE_ANIM_H_INCLUDED


#include "board.h"
#include "pieceSprites.h"



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
void drawMoveAnimation( BoardState  *inState,
                        Move        *inMove,
                        Captured    *inCaptured,
                        BoardState  *inNewState,
                        int          inMoveProgress );



#ifdef PIECE_ANIM_IMPLEMENTATION


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
typedef void (*MoveAnimDrawFunction)( BoardState  *inState,
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
static void noPieceDraw( BoardState  *inState,
                         Move        *inMove,
                         Captured    *inCaptured,
                         BoardState  *inNewState,
                         int          inMoveProgress ) {
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
    (void)inState;
    (void)inMove;
    (void)inCaptured;
    (void)inNewState;
    (void)inMoveProgress;
    return 1;
    }



/* draws entire board with move animation according to inMoveProgress */
static void defaultPieceDraw( BoardState  *inState,
                              Move        *inMove,
                              Captured    *inCaptured,
                              BoardState  *inNewState,
                              int          inMoveProgress ) {
    (void)inState;
    (void)inMove;
    (void)inCaptured;
    (void)inNewState;
    (void)inMoveProgress;
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

    ChessPiece  t  =
        inState->grid[ inMove->startPos[0] ][ inMove->startPos[1] ];
    
    return stepFunctions[ t ]( inState,
                               inMove,
                               inCaptured,
                               inNewState,
                               inMoveProgress );
    }




/* draws entire board with move animation according to inMoveProgress */
void drawMoveAnimation( BoardState  *inState,
                        Move        *inMove,
                        Captured    *inCaptured,
                        BoardState  *inNewState,
                        int          inMoveProgress ) {

    ChessPiece  t  =
        inState->grid[ inMove->startPos[0] ][ inMove->startPos[1] ];
    
    return drawFunctions[ t ]( inState,
                               inMove,
                               inCaptured,
                               inNewState,
                               inMoveProgress );
    }



#endif


#endif
