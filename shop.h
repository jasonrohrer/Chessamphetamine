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


void shopInit( void );



/* refresh the shop, rolling new pieces from the internal shop decks,
   and incrementing prices */
void shopReroll( void );

/* resets the shop back to its starting state
   ( starting prices, fully shuffled decks )
*/
void shopReset( void );



void shopDraw( int  inPosX,
               int  inPosY );


void shopStep( void );



#ifdef SHOP_IMPLEMENTATION


#include "deck.h"
#include "numbers.h"
#include "pieceSprites.h"

#include "memoryRegister.h"


/* one free deck, one paid deck with everything
   and two paid decks with more and more rarity */
#define             NUM_SHOP_DECKS  5


static  Deck        shopDecks             [ NUM_SHOP_DECKS ];
static  int         shopStartingPrices    [ NUM_SHOP_DECKS ]  =  {  0,
                                                                    1,
                                                                    2,
                                                                    4,
                                                                    8 };
static  int         shopCurrentPrices     [ NUM_SHOP_DECKS ];
static  ChessPiece  shopItems             [ NUM_SHOP_DECKS ];

static  int         shopPriceIncrements   [ NUM_SHOP_DECKS ]  =  {  0,
                                                                    1,
                                                                    1,
                                                                    1,
                                                                    2 };
    



static void shopSetStartingPrices( void ) {
    int  i;

    for( i = 0;
         i < NUM_SHOP_DECKS;
         i ++ ) {

        shopCurrentPrices[ i ] = shopStartingPrices[ i ];
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





void shopInit( void ) {

    /* each more expensive deck focuses on double the rarity of
       the base two decks */

    int  rarityFilter  =  MAX_DECK_PIECE_OCCURRENCE;
    int  i;
    
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

    shopSetStartingPrices();

    shopInternalReroll();

    REGISTER_ARRAY_MEM( shopDecks );

    REGISTER_ARRAY_MEM( shopCurrentPrices );
    
    REGISTER_ARRAY_MEM( shopItems );
    }



void shopReroll( void ) {
    shopInternalReroll();

    shopIncrementPrices();
    }



void shopReset( void ) {

    int  i;
    
    shopSetStartingPrices();

    for( i = 0;
         i < NUM_SHOP_DECKS;
         i ++ ) {

        deckReshuffleAll( &( shopDecks[ i ] ) );
        }
    }



void shopDraw( int  inPosX,
               int  inPosY ) {

    int  hopSize       =  30;
    int  numStartHops  =  NUM_SHOP_DECKS / 2;
    int  startHop      =  hopSize * numStartHops;
    int  curPos;
    int  i;
    
    if( ( NUM_SHOP_DECKS % 2 ) == 0 ) {
        /* center between two middle slots */
        startHop -= hopSize / 2;
        }
    
    maxigin_drawResetColor();

    curPos  = inPosX - startHop;

    for( i = 0;
         i < NUM_SHOP_DECKS;
         i ++ ) {

        ChessPiece  p  =  shopItems[ i ];

        if( p != noPiece ) {
            
            drawPiece( p | CHESS_WHITE,
                       curPos,
                       inPosY );

            maxigin_drawResetColor();
            
            numberDrawCenter( shopCurrentPrices[ i ],
                              curPos,
                              inPosY + 12,
                              1 );
            }

        curPos += hopSize;
        }
    
    }



void shopStep( void ) {

    /* fixme
       react to mouse and controller

       show piece info panel
    */
    }



#endif

#endif
