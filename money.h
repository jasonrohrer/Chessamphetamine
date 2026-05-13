/*
  Include in your C code wherever like so:

      #include "money.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define MONEY_IMPLEMENTATION
      #include "money.h"

*/

#ifndef MONEY_H_INCLUDED
#define MONEY_H_INCLUDED


void moneyInit( int inStartVal );


void moneyAdd( int inValToAdd );


void moneyAddCapture( ChessPiece inPiece );


void moneyDraw( int  inPosX,
                int  inPosY );


void moneyStep( void );



#ifdef MONEY_IMPLEMENTATION


#include "chess.h"
#include "arraySizeCheck.h"
#include "memoryRegister.h"


/*
  how much money you get for capturing a given piece
  pawns are 1
  non-pawns are 2
  except for king, which is 4
*/
static int pieceCaptureMoney[] = { 0,
                                   1,
                                   2,
                                   2,
                                   2,
                                   2,
                                   4,
                                   2,
                                   2,
                                   2,
                                   2 };

CHECK_ARRAY_LENGTH( pieceCaptureMoney,
                    NUM_CHESS_PIECES );


static int coinSprite;

static int moneyVal;

static int moneyFont;


void moneyInit( int inStartVal ) {

    int  fontStrip;

    
    coinSprite = maxigin_initSprite( "coin.tga" );

    if( coinSprite != -1 ) {
        maxigin_initMakeGlowSprite( coinSprite,
                                    4,
                                    2 );
        }

    moneyVal = inStartVal;


    /* same as modifier font, but fixed width */
    fontStrip = maxigin_initSpriteStrip( "modifierFont.tga",
                                         8 );

    if( fontStrip != -1 ) {

        maxigin_initMakeGlowSpriteStrip( fontStrip,
                                         2,
                                         2 );

        moneyFont = maxigin_initFont( fontStrip,
                                      "modifierFont.txt",
                                      0,
                                      4,
                                      7 );
        }

    REGISTER_VAL_MEM( moneyVal );
    }



void moneyDraw( int  inPosX,
                int  inPosY ) {

    const char  *displayText;
    

    drawSetPieceColor( CHESS_WHITE );

    
    maxigin_drawSprite( coinSprite,
                        inPosX,
                        inPosY );

    displayText = maxigin_intToString( moneyVal );
        
    maxigin_drawText( moneyFont,
                      displayText,
                      inPosX - 9,
                      inPosY,
                      MAXIGIN_RIGHT );

    }



void moneyStep( void ) {

    /*moneyVal ++;*/

    }


void moneyAdd( int  inValToAdd ) {
    moneyVal += inValToAdd;
    }


void moneyAddCapture( ChessPiece inPiece ) {

    ChessPiece  c  =  inPiece & CHESS_COLOR_MASK;
    ChessPiece  t  =  inPiece & CHESS_TYPE_MASK;

    if( c == CHESS_BLACK ) {

        moneyVal += pieceCaptureMoney[ t ];
        }
    }





#endif



#endif
