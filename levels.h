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


/*
  always call getEmptyLevel first, to clear the board

  outState is filled with the enemy pieces and the player's king piece
            and pieces drawn from the player's deck

   inSideToAdd can be CHESS_WHITE or CHESS_BLACK
               CHESS_WHITE simply adds white pieces to an existing board
               CHESS_BLACK simply adds black pieces to an existing board
*/
void getLevel( int          inLevelNumber,
               BoardState  *outState,
               int          inSideToAdd );


void getEmptyLevel( BoardState  *outState );



/* outFormation is where the formation will go.  0 for empty,
   1 for regular piece, 2 for king.

   inSideColor  is either CHESS_WHITE or CHESS_BLACK
*/
void levelGetRandomFormation( char  outFormation[BH][BW],
                              int   inNumNonKingPieces,
                              int   inSideColor );




#endif



#ifdef  LEVELS_IMPLEMENTATION

#ifndef LEVELS_IMPLEMENTATION_INCLUDED
#define LEVELS_IMPLEMENTATION_INCLUDED




#include "memoryRegister.h"

#include "formation.h"
#include "playerDeck.h"


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
               int          inSideToAdd ) {

    int   y;
    int   x;
    
    

    if( inSideToAdd == CHESS_WHITE ) {
        /* clear board first, then draw from player
           deck into their formation  */

        for( y = 0;
             y < BH;
             y ++ ) {

            for( x = 0;
                 x < BW;
                 x ++ ) {

                char  fSpot  =  formationGet( y,
                                              x );
                
                if( fSpot == 1 ) {
                    outState->grid[ y ][ x ] =
                        playerDeckDraw() | CHESS_WHITE;
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
    else if( inSideToAdd == CHESS_BLACK ) {

        int  numEnemyPieces  =  getNumEnemyPieces( inLevelNumber );


        static  char  form[ BH ][ BW ];

        /* add new pieces to enemy deck based on level number */
        prepareEnemyDeck( inLevelNumber );

        levelGetRandomFormation( form,
                                 numEnemyPieces,
                                 CHESS_BLACK );

        for( y = 0;
             y < BH;
             y ++ ) {

            for( x = 0;
                 x < BW;
                 x ++ ) {

                char  fSpot  =  form[ y ][ x ];


                if( fSpot == 0 ) {
                    continue;
                    }

                if( fSpot == 2 ) {
                    outState->grid[ y ][ x ]  = king | CHESS_BLACK;
                    outState->kingExists[ 1 ] =  1;
                    continue;
                    }
                if( fSpot == 1 ) {
                    outState->grid[ y ][ x ] =
                        deckDraw( &enemyDeck ) | CHESS_BLACK;
                    }
                }
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



void levelGetRandomFormation( char  outFormation[BH][BW],
                              int   inNumNonKingPieces,
                              int   inSideColor ) {

    int   y;
    int   x;
    int   kingRow;
    int   kingCol;
    int   protRow;
    int   protColA;
    int   protColB;
    int   s;
    int   numFilled;
    
    /* enemy spots in first 3 rows */
    static  int  spots[ 3 * BW ];

    
    for( y = 0;
         y < BH;
         y ++ ) {

        for( x = 0;
             x < BW;
             x ++ ) {

            outFormation[ y ][ x ]  = 0;
            }
        }

    /* place king anywhere in first two rows */

    if( inSideColor == CHESS_BLACK ) {
        kingRow  =  maxigin_randRange( &levelsRand,
                                       0,
                                       1 );
        }
    else {
        kingRow  =  maxigin_randRange( &levelsRand,
                                       BH - 1,
                                       BH - 2 );
        }
    
        
    kingCol  =  maxigin_randRange( &levelsRand,
                                   0,
                                   BW - 1 );

    outFormation[ kingRow ][ kingCol ] = 2;
    

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

    if( inSideColor == CHESS_BLACK ) {

        protRow = kingRow + 1;
        }
    else {
        protRow = kingRow - 1;
        }
    
    outFormation[ protRow ][ protColA ] = 1;
    outFormation[ protRow ][ protColB ] = 1;

    inNumNonKingPieces -= 2;
        

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
    
    while( numFilled < inNumNonKingPieces
           &&
           s < 3 * BW ) {

        y = spots[ s ] / BW;
        x = spots[ s ] - y * BW;

        if( inSideColor == CHESS_WHITE ) {

            /* other back rows for white */
            y += BH - 4;
            }
            

        if( outFormation[ y ][ x ] == 0 ) {
                
            outFormation[ y ][ x ] = 1;
            numFilled ++;
            }
        s ++;
        }
    }




#endif

#endif
