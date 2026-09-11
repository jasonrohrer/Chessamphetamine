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



ChessPiece playerDeckDraw( void );


char playerDeckJustRefreshed( void );


void playerDeckReshuffle( void );


void playerDeckReturnPieceUnplayed( ChessPiece   inPiece );

void playerDeckReturnPiecePlayed( ChessPiece   inPiece );



/* adds a newly purchased piece to the player deck setup */
void playerDeckAddPiece( ChessPiece   inPiece );


/* total size, including played and unplayed */
int playerDeckGetSize( void );


int playerDeckGetReadyCount( void );



/* gets a static array of flags indicating whether each position
   in deck is played or not */
char *playerDeckGetPiecePlayedMap( void );





#endif



#ifdef  PLAYER_DECK_IMPLEMENTATION

#ifndef PLAYER_DECK_IMPLEMENTATION_INCLUDED
#define PLAYER_DECK_IMPLEMENTATION_INCLUDED

#include "deck.h"


static  int         playerDeckNumPlayed                 =  0;

static  ChessPiece  playerDeckPlayed[ MAX_DECK_SIZE ];


static  Deck  playerDrawDeck;

static  int   playerDeckRefreshSound  =  -1;

static  char  justRefreshed           =   0;


void playerDeckInit( void ) {

    playerDeckRefreshSound = maxigin_initSoundEffect( "deckRefresh_sd_20.wav" );
    
    REGISTER_VAL_MEM( playerDeckNumPlayed );
    REGISTER_ARRAY_MEM( playerDeckPlayed );

    REGISTER_VAL_MEM( playerDrawDeck );
    }



void  playerDeckSetupFresh( void ) {
    playerDeckNumPlayed = 0;

    getPlayerStartDeck( &playerDrawDeck );
    }



Deck *playerDeckGetDrawDeck( void ) {
    return &playerDrawDeck;
    }



ChessPiece playerDeckDraw( void ) {

    ChessPiece  p  =  deckDraw( &playerDrawDeck );

    if( p == noPiece
        &&
        playerDeckGetSize() > 0 ) {

        /* a non-empty deck, but all pieces not present
           means we've played through all pieces
           Return all played pieces back to present status */
        int  i;

        for( i = 0;
             i < playerDeckNumPlayed;
             i ++ ) {
            deckReturnPiece( &playerDrawDeck,
                             playerDeckPlayed[ i ] );
            }
        playerDeckNumPlayed = 0;

        maxigin_playSoundEffect( playerDeckRefreshSound,
                                 256 );

        justRefreshed = 1;

        /* Note that if we still have some pieces out that haven't
           been marked as played yet, those would NOT be re-marked
           as present in that case.
           So, even after we return all pieces and trigger a reshuffle
           with a redraw here, some pieces might still be marked as
           not present */
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

    /* this returns a piece to the back of the deck, and
       remarks it as present, but we still have to draw through
       the rest of the un-drawn deck before we can ever re-draw this
       returned piece */
    deckReturnPiece( &playerDrawDeck,
                     inPiece );
    }



void playerDeckReturnPiecePlayed( ChessPiece   inPiece ) {

    if( playerDeckNumPlayed < MAX_DECK_SIZE - 1 ) {

        playerDeckPlayed[ playerDeckNumPlayed ] = inPiece;
        playerDeckNumPlayed ++;
        }
    }



void playerDeckAddPiece( ChessPiece   inPiece ) {
    deckAddPiece( &playerDrawDeck,
                  inPiece );
    }



int playerDeckGetSize( void ) {
    return deckGetSize( &playerDrawDeck );
    }



int playerDeckGetReadyCount( void ) {

    int  count  =  0;
    int  i;

    for( i = 0;
         i < playerDrawDeck.numPieces;
         i ++ ) {

        if( playerDrawDeck.present[ i ] ) {
            count ++;
            }
        }

    return count;
    }



char *playerDeckGetPiecePlayedMap( void ) {

    static  char  map[ MAX_DECK_SIZE ];

    static  char  playedUsed[ MAX_DECK_SIZE ];

    int  i;
    int  deckSize  =  deckGetSize( &playerDrawDeck );

    for( i = 0;
         i < MAX_DECK_SIZE;
         i ++ ) {
        
        map       [ i ] = 0;
        playedUsed[ i ] = 0;
        }

    for( i = 0;
         i < deckSize;
         i ++ ) {

        int  j;

        ChessPiece  p  =  playerDrawDeck.pieces[ i ];

        if( playerDrawDeck.present[ i ] ) {
            /* present pieces never marked as played */
            continue;
            }

        for( j = 0;
             j < playerDeckNumPlayed;
             j ++ ) {

            if( ! playedUsed[ j ]
                &&
                playerDeckPlayed[ j ] == p ) {

                map[ i ] = 1;
                playedUsed[ j ] = 1;
                
                break;
                }
            }
        }
    

    return map;
    }



#endif
#endif
