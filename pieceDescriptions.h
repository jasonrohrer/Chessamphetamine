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

static  char  pieceWordBuffer[ MAX_NUM_WORDS ][ MAX_WORD_LEN + 1 ];

static  int   pieceNumWords  =  0;


#define  MAX_NUM_LINES  20
#define  MAX_LINE_LEN   40


static  char  pieceLineBuffer[ MAX_NUM_LINES ][ MAX_LINE_LEN + 1 ];

static  int   pieceNumLines  =  0;



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



static void pieceSplitWords( const char  *inString ) {

    const char  *working  = inString;

    int          i;

    pieceNumWords = 0;

    i = findNextSpaceIndex( working );

    while( i != -1 ) {

        int j;

        if( i > MAX_WORD_LEN ) {
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

        if( i > MAX_WORD_LEN ) {
            /* last word too long, skip it */
            return;
            }

        pieceWordBuffer[ pieceNumWords ][ i ] = working[i];

        i ++;
        }

    pieceWordBuffer[ pieceNumWords ][ i ] = '\0';


    pieceNumWords ++;
    }



static void pieceClearLines( void ) {

    int  i;


    for( i = 0;
         i < MAX_NUM_LINES;
         i ++ ) {
        pieceLineBuffer[ i ][ 0 ] = '\0';
        }
    pieceNumLines = 0;
    }



static void pieceSplitLinesNoSpaces( const char  *inString,
                                     int          inMaxPixelWidth ) {

    char  *workingString  =  (char*)inString;
    int    lineI          =  0;
    
    pieceClearLines();

    while( workingString[0] != '\0' ) {

        int  codePointLen  =
                 maxigin_getNextUTF8CodePointLength( workingString );
        int  i;
        int  pixLen;

        if( codePointLen == -1 ) {
            /* error */
            return;
            }

        if( lineI + codePointLen > MAX_LINE_LEN ) {
            /* not enough byte room on this line */
            pieceNumLines ++;

            if( pieceNumLines >= MAX_NUM_LINES ) {
                /* too many lines */
                return;
                }
            
            lineI = 0;
            continue;
            }

        /* room on line for more bytes */
            

        for( i = 0;
             i < codePointLen;
             i ++ ) {

            pieceLineBuffer[ pieceNumLines ][ lineI ++ ] =
                workingString[ i ];
            }

        /* terminate so we can measure it */
        pieceLineBuffer[ pieceNumLines ][ lineI ] = '\0';

        pixLen = maxigin_measureLangTextString(
                     pieceLineBuffer[ pieceNumLines ] );
            
        if( pixLen > inMaxPixelWidth ) {
            /* not enough pixel room on this line */
            pieceNumLines ++;

            if( pieceNumLines >= MAX_NUM_LINES ) {
                /* too many lines */
                return;
                }
            
            lineI = 0;
            continue;
            }

        /* there's room, skip code point and keep going */

        workingString = &( workingString[ codePointLen ] );
        }

    /* count last line */
    pieceNumLines ++;
    }



/* 1 on success
   0 on failure (no room) */
static char addWordToLine( int  inLineIndex,
                           int  inWordIndex ) {

    int  lineLen  =  maxigin_stringLength( pieceLineBuffer[ inLineIndex ] );
    int  wordLen  =  maxigin_stringLength( pieceWordBuffer[ inWordIndex ] );

    if( lineLen + wordLen + 1 <= MAX_LINE_LEN ) {

        int  i;

        
        if( lineLen > 0 ) {
            /* replace previous term with space */
            pieceLineBuffer[ inLineIndex ][ lineLen ] = ' ';
            lineLen ++;
            }
        

        for( i = 0;
             i <= wordLen;   /* include \0 term */
             i ++ ) {

            pieceLineBuffer[ inLineIndex ][ lineLen + i ]
                = pieceWordBuffer[ inWordIndex ][ i ];
            }

        return 1;
        }
    return 0;
    }



static void removeWordFromLine( int  inLineIndex,
                                int  inWordIndex ) {

    int  lineLen  =  maxigin_stringLength( pieceLineBuffer[ inLineIndex ] );
    int  wordLen  =  maxigin_stringLength( pieceWordBuffer[ inWordIndex ] );

    if( lineLen >= wordLen ) {

        if( lineLen >= wordLen + 1 ) {
            /* space was added */
            lineLen --;
            }

        pieceLineBuffer[ inLineIndex ][ lineLen - wordLen ] = '\0';
        }

    }







static void pieceSplitLines( const char  *inString,
                             int          inMaxPixelWidth ) {


    int  nextWord  =  0;
    
    if( ! maxigin_doesLanguageHaveWords() ) {
        
        pieceSplitLinesNoSpaces( inString,
                                 inMaxPixelWidth );
        return;
        }

    pieceSplitWords( inString );

    pieceClearLines();


    while( nextWord < pieceNumWords ) {

        if( addWordToLine( pieceNumLines,
                           nextWord ) ) {

            int  pixLen =
                maxigin_measureLangTextString(
                    pieceLineBuffer[ pieceNumLines ] );
            
            if( pixLen > inMaxPixelWidth ) {
                /* too big, line is full */

                removeWordFromLine( pieceNumLines,
                                    nextWord );

                pieceNumLines++;

                if( pieceNumLines >= MAX_NUM_LINES ) {
                    return;
                    }
                }
            else {
                /* word fits, go on to next */
                nextWord ++;
                }
            }
        }

    /* count last line */
    pieceNumLines ++;
    }




void drawPieceInfoPanel( ChessPiece     inPiece,
                         int            inCenterX,
                         int            inCenterY,
                         unsigned char  inFade ) {

    ChessPiece   c  =  inPiece & CHESS_COLOR_MASK;
    ChessPiece   t  =  inPiece & CHESS_TYPE_MASK;

    const char  *title;
    const char  *desc;
    
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
                                        inCenterY - 60 + 14 * w,
                                        MAXIGIN_CENTER );
            }
        }


    
    desc = maxigin_getLangText( pieceDescriptionKeys[ t ] );

    pieceSplitLines( desc,
                     99 );

    if( pieceNumLines > 0 ) {

        int  n;

        for( n = 0;
             n < pieceNumLines;
             n ++ ) {
            
            maxigin_drawLangTextString( pieceLineBuffer[ n ],
                                        inCenterX - 47,
                                        inCenterY - 60 + 30 + 14 * n,
                                        MAXIGIN_LEFT);
            }
        }
    }



#endif

#endif
