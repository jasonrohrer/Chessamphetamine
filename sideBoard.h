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
                    int  inActionHandle,
                    int  inCenterX,
                    int  inBottomSlotY );


/* returns 0 if side board contains same pieces that are available in
   the deck (i.e. redrawing is a waste of money) */
char sideBoardIsRedrawHelpful( void );


void sideBoardRedraw( void );


/* returns pieces to player deck and fills sideboard with noPiece */
void sideBoardReturnPieces( void );


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


void sideBoardResetCost( void );


int sideBoardGetPlacementCost( void );

int sideBoardGetNextPlacementCost( void );




/* returns piece being moused over */
ChessPiece sideBoardStep( int             inPieceLiftSound,
                          int             inPickFailedSound,
                          char            inBlockPurchase,
                          ChessPiece     *outPickedPiece,
                          unsigned char  *outOverPieceFade );


void sideBoardDraw( void );


char sideBoardIsMouseOver( void );

void sideBoardClearPick( void );


void sideBoardGrabController( void );

void sideBoardDropController( void );

char sideBoardStillHoldingController( void );


int sideBoardGetNumSlots( void );


int sideBoardGetExtraDeploymentCost( ChessPiece  inPiece );



#endif



#ifdef  SIDE_BOARD_IMPLEMENTATION

#ifndef SIDE_BOARD_IMPLEMENTATION_INCLUDED
#define SIDE_BOARD_IMPLEMENTATION_INCLUDED


#include "unlocks.h"

#include "cost.h"

#include "money.h"

#include "playerDeck.h"

#include "nav.h"

#include "slotLift.h"



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
static  int            sbActionHandle         =  -1;

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
static  int            sbPrevSlot             =   0;

static  int            sbPlacePieceCost       =  -1;
static  int            sbNumPlaced            =   0;
static  char           sbPurchaseBlocked      =   0;



#define PIECE_DEPLOYMENT_COST_LIST( C, V )   \
    V( C, 0,   noPiece,      0   ) \
    V( C, 1,   pawn,         2   ) \
    V( C, 2,   bishop,       2   ) \
    V( C, 3,   knight,       1   ) \
    V( C, 4,   rook,         5   ) \
    V( C, 5,   queen,        6   ) \
    V( C, 6,   king,         0   ) \
    V( C, 7,   laserRook,    10  ) \
    V( C, 8,   laserPawn,    3   ) \
    V( C, 9,   doublingPawn, 2   ) \
    V( C, 10,  addingRook,   6   ) \
    V( C, 11,  rocket,       2   )

static int pieceDeploymentCost[] = {
    MAKE_CHESS_ARRAY( PIECE_DEPLOYMENT_COST_LIST )
    };

CHECK_CHESS_ARRAY( pieceDeploymentCost,
                   PIECE_DEPLOYMENT_COST_LIST );


/* These functions create a hybrid cost that starts at 0 and goes up like
   this:

   0, 2, 3, 3, 4, 4, 4, 5, 5, 5, 5, 6, ...
*/

static int getNextPlacementCost( void ) {

    /* temporarily disabled entirely */
    return 0;
    
    if( sbNumPlaced == 0 ) {
        return 0;
        }
    else {
        return costGet( sbPlacePieceCost );
        }
    }



static int getNextNextPlacementCost( void ) {
    
    /* temporarily disabled entirely */
    return 0;
    
    if( sbNumPlaced == 0 ) {
        return costGet( sbPlacePieceCost );
        }
    else {
        return costIncrementPeek( sbPlacePieceCost );
        }
    }



static void resetPlacementCost( void ) {

    costResetIncrement( sbPlacePieceCost );
    sbNumPlaced = 0;
    }



static void incrementPlacementCost( void ) {
    if( sbNumPlaced > 0 ) {
        costIncrement( sbPlacePieceCost );
        }
        
    sbNumPlaced ++;
    }


void sideBoardInit( int  inPointerActionHandle,
                    int  inActionHandle,
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
    sbActionHandle        = inActionHandle;

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

    /* part of hybrid cost provided by functions above,
       1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5, ....   */
    sbPlacePieceCost = costPlateauInit( 1,
                                        1,
                                        1,
                                        0 );

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
    REGISTER_VAL_MEM( sbPrevSlot );

    REGISTER_VAL_MEM( sbPurchaseBlocked );

    REGISTER_VAL_MEM( sbNumPlaced );
    }



char sideBoardIsRedrawHelpful( void ) {

    ChessPiece   p  =  sideBoard[ 0 ];
    int          i;
    Deck        *d;

    if( playerDeckGetReadyCount() == 0 ) {
        return 0;
        }
    
    if( p == noPiece ) {
        return 1;
        }

    for( i = 1;
         i < sbNumSlots;
         i   ++ ) {

        if( sideBoard[ i ] != p ) {
            return 1;
            }
        }

    /* got to here:  side board contains identical pieces */

    /* does deck contain all the same piece, in ready pieces */

    d = playerDeckGetDrawDeck();

    for( i = 0;
         i < d->numPieces;
         i   ++ ) {
        
        if( d->pieces[ i ] != p ) {
            /* found some piece in draw deck that isn't same */
            return 1;
            }
        }
    
    return 0;
    }



void sideBoardRedraw( void ) {
    
    int  i;
    
    sbNumSlots = sbBaseNumSlots + unlocksGetExtraSideboardSlots();

    if( sbNumSlots > SIDE_BOARD_MAX_SLOTS ) {
        sbNumSlots = SIDE_BOARD_MAX_SLOTS;
        }

    for( i = 0;
         i < sbNumSlots;
         i ++ ) {

        if( sideBoard[ i ] != noPiece ) {
            playerDeckReturnPieceUnplayed( sideBoard[ i ] );
            }

        sideBoard[i] = playerDeckDraw();
        }

    sbPickedIndex = -1;

    if( sbHoldingController ) {
        sbHighlightFade[ 0 ] = 255;
        }
    else {
        sbOverSlot = -1;
        }
    }



void sideBoardReturnPieces( void ) {

    int  i;
    
    sbNumSlots = sbBaseNumSlots + unlocksGetExtraSideboardSlots();

    if( sbNumSlots > SIDE_BOARD_MAX_SLOTS ) {
        sbNumSlots = SIDE_BOARD_MAX_SLOTS;
        }

    for( i = 0;
         i < sbNumSlots;
         i ++ ) {

        if( sideBoard[ i ] != noPiece ) {
            playerDeckReturnPieceUnplayed( sideBoard[ i ] );
            }
        
        sideBoard[ i ] = noPiece;
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

    sbPickedIndex = -1;

    return retVal;
    }


static void sbForceFullLiftOneSpot( int  inSpotIndex ) {
    sbLift      [ inSpotIndex ] = sbMaxLift;
    sbSmoothLift[ inSpotIndex ] = MAXIGIN_GAME_NATIVE_H;
    }



ChessPiece sideBoardStep( int             inPieceLiftSound,
                          int             inPickFailedSound,
                          char            inBlockPurchase,
                          ChessPiece     *outPickedPiece,
                          unsigned char  *outOverPieceFade ) {
    
    int            pointerX;
    int            pointerY;
    int            i;
    int            r                    =  mingin_getStepsPerSecond();
    int            deltaFade            =  ( 20 * 60 ) / r;
    char           liftPhaseDone        =  0;
    char           controllerMovedSlot  =  0;
    unsigned char  maxFade              =  0;

    sbPurchaseBlocked = inBlockPurchase;

    if( unlocksIsViewerActive() ) {
        if( sbOverSlot != -1 ) {
            sbPrevSlot = sbOverSlot;
            }
        sbOverSlot = -1;
        }
    
    if( maxigin_getActivePointerLocation( &pointerX,
                                          &pointerY ) ) {
        if( sbOverSlot != -1 ) {
            sbPrevSlot = sbOverSlot;
            }
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
                    sbPrevSlot = i;

                    if( sbLift[ sbOverSlot ] == 0 ) {
                        sbHighlightFade[ sbOverSlot ] = 255;
                        }
                    else {
                        sbHighlightFade[ sbOverSlot ] = 0;
                        }
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
            sbPrevSlot = sbOverSlot;
            
            controllerMovedSlot = 1;
            sbHighlightFade[ sbOverSlot ] = 255;
            unlocksCancelViewer();
            }
        else if( navY > 0 ) {
            sbOverSlot --;
            if( sbOverSlot < 0 ) {
                sbOverSlot = sbNumSlots - 1;
                }
            sbPrevSlot = sbOverSlot;
            
            controllerMovedSlot = 1;
            if( sbLift[ sbOverSlot ] == 0 ) {
                sbHighlightFade[ sbOverSlot ] = 255;
                }
            else {
                sbHighlightFade[ sbOverSlot ] = 0;
                }
            unlocksCancelViewer();
            }
        else if( navX > 0 ) {
            /* moving back to board */
            if( sbOverSlot != -1 ) {
                sbPrevSlot = sbOverSlot;
                } 
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

        if( sbHighlightFade[i] > maxFade ) {
            maxFade = sbHighlightFade[i];
            }
        }

    *outOverPieceFade = maxFade;


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

    *outPickedPiece = noPiece;

    if( sbOverSlot == -1 ) {
        return noPiece;
        }

    if( ! sbActionDown
        &&
        ( maxigin_isButtonDown( sbPointerActionHandle )
          ||
          maxigin_isButtonDown( sbActionHandle ) ) ) {

        if( sideBoard[ sbOverSlot ] != noPiece ) {

            if( inBlockPurchase ) {

                /* purchase blocked, return picked piece without acting
                   on it */
                *outPickedPiece = sideBoard[ sbOverSlot ];

                sbPickedIndex = sbOverSlot;
                }
            else {
                /* treat click/action as purchase */

                int  totalCost =
                     getNextPlacementCost() +
                     sideBoardGetExtraDeploymentCost( sideBoard[ sbOverSlot ] );
                

                if( moneyGetTotal() < totalCost ) {
                    /* can't afford */
                
                    maxigin_playSoundEffect( inPickFailedSound,
                                             256 );
                    }
                else {
                    moneyAdd( - totalCost );
                    incrementPlacementCost();
                
                    *outPickedPiece = sideBoard[ sbOverSlot ];

                    sideBoard[ sbOverSlot ] = playerDeckDraw();

            
            

                    if( sideBoard[ sbOverSlot ] != noPiece ) {
                        sbForceFullLiftOneSpot( sbOverSlot );
                        sbDropping = 1;
                        }

                    playBeepUpSound();

                    for( i = 0;
                         i < sbNumSlots;
                         i ++ ) {

                        sbHighlightFade[i] = 0;
                        }
                    *outOverPieceFade = 0;
                    }
                }
            }
        sbActionDown = 1;
        }

    if( ! maxigin_isButtonDown( sbPointerActionHandle )
        &&
        ! maxigin_isButtonDown( sbActionHandle ) ) {
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

            /* no longer draw special highlight of picked slot */
            if( 0
                &&
                sbPickedIndex == i ) {
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
                
                if( ! sbPurchaseBlocked ) {

                    int  deployCost =
                        sideBoardGetExtraDeploymentCost( sideBoard[i] );

                    moneyDrawCoin( sbSlotPosX[i] + 16,
                                   sbSlotPosY[i] - 8,
                                   sbHighlightFade[i] / 4 );
                    
                    colorsApplyMoneyColor();
                    
                    maxigin_drawSetAlpha( sbHighlightFade[i] );

                    numberDrawCenter( deployCost,
                                      sbSlotPosX[i] + 16,
                                      sbSlotPosY[i],
                                      1 );
                    }

                }

            if( sbHoldingController
                &&
                sbOverSlot == i ) {
                
                maxigin_drawButtonHintSprite(
                    sbActionHandle,
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



void sideBoardResetCost( void ) {
    resetPlacementCost();
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

        sbForceFullLiftOneSpot( i );
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
    
    /* always jump to previous slot */
    sbOverSlot = sbPrevSlot;
    
    sbHighlightFade[ sbOverSlot ] = 255;
    unlocksCancelViewer();
    }



void sideBoardDropController( void ) {
    sbHoldingController = 0;
    sbOverSlot = -1;
    }



char sideBoardStillHoldingController( void ) {
    return sbHoldingController;
    }



int sideBoardGetNumSlots( void ) {
    return sbNumSlots;
    }



int sideBoardGetPlacementCost( void ) {

    return getNextPlacementCost();
    }



int sideBoardGetNextPlacementCost( void ) {

    return getNextNextPlacementCost();
    }



int sideBoardGetExtraDeploymentCost( ChessPiece  inPiece ) {
    return pieceDeploymentCost[ inPiece ];
    }



#endif



#endif
