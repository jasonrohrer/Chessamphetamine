/*
  Include in your C code wherever like so:

      #include "sideBoard.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define SIDE_BOARD_IMPLEMENTATION
      #include "sideBoard.h"

*/

#ifndef SIDE_BOARD_H_INCLUDED
#define SIDE_BOARD_H_INCLUDED


#include "deck.h"


#define  SIDE_BOARD_MAX_SLOTS  10


void sideBoardInit( int  inPointerActionHandle,
                    int  inCenterX,
                    int  inBottomSlotY );

void sideBoardSetNumSlots( int  inNumSlots );


void sideBoardRedraw( Deck *inDeck );


/* swaps a piece onto the side board if anything on the side board is selected
   returns noPiece if the swap failed */
ChessPiece sideBoardSwap( ChessPiece  inNewPiece );


/* returns 1 if something on the side board is selected, 0 if not */
char sideBoardStep( void );


void sideBoardDraw( void );




#ifdef SIDE_BOARD_IMPLEMENTATION


static  ChessPiece  sideBoard[ SIDE_BOARD_MAX_SLOTS ];

static  int         sbNumSlots             =  5;

static  int         sbPointerActionHandle  =  -1;
static  int         sbCenterX;
static  int         sbBottomSlotY;
static  int         sbPickedIndex          =  -1;


void sideBoardInit( int  inPointerActionHandle,
                    int  inCenterX,
                    int  inBottomSlotY ) {
    int  i;
    
    sbPointerActionHandle = inPointerActionHandle;
    sbCenterX             = inCenterX;
    sbBottomSlotY         = inBottomSlotY;

    for( i = 0;
         i < SIDE_BOARD_MAX_SLOTS;
         i ++ ) {
        sideBoard[ i ] = noPiece;
        }
    }
    


void sideBoardSetNumSlots( int  inNumSlots ) {
    
    sbNumSlots = inNumSlots;
    }



void sideBoardRedraw( Deck *inDeck ) {
    
    int  i;

    for( i = 0;
         i < sbNumSlots;
         i ++ ) {

        if( sideBoard[ i ] != noPiece ) {
            deckReturnPiece( inDeck,
                             sideBoard[ i ] );
            }

        sideBoard[i] = deckDraw( inDeck );
        }

    sbPickedIndex = -1;
    }



/* swaps a piece onto the side board if anything on the side board is selected
   returns noPiece if the swap failed */
ChessPiece sideBoardSwap( ChessPiece  inNewPiece ) {

    ChessPiece  retVal  =  noPiece;

    if( sbPickedIndex < sbNumSlots
        &&
        sbPickedIndex >= 0 ) {

        retVal = sideBoard[ sbPickedIndex ];
        
        sideBoard[ sbPickedIndex ] = inNewPiece;
        }

    return retVal;
    }



/* returns 1 if something on the sideboard is selected, 0 if not */
char sideBoardStep( void ) {
    return 0;
    }



void sideBoardDraw( void ) {
    }





#endif



#endif
