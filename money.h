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


void moneyInit( int  inStartVal,
                int  inSpendSound );


void moneyAdd( int inValToAdd );


void moneyAddCapture( ChessPiece inPiece );

int  moneyGetTotal( void );


/* adds captured money value that is delayed until later */
void moneyAddCaptureDelayed( ChessPiece inPiece );


void moneyReleaseDelayed( void );



void moneyDraw( int  inPosX,
                int  inPosY );


void moneyStep( void );


/* returns 1 if all money adding animations are settled and done */
char moneyIsSettled( void );


void moneyForce( int  inVal );





#ifdef MONEY_IMPLEMENTATION


#include "chess.h"
#include "arraySizeCheck.h"
#include "memoryRegister.h"
#include "numbers.h"
#include "util.h"

/*
  how much money you get for capturing a given piece
  pawns are 1
  non-pawns are 2
  except for king, which is 4
*/
#define PIECE_CAPTURE_MONEY_LIST( C, V )   \
    V( C, 0,   noPiece,      0   ) \
    V( C, 1,   pawn,         1   ) \
    V( C, 2,   bishop,       2   ) \
    V( C, 3,   knight,       2   ) \
    V( C, 4,   rook,         2   ) \
    V( C, 5,   queen,        2   ) \
    V( C, 6,   king,         4 ) \
    V( C, 7,   laserRook,    2   ) \
    V( C, 8,   laserPawn,    2   ) \
    V( C, 9,   doublingPawn, 2   ) \
    V( C, 10,  addingRook,   2   ) \
    V( C, 11,  rocket,       2   )

static int pieceCaptureMoney[] = {
    MAKE_CHESS_ARRAY( PIECE_CAPTURE_MONEY_LIST )
    };

CHECK_CHESS_ARRAY( pieceCaptureMoney,
                   PIECE_CAPTURE_MONEY_LIST );



static int   coinSprite;

static int   coinSound;
static int   spendSound;

static int   moneyVal;
static int   moneyToAdd            =  0;
static int   delayedMoneyToAdd     =  0;

static int   moneyAddProgress;
static int   moneyAddProgressMax   =  100;
static char  moneyProgressMidPeak  =  0;



void moneyInit( int inStartVal,
                int inSpendSound ) {
    
    coinSprite = maxigin_initSprite( "coin.tga" );

    if( coinSprite != -1 ) {
        maxigin_initMakeGlowSprite( coinSprite,
                                    4,
                                    2 );
        }

    moneyVal = inStartVal;

    moneyAddProgress = 0;
    moneyProgressMidPeak = 0;

    coinSound = maxigin_initSoundEffect( "coin_sd_4.wav" );

    spendSound = inSpendSound;
    
    REGISTER_VAL_MEM( moneyVal );
    REGISTER_VAL_MEM( moneyToAdd );
    REGISTER_VAL_MEM( moneyAddProgress );
    REGISTER_VAL_MEM( moneyProgressMidPeak );
    }



void moneyForce( int  inVal ) {
    moneyVal = inVal;
    moneyToAdd = 0;
    delayedMoneyToAdd = 0;
    }
    



void moneyDraw( int  inPosX,
                int  inPosY ) {
    
    int            bounceY       =  0;
    unsigned char  glowFade      =  0;
    
    if( moneyAddProgress > 0 ) {

        bounceY = - parabola( moneyAddProgress,
                              moneyAddProgressMax,
                              10 );

        glowFade = (unsigned char)parabola( moneyAddProgress,
                                            moneyAddProgressMax,
                                            255 );
        }
        
    
    drawSetPieceColor( CHESS_WHITE );

    
    maxigin_drawSprite( coinSprite,
                        inPosX,
                        inPosY + bounceY );

    if( glowFade > 0 ) {
        maxigin_drawSetAlpha( glowFade );
        
        maxigin_drawSpriteGlowOnly( coinSprite,
                                    inPosX,
                                    inPosY + bounceY );

        maxigin_drawSetAlpha( glowFade / 2 );
        
        maxigin_drawSpriteGlowOnly( coinSprite,
                                    inPosX,
                                    inPosY + bounceY );
        maxigin_drawSetAlpha( 255 );
        }

    numberDraw( moneyVal,
                inPosX - 9,
                inPosY,
                1 );

    if( glowFade > 0 ) {
        maxigin_drawSetAlpha( glowFade / 2 );

        maxigin_drawToggleAdditive( 1 );

        numberDraw( moneyVal,
                    inPosX - 9,
                    inPosY,
                    1 );

        maxigin_drawToggleAdditive( 0 );
        maxigin_drawSetAlpha( 255 );
        }
    }



void moneyStep( void ) {

    int  r  = mingin_getStepsPerSecond();

    if( moneyToAdd == 0
        &&
        moneyAddProgress == 0 ) {
        return;
        }

    moneyAddProgress += ( 10 * 60 ) / r;

    if( ! moneyProgressMidPeak
        &&
        moneyAddProgress >= moneyAddProgressMax / 2 ) {

        if( moneyToAdd > 0 ) {
            moneyVal += 1;

            moneyToAdd -= 1;
    
            maxigin_playSoundEffect( coinSound,
                                     256 );
            }
        else {
            moneyVal -= 1;

            moneyToAdd += 1;
    
            maxigin_playSoundEffect( spendSound,
                                     256 );
            }

        moneyProgressMidPeak = 1;
        }
    else if( moneyProgressMidPeak
             &&
             moneyAddProgress >= moneyAddProgressMax ) {
        /* start next increment */
        moneyProgressMidPeak = 0;
        moneyAddProgress = 0;
        }

    }



void moneyAdd( int  inValToAdd ) {
    moneyToAdd += inValToAdd;
    }



int moneyGetTotal( void ) {
    return  moneyVal + moneyToAdd;
    }



static int getCaptureValue( ChessPiece  inPiece ) {
    
    ChessPiece  c    =  inPiece & CHESS_COLOR_MASK;
    ChessPiece  t    =  inPiece & CHESS_TYPE_MASK;
    
    if( c == CHESS_BLACK ) {
       return pieceCaptureMoney[ t ];
        }
    return 0;
    }



void moneyAddCapture( ChessPiece inPiece ) {
    moneyToAdd += getCaptureValue( inPiece );
    }



void moneyAddCaptureDelayed( ChessPiece inPiece ) {
    delayedMoneyToAdd += getCaptureValue( inPiece );
    }



void moneyReleaseDelayed( void ) {
    moneyToAdd += delayedMoneyToAdd;
    delayedMoneyToAdd = 0;
    }



char moneyIsSettled( void ) {
    if( moneyToAdd == 0
        &&
        moneyAddProgress == 0 ) {
        return 1;
        }
    return 0;
    }



#endif



#endif
