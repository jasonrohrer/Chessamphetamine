/*
  Include in your C code wherever like so:

      #include "chess.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define CHESS_IMPLEMENTATION
      #include "chess.h"

*/

#ifndef CHESS_H_INCLUDED
#define CHESS_H_INCLUDED

typedef  unsigned char  ChessPiece;


/* enum defining all possible chess piece types */
enum{
    noPiece = 0,
    pawn,
    bishop,
    knight,
    rook,
    queen,
    king,
    NUM_CHESS_PIECES };

/* store color in 8th bit.  This gives us 127 unique pieces in both colors */
#define  CHESS_TYPE_MASK    0x7F
#define  CHESS_COLOR_MASK   0x80
#define  CHESS_WHITE        0x00
#define  CHESS_BLACK        0x80

/* rows, columns, and num square */
#define  BH                 8
#define  BW                 8
#define  BN                 ( BH * BW )

typedef struct BoardState{
        
        ChessPiece  grid[BH][BW];

        int         nextToMove;

    } BoardState;



typedef struct Move{

        /* in row, col order */
        unsigned char  startPos[2];
        unsigned char  endPos  [2];
        
    } Move;



void chessInit( void );




/* fills outState with the starting board state */
void getStartBoard( BoardState  *outState );



/* returns 1 if move possible, 0 if not */
char getRandomMove( BoardState  *inState,
                    Move        *outMove,
                    char         inWhiteTurn );



void applyMove( BoardState  *inState,
                Move        *inMove );




#ifdef CHESS_IMPLEMENTATION


/* the signature for a piece move function.
   We define one of these for each piece type in the enum above.

   These functions assume that a piece of the correct type is at inPieceRow
   and inPieceCol (so the function doesn't have to check this.

   These functions just look for legal moves for a given piece, in isolation,
   without considering rules around a king in check.

   Though the King function will never pick a move that moves into check.

   Returns the number of moves.
*/
typedef int (*PieceMoveFunction)( BoardState     *inState,
                                  /* color of the piece */
                                  unsigned char   inPieceColor,
                                  /* starting position of the piece */
                                  int             inPieceRow,
                                  int             inPieceCol,
                                  /* possible move rows and cols */
                                  unsigned char   outDestRows[BN],
                                  unsigned char   outDestCols[BN],
                                  /* resulting board states from possible
                                     moves */
                                  BoardState      outStates  [BN] );



static char isKingInCheck( BoardState  *inState,
                           int          inVictimKingColor );




static int noPieceMove( BoardState     *inState,
                        unsigned char   inPieceColor,
                        int             inPieceRow,
                        int             inPieceCol,
                        unsigned char   outDestRows[BN],
                        unsigned char   outDestCols[BN],
                        BoardState      outStates  [BN] ) {
    
    /* suppress warnings for unused params */
    (void)inState;
    (void)inPieceColor;
    (void)inPieceRow;
    (void)inPieceCol;
    (void)outDestRows;
    (void)outDestCols;
    (void)outStates;
    
    return 0;
    }



/* ignores en passant rules */
static int pawnMove( BoardState     *inState,
                     unsigned char   inPieceColor,
                     int             inPieceRow,
                     int             inPieceCol,
                     unsigned char   outDestRows[BN],
                     unsigned char   outDestCols[BN],
                     BoardState      outStates  [BN] ) {

    int  moveDir       =  1;
    int  maxDist       =  1;
    int  otherColor    =  CHESS_WHITE;
    int  n             =  0;
    int  i;
    
    if( inPieceColor == CHESS_WHITE ) {
        moveDir = -1;
        otherColor = CHESS_BLACK;

        if( inPieceRow == BH - 2 ) {
            maxDist = 2;
            }
        if( inPieceRow == 0 ) {
            /* at end, no move */
            return 0;
            }
        }
    else {
        moveDir = 1;
        otherColor = CHESS_WHITE;

        if( inPieceRow == 1 ) {
            maxDist = 2;
            }
        if( inPieceRow == BH - 1 ) {
            /* at end, no move */
            return 0;
            }
        }

    /* first, look at all forward moves, which can only go to empty squares
       and can only pass through empty squares if doing a double-move */
    for( i = 1;
         i <= maxDist;
         i ++ ) {

        /* viaRow might be the same as newRow if we're considering 1-step
           moves */
        unsigned char  newRow  =  (unsigned char)( inPieceRow + (moveDir * i) );
        unsigned char  viaRow  =  (unsigned char)( inPieceRow +  moveDir      );
        
        if( inState->grid[ newRow ][ inPieceCol ] == noPiece
            &&
            inState->grid[ viaRow ][ inPieceCol ] == noPiece ) {
            
            outDestRows[n] = newRow;
            outDestCols[n] = (unsigned char)inPieceCol;

            /* copy state to start with */
            outStates[n]   = *inState;

            /* copy piece into new spot */
            outStates[n].grid    [ newRow     ][ inPieceCol ] =
                outStates[n].grid[ inPieceRow ][ inPieceCol ];

            /* leave empty space behind */
            outStates[n].grid[ inPieceRow ][ inPieceCol ] = noPiece;

            outStates[n].nextToMove = otherColor;
            
            /* fixme:
               promote to Queen in final row */
            
            n++;
            }
        }

    /* capture moves */

    /* loop over left/right diagonal moves */
    for( i =  -1;
         i <=  1;
         i +=  2 ) {
        
        unsigned char  newRow    =  (unsigned char)( inPieceRow +  moveDir );
        int            newCol    =  inPieceCol + i;
        ChessPiece     targetP;
        
        if( newCol < 0
            ||
            newCol >= BW ) {
            continue;
            }

        targetP = inState->grid[ newRow ][ newCol ];

        if( targetP == noPiece ) {
            continue;
            }

        if( ( targetP & CHESS_COLOR_MASK ) == otherColor ) {

            /* can capture this piece diagonally */
            
            outDestRows[n] = newRow;
            outDestCols[n] = (unsigned char)newCol;

            /* copy state to start with */
            outStates[n]   = *inState;

            /* copy piece into new spot
               (overwrite captured piece) */
            outStates[n].grid    [ newRow     ][ newCol     ] =
                outStates[n].grid[ inPieceRow ][ inPieceCol ];

            /* leave empty space behind */
            outStates[n].grid[ inPieceRow ][ inPieceCol ] = noPiece;

            outStates[n].nextToMove = otherColor;

            /* fixme:
               promote to Queen in final row */
            
            n++;
            }
        
        }
    
    return n;
    }



static int bishopMove( BoardState     *inState,
                       unsigned char   inPieceColor,
                       int             inPieceRow,
                       int             inPieceCol,
                       unsigned char   outDestRows[BN],
                       unsigned char   outDestCols[BN],
                       BoardState      outStates  [BN] ) {

    
    static  int  dirs[4][2] = { { -1, -1 },
                                {  1, -1 },
                                {  1,  1 },
                                { -1,  1 } };

    int  otherColor    =  CHESS_WHITE;
    int  n             =  0;
    int  d;
    int  i;
    int  maxDiagDist;

    
        
    if( inPieceColor == CHESS_WHITE ) {
        otherColor =  CHESS_BLACK;
        }

    maxDiagDist = BH - 1;

    if( BW - 1 < maxDiagDist ) {
        /* non-square board, diags shorter than longest side */
        maxDiagDist = BW - 1;
        }


    for( d = 0;
         d < 4;
         d ++ ) {

        int  dY  =  dirs[d][0];
        int  dX  =  dirs[d][1];
        
        for( i = 1;
             i <= maxDiagDist;
             i ++ ) {

            int         destY  = dY * i + inPieceRow;
            int         destX  = dX * i + inPieceCol;
            ChessPiece  destP;
            
            /* watch for out of bounds,
               stop pushing in this diag direction */
            if( destY < 0
                ||
                destY >= BH ) {
                break;
                }
            if( destX < 0
                ||
                destX >= BW ) {
                break;
                }

            destP = inState->grid[ destY ][ destX ];
            

            if( destP == noPiece
                ||
                ( destP & CHESS_COLOR_MASK ) == otherColor ) {
                
                /* empty spot, or capturable piece, bishop can move here */

                outDestRows[n] = (unsigned char)destY;
                outDestCols[n] = (unsigned char)destX;

                /* copy state to start with */
                outStates[n]   = *inState;

                /* copy piece into new spot */
                outStates[n].grid    [ destY      ][ destX      ] =
                    outStates[n].grid[ inPieceRow ][ inPieceCol ];

                /* leave empty space behind */
                outStates[n].grid[ inPieceRow ][ inPieceCol ] = noPiece;

                outStates[n].nextToMove = otherColor;

                n++;

                if( destP != noPiece ) {
                    /* can't move to spots on this diag
                       beyond capturable piece */
                    break;
                    }
                }
            else {
                /* a blocking piece on this diag */
                break;
                }
            }
        }
    
    return n;
    }



static int knightMove( BoardState     *inState,
                       unsigned char   inPieceColor,
                       int             inPieceRow,
                       int             inPieceCol,
                       unsigned char   outDestRows[BN],
                       unsigned char   outDestCols[BN],
                       BoardState      outStates  [BN] ) {

    
    static  int  dirs[8][2] = { { -1,  2 },
                                { -2, -1 },
                                { -2,  1 },
                                { -1,  2 },
                                
                                {  1,  2 },
                                {  2,  1 },
                                {  2, -1 },
                                {  1, -2 } };

    int  otherColor  =  CHESS_WHITE;
    int  n           =  0;
    int  d;
        
    if( inPieceColor == CHESS_WHITE ) {
        otherColor =  CHESS_BLACK;
        }

    for( d = 0;
         d < 8;
         d ++ ) {

        int         destY   =  dirs[d][0] + inPieceRow;
        int         destX   =  dirs[d][1] + inPieceCol;
        ChessPiece  destP;
            
        /* watch for out of bounds
           skip these and move on to other options */
        if( destY < 0
            ||
            destY >= BH ) {
            continue;
            }
        if( destX < 0
            ||
            destX >= BW ) {
            continue;
            }

        destP = inState->grid[ destY ][ destX ];
            

        if( destP == noPiece
            ||
            ( destP & CHESS_COLOR_MASK ) == otherColor ) {
                
            /* empty spot, or capturable piece, knight can move here */

            outDestRows[n] = (unsigned char)destY;
            outDestCols[n] = (unsigned char)destX;

            /* copy state to start with */
            outStates[n]   = *inState;

            /* copy piece into new spot */
            outStates[n].grid    [ destY      ][ destX      ] =
                outStates[n].grid[ inPieceRow ][ inPieceCol ];

            /* leave empty space behind */
            outStates[n].grid[ inPieceRow ][ inPieceCol ] = noPiece;

            outStates[n].nextToMove = otherColor;

            n++;
            }
        }
    
    return n;
    }



static int rookMove( BoardState     *inState,
                     unsigned char   inPieceColor,
                     int             inPieceRow,
                     int             inPieceCol,
                     unsigned char   outDestRows[BN],
                     unsigned char   outDestCols[BN],
                     BoardState      outStates  [BN] ) {

    
    static  int  dirs[4][2] = { { -1,  0 },
                                {  1,  0 },
                                {  0, -1 },
                                {  0,  1 } };

    int  otherColor  =  CHESS_WHITE;
    int  n           =  0;
    int  d;
    int  i;
    int  maxDist;

    
        
    if( inPieceColor == CHESS_WHITE ) {
        otherColor =  CHESS_BLACK;
        }

    maxDist = BH - 1;

    if( BW - 1 > maxDist ) {
        /* non-square board, consider moves as long as longest side */
        maxDist = BW - 1;
        }


    for( d = 0;
         d < 4;
         d ++ ) {

        int  dY  =  dirs[d][0];
        int  dX  =  dirs[d][1];
        
        for( i = 1;
             i <= maxDist;
             i ++ ) {

            int         destY  = dY * i + inPieceRow;
            int         destX  = dX * i + inPieceCol;
            ChessPiece  destP;
            
            /* watch for out of bounds,
               stop pushing in this diag direction */
            if( destY < 0
                ||
                destY >= BH ) {
                break;
                }
            if( destX < 0
                ||
                destX >= BW ) {
                break;
                }

            destP = inState->grid[ destY ][ destX ];
            

            if( destP == noPiece
                ||
                ( destP & CHESS_COLOR_MASK ) == otherColor ) {
                
                /* empty spot, or capturable piece, rook can move here */

                outDestRows[n] = (unsigned char)destY;
                outDestCols[n] = (unsigned char)destX;

                /* copy state to start with */
                outStates[n]   = *inState;

                /* copy piece into new spot */
                outStates[n].grid    [ destY      ][ destX      ] =
                    outStates[n].grid[ inPieceRow ][ inPieceCol ];

                /* leave empty space behind */
                outStates[n].grid[ inPieceRow ][ inPieceCol ] = noPiece;

                outStates[n].nextToMove = otherColor;
                
                n++;

                if( destP != noPiece ) {
                    /* can't move to spots on this move dir
                       beyond capturable piece */
                    break;
                    }
                }
            else {
                /* a blocking piece on this move dir */
                break;
                }
            }
        }
    
    return n;
    }



static int queenMove( BoardState     *inState,
                      unsigned char   inPieceColor,
                      int             inPieceRow,
                      int             inPieceCol,
                      unsigned char   outDestRows[BN],
                      unsigned char   outDestCols[BN],
                      BoardState      outStates  [BN] ) {

    
    static  int  dirs[8][2] = { { -1,  0 },
                                {  1,  0 },
                                {  0, -1 },
                                {  0,  1 },
                                { -1, -1 },
                                {  1, -1 },
                                {  1,  1 },
                                { -1,  1 }};

    int  otherColor  =  CHESS_WHITE;
    int  n           =  0;
    int  d;
    int  i;
    int  maxDist;
    
        
    if( inPieceColor == CHESS_WHITE ) {
        otherColor =  CHESS_BLACK;
        }

    maxDist = BH - 1;

    if( BW - 1 > maxDist ) {
        /* non-square board, consider moves as long as longest side */
        maxDist = BW - 1;
        }


    for( d = 0;
         d < 8;
         d ++ ) {

        int  dY  =  dirs[d][0];
        int  dX  =  dirs[d][1];
        
        for( i = 1;
             i <= maxDist;
             i ++ ) {

            int         destY  = dY * i + inPieceRow;
            int         destX  = dX * i + inPieceCol;
            ChessPiece  destP;
            
            /* watch for out of bounds,
               stop pushing in this diag direction */
            if( destY < 0
                ||
                destY >= BH ) {
                break;
                }
            if( destX < 0
                ||
                destX >= BW ) {
                break;
                }

            destP = inState->grid[ destY ][ destX ];
            

            if( destP == noPiece
                ||
                ( destP & CHESS_COLOR_MASK ) == otherColor ) {
                
                /* empty spot, or capturable piece, queen can move here */

                outDestRows[n] = (unsigned char)destY;
                outDestCols[n] = (unsigned char)destX;

                /* copy state to start with */
                outStates[n]   = *inState;

                /* copy piece into new spot */
                outStates[n].grid    [ destY      ][ destX      ] =
                    outStates[n].grid[ inPieceRow ][ inPieceCol ];

                /* leave empty space behind */
                outStates[n].grid[ inPieceRow ][ inPieceCol ] = noPiece;

                outStates[n].nextToMove = otherColor;

                n++;

                if( destP != noPiece ) {
                    /* can't move to spots on this move dir
                       beyond capturable piece */
                    break;
                    }
                }
            else {
                /* a blocking piece on this move dir */
                break;
                }
            }
        }
    
    return n;
    }


/* fixme:
   king should never move into check */
static int kingMove( BoardState     *inState,
                     unsigned char   inPieceColor,
                     int             inPieceRow,
                     int             inPieceCol,
                     unsigned char   outDestRows[BN],
                     unsigned char   outDestCols[BN],
                     BoardState      outStates  [BN] ) {

    
    static  int  dirs[8][2] = { { -1,  0 },
                                {  1,  0 },
                                {  0, -1 },
                                {  0,  1 },
                                { -1, -1 },
                                {  1, -1 },
                                {  1,  1 },
                                { -1,  1 }};

    int  otherColor  =  CHESS_WHITE;
    int  n           =  0;
    int  d;
        
    if( inPieceColor == CHESS_WHITE ) {
        otherColor =  CHESS_BLACK;
        }

    for( d = 0;
         d < 8;
         d ++ ) {

        int         destY  = dirs[d][0] + inPieceRow;
        int         destX  = dirs[d][1] + inPieceCol;
        ChessPiece  destP;
            
        /* watch for out of bounds,
           continue checking other options */
        if( destY < 0
            ||
            destY >= BH ) {
            continue;
            }
        if( destX < 0
            ||
            destX >= BW ) {
            continue;
            }

        destP = inState->grid[ destY ][ destX ];
            

        if( destP == noPiece
            ||
            ( destP & CHESS_COLOR_MASK ) == otherColor ) {
                
            /* empty spot, or capturable piece, king can move here */

            outDestRows[n] = (unsigned char)destY;
            outDestCols[n] = (unsigned char)destX;

            /* copy state to start with */
            outStates[n]   = *inState;

            /* copy piece into new spot */
            outStates[n].grid    [ destY      ][ destX      ] =
                outStates[n].grid[ inPieceRow ][ inPieceCol ];

            /* leave empty space behind */
            outStates[n].grid[ inPieceRow ][ inPieceCol ] = noPiece;

            outStates[n].nextToMove = otherColor;

            n++;
            }
        }
    
    return n;
    }



static PieceMoveFunction moveFunctions[ NUM_CHESS_PIECES ] = { noPieceMove,
                                                               pawnMove,
                                                               bishopMove,
                                                               knightMove,
                                                               rookMove,
                                                               queenMove,
                                                               kingMove };

static char doesKingExist( BoardState  *inState,
                           int          inKingColor ) {
    int  y;
    int  x;

    for( y = 0;
         y < BH;
         y ++ ) {
        
        for( x = 0;
             x < BW;
             x ++ ) {

            ChessPiece  p  = inState->grid[ y ][ x ];

            if( ( p & CHESS_TYPE_MASK ) == king
                &&
                ( p & CHESS_COLOR_MASK ) == inKingColor ) {
                return 1;
                }
            }
        }
    return 0;
    }



static char isKingInCheck( BoardState  *inState,
                           int          inVictimKingColor ) {
    /* fixme */

    int  y;
    int  x;
    
    static  BoardState     resultStates[BN];
    static  unsigned char  destRows    [BN];
    static  unsigned char  destCols    [BN];


    if( ! doesKingExist( inState,
                         inVictimKingColor ) ) {
        /* no victim king present at all, he can't be in check
           this will usually only happen during recursive in-check
           tests where we let a move play out to see if the king is taken */
        return 0;
        }

    /* victim king is present

       Now test all possible moves to see if any move captures him

       If so, he's currently in check (one more move would capture him */
    
    for( y = 0;
         y < BH;
         y ++ ) {

        for( x = 0;
             x < BW;
             x ++ ) {
            
    
            ChessPiece  p       =  inState->grid[ y ][ x ];
            ChessPiece  pType   =  p & CHESS_TYPE_MASK;
            ChessPiece  pColor  =  p & CHESS_COLOR_MASK;
            int         n;
            int         i;

            /* check if any resulting states of moving this piece
               result in the king being captured */
            
            n = moveFunctions[ pType ]( inState,
                                        pColor,
                                        y,
                                        x,
                                        destRows,
                                        destCols,
                                        resultStates );
            for( i = 0;
                 i < n;
                 i ++ ) {

                if( ! doesKingExist( &( resultStates[i] ),
                                     inVictimKingColor ) ) {
                    /* king was present, but gone one move later
                       he was in check */

                    return 1;
                    }
                }
            }
        }

    /* all possible single moves lead to states where victim
       king is still present (not captured) */
    
    return 0;
    }


    


static  MaxiginRand  chessRand;


void chessInit( void ) {
    maxigin_randSeed( &chessRand,
                      12453597 );
    }
     


void getStartBoard( BoardState  *outState ) {

    int  x;
    int  y;
    int  i;
    
    for( y = 0;
         y < BH;
         y ++ ) {
        
        for( x = 0;
             x < BW;
             x ++ ) {

            outState->grid[y][x] = noPiece;
            }
        }
    

    /* fill out whole starting board */
    outState->grid[0][0] = rook   | CHESS_BLACK;
    outState->grid[0][1] = knight | CHESS_BLACK;
    outState->grid[0][2] = bishop | CHESS_BLACK;
    outState->grid[0][3] = queen  | CHESS_BLACK;
    outState->grid[0][4] = king   | CHESS_BLACK;
    outState->grid[0][5] = bishop | CHESS_BLACK;
    outState->grid[0][6] = knight | CHESS_BLACK;
    outState->grid[0][7] = rook   | CHESS_BLACK;

    for( i = 0;
         i < 8;
         i ++ ) {
        outState->grid[1][i] = pawn | CHESS_BLACK;
        }

    outState->grid[7][0] = rook   | CHESS_WHITE;
    outState->grid[7][1] = knight | CHESS_WHITE;
    outState->grid[7][2] = bishop | CHESS_WHITE;
    outState->grid[7][3] = queen  | CHESS_WHITE;
    outState->grid[7][4] = king   | CHESS_WHITE;
    outState->grid[7][5] = bishop | CHESS_WHITE;
    outState->grid[7][6] = knight | CHESS_WHITE;
    outState->grid[7][7] = rook   | CHESS_WHITE;

    for( i = 0;
         i < 8;
         i ++ ) {
        outState->grid[6][i] = pawn | CHESS_WHITE;
        }

    outState->nextToMove = CHESS_WHITE;
    }



static int getPiecePossibleMoves( BoardState     *inState,
                                  int             inPieceRow,
                                  int             inPieceCol,
                                  unsigned char   outRows[BN],
                                  unsigned char   outCols[BN] ) {

    /* for now, we don't do anything with the result states *
       FIXME */
    static  BoardState     resultStates[BN];
    static  unsigned char  resultRows  [BN];
    static  unsigned char  resultCols  [BN];
    
    ChessPiece  p             =  inState->grid[ inPieceRow ][ inPieceCol ];
    ChessPiece  pType         =  p & CHESS_TYPE_MASK;
    ChessPiece  pColor        =  p & CHESS_COLOR_MASK;
    int         numMoves;
    int         numGoodMoves  =  0;
    int         m;
    
        
    numMoves = moveFunctions[ pType ]( inState,
                                       pColor,
                                       inPieceRow,
                                       inPieceCol,
                                       resultRows,
                                       resultCols,
                                       resultStates );

    /* filter moves to remove illegal moves that put our king in check */
    for( m = 0;
         m < numMoves;
         m ++ ) {

        if( ! isKingInCheck( &( resultStates[m] ),
                             pColor ) ) {

            outRows[ numGoodMoves ] = resultRows[ m ];
            outCols[ numGoodMoves ] = resultCols[ m ];

            numGoodMoves ++;
            }
        }

    return numGoodMoves;   
    }



char getRandomMove( BoardState  *inState,
                    Move        *outMove,
                    char         inWhiteTurn ) {

    /* fixme:  pay attention to limits on where piece can actually move */

    static  unsigned char  possiblePieceRow[BN];
    static  unsigned char  possiblePieceCol[BN];
    static  unsigned char  possibleDestRow [BN];
    static  unsigned char  possibleDestCol [BN];

    int             numPossiblePieces = 0;
    int             piecePick;
    int             p;
    unsigned char   x;
    unsigned char   y;
    int            *shuffle;
    
    
    for( y = 0;
         y < BH;
         y ++ ) {
        
        for( x = 0;
             x < BW;
             x ++ ) {

            if( inState->grid[y][x] != noPiece ) {

                if( ( inWhiteTurn
                      &&
                      ( inState->grid[y][x] & 0x80 ) == CHESS_WHITE )
                    ||
                    ( ! inWhiteTurn
                      &&
                      ( inState->grid[y][x] & 0x80 ) == CHESS_BLACK ) ) {
                    
                    possiblePieceRow[numPossiblePieces] = y;
                    possiblePieceCol[numPossiblePieces] = x;
                    numPossiblePieces ++;
                    }
                }
            }
        }

    if( numPossiblePieces == 0 ) {
        return 0;
        }


    /* fixme:
       this is just a truly random move for testing */

    /* try picking a piece and try moving it 10 times before giving up */

    shuffle = maxigin_genShuffle( &chessRand,
                                  0,
                                  numPossiblePieces - 1 );
    for( p = 0;
         p < numPossiblePieces;
         p ++ ) {

        int  numMoves;
        int  m;
        
        piecePick = shuffle[ p ];
        
        y = possiblePieceRow[ piecePick ];
        x = possiblePieceCol[ piecePick ];

        numMoves = getPiecePossibleMoves( inState,
                                          y,
                                          x,
                                          possibleDestRow,
                                          possibleDestCol );

        if( numMoves > 0 ) {
            m = maxigin_randRange( &chessRand,
                                   0,
                                   numMoves - 1 );
            outMove->startPos[0] = y;
            outMove->startPos[1] = x;
    
            outMove->endPos  [0] = possibleDestRow[m];
            outMove->endPos  [1] = possibleDestCol[m];

            return 1;
            }
        }
       
    
    /* tried all possible pieces, none could move*/
    return 0;
    }



void applyMove( BoardState  *inState,
                Move        *inMove ) {
    inState->grid[ inMove->endPos[0] ][ inMove->endPos[1] ]
        = inState->grid[ inMove->startPos[0] ][ inMove->startPos[1] ];

    /* leave empty square behind */
    inState->grid[ inMove->startPos[0] ][ inMove->startPos[1] ] =
        noPiece;
    }


#endif


#endif
