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
#define  CHESS_PIECE_MASK   0x7F
#define  CHESS_COLOR_MASK   0x80
#define  CHESS_WHITE        0x00
#define  CHESS_BLACK        0x80


typedef struct BoardState{
        
        ChessPiece  squareStates[8][8];

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
         y < 8;
         y ++ ) {
        
        for( x = 0;
             x < 8;
             x ++ ) {

            outState->squareStates[y][x] = noPiece;
            }
        }
    

    /* fixme:
       fill out whole starting board
       why aren't color bits working to toggle draw colors ?  */
    outState->squareStates[0][0] = rook   | CHESS_BLACK;
    outState->squareStates[0][1] = knight | CHESS_BLACK;
    outState->squareStates[0][2] = bishop | CHESS_BLACK;
    outState->squareStates[0][3] = queen  | CHESS_BLACK;
    outState->squareStates[0][4] = king   | CHESS_BLACK;
    outState->squareStates[0][5] = bishop | CHESS_BLACK;
    outState->squareStates[0][6] = knight | CHESS_BLACK;
    outState->squareStates[0][7] = rook   | CHESS_BLACK;

    for( i = 0;
         i < 8;
         i ++ ) {
        outState->squareStates[1][i] = pawn | CHESS_BLACK;
        }

    /* fixme:
       overlapping extra row, just for testing visuals */
    /*
    outState->squareStates[2][0] = rook   | CHESS_BLACK;
    outState->squareStates[2][1] = knight | CHESS_BLACK;
    outState->squareStates[2][2] = bishop | CHESS_BLACK;
    outState->squareStates[2][3] = queen  | CHESS_BLACK;
    outState->squareStates[2][4] = king   | CHESS_BLACK;
    outState->squareStates[2][5] = bishop | CHESS_BLACK;
    outState->squareStates[2][6] = knight | CHESS_BLACK;
    outState->squareStates[2][7] = rook   | CHESS_BLACK;
    */

    outState->squareStates[7][0] = rook   | CHESS_WHITE;
    outState->squareStates[7][1] = knight | CHESS_WHITE;
    outState->squareStates[7][2] = bishop | CHESS_WHITE;
    outState->squareStates[7][3] = queen  | CHESS_WHITE;
    outState->squareStates[7][4] = king   | CHESS_WHITE;
    outState->squareStates[7][5] = bishop | CHESS_WHITE;
    outState->squareStates[7][6] = knight | CHESS_WHITE;
    outState->squareStates[7][7] = rook   | CHESS_WHITE;

    for( i = 0;
         i < 8;
         i ++ ) {
        outState->squareStates[6][i] = pawn | CHESS_WHITE;
        }

    }



char getRandomMove( BoardState  *inState,
                    Move        *outMove,
                    char         inWhiteTurn ) {

    /* fixme:  pay attention to limits on where piece can actually move */

    static  unsigned char  possiblePieceRow[64];
    static  unsigned char  possiblePieceCol[64];

    int            numPossiblePieces = 0;
    int            movePick;
    unsigned char  x;
    unsigned char  y;
    unsigned char  x2;
    unsigned char  y2;
    unsigned char  startPosFlag;
    
    
    for( y = 0;
         y < 8;
         y ++ ) {
        
        for( x = 0;
             x < 8;
             x ++ ) {

            if( inState->squareStates[y][x] != noPiece ) {

                if( ( inWhiteTurn
                      &&
                      ( inState->squareStates[y][x] & 0x80 ) == CHESS_WHITE )
                    ||
                    ( ! inWhiteTurn
                      &&
                      ( inState->squareStates[y][x] & 0x80 ) == CHESS_BLACK ) ) {
                    
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


    movePick = maxigin_randRange( &chessRand,
                                  0,
                                  numPossiblePieces - 1 );

    y = possiblePieceRow[ movePick ];
    x = possiblePieceCol[ movePick ];

    startPosFlag = inState->squareStates[y][x] & 0x80;
    
    x2 = x;
    y2 = y;
        

    /* re-roll until we move off of start pos
       and until we land on empty spot or enemy piece */
    while( ( x2 == x
             &&
             y2 == y )
           ||
           ( inState->squareStates[y2][x2] != noPiece
             &&
             ( inState->squareStates[y2][x2] & 0x80 ) == startPosFlag ) ) {
        
    
        y2 = (unsigned char)( maxigin_randRange( &chessRand,
                                                 0,
                                                 7 ) );
        x2 = (unsigned char)( maxigin_randRange( &chessRand,
                                                 0,
                                                 7 ) );
        }

    
    outMove->startPos[0] = y;
    outMove->startPos[1] = x;
    
    outMove->endPos  [0] = y2;
    outMove->endPos  [1] = x2;
    
    return 1;
    }



void applyMove( BoardState  *inState,
                Move        *inMove ) {
    inState->squareStates[ inMove->endPos[0] ][ inMove->endPos[1] ]
        = inState->squareStates[ inMove->startPos[0] ][ inMove->startPos[1] ];

    /* leave empty square behind */
    inState->squareStates[ inMove->startPos[0] ][ inMove->startPos[1] ] =
        noPiece;
    }


#endif


#endif
