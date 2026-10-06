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
                int  inSpendSound,
                int  inUnusedDrawSound );


void moneyAdd( int  inValToAdd );

void moneyAddDelayed( int  inValToAdd );

void moneyAddOverrunDelayed( void );



void moneyAddCapture( ChessPiece  inPiece );

int  moneyGetTotal( void );


/* will wait until all added money and delayed money is done
   before releasing bonus */
void moneyAddUnusedDraws( int  inNumUnused );


void moneyAddBailout( int  inLevelNumber );
void moneyReleaseBailout( void );


/* adds captured money value that is delayed until later */
void moneyAddCaptureDelayed( ChessPiece  inPiece );


void moneyReleaseDelayed( void );



void moneyDraw( int  inPosX,
                int  inPosY );



void moneyDrawCoin( int            inPosX,
                    int            inPosY,
                    unsigned char  inFade );



void moneyStep( void );


/* returns 1 if all money adding animations are settled and done */
char moneyIsSettled( void );


void moneyForce( int  inVal );


char moneyGetSubMessageShowing( void );




#endif



#ifdef  MONEY_IMPLEMENTATION

#ifndef MONEY_IMPLEMENTATION_INCLUDED
#define MONEY_IMPLEMENTATION_INCLUDED




#include "chess.h"
#include "arraySizeCheck.h"
#include "memoryRegister.h"
#include "numbers.h"
#include "util.h"


/* how much money you get for winning by overrun
   The cheapest pieces in shop are 2, so you can always
   buy something, and you're never totally stuck
*/
#define  OVERRUN_MONEY_VALUE          2

#define  EXTRA_BONUS_PER_UNUSED_DRAW  0


/*
  how much money you get for capturing a given piece
  pawns are 1
  non-pawns are 3
  except for king, which is 6
*/
#define NON_PAWN_CAPTURE_VALUE  3

#define PIECE_CAPTURE_MONEY_LIST( C, V )   \
    V( C, 0,   noPiece,      0                        ) \
    V( C, 1,   pawn,         1                        ) \
    V( C, 2,   bishop,       NON_PAWN_CAPTURE_VALUE   ) \
    V( C, 3,   knight,       NON_PAWN_CAPTURE_VALUE   ) \
    V( C, 4,   rook,         NON_PAWN_CAPTURE_VALUE   ) \
    V( C, 5,   queen,        NON_PAWN_CAPTURE_VALUE   ) \
    V( C, 6,   king,         6                        ) \
    V( C, 7,   laserRook,    NON_PAWN_CAPTURE_VALUE   ) \
    V( C, 8,   laserPawn,    NON_PAWN_CAPTURE_VALUE   ) \
    V( C, 9,   doublingPawn, NON_PAWN_CAPTURE_VALUE   ) \
    V( C, 10,  addingRook,   NON_PAWN_CAPTURE_VALUE   ) \
    V( C, 11,  rocket,       NON_PAWN_CAPTURE_VALUE   )

static int pieceCaptureMoney[] = {
    MAKE_CHESS_ARRAY( PIECE_CAPTURE_MONEY_LIST )
    };

CHECK_CHESS_ARRAY( pieceCaptureMoney,
                   PIECE_CAPTURE_MONEY_LIST );



static int   coinSprite;

static int   coinSound;
static int   unusedDrawSound;
static int   spendSound;

static int   moneyVal;
static int   moneyToAdd            =  0;
static int   delayedMoneyToAdd     =  0;

static int   unusedDraws           =  0;
static char  unusedDrawsShowing    =  0;

static int   unusedDrawPreSteps    =  0;
static int   unusedDrawPostSteps   =  0;

static int   moneyBailout          =  0;


static int   moneyAddProgress;
static int   moneyAddProgressMax   =  100;
static char  moneyProgressMidPeak  =  0;

static int   lang_unusedDraws;
static int   lang_bailout;



void moneyInit( int  inStartVal,
                int  inSpendSound,
                int  inUnusedDrawSound ) {

    lang_unusedDraws = maxigin_initTranslationKey( "unusedDraws" );
    lang_bailout     = maxigin_initTranslationKey( "bailout"     );
        
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
    unusedDrawSound = inUnusedDrawSound;

    spendSound = inSpendSound;
    
    REGISTER_VAL_MEM( moneyVal );
    REGISTER_VAL_MEM( moneyToAdd );
    REGISTER_VAL_MEM( moneyAddProgress );
    REGISTER_VAL_MEM( moneyProgressMidPeak );

    REGISTER_VAL_MEM( unusedDraws );
    REGISTER_VAL_MEM( unusedDrawsShowing );

    REGISTER_VAL_MEM( moneyBailout );
    }



void moneyForce( int  inVal ) {
    moneyVal = inVal;
    moneyToAdd = 0;
    delayedMoneyToAdd = 0;
    }
    


void moneyDrawCoin( int            inPosX,
                    int            inPosY,
                    unsigned char  inFade ) {
    
    colorsApplyMoneyColor();

    maxigin_drawSetAlpha( inFade );
    
    maxigin_drawSprite( coinSprite,
                        inPosX,
                        inPosY );
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
        
    
    colorsApplyMoneyColor();

    
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

    numberDrawRublesRight( moneyVal,
                           inPosX - 9,
                           inPosY,
                           1 );

    if( glowFade > 0 ) {
        maxigin_drawSetAlpha( glowFade / 2 );

        maxigin_drawToggleAdditive( 1 );

        numberDrawRublesRight( moneyVal,
                               inPosX - 9,
                               inPosY,
                               1 );

        maxigin_drawToggleAdditive( 0 );
        maxigin_drawSetAlpha( 255 );
        }

    
    if( moneyBailout > 0
        &&
        moneyToAdd > 0 ) {

        colorsApply( COLOR_BAILOUT );

        maxigin_setLanguageFontIndex( 1 );

        maxigin_drawLangText( lang_bailout,
                              inPosX,
                              inPosY + 14,
                              MAXIGIN_RIGHT );
        numberDraw( moneyBailout,
                    inPosX + 15,
                    inPosY + 14,
                    1 );

        maxigin_setLanguageFontIndex( 0 );

        }
    else if( unusedDrawsShowing ) {

        maxigin_drawResetColor();

        maxigin_setLanguageFontIndex( 1 );

        maxigin_drawLangText( lang_unusedDraws,
                              inPosX + 5,
                              inPosY + 14,
                              MAXIGIN_RIGHT );
        numberDraw( unusedDraws,
                    inPosX + 15,
                    inPosY + 14,
                    1 );

        maxigin_setLanguageFontIndex( 0 );
        }
    }



void moneyStep( void ) {

    int  r  = mingin_getStepsPerSecond();

    if( moneyToAdd == 0
        &&
        moneyAddProgress == 0
        &&
        unusedDraws == 0
        &&
        ! unusedDrawsShowing ) {
        return;
        }

    if( moneyToAdd != 0
        ||
        moneyAddProgress != 0 ) {
        
        moneyAddProgress += ( 10 * 60 ) / r;

        if( ! moneyProgressMidPeak
            &&
            moneyAddProgress >= moneyAddProgressMax / 2 ) {

            if( moneyToAdd > 0 ) {
                moneyVal += 1;

                moneyToAdd -= 1;
                
                if( moneyBailout > 0 ) {
                    moneyBailout -= 1;
                    }

                if( moneyToAdd == 0
                    &&
                    moneyBailout > 0 ) {
                    /* done adding delayed bailout money, clear it */
                    moneyBailout = 0;
                    }
    
                maxigin_playSoundEffect( coinSound,
                                         256 );
                }
            else {
                if( moneyToAdd <= -50 ) {
                    moneyVal   -= 10;
                    moneyToAdd += 10;

                    maxigin_playSoundEffect( spendSound,
                                             512 );
                    }
                else if( moneyToAdd <= -10 ) {
                    moneyVal   -= 5;
                    moneyToAdd += 5;
                
                    maxigin_playSoundEffect( spendSound,
                                             384 );
                    }
                else {
                    moneyVal -= 1;

                    moneyToAdd += 1;
                
                    maxigin_playSoundEffect( spendSound,
                                             256 );
                    }
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
    

    if( ( unusedDraws > 0
          ||
          unusedDrawsShowing )
        &&
        moneyToAdd == 0
        &&
        delayedMoneyToAdd == 0 ) {

        int  stepDur  =  ( r * 15 ) / 60;

        if( unusedDraws > 0
            &&
            unusedDrawPreSteps < stepDur ) {
            
            unusedDrawPreSteps ++;

            if( ! unusedDrawsShowing
                &&
                unusedDrawPreSteps >= 0 ) {
                /* just crossed the threshold where we should show it */
                unusedDrawsShowing = 1;

                /* rewind back to negative, now that it's showing, to
                   give the user a chance to see it before the first decrement */
                unusedDrawPreSteps = - stepDur;
                }

            if( unusedDrawPreSteps >= stepDur ) {
                maxigin_playSoundEffect( unusedDrawSound,
                                 512 );
                unusedDraws --;
                
                unusedDrawPostSteps = 1;
                }
            }
        else if( unusedDrawPostSteps > 0
                 &&
                 unusedDrawPostSteps < stepDur ) {
            unusedDrawPostSteps ++;
            
            if( unusedDrawPostSteps >= stepDur ) {

                moneyToAdd += EXTRA_BONUS_PER_UNUSED_DRAW;
                unusedDrawPostSteps = 0;
                unusedDrawPreSteps = 0;
                }
            }
        }

    if( unusedDrawsShowing
        &&
        unusedDrawPostSteps == 0
        &&
        moneyToAdd == 0
        &&
        moneyAddProgress == 0
        &&
        unusedDraws == 0 ) {

        /* completely done adding money for unused draws */
        unusedDrawsShowing = 0;
        }
    
    }



void moneyAdd( int  inValToAdd ) {
    moneyToAdd += inValToAdd;
    }



void moneyAddDelayed( int  inValToAdd ) {
    delayedMoneyToAdd += inValToAdd;
    }


int moneyGetTotal( void ) {
    return  moneyVal + moneyToAdd;
    }



void moneyAddOverrunDelayed( void ) {
    moneyAddDelayed( OVERRUN_MONEY_VALUE );
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



void moneyAddUnusedDraws( int  inNumUnused ) {
    
    int  r        =  mingin_getStepsPerSecond();
    int  stepDur  =  ( r * 15 ) / 60;

    if( EXTRA_BONUS_PER_UNUSED_DRAW == 0 ) {
        /* nothing to add */
        return;
        }

    /* first pre-step is longer, to give previous money a chance to settle */
    unusedDrawPreSteps  =  -stepDur;
    unusedDrawPostSteps =   0;

    /* hide them at first, until first pre-step becomes positive */
    unusedDrawsShowing = 0;
    
    
    unusedDraws += inNumUnused;
    }


char moneyGetSubMessageShowing( void ) {
    /* return 1 if they are showing now or going to be showing soon */
    
    return
        unusedDraws > 0
        ||
        unusedDrawsShowing
        ||
        moneyBailout > 0;
    }



void moneyAddBailout( int  inLevelNumber ) {

    moneyBailout = 10 + inLevelNumber;

    if( moneyBailout > 25 ) {
        moneyBailout = 25;
        }
    }



void moneyReleaseBailout( void ) {
    if( moneyBailout > 0 ) {
        moneyToAdd += moneyBailout;
        }
    }



#endif



#endif
