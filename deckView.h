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



static  DeckViewSlot   deckViewSlots[ MAX_DECK_SIZE ];

static  int            deckViewNumFullSlots              =  0;

#define  DECK_VIEW_ROWS       4
#define  DECK_VIEW_COLS       6
#define  DECK_VIEW_VIS_SLOTS  ( DECK_VIEW_ROWS * DECK_VIEW_COLS )


static  int            deckViewSlotPosX     [ DECK_VIEW_VIS_SLOTS ];
static  int            deckViewSlotPosY     [ DECK_VIEW_VIS_SLOTS ];
static  unsigned char  deckViewHighlightFade[ DECK_VIEW_VIS_SLOTS ];

static  int            deckViewCellSizeX                =  BOARD_SQUARE_SIZE;
static  int            deckViewCellSizeY                =  2 * BOARD_SQUARE_SIZE;
static  int            deckViewPageNumber               =  0;


static void deckViewClear( void ) {
    deckViewNumFullSlots = 0;
    deckViewPageNumber   = 0;
    }



void deckViewInit(  int  inCenterX,
                    int  inCenterY ) {

    int  x;
    int  y;
    int  i      =  0;
    int  yPos;
    
    deckViewCenterX = inCenterX;
    deckViewCenterY = inCenterY;

    deckViewClear();

    yPos  =
        deckViewCenterY
        - ( ( DECK_VIEW_ROWS -  1 ) * deckViewCellSizeY ) / 2;

    for( y = 0;
         y < DECK_VIEW_ROWS;
         y ++ ) {

        /* restart xPos for each row */
        int  xPos  =
            deckViewCenterX
            - ( ( DECK_VIEW_COLS -  1 ) * deckViewCellSizeX ) / 2;

        for( x = 0;
             x < DECK_VIEW_COLS;
             x ++ ) {

            deckViewSlotPosX[i] = xPos;
            deckViewSlotPosY[i] = yPos;
            
            i ++;
            xPos += deckViewCellSizeX;
            }
        
        yPos += deckViewCellSizeY;
        }

    for( i = 0;
         i < DECK_VIEW_VIS_SLOTS;
         i ++ ) {
        deckViewHighlightFade[i] = 0;
        }
    
    REGISTER_ARRAY_MEM( deckViewSlots        );
    REGISTER_ARRAY_MEM( deckViewHighlightFade );

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

    int  skip  =  deckViewPageNumber * DECK_VIEW_VIS_SLOTS;

    int  i     =  skip;

    int  y;
    int  x;
    

    for( y = 0;
         y < DECK_VIEW_ROWS
         &&
         i < deckViewNumFullSlots;
         y ++ ) {

        for( x = 0;
             x < DECK_VIEW_COLS
             &&
             i < deckViewNumFullSlots;
             x ++ ) {

            int  xPos  =  deckViewSlotPosX[i];
            int  yPos  =  deckViewSlotPosY[i];
            
            drawPiece( deckViewSlots[i].piece,
                       xPos,
                       yPos );

            if( ! deckViewSlots[i].present ) {

                drawPieceShadowOnly( deckViewSlots[i].piece,
                                     xPos,
                                     yPos );

                /* this is place-holder anyway...
                   another one lower down */
                drawPieceShadowOnly( deckViewSlots[i].piece,
                                     xPos,
                                     yPos + BOARD_SQUARE_SIZE / 2 );
                }

            if( deckViewHighlightFade[i - skip] > 0 ) {
                drawPieceHighlight( deckViewSlots[i].piece,
                                     xPos,
                                     yPos,
                                     deckViewHighlightFade[i] );
                }
            
            i ++;
            }
        
        }

    }



ChessPiece deckViewStep( void ) {

    /* fixme:
       also handle case where controller is used */
    
    int  pointerX;
    int  pointerY;
    int  i;
    int  skip       =  deckViewPageNumber * DECK_VIEW_VIS_SLOTS;
    int  r          =  mingin_getStepsPerSecond();
    int  deltaFade  =  ( 20 * 60 ) / r;
    int  overSlot   =  -1;

    
    if( ! maxigin_getPointerLocation( &pointerX,
                                      &pointerY ) ) {
        /* pointer not available */
        return noPiece;
        }
    

    for( i = 0;
         i < DECK_VIEW_VIS_SLOTS;
         i ++ ) {

        ChessPiece  p;

        if( i + skip >= deckViewNumFullSlots ) {
            break;
            }
        
        p =  deckViewSlots[ i + skip ].piece;

        if( p != noPiece ) {

            if( getPixelOverPiece( p | CHESS_WHITE,
                                   deckViewSlotPosX[i],
                                   deckViewSlotPosY[i],
                                   pointerX,
                                   pointerY ) ) {

                overSlot = i;
                deckViewHighlightFade[ i ] = 255;
                break;
                }
            }
        }

    for( i = 0;
         i < DECK_VIEW_VIS_SLOTS;
         i ++ ) {

        if( i + skip >= deckViewNumFullSlots ) {
            break;
            }

        if( i != overSlot
            &&
            deckViewHighlightFade[i] > 0 ) {

            int  newHighlight = deckViewHighlightFade[i] - deltaFade;

            if( newHighlight > 0 ) {
                deckViewHighlightFade[i] = (unsigned char)newHighlight;
                }
            else {
                deckViewHighlightFade[i] = 0;
                }
            }
        }

    if( overSlot != -1 ) {
        return deckViewSlots[ overSlot + skip ].piece;
        }
    else {
        return noPiece;
        }
    }




#endif
#endif
