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
               int  inCenterX,
               int  inCenterY );



/* refresh the shop, rolling new pieces from the internal shop decks,
   and incrementing prices */
void shopReroll( void );

/* resets the shop back to its starting state
   ( starting prices, fully shuffled decks )
*/
void shopReset( void );



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
static  unsigned char  shopSlotHighlightFade [ NUM_SHOP_SLOTS ];
static  char           shopActionDown                             =   0;


static  int            purchaseSound                              =  -1;

static  int            lang_shopTitle                             =  -1;
static  int            lang_sale                                  =  -1;

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

    for( i = 0;
         i < NUM_SHOP_SLOTS;
         i ++ ) {
        
        shopItems[ i ] = rarityRollPiece();

        shopSlotPrices[ i ] = shopPrices[ shopItems[ i ] ];


        if( roll( &shopOnSaleRoll ) ) {
            
            shopIsOnSale[ i ] = 1;
            }
        else {
            shopIsOnSale[ i ] = 0;
            }
        
        if( shopIsOnSale[ i ] ) {
            int  discount  =  shopDiscountPercent[ i ] * shopSlotPrices[ i ];

            discount /= 100;

            shopSlotPrices[ i ] -= discount;
            }
        }
    }





void shopInit( int  inPointerActionHandle,
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
    
    
    maxigin_randSeed( &shopRand,
                      mingin_getEntropySeed() );

    rollSetup( &shopOnSaleRoll,
               shopOnSaleOneIn,
               1,
               1 );

    shopPointerActionHandle = inPointerActionHandle;

    shopCenterX = inCenterX;
    shopCenterY = inCenterY;
    

    purchaseSound = maxigin_initSoundEffect( "purchase_sd_30.wav" );

    lang_shopTitle  = maxigin_initTranslationKey( "shopTitle" );
    lang_sale  = maxigin_initTranslationKey( "sale" );
    

    /* all have discount turned off, but potential 50 % discount for now */
    shopIsOnSale[ 0 ] = 0;
    shopDiscountPercent[ 0 ] = 50;

    for( i = 1;
         i < NUM_SHOP_SLOTS;
         i ++ ) {

        shopIsOnSale       [ i ] =  0;
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
                               /* fixme... need controller mapping for this */
                               -1 );
    
    doneButton = buttonInit( maxigin_initSprite( "doneButton.tga" ),
                             -1,
                             maxigin_initSprite( "doneButtonPressed.tga" ),
                             shopCenterX + 70,
                             shopCenterY + 70,
                             1,
                             shopPointerActionHandle,
                             /* fixme... need controller mapping for this */
                             -1 );

    REGISTER_VAL_MEM( shopRand );

    REGISTER_VAL_MEM( shopOnSaleRoll );

    REGISTER_ARRAY_MEM( shopSlotPrices );
    REGISTER_ARRAY_MEM( shopIsOnSale );
    
    REGISTER_ARRAY_MEM( shopItems );

    REGISTER_ARRAY_MEM( shopSlotLift );
    REGISTER_ARRAY_MEM( shopSlotSmoothLift );

    REGISTER_VAL_MEM( shopSlotsLifting );
    REGISTER_VAL_MEM( shopSlotsDropping );
    }



void shopReroll( void ) {
    shopInternalReroll();

    shopSelectedSlot = -1;
    shopResetHightlighFades();
    shopActionDown = 0;
    shoppingDone   = 0;

    costResetIncrement( shopRerollCost );
    costLevelIncrement( shopRerollCost );

    buttonReset( doneButton );
    }



void shopReset( void ) {

    shopInternalReroll();

    shopSelectedSlot = -1;
    shopActionDown   =  0;
    shoppingDone     =  0;

    costFullReset( shopRerollCost );

    buttonReset( doneButton );
    }



void shopDraw( void ) {

    int  i;

    maxigin_drawResetColor();

    maxigin_setLanguageFontIndex( 1 );
    
    maxigin_drawLangText( lang_shopTitle,
                          shopCenterX,
                          shopCenterY - 60,
                          MAXIGIN_CENTER );
    
    maxigin_setLanguageFontIndex( 0 );

    for( i = 0;
         i < NUM_SHOP_SLOTS;
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
            
    numberDraw( costGet( shopRerollCost ),
                    shopCenterX - 35,
                    shopCenterY + 50,
                    1 );
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
    int  pointerX;
    int  pointerY;
    int  i;
    int  r              =  mingin_getStepsPerSecond();
    int  deltaFade      =  ( 20 * 60 ) / r;
    int  liftPhaseDone  =  0;

    if( buttonIsNewPressed( doneButton ) ) {
        shoppingDone = 1;
        return noPiece;
        }

    if( buttonIsNewPressed( rerollButton ) ) {

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
                                  NUM_SHOP_SLOTS,
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
    

    
    
    if( ! maxigin_getPointerLocation( &pointerX,
                                      &pointerY ) ) {
        /* pointer not available */
        return noPiece;
        }
    

    shopSelectedSlot = -1;
    
    for( i = 0;
         i < NUM_SHOP_SLOTS;
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

    for( i = 0;
         i < NUM_SHOP_SLOTS;
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

    if( ! maxigin_isButtonDown( shopPointerActionHandle ) ) {
        shopActionDown = 0;
        }

    return shopItems[ shopSelectedSlot ];
    }



char isShoppingDone( void ) {
    return shoppingDone;
    }



#endif

#endif
