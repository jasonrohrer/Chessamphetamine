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
                     int  inPickFailedSound );


char isShoppingDone( void );




#ifdef SHOP_IMPLEMENTATION


#include "deck.h"
#include "numbers.h"
#include "pieceSprites.h"

#include "memoryRegister.h"
#include "mingin.h"

#include "button.h"



/* one free deck, one paid deck with everything
   and two paid decks with more and more rarity */
#define                NUM_SHOP_DECKS  6


static  Deck           shopDecks             [ NUM_SHOP_DECKS ];
static  int            shopStartingPrices    [ NUM_SHOP_DECKS ]   =  {  0,
                                                                        1,
                                                                        2,
                                                                        4,
                                                                        8,
                                                                        16 };
static  int            shopCurrentPrices     [ NUM_SHOP_DECKS ];
static  ChessPiece     shopItems             [ NUM_SHOP_DECKS ];

static  int            shopPriceIncrements   [ NUM_SHOP_DECKS ]   =  {  0,
                                                                        1,
                                                                        1,
                                                                        1,
                                                                        2,
                                                                        2 };
static  int            shopSlotPosX          [ NUM_SHOP_DECKS ];
static  int            shopSlotPosY          [ NUM_SHOP_DECKS ];
static  int            shopSelectedSlot                           =  -1;
static  unsigned char  shopSlotHighlightFade [ NUM_SHOP_DECKS ];
static  char           shopActionDown                             =   0;


static  int            purchaseSound                              =  -1;

static  int            lang_shopTitle                             =  -1;

static  char           shoppingDone                               =   0;

static  int            doneButton                                 =  -1;

static  int            shopPointerActionHandle                    =  -1;

static  int            shopCenterX;
static  int            shopCenterY;



static void shopSetStartingPrices( void ) {
    int  i;

    for( i = 0;
         i < NUM_SHOP_DECKS;
         i ++ ) {

        shopCurrentPrices[ i ] = shopStartingPrices[ i ];
        }
    }



static void shopResetHightlighFades( void ) {
    int  i;

    for( i = 0;
         i < NUM_SHOP_DECKS;
         i ++ ) {

        shopSlotHighlightFade[ i ] = 0;
        }
    }



/* rerolls without updating prices */
static void shopInternalReroll( void ) {
    int  i;

    for( i = 0;
         i < NUM_SHOP_DECKS;
         i ++ ) {
        shopItems[ i ] = deckDraw( &( shopDecks[i] ) );
        }
    }



static void shopIncrementPrices( void ) {
    int  i;

    for( i = 0;
         i < NUM_SHOP_DECKS;
         i ++ ) {
        shopCurrentPrices[ i ] = shopPriceIncrements[ i ];
        }
    }





void shopInit( int  inPointerActionHandle,
               int  inCenterX,
               int  inCenterY ) {

    /* each more expensive deck focuses on double the rarity of
       the base two decks */

    int  rarityFilter  =  MAX_DECK_PIECE_OCCURRENCE;
    int  i;
    int  hopSize       =  30;
    int  numStartHops  =  NUM_SHOP_DECKS / 2;
    int  startHop      =  hopSize * numStartHops;
    int  curPos;

    shopPointerActionHandle = inPointerActionHandle;

    shopCenterX = inCenterX;
    shopCenterY = inCenterY;
    

    purchaseSound = maxigin_initSoundEffect( "purchase_sd_30.wav" );

    lang_shopTitle  = maxigin_initTranslationKey( "shopTitle" );
    
    getShopDeck( &( shopDecks[ 0 ] ),
                    rarityFilter );
    
    getShopDeck( &( shopDecks[ 1 ] ),
                    rarityFilter );

    for( i = 2;
         i < NUM_SHOP_DECKS;
         i ++ ) {

        rarityFilter /= 2;

        if( rarityFilter <= 0 ) {
            rarityFilter = 1;
            }
        
        getShopDeck( &( shopDecks[ i ] ),
                    rarityFilter );
        }



    /* set up slot positions */
 
    
    if( ( NUM_SHOP_DECKS % 2 ) == 0 ) {
        /* center between two middle slots */
        startHop -= hopSize / 2;
        }
    
    curPos  = - startHop;

    for( i = 0;
         i < NUM_SHOP_DECKS;
         i ++ ) {

        shopSlotPosX[ i ] =  curPos;
        shopSlotPosY[ i ] =  0;

        curPos += hopSize;
        }
    

    shopSetStartingPrices();

    shopInternalReroll();

    shopResetHightlighFades();


    doneButton = buttonInit( maxigin_initSprite( "doneButton.tga" ),
                             -1,
                             maxigin_initSprite( "doneButtonPressed.tga" ),
                             shopCenterX,
                             shopCenterY + 50,
                             1,
                             shopPointerActionHandle,
                             /* fixme... need controller mapping for this */
                             -1 );

    REGISTER_ARRAY_MEM( shopDecks );

    REGISTER_ARRAY_MEM( shopCurrentPrices );
    
    REGISTER_ARRAY_MEM( shopItems );
    }



void shopReroll( void ) {
    shopInternalReroll();

    shopIncrementPrices();

    shopSelectedSlot = -1;
    shopResetHightlighFades();
    shopActionDown = 0;
    shoppingDone   = 0;

    buttonReset( doneButton );
    }



void shopReset( void ) {

    int  i;
    
    shopSetStartingPrices();

    for( i = 0;
         i < NUM_SHOP_DECKS;
         i ++ ) {

        deckReshuffleAll( &( shopDecks[ i ] ) );
        }

    shopSelectedSlot = -1;
    shopActionDown   =  0;
    shoppingDone     =  0;
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
         i < NUM_SHOP_DECKS;
         i ++ ) {

        ChessPiece  p  =  shopItems[ i ];

        if( p != noPiece ) {
            
            drawPiece( p | CHESS_WHITE,
                       shopCenterX + shopSlotPosX[i],
                       shopCenterY + shopSlotPosY[i] );

            if( shopSlotHighlightFade[i] > 0 ) {
                drawPieceHightlight( p | CHESS_WHITE,
                                     shopCenterX + shopSlotPosX[i],
                                     shopCenterY + shopSlotPosY[i],
                                     shopSlotHighlightFade[i] );
                }
            

            maxigin_drawResetColor();
            
            numberDrawCenter( shopCurrentPrices[ i ],
                              shopCenterX + shopSlotPosX[i],
                              shopCenterY + shopSlotPosY[i] + 12,
                              1 );
            }

        }

    buttonDraw( doneButton );
    
    }



ChessPiece shopStep( Deck  *inPlayerDeck,
                     int  inPickFailedSound ) {

    /* fixme
       react to mouse and controller

       show piece info panel
    */

    /* fixme:
       also handle case where controller is used */
    int  pointerX;
    int  pointerY;
    int  i;
    int  r          =  mingin_getStepsPerSecond();
    int  deltaFade  =  ( 20 * 60 ) / r;

    if( buttonIsNewPressed( doneButton ) ) {
        shoppingDone = 1;
        return noPiece;
        }
    
    if( ! maxigin_getPointerLocation( &pointerX,
                                      &pointerY ) ) {
        /* pointer not available */
        return noPiece;
        }
    

    shopSelectedSlot = -1;
    
    for( i = 0;
         i < NUM_SHOP_DECKS;
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
         i < NUM_SHOP_DECKS;
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

            if( shopCurrentPrices[ shopSelectedSlot ]
                <=
                moneyGetTotal() ) {

                /* can afford */

                moneyAdd( - shopCurrentPrices[ shopSelectedSlot ] );

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
