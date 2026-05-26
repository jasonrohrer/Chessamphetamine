#ifndef CHESS_ARRAY_CHECK_H_INCLUDED
#define CHESS_ARRAY_CHECK_H_INCLUDED



#include "arraySizeCheck.h"



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
