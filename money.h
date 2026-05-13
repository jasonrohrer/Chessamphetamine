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


/* adds captured money value that is delayed until later */
void moneyAddCaptureDelayed( ChessPiece inPiece );


void moneyReleaseDelayed( void );



void moneyDraw( int  inPosX,
                int  inPosY );


void moneyStep( void );


/* returns 1 if all money adding animations are settled and done */
char moneyIsSettled( void );




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

static int moneyFont;

static int coinSound;


static int  moneyVal;
static int  moneyToAdd            =  0;
static int  delayedMoneyToAdd     =  0;

static int  moneyAddProgress;
static int  moneyAddProgressMax   =  100;
static char moneyProgressMidPeak  =  0;



void moneyInit( int inStartVal ) {

    int  fontStrip;

    
    coinSprite = maxigin_initSprite( "coin.tga" );

    if( coinSprite != -1 ) {
        maxigin_initMakeGlowSprite( coinSprite,
                                    4,
                                    2 );
        }

    moneyVal = inStartVal;

    moneyAddProgress = 0;
    moneyProgressMidPeak = 0;
    

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

    coinSound = maxigin_initSoundEffect( "coin_sd_4.wav" );

    REGISTER_VAL_MEM( moneyVal );
    REGISTER_VAL_MEM( moneyToAdd );
    REGISTER_VAL_MEM( moneyAddProgress );
    REGISTER_VAL_MEM( moneyProgressMidPeak );
    }


static int parabola( int  inT,
                     int  inTMax,
                     int  inPeak ) {
    /* float formula is
       y = 4 * inPeak * ( inT / inTMax ) * ( 1 - inT / inTMax )

       rewritten with division at end for fixed point as:

       y = 4 * inPeak * (inT * (inTMax - inT ) ) / ( inTMax * inTMax )

    */
    
    long  tMax2  =  (long)inTMax * (long)inTMax;
    long  t      =  (long)inT;
    long  y      =  4 * inPeak * ( t * (inTMax - t ) );

    return (int)( y / tMax2 );
    }



void moneyDraw( int  inPosX,
                int  inPosY ) {

    const char    *displayText;
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

    displayText = maxigin_intToString( moneyVal );
        
    maxigin_drawText( moneyFont,
                      displayText,
                      inPosX - 9,
                      inPosY,
                      MAXIGIN_RIGHT );

    if( glowFade > 0 ) {
        maxigin_drawSetAlpha( glowFade / 2 );

        maxigin_drawToggleAdditive( 1 );
        
        maxigin_drawText( moneyFont,
                      displayText,
                      inPosX - 9,
                      inPosY,
                      MAXIGIN_RIGHT );

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

        moneyVal += 1;

        moneyToAdd -= 1;
    
        maxigin_playSoundEffect( coinSound,
                                 256 );

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
