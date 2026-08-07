/*
  Include in your C code wherever like so:

      #include "deck.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define DECK_IMPLEMENTATION
      #include "deck.h"

*/

#ifndef DECK_H_INCLUDED
#define DECK_H_INCLUDED



#define  MAX_DECK_SIZE   256


typedef struct Deck {

        int  numPieces;

        /* starts at ( numPieces - 1 ) for a fresh deck, and advances
           toward 0
           -1 means the whole deck has been drawn */
        int  drawPos;

        ChessPiece  pieces[ MAX_DECK_SIZE ];

        /* this tracks whether a piece in the deck is present or not
           for the player deck in particular, pieces can be "out"
           on the board, and shouldn't be redrawable, even if the deck
           needs to be reshuffled.
        */
        char        present[ MAX_DECK_SIZE ];

        /* presence in deck can be ignored for enemy deck */
        char        trackPresent;
        
    } Deck;



void  deckInit( void );



/* gets a shuffled player start deck */
void getPlayerStartDeck( Deck  *outDeck );


void getEmptyDeck( Deck  *outDeck,
                   char   inTrackPresent );



/* draws a piece from deck
   reshuffles deck as-needed */
ChessPiece deckDraw( Deck  *inDeck );


/* returns a piece that has been drawn before back to the deck
   (piece won't be drawable again until deck gets reshuffled when it reaches
   end )

   Only has an effect on decks that have trackPresent enabled.
*/
void deckReturnPiece( Deck        *inDeck,
                      ChessPiece   inPiece );



/* adds piece to end of deck, and leaves
   drawPos alone (so new piece goes into "already drawn" section at end)
   if there's not enough room, a piece at the end is replaced */
void deckAddPiece( Deck        *inDeck,
                   ChessPiece   inPiece );



/* reshuffles all and resets draw pos to (numPieces - 1) */
void deckReshuffleAll( Deck  *inDeck );



/* reshuffles only pieces from drawPos and earlier, leaving later pieces
   (those already drawn) alone */
void deckReshuffleRemaining( Deck  *inDeck );


void deckDrawDebugInfo( Deck  *inDeck,
                        int    inFontHandle,
                        int    inCenterX,
                        int    inCenterY );



#endif



#ifdef  DECK_IMPLEMENTATION

#ifndef DECK_IMPLEMENTATION_INCLUDED
#define DECK_IMPLEMENTATION_INCLUDED





/*
  Player's starting deck contains the 15 usual chess pieces,
  except for the king
*/
#define PLAYER_DECK_OCCURRENCE_LIST( C, V )  \
    V( C, 0,   noPiece,      0   )           \
    V( C, 1,   pawn,         8   )           \
    V( C, 2,   bishop,       2   )           \
    V( C, 3,   knight,       2   )           \
    V( C, 4,   rook,         2   )           \
    V( C, 5,   queen,        1   )           \
    V( C, 6,   king,         0   )           \
    V( C, 7,   laserRook,    0   )           \
    V( C, 8,   laserPawn,    0   )           \
    V( C, 9,   doublingPawn, 0   )           \
    V( C, 10,  addingRook,   0   )           \
    V( C, 11,  rocket,       0   )

static  int  deckPlayerOccurrence[] = {
    MAKE_CHESS_ARRAY( PLAYER_DECK_OCCURRENCE_LIST )
    };

CHECK_CHESS_ARRAY( deckPlayerOccurrence,
                   PLAYER_DECK_OCCURRENCE_LIST );


static  MaxiginRand  deckRand;



void  deckInit( void ) {

    maxigin_randSeed( &deckRand,
                      mingin_getEntropySeed() );
    REGISTER_VAL_MEM( deckRand );
    
    }



static void deckReshuffleRange( Deck  *inDeck,
                                int    inLastIndex ) {

    static  int         shuffleIndices    [ MAX_DECK_SIZE ];
    static  ChessPiece  shuffleTemp       [ MAX_DECK_SIZE ];
    static  char        shuffleTempPresent[ MAX_DECK_SIZE ];
    
    int  i;

    for( i = 0;
         i <= inLastIndex;
         i ++ ) {

        shuffleIndices[i] = i;

        shuffleTemp       [ i ] = inDeck->pieces [ i ];
        shuffleTempPresent[ i ] = inDeck->present[ i ];
        }

    maxigin_shuffle( &deckRand,
                     inLastIndex + 1,
                     shuffleIndices );

    for( i = 0;
         i <= inLastIndex;
         i ++ ) {

        inDeck->pieces [ i ] = shuffleTemp       [ shuffleIndices[i] ];
        inDeck->present[ i ] = shuffleTempPresent[ shuffleIndices[i] ];
        }
    }



void deckReshuffleAll( Deck  *inDeck ) {
    inDeck->drawPos = inDeck->numPieces - 1;

    deckReshuffleRemaining( inDeck );

    if( inDeck->trackPresent ) {

        /* put all non-present pieces at end */

        static  ChessPiece  tempDeck   [ MAX_DECK_SIZE ];
        static  char        tempPresent[ MAX_DECK_SIZE ];

        int  i;
        int  tempPos = 0;

        int  numPresent  = 0;

        /* first present pieces */
        for( i = 0;
             i < inDeck->numPieces;
             i ++ ) {

            if( inDeck->present[i] ) {

                tempDeck   [ tempPos ] = inDeck->pieces [i];
                tempPresent[ tempPos ] = inDeck->present[i];

                tempPos ++;
                numPresent ++;
                }
            }

        /* now non-present pieces at end */
        for( i = 0;
             i < inDeck->numPieces;
             i ++ ) {

            if( ! inDeck->present[i] ) {

                tempDeck   [ tempPos ] = inDeck->pieces [i];
                tempPresent[ tempPos ] = inDeck->present[i];

                tempPos ++;
                }
            }

        /* now copy back to main deck */
        for( i = 0;
             i < inDeck->numPieces;
             i ++ ) {

            inDeck->pieces [i] = tempDeck   [ i ];
            inDeck->present[i] = tempPresent[ i ];
            }

        /* skip non-present when drawing,
           but only if we can
           if all pieces are not present, leave drawPos alone
           and allow redrawing of non-present pieces */
        if( numPresent > 0 ) {
            inDeck->drawPos = numPresent - 1;
            }
        }
    }



void deckReshuffleRemaining( Deck  *inDeck ) {
    deckReshuffleRange( inDeck,
                        inDeck->drawPos );
    }



void deckAddPiece( Deck        *inDeck,
                   ChessPiece   inPiece ) {

    int  newIndex;

    if( inDeck->numPieces < MAX_DECK_SIZE ) {

        newIndex = inDeck->numPieces;
        inDeck->numPieces ++;
        }
    else {
        /* deck already full, replace piece at end */
        newIndex = MAX_DECK_SIZE - 1;
        }

    inDeck->pieces[ newIndex ]  = inPiece;
    inDeck->present[ newIndex ] = 1;
    }



static void getFreshDeck( Deck  *outDeck,
                          int   *inOccurenceList  ) {
    int  i;
    int  n  =  0;

    for( i = FIRST_CHESS_PIECE;
         i < NUM_CHESS_PIECES;
         i ++ ) {

        int  o;

        for( o = 0;
             o < inOccurenceList[i];
             o ++ ) {

            outDeck->pieces[n] = (ChessPiece)i;
            outDeck->present[n] = 1;
            n++;
            }
        }

    outDeck->numPieces    = n;
    outDeck->drawPos      = 0;
    outDeck->trackPresent = 0;

    deckReshuffleAll( outDeck );
    }

                          

void getPlayerStartDeck( Deck  *outDeck ) {
    /* no extra copies in player deck */
    getFreshDeck( outDeck,
                  deckPlayerOccurrence  );

    /* player's deck tracks card presence, only allows drawing of
       cards that are present in deck, even after reshuffle */
    outDeck->trackPresent = 1;
    }


void getEmptyDeck( Deck  *outDeck,
                   char   inTrackPresent ) {
    
    outDeck->numPieces    = 0;
    outDeck->drawPos      = -1;
    outDeck->trackPresent = inTrackPresent;
    }





ChessPiece deckDraw( Deck  *inDeck ) {

    /* we can assume, even if our deck tracks present
       that the present pieces are always in a block at drawPos
       and lower.

       During reshuffle, we stick all non-present pieces at end

       In case where all pieces non-present, we allow redrawing of
       non-present pieces.
    */
    
    ChessPiece  p;

    if( inDeck->drawPos < 0 ) {
        deckReshuffleAll( inDeck );
        }

    if( inDeck->drawPos < 0 ) {
        /* empty deck */
        return noPiece;
        }

    p = inDeck->pieces[ inDeck->drawPos ];

    if( inDeck->trackPresent ) {
        inDeck->present[ inDeck->drawPos ] = 0;
        }
    
    inDeck->drawPos --;

    if( inDeck->drawPos < 0 ) {
        deckReshuffleAll( inDeck );
        }

    return p;
    }



void deckReturnPiece( Deck        *inDeck,
                      ChessPiece   inPiece ) {

    ChessPiece  pieceType  =  inPiece & CHESS_TYPE_MASK;

    if( inDeck->trackPresent ) {
        
        int  i;

        for( i =  inDeck->numPieces - 1;
             i >= 0;
             i -- ) {

            if( ! inDeck->present[ i ]
                &&
                inDeck->pieces[ i ] == pieceType ) {

                inDeck->present[ i ] = 1;
                return;
                }
            }
        }
    }



void deckDrawDebugInfo( Deck  *inDeck,
                        int    inFontHandle,
                        int    inCenterX,
                        int    inCenterY ) {
    int  i;

    int  y  =  inCenterY;
    

    for( i =  inDeck->numPieces - 1;
         i >= 0;
         i -- ) {

        const char *name = getPieceName( inDeck->pieces[i] );

        if( i <= inDeck->drawPos ) {

            maxigin_drawSetColor( 255,
                                  255,
                                  255,
                                  255 );
            }
        else {
            
            if( inDeck->present[ i ] ) {
                maxigin_drawSetColor( 0,
                                      255,
                                      0,
                                      255 );
                }
            else {
                maxigin_drawSetColor( 255,
                                      0,
                                      0,
                                      255 );
                }
            }

        maxigin_drawText( inFontHandle,
                          name,
                          inCenterX,
                          y,
                          MAXIGIN_CENTER );

        y += 8;
        }

    }




#endif
#endif
