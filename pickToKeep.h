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


void pickToKeepInit( int    inPointerActionHandle,
                     int    inActionHandle,
                     int    inCenterX,
                     int    inCenterY  );

void pickToKeepSetup( void );

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
#include "unlocks.h"
#include "tipDisplay.h"


#define  NUM_PICK_TO_KEEP_CHOICES  3


static  ChessPiece     pickChoices  [ NUM_PICK_TO_KEEP_CHOICES ];
static  int            pickSlotsX   [ NUM_PICK_TO_KEEP_CHOICES ];
static  int            pickSlotsY   [ NUM_PICK_TO_KEEP_CHOICES ];
static  unsigned char  pickSlotsFade[ NUM_PICK_TO_KEEP_CHOICES ];

static  int            pickOverChoice            =  -1;
static  ChessPiece     pickFinalChoice           =  noPiece;
static  int            pickPointerActionHandle;
static  int            pickActionHandle;
static  char           canPick                   =   0;



void pickToKeepInit( int  inPointerActionHandle,
                     int  inActionHandle,
                     int  inCenterX,
                     int  inCenterY ) {

    int  xSep   =  50;
    int  xCur;
    int  i;
    
    pickPointerActionHandle = inPointerActionHandle;
    pickActionHandle = inActionHandle;

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

        pickSlotsX [ i ] = xCur;
        pickSlotsY [ i ] = inCenterY;

        pickChoices[ i ] = noPiece;

        pickSlotsFade[ i ] = 0;
        
        xCur += xSep;
        }

    REGISTER_ARRAY_MEM( pickChoices );
    REGISTER_ARRAY_MEM( pickSlotsFade );
    
    REGISTER_VAL_MEM( pickFinalChoice );
    REGISTER_VAL_MEM( pickOverChoice );
    REGISTER_VAL_MEM( canPick );
    }

    

void pickToKeepSetup( void ) {

    int  i;
    
    playerDeckFullRefresh();

    pickOverChoice = -1;
    pickFinalChoice = noPiece;

    canPick = 0;

    for( i = 0;
         i < NUM_PICK_TO_KEEP_CHOICES;
         i   ++ ) {

        /* some of these might be noPiece, but that's okay */
        pickChoices[ i ] = playerDeckDraw();

        if( pickChoices[ i ] != noPiece ) {
            canPick = 1;
            }
        
        pickSlotsFade[ i ] = 0;
        }
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

            if( pickSlotsFade[ i ] > 0 ) {
                drawPieceGlowOnly( p,
                                   pickSlotsX[ i ],
                                   pickSlotsY[ i ],
                                   pickSlotsFade[ i ] );
                }
            }
        }
    }



ChessPiece pickToKeepStep( void ) {

    int   pointerX;
    int   pointerY;
    int   i;
    int   r                    =  mingin_getStepsPerSecond();
    int   deltaFade            =  ( 20 * 60 ) / r;
    
    char  controllerMovedSlot  =  0;

    if( unlocksIsViewerActive()
        ||
        tipDisplayIsShowing() ) {
        pickOverChoice = -1;
        }
    
    if( maxigin_getActivePointerLocation( &pointerX,
                                          &pointerY ) ) {
        
        pickOverChoice = -1;
        
        for( i = 0;
             i < NUM_PICK_TO_KEEP_CHOICES;
             i ++ ) {

            ChessPiece  p  =  pickChoices[ i ];

            if( p != noPiece ) {

                if( getPixelOverPiece( p | CHESS_WHITE,
                                       pickSlotsX[ i ],
                                       pickSlotsY[ i ],
                                       pointerX,
                                       pointerY ) ) {

                    pickOverChoice = i;
                    pickSlotsFade[ i ] = 255;
                    break;
                    }
                }
            }
        }
    else {
        /* examine slots with controller */

        static  char  presentMap[ NUM_PICK_TO_KEEP_CHOICES ];

        
        int  pickX  =  -1;
        int  pickY  =  -1;

        
        for( i = 0;
             i < NUM_PICK_TO_KEEP_CHOICES;
             i ++ ) {
            
            if( pickChoices[ i ] != noPiece ) {

                presentMap[ i ] = 1;
                }
            else {
                presentMap[ i ] = 0;
                }
            }

        if( pickOverChoice != -1 ) {
            pickY = 0;
            pickX = pickOverChoice;
            }

        sparseGridNav( presentMap,
                       NUM_PICK_TO_KEEP_CHOICES,
                       1,
                       &pickX,
                       &pickY );

        if( pickX != -1
            &&
            pickY != -1 ) {

            int  old  =  pickOverChoice;
            
            pickOverChoice = pickX;

            pickSlotsFade[ pickOverChoice ] = 255;

            if( old != pickOverChoice ) {
                if( old != -1 ) {
                    controllerMovedSlot = 1;
                    }
                unlocksCancelViewer();
                }
            }
        }

    /* fade out glows */
    for( i = 0;
         i < NUM_PICK_TO_KEEP_CHOICES;
         i ++ ) {


        if( i != pickOverChoice
            &&
            pickSlotsFade[ i ] > 0 ) {

            int  newHighlight  =  pickSlotsFade[ i ] - deltaFade;

            if( newHighlight > 0 ) {
                pickSlotsFade[ i ] = (unsigned char)newHighlight;
                }
            else {
                pickSlotsFade[ i ] = 0;
                }
            }
        }

    if( controllerMovedSlot ) {
        /* return noPiece for one step, to allow piece info panel
           to fade slightly, and so that game will play sound */
        
        return noPiece;
        }
    
    if( pickOverChoice != -1 ) {
        return pickChoices[ pickOverChoice ];
        }
    else {
        
        return noPiece;
        }
    }



ChessPiece pickToKeepGetPicked( void ) {
    return noPiece;
    }


char pickToKeepCanPick( void ) {
    return canPick;
    }


#endif



#endif

