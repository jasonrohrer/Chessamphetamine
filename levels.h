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


void levelsInit( void );


/* outState is filled with the enemy pieces and the player's king piece
   
   outPlayerPieceSpots is filled with an empty board state with "pawn"
   pieces where the player's pieces should go
*/
void getLevel( int          inLevelNumber,
               BoardState  *outState,
               BoardState  *outPlayerPieceSpots );





#ifdef LEVELS_IMPLEMENTATION


#include "memoryRegister.h"


#define NUM_POSSIBLE_LEVELS  256


/* list of possible pieces per level, terminated with noPiece
   never contains a King, because all levels have a king */
static  ChessPiece  possiblePieces  [ NUM_POSSIBLE_LEVELS ][ NUM_CHESS_PIECES ];

/* if all likelihoods are the same, then all pieces drawn with equal chance */
static  int         pieceLikelihoods[ NUM_POSSIBLE_LEVELS ][ NUM_CHESS_PIECES ];


/* starting piece locations
   0  empty
   1  player's king
   2  enemy's  king
   3  player's piece spot
   4  enemy's piece spot  */
static  char         pieceLayouts[ NUM_POSSIBLE_LEVELS ][ BH ][ BW ];

static  MaxiginRand  levelsRand;



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



void levelsInit( void ) {

    /* layouts are represented by blocks of pixels */
    int             layoutPixelHeight  =  BH + 1;
    
    unsigned char  *loadedBytes;
    int             w;
    int             h;
    char            loaded;
    int             numLoadedLayouts;
    int             i;


    maxigin_randSeed( &levelsRand,
                      mingin_getEntropySeed() );
    
    makeDefaultLayouts();
    
    loaded = maxigin_initTempSprite( "levels.tga",
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

        int  startPixel  =  i * layoutPixelHeight * BW;
        int  p           =  startPixel * 4;
        
        int  y;
        int  x;

        for( y = 0;
             y < BH;
             y ++ ) {

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
            }
        }


    

    for( i = 0;
         i < NUM_POSSIBLE_LEVELS;
         i ++ ) {
        int  p;

        for( p = 0;
             p < NUM_CHESS_PIECES;
             p ++ ) {
            possiblePieces[i][p] = noPiece;
            }

        /* all levels can have pawns */
        possiblePieces[i][0] = pawn;

        /* bishop introduced on level 1 */
        if( i > 0 ) {
            possiblePieces[i][1] = bishop;
            }

        /* knight introduced on level 3 */
        if( i > 2 ) {
            possiblePieces[i][2] = bishop;
            }

        /* rook introduced on level 8 */
        if( i > 7 ) {
            possiblePieces[i][3] = bishop;
            }
        
        /* queen on level 16 */
        if( i > 15 ) {
            possiblePieces[i][4] = queen;
            }

        /* laserPawn on level 25 */
        if( i > 24 ) {
            possiblePieces[i][5] = laserPawn;
            }

        /* adding rook on level 32 */
        if( i > 31 ) {
            possiblePieces[i][6] = addingRook;
            }
        
        /* doubling pawn on level 48 */
        if( i > 47 ) {
            possiblePieces[i][7] = doublingPawn;
            }
        
        /* laser rook on level 64 */
        if( i > 63 ) {
            possiblePieces[i][8] = laserRook;
            }
        }


    /* use standardized likelihoods for now, based on starting pieces
       on normal chess board */
    
    for( i = 0;
         i < NUM_POSSIBLE_LEVELS;
         i ++ ) {
        
        int  p;

        for( p = 0;
             p < NUM_CHESS_PIECES;
             p ++ ) {

            int  r = 0;

            switch( possiblePieces[i][p] ) {
                case noPiece:
                    r = 0;
                    break;
                case pawn:
                    r = 8;
                    break;
                case bishop:
                case knight:
                case rook:
                    r = 2;
                    break;
                case queen:
                    r = 1;
                    break;
                default:
                    /* all special pieces have same likelihood as queen
                       for now */
                    r = 1;
                }

            pieceLikelihoods[i][p] = r;
                
            }
        }


    REGISTER_VAL_MEM( levelsRand );
    }



static ChessPiece pickRandomLevelPiece( int  inLevelNumber ) {

    /* fixme */

    int  likelihoodSum  =  0;
    int  p;
    int  i              =  inLevelNumber;
    int  pick;

    for( p = 0;
         p < NUM_CHESS_PIECES;
         p ++ ) {

        if( possiblePieces[ i ][ p ] == noPiece ) {
            /* end of list */
            break;
            }

        likelihoodSum += pieceLikelihoods[ i ][ p ];
        }

    if( likelihoodSum == 0 ) {
        /* this should never happen */
        mingin_log( "likelihoodSum is zero "
                    "in pickRandomEnemyPiece in levels.h\n" );
        return pawn;
        }

    pick = (int)( maxigin_randRange( &levelsRand,
                                     0,
                                     likelihoodSum - 1 ) );

    likelihoodSum = 0;
    for( p = 0;
         p < NUM_CHESS_PIECES;
         p ++ ) {

        if( possiblePieces[ i ][ p ] == noPiece ) {
            /* end of list ?? */
            mingin_log( "walked end of list when looking for chosen bin "
                        "in pickRandomEnemyPiece in levels.h\n" );
            return pawn;
            }

        likelihoodSum += pieceLikelihoods[ i ][ p ];

        if( likelihoodSum > pick ) {
            /* just added a bin that took us over the top
               pick is in that bin */

            return possiblePieces[ i ][ p ];
            }
        }

    mingin_log( "got to end of loop without ever finding pick "
                "in pickRandomEnemyPiece in levels.h\n" );
    
    return pawn;
    }



void getLevel( int          inLevelNumber,
               BoardState  *outState,
               BoardState  *outPlayerPieceSpots ) {

    int  y;
    int  x;
    int  i;

    /* if beyond our max number of hand-authored levels, pick
       a random level from in the second half */
    if( inLevelNumber >= NUM_POSSIBLE_LEVELS ) {

        inLevelNumber = maxigin_randRange( &levelsRand,
                                           NUM_POSSIBLE_LEVELS / 2,
                                           NUM_POSSIBLE_LEVELS - 1 );
        }

    i = inLevelNumber;

    outState->kingExists[0] = 0;
    outState->kingExists[1] = 0;
    

    for( y = 0;
         y < BH;
         y ++ ) {

        for( x = 0;
             x < BW;
             x ++ ) {

            int  p  =  pieceLayouts[ i ][ y ][ x ];

            outState           ->grid[ y ][ x ] = noPiece;
            outPlayerPieceSpots->grid[ y ][ x ] = noPiece;

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
                
                outPlayerPieceSpots->grid[ y ][ x ] = pawn;
                
                continue;
                }
            if( p == 4 ) {
                /* enemy piece */

                outState->grid[ y ][ x ] =
                    pickRandomLevelPiece( i ) | CHESS_BLACK;
                }
            }
        }
    
    outState->nextToMove = CHESS_WHITE;
    outState->moveCount = 0;
    }



#endif

#endif
