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

/* use decks for two things:

   1.  The player's growing deck, which starts as a starter deck and adds
       pieces from the shop.

   2.  The game deck used to populate pieces from the shop.

   It's this second use that is more complicated, because we populate
   the shop deck based on rarities and draw from it, and then re-shuffle
   when it's empty.
*/



/* this controls the max rarity possible,
   because some pieces can be 10x less common as a piece that occurs 10x more
   often */
#define  MAX_DECK_PIECE_OCCURRENCE  10

/* if we just filled the deck one time, based on piece occurrence,
   there would be exactly 1 of each of the rarest pieces in the deck.
   
   If the deck had 100 pieces, you would be waiting 100 draws to see a repeat
   of those rarest pieces (ignoring the effect of re-shuffling when the deck
   runs out).
   
   A single deck like this is the utlimate variance reducer, where you draw the
   1/100 piece exactly once every 100 draws, and you draw the 1/10 piece exactly
   10 times across the deck of 100 pieces.  I.e., the player is never waiting
   forever for a specific rare piece to drop (which can happen if we just
   draw pieces based on rarity using an RNG without a deck of pieces).
   
   We can add more variance back in by shuffling multiple decks together, so
   across 100 draws, we sometimes see more clustering of rare pieces, larger
   gaps, etc.

   The expected values are still the same (the rare piece is expected to
   be drawn in 1/100 draws), but the variance is larger.

   And yet in the worst case, the player never waits forever for a specific
   rare piece to drop (worst case, if we shuffle 4 decks of 100 pieces together,
   is that they go 396 draws without seeing a specific rare piece (and then see
   all 4 copies right at the end).
*/
#define  SHOP_DECK_VARIANCE_FACTOR        4

#define  MAX_DECK_SIZE              ( NUM_CHESS_PIECES              \
                                      * MAX_DECK_PIECE_OCCURRENCE   \
                                      * SHOP_DECK_VARIANCE_FACTOR )

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

        /* presence in deck can be ignored for shop deck */
        char        trackPresent;
        
    } Deck;



void  deckInit( void );


/* gets a shuffled player start deck */
void getPlayerStartDeck( Deck  *outDeck );


/* gets a shuffled shop deck
   inRarityFilter filters out more common pieces by only including
   pieces with occurrence factor  <=  inRarityFilter
   passing MAX_DECK_PIECE_OCCURRENCE includes all pieces
   passing 1  includes only the most rare pieces */
void getShopDeck( Deck  *outDeck,
                  int    inRarityFilter );



/* draws a piece from deck
   reshuffles deck as-needed */
ChessPiece deckDraw( Deck  *inDeck );


/* returns a piece that has been drawn to the deck
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





#ifdef DECK_IMPLEMENTATION


#define SHOP_OCCURRENCE_LIST( C, V )  \
    V( C, 0,   noPiece,      0   )    \
    V( C, 1,   pawn,         10  )    \
    V( C, 2,   bishop,       8   )    \
    V( C, 3,   knight,       8   )    \
    V( C, 4,   rook,         6   )    \
    V( C, 5,   queen,        4   )    \
    V( C, 6,   king,         0   )    \
    V( C, 7,   laserRook,    2   )    \
    V( C, 8,   laserPawn,    3   )    \
    V( C, 9,   doublingPawn, 1   )    \
    V( C, 10,  addingRook,   1   )    \
    V( C, 11,  rocket,       2   )

static  int  deckShopOccurrence[] = {
    MAKE_CHESS_ARRAY( SHOP_OCCURRENCE_LIST )
    };

CHECK_CHESS_ARRAY( deckShopOccurrence,
                   SHOP_OCCURRENCE_LIST );


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
    V( C, 9,   doublingPawn, 4   )           \
    V( C, 10,  addingRook,   0   )           \
    V( C, 11,  rocket,       4   )

static  int  deckPlayerOccurrence[] = {
    MAKE_CHESS_ARRAY( PLAYER_DECK_OCCURRENCE_LIST )
    };

CHECK_CHESS_ARRAY( deckPlayerOccurrence,
                   PLAYER_DECK_OCCURRENCE_LIST );


static  MaxiginRand  deckRand;



void  deckInit( void ) {

    int  i;
    
    maxigin_randSeed( &deckRand,
                      mingin_getEntropySeed() );
    REGISTER_VAL_MEM( deckRand );

    for( i = FIRST_CHESS_PIECE;
         i < NUM_CHESS_PIECES;
         i ++ ) {

        if( deckShopOccurrence[i] > MAX_DECK_PIECE_OCCURRENCE ) {

            maxigin_logInt( 
                maxigin_stringConcat3(
                    "ERROR:  ",
                    getPieceName( (ChessPiece)i ),
                    " has larger than MAX_DECK_PIECE_OCCURRENCE"
                    " in in deckShopOccurrence (deck.h):  " ),
                deckShopOccurrence[i] );

            deckShopOccurrence[i] = MAX_DECK_PIECE_OCCURRENCE;
            }

        if( deckPlayerOccurrence[i] > MAX_DECK_PIECE_OCCURRENCE ) {

            maxigin_logInt( 
                maxigin_stringConcat3(
                    "ERROR:  ",
                    getPieceName( (ChessPiece)i ),
                    " has larger than MAX_DECK_PIECE_OCCURRENCE"
                    " in deckPlayerOccurrence (deck.h):  " ),
                deckPlayerOccurrence[i] );

            deckPlayerOccurrence[i] = MAX_DECK_PIECE_OCCURRENCE;
            }
        }
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

    inDeck->pieces[ newIndex ] = inPiece;
    }



static void getFreshDeck( Deck  *outDeck,
                          int   *inOccurenceList,
                          int    inVarianceFactor,
                          int    inRarityFilter ) {
    int  i;
    int  n  =  0;

    for( i = FIRST_CHESS_PIECE;
         i < NUM_CHESS_PIECES;
         i ++ ) {

        if( inOccurenceList[i] <= inRarityFilter ) {
            
            int  v;

            for( v = 0;
                 v < inVarianceFactor;
                 v ++ ) {
            
                int  o;

                for( o = 0;
                     o < inOccurenceList[i];
                     o ++ ) {

                    outDeck->pieces[n] = (ChessPiece)i;
                    outDeck->present[n] = 1;
                    n++;
                    }
                }
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
                  deckPlayerOccurrence,
                  1,
                  MAX_DECK_PIECE_OCCURRENCE );

    /* player's deck tracks card presence, only allows drawing of
       cards that are present in deck, even after reshuffle */
    outDeck->trackPresent = 1;
    }



void getShopDeck( Deck  *outDeck,
                  int    inRarityFilter ) {
    getFreshDeck( outDeck,
                  deckShopOccurrence,
                  SHOP_DECK_VARIANCE_FACTOR,
                  inRarityFilter );
    }



ChessPiece deckDraw( Deck  *inDeck ) {

    /* we can assume, even if our deck tracks present
       that the present pieces are always in a block at drawPost
       and lower.

       During reshuffle, we stick all non-present pieces at end

       In case where all pieces non-present, we allow redrawing of
       non-present pieces.
    */
    
    ChessPiece  p  =  inDeck->pieces[ inDeck->drawPos ];

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




#endif
#endif
