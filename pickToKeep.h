/*
  Include in your C code wherever like so:

      #include "pickToKeep.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define PARTICLE_SPRITES_IMPLEMENTATION
      #include "pickToKeep.h"

*/

#ifndef PICK_TO_KEEP_H_INCLUDED
#define PICK_TO_KEEP_H_INCLUDED


#include "deck.h"


void pickToKeepSetup( int    inPointerActionHandle,
                      int    inActionHandle,
                      int    inCenterX,
                      int    inCenterY  );

void pickToKeepDraw( void );


/* returns moused-over piece in shop (for display on external info panel)
   or noPiece if nothing moused over */
ChessPiece pickToKeepStep( void );


/* returns noPiece until something is picked
   resets back to noPiece after next calle to pickToKeepSetup */
ChessPiece pickToKeepGetPicked( void );


char pickToKeepCanPick( void );



#endif



#ifdef  PICK_TO_KEEP_IMPLEMENTATION

#ifndef PICK_TO_KEEP_IMPLEMENTATION_INCLUDED
#define PICK_TO_KEEP_IMPLEMENTATION_INCLUDED


#include "shop.h"
#include "memoryRegister.h"
#include "playerDeck.h"


#define  NUM_PICK_TO_KEEP_CHOICES  3


static  ChessPiece  pickChoices[ NUM_PICK_TO_KEEP_CHOICES ];
static  int         pickSlotsX [ NUM_PICK_TO_KEEP_CHOICES ];
static  int         pickSlotsY [ NUM_PICK_TO_KEEP_CHOICES ];

static  int         pickOverChoice            =  -1;
static  ChessPiece  pickFinalChoice           =  noPiece;
static  int         pickPointerActionHandle;
static  int         pickActionHandle;
static  char        canPick                   =   0;


void pickToKeepSetup( int    inPointerActionHandle,
                      int    inActionHandle,
                      int    inCenterX,
                      int    inCenterY ) {

    int  xSep   =  50;
    int  xCur;
    int  i;
    
    pickPointerActionHandle = inPointerActionHandle;
    pickActionHandle = inActionHandle;

    playerDeckFullRefresh();

    pickOverChoice = -1;
    pickFinalChoice = noPiece;

    if( NUM_PICK_TO_KEEP_CHOICES % 2 == 0 ) {

        int  sepHalfSteps  =  ( NUM_PICK_TO_KEEP_CHOICES - 1 ) / 2;

        xCur = inCenterX - xSep * sepHalfSteps - xSep / 2;
        }
    else {
        /* odd number of slots */
        int  sepHalfSteps  =  NUM_PICK_TO_KEEP_CHOICES / 2;

        xCur = inCenterX - xSep * sepHalfSteps;
        }

    canPick = 0;

    for( i = 0;
         i < NUM_PICK_TO_KEEP_CHOICES;
         i   ++ ) {

        /* some of these might be noPiece, but that's okay */
        pickChoices[ i ] = playerDeckDraw();

        if( pickChoices[ i ] != noPiece ) {
            canPick = 1;
            }
        
        pickSlotsX [ i ] = xCur;
        pickSlotsY [ i ] = inCenterY;
        
        xCur += xSep;
        }

    REGISTER_ARRAY_MEM( pickChoices );
    REGISTER_VAL_MEM( pickFinalChoice );
    }

    

void pickToKeepDraw( void ) {

    int  i;
    
    for( i = 0;
         i < NUM_PICK_TO_KEEP_CHOICES;
         i   ++ ) {
        if( pickChoices[ i ] != noPiece ) {

            ChessPiece  p  =  pickChoices[ i ] | CHESS_WHITE;
            
            drawPiece( p,
                       pickSlotsX[ i ],
                       pickSlotsY[ i ] );

            if( pickOverChoice == i ) {
                drawPieceGlowOnly( p,
                                   pickSlotsX[ i ],
                                   pickSlotsY[ i ],
                                   255 );
                }
            }
        }
    }



ChessPiece pickToKeepStep( void ) {
    return noPiece;
    }



ChessPiece pickToKeepGetPicked( void ) {
    return noPiece;
    }


char pickToKeepCanPick( void ) {
    return canPick;
    }


#endif



#endif

