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


void drawPieceInfoPanel( ChessPiece     inPiece,
                         int            inCenterX,
                         int            inCenterY,
                         unsigned char  inFade );




#ifdef PIECE_DESCRIPTIONS_IMPLEMENTATION


#include "maxigin.h"



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
    V( C, 7,   laserRook,    "laserRookDesc"    ) \
    V( C, 8,   laserPawn,    "laserPawnDesc"    ) \
    V( C, 9,   doublingPawn, "doublingPawnDesc" ) \
    V( C, 10,  addingRook,   "addingRookDesc"   ) \
    V( C, 11,  rocket,       "rocketDesc"       )

static  const char  *pieceDescriptionKeyStrings[] = {
    MAKE_CHESS_ARRAY( PIECE_DESCRIPTION_KEY_STRING_LIST )
    };

CHECK_CHESS_ARRAY( pieceDescriptionKeyStrings,
                   PIECE_DESCRIPTION_KEY_STRING_LIST );


static  int  pieceDescriptionKeys[ NUM_CHESS_PIECES ];




static  int  infoPanelSprite  =  -1;


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

    infoPanelSprite = maxigin_initSprite( "pieceInfoPanel.tga" );

    maxigin_initMakeGlowSprite( infoPanelSprite,
                                4,
                                2 );
    }


#define  MAX_NUM_WORDS  100
#define  MAX_WORD_LEN    20

static  char  pieceWordBuffer[ MAX_NUM_WORDS ][ MAX_WORD_LEN ];

static  int   pieceNumWords  =  0;

/*
#define  MAX_NUM_LINES  20
#define  MAX_LINE_LEN   40


static  char  pieceLineBuffer[ MAX_NUM_LINES ][ MAX_LINE_LEN ];

static  int   pieceNumLines  =  0;
*/


static int findNextSpaceIndex(  const char  *inString ) {

    int  i  =  0;

    while( inString[i] != '\0' ) {

        if( inString[i] == 0x20 ) {

            return i;
            }
        i++;
        }

    return -1;
    }



static void pieceSplitWordsNoSpaces( const char  *inString ) {

    (void)inString;

    }


static void pieceSplitWords( const char  *inString ) {

    const char  *working  = inString;

    int          i;

    if( 0 ) {
        /* fixme:
           check if it's a no-words language */
        pieceSplitWordsNoSpaces( inString );
        return;
        }
    

    pieceNumWords = 0;

    i = findNextSpaceIndex( working );

    while( i != -1 ) {

        int j;

        if( i >= MAX_WORD_LEN ) {
            /* word too long, skip it */
            
            working = &( working[i + 1] );
            i = findNextSpaceIndex( working );
            continue;
            }

        for( j = 0;
             j < i;
             j ++ ) {

            pieceWordBuffer[ pieceNumWords ][ j ] = working[j];
            }
        
        /* terminate where space ends word */
        pieceWordBuffer[ pieceNumWords ][ i ] = '\0';

        pieceNumWords ++ ;


        if( pieceNumWords >= MAX_NUM_WORDS ) {
            /* too many words, bail */
            return;
            }

        working = &( working[i + 1] );
        i = findNextSpaceIndex( working );
        }
        

    /* no space found, final word in string */

    i = 0;

    while( working[i] != '\0' ) {

        if( i >= MAX_WORD_LEN ) {
            /* last word too long, skip it */
            return;
            }

        pieceWordBuffer[ pieceNumWords ][ i ] = working[i];

        i ++;
        }

    pieceWordBuffer[ pieceNumWords ][ i ] = '\0';


    pieceNumWords ++;
    }



void drawPieceInfoPanel( ChessPiece     inPiece,
                         int            inCenterX,
                         int            inCenterY,
                         unsigned char  inFade ) {

    ChessPiece   c  =  inPiece & CHESS_COLOR_MASK;
    ChessPiece   t  =  inPiece & CHESS_TYPE_MASK;

    const char  *title;
    
    drawSetPieceColor( c );

    maxigin_drawSetAlpha( inFade );

    maxigin_drawSprite( infoPanelSprite,
                        inCenterX,
                        inCenterY );

    maxigin_drawSetColor( 255,
                          255,
                          255,
                          inFade );

    title = maxigin_getLangText( pieceNameKeys[ t ] );

    pieceSplitWords( title );

    if( pieceNumWords > 0 ) {

        int  w;

        for( w = 0;
             w < pieceNumWords;
             w ++ ) {
            
            maxigin_drawLangTextString( pieceWordBuffer[ w ],
                                        inCenterX,
                                        inCenterY - 60 + 12 * w,
                                        MAXIGIN_CENTER );
            }
        }
    
    }



#endif

#endif
