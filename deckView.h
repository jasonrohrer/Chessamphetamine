/*
  Include in your C code wherever like so:

      #include "deckView.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define DECK_VIEW_IMPLEMENTATION
      #include "deckView.h"

*/

#ifndef DECK_VIEW_H_INCLUDED
#define DECK_VIEW_H_INCLUDED

#include "deck.h"


void deckViewInit(  int  inCenterX,
                    int  inCenterY );


void deckViewSet( Deck *inDeck );


void deckViewDraw( void );


/* returns current moused-over piece for sidebar info panel */
ChessPiece deckViewStep( void );



#ifdef DECK_VIEW_IMPLEMENTATION


#include "memoryRegister.h"


static  int  deckViewCenterX;
static  int  deckViewCenterY;


typedef struct DeckViewSlot {
        
        ChessPiece  piece;

        char        present;
        
    } DeckViewSlot;



static  DeckViewSlot  deckViewSlots[ MAX_DECK_SIZE ];

static  int           deckViewNumFullSlots             =  0;

static  int           deckViewRowsPerPage              =  6;
static  int           deckViewColsPerPage              =  6;
static  int           deckViewSlotsPerPage;
static  int           deckViewCellSize                 =  BOARD_SQUARE_SIZE;
static  int           deckViewPageNumber               =  0;


static void deckViewClear( void ) {
    deckViewNumFullSlots = 0;
    deckViewPageNumber   = 0;
    }



void deckViewInit(  int  inCenterX,
                    int  inCenterY ) {
    
    deckViewCenterX = inCenterX;
    deckViewCenterY = inCenterY;

    deckViewSlotsPerPage = deckViewRowsPerPage * deckViewColsPerPage;

    deckViewClear();

    REGISTER_ARRAY_MEM( deckViewSlots );

    REGISTER_VAL_MEM( deckViewNumFullSlots );
    REGISTER_VAL_MEM( deckViewPageNumber   );
    }



void deckViewSet( Deck *inDeck ) {

    int  i;
    int  n   =  0;

    deckViewPageNumber   = 0;
    deckViewNumFullSlots = inDeck->numPieces;
    

    /* show in order
       so we don't give away shuffled order */
    for( i = FIRST_CHESS_PIECE;
         i < NUM_CHESS_PIECES;
         i ++ ) {
        
        char  present;

        /* show present pieces of this type first,
           followed by non-present ones */
        for( present = 1;
             present >= 0;
             present -- ) {

            int  p;

            for( p = 0;
                 p < inDeck->numPieces;
                 p ++ ) {

                if( inDeck->present[p] == present ) {
                    
                    ChessPiece  thisPiece  =  inDeck->pieces[p];

                    if( thisPiece == i ) {
                        deckViewSlots[n].piece   = thisPiece;
                        deckViewSlots[n].present = present;
                        n++;
                        }
                    }
                }
            }
        }
    }



void deckViewDraw( void ) {

    int  skip  =  deckViewPageNumber * deckViewSlotsPerPage;

    int  i     =  skip;
    
    
    int  yPos  =
        deckViewCenterY
        - ( ( deckViewRowsPerPage -  1 ) * deckViewCellSize ) / 2;

    int  y;
    int  x;
    

    for( y = 0;
         y < deckViewRowsPerPage
         &&
         i < deckViewNumFullSlots;
         y ++ ) {

        /* restart xPos for each row */
        int  xPos  =
            deckViewCenterX
            - ( ( deckViewColsPerPage -  1 ) * deckViewCellSize ) / 2;

        for( x = 0;
             x < deckViewRowsPerPage
             &&
             i < deckViewNumFullSlots;
             x ++ ) {

            drawPiece( deckViewSlots[i].piece,
                       xPos,
                       yPos );

            if( ! deckViewSlots[i].present ) {

                drawPieceShadowOnly( deckViewSlots[i].piece,
                                     xPos,
                                     yPos );
                }


            xPos += deckViewCellSize;
            }
        
        yPos += deckViewCellSize;
        }

    }



ChessPiece deckViewStep( void ) {
    /* fixme */
    return noPiece;
    }




#endif
#endif
