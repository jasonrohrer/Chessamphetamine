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





/* fills outState with the starting board state */
void getStartBoard( BoardState  *outState );



#ifdef CHESS_IMPLEMENTATION



void getStartBoard( BoardState  *outState ) {

    int  x;
    int  y;

    for( y = 0;
         y < 8;
         y ++ ) {
        
        for( x = 0;
             x < 8;
             x ++ ) {

            outState->squareStates[y][x] = noPiece;
            }
        }
    
            
    outState->squareStates[0][0] = rook | CHESS_BLACK;
    outState->squareStates[7][0] = rook | CHESS_WHITE;
    }


#endif


#endif
