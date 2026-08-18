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

   inSideToAdd can be CHESS_WHITE or CHESS_BLACK
               CHESS_WHITE clears the board before adding white pieces
               CHESS_BLACK simply adds black pieces to an existing board
*/
void getLevel( int          inLevelNumber,
               BoardState  *outState,
               Deck        *inPlayerDeck,
               int          inSideToAdd );


void getEmptyLevel( BoardState  *outState );




#endif



#ifdef  LEVELS_IMPLEMENTATION

#ifndef LEVELS_IMPLEMENTATION_INCLUDED
#define LEVELS_IMPLEMENTATION_INCLUDED




#include "memoryRegister.h"

#include "formation.h"


static  MaxiginRand  levelsRand;
static  Deck         enemyDeck;



static int saturationCurve( int  inStart,
                            int  inMax,
                            int  inK,
                            int  inL ) {
    return
        inStart
        +
        ( ( (inMax - inStart) * inL )
          /
          ( inL + inK ) );
    }



/* number of enemy pieces in addition to the king */
static int getNumEnemyPieces( int  inLevelNumber ) {

    /* start at 2 */
    int  start  =   2;
    /* reach halfway in max curve by level 20 */
    int  kmax   =  20;
    /* reach halfway in min curve by level 30 */
    int  kmin   =  30;

    /* the min and max curves define a growing window of
       possible piece densities as we go up in levels.
       For level 0,   we always have   2        pieces.
       For level 8,   we have between  4 and  8 pieces.
       For level 100, we have between 12 and 19 pieces.
    */
    int  max    =  saturationCurve( start,
                                    23,
                                    kmax,
                                    inLevelNumber );
    
    int  min    =  saturationCurve( start,
                                    15,
                                    kmin,
                                    inLevelNumber );

    
    return maxigin_randRange( &levelsRand,
                              min,
                              max );
    }
    
    



void levelsInit( void ) {

    maxigin_randSeed( &levelsRand,
                      mingin_getEntropySeed() );

    getEmptyDeck( &enemyDeck,
                  0 );

    REGISTER_VAL_MEM( levelsRand );
    REGISTER_VAL_MEM( enemyDeck  );
    }


static void prepareEnemyDeck( int  inLevelNumber ) {
    if( inLevelNumber == 0 ) {

        /* 4 pieces in deck at level 0 */

        /* whenever asked for level 0 again, start of new game,
           rebuild deck from start */
        getEmptyDeck( &enemyDeck,
                      0 );
        
        deckAddPiece( &enemyDeck,
                      pawn );
        deckAddPiece( &enemyDeck,
                      bishop );
        deckAddPiece( &enemyDeck,
                      bishop );
        deckAddPiece( &enemyDeck,
                      knight );
        }
    if( inLevelNumber == 1 ) {
        deckAddPiece( &enemyDeck,
                      rook );
        }
    if( inLevelNumber == 2 ) {
        deckAddPiece( &enemyDeck,
                      rook );
        }

    if( inLevelNumber == 3 ) {
        deckAddPiece( &enemyDeck,
                      knight );
        }
    
    if( inLevelNumber == 4 ) {
        /* 8 pieces in deck at level 4,
           including Queen */
        deckAddPiece( &enemyDeck,
                      queen );
        }

    if( inLevelNumber > 4
        &&
        inLevelNumber <= 16 ) {

        /* add up to 12 extra normal pieces
           between level 5 and level 16,
           we get extra normal chess pieces added at random */
        deckAddPiece(
            &enemyDeck,
            (ChessPiece)( maxigin_randRange( &levelsRand,
                                             FIRST_CHESS_PIECE,
                                             LAST_NORMAL_CHESS_PIECE ) ) );
        }

    /* by level 16, our deck has 20 pieces */

    if( inLevelNumber > 16
        &&
        inLevelNumber <= 46 ) {
        /* add up to 30 extra special pieces
           between level 17 and level 46, we get extra special
           chess pieces added at random */

        deckAddPiece(
            &enemyDeck,
            (ChessPiece)( maxigin_randRange(
                              &levelsRand,
                              FIRST_SPECIAL_CHESS_PIECE,
                              LAST_ENEMY_SPECIAL_CHESS_PIECE ) ) );
        }

    /* by level 46, our deck has 50 pieces */
    
    if( inLevelNumber > 46 ) {

        /* add random non-King piece */

        int         t  =  0;
        ChessPiece  p  =  (ChessPiece)( maxigin_randRange(
                                            &levelsRand,
                                            FIRST_CHESS_PIECE,
                                            LAST_CHESS_PIECE ) );
        /* 10 trials before giving up */
        while( p == king
               &&
               t < 10 ) {

            p = (ChessPiece)( maxigin_randRange(
                                  &levelsRand,
                                  FIRST_CHESS_PIECE,
                                  LAST_CHESS_PIECE ) );
            t ++;
            }

        /* by level 100, our deck has 104 pieces */

        /* after level 100 we stop adding extra pieces, so the deck
           doesn't just keep growing in size.
           
           However, we keep replacing pieces with a random piece,
           so deck gradually keeps changing composition forever
        */

        if( p != king ) {

            if( inLevelNumber <= 100 ) {
                deckAddPiece( &enemyDeck,
                              p );
                }
            else {

                ChessPiece  oldP  =  deckDraw( &enemyDeck );

                deckReplacePiece( &enemyDeck,
                                  oldP,
                                  p );
                }
            }
        }

    deckReshuffleAll( &enemyDeck );
    }



void getLevel( int          inLevelNumber,
               BoardState  *outState,
               Deck        *inPlayerDeck,
               int          inSide ) {

    int   y;
    int   x;
    
    

    if( inSide == CHESS_WHITE ) {
        /* clear board first, then draw from player
           deck into their formation  */

        outState->kingExists[0] = 0;
        outState->kingExists[1] = 0;
    
        for( y = 0;
             y < BH;
             y ++ ) {

            for( x = 0;
                 x < BW;
                 x ++ ) {

                char  fSpot  =  formationGet( y,
                                              x );
                
                outState->grid[ y ][ x ] = noPiece;

                if( fSpot == 1 ) {
                    outState->grid[ y ][ x ] =
                        deckDraw( inPlayerDeck ) | CHESS_WHITE;
                    continue;
                    }
                else if( fSpot == 2 ) {
                    outState->grid[ y ][ x ]  = king | CHESS_WHITE;
                    outState->kingExists[ 0 ] =  1;
                    continue;
                    }
                }
            }
        }
    else if( inSide == CHESS_BLACK ) {

        int  numEnemyPieces  =  getNumEnemyPieces( inLevelNumber );

        /* place king anywhere in first two rows */
        
        int  kingRow  =  maxigin_randRange( &levelsRand,
                                            0,
                                            1 );
        int  kingCol  =  maxigin_randRange( &levelsRand,
                                            0,
                                            BW - 1 );

        int  protColA;
        int  protColB;

        /* enemy spots in first 3 rows */
        static  int  spots[ 3 * BW ];

        int  s;
        int  numFilled;
        

        /* add new pieces to enemy deck based on level number */
        prepareEnemyDeck( inLevelNumber );
        
        
        outState->grid[ kingRow ][ kingCol ]  = king | CHESS_BLACK;
        outState->kingExists[ 1 ] =  1;

        protColA = kingCol;

        if( kingCol >= BW / 2 ) {
            /* king on right side of board
               two protection pieces in front and to front left */
            protColB = kingCol - 1;
            }
        else {
            /* front right protection */
            protColB = kingCol + 1;
            }

        outState->grid[ kingRow + 1 ][ protColA ] =
            deckDraw( &enemyDeck ) | CHESS_BLACK;
        
        outState->grid[ kingRow + 1 ][ protColB ] =
            deckDraw( &enemyDeck ) | CHESS_BLACK;

        numEnemyPieces -= 2;
        

        /* random spots in first three rows */
            

        for( s = 0;
             s < 3 * BW;
             s ++ ) {
            spots[ s ] = s;
            }
        maxigin_shuffle( &levelsRand,
                         3 * BW,
                         spots );
        s = 0;

        numFilled = 0;
        while( numFilled < numEnemyPieces
               &&
               s < 3 * BW ) {

            y = spots[ s ] / BW;
            x = spots[ s ] - y * BW;

            if( outState->grid[ y ][ x ] == noPiece ) {
                
                outState->grid[ y ][ x ] = deckDraw( &enemyDeck ) | CHESS_BLACK;
                numFilled ++;
                }
            s ++;
            }
        }
    
    outState->nextToMove = CHESS_WHITE;
    outState->moveCount = 0;
    }



void getEmptyLevel( BoardState  *outState ) {

    int   y;
    int   x;

    for( y = 0;
         y < BH;
         y ++ ) {

        for( x = 0;
             x < BW;
             x ++ ) {

            outState->grid[ y ][ x ]  = noPiece;
            }
        }
                    
    outState->kingExists[0] = 0;
    outState->kingExists[1] = 0;

    outState->nextToMove = CHESS_WHITE;
    outState->moveCount = 0;
    }



#endif

#endif
