/*
  Include in your C code wherever like so:

      #include "pieceDescriptions.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define PIECE_DESCRIPTIONS_IMPLEMENTATION
      #include "pieceDescriptions.h"

*/

#ifndef PIECE_DESCRIPTIONS_H_INCLUDED
#define PIECE_DESCRIPTIONS_H_INCLUDED



#include "chess.h"
#include "particleSprite.h"



void pieceDescriptionsInit( void );



#ifdef PIECE_DESCRIPTIONS_IMPLEMENTATION



#define PIECE_NAME_KEY_STRING_LIST( C, V )    \
    V( C, 0,   noPiece,      ""             ) \
    V( C, 1,   pawn,         "pawn"         ) \
    V( C, 2,   bishop,       "bishop"       ) \
    V( C, 3,   knight,       "knight"       ) \
    V( C, 4,   rook,         "rook"         ) \
    V( C, 5,   queen,        "queen"        ) \
    V( C, 6,   king,         "king"         ) \
    V( C, 7,   laserRook,    "laserRook"    ) \
    V( C, 8,   laserPawn,    "laserPawn"    ) \
    V( C, 9,   doublingPawn, "doublingPawn" ) \
    V( C, 10,  addingRook,   "addingRook"   ) \
    V( C, 11,  rocket,       "rocket"       )

static  const char  *pieceNameKeyStrings[] = {
    MAKE_CHESS_ARRAY( PIECE_NAME_KEY_STRING_LIST )
    };

CHECK_CHESS_ARRAY( pieceNameKeyStrings,
                   PIECE_NAME_KEY_STRING_LIST );


static  int  pieceNameKeys[ NUM_CHESS_PIECES ];



#define PIECE_DESCRIPTION_KEY_STRING_LIST( C, V ) \
    V( C, 0,   noPiece,      ""                 ) \
    V( C, 1,   pawn,         "pawnDesc"         ) \
    V( C, 2,   bishop,       "bishopDesc"       ) \
    V( C, 3,   knight,       "knightDesc"       ) \
    V( C, 4,   rook,         "rookDesc"         ) \
    V( C, 5,   queen,        "queenDesc"        ) \
    V( C, 6,   king,         "kingDesc"         ) \
    V( C, 7,   laserRook,    "rookDesc"         ) \
    V( C, 8,   laserPawn,    "pawnDesc"         ) \
    V( C, 9,   doublingPawn, "doublingPawnDesc" ) \
    V( C, 10,  addingRook,   "addingRookDesc"   ) \
    V( C, 11,  rocket,       "rocketDesc"       )

static  const char  *pieceDescriptionKeyStrings[] = {
    MAKE_CHESS_ARRAY( PIECE_DESCRIPTION_KEY_STRING_LIST )
    };

CHECK_CHESS_ARRAY( pieceDescriptionKeyStrings,
                   PIECE_DESCRIPTION_KEY_STRING_LIST );


static  int  pieceDescriptionKeys[ NUM_CHESS_PIECES ];





void pieceDescriptionsInit( void ) {

    int  i;

    pieceNameKeys       [ noPiece ] = -1;
    pieceDescriptionKeys[ noPiece ] = -1;

    for( i = FIRST_CHESS_PIECE;
         i < NUM_CHESS_PIECES;
         i ++ ) {

        pieceNameKeys[ i ] =
            maxigin_initTranslationKey( pieceNameKeyStrings[i] );

        pieceDescriptionKeys[ i ] =
            maxigin_initTranslationKey( pieceDescriptionKeyStrings[i] );
        }
    }


#endif

#endif
