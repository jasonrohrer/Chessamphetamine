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

  outState is filled with the enemy pieces
*/
void getEnemyLevel( int          inLevelNumber,
                    BoardState  *outState );


void getEmptyLevel( BoardState  *outState );



/* outFormation is where the formation will go.  0 for empty,
   1 for regular piece, 2 for king.

   inSideColor  is either CHESS_WHITE or CHESS_BLACK
*/
void levelGetRandomFormation( char  outFormation[ BH ][ BW ],
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


/* for definition of ENEMY_DECK_FLAG_LIST */
#include "pieceSpecs.h"


/* we use these numerical flags to mark tiers of enemy pieces
   0 => pieces that are NEVER in enemy deck
   1 => regular chess pieces
   2 => special pieces tier A
   3 => special pieces tier B (more insane pieces)
*/

static int enemyDeckFlags[] = {
    MAKE_CHESS_ARRAY( ENEMY_DECK_FLAG_LIST )
    };

CHECK_CHESS_ARRAY( enemyDeckFlags,
                   ENEMY_DECK_FLAG_LIST );



#define NUM_ENEMY_TIERS  4

int         numPerTier[ NUM_ENEMY_TIERS ];
ChessPiece  enemyTiers[ NUM_ENEMY_TIERS ][ NUM_CHESS_PIECES ];

int         numInAllTiers;
ChessPiece  allEnemyTiers[ NUM_CHESS_PIECES ];



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

    int         i;
    ChessPiece  p;
    

    maxigin_randSeed( &levelsRand,
                      mingin_getEntropySeed() );

    getEmptyDeck( &enemyDeck,
                  0 );


    /* set up enemy piece tiers */

    for( i = 0;
         i < NUM_ENEMY_TIERS;
         i   ++ ) {
        numPerTier[ i ] = 0;
        }

    numInAllTiers = 0;
    

    for( p = FIRST_CHESS_PIECE;
         p < NUM_CHESS_PIECES;
         p   ++ ) {

        int  flag  =  enemyDeckFlags[ p ];

        if( flag >= NUM_ENEMY_TIERS ) {
            continue;
            }

        enemyTiers[ flag ][ numPerTier[ flag ] ] = p;

        numPerTier[ flag ] ++;

        if( flag > 0 ) {
            /* all non-zero tiers go in here */

            allEnemyTiers[ numInAllTiers ] = p;
            numInAllTiers++;
            }
        }

    
    REGISTER_VAL_MEM( levelsRand );
    REGISTER_VAL_MEM( enemyDeck  );
    }



/* pass -1 to get from all tiers together */
static ChessPiece getRandEnemyPieceFromTier( int  inTier ) {

    int  i;

    if( inTier == -1 ) {

        if( numInAllTiers == 0 ) {
            return pawn;
            }
        i = maxigin_randRange( &levelsRand,
                               0,
                               numInAllTiers - 1 );

        return allEnemyTiers[ i ];
        }

    
    if( numPerTier[ inTier ] <= 0 ) {
        return pawn;
        }

    i = maxigin_randRange( &levelsRand,
                           0,
                           numPerTier[ inTier ] - 1 );

    return enemyTiers[ inTier ][ i ];
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
        inLevelNumber <= 12 ) {

        /* add up to 8 extra normal pieces
           between level 5 and level 12,
           we get extra normal chess pieces added at random */
        deckAddPiece(
            &enemyDeck,
            getRandEnemyPieceFromTier( 1 ) );
        }

    /* by level 12, our deck has 16 pieces */

    if( inLevelNumber > 12
        &&
        inLevelNumber <= 46 ) {
        /* add up to 34 extra special pieces
           between level 13 and level 46, we get extra special
           chess pieces added at random */

        deckAddPiece(
            &enemyDeck,
            getRandEnemyPieceFromTier( 2 ) );
        }

    /* by level 46, our deck has 50 pieces */
    
    if( inLevelNumber > 46 ) {

        /* add random piece from all tiers, including
           ridiculous level-3 tier */

        ChessPiece  p  =  getRandEnemyPieceFromTier( -1 );
  

        /* by level 100, our deck has 104 pieces */

        /* after level 100 we stop adding extra pieces, so the deck
           doesn't just keep growing in size.
           
           However, we keep replacing pieces with a random piece,
           so deck gradually keeps changing composition forever
        */

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

    deckReshuffleAll( &enemyDeck );
    }



void getEnemyLevel( int inLevelNumber,
                    BoardState  *outState ) {

    int   y;
    int   x;
    int   numEnemyPieces  =  getNumEnemyPieces( inLevelNumber );

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
                outState->kingExists[ CHESS_BLACK >> 7 ] =  1;
                continue;
                }
            if( fSpot == 1 ) {
                outState->grid[ y ][ x ] =
                    deckDraw( &enemyDeck ) | CHESS_BLACK;
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



void levelGetRandomFormation( char  outFormation[ BH ][ BW ],
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
                                       BH - 2,
                                       BH - 1 );
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
            y += BH - 3;
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
