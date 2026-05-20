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

#include "memoryRegister.h"


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
    laserRook,
    laserPawn,
    doublingPawn,
    addingRook,
    rocket,
    NUM_CHESS_PIECES };

#define  FIRST_CHESS_PIECE  pawn





    
    

/* store color in 8th bit.  This gives us 127 unique pieces in both colors */
#define  CHESS_TYPE_MASK    0x7F
#define  CHESS_COLOR_MASK   0x80
#define  CHESS_WHITE        0x00
#define  CHESS_BLACK        0x80

/* rows, columns, and num square */
#define  BH                 8
#define  BW                 8
#define  BMAX               8    /* for non-rect boards, max dimension */
#define  BN                 ( BH * BW )

typedef struct BoardState{
        
        ChessPiece  grid[BH][BW];

        int         nextToMove;

        int         moveCount;

        /* can index with  (piece >> 7)  or  (pieceColor >> 7) */
        char        kingExists[2];

    } BoardState;



typedef struct Move{

        /* in row, col order */
        unsigned char  startPos[2];
        unsigned char  endPos  [2];
        
    } Move;



typedef struct BoardPiece {
        
        ChessPiece  p;
        
        int         row;
        
        int         col;
        
    } BoardPiece;



typedef struct Captured {

        /* number of pieces captured */
        int        num;
        
        /* list of up to BN pieces, with only the first num entries populated */
        BoardPiece pieces[ BN ];
        
    } Captured;



/* for effects on board spaces */
typedef  unsigned char  SpaceEffect;


enum{
    noEffect = 0,
    add,
    multiply,
    NUM_SPACE_EFFECTS };


#define  FIRST_SPACE_EFFECT  multiply



/* each space on the board can be possibly affected by all pieces
   on the board.
*/
typedef struct ActiveSpaceEffects {

        int               num;
        SpaceEffect       effectType [ BN ];
        int               effectValue[ BN ];
        /* where on the board each effect came from */
        int               sourceRow  [ BN ];
        int               sourceCol  [ BN ];
    } ActiveSpaceEffects;



typedef struct FullBoardSpaceEffects {
        
        ActiveSpaceEffects grid[ BH ][ BW ];
        
    } FullBoardSpaceEffects;




typedef struct TotalSpaceEffects {
        
        int  effectValue[ NUM_SPACE_EFFECTS ];
        
    } TotalSpaceEffects;
        
    

/* gets the per-square, non-compounded space effects for the whole board */
void getSpaceEffects( BoardState             *inState,
                      int                     inAffectedColor,
                      FullBoardSpaceEffects  *outEffects );


/* computes the final compounded space effects for a specific spot on the board
   
   Updates inOutEffects, compounding all squares that affect the target
   space, while leaving other squares untouched.
 */
void compoundSpaceEffects( int                     inTargetRow,
                           int                     inTargetCol,
                           FullBoardSpaceEffects  *inOutEffects );


/* computes the final total space effects for a specific spot on the board.

   inEffects must be compounded for target space before calling this function.

   Leaves inEffects untouched.

   Puts totals into outTotals
 */
void getTotalSpaceEffects( int                     inTargetRow,
                           int                     inTargetCol,
                           FullBoardSpaceEffects  *inEffects,
                           TotalSpaceEffects      *outTotals );

        
        
        




void chessInit( void );


void chessSeed( unsigned long  inSeed );




/* fills outState with the starting board state */
void getStartBoard( BoardState  *outState );



/* fills outState with a test board state */
void getTestBoard( BoardState  *outState );



/* returns 1 if move possible, 0 if not */
char getRandomMove( BoardState  *inState,
                    char         inAvoidCheck,
                    Move        *outMove,
                    Captured    *outCaptured,
                    BoardState  *outNewState );



/* if a capture is possible, makes move that performs most valuable capture
   if not, makes a random move */
char getGreedyMove( BoardState  *inState,
                    Move        *outMove,
                    Captured    *outCaptured,
                    BoardState  *outNewState );



/* makes a greedy move 3/4 of the time and random move 1/4 of the time */
char getMixedMove( BoardState  *inState,
                   Move        *outMove,
                   Captured    *outCaptured,
                   BoardState  *outNewState );



/* decides internally what type of move to generate */
char getChessMove( BoardState  *inState,
                   Move        *outMove,
                   Captured    *outCaptured,
                   BoardState  *outNewState );



/* updates inState to reflect move described by inMove and inNewState */
void applyMove( BoardState  *inState,
                Move        *inMove,
                BoardState  *inNewState );



/* gets board state as a nicely-formatted text table
   returns a string from a pool of 4 statically allocated buffers,
   round-robin */
const char *getBoardStateString( BoardState  *inState );



/* gets the score for a board state, where it's good for white if the
   score is positive, and good for black if the score is negative */
int getScore( BoardState  *inState );


/* returns 1 if one side is checkmated and sets
   outColor to the losing side */
char isCheckmate( BoardState  *inState,
                  int         *outColor );


/* gets the number of board states that were considered when picking
   the last move */
int getStateCountLastMove( void );


/* returns 1 on success, 0 if inLogNumber is beyond end of log */
char getLoggedState( int          inLogNumber,
                     BoardState  *outState,
                     int         *outBestScore );




#ifdef CHESS_IMPLEMENTATION

#include "arraySizeCheck.h"
#include "maxigin.h"



static  MaxiginRand  chessRand;



static int pieceValue[] = { 0,
                            1,
                            3,
                            3,
                            5,
                            9,
                            999,
                            7,
                            2,
                            2,
                            6,
                            2 };

CHECK_ARRAY_LENGTH( pieceValue,
                    NUM_CHESS_PIECES );


static char pieceChars[] = { '+',
                             'p',
                             'b',
                             'n',
                             'r',
                             'q',
                             'k',
                             'l',
                             'o',
                             'd',
                             'a',
                             'v' };

CHECK_ARRAY_LENGTH( pieceChars,
                    NUM_CHESS_PIECES );


static  int  statesTestedLastMove  =  0;



/* our new semantics above mark "checkmate" as king being captured
   this function detects the traditional definition, where the king
   is in check and there's no move that resolves this */
char isForcedCheckmate( BoardState  *inState,
                        int         *outColor );



/* the signature for a piece move function.
   We define one of these for each piece type in the enum above.

   These functions assume that a piece of the correct type is at inPieceRow
   and inPieceCol (so the function doesn't have to check this.

   These functions just look for legal moves for a given piece, in isolation,
   without considering rules around a king in check.

   If inMaySkipNonKingCaptureMoves is 1, then the move function MAY skip
   returning any moves that don't involve capturing a king, to reduce
   computation.
   This flag is advisory only, and if it's actually more complicated for a
   specific piece to skip non-king-capture moves, it can include them.  The
   whole point of this call is to reduce computation when possible, for
   calls to this function that don't care about non-king-capture moves (such as
   king-in-check detection)

   Returns the number of moves.
*/
typedef int (*PieceMoveFunction)( BoardState     *inState,
                                  /* color of the piece */
                                  unsigned char   inPieceColor,
                                  /* starting position of the piece */
                                  int             inPieceRow,
                                  int             inPieceCol,
                                  char            inMaySkipNonKingCaptureMoves,
                                  /* possible move rows and cols */
                                  unsigned char   outDestRows[BN],
                                  unsigned char   outDestCols[BN],
                                  /* resulting piece capture lists
                                     from possible moves */
                                  Captured        outCaptured[BN],
                                  /* resulting board states from possible
                                     moves */
                                  BoardState      outStates  [BN] );



static char isKingInCheck( BoardState  *inState,
                           int          inVictimKingColor );


static char isKingInCheckGetMove( BoardState  *inState,
                                  int          inVictimKingColor,
                                  int         *outKingX,
                                  int         *outKingY,
                                  Move        *outMove );



static int noPieceMove( BoardState     *inState,
                        unsigned char   inPieceColor,
                        int             inPieceRow,
                        int             inPieceCol,
                        char            inMaySkipNonKingCaptureMoves,
                        unsigned char   outDestRows[BN],
                        unsigned char   outDestCols[BN],
                        Captured        outCaptured[BN],
                        BoardState      outStates  [BN] ) {
    
    /* suppress warnings for unused params */
    (void)inState;
    (void)inPieceColor;
    (void)inPieceRow;
    (void)inPieceCol;
    (void)inMaySkipNonKingCaptureMoves;
    (void)outDestRows;
    (void)outDestCols;
    (void)outCaptured;
    (void)outStates;
    
    return 0;
    }



static void addCapturedPiece( Captured    *inCaptured,
                              BoardState  *inState,
                              int          inRow,
                              int          inCol ) {

    BoardPiece  *bp  =  &( inCaptured->pieces[ inCaptured->num ] );

    bp->p   = inState->grid[ inRow ][ inCol ];
    bp->row = inRow;
    bp->col = inCol;

    if( ( bp->p & CHESS_TYPE_MASK ) == king ) {

        inState->kingExists[ ( bp->p ) >> 7 ] = 0;
        }

    inCaptured->num ++;
    }



/* ignores en passant rules */
static int pawnMove( BoardState     *inState,
                     unsigned char   inPieceColor,
                     int             inPieceRow,
                     int             inPieceCol,
                     char            inMaySkipNonKingCaptureMoves,
                     unsigned char   outDestRows[BN],
                     unsigned char   outDestCols[BN],
                     Captured        outCaptured[BN],
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
    if( ! inMaySkipNonKingCaptureMoves )
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

            /* no capture */
            outCaptured[n].num = 0;
            
            /* promote to Queen in final row */

            if( ( moveDir == 1
                  &&
                  newRow == BH - 1 )
                ||
                ( moveDir == -1
                  &&
                  newRow == 0 ) ) {

                /* count our pawn as captured during promotion */
                addCapturedPiece( &( outCaptured[n] ),
                                  &( outStates[n]   ),
                                  newRow,
                                  inPieceCol );
                
                outStates[n].grid[ newRow ][ inPieceCol ] = inPieceColor | queen;
                }
            
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

        if( inMaySkipNonKingCaptureMoves
            &&
            ( targetP & CHESS_TYPE_MASK ) != king ) {
            continue;
            }

        if( ( targetP & CHESS_COLOR_MASK ) == otherColor ) {

            /* can capture this piece diagonally */
            
            outDestRows[n] = newRow;
            outDestCols[n] = (unsigned char)newCol;

            /* copy state to start with */
            outStates[n]   = *inState;

            /* add to captured list */
            outCaptured[n].num = 0;
            addCapturedPiece( &( outCaptured[n] ),
                              &( outStates[n]   ),
                              newRow,
                              newCol );

            /* copy piece into new spot
               (overwrite captured piece) */
            outStates[n].grid    [ newRow     ][ newCol     ] =
                outStates[n].grid[ inPieceRow ][ inPieceCol ];

            /* leave empty space behind */
            outStates[n].grid[ inPieceRow ][ inPieceCol ] = noPiece;

            outStates[n].nextToMove = otherColor;

            /* promote to Queen in final row */

            if( ( moveDir == 1
                  &&
                  newRow == BH - 1 )
                ||
                ( moveDir == -1
                  &&
                  newRow == 0 ) ) {

                /* count our pawn as captured during promotion */
                addCapturedPiece( &( outCaptured[n] ),
                                  &( outStates[n]   ),
                                  newRow,
                                  inPieceCol );
                
                outStates[n].grid[ newRow ][ newCol ] = inPieceColor | queen;
                }
            
            n++;
            }
        
        }
    
    return n;
    }



static int bishopMove( BoardState     *inState,
                       unsigned char   inPieceColor,
                       int             inPieceRow,
                       int             inPieceCol,
                       char            inMaySkipNonKingCaptureMoves,
                       unsigned char   outDestRows[BN],
                       unsigned char   outDestCols[BN],
                       Captured        outCaptured[BN],
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

                if( inMaySkipNonKingCaptureMoves
                    &&
                    ( destP & CHESS_TYPE_MASK ) != king ) {
                    if( destP == noPiece ) {
                        continue;
                        }
                    else {
                        break;
                        }
                    }

                outDestRows[n] = (unsigned char)destY;
                outDestCols[n] = (unsigned char)destX;

                /* copy state to start with */
                outStates[n]   = *inState;

                outCaptured[n].num = 0;
                
                if( destP != noPiece ) {
                    /* add to captured list */
                    addCapturedPiece( &( outCaptured[n] ),
                                      &( outStates[n]   ),
                                      destY,
                                      destX );
                    }

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
                       char            inMaySkipNonKingCaptureMoves,
                       unsigned char   outDestRows[BN],
                       unsigned char   outDestCols[BN],
                       Captured        outCaptured[BN],
                       BoardState      outStates  [BN] ) {

    
    static  int  dirs[8][2] = { { -1,  2 },
                                { -2, -1 },
                                { -2,  1 },
                                { -1, -2 },
                                
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

            if( inMaySkipNonKingCaptureMoves
                &&
                ( destP & CHESS_TYPE_MASK ) != king ) {

                continue;
                }
            
            outDestRows[n] = (unsigned char)destY;
            outDestCols[n] = (unsigned char)destX;

            /* copy state to start with */
            outStates[n]   = *inState;

            outCaptured[n].num = 0;
            
            if( destP != noPiece ) {
                /* add to captured list */
                addCapturedPiece( &( outCaptured[n] ),
                                  &( outStates[n]   ),
                                  destY,
                                  destX );
                }
            
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
                     char            inMaySkipNonKingCaptureMoves,
                     unsigned char   outDestRows[BN],
                     unsigned char   outDestCols[BN],
                     Captured        outCaptured[BN],
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

                if( inMaySkipNonKingCaptureMoves
                    &&
                    ( destP & CHESS_TYPE_MASK ) != king ) {
                    if( destP == noPiece ) {
                        continue;
                        }
                    else {
                        break;
                        }
                    }
                
                outDestRows[n] = (unsigned char)destY;
                outDestCols[n] = (unsigned char)destX;

                /* copy state to start with */
                outStates[n]   = *inState;

                outCaptured[n].num = 0;
            
                if( destP != noPiece ) {
                    /* add to captured list */
                    addCapturedPiece( &( outCaptured[n] ),
                                      &( outStates[n]   ),
                                      destY,
                                      destX );
                    }

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
                      char            inMaySkipNonKingCaptureMoves,
                      unsigned char   outDestRows[BN],
                      unsigned char   outDestCols[BN],
                      Captured        outCaptured[BN],
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

                if( inMaySkipNonKingCaptureMoves
                    &&
                    ( destP & CHESS_TYPE_MASK ) != king ) {
                    if( destP == noPiece ) {
                        continue;
                        }
                    else {
                        break;
                        }
                    }
                
                outDestRows[n] = (unsigned char)destY;
                outDestCols[n] = (unsigned char)destX;

                /* copy state to start with */
                outStates[n]   = *inState;

                outCaptured[n].num = 0;
            
                if( destP != noPiece ) {
                    /* add to captured list */
                    addCapturedPiece( &( outCaptured[n] ),
                                      &( outStates[n]   ),
                                      destY,
                                      destX );
                    }

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



static int kingMove( BoardState     *inState,
                     unsigned char   inPieceColor,
                     int             inPieceRow,
                     int             inPieceCol,
                     char            inMaySkipNonKingCaptureMoves,
                     unsigned char   outDestRows[BN],
                     unsigned char   outDestCols[BN],
                     Captured        outCaptured[BN],
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

            if( inMaySkipNonKingCaptureMoves
                &&
                ( destP & CHESS_TYPE_MASK ) != king ) {
                continue;
                }
            
            outDestRows[n] = (unsigned char)destY;
            outDestCols[n] = (unsigned char)destX;

            /* copy state to start with */
            outStates[n]   = *inState;

            outCaptured[n].num = 0;
            
            if( destP != noPiece ) {
                /* add to captured list */
                addCapturedPiece( &( outCaptured[n] ),
                                  &( outStates[n]   ),
                                  destY,
                                  destX );
                }

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



/* returns pointer to statically allocated effects totals */
static int getTotalEffectsRepeatValue( BoardState  *inState,
                                       int          inAffectedColor,
                                       int          inPieceRow,
                                       int          inPieceCol ) {

    static  FullBoardSpaceEffects  effects;
    static  TotalSpaceEffects      totals;
    
    getSpaceEffects( inState,
                     inAffectedColor,
                     &effects );

    compoundSpaceEffects( inPieceRow,
                          inPieceCol,
                          &effects );

    getTotalSpaceEffects( inPieceRow,
                          inPieceCol,
                          &effects,
                          &totals );

    if( totals.effectValue[ multiply ] > 0 ) {
        return totals.effectValue[ multiply ];
        }
    else {
        return 1;
        }
    }




/* same as rook move, but fires 4-way lasers at end */
static int laserRookMove( BoardState     *inState,
                          unsigned char   inPieceColor,
                          int             inPieceRow,
                          int             inPieceCol,
                          char            inMaySkipNonKingCaptureMoves,
                          unsigned char   outDestRows[BN],
                          unsigned char   outDestCols[BN],
                          Captured        outCaptured[BN],
                          BoardState      outStates  [BN] ) {

    static  int  dirs[4][2] = { { -1,  0 },
                                {  1,  0 },
                                {  0, -1 },
                                {  0,  1 } };

    int  maxDist   =  BH - 1;
    int  i;
    int  numMoves  =  rookMove( inState,
                                inPieceColor,
                                inPieceRow,
                                inPieceCol,
                                /* do NOT skip non-capture moves in rook call,
                                   since then we'll miss cases where we
                                   don't capture with move but capture with
                                   laser after */
                                0,
                                outDestRows,
                                outDestCols,
                                outCaptured,
                                outStates );

    (void)inMaySkipNonKingCaptureMoves;
    

    if( BW - 1 > maxDist ) {
        /* non-square board, consider moves as long as longest side */
        maxDist = BW - 1;
        }

    /* fire lasers after moving */

    for( i = 0;
         i < numMoves;
         i ++ ) {
        
        int          r          =  outDestRows[i];
        int          c          =  outDestCols[i];
        BoardState  *s          =  &( outStates[i] );
        int          v;
        int          d;
        int          repeatVal  =  getTotalEffectsRepeatValue( s,
                                                               inPieceColor,
                                                               r,
                                                               c );
        for( v = 0;
             v < repeatVal;
             v++ )
        for( d = 0;
             d < 4;
             d ++ ) {

            int  dist;

            for( dist =  1;
                 dist <= maxDist;
                 dist ++ ) {
                
                int         dy  =  r + dirs[ d ][ 0 ] * dist;
                int         dx  =  c + dirs[ d ][ 1 ] * dist;
                ChessPiece  p;
                
                if( dy < 0
                    ||
                    dy >= BH ) {
                    /* hit top/bottom without finding piece to hit */
                    break;
                    }
                if( dx < 0
                    ||
                    dx >= BW ) {
                    /* hit left/right without finding piece to hit */
                    break;
                    }

                p = s->grid[ dy ][ dx ];
                
                if( p != noPiece ) {

                    if( ( p & CHESS_COLOR_MASK ) == s->nextToMove ) {
                        /* opponent piece */

                        addCapturedPiece( &( outCaptured[i] ),
                                          s,
                                          dy,
                                          dx );
                        
                        /* destroy piece */
                        s->grid[ dy ][ dx ] = noPiece;
                        }
                    /* if it's our piece, we don't destroy it, but
                       stop laser */

                    /* stop looking in dir after first piece hit */
                    break;
                    }
                }

            }

        }

    return numMoves;
    }



/* same as pawn move, but fires 1 laser at end */
static int laserPawnMove( BoardState     *inState,
                          unsigned char   inPieceColor,
                          int             inPieceRow,
                          int             inPieceCol,
                          char            inMaySkipNonKingCaptureMoves,
                          unsigned char   outDestRows[BN],
                          unsigned char   outDestCols[BN],
                          Captured        outCaptured[BN],
                          BoardState      outStates  [BN] ) {

    int  dirY      =  -1;
    int  maxDist   =  BH - 1;
    int  i;
    int  numMoves  =  pawnMove( inState,
                                inPieceColor,
                                inPieceRow,
                                inPieceCol,
                                /* do NOT skip non-capture moves in pawn call,
                                   since then we'll miss cases where we
                                   don't capture with move but capture with
                                   laser after */
                                0,
                                outDestRows,
                                outDestCols,
                                outCaptured,
                                outStates );

    (void)inMaySkipNonKingCaptureMoves;
    

    if( inState->nextToMove == CHESS_BLACK ) {
        dirY = 1;
        }

    /* fire laser after moving */

    for( i = 0;
         i < numMoves;
         i ++ ) {
        
        int          r          =  outDestRows[i];
        int          c          =  outDestCols[i];
        BoardState  *s          =  &( outStates[i] );
        int          dist;
        int          v;
        int          repeatVal  =  getTotalEffectsRepeatValue( s,
                                                               inPieceColor,
                                                               r,
                                                               c );
        for( v = 0;
             v < repeatVal;
             v++ )
        for( dist =  1;
             dist <= maxDist;
             dist ++ ) {
                
            int         dy  =  r + dirY * dist;
            ChessPiece  p;
                
            if( dy < 0
                ||
                dy >= BH ) {
                /* hit top/bottom without finding piece to hit */
                break;
                }

            p = s->grid[ dy ][ c ];
                
            if( p != noPiece ) {

                if( ( p & CHESS_COLOR_MASK ) == s->nextToMove ) {
                    /* opponent piece */

                    addCapturedPiece( &( outCaptured[i] ),
                                      s,
                                      dy,
                                      c );
                        
                    /* destroy piece */
                    s->grid[ dy ][ c ] = noPiece;
                    }
                /* if it's our piece, we don't destroy it, but
                   stop laser */

                /* stop looking in dir after first piece hit */
                break;
                }
            }
        }

    return numMoves;
    }




/* fixme... they just move like pawns for now */
static int rocketMove( BoardState     *inState,
                       unsigned char   inPieceColor,
                       int             inPieceRow,
                       int             inPieceCol,
                       char            inMaySkipNonKingCaptureMoves,
                       unsigned char   outDestRows[BN],
                       unsigned char   outDestCols[BN],
                       Captured        outCaptured[BN],
                       BoardState      outStates  [BN] ) {
    
    int  otherColor    =  CHESS_WHITE;
    int  numEnemy      =  0;
    int  x;
    int  y;
    int  pick;
    int  r;
    int  c;
    
    static  BoardPiece  enemy[BN];

    if( inMaySkipNonKingCaptureMoves ) {
        /* never count a rocket's existence as putting the king in check,
           so when we're testing for check, return no moves */
        return 0;
        }
    
    if( inPieceColor == CHESS_WHITE ) {
        otherColor = CHESS_BLACK;
        }

    for( y = 0;
         y < BH;
         y ++ ) {

        for( x = 0;
             x < BW;
             x ++ ) {

            ChessPiece  p  = inState->grid[y][x];

            if( p != noPiece
                &&
                ( p & CHESS_COLOR_MASK ) == otherColor
                &&
                ( p & CHESS_TYPE_MASK  ) != king ) {

                enemy[numEnemy].p   = p;
                enemy[numEnemy].row = y;
                enemy[numEnemy].col = x;
                numEnemy++;
                } 
            }
        }

    if( numEnemy == 0 ) {
        return 0;
        }
    
    pick = maxigin_randRange( &chessRand,
                              0,
                              numEnemy - 1 );

    r = enemy[pick].row;
    c = enemy[pick].col;

    outDestRows[0] = (unsigned char)r;
    outDestCols[0] = (unsigned char)c;

    /* copy state to start */
    outStates[0] = *inState;

    outCaptured[0].num = 0;
    
    /* count rocked as captured
       maybe don't do this for now... */
    if( 0 )
    addCapturedPiece( &( outCaptured[0] ),
                      inState,
                      inPieceRow,
                      inPieceCol );
    
    /* target is captured too */
    addCapturedPiece( &( outCaptured[0] ),
                      inState,
                      r,
                      c );

    /* clear rocket */
    outStates[0].grid[ inPieceRow ][ inPieceCol ] = noPiece;

    /* destroy target */
    outStates[0].grid[ r ][ c ] = noPiece;

    
    outStates[0].nextToMove = otherColor;

    return 1;
    }



static PieceMoveFunction moveFunctions[] = { noPieceMove,
                                             pawnMove,
                                             bishopMove,
                                             knightMove,
                                             rookMove,
                                             queenMove,
                                             kingMove,
                                             laserRookMove,
                                             laserPawnMove,
                                             pawnMove,
                                             rookMove,
                                             rocketMove };
CHECK_ARRAY_LENGTH( moveFunctions,
                    NUM_CHESS_PIECES );


/* this tells the AI to re-pick a piece's move after game-tree search selects
   a given piece to move.

   Randomized pieces pick a random-move during game-tree search, which
   means the AI will only prefer them to move when they randomly do a really
   good move that increases the score.

   However, we don't only want these pieces making really good moves.

   We want them to make random moves, so we re-pick their move after
   the game tree search is done
*/
static char  repickMoveAtEnd[] = { 0,
                                   0,
                                   0,
                                   0,
                                   0,
                                   0,
                                   0,
                                   0,
                                   0,
                                   0,
                                   0,
                                   1 };

CHECK_ARRAY_LENGTH( repickMoveAtEnd,
                    NUM_CHESS_PIECES );
    


static char doesKingExist( BoardState  *inState,
                           int          inKingColor ) {

    return inState->kingExists[ inKingColor >> 7 ];
    }



static char isKingInCheck( BoardState  *inState,
                           int          inVictimKingColor ) {
    
    return isKingInCheckGetMove( inState,
                                 inVictimKingColor,
                                 0,
                                 0,
                                 0 );
    }



static void findKing( BoardState  *inState,
                      int          inTargetKingColor,
                      int         *outKingX,
                      int         *outKingY ) {

    unsigned char  y;
    unsigned char  x;

    for( y = 0;
         y < BH;
         y ++ ) {

        for( x = 0;
             x < BW;
             x ++ ) {
            
    
            ChessPiece  p       =  inState->grid[ y ][ x ];
            ChessPiece  pType   =  p & CHESS_TYPE_MASK;
            ChessPiece  pColor  =  p & CHESS_COLOR_MASK;

            if( pType  == king
                &&
                pColor == inTargetKingColor ) {
                
                *outKingX = x;
                *outKingY = y;
                
                return;
                }
            }
        }
    }



static char isKingInCheckGetMove( BoardState  *inState,
                                  int          inVictimKingColor,
                                  int         *outKingX,
                                  int         *outKingY,
                                  Move        *outMove ) {

    unsigned char  y;
    unsigned char  x;
    
    static  BoardState     resultStates  [BN];
    static  unsigned char  destRows      [BN];
    static  unsigned char  destCols      [BN];
    static  Captured       resultCaptured[BN];

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

            if( pColor == inVictimKingColor ) {
                /* victim's king's color can't move to capture him, skip */
                continue;
                }

            /* check if any resulting states of moving this piece
               result in the king being captured */
            
            n = moveFunctions[ pType ]( inState,
                                        pColor,
                                        y,
                                        x,
                                        /* may skip non-captures
                                           since we only care about
                                           king capture detection */
                                        1,
                                        destRows,
                                        destCols,
                                        resultCaptured,
                                        resultStates );
            for( i = 0;
                 i < n;
                 i ++ ) {

                if( ! doesKingExist( &( resultStates[i] ),
                                     inVictimKingColor ) ) {
                    /* king was present, but gone one move later
                       he was in check */

                    if( outMove != 0 ) {
                        outMove->startPos[0] = y;
                        outMove->startPos[1] = x;
                        outMove->endPos[0] = destRows[i];
                        outMove->endPos[1] = destCols[i];
                        }
                    if( outKingX != 0
                        &&
                        outKingY != 0 ) {

                        findKing( inState,
                                  inVictimKingColor,
                                  outKingX,
                                  outKingY );
                        
                        }
                    

                    return 1;
                    }
                }
            }
        }

    /* all possible single moves lead to states where victim
       king is still present (not captured) */
    
    return 0;
    }


    





void chessSeed( unsigned long  inSeed ) {
    maxigin_randSeed( &chessRand,
                      inSeed );
    }



void chessInit( void ) {

    /* stalemate */
    chessSeed( 12036704 );

    /* draw */
    if(0)chessSeed( 12035857 );

    
    REGISTER_VAL_MEM( chessRand );
    }



static void clearBoard( BoardState  *outState ) {

    int  x;
    int  y;
    
    for( y = 0;
         y < BH;
         y ++ ) {
        
        for( x = 0;
             x < BW;
             x ++ ) {

            outState->grid[y][x] = noPiece;
            }
        }
    }

    
void getStartBoard( BoardState  *outState ) {

    int  i;
    
    clearBoard( outState );

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

    outState->grid[7][0] = rook      | CHESS_WHITE;
    outState->grid[7][1] = knight    | CHESS_WHITE;
    outState->grid[7][2] = bishop    | CHESS_WHITE;
    outState->grid[7][3] = queen     | CHESS_WHITE;
    outState->grid[7][4] = king      | CHESS_WHITE;
    outState->grid[7][5] = bishop    | CHESS_WHITE;
    outState->grid[7][6] = knight    | CHESS_WHITE;
    outState->grid[7][7] = rook | CHESS_WHITE;

    for( i = 0;
         i < 8;
         i ++ ) {
        outState->grid[6][i] = rocket | CHESS_WHITE;
        }

    outState->grid[6][1] = rocket | CHESS_WHITE;
    outState->grid[6][2] = rocket | CHESS_WHITE;
    
    outState->nextToMove = CHESS_WHITE;
    outState->moveCount  = 0;

    outState->kingExists[0] = 1;
    outState->kingExists[1] = 1;
    }



void getTestBoard( BoardState  *outState ) {

    int  i;

    clearBoard( outState );

    outState->grid[0][4] = king   | CHESS_BLACK;
    outState->grid[1][4] = pawn   | CHESS_BLACK;
    outState->grid[2][4] = pawn   | CHESS_BLACK;
    outState->grid[2][5] = rook  | CHESS_BLACK;
    outState->grid[3][3] = rook  | CHESS_BLACK;
    outState->grid[3][4] = rook  | CHESS_BLACK;
    outState->grid[3][5] = rook  | CHESS_BLACK;
    
    outState->grid[4][0] = laserRook | CHESS_WHITE;
    outState->grid[4][2] = pawn | CHESS_WHITE;
    outState->grid[3][0] = pawn | CHESS_WHITE;
    outState->grid[6][5] = king | CHESS_WHITE;

    
    outState->grid[6][4] = doublingPawn  | CHESS_WHITE;
    outState->grid[5][4] = doublingPawn  | CHESS_WHITE;
    outState->grid[5][5] = addingRook  | CHESS_WHITE;
    outState->grid[5][6] = addingRook  | CHESS_WHITE;


    for( i = 0;
         i < BW;
         i ++ ) {
        outState->grid[7][i] = addingRook  | CHESS_WHITE;
        }
    

    outState->nextToMove = CHESS_WHITE;
    outState->moveCount  = 0;
    outState->kingExists[0] = 1;
    outState->kingExists[1] = 1;
    }

/*
void getTestBoard( BoardState  *outState ) {


    clearBoard( outState );

    outState->grid[0][4] = king   | CHESS_BLACK;
    outState->grid[1][3] = pawn   | CHESS_BLACK;
    outState->grid[1][4] = pawn   | CHESS_BLACK;
    
    outState->grid[6][3] = bishop | CHESS_WHITE;
    outState->grid[6][4] = pawn | CHESS_WHITE;
    outState->grid[7][4] = king | CHESS_WHITE;

    

    outState->nextToMove = CHESS_WHITE;
    outState->moveCount  = 0;
    outState->kingExists[0] = 1;
    outState->kingExists[1] = 1;
    }
*/



#define LOG_CHESS_STATES     0

/* set to a large value to actually log states
   set to 1 when not doing debug logging, to save RAM*/
#define MAX_LOGGED_STATES    1

static  int         bestTopLevelScore;

static  BoardState  stateLog[ MAX_LOGGED_STATES ];
static  int         bestScoreLog[ MAX_LOGGED_STATES ];
static  int         logCount  =  0;



char getLoggedState( int          inLogNumber,
                     BoardState  *outState,
                     int         *outBestScore ) {
    if( inLogNumber >= logCount ) {
        return 0;
        }
    
    *outState     = stateLog[ inLogNumber ];
    *outBestScore = bestScoreLog[ inLogNumber ];
    return 1;
    }



static int getPiecePossibleMoves( BoardState     *inState,
                                  int             inPieceRow,
                                  int             inPieceCol,
                                  char            inAvoidCheck,
                                  unsigned char   outRows    [BN],
                                  unsigned char   outCols    [BN],
                                  Captured        outCaptured[BN],
                                  BoardState      outStates  [BN] ) {
    
    static  BoardState     resultStates  [BN];
    static  unsigned char  resultRows    [BN];
    static  unsigned char  resultCols    [BN];
    static  Captured       resultCaptured[BN];
    
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
                                       0,
                                       resultRows,
                                       resultCols,
                                       resultCaptured,
                                       resultStates );

    /* filter moves to remove illegal moves that put our king in check */
    for( m = 0;
         m < numMoves;
         m ++ ) {

        resultStates[m].moveCount ++;

        if( ! inAvoidCheck
            ||
            ! isKingInCheck( &( resultStates[m] ),
                             pColor ) ) {

            outCaptured[ numGoodMoves ] = resultCaptured[ m ];
            outStates  [ numGoodMoves ] = resultStates  [ m ];
            outRows    [ numGoodMoves ] = resultRows    [ m ];
            outCols    [ numGoodMoves ] = resultCols    [ m ];

            numGoodMoves ++;
            }
        
        if( LOG_CHESS_STATES
            &&
            logCount < MAX_LOGGED_STATES ) {
            
            stateLog[ logCount ] = resultStates  [ m ];

            bestScoreLog[ logCount ] = bestTopLevelScore;
            logCount++;
            }
        }

    statesTestedLastMove += numMoves;
    
    return numGoodMoves;   
    }



const char *getBoardStateString( BoardState  *inState ) {

    enum{         bufferSize  =  4 * BN,
                  numBuffers  =  4 };
        
    static  char  buffer[ numBuffers ][ bufferSize ];
    static  int   b           =  0;
    

    const char   *nextToMove  =  "Next to move: ";
    int           nLen        =  maxigin_stringLength( nextToMove );
    int           y;
    int           x;
    int           i           =  0;
    int           n;
    char         *returnBuff;
    
    for( y = 0;
         y < BH;
         y ++ ) {
        
        for( x = 0;
             x < BW;
             x ++ ) {

            ChessPiece  p  =  inState->grid[y][x];
            ChessPiece  t  =  p & CHESS_TYPE_MASK;
            
            if( t == noPiece ) {
                buffer[b][ i ] = pieceChars[ t ];
                }
            else {
                if( ( inState->grid[y][x] & CHESS_COLOR_MASK ) == CHESS_BLACK ) {
                    buffer[b][ i ] = pieceChars[ t ];
                    }
                else {
                    /* make upper case for white */
                    buffer[b][ i ] = (char)( pieceChars[ t ] - 32 );
                    }
                }
            
            i ++;

            buffer[b][ i ] = ' ';

            i ++;
            }

        buffer[b][ i ] = '\n';
        i ++;
        }


    for( n = 0;
         n < nLen;
         n ++ ) {

        buffer[b][ i++ ] = nextToMove[n];
        }
    
    if( inState->nextToMove == CHESS_WHITE ) {
        buffer[b][ i++ ] = 'W';
        }
    else {
        buffer[b][ i++ ] = 'b';
        }
    

    buffer[b][ i ] = '\0';

    returnBuff = buffer[b];

    b ++;

    /* wrap around */
    if( b >= numBuffers ) {
        b = 0;
        }
    
    return returnBuff;
    }



char getRandomMove( BoardState  *inState,
                    char         inAvoidCheck,
                    Move        *outMove,
                    Captured    *outCaptured,
                    BoardState  *outNewState ) {

    /*  pay attention to limits on where piece can actually move */

    static  unsigned char  possiblePieceRow[BN];
    static  unsigned char  possiblePieceCol[BN];
    static  unsigned char  possibleDestRow [BN];
    static  unsigned char  possibleDestCol [BN];
    static  Captured       possibleCaptured[BN];
    static  BoardState     possibleStates  [BN];
    
    int             numPossiblePieces  =  0;
    int             piecePick;
    int             p;
    unsigned char   x;
    unsigned char   y;
    int            *shuffle;
    int             colorToMove        =  inState->nextToMove;
    
    for( y = 0;
         y < BH;
         y ++ ) {
        
        for( x = 0;
             x < BW;
             x ++ ) {

            if( inState->grid[y][x] != noPiece ) {

                if( ( inState->grid[y][x] & CHESS_COLOR_MASK ) == colorToMove ) {
                
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


    /* shuffle possible pieces to move, then keep walking through
       options until we find a piece that can actually move */
    
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
                                          inAvoidCheck,
                                          possibleDestRow,
                                          possibleDestCol,
                                          possibleCaptured,
                                          possibleStates );

        if( numMoves > 0 ) {
            m = maxigin_randRange( &chessRand,
                                   0,
                                   numMoves - 1 );
            outMove->startPos[0] = y;
            outMove->startPos[1] = x;
    
            outMove->endPos  [0] = possibleDestRow[m];
            outMove->endPos  [1] = possibleDestCol[m];

            *outCaptured = possibleCaptured[m];
            *outNewState = possibleStates  [m];
            
            return 1;
            }
        }
       
    
    /* tried all possible pieces, none could move*/
    return 0;
    }



static char isAnyMovePossible( BoardState  *inState,
                               char         inAvoidCheck ) {

    static  unsigned char  possibleDestRow [BN];
    static  unsigned char  possibleDestCol [BN];
    static  Captured       possibleCaptured[BN];
    static  BoardState     possibleStates  [BN];

    unsigned char   x;
    unsigned char   y;
    int             colorToMove        =  inState->nextToMove;
    
    for( y = 0;
         y < BH;
         y ++ ) {
        
        for( x = 0;
             x < BW;
             x ++ ) {

            if( inState->grid[y][x] != noPiece ) {

                if( ( inState->grid[y][x] & CHESS_COLOR_MASK ) == colorToMove ) {

                    int numMoves = getPiecePossibleMoves( inState,
                                                          y,
                                                          x,
                                                          inAvoidCheck,
                                                          possibleDestRow,
                                                          possibleDestCol,
                                                          possibleCaptured,
                                                          possibleStates );
                    if( numMoves > 0 ) {
                        return 1;
                        }
                    }
                }
            }
        }
    
    /* tried all possible pieces, none could move */
    return 0;
    }



int getScore( BoardState *inState ) {

    int  score  = 0;
    int  y;
    int  x;
    
    for( y = 0;
         y < BH;
         y ++ ) {
        
        for( x = 0;
             x < BW;
             x ++ ) {

            ChessPiece p   =  inState->grid[ y ][ x ];
            ChessPiece c;
            ChessPiece t;
            
            if( p == noPiece ) {
                continue;
                }
            c =  p & CHESS_COLOR_MASK;
            t =  p & CHESS_TYPE_MASK;
            
            if( c == CHESS_BLACK ) {
                score -= pieceValue[ t ];
                }
            else {
                score += pieceValue[ t ];
                }
            }

        }

    return score;
    }



#define  MAX_DEPTH  5
#define  MAX_SCORE  9999


static  int  checkmateScore  =  MAX_SCORE - 1;


/* inBailAboveScore and inBailBelowScore are used for alpha-beta style
   pruning */
static char getGreedyDepthMove( BoardState  *inState,
                                char         inAvoidCheck,
                                Move        *outMove,
                                Captured    *outCaptured,
                                BoardState  *outNewState,
                                int         *outScore,
                                int          inBailAboveScore,
                                int          inBailBelowScore,
                                int          inDepth ) {
    
    /* look at all pieces that can move */

    /* index with inDepth here so deeper recursions don't clobber our
       static state at the current level */
    static  unsigned char  possiblePieceRow[ MAX_DEPTH ][BN];
    static  unsigned char  possiblePieceCol[ MAX_DEPTH ][BN];
    static  int            pieceLookOrder  [ MAX_DEPTH ][BN];
    
    /* for current piece that we're looking at, what are the possible moves */
    static  unsigned char  possibleDestRow [ MAX_DEPTH ][BN];
    static  unsigned char  possibleDestCol [ MAX_DEPTH ][BN];
    static  Captured       possibleCaptured[ MAX_DEPTH ][BN];
    static  BoardState     possibleStates  [ MAX_DEPTH ][BN];

    /* for shuffling the possible moves of our current piece before
       considering them */
    static  int            moveLookOrder   [ MAX_DEPTH ][BN];

    static Captured        nextMoveCaptured[ MAX_DEPTH ];
    static BoardState      nextMoveState   [ MAX_DEPTH ];
    
    
    int             foundBest          =  0;
    int             bestScore          =  - MAX_SCORE - 1;
    int             numPossiblePieces  =  0;
    int             piecePick;
    int             p;
    unsigned char   x;
    unsigned char   y;
    int             colorToMove        =  inState->nextToMove;
    int             i;
    Move            nextMove;

    if( colorToMove == CHESS_BLACK ) {
        bestScore = MAX_SCORE + 1;
        }

    if( inDepth == 1 ) {
        bestTopLevelScore = bestScore;
        }
    
    for( y = 0;
         y < BH;
         y ++ ) {
        
        for( x = 0;
             x < BW;
             x ++ ) {

            if( inState->grid[y][x] != noPiece ) {

                if( ( inState->grid[y][x] & CHESS_COLOR_MASK ) == colorToMove ) {
                
                    possiblePieceRow[ inDepth ][ numPossiblePieces ] = y;
                    possiblePieceCol[ inDepth ][ numPossiblePieces ] = x;
                    numPossiblePieces ++;
                    }
                }
            }
        }

    if( numPossiblePieces == 0 ) {
        return 0;
        }

    /* shuffle an index into our possible pieces */
    for( i = 0;
         i < numPossiblePieces;
         i ++ ) {
        pieceLookOrder[ inDepth ][i] = i;
        }
    maxigin_shuffle( &chessRand,
                     numPossiblePieces,
                     pieceLookOrder[ inDepth ] );
    
    for( p = 0;
         p < numPossiblePieces;
         p ++ ) {

        int  numMoves;
        int  m;
        
        piecePick = pieceLookOrder[ inDepth ][ p ];
        
        y = possiblePieceRow[ inDepth ][ piecePick ];
        x = possiblePieceCol[ inDepth ][ piecePick ];

        numMoves = getPiecePossibleMoves( inState,
                                          y,
                                          x,
                                          inAvoidCheck,
                                          possibleDestRow [ inDepth ],
                                          possibleDestCol [ inDepth ],
                                          possibleCaptured[ inDepth ],
                                          possibleStates  [ inDepth ] );

        if( numMoves > 0 ) {

            /* shuffle an index into our possible moves */
            for( i = 0;
                 i < numMoves;
                 i ++ ) {
                moveLookOrder[ inDepth ][i] = i;
                }
            maxigin_shuffle( &chessRand,
                             numMoves,
                             moveLookOrder[ inDepth ] );

            /* now walk through in shuffled order and
               find first move that has higher score than best so far */

            /* if there are multiple moves for this piece that have the same
               higher score, a random one will be picked due to the shuffling */

            for( i = 0;
                 i < numMoves;
                 i ++ ) {

                int   score;
                int   checkmateVictimColor;
                char  forcedCheckmate;
                char  checkmate;
                
                
                m = moveLookOrder[ inDepth ][ i ];

                /* code for debugging, to peek at board states
                if( inDepth == 1 ) {
                    const char  *stateStringOld =
                        getBoardStateString( inState );
                    const char  *stateString =
                        getBoardStateString( &( possibleStates[ inDepth ][m] ) );

                    (void)stateStringOld;
                    (void)stateString;

                    checkmate = 0;
                    }
                */

                checkmate = isCheckmate( &( possibleStates[ inDepth ][m] ),
                                         &checkmateVictimColor );

                if( ! checkmate ) {
                    forcedCheckmate =
                        isForcedCheckmate( &( possibleStates[ inDepth ][m] ),
                                           &checkmateVictimColor );
                    }

                if( checkmate ) {
                    /* already at checkmate state here, don't search
                       deeper */
                    if( checkmateVictimColor == CHESS_BLACK ) {
                        score =    checkmateScore;
                        }
                    else {
                        score =  - checkmateScore;
                        }
                    }
                else if( forcedCheckmate ) {
                    /* taking king forced, one move away */
                    /* not quite as valuable as immediately taking
                       the king */
                    if( checkmateVictimColor == CHESS_BLACK ) {
                        score =      checkmateScore - 1;
                        }
                    else {
                        score =  - ( checkmateScore - 1 );
                        }
                    }
                else {
                    
                    score = getScore( &( possibleStates[ inDepth ][m] ) );

                    if( inDepth > 0 ) {
                        /* not in checkmate or forced checkmate state after
                           one move
                       
                           continue searching deeper */
                        
                        int   nextDepth  =  inDepth - 1;
                        char  nextFound;
                        int   nextScore;
                        int   bailAbove  =  MAX_SCORE + 1;
                        int   bailBelow  =  - MAX_SCORE - 1;

                        if( foundBest ) {

                            /* setup limits for alpha-beta pruning */

                            if( colorToMove == CHESS_WHITE ) {
                                /* our best score so far is a high water mark
                                   ignore any moves that lead to ANY next
                                   move that is lower than that */
                                bailBelow = bestScore;
                                }
                            else {
                                /* we're black, and best move is a low
                                   water mark */
                                bailAbove = bestScore;
                                }
                            }
                        

                        /* also carry any limits from further up */
                        if( bailAbove > inBailAboveScore ) {
                            bailAbove = inBailAboveScore;
                            }
                        if( bailBelow < inBailBelowScore ) {
                            bailBelow = inBailBelowScore;
                            }
                            
                        
                        /* avoid check at first when looking at next move */
                        nextFound =
                            getGreedyDepthMove(
                                &( possibleStates[ inDepth ][m] ),
                                1,
                                &nextMove,
                                &( nextMoveCaptured[ nextDepth ] ),
                                &( nextMoveState   [ nextDepth ] ),
                                &nextScore,
                                bailAbove,
                                bailBelow,
                                nextDepth );
                        if( nextFound ) {
                            score = nextScore;
                            }
                        else {
                            /* no move for opponent?

                               but they're not currently checkmated
                               or in a forced checkmate situation
                               this means they are stalemated,
                               but they will move into check on their next
                               move anyway */

                            /* count such situations as slightly
                               less valuable than a force checkmate */

                            if( possibleStates[ inDepth ][ m ].nextToMove
                                == CHESS_BLACK ) {
                                
                                score =      checkmateScore - 2;
                                }
                            else {
                                score =  - ( checkmateScore - 2 );
                                }
                            }
                        }
                    }
                

                if( ( colorToMove == CHESS_WHITE
                      &&
                      score > bestScore )
                    ||
                    ( colorToMove == CHESS_BLACK
                      &&
                      score < bestScore ) ) {

                    foundBest = 1;
                    bestScore = score;

                    if( inDepth == 1 ) {
                        bestTopLevelScore = bestScore;
                        }

                    outMove->startPos[0] = y;
                    outMove->startPos[1] = x;
                    outMove->endPos[0]   = possibleDestRow [ inDepth ][m];
                    outMove->endPos[1]   = possibleDestCol [ inDepth ][m];
                    *outCaptured         = possibleCaptured[ inDepth ][m];
                    *outNewState         = possibleStates  [ inDepth ][m];

                    if( bestScore >= inBailAboveScore
                        ||
                        bestScore <= inBailBelowScore ) {

                        /* prune rest of search */
                        *outScore = bestScore;
                        
                        return 1;
                        }
                    
                    /* code for debugging, to peek at board states
                    if( inDepth == 1 ) {
                        const char  *stateStringOld =
                            getBoardStateString( inState );
                        const char  *stateString =
                            getBoardStateString( outNewState );

                        (void)stateStringOld;
                        (void)stateString;

                        foundBest = 1;
                        }
                    */
                    }
                }
            }
        }

    /* if all moves are equal (no captures possible, no score change possible),
       then we end up picking a random move above, due to the shuffles */

    *outScore = bestScore;
    
    if( foundBest ) {
        return 1;
        }
        
        
    /* tried all possible pieces, none could move*/
    return 0;
    }



char getGreedyMove( BoardState  *inState,
                    Move        *outMove,
                    Captured    *outCaptured,
                    BoardState  *outNewState ) {

    int   nextScore;

    char  canMove;
    
    canMove = getGreedyDepthMove( inState,
                                  1,
                                  outMove,
                                  outCaptured,
                                  outNewState,
                                  &nextScore,
                                  MAX_SCORE + 1,
                                  - MAX_SCORE - 1,
                                  1 );

    if( ! canMove ) {
        /* stuck with no moves that don't move into check */

        /* try again, allowing moving into check,
           but just find the "best" shallow move,
           no sense in searching multiple moves ahead in this case,
           since we can't avoid moving into check, and are going to lose
           on the next move anyway */
        canMove = getGreedyDepthMove( inState,
                                      0,
                                      outMove,
                                      outCaptured,
                                      outNewState,
                                      &nextScore,
                                      MAX_SCORE + 1,
                                      - MAX_SCORE - 1,
                                      0 );
        }

    return canMove;
    }



char getMixedMove( BoardState  *inState,
                   Move        *outMove,
                   Captured    *outCaptured,
                   BoardState  *outNewState ) {

    int  pick  =  maxigin_randRange( &chessRand,
                                     1,
                                     100 );

    if( pick <= 75 ) {

        return getGreedyMove( inState,
                              outMove,
                              outCaptured,
                              outNewState );
        }
    else {
        char  canMove =  getRandomMove( inState,
                                        1,
                                        outMove,
                                        outCaptured,
                                        outNewState );

        if( ! canMove ) {
            /* stuck with no moves that don't move into check */

            /* try again, allowing moving into check */
            canMove = getRandomMove( inState,
                                     0,
                                     outMove,
                                     outCaptured,
                                     outNewState );
            }
        return canMove;
        }
    }



static char getChessMoveInternal( BoardState  *inState,
                                  Move        *outMove,
                                  Captured    *outCaptured,
                                  BoardState  *outNewState ) {

    statesTestedLastMove = 0;
    logCount             = 0;
    bestTopLevelScore    = 0;
    
    if( inState->nextToMove == CHESS_BLACK ) {
        return getMixedMove( inState,
                             outMove,
                             outCaptured,
                             outNewState );
        }
    else {
        /* white always makes greedy move,
           so they never hang a queen, etc.  */
        return getGreedyMove( inState,
                              outMove,
                              outCaptured,
                              outNewState );
        }
    }



char getChessMove( BoardState  *inState,
                   Move        *outMove,
                   Captured    *outCaptured,
                   BoardState  *outNewState ) {

    char        foundMove    =  getChessMoveInternal( inState,
                                                      outMove,
                                                      outCaptured,
                                                      outNewState );
    ChessPiece  movingPiece;

    if( ! foundMove ) {
        return foundMove;
        }


    movingPiece = inState->grid[ outMove->startPos[0] ]
                               [ outMove->startPos[1] ];
    

    if( ! repickMoveAtEnd[ movingPiece & CHESS_TYPE_MASK ] ) {
        return foundMove;
        }
    else {

        /* else we need to repick */

        static  unsigned char  repickRows    [ BN ];
        static  unsigned char  repickCols    [ BN ];
        static  Captured       repickCaptured[ BN ];
        static  BoardState     repickStates  [ BN ];

        int  numMoves;
        

        

        numMoves = getPiecePossibleMoves( inState,
                                          outMove->startPos[0],
                                          outMove->startPos[1],
                                          1,
                                          repickRows,
                                          repickCols,
                                          repickCaptured,
                                          repickStates );

        if( numMoves == 0 ) {
            return 0;
            }

        /* assume that all pieces that require a repick are producing
           randomized moves.  They usually return 1 move, but always
           return the first move regardless */

        outMove->endPos[0] = repickRows    [0];
        outMove->endPos[1] = repickCols    [0];
        *outCaptured       = repickCaptured[0];
        *outNewState       = repickStates  [0];

        return 1;
        }
    }

    



int getStateCountLastMove( void ) {
    return statesTestedLastMove;
    }



void applyMove( BoardState  *inState,
                Move        *inMove,
                BoardState  *inNewState ) {

    /* just copy state over, don't need to actually apply move */
    *inState = *inNewState;

    return;

    /* legacy code where move is applied here */
    
    inState->grid[ inMove->endPos[0] ][ inMove->endPos[1] ]
        = inState->grid[ inMove->startPos[0] ][ inMove->startPos[1] ];

    /* leave empty square behind */
    inState->grid[ inMove->startPos[0] ][ inMove->startPos[1] ] =
        noPiece;

    if( inState->nextToMove == CHESS_WHITE ) {
        inState->nextToMove =  CHESS_BLACK;
        }
    else {
        inState->nextToMove =  CHESS_WHITE;
        }
    }




/* helper function that can be called from debugger */
void printState( BoardState  *inState );



void printState( BoardState  *inState ) {

    maxigin_logString( "State:\n",
                       getBoardStateString( inState ) );
    }



    


char isCheckmate( BoardState  *inState,
                  int         *outColor ) {

    /* change semantics of this
       Count as checkmate if king is gone */

    /* if both kings are gone, count as a loss for black */
    if( ! doesKingExist( inState,
                         CHESS_BLACK ) ) {
        *outColor = CHESS_BLACK;
        return 1;
        }

    if( ! doesKingExist( inState,
                         CHESS_WHITE ) ) {
        *outColor = CHESS_WHITE;
        return 1;
        }

    return 0;
    }



char isForcedCheckmate( BoardState  *inState,
                        int         *outColor ) {

    if( isKingInCheck( inState,
                       inState->nextToMove ) ) {

        /* no moving into check */

        if( isAnyMovePossible( inState,
                               1 ) ) {
            return 0;
            }
    
        *outColor = inState->nextToMove;
        return 1;
        }

    return 0;
    }



/* the signature for a piece effects function.
   We define one of these for each piece type in the enum above.

   Updates the passed-in effects map to include effects caused by a given
   piece.
*/
typedef void (*PieceEffectsFunction)( ChessPiece              inPiece,
                                      int                     inPieceRow,
                                      int                     inPieceCol,
                                      FullBoardSpaceEffects  *inEffects );



static void nullEffects( ChessPiece              inPiece,
                         int                     inPieceRow,
                         int                     inPieceCol,
                         FullBoardSpaceEffects  *inEffects ) {

    (void)inPiece;
    (void)inPieceRow;
    (void)inPieceCol;
    (void)inEffects;
    }



static void doublingPawnEffects( ChessPiece              inPiece,
                                 int                     inPieceRow,
                                 int                     inPieceCol,
                                 FullBoardSpaceEffects  *inEffects ) {

    int  yDir      = -1;
    int  targetY;

    if( ( inPiece & CHESS_COLOR_MASK ) == CHESS_BLACK ) {
        yDir = 1;
        }

    targetY = inPieceRow + yDir;

    if( targetY >= 0
        &&
        targetY < BH ) {

        int  oldNum  =  inEffects->grid[ targetY ][ inPieceCol ].num;

        inEffects->grid[ targetY ][ inPieceCol ].effectType [ oldNum ] =
            multiply;
        
        inEffects->grid[ targetY ][ inPieceCol ].effectValue[ oldNum ] =
            2;
        
        inEffects->grid[ targetY ][ inPieceCol ].sourceRow[ oldNum ] =
            inPieceRow;
        
        inEffects->grid[ targetY ][ inPieceCol ].sourceCol[ oldNum ] =
            inPieceCol;
        

        inEffects->grid[ targetY ][ inPieceCol ].num ++;
        }

    }



static void addingRookEffects( ChessPiece              inPiece,
                               int                     inPieceRow,
                               int                     inPieceCol,
                               FullBoardSpaceEffects  *inEffects ) {

    static  int  dX[4]  =  { 1, -1,  0,  0 };
    static  int  dY[4]  =  { 0,  0, -1,  1 };

    int  i;

    (void)inPiece;

    for( i = 0;
         i < 4;
         i ++ ) {

        int  targetX  =  inPieceCol + dX[i];
        int  targetY  =  inPieceRow + dY[i];

        if( targetX >= 0
            &&
            targetX < BW
            &&
            targetY >= 0
            &&
            targetY < BH ) {

            int  oldNum  =  inEffects->grid[ targetY ][ targetX ].num;

            inEffects->grid[ targetY ][ targetX ].effectType [ oldNum ] =
                add;
        
            inEffects->grid[ targetY ][ targetX ].effectValue[ oldNum ] =
                1;
        
            inEffects->grid[ targetY ][ targetX ].sourceRow[ oldNum ] =
                inPieceRow;
        
            inEffects->grid[ targetY ][ targetX ].sourceCol[ oldNum ] =
                inPieceCol;
        

            inEffects->grid[ targetY ][ targetX ].num ++;
            }
        }
    }



static PieceEffectsFunction effectsFunctions[] =
                                { nullEffects,
                                  nullEffects,
                                  nullEffects,
                                  nullEffects,
                                  nullEffects,
                                  nullEffects,
                                  nullEffects,
                                  nullEffects,
                                  nullEffects,
                                  doublingPawnEffects,
                                  addingRookEffects,
                                  nullEffects,
                                    };

CHECK_ARRAY_LENGTH( effectsFunctions,
                    NUM_CHESS_PIECES );


static void clearSpaceEffects( FullBoardSpaceEffects  *outEffects ) {

    int  y;
    int  x;

    for( y = 0;
         y < BH;
         y ++ ) {

        for( x = 0;
             x < BW;
             x ++ ) {

            outEffects->grid[y][x].num = 0;
            }
        }
    }


void getSpaceEffects( BoardState             *inState,
                      int                     inAffectedColor,
                      FullBoardSpaceEffects  *outEffects ) {

    int  y;
    int  x;

    clearSpaceEffects( outEffects );

    for( y = 0;
         y < BH;
         y ++ ) {

        for( x = 0;
             x < BW;
             x ++ ) {

            ChessPiece  p  = inState->grid[y][x];
            ChessPiece  t;
            ChessPiece  c;
            
            if( p == noPiece ) {
                continue;
                }

            c = p & CHESS_COLOR_MASK;

            if( c != inAffectedColor ) {
                /* only compute effects for next to move */
                continue;
                }
            
            t = p & CHESS_TYPE_MASK;

            effectsFunctions[ t ]( p,
                                   y,
                                   x,
                                   outEffects );
            }
        }
    }





static  char  visitFlags[ BH ][ BW ];



static void compoundSpaceEffectsRec( int                     inTargetRow,
                                     int                     inTargetCol,
                                     FullBoardSpaceEffects  *inOutEffects ) {

    ActiveSpaceEffects  *a  =
        &( inOutEffects->grid[ inTargetRow ][ inTargetCol ] );

    int                  i;

    visitFlags[ inTargetRow ][ inTargetCol ] = 1;
    
    for( i = 0;
         i < a->num;
         i ++ ) {
        
        int                  value  =  a->effectValue[ i ];
        int                  sR     =  a->sourceRow  [ i ];
        int                  sC     =  a->sourceCol  [ i ];
        ActiveSpaceEffects  *sA     =  &( inOutEffects->grid[ sR ][ sC ] );
        int                  sI;

        if( visitFlags[ sR ][ sC ] ) {
            /* skip any nodes we've already visited entirely
               since we are computing compound rec for a specific space,
               and each node should only affect the compound once  */

            /* also clear these out entirely, since we've already hit them
               before elsewhere in the tree affecting our target space */
            a->effectType [ i ] = noEffect;
            a->effectValue[ i ] = 0;
            
            continue;
            }
        
        /* recurse */
        compoundSpaceEffectsRec( sR,
                                 sC,
                                 inOutEffects );

        /* multiply happens first */
        for( sI = 0;
             sI < sA->num;
             sI ++ ) {

            if( sA->effectType[ sI ] == multiply ) {
                value *= sA->effectValue[ sI ];
                }
            }
        
        /* then addition */
        for( sI = 0;
             sI < sA->num;
             sI ++ ) {

            if( sA->effectType[ sI ] == add ) {
                value += sA->effectValue[ sI ];
                }
            }

        a->effectValue[ i ] = value;
        }
    }
    



void compoundSpaceEffects( int                     inTargetRow,
                           int                     inTargetCol,
                           FullBoardSpaceEffects  *inOutEffects ) {
    
    int y;
    int x;
    
    for( y = 0;
         y < BH;
         y ++ ) {

        for( x = 0;
             x < BW;
             x ++ ) {
            visitFlags[ y ][ x ] = 0;
            }
        }

    compoundSpaceEffectsRec( inTargetRow,
                             inTargetCol,
                             inOutEffects );

    }



void getTotalSpaceEffects( int                     inTargetRow,
                           int                     inTargetCol,
                           FullBoardSpaceEffects  *inEffects,
                           TotalSpaceEffects      *outTotals ) {

    ActiveSpaceEffects  *a          =
        &( inEffects->grid[ inTargetRow ][ inTargetCol ] );
    int                  i;
    int                  repeatVal  =  1;
    
    for( i = 0;
         i < NUM_SPACE_EFFECTS;
         i ++ ) {
        
        outTotals->effectValue[ i ] = 0;
        }

    
    /* multiply happens first */
    for( i = 0;
         i < a->num;
         i ++ ) {

        if( a->effectType[ i ] == multiply ) {
            repeatVal *= a->effectValue[ i ];
            }
        }

    /* then add */
    for( i = 0;
         i < a->num;
         i ++ ) {

        if( a->effectType[ i ] == add ) {
            repeatVal += a->effectValue[i];
            }
        }

    /* stick result in the multiply effect
       which means the piece should repeat its special abilities this many
       times */
    outTotals->effectValue[ multiply ] = repeatVal;


    /* now sum any non-mult and non-add effects */
    for( i = 0;
         i < a->num;
         i ++ ) {

        SpaceEffect  t  =  a->effectType[ i ];
        
        if( t == add ) {
            continue;
            }
        
        if( t == multiply ) {
            continue;
            }
        outTotals->effectValue[ t ] += a->effectValue[i];
        }

    /* now multiply non-mult, non-add effects by repeat value */
    if( repeatVal > 1 ) {
        
        for( i = 0;
             i < a->num;
             i ++ ) {

            SpaceEffect  t  =  a->effectType[ i ];

            if( outTotals->effectValue[ t ] == 0 ) {
                continue;
                }
        
            if( t == add ) {
                continue;
                }
        
            if( t == multiply ) {
                continue;
                }
        
            outTotals->effectValue[ t ] *= repeatVal;
            }
        }
    
    }




#endif


#endif
