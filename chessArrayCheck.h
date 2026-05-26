#ifndef CHESS_ARRAY_CHECK_H_INCLUDED
#define CHESS_ARRAY_CHECK_H_INCLUDED



#include "arraySizeCheck.h"


/*
  The helper macro in arraySizeCheck only get us so far.  Yes, we
  can ensure that every array has exactly NUM_CHESS_PIECES elements,
  but we cannot ensure that those elements are in the right order, which becomes
  more error-prone the longer the arrays get.  Chessamphetamine will eventually
  have dozens and dozens of pieces, and a bunch of long arrays make it difficult
  to make sure a new item has values added in the right places in each array.

  Unfortunately, C89 doesn't let us specify an index for each array
  initialization value.  The values must simply be provided in order.

  These new helper macros allow for far more robustness, ensuring both
  that there are ENOUGH elements in the array and that they are in the right
  order.  It also gives us more information, when looking at a given array,
  about what value belongs to what piece.

  With these macros, we can do something like this, below.

  Note that "C" and "V" are arbitrary place-holders that are used internally
  by the macro.  They can be replaced with any strings you want.

  The indices that come after the C must be in order.

  If anything is amiss (the indices are out of order, the piece enums are
  out of order, or a piece is missing), an error will be thrown.

  The only thing that will NOT be caught is if the indices are out of order
  AND their matching piece enums are ALSO out of order.  But this can be
  detected visually, since the indices will be out of order.

  #define PIECE_VALUE_LIST( C, V )   \
      V( C, 0,   noPiece,      0   ) \
      V( C, 1,   pawn,         1   ) \
      V( C, 2,   bishop,       3   ) \
      V( C, 3,   knight,       3   ) \
      V( C, 4,   rook,         5   ) \
      V( C, 5,   queen,        9   ) \
      V( C, 6,   king,         999 ) \
      V( C, 7,   laserRook,    7   ) \
      V( C, 8,   laserPawn,    2   ) \
      V( C, 9,   doublingPawn, 2   ) \
      V( C, 10,  addingRook,   6   ) \
      V( C, 11,  rocket,       2   )

  static int pieceValue[] = {
      MAKE_CHESS_ARRAY( PIECE_VALUE_LIST )
      };

  CHECK_CHESS_ARRAY( pieceValue,
                     PIECE_VALUE_LIST );


  The ARRAY_PAIR macro is provided to help with 2D arrays of pair values,
  like this:

  #define PIECE_EXTRA_OFFSET_Y_LIST( C, V )            \
      V( C, 0,   noPiece,      ARRAY_PAIR(  0,  0 )  ) \
      V( C, 1,   pawn,         ARRAY_PAIR(  0,  0 )  ) \
      V( C, 2,   bishop,       ARRAY_PAIR(  0,  0 )  ) \
      V( C, 3,   knight,       ARRAY_PAIR(  0,  0 )  ) \
      V( C, 4,   rook,         ARRAY_PAIR(  0,  0 )  ) \
      V( C, 5,   queen,        ARRAY_PAIR(  0,  0 )  ) \
      V( C, 6,   king,         ARRAY_PAIR(  0,  0 )  ) \
      V( C, 7,   laserRook,    ARRAY_PAIR(  2,  2 )  ) \
      V( C, 8,   laserPawn,    ARRAY_PAIR( -1,  0 )  ) \
      V( C, 9,   doublingPawn, ARRAY_PAIR(  0,  0 )  ) \
      V( C, 10,  addingRook,   ARRAY_PAIR(  0,  0 )  ) \
      V( C, 11,  rocket,       ARRAY_PAIR(  0,  0 )  )

  static int pieceExtraOffsetY[][2] = {
      MAKE_CHESS_ARRAY( PIECE_EXTRA_OFFSET_Y_LIST )
      };

  CHECK_CHESS_ARRAY( pieceExtraOffsetY,
                     PIECE_EXTRA_OFFSET_Y_LIST );
*/




#define CHESS_ARRAY_VALUE( arrayName, index, pieceEnum, value ) value,


#define MAKE_CHESS_ARRAY( inValueList ) \
    inValueList( dummyArrayName, CHESS_ARRAY_VALUE )


#define CHECK_PIECE_ARRAY_ORDER( arrayName, index, pieceEnum, value )    \
    typedef char ERROR__piece_order_mismatch_in__ ## arrayName ## __ ##  \
                 pieceEnum ## __has_WRONG_index__ ##                     \
                 index [  ( pieceEnum == index ) ? 1 : -1 ];


#define CHECK_CHESS_ARRAY( inArray, inValueList )  \
    inValueList( inArray, CHECK_PIECE_ARRAY_ORDER )  \
    CHECK_ARRAY_LENGTH( inArray, NUM_CHESS_PIECES )


#define ARRAY_PAIR( inA, inB ) \
    { inA, inB }


#endif
