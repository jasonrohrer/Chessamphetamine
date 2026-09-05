/*
  Include in your C code wherever like so:

      #include "shop.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define SHOP_IMPLEMENTATION
      #include "shop.h"

*/

#ifndef SHOP_H_INCLUDED
#define SHOP_H_INCLUDED


void shopInit( int  inPointerActionHandle,
               int  inDynamicRerollButtonHandle,
               int  inDynamicDoneButtonHandle,
               int  inCenterX,
               int  inCenterY );



/* refresh the shop, rolling new pieces from the internal shop decks,
   and incrementing prices
   size of deck determines whether a new formation slot is offered
   for sale */
void shopReroll( Deck  *inPlayerDeck );


/* resets the shop back to its starting state
   ( starting prices, fully shuffled decks )
*/
void shopReset( Deck  *inPlayerDeck );



void shopDraw( void );


/* returns moused-over piece in shop (for display on external info panel)
   or noPiece if nothing moused over

*/
ChessPiece shopStep( Deck  *inPlayerDeck,
                     int    inPickFailedSound,
                     int    inPieceLiftSound );


char isShoppingDone( void );




#endif



#ifdef  SHOP_IMPLEMENTATION

#ifndef SHOP_IMPLEMENTATION_INCLUDED
#define SHOP_IMPLEMENTATION_INCLUDED




#include "deck.h"
#include "numbers.h"
#include "pieceSprites.h"

#include "memoryRegister.h"
#include "mingin.h"

#include "button.h"
#include "slotLift.h"
#include "cost.h"

#include "unlocks.h"



/* pawns are never sold in shop */
#define SHOP_PRICE_LIST( C, V )  \
    V( C, 0,   noPiece,      0   )    \
    V( C, 1,   pawn,         0   )    \
    V( C, 2,   bishop,       2   )    \
    V( C, 3,   knight,       2   )    \
    V( C, 4,   rook,         4   )    \
    V( C, 5,   queen,        7   )    \
    V( C, 6,   king,         0   )    \
    V( C, 7,   laserRook,    10  )    \
    V( C, 8,   laserPawn,    6   )    \
    V( C, 9,   doublingPawn, 6   )    \
    V( C, 10,  addingRook,   7   )    \
    V( C, 11,  rocket,       7   )

static  int  shopPrices[] = {
    MAKE_CHESS_ARRAY( SHOP_PRICE_LIST )
    };

CHECK_CHESS_ARRAY( shopPrices,
                   SHOP_PRICE_LIST );


/* one free deck, one paid deck with everything
   and two paid decks with more and more rarity */
#define                NUM_SHOP_SLOTS  6

static  int            shopBaseVisibleSlots                       =  5;
static  int            shopNumVisibleSlots                        =  5;

static  char           shopIsPermaSale       [ NUM_SHOP_SLOTS ];
static  char           shopIsOnSale          [ NUM_SHOP_SLOTS ];
static  int            shopDiscountPercent   [ NUM_SHOP_SLOTS ];
static  int            shopSlotPrices        [ NUM_SHOP_SLOTS ];
static  ChessPiece     shopItems             [ NUM_SHOP_SLOTS ];

static  int            shopSlotPosX          [ NUM_SHOP_SLOTS ];
static  int            shopSlotPosY          [ NUM_SHOP_SLOTS ];
static  int            shopSlotLift          [ NUM_SHOP_SLOTS ];
static  int            shopSlotSmoothLift    [ NUM_SHOP_SLOTS ];
static  char           shopSlotsLifting                           =  0;
static  char           shopSlotsDropping                          =  0;

static  int            shopSelectedSlot                           =  -1;
static  char           shopOverNewSpot                            =   0;

static  unsigned char  shopSlotHighlightFade [ NUM_SHOP_SLOTS ];
static  char           shopActionDown                             =   0;


static  int            purchaseSound                              =  -1;

static  int            lang_shopTitle                             =  -1;
static  int            lang_shopInstructA                         =  -1;
static  int            lang_shopInstructB                         =  -1;
static  int            lang_sale                                  =  -1;
static  int            lang_permanent                             =  -1;
static  int            lang_newSpotInA                            =  -1;
static  int            lang_newSpotInB                            =  -1;

static  char           shoppingDone                               =   0;

static  int            doneButton                                 =  -1;
static  int            rerollButton                               =  -1;

static  int            shopPointerActionHandle                    =  -1;

static  int            shopCenterX;
static  int            shopCenterY;

static  MaxiginRand    shopRand;
static  int            shopOnSaleOneIn                            =  10;
static  RollInfo       shopOnSaleRoll;

static  int            shopRerollCost;
static  int            newFormationSpotCost;

static  int            newFormSpotY;
static  int            newSpotBought                              =  0;
static  int            newSpotAvail                               =  0;
static  unsigned char  newSpotHighlightFade                       =  0;
static  int            numLeftForNewSpot                          =  0;

static  char           shopSlotPickedWithController               =  0;


static void shopResetHightlighFades( void ) {
    int  i;

    for( i = 0;
         i < NUM_SHOP_SLOTS;
         i ++ ) {

        shopSlotHighlightFade[ i ] = 0;
        }
    }



/* rerolls and updates prices */
static void shopInternalReroll( void ) {

    int  i;
    int  minNumSale;
    
    shopNumVisibleSlots = shopBaseVisibleSlots + unlocksGetExtraShopSlots();

    if( shopNumVisibleSlots > NUM_SHOP_SLOTS ) {
        shopNumVisibleSlots = NUM_SHOP_SLOTS;
        }

    minNumSale = unlocksGetMinNumSaleSlots();

    if( minNumSale > shopNumVisibleSlots ) {
        minNumSale = shopNumVisibleSlots;
        }

    for( i = 0;
         i < shopNumVisibleSlots;
         i ++ ) {
        if( i < minNumSale ) {
            shopIsOnSale   [ i ] = 1;
            shopIsPermaSale[ i ] = 1;
            }
        else {
            shopIsOnSale   [ i ] = 0;
            shopIsPermaSale[ i ] = 0;
            }
        }

    for( i = 0;
         i < shopNumVisibleSlots;
         i ++ ) {
        
        shopItems[ i ] = rarityRollPiece();

        shopSlotPrices[ i ] = shopPrices[ shopItems[ i ] ];

        /* don't roll if slot is already on sale, b/c we don't want
           to polute the variance reduction of the roll mechanism */
        if( ! shopIsOnSale[ i ] ) {
            if( roll( &shopOnSaleRoll ) ) {
                shopIsOnSale[ i ] = 1;
                }
            }
        }
    

    for( i = 0;
         i < shopNumVisibleSlots;
         i ++ ) {
        if( shopIsOnSale[ i ] ) {
            int  discount  =  shopDiscountPercent[ i ] * shopSlotPrices[ i ];

            discount /= 100;

            shopSlotPrices[ i ] -= discount;
            }
        }
    }




static void shopSetNewSpotAvail( Deck  *inPlayerDeck ) {
    newSpotAvail = 0;

    /* they start with a 15-piece deck and 2 spots
       Once they have an 18-piece deck, they can buy another spot
       Then they can buy another when they have a 24-piece deck */
    if( formationGetNumNonKingSpots() < deckGetSize( inPlayerDeck ) / 6  ) {
        newSpotAvail = 1;
        numLeftForNewSpot = 0;
        }
    else {
        numLeftForNewSpot =
            ( formationGetNumNonKingSpots() + 1 ) * 6
            - deckGetSize( inPlayerDeck );
        }
    }



void shopInit( int  inPointerActionHandle,
               int  inDynamicRerollButtonHandle,
               int  inDynamicDoneButtonHandle,
               int  inCenterX,
               int  inCenterY ) {
    
    int  i;
    int  hopSize       =  30;
    int  numStartHops  =  NUM_SHOP_SLOTS / 2;
    int  startHop      =  hopSize * numStartHops;
    int  curPos;


    /* reroll costs are 5, 6, 8, 11, 15, etc. */
    /* don't increase as levels go up */
    shopRerollCost = costInit( 5,
                               1,
                               -1,
                               -1,
                               1,
                               0,
                               -1,
                               -1,
                               0 );

    /* new slots cost 3, 4, 5, 6, 7, etc */
    newFormationSpotCost = costInit( 3,
                                     2,
                                     -1,
                                     -1,
                                     0,
                                     0,
                                     -1,
                                     -1,
                                     0 );
    
    
    maxigin_randSeed( &shopRand,
                      mingin_getEntropySeed() );

    rollSetup( &shopOnSaleRoll,
               shopOnSaleOneIn,
               1,
               1 );

    shopPointerActionHandle = inPointerActionHandle;

    shopCenterX = inCenterX;
    shopCenterY = inCenterY;

    newFormSpotY = shopCenterY + 95;

    purchaseSound = maxigin_initSoundEffect( "purchase_sd_30.wav" );

    lang_shopTitle      = maxigin_initTranslationKey( "shopTitle"      );
    lang_shopInstructA  = maxigin_initTranslationKey( "shopInstructA"  );
    lang_shopInstructB  = maxigin_initTranslationKey( "shopInstructB"  );
    lang_sale           = maxigin_initTranslationKey( "sale"           );
    lang_permanent      = maxigin_initTranslationKey( "permanent"      );
    lang_newSpotInA     = maxigin_initTranslationKey( "newSpotInA"     );
    lang_newSpotInB     = maxigin_initTranslationKey( "newSpotInB"     );
    

    /* all have discount turned off, but potential 50 % discount for now */
    shopIsOnSale[ 0 ] = 0;
    shopDiscountPercent[ 0 ] = 50;

    for( i = 1;
         i < NUM_SHOP_SLOTS;
         i ++ ) {

        shopIsOnSale       [ i ] =  0;
        shopIsPermaSale    [ i ] =  0;
        shopDiscountPercent[ i ] = 50;

        shopSlotLift       [ i ] =  0;
        shopSlotSmoothLift [ i ] =  0;
        }

    shopSlotsLifting = 0;
    shopSlotsDropping = 0;


    /* set up slot positions */
 
    
    if( ( NUM_SHOP_SLOTS % 2 ) == 0 ) {
        /* center between two middle slots */
        startHop -= hopSize / 2;
        }
    
    curPos  = - startHop;

    for( i = 0;
         i < NUM_SHOP_SLOTS;
         i ++ ) {

        shopSlotPosX[ i ] =  curPos;
        shopSlotPosY[ i ] =  0;

        curPos += hopSize;
        }
    

    shopInternalReroll();

    shopResetHightlighFades();


    rerollButton = buttonInit( maxigin_initSprite( "rerollButton.tga" ),
                               -1,
                               maxigin_initSprite( "rerollButtonPressed.tga" ),
                               shopCenterX,
                               shopCenterY + 50,
                               1,
                               shopPointerActionHandle,
                               inDynamicRerollButtonHandle );
    
    doneButton = buttonInit( maxigin_initSprite( "doneButton.tga" ),
                             -1,
                             maxigin_initSprite( "doneButtonPressed.tga" ),
                             shopCenterX + 70,
                             shopCenterY + 50,
                             1,
                             shopPointerActionHandle,
                             inDynamicDoneButtonHandle );

    REGISTER_VAL_MEM( shopRand );

    REGISTER_VAL_MEM( shopOnSaleRoll );

    REGISTER_ARRAY_MEM( shopSlotPrices );
    REGISTER_ARRAY_MEM( shopIsOnSale );
    REGISTER_ARRAY_MEM( shopIsPermaSale );
    
    REGISTER_ARRAY_MEM( shopItems );

    REGISTER_ARRAY_MEM( shopSlotLift );
    REGISTER_ARRAY_MEM( shopSlotSmoothLift );

    REGISTER_VAL_MEM( shopSlotsLifting );
    REGISTER_VAL_MEM( shopSlotsDropping );

    REGISTER_VAL_MEM( shoppingDone );

    REGISTER_VAL_MEM( newSpotBought );
    REGISTER_VAL_MEM( newSpotAvail );
    REGISTER_VAL_MEM( numLeftForNewSpot );

    REGISTER_VAL_MEM( newSpotHighlightFade );

    REGISTER_VAL_MEM( shopNumVisibleSlots );

    REGISTER_VAL_MEM( shopSelectedSlot );
    REGISTER_VAL_MEM( shopOverNewSpot );

    REGISTER_VAL_MEM( shopSlotPickedWithController );
    }



void shopReroll( Deck  *inPlayerDeck ) {
    shopInternalReroll();

    shopSelectedSlot = -1;
    shopOverNewSpot  =  0;
    
    shopSlotPickedWithController = 0;
    
    shopResetHightlighFades();
    shopActionDown = 0;
    shoppingDone   = 0;
    newSpotBought  = 0;

    shopSetNewSpotAvail( inPlayerDeck );

    newSpotHighlightFade = 0;

    costResetIncrement( shopRerollCost );
    costLevelIncrement( shopRerollCost );

    buttonReset( doneButton );
    buttonReset( rerollButton );
    }



void shopReset( Deck  *inPlayerDeck ) {

    shopInternalReroll();

    shopSelectedSlot = -1;
    shopActionDown   =  0;
    shoppingDone     =  0;
    newSpotBought    =  0;
    
    shopSlotPickedWithController = 0;

    shopSetNewSpotAvail( inPlayerDeck );
    
    
    newSpotHighlightFade = 0;

    costFullReset( shopRerollCost );
    costFullReset( newFormationSpotCost );

    buttonReset( doneButton );
    }



void shopDraw( void ) {

    int  i;

    maxigin_drawResetColor();

    maxigin_setLanguageFontIndex( 1 );

    /* skip shop title for now */
    if( 0 )
    maxigin_drawLangText( lang_shopTitle,
                          shopCenterX,
                          shopCenterY - 70,
                          MAXIGIN_CENTER );

    maxigin_drawLangText( lang_shopInstructA,
                          shopCenterX,
                          shopCenterY - 95,
                          MAXIGIN_CENTER );
    maxigin_drawLangText( lang_shopInstructB,
                          shopCenterX,
                          shopCenterY - 75,
                          MAXIGIN_CENTER );
    
    maxigin_setLanguageFontIndex( 0 );

    for( i = 0;
         i < shopNumVisibleSlots;
         i ++ ) {

        ChessPiece  p  =  shopItems[ i ];

        if( p != noPiece ) {

            int  pieceYBase    =  shopCenterY + shopSlotPosY[i];
            int  pieceYLifted  =  pieceYBase - shopSlotSmoothLift[i];
            
            drawPiece( p | CHESS_WHITE,
                       shopCenterX + shopSlotPosX[i],
                       pieceYLifted );

            if( shopSlotHighlightFade[i] > 0 ) {
                drawPieceHighlight( p | CHESS_WHITE,
                                    shopCenterX + shopSlotPosX[i],
                                    pieceYLifted,
                                    shopSlotHighlightFade[i] );
                }
            if( shopSelectedSlot == i
                &&
                shopSlotPickedWithController ) {

                maxigin_drawButtonHintSprite(
                    shopPointerActionHandle,
                    shopCenterX + shopSlotPosX[i] - 5,
                    pieceYLifted );
                }
            

            if( ! shopSlotsLifting ) {
                
                maxigin_drawResetColor();
            
                numberDrawCenter( shopSlotPrices[ i ],
                                  shopCenterX + shopSlotPosX[i],
                                  pieceYBase + 12,
                                  1 );

                if( shopIsOnSale[ i ] ) {

                    numberDrawCenter( shopPrices[p],
                                      shopCenterX + shopSlotPosX[i],
                                      pieceYBase + 22,
                                      1 );

                    maxigin_drawSetColor( 255,
                                          0,
                                          0,
                                          255 );
                    maxigin_setLanguageFontIndex( 1 );
    
                    maxigin_drawLangText( lang_sale,
                                          shopCenterX + shopSlotPosX[i],
                                          pieceYBase - 40,
                                          MAXIGIN_CENTER );

                    if( shopIsPermaSale[ i ] ) {
                        
                        int  aboveS;
                        int  belowS;
                        int  aboveP;
                        int  belowP;

                        maxigin_measureLangTextVertical( lang_sale,
                                                         &aboveS,
                                                         &belowS );
                        
                        maxigin_measureLangTextVertical( lang_permanent,
                                                         &aboveP,
                                                         &belowP );
                        
                        maxigin_drawLangText( lang_permanent,
                                              shopCenterX + shopSlotPosX[i],
                                              pieceYBase - 43 - aboveS - belowP,
                                              MAXIGIN_CENTER );
                        }
    
                    maxigin_setLanguageFontIndex( 0 );

                    numberDrawText( "\\",
                                    shopCenterX + shopSlotPosX[i],
                                    pieceYBase + 22,
                                    0,
                                    MAXIGIN_CENTER );
                    }
                }
            }
        }

    buttonDraw( rerollButton );
    buttonDraw( doneButton );

    maxigin_drawResetColor();
            
    numberDrawCenter( costGet( shopRerollCost ),
                    shopCenterX,
                    shopCenterY + 59,
                    1 );

    if( formationHasRoomForNewSpot()
        &&
        ! newSpotBought
        &&
        newSpotAvail ) {

        int  spotSprite  =  formationGetSpotSprite();

        maxigin_drawResetColor();

        maxigin_setLanguageFontIndex( 1 );
    
        maxigin_drawLangText( formationGetNewSpotLangHandle(),
                              shopCenterX,
                              newFormSpotY - 17,
                              MAXIGIN_CENTER );
    
        maxigin_setLanguageFontIndex( 0 ); 

        maxigin_drawSprite( spotSprite,
                            shopCenterX,
                            newFormSpotY );

        if( newSpotHighlightFade > 0 ) {

            maxigin_drawSetAlpha( newSpotHighlightFade );
            
            maxigin_drawSpriteGlowOnly( spotSprite,
                                        shopCenterX,
                                        newFormSpotY );
            maxigin_drawResetColor();
            }

        if( shopOverNewSpot
            &&
            shopSlotPickedWithController ) {

            maxigin_drawButtonHintSprite(
                shopPointerActionHandle,
                shopCenterX - 13,
                newFormSpotY + 5 );
            }

        numberDrawCenter( costGet( newFormationSpotCost ),
                          shopCenterX,
                          newFormSpotY + 17,
                          1 );
        }
    else if( formationHasRoomForNewSpot()
             &&
             ! newSpotBought
             &&
             ! newSpotAvail ) {

        maxigin_drawResetColor();
        
        maxigin_setLanguageFontIndex( 1 );
    
        maxigin_drawLangText( lang_newSpotInA,
                              shopCenterX - 57,
                              newFormSpotY - 17,
                              MAXIGIN_RIGHT );

        maxigin_drawSetColor( 255, 255, 0, 255 );
        
        numberDrawCenter( numLeftForNewSpot,
                          shopCenterX - 50,
                          newFormSpotY - 17,
                          1 );
        
        maxigin_drawResetColor();
        
        maxigin_drawLangText( lang_newSpotInB,
                              shopCenterX - 43,
                              newFormSpotY - 17,
                              MAXIGIN_LEFT );
    
        maxigin_setLanguageFontIndex( 0 );
        }
    
    }



ChessPiece shopStep( Deck  *inPlayerDeck,
                     int  inPickFailedSound,
                     int  inPieceLiftSound ) {

    /* fixme
       react to mouse and controller

       show piece info panel
    */

    /* fixme:
       also handle case where controller is used */
    int   pointerX;
    int   pointerY;
    int   i;
    int   r                    =  mingin_getStepsPerSecond();
    int   deltaFade            =  ( 20 * 60 ) / r;
    int   liftPhaseDone        =  0;
    char  controllerMovedSlot  =  0;
    
    
    if( buttonIsNewPressed( doneButton ) ) {
        unlocksCancelViewer();
        shoppingDone = 1;
        return noPiece;
        }

    if( buttonIsNewPressed( rerollButton ) ) {
        unlocksCancelViewer();

        if( moneyGetTotal() < costGet( shopRerollCost ) ) {
            /* fail */
            maxigin_playSoundEffect( inPickFailedSound,
                                     256 );
            }
        else {
            moneyAdd( - costGet( shopRerollCost ) );

            shopSlotsLifting = 1;

            costIncrement( shopRerollCost );
            }
        }



    liftPhaseDone = slotLiftStep( shopSlotsLifting,
                                  shopSlotsDropping,
                                  100,
                                  shopNumVisibleSlots,
                                  shopSlotLift,
                                  shopSlotSmoothLift,
                                  inPieceLiftSound );

    if( liftPhaseDone ) {
        
        if( shopSlotsLifting ) {

            /* reroll while they are lifted off screen */
            shopInternalReroll();
            
            shopSlotsLifting = 0;
            shopSlotsDropping = 1;
            }
        else if( shopSlotsDropping ) {
            shopSlotsDropping = 0;

            buttonReset( rerollButton );
            }
        }
    

    if( unlocksIsViewerActive() ) {
        shopSelectedSlot = -1;
        shopOverNewSpot  =  0;
        }
    
    
    if( maxigin_getPointerLocation( &pointerX,
                                    &pointerY ) ) {
        shopSlotPickedWithController = 0;
        
        shopSelectedSlot = -1;
        shopOverNewSpot  =  0;
    
        for( i = 0;
             i < shopNumVisibleSlots;
             i ++ ) {

            ChessPiece  p  =  shopItems[ i ];

            if( p != noPiece ) {

                if( getPixelOverPiece( p | CHESS_WHITE,
                                       shopCenterX + shopSlotPosX[i],
                                       shopCenterY + shopSlotPosY[i],
                                       pointerX,
                                       pointerY ) ) {

                    shopSelectedSlot = i;
                    shopSlotHighlightFade[ i ] = 255;
                    break;
                    }
                }
            }

        if( formationHasRoomForNewSpot()
            &&
            ! newSpotBought
            &&
            newSpotAvail ) {

            int  spotR  =  BOARD_SQUARE_SIZE / 2;

            if( pointerX > shopCenterX - spotR
                &&
                pointerX < shopCenterX + spotR
                &&
                pointerY > newFormSpotY - spotR
                &&
                pointerY < newFormSpotY + spotR ) {

                shopOverNewSpot = 1;

                if( newSpotHighlightFade < 255 ) {
                    maxigin_playSoundEffect( inPieceLiftSound,
                                             256 );
                    }
                newSpotHighlightFade = 255;
                }
            }
        }
    else {
        /* controller can pan through slots and potentially new spot beneath */
        int  dirX;
        int  dirY;

        shopSlotPickedWithController = 1;
        
        if( shopSelectedSlot != -1 ) {

            navGetDir( 0,
                       &dirX,
                       &dirY );

            if( dirY == 1
                &&
                formationHasRoomForNewSpot()
                &&
                ! newSpotBought
                &&
                newSpotAvail ) {
                
                /* down to new spot */
                shopSelectedSlot = -1;
                shopOverNewSpot = 1;

                unlocksCancelViewer();
            
                if( newSpotHighlightFade < 255 ) {
                    maxigin_playSoundEffect( inPieceLiftSound,
                                             256 );
                    }
                newSpotHighlightFade = 255;
                }
            else if( dirX != 0 ) {

                /* left or right in shop row */

                int  start  = shopSelectedSlot;

                shopSelectedSlot += dirX;
                if( shopSelectedSlot < 0 ) {
                    shopSelectedSlot = shopNumVisibleSlots - 1;
                    }
                else if( shopSelectedSlot >= shopNumVisibleSlots ) {
                    
                    shopSelectedSlot = 0;
                    }
                while( shopSelectedSlot != start
                       &&
                       shopItems[ shopSelectedSlot ] == noPiece ) {
                    
                    shopSelectedSlot += dirX;
                    if( shopSelectedSlot < 0 ) {
                        shopSelectedSlot = shopNumVisibleSlots - 1;
                        }
                    else if( shopSelectedSlot >= shopNumVisibleSlots ) {
                        shopSelectedSlot = 0;
                        }
                    }
                if( shopItems[ shopSelectedSlot ] == noPiece ) {
                    shopSelectedSlot = -1;
                    }
                else {
                    shopSlotHighlightFade[ shopSelectedSlot ] = 255;
                    }

                if( shopSelectedSlot != start ) {
                    controllerMovedSlot = 1;
                    }
                
                shopOverNewSpot = 0;
                unlocksCancelViewer();
                }
            }
        else if( shopOverNewSpot ) {
            navGetDir( 1,
                       &dirX,
                       &dirY );

            if( dirX != 0
                ||
                dirY < 0 ) {

                /* up back to shop row */
                shopSelectedSlot = -1;
                
                for( i = 0;
                     i < shopNumVisibleSlots;
                     i ++ ) {
                    if( shopItems[ i ] != noPiece ) {
                        shopSelectedSlot = i;
                        shopSlotHighlightFade[ i ] = 255;
                        break;
                        }
                    }
                if( shopSelectedSlot != -1 ) {
                    shopOverNewSpot = 0;
                    unlocksCancelViewer();
                    }
                }
            }
        else if( shopSelectedSlot == -1
                 &&
                 ! shopOverNewSpot ) {
            navGetDir( 0,
                       &dirX,
                       &dirY );

            if( dirY == 1
                &&
                formationHasRoomForNewSpot()
                &&
                ! newSpotBought
                &&
                newSpotAvail ) {
                
                /* down to new spot */
                shopSelectedSlot = -1;
                shopOverNewSpot = 1;

                unlocksCancelViewer();
            
                if( newSpotHighlightFade < 255 ) {
                    maxigin_playSoundEffect( inPieceLiftSound,
                                             256 );
                    }
                newSpotHighlightFade = 255;
                }
            else if( dirX == 1
                     ||
                     dirY == -1 ) {
                shopSelectedSlot = 0;
                while( shopSelectedSlot < shopNumVisibleSlots
                       &&
                       shopItems[ shopSelectedSlot ] == noPiece ) {
                    shopSelectedSlot ++;
                    }
                if( shopSelectedSlot >= shopNumVisibleSlots ) {
                    shopSelectedSlot = -1;
                    }
                else {
                    shopSlotHighlightFade[ shopSelectedSlot ] = 255;
                    }
                unlocksCancelViewer();
                }
            else if( dirX == -1 ) {
                shopSelectedSlot = shopNumVisibleSlots - 1;
                
                while( shopSelectedSlot >= 0
                       &&
                       shopItems[ shopSelectedSlot ] == noPiece ) {
                    shopSelectedSlot --;
                    }
                if( shopSelectedSlot < 0 ) {
                    shopSelectedSlot = -1;
                    }
                else {
                    shopSlotHighlightFade[ shopSelectedSlot ] = 255;
                    }
                unlocksCancelViewer();
                }
            
            }
        }
    

    
    if( ! shopOverNewSpot ) {
        int  newHighlight;

        newHighlight = newSpotHighlightFade - deltaFade;

        if( newHighlight > 0 ) {
            newSpotHighlightFade = (unsigned char)newHighlight;
            }
        else {
            newSpotHighlightFade = 0;
            }
        }
    
        

    for( i = 0;
         i < shopNumVisibleSlots;
         i ++ ) {

        if( i != shopSelectedSlot
            &&
            shopSlotHighlightFade[i] > 0 ) {

            int  newHighlight = shopSlotHighlightFade[i] - deltaFade;

            if( newHighlight > 0 ) {
                shopSlotHighlightFade[i] = (unsigned char)newHighlight;
                }
            else {
                shopSlotHighlightFade[i] = 0;
                }
            }
        }

    
    if( ! maxigin_isButtonDown( shopPointerActionHandle ) ) {
        shopActionDown = 0;
        }

    
    if( ! shopActionDown
        &&
        shopOverNewSpot
        &&
        maxigin_isButtonDown( shopPointerActionHandle ) ) {
        
        if( costGet( newFormationSpotCost )
            <= moneyGetTotal() ) {

            moneyAdd( - costGet( newFormationSpotCost ) );

            formationAddNewSpot();

            costIncrement( newFormationSpotCost );

            newSpotBought = 1;

            maxigin_playSoundEffect( purchaseSound,
                                     256 );
            }
        else {
            /* can't afford */
            maxigin_playSoundEffect( inPickFailedSound,
                                     256 );
            }
        shopActionDown = 1;
        }

    if( shopSelectedSlot == -1 ) {
        return noPiece;
        }

    if( ! shopActionDown
        &&
        maxigin_isButtonDown( shopPointerActionHandle ) ) {

        if( shopItems[ shopSelectedSlot ] != noPiece ) {

            /* picking a piece to buy */

            if( shopSlotPrices[ shopSelectedSlot ]
                <=
                moneyGetTotal() ) {

                /* can afford */

                moneyAdd( - shopSlotPrices[ shopSelectedSlot ] );

                deckAddPiece( inPlayerDeck,
                              shopItems[ shopSelectedSlot ] );

                deckReturnAll   ( inPlayerDeck );
                deckReshuffleAll( inPlayerDeck );

                /* deck grew... does this make new slot purchase avail? */
                shopSetNewSpotAvail( inPlayerDeck );

                shopItems[ shopSelectedSlot ] = noPiece;

                maxigin_playSoundEffect( purchaseSound,
                                         256 );
                }
            else {
                /* can't afford */

                maxigin_playSoundEffect( inPickFailedSound,
                                         256 );
                }
            }
        shopActionDown = 1;
        }

    if( controllerMovedSlot ) {
        /* return noPiece for one step, to allow piece info panel
           to fade slightly, and so that game will play sound */
        return noPiece;
        }
    
    return shopItems[ shopSelectedSlot ];
    }



char isShoppingDone( void ) {
    return shoppingDone;
    }



#endif

#endif
