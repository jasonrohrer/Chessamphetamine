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
               int  inActionHandle,
               int  inDynamicRerollButtonHandle,
               int  inDynamicDoneButtonHandle,
               int  inCenterX,
               int  inCenterY );



/* refresh the shop, rolling new pieces into the slots,
   and incrementing prices */
void shopReroll( void );

void shopLevelIncrement( void );



/* resets the shop back to its starting state
   ( starting prices, fully shuffled decks )
*/
void shopReset( void );



void shopDraw( void );


/* returns moused-over piece in shop (for display on external info panel)
   or noPiece if nothing moused over

*/
ChessPiece shopStep( int    inPickFailedSound,
                     int    inPieceLiftSound );


char isShoppingDone( void );




#endif



#ifdef  SHOP_IMPLEMENTATION

#ifndef SHOP_IMPLEMENTATION_INCLUDED
#define SHOP_IMPLEMENTATION_INCLUDED


#include "playerDeck.h"
#include "numbers.h"
#include "pieceSprites.h"

#include "memoryRegister.h"
#include "mingin.h"

#include "button.h"
#include "slotLift.h"
#include "cost.h"

#include "unlocks.h"
#include "colors.h"


/* pawns are never sold in shop */
#define SHOP_PRICE_LIST( C, V )  \
    V( C, 0,   noPiece,      0   )    \
    V( C, 1,   pawn,         1   )    \
    V( C, 2,   bishop,       1   )    \
    V( C, 3,   knight,       3   )    \
    V( C, 4,   rook,         5   )    \
    V( C, 5,   queen,        0   )    \
    V( C, 6,   king,         0   )    \
    V( C, 7,   laserRook,    30  )    \
    V( C, 8,   laserPawn,    8   )    \
    V( C, 9,   doublingPawn, 8   )    \
    V( C, 10,  addingRook,   14  )    \
    V( C, 11,  rocket,       8   )

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
static  char           shopSlotLocked        [ NUM_SHOP_SLOTS ];
static  int            shopLockOver                               =  -1;
static  char           shopAllLocked                              =   0;

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
static  int            lockSound                                  =  -1;

static  int            lang_shopTitle                             =  -1;
static  int            lang_shopInstructA                         =  -1;
static  int            lang_shopInstructB                         =  -1;
static  int            lang_sale                                  =  -1;
static  int            lang_permanent                             =  -1;


static  char           shoppingDone                               =   0;

static  int            doneButton                                 =  -1;
static  int            rerollButton                               =  -1;

static  int            shopPointerActionHandle                    =  -1;
static  int            shopActionHandle                           =  -1;

static  int            shopCenterX;
static  int            shopCenterY;

static  MaxiginRand    shopRand;
static  int            shopOnSaleOneIn                            =  10;
static  RollInfo       shopOnSaleRoll;

static  int            shopRerollCost;


static  char           shopSlotPickedWithController               =  0;

/* show things that are at most double what the player currently has to spend */
static  int            shopMaxBudgetPercentToShow                 =  200;

static  int            shopUnlockedSprite                         =  -1;
static  int            shopUnlockedClickMaskSprite                =  -1;
static  int            shopLockedSprite                           =  -1;
static  int            shopUnlockedYOffset                        =  35;


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

    int  skipListSize  =  0;
    int  maxPrice;
    
    static  ChessPiece  skipList[ NUM_CHESS_PIECES ];


    maxPrice = ( moneyGetTotal() * shopMaxBudgetPercentToShow ) / 100;

    /* if they have very little money, still show them the bare basics */
    if( maxPrice < 10 ) {
        maxPrice = 10;
        }
    
    
    for( i = 0;
         i < NUM_CHESS_PIECES;
         i ++ ) {
        if( shopPrices[ i ] > maxPrice ) {

            skipList[ skipListSize ] = (ChessPiece)i;
            skipListSize ++;
            }
        }

    raritySetSkipList( skipListSize,
                       skipList );
    
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
        
        if( shopSlotLocked[ i ] ) {
            continue;
            }
        
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

        char  sameAsOther  =  1;

        if( shopSlotLocked[ i ] ) {
            continue;
            }

        /* make sure each shop item is unique */
        while( sameAsOther ) {

            int  o;
        
            shopItems[ i ] = rarityRollPiece();

            sameAsOther = 0;

            /* disable prevention of duplicate pieces in shop */
            if( 0 )
            for( o = 0;
                 o < i;
                 o   ++ ) {

                if( shopItems[ o ] == shopItems[ i ] ) {
                    sameAsOther = 1;
                    break;
                    }
                }
            }
        

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

        if( shopSlotLocked[ i ] ) {
            continue;
            }
        
        if( shopIsOnSale[ i ] ) {
            int  discount  =  shopDiscountPercent[ i ] * shopSlotPrices[ i ];

            discount /= 100;

            if( discount > 0 ) {
                shopSlotPrices[ i ] -= discount;
                }
            else {
                /* price already too low to discount, disable
                   sale status to avoid confusion */
                shopIsOnSale[ i ] = 0;
                }
            }
        }
    }



void shopInit( int  inPointerActionHandle,
               int  inActionHandle,
               int  inDynamicRerollButtonHandle,
               int  inDynamicDoneButtonHandle,
               int  inCenterX,
               int  inCenterY ) {
    
    int  i;
    int  hopSize       =  30;
    int  numStartHops  =  NUM_SHOP_SLOTS / 2;
    int  startHop      =  hopSize * numStartHops;
    int  curPos;

    
    /* reroll costs are 1, 2, 4, 7, etc. */
    /* increase every other level too */
    shopRerollCost = costInit( 1,
                               1,
                               -1,
                               -1,
                               1,
                               0,
                               -1,
                               3,  /* base cost goes up by 1 every 3 levels */
                               0 );
    
    
    maxigin_randSeed( &shopRand,
                      mingin_getEntropySeed() );

    rollSetup( &shopOnSaleRoll,
               shopOnSaleOneIn,
               1,
               1 );

    shopPointerActionHandle = inPointerActionHandle;
    shopActionHandle        = inActionHandle;
    
    shopCenterX = inCenterX;
    shopCenterY = inCenterY;


    purchaseSound = maxigin_initSoundEffect( "purchase_sd_30.wav" );
    lockSound     = maxigin_initSoundEffect( "lock_sd_6.wav" );

    lang_shopTitle          = maxigin_initTranslationKey( "shopTitle"          );
    lang_shopInstructA      = maxigin_initTranslationKey( "shopInstructA"      );
    lang_shopInstructB      = maxigin_initTranslationKey( "shopInstructB"      );
    lang_sale               = maxigin_initTranslationKey( "sale"               );
    lang_permanent          = maxigin_initTranslationKey( "permanent"          );
    
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

        shopSlotLocked     [ i ] =  0;
        }

    shopSlotsLifting = 0;
    shopSlotsDropping = 0;
    shopLockOver      = -1;


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
                               shopCenterY + 60,
                               1,
                               shopPointerActionHandle,
                               inDynamicRerollButtonHandle,
                               -1 );
    
    doneButton = buttonInit( maxigin_initSprite( "doneButton.tga" ),
                             -1,
                             maxigin_initSprite( "doneButtonPressed.tga" ),
                             shopCenterX + 70,
                             shopCenterY + 60,
                             1,
                             shopPointerActionHandle,
                             inDynamicDoneButtonHandle,
                             -1 );


    shopUnlockedSprite          = maxigin_initSprite( "unlocked.tga"          );
    shopUnlockedClickMaskSprite = maxigin_initSprite( "unlockedClickMask.tga" );
    

    shopLockedSprite = maxigin_initSprite( "locked.tga" );

    /* hazy, faded black shadow  top-to-bottom */
    maxigin_initMakeDropShadowSprite(
        shopLockedSprite,
        3,
        2,
        255,
        255,
        100,
        0,
        100,
        0 );

    REGISTER_VAL_MEM( shopRand );

    REGISTER_VAL_MEM( shopOnSaleRoll );

    REGISTER_ARRAY_MEM( shopSlotPrices );
    REGISTER_ARRAY_MEM( shopIsOnSale );
    REGISTER_ARRAY_MEM( shopIsPermaSale );
    
    REGISTER_ARRAY_MEM( shopItems );

    REGISTER_ARRAY_MEM( shopSlotLift );
    REGISTER_ARRAY_MEM( shopSlotSmoothLift );
    REGISTER_ARRAY_MEM( shopSlotHighlightFade );

    REGISTER_VAL_MEM( shopSlotsLifting );
    REGISTER_VAL_MEM( shopSlotsDropping );

    REGISTER_VAL_MEM( shoppingDone );
    
    REGISTER_VAL_MEM( shopNumVisibleSlots );

    REGISTER_VAL_MEM( shopSelectedSlot );

    REGISTER_VAL_MEM( shopSlotPickedWithController );

    REGISTER_ARRAY_MEM( shopSlotLocked );
    
    REGISTER_VAL_MEM( shopLockOver );
    REGISTER_VAL_MEM( shopAllLocked );
    }



void shopReroll( void ) {
    shopInternalReroll();

    shopSelectedSlot = -1;

    shopSlotPickedWithController = 0;
    
    shopResetHightlighFades();
    shopActionDown = 0;
    shoppingDone   = 0;
    
    buttonReset( doneButton );
    buttonReset( rerollButton );
    }



void shopLevelIncrement( void ) {
    costResetIncrement( shopRerollCost );
    costLevelIncrement( shopRerollCost );
    }



void shopReset( void ) {

    int  i;

    shopInternalReroll();

    shopSelectedSlot = -1;
    shopActionDown   =  0;
    shoppingDone     =  0;

    
    shopSlotPickedWithController = 0;

    costFullReset( shopRerollCost );

    buttonReset( doneButton );

    for( i = 0;
         i < NUM_SHOP_SLOTS;
         i ++ ) {
        
        shopSlotLocked[ i ] = 0;
        }
     
    shopAllLocked = 0;
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

            int  pieceXBase    =  shopCenterX + shopSlotPosX[i];
            int  pieceYBase    =  shopCenterY + shopSlotPosY[i];
            int  pieceYLifted  =  pieceYBase - shopSlotSmoothLift[i];
            int  lockSprite;
            int  lockY;
            int  lockLimit     =  3;
            

            if( shopSlotLocked[ i ] ) {
                if( pieceYLifted < pieceYBase - lockLimit ) {
                    pieceYLifted = pieceYBase - lockLimit;
                    }
                }
            
            drawPiece( p | CHESS_WHITE,
                       shopCenterX + shopSlotPosX[i],
                       pieceYLifted );

            if( shopSlotHighlightFade[i] > 0 ) {
                drawPieceHighlight( p | CHESS_WHITE,
                                    pieceXBase,
                                    pieceYLifted,
                                    shopSlotHighlightFade[i] );
                }
            if( shopSelectedSlot == i
                &&
                shopSlotPickedWithController ) {

                maxigin_drawButtonHintSprite(
                    shopActionHandle,
                    pieceXBase - 5,
                    pieceYLifted );
                }
            

            if( pieceYLifted >= pieceYBase - lockLimit ) {

                moneyDrawCoin( pieceXBase,
                               pieceYBase + 12 + 8,
                               64 );
                
                colorsApplyMoneyColor();
            
                numberDrawCenter( shopSlotPrices[ i ],
                                  pieceXBase,
                                  pieceYBase + 12,
                                  1 );

                if( shopIsOnSale[ i ] ) {

                    numberDrawCenter( shopPrices[p],
                                      pieceXBase,
                                      pieceYBase + 22,
                                      1 );

                    colorsApply( COLOR_SALE );
                    
                    maxigin_setLanguageFontIndex( 1 );
    
                    maxigin_drawLangText( lang_sale,
                                          pieceXBase,
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
                                              pieceXBase,
                                              pieceYBase - 43 - aboveS - belowP,
                                              MAXIGIN_CENTER );
                        }
    
                    maxigin_setLanguageFontIndex( 0 );

                    numberDrawText( "\\",
                                    pieceXBase,
                                    pieceYBase + 22,
                                    0,
                                    MAXIGIN_CENTER );
                    }
                }

            maxigin_drawResetColor();
            
            if( shopSlotLocked[ i ] ) {
                lockSprite = shopLockedSprite;
                lockY      = pieceYBase + 18;
                }
            else {
                lockSprite = shopUnlockedSprite;
                lockY      = pieceYBase + shopUnlockedYOffset;
                }
                                
            maxigin_drawSprite( lockSprite,
                                pieceXBase,
                                lockY );

            if( shopLockOver == i ) {
                maxigin_drawToggleAdditive( 1 );

                maxigin_drawSetAlpha( 92 );

                maxigin_drawSprite( lockSprite,
                                    pieceXBase,
                                    lockY );
            
                maxigin_drawResetColor();
            
                maxigin_drawToggleAdditive( 0 );
                }

            }
        }

    if( shopAllLocked ) {
        buttonDrawDisabled( rerollButton );
        }
    else {
        buttonDraw( rerollButton );
        }
    
    buttonDraw( doneButton );


    moneyDrawCoin( shopCenterX,
                   shopCenterY + 66 + 8,
                   64 );
    
    colorsApplyMoneyColor();
            
    numberDrawCenter( costGet( shopRerollCost ),
                    shopCenterX,
                    shopCenterY + 66,
                    1 );
    }



ChessPiece shopStep( int  inPickFailedSound,
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

    if( ! shopAllLocked
        &&
        buttonIsNewPressed( rerollButton ) ) {
        
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
        }
    
    
    if( maxigin_getActivePointerLocation( &pointerX,
                                          &pointerY ) ) {

        char  overAnyLocks  =  0;

        shopSlotPickedWithController = 0;
        
        shopSelectedSlot = -1;
    
        for( i = 0;
             i < shopNumVisibleSlots;
             i ++ ) {

            ChessPiece  p  =  shopItems[ i ];

            if( p != noPiece ) {

                int  pieceXBase  =  shopCenterX + shopSlotPosX[i];
                int  pieceYBase  =  shopCenterY + shopSlotPosY[i];
                
                if( getPixelOverPiece( p | CHESS_WHITE,
                                       pieceXBase,
                                       pieceYBase,
                                       pointerX,
                                       pointerY ) ) {

                    shopSelectedSlot = i;
                    shopSlotHighlightFade[ i ] = 255;
                    break;
                    }

                if( maxigin_isPointerInsideSprite(
                        shopUnlockedClickMaskSprite,
                        pieceXBase,
                        pieceYBase + shopUnlockedYOffset ) ) {

                    int  old  = shopLockOver;
                    
                    shopLockOver = i;

                    if( shopLockOver != old ) {
                        maxigin_playSoundEffect( inPieceLiftSound,
                                                 256 );
                        }
                    overAnyLocks = 1;
                    }
                }
            }

        if( ! overAnyLocks
            &&
            shopLockOver != -1 ) {
            
            shopLockOver = -1;
            
            maxigin_playSoundEffect( inPieceLiftSound,
                                     256 );
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

            if( dirX != 0
                ||
                dirY != 0 ) {

                /* left or right in shop row */

                int  dir  =  dirX;

                int  start  = shopSelectedSlot;

                if( dir == 0 ) {
                    dir = dirY;
                    }

                shopSelectedSlot += dir;
                if( shopSelectedSlot < 0 ) {
                    shopSelectedSlot = shopNumVisibleSlots - 1;
                    }
                else if( shopSelectedSlot >= shopNumVisibleSlots ) {
                    
                    shopSelectedSlot = 0;
                    }
                while( shopSelectedSlot != start
                       &&
                       shopItems[ shopSelectedSlot ] == noPiece ) {
                    
                    shopSelectedSlot += dir;
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

                unlocksCancelViewer();
                }
            }
        else if( shopSelectedSlot == -1 ) {
            navGetDir( 0,
                       &dirX,
                       &dirY );

            if( dirX == 1
                     ||
                     dirY == 1 ) {
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
            else if( dirX == -1
                     ||
                     dirY == -1 ) {
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

    shopAllLocked = 1;

    for( i = 0;
         i < shopNumVisibleSlots;
         i ++ ) {

        
        }

    

    for( i = 0;
         i < shopNumVisibleSlots;
         i ++ ) {

        shopAllLocked = shopAllLocked && shopSlotLocked[ i ];

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

    
    if( ! maxigin_isButtonDown( shopPointerActionHandle )
        &&
        ! maxigin_isButtonDown( shopActionHandle ) ) {
        shopActionDown = 0;
        }
    

    

    if( ! shopActionDown
        &&
        ( maxigin_isButtonDown( shopPointerActionHandle )
          ||
          maxigin_isButtonDown( shopActionHandle ) ) ) {

        if( shopSelectedSlot != -1
            &&
            shopItems[ shopSelectedSlot ] != noPiece ) {

            /* picking a piece to buy */

            if( shopSlotPrices[ shopSelectedSlot ]
                <=
                moneyGetTotal() ) {

                /* can afford */

                moneyAdd( - shopSlotPrices[ shopSelectedSlot ] );

                playerDeckAddPiece( shopItems[ shopSelectedSlot ] );
                playerDeckReshuffle();

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
        else if( shopLockOver != -1 ) {

            /* toggle lock/unlock */
            shopSlotLocked[ shopLockOver ] = ! shopSlotLocked[ shopLockOver ];

            if( shopSlotLocked[ shopLockOver ] ) {
                maxigin_playSoundEffect( lockSound,
                                         300 );
                }
            else {
                playAddSound();
                }
            
            }
        shopActionDown = 1;
        }

    if( controllerMovedSlot ) {
        /* return noPiece for one step, to allow piece info panel
           to fade slightly, and so that game will play sound */
        return noPiece;
        }
    
    if( shopSelectedSlot == -1 ) {
        return noPiece;
        }
    
    return shopItems[ shopSelectedSlot ];
    }



char isShoppingDone( void ) {
    return shoppingDone;
    }



#endif

#endif
