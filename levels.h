/*
  Include in your C code wherever like so:

      #include "levels.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define LEVELS_IMPLEMENTATION
      #include "levels.h"

*/

#ifndef LEVELS_H_INCLUDED
#define LEVELS_H_INCLUDED

#include "deck.h"


void levelsInit( void );


/* outState is filled with the enemy pieces and the player's king piece
            and pieces drawn from the player's deck
*/
void getLevel( int          inLevelNumber,
               BoardState  *outState,
               Deck        *inPlayerDeck );





#ifdef LEVELS_IMPLEMENTATION


#include "memoryRegister.h"


#define NUM_POSSIBLE_LEVELS  256


/* starting piece locations
   0  empty
   1  player's king
   2  enemy's  king
   3  player's piece spot
   4  enemy's piece spot  */
static  char         pieceLayouts[ NUM_POSSIBLE_LEVELS ][ BH ][ BW ];

static  MaxiginRand  levelsRand;

static  const char  *levelsFile       =  "levels.tga";

static  int          numLoadedLevels  =  0;


static void makeDefaultLayouts( void ) {
    int  i;

    for( i = 0;
         i < NUM_POSSIBLE_LEVELS;
         i ++ ) {

        int  y;
        int  x;

        for( y = 0;
             y < BH;
             y ++ ) {

            for( x = 0;
                 x < BW;
                 x ++ ) {
                
                pieceLayouts[i][y][x] = 0;
                }
            }

        /* normal chess layout, default */

        /* fill first two and last two rows with pieces */
        for( y = BH - 2;
             y < BH;
             y ++ ) {

            for( x = 0;
                 x < BW;
                 x ++ ) {
                
                pieceLayouts[i][y][x] = 3;
                }
            }
        
        for( y = 0;
             y < 2;
             y ++ ) {

            for( x = 0;
                 x < BW;
                 x ++ ) {
                
                pieceLayouts[i][y][x] = 4;
                }
            }

           

        /* kings */

        pieceLayouts[i][ BH - 1 ][ 4 ] = 1;
        pieceLayouts[i][ 0      ][ 4 ] = 2;
        }


    
    }


static void levelsReload( void ) {

    /* layouts are represented by blocks of pixels */
    int             layoutPixelHeight  =  BH + 1;
    
    unsigned char  *loadedBytes;
    int             w;
    int             h;
    char            loaded;
    int             numLoadedLayouts;
    int             i;
    
    
    makeDefaultLayouts();
    
    loaded = maxigin_loadTempSprite( levelsFile,
                                     &loadedBytes,
                                     &w,
                                     &h );

    if( !loaded ) {
        return;
        }


    numLoadedLayouts = h / layoutPixelHeight;

    if( numLoadedLayouts > NUM_POSSIBLE_LEVELS ) {
        numLoadedLayouts = NUM_POSSIBLE_LEVELS;
        }

    for( i = 0;
         i < numLoadedLayouts;
         i ++ ) {

        /* image w might be bigger than BW
           there can be comments, etc off to right side */
        int  startPixel  =  i * layoutPixelHeight * w;
        
        
        int  y;
        int  x;

        for( y = 0;
             y < BH;
             y ++ ) {
            
            int  p           =  startPixel * 4;

            for( x = 0;
                 x < BW;
                 x ++ ) {

                unsigned char  r  =  loadedBytes[ p ++ ];
                unsigned char  g  =  loadedBytes[ p ++ ];
                unsigned char  b  =  loadedBytes[ p ++ ];
                unsigned char  a  =  loadedBytes[ p ++ ];
                
                if( a == 0 ) {
                    pieceLayouts[i][y][x] = 0;
                    continue;
                    }

                if( r == 255
                    &&
                    g == 0
                    &&
                    b == 0 ) {

                    /* enemy king */
                    pieceLayouts[i][y][x] = 2;
                    continue;
                    }
                
                if( r == 0
                    &&
                    g == 255
                    &&
                    b == 0 ) {

                    /* player king */
                    pieceLayouts[i][y][x] = 1;
                    continue;
                    }

                if( r == 255
                    &&
                    g == 255
                    &&
                    b == 255 ) {

                    /* player piece spot*/
                    pieceLayouts[i][y][x] = 3;
                    continue;
                    }

                if( r == 0
                    &&
                    g == 0
                    &&
                    b == 0 ) {

                    /* enemy piece spot*/
                    pieceLayouts[i][y][x] = 4;
                    continue;
                    }
                
                }

            /* go to start of next row, skipping any extra
               pixels beyond BW */
            startPixel += w;
            }
        }

    numLoadedLevels = numLoadedLayouts;
    }

    



void levelsInit( void ) {

    maxigin_randSeed( &levelsRand,
                      mingin_getEntropySeed() );

    levelsReload();

    REGISTER_VAL_MEM( levelsRand );
    }



void getLevel( int          inLevelNumber,
               BoardState  *outState,
               Deck        *inPlayerDeck ) {

    int   y;
    int   x;

    Deck  enemyDeck;

    int   layoutIndex  =  inLevelNumber;
    

    if( mingin_getBulkDataChanged( levelsFile ) ) {
        levelsReload();
        }
    

    /* if beyond our max number of hand-authored levels, pick
       a random level from in the second half */
    if( layoutIndex >= numLoadedLevels ) {

        layoutIndex = maxigin_randRange( &levelsRand,
                                         numLoadedLevels / 2,
                                         numLoadedLevels - 1 );
        }


    getEmptyDeck( &enemyDeck,
                  0 );

    if( inLevelNumber >= 0 ) {
        deckAddPiece( &enemyDeck,
                      pawn );
        deckAddPiece( &enemyDeck,
                      bishop );
        deckAddPiece( &enemyDeck,
                      bishop );
        deckAddPiece( &enemyDeck,
                      knight );
        }
    if( inLevelNumber >= 1 ) {
        deckAddPiece( &enemyDeck,
                      rook );
        }
    if( inLevelNumber >= 2 ) {
        deckAddPiece( &enemyDeck,
                      rook );
        }

    if( inLevelNumber >= 3 ) {
        deckAddPiece( &enemyDeck,
                      knight );
        }
    
    if( inLevelNumber >= 4 ) {
        deckAddPiece( &enemyDeck,
                      queen );
        }

    if( inLevelNumber == 8 ) {
        /* special case for 8
           standard set of pieces
           but don't add 8 pawns for future levels */
        int  p;
        
        for( p = 0;
             p < 7;
             p ++ ) {
            deckAddPiece( &enemyDeck,
                          pawn );
            }
        }
        
            

    if( inLevelNumber >= numLoadedLevels ) {

        /* add random pieces to enemy deck for beyond levels */
        int          extra       =  inLevelNumber - numLoadedLevels + 1;
        int          i;
        for( i = 0;
             i < extra;
             i ++ ) {
            
            deckAddPiece(
                &enemyDeck,
                (ChessPiece)( maxigin_randRange( &levelsRand,
                                                 FIRST_CHESS_PIECE,
                                                 LAST_CHESS_PIECE ) ) );
            }
        }

    deckReshuffleAll( &enemyDeck );
    
    
    
    outState->kingExists[0] = 0;
    outState->kingExists[1] = 0;
    

    for( y = 0;
         y < BH;
         y ++ ) {

        for( x = 0;
             x < BW;
             x ++ ) {

            int  p  =  pieceLayouts[ layoutIndex ][ y ][ x ];

            outState->grid[ y ][ x ] = noPiece;

            if( p == 0 ) {
                continue;
                }

            if( p == 1 ) {
                outState->grid[ y ][ x ]  = king | CHESS_WHITE;
                outState->kingExists[ 0 ] =  1;
                continue;
                }

            if( p == 2 ) {
                outState->grid[ y ][ x ]  = king | CHESS_BLACK;
                outState->kingExists[ 1 ] =  1;
                continue;
                }

            if( p == 3 ) {
                /* spot for player piece */
                
                outState->grid[ y ][ x ] =
                    deckDraw( inPlayerDeck ) | CHESS_WHITE;
                
                continue;
                }
            if( p == 4 ) {
                /* enemy piece */
                ChessPiece enemyPiece = deckDraw( &enemyDeck );

                outState->grid[ y ][ x ] =
                    enemyPiece | CHESS_BLACK;
                }
            }
        }
    
    outState->nextToMove = CHESS_WHITE;
    outState->moveCount = 0;
    }



#endif

#endif
