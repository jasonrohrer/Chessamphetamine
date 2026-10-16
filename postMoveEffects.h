/*
  Include in your C code wherever like so:

      #include "postMoveEffects.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define POST_MOVE_EFFECTS_IMPLEMENTATION
      #include "postMoveEffects.h"

*/

#ifndef POST_MOVE_EFFECTS_H_INCLUDED
#define POST_MOVE_EFFECTS_H_INCLUDED

#include "chess.h"


void postMoveEffectsInit( void );


typedef struct PostMoveEffects {

        int  addedMoney;
        
    } PostMoveEffects;



void postMoveEffectsCompute( BoardState       *inState,
                             Move             *inMove,
                             BoardState       *inNewState,
                             PostMoveEffects  *outEffects );



#endif



#ifdef  POST_MOVE_EFFECTS_IMPLEMENTATION

#ifndef POST_MOVE_EFFECTS_IMPLEMENTATION_INCLUDED
#define POST_MOVE_EFFECTS_IMPLEMENTATION_INCLUDED



/* the signature for a post effects function.
   We define one of these for each piece type in the chess.h enum.

   Updates the passed-in effects map to include effects caused by a given
   piece that moved.
*/
typedef void (*PostEffectsFunction)( BoardState       *inState,
                                     Move             *inMove,
                                     BoardState       *inNewState,
                                     PostMoveEffects  *inEffects );



static void nullPostEffects( BoardState       *inState,
                             Move             *inMove,
                             BoardState       *inNewState,
                             PostMoveEffects  *inEffects ) {
    (void)inState;
    (void)inMove;
    (void)inNewState;
    (void)inEffects;
    }



static void oneRubleEffects( BoardState       *inState,
                             Move             *inMove,
                             BoardState       *inNewState,
                             PostMoveEffects  *inEffects ) {
    (void)inState;
    (void)inMove;
    (void)inNewState;

    inEffects->addedMoney ++;
    }



static PostEffectsFunction postEffectsFunctions[] = {
    MAKE_CHESS_ARRAY( POST_EFFECTS_FUNCTION_LIST )
    };



CHECK_CHESS_ARRAY( postEffectsFunctions,
                   POST_EFFECTS_FUNCTION_LIST );



static void getEmptyPostEffects( PostMoveEffects  *outEffects ) {
    outEffects->addedMoney = 0;
    }



void postMoveEffectsCompute( BoardState       *inState,
                             Move             *inMove,
                             BoardState       *inNewState,
                             PostMoveEffects  *outEffects ) {

    ChessPiece       p  =
                     inNewState->grid[ inMove->endPos[0] ][ inMove->endPos[1] ];
    ChessPiece       t  =  p & CHESS_TYPE_MASK;
    ChessPiece       c  =  p & CHESS_COLOR_MASK;

    getEmptyPostEffects( outEffects );

    if( p == noPiece
        ||
        c == CHESS_BLACK ) {
        /* enemy pieces never have post effects */
        return;
        }

    postEffectsFunctions[ t ]( inState,
                               inMove,
                               inNewState,
                               outEffects );
    }



#endif

#endif
