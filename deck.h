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
#define  DECK_VARIANCE_FACTOR        4

#define  MAX_DECK_SIZE              ( NUM_CHESS_PIECES             \
                                      * MAX_DECK_PIECE_OCCURRENCE  \
                                      * DECK_VARIANCE_FACTOR )

typedef struct Deck {

        int  numPieces;

        /* starts at ( numPieces - 1 ) for a fresh deck, and advances
           toward 0
           -1 means the whole deck has been drawn */
        int  drawPos;

        ChessPiece  pieces[ MAX_DECK_SIZE ];
        
    } Deck;



/* gets a shuffled player start deck */
void getPlayerStartDeck( Deck  *outDeck );


/* gets a shuffled shop deck */
void getShopDeck( Deck  *outDeck );




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


#define SHOP_OCCURRENCE_LIST( C, V )      \
    V( C, 0,   noPiece,      0   ) \
    V( C, 1,   pawn,         10  ) \
    V( C, 2,   bishop,       8   ) \
    V( C, 3,   knight,       8   ) \
    V( C, 4,   rook,         6   ) \
    V( C, 5,   queen,        4   ) \
    V( C, 6,   king,         0   ) \
    V( C, 7,   laserRook,    2   ) \
    V( C, 8,   laserPawn,    3   ) \
    V( C, 9,   doublingPawn, 1   ) \
    V( C, 10,  addingRook,   1   ) \
    V( C, 11,  rocket,       2   )

static int deckShopOccurrence[] = {
    MAKE_CHESS_ARRAY( SHOP_OCCURRENCE_LIST )
    };

CHECK_CHESS_ARRAY( deckShopOccurrence,
                   SHOP_OCCURRENCE_LIST );

#endif
#endif
