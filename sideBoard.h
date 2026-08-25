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


void sideBoardRedraw( Deck *inDeck );


/* swaps a piece onto the side board if anything on the side board is selected
   returns noPiece if the swap failed */
ChessPiece sideBoardSwap( ChessPiece  inNewPiece );


/* initiates lift, which happens over sideBoardSteps
   call again to check if lift is done */
char sideBoardLift( void );

void sideBoardForceFullLift( void );


/* initiates return back down from lift
   call again to check if done */
char sideBoardUnlift( void );


void sideBoardShowRedraw( char  inShow );




/* returns piece being moused over */
ChessPiece sideBoardStep( int  inPieceLiftSound );


void sideBoardDraw( void );


char sideBoardIsMouseOver( void );

void sideBoardClearPick( void );


void sideBoardGrabController( void );

char sideBoardStillHoldingController( void );



#endif



#ifdef  SIDE_BOARD_IMPLEMENTATION

#ifndef SIDE_BOARD_IMPLEMENTATION_INCLUDED
#define SIDE_BOARD_IMPLEMENTATION_INCLUDED


#include "unlocks.h"



static  ChessPiece     sideBoard      [ SIDE_BOARD_MAX_SLOTS ];
static  int            sbLift         [ SIDE_BOARD_MAX_SLOTS ];
static  int            sbSmoothLift   [ SIDE_BOARD_MAX_SLOTS ];
static  unsigned char  sbHighlightFade[ SIDE_BOARD_MAX_SLOTS ];
static  int            sbSlotPosX     [ SIDE_BOARD_MAX_SLOTS ];
static  int            sbSlotPosY     [ SIDE_BOARD_MAX_SLOTS ];

static  int            sbBaseNumSlots         =  2;
static  int            sbNumSlots             =  2;
static  int            sbMaxLift              =  100;


static  int            sbPointerActionHandle  =  -1;
static  int            sbPickedIndex          =  -1;
static  char           sbLifting              =   0;
static  char           sbDropping             =   0;

static  int            sbSlotSprite           =  -1;
static  int            sbSlotPickedSprite     =  -1;
static  int            sbSlotRedrawSprite     =  -1;

static  char           sbActionDown           =   0;
static  char           sbRedrawShowing        =   0;
static  char           sbHoldingController    =   0;
static  int            sbOverSlot             =  -1;


void sideBoardInit( int  inPointerActionHandle,
                    int  inCenterX,
                    int  inBottomSlotY ) {
    int  i;
    int  ySep  =  35;
    int  yPos  =  inBottomSlotY;

    sbSlotSprite       = maxigin_initSprite( "sideBoardSlot.tga"       );
    sbSlotPickedSprite = maxigin_initSprite( "sideBoardSlotPicked.tga" );
    sbSlotRedrawSprite = maxigin_initSprite( "sideBoardSlotRedraw.tga" );

    maxigin_initMakeGlowSprite( sbSlotPickedSprite,
                                4,
                                2 );

    maxigin_initMakeGlowSprite( sbSlotRedrawSprite,
                                4,
                                2 );
    
    sbPointerActionHandle = inPointerActionHandle;

    for( i = 0;
         i < SIDE_BOARD_MAX_SLOTS;
         i ++ ) {
        
        sideBoard      [ i ] = noPiece;
        sbLift         [ i ] = 0;
        sbSmoothLift   [ i ] = 0;
        sbSlotPosX     [ i ] = inCenterX;
        sbSlotPosY     [ i ] = yPos;
        sbHighlightFade[ i ] = 0;

        yPos -= ySep;
        }


    REGISTER_ARRAY_MEM( sideBoard );
    REGISTER_ARRAY_MEM( sbLift );
    REGISTER_ARRAY_MEM( sbSmoothLift );
    REGISTER_VAL_MEM( sbNumSlots );
    REGISTER_VAL_MEM( sbLifting );
    REGISTER_VAL_MEM( sbDropping );
    REGISTER_VAL_MEM( sbPickedIndex );

    REGISTER_ARRAY_MEM( sbHighlightFade );

    REGISTER_VAL_MEM( sbRedrawShowing );

    REGISTER_VAL_MEM( sbHoldingController );

    REGISTER_VAL_MEM( sbOverSlot );
    }



void sideBoardRedraw( Deck *inDeck ) {
    
    int  i;
    
    sbNumSlots = sbBaseNumSlots + unlocksGetExtraSideboardSlots();

    if( sbNumSlots > SIDE_BOARD_MAX_SLOTS ) {
        sbNumSlots = SIDE_BOARD_MAX_SLOTS;
        }

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

    if( sbHoldingController ) {
        sbOverSlot = 0;
        sbHighlightFade[ 0 ] = 255;
        }
    else {
        sbOverSlot = -1;
        }
    }



/* swaps a piece onto the side board if anything on the side board is selected
   returns noPiece if the swap failed */
ChessPiece sideBoardSwap( ChessPiece  inNewPiece ) {

    ChessPiece  retVal  =  noPiece;

    if( sbPickedIndex < sbNumSlots
        &&
        sbPickedIndex >= 0 ) {

        retVal = sideBoard[ sbPickedIndex ];
        
        sideBoard[ sbPickedIndex ] = inNewPiece & CHESS_TYPE_MASK;
        }

    return retVal;
    }




ChessPiece sideBoardStep( int  inPieceLiftSound ) {

    /* fixme
       react to controller

       show piece info panel
    */

    /* fixme:
       also handle case where controller is used */
    
    int   pointerX;
    int   pointerY;
    int   i;
    int   r                    =  mingin_getStepsPerSecond();
    int   deltaFade            =  ( 20 * 60 ) / r;
    char  liftPhaseDone        =  0;
    char  controllerMovedSlot  =  0;

    if( unlocksIsViewerActive() ) {
        sbOverSlot = -1;
        }
    
    if( maxigin_getPointerLocation( &pointerX,
                                    &pointerY ) ) {
        sbOverSlot = -1;
        sbHoldingController = 0;
        
        for( i = 0;
             i < sbNumSlots;
             i ++ ) {

            ChessPiece  p  =  sideBoard[ i ];

            if( p != noPiece ) {

                if( getPixelOverPiece( p | CHESS_WHITE,
                                       sbSlotPosX[i],
                                       sbSlotPosY[i],
                                       pointerX,
                                       pointerY ) ) {

                    sbOverSlot = i;
                    sbHighlightFade[ i ] = 255;
                    break;
                    }
                }
            }
        }
    else if( sbHoldingController ) {

        int  navX;
        int  navY;
        
        navGetDir( 1,
                   &navX,
                   &navY );

        if( navY < 0 ) {
            sbOverSlot ++;
            if( sbOverSlot >= sbNumSlots ) {
                sbOverSlot = 0;
                }
            controllerMovedSlot = 1;
            sbHighlightFade[ sbOverSlot ] = 255;
            unlocksCancelViewer();
            }
        else if( navY > 0 ) {
            sbOverSlot --;
            if( sbOverSlot < 0 ) {
                sbOverSlot = sbNumSlots - 1;
                }
            controllerMovedSlot = 1;
            sbHighlightFade[ sbOverSlot ] = 255;
            unlocksCancelViewer();
            }
        else if( navX > 0 ) {
            /* moving back to board */
            sbOverSlot = -1;
            sbHoldingController = 0;
            unlocksCancelViewer();
            }
        }
    

    for( i = 0;
         i < sbNumSlots;
         i ++ ) {

        if( i != sbOverSlot
            &&
            sbHighlightFade[i] > 0 ) {

            int  newHighlight = sbHighlightFade[i] - deltaFade;

            if( newHighlight > 0 ) {
                sbHighlightFade[i] = (unsigned char)newHighlight;
                }
            else {
                sbHighlightFade[i] = 0;
                }
            }
        }


    liftPhaseDone = slotLiftStep( sbLifting,
                                  sbDropping,
                                  sbMaxLift,
                                  sbNumSlots,
                                  sbLift,
                                  sbSmoothLift,
                                  inPieceLiftSound );

    if( liftPhaseDone
        &&
        sbDropping ) {
        
        sbDropping = 0;
        }
    

    if( sbOverSlot == -1 ) {
        return noPiece;
        }

    if( ! sbActionDown
        &&
        maxigin_isButtonDown( shopPointerActionHandle ) ) {

        if( sideBoard[ sbOverSlot ] != noPiece ) {

            /* picking a piece to swap */
            if( sbOverSlot != sbPickedIndex ) {
                    
            
                sbPickedIndex = sbOverSlot;

                playBeepDownSound();
                }
            else {
                /* unpick */
                sbPickedIndex = -1;
                playBeepUpSound();
                }
            }
        sbActionDown = 1;
        }

    if( ! maxigin_isButtonDown( shopPointerActionHandle ) ) {
        sbActionDown = 0;
        }

    if( controllerMovedSlot ) {
        /* return noPiece for one step, to allow piece info panel
           to fade slightly, and so that game will play sound */
        return noPiece;
        }

        
    return sideBoard[ sbOverSlot ];
    }



void sideBoardDraw( void ) {

    int  i;

    for( i = sbNumSlots -  1;
         i >= 0;
         i -- ) {

        maxigin_drawResetColor();

        if( sbRedrawShowing ) {
            maxigin_drawSprite( sbSlotRedrawSprite,
                                    sbSlotPosX[i],
                                    sbSlotPosY[i] );
            }
        else {
            
            if( sbPickedIndex == i ) {
                maxigin_drawSprite( sbSlotPickedSprite,
                                    sbSlotPosX[i],
                                    sbSlotPosY[i] );
                }
            else {
                maxigin_drawSprite( sbSlotSprite,
                                    sbSlotPosX[i],
                                    sbSlotPosY[i] );
                }
            }
        
        if( sideBoard[i] != noPiece ) {
            
            drawPiece( sideBoard [i] | CHESS_WHITE,
                       sbSlotPosX[i],
                       sbSlotPosY[i] - sbSmoothLift[i] );
            
            if( sbHighlightFade[i] > 0 ) {
                drawPieceHighlight( sideBoard      [i] | CHESS_WHITE,
                                    sbSlotPosX     [i],
                                    sbSlotPosY     [i] - sbSmoothLift[i],
                                    sbHighlightFade[i] );
                }

            if( sbHoldingController
                &&
                sbOverSlot == i ) {
                
                maxigin_drawButtonHintSprite(
                    sbPointerActionHandle,
                    sbSlotPosX[i] - 10,
                    sbSlotPosY[i] );
                }
            }

        }
    
    }



char  sideBoardIsMouseOver( void ) {
    int  pointerX;
    int  pointerY;

    
    if( ! maxigin_getPointerLocation( &pointerX,
                                      &pointerY ) ) {
        /* pointer not available */
        return 0;
        }

    if( pointerX > sbSlotPosX[0] - 12
        &&
        pointerX < sbSlotPosX[0] + 12 ) {
        
        return 1;
        }
    
    return 0;
    }



void sideBoardShowRedraw( char  inShow ) {
    sbRedrawShowing = inShow;
    }


char sideBoardLift( void ) {

    int   i;
    
    sbLifting  = 1;
    sbDropping = 0;
    
    for( i = 0;
         i < sbNumSlots;
         i ++ ) {

        if( sbLift[ i ] < sbMaxLift ) {
            return 0;
            }
        }
    
    return 1;
    }



void sideBoardForceFullLift( void ) {

    int   i;
    
    sbLifting  = 1;
    sbDropping = 0;
    
    for( i = 0;
         i < sbNumSlots;
         i ++ ) {

        sbLift[ i ] = sbMaxLift;
        sbSmoothLift[ i ] = MAXIGIN_GAME_NATIVE_H;
        }
    }



char sideBoardUnlift( void ) {
    
    int   i;
    
    sbLifting  = 0;
    sbDropping = 1;
    
    for( i = 0;
         i < sbNumSlots;
         i ++ ) {

        if( sbLift[ i ] > 0 ) {
            return 0;
            }
        }
    
    return 1;
    }


void sideBoardClearPick( void ) {
    sbPickedIndex = -1;
    }



void sideBoardGrabController( void ) {
    sbHoldingController = 1;
    sbOverSlot = 0;
    sbHighlightFade[ 0 ] = 255;
    unlocksCancelViewer();
    }



char sideBoardStillHoldingController( void ) {
    return sbHoldingController;
    }



#endif



#endif
