/*
  Include in your C code wherever like so:

      #include "playerDeck.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define PLAYER_DECK_IMPLEMENTATION
      #include "playerDeck.h"

*/

#ifndef PLAYER_DECK_H_INCLUDED
#define PLAYER_DECK_H_INCLUDED


void playerDeckInit( void );


/* returns player deck setup to initial, starting configuration */
void  playerDeckSetupFresh( void );


/* gets pointer to internal player draw deck */
Deck *playerDeckGetDrawDeck( void );


Deck *playerDeckGetDiscardDeck( void );



ChessPiece playerDeckDraw( void );


char playerDeckJustRefreshed( void );


void playerDeckReshuffle( void );


void playerDeckReturnPieceUnplayed( ChessPiece   inPiece );

void playerDeckReturnPiecePlayed( ChessPiece   inPiece );


/* after a move, check if there are any pieces that weren't captured
   but still need to be returned to the deck as played.

   Rockets are one example of this (they destroy themselves and go back
   into the discard pile). */
void playerDeckHandleSpecialPlayedReturn( BoardState  *inState,
                                          Move        *outMove,
                                          Captured    *outCaptured,
                                          BoardState  *outNewState );



/* adds a newly purchased piece to the player deck setup */
void playerDeckAddPiece( ChessPiece   inPiece );


/* total size, including played and unplayed */
int playerDeckGetSize( void );


int playerDeckGetReadyCount( void );

int playerDeckGetDiscardCount( void );




#endif



#ifdef  PLAYER_DECK_IMPLEMENTATION

#ifndef PLAYER_DECK_IMPLEMENTATION_INCLUDED
#define PLAYER_DECK_IMPLEMENTATION_INCLUDED

#include "deck.h"


static  Deck  playerDrawDeck;
static  Deck  playerDiscardDeck;


static  int   playerDeckRefreshSound  =  -1;

static  char  justRefreshed           =   0;


void playerDeckInit( void ) {

    playerDeckRefreshSound = maxigin_initSoundEffect( "deckRefresh_sd_20.wav" );
   
    REGISTER_VAL_MEM( playerDrawDeck );
    REGISTER_VAL_MEM( playerDiscardDeck );
    }



void  playerDeckSetupFresh( void ) {

    getPlayerStartDeck( &playerDrawDeck );
    /* remove on draw here too */
    getEmptyDeck( &playerDiscardDeck,
                  1 );
    }



Deck *playerDeckGetDrawDeck( void ) {
    return &playerDrawDeck;
    }


Deck *playerDeckGetDiscardDeck( void ) {
    return &playerDiscardDeck;
    }



ChessPiece playerDeckDraw( void ) {

    ChessPiece  p  =  deckDraw( &playerDrawDeck );

    if( p == noPiece
        &&
        playerDeckGetSize() > 0
        &&
        ! justRefreshed ) {

        /* don't repeat this if justRefreshed,
           becuase it means each draw from the deck is failing with noPiece,
           which means our deck is empty, and we don't want to play the
           playerDeckRefreshSound multiple times */

        /* a non-empty deck, but all pieces not present
           means we've played through all pieces
           Return all played pieces back to present status */

        ChessPiece  played  =  deckDraw( &playerDiscardDeck );

        while( played != noPiece ) {
            deckAddPiece( &playerDrawDeck,
                          played );
            played = deckDraw( &playerDiscardDeck );
            }

        maxigin_playSoundEffect( playerDeckRefreshSound,
                                 256 );

        justRefreshed = 1;

        p = deckDraw( &playerDrawDeck );
        }

    return p;
    }



char playerDeckJustRefreshed( void ) {

    char  val  =  justRefreshed;

    justRefreshed = 0;

    return val;
    }



void playerDeckReshuffle( void ) {
    deckReshuffleAll( &playerDrawDeck );
    }



void playerDeckReturnPieceUnplayed( ChessPiece   inPiece ) {

    /* this returns a piece to the back of the deck,
       but we still have to draw through
       the rest of the un-drawn deck before we can ever re-draw this
       returned piece */
    deckAddPiece( &playerDrawDeck,
                  inPiece );
    }



void playerDeckReturnPiecePlayed( ChessPiece   inPiece ) {

    deckAddPiece( &playerDiscardDeck,
                  inPiece );
    }



void playerDeckAddPiece( ChessPiece   inPiece ) {
    deckAddPiece( &playerDrawDeck,
                  inPiece );
    }



int playerDeckGetSize( void ) {
    return deckGetSize( &playerDrawDeck ) + deckGetSize( &playerDiscardDeck );
    }



int playerDeckGetReadyCount( void ) {

    return deckGetSize( &playerDrawDeck );
    }



int playerDeckGetDiscardCount( void ) {

    return deckGetSize( &playerDiscardDeck );
    }




typedef void (*SpecialReturnFunction)( BoardState  *inState,
                                       Move        *outMove,
                                       Captured    *outCaptured,
                                       BoardState  *outNewState );


static void noSpecialReturn( BoardState  *inState,
                             Move        *outMove,
                             Captured    *outCaptured,
                             BoardState  *outNewState ) {
    (void)inState;
    (void)outMove;
    (void)outCaptured;
    (void)outNewState;
    }


static void rocketSpecialReturn( BoardState  *inState,
                                 Move        *outMove,
                                 Captured    *outCaptured,
                                 BoardState  *outNewState ) {

    /* rocket always returns itself to discard pile after moving (firing) */
    int  x;
    int  y;

    ChessPiece  p;
    
    (void)outCaptured;
    (void)outNewState;

    y = outMove->startPos[ 0 ];
    x = outMove->startPos[ 1 ];

    p = inState->grid[ y ][ x ];

    if( ( p & CHESS_TYPE_MASK  ) == rocket
        &&
        ( p & CHESS_COLOR_MASK ) == CHESS_WHITE ) {

        /* sanity check, yes we are a white rocket */

        playerDeckReturnPiecePlayed( p );
        }
    }



#define SPECIAL_RETURN_FUNCTION_LIST( C, V )       \
    V( C, 0,   noPiece,      noSpecialReturn     ) \
    V( C, 1,   pawn,         noSpecialReturn     ) \
    V( C, 2,   bishop,       noSpecialReturn     ) \
    V( C, 3,   knight,       noSpecialReturn     ) \
    V( C, 4,   rook,         noSpecialReturn     ) \
    V( C, 5,   queen,        noSpecialReturn     ) \
    V( C, 6,   king,         noSpecialReturn     ) \
    V( C, 7,   laserRook,    noSpecialReturn     ) \
    V( C, 8,   laserPawn,    noSpecialReturn     ) \
    V( C, 9,   doublingPawn, noSpecialReturn     ) \
    V( C, 10,  addingRook,   noSpecialReturn     ) \
    V( C, 11,  rocket,       rocketSpecialReturn )

static SpecialReturnFunction pieceSpecialReturnFunctions[] = {
    MAKE_CHESS_ARRAY( SPECIAL_RETURN_FUNCTION_LIST )
    };

CHECK_CHESS_ARRAY( pieceSpecialReturnFunctions,
                   SPECIAL_RETURN_FUNCTION_LIST );


void playerDeckHandleSpecialPlayedReturn( BoardState  *inState,
                                          Move        *outMove,
                                          Captured    *outCaptured,
                                          BoardState  *outNewState ) {
    int  x;
    int  y;

    ChessPiece  p;
    ChessPiece  t;

    y = outMove->startPos[ 0 ];
    x = outMove->startPos[ 1 ];

    p = inState->grid[ y ][ x ];

    if( p == noPiece ) {
        return;
        }

    if( ( p & CHESS_COLOR_MASK ) == CHESS_BLACK ) {
        return;
        }

    t = p & CHESS_TYPE_MASK;

    /* player's piece initiated this move */

    pieceSpecialReturnFunctions[ t ]( inState,
                                      outMove,
                                      outCaptured,
                                      outNewState );
    }



#endif
#endif
