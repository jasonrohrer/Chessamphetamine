
#include "gameSize.h"


#include "maxigin.h"





#define CHESS_IMPLEMENTATION
#define BOARD_IMPLEMENTATION
#define PARTICLE_SPRITE_IMPLEMENTATION
#define PIECE_SPRITES_IMPLEMENTATION
#define MOVE_ANIM_IMPLEMENTATION
#define MONEY_IMPLEMENTATION
#define NUMBERS_IMPLEMENTATION
#define CHECK_DISPLAY_IMPLEMENTATION
#define UTIL_IMPLEMENTATION
#define COLORS_IMPLEMENTATION
#define FIXED_MATH_IMPLEMENTATION
#define PINCH_IMPLEMENTATION
#define PARTICLE_SYSTEM_IMPLEMENTATION
#define PIECE_DESCRIPTIONS_IMPLEMENTATION
#define NAV_IMPLEMENTATION
#define LEVELS_IMPLEMENTATION
#define DECK_IMPLEMENTATION


#include "chess.h"

#include "board.h"

#include "particleSprite.h"

#include "pieceSprites.h"

#include "moveAnim.h"

#include "money.h"

#include "simTest.h"

#include "numbers.h"

#include "checkDisplay.h"

#include "util.h"

#include "colors.h"

#include "fixedMath.h"

#include "pinch.h"

#include "particleSystem.h"

#include "pieceDescriptions.h"

#include "nav.h"

#include "levels.h"

#include "deck.h"


enum GameUserAction {
    JUMP,
    ACTION,
    DRAW,
    BOMB,
    REMAP,
    CRASH,
    TOGGLE_MOVE_LOG,
    ADVANCE_MOVE_LOG,
    BACK_MOVE_LOG,
    SHIFT,
    CTRL,
    RAND_COLORS,
    ROT_COLORS_0,
    ROT_COLORS_1,
    ROT_COLORS_2,
    ROT_COLORS_ALL,
    PRINT_COLORS,
    BOX_THICK
    };

static char actionHeldDown = 0;


#define  MAX_NUM_BULLETS  20

typedef struct Vector{
        int  x;
        int  y;
    } Vector;


static  char           bulletOn[ MAX_NUM_BULLETS ];

static  Vector         bulletPos[ MAX_NUM_BULLETS ];

static  Vector         bulletSpeed[ MAX_NUM_BULLETS ];

static  unsigned char  bulletFade[ MAX_NUM_BULLETS ];


static  char           bombOn[ MAX_NUM_BULLETS ];

static  Vector         bombPos[ MAX_NUM_BULLETS ];

static  Vector         bombSpeed[ MAX_NUM_BULLETS ];

static  int            bombStripIndex[ MAX_NUM_BULLETS ];
static  int            bombProgress[ MAX_NUM_BULLETS ];
static  int            bombBurstProgress[ MAX_NUM_BULLETS ];
static  unsigned char  bombBurstFade[ MAX_NUM_BULLETS ];


static  int            stepsSinceLastBullet  =    0;
static  int            msBetweenBullets      =  100;


static  int            boxPosX               =   -1;
static  int            boxPosY               =   -1;
static  int            boxW                  =   10;
static  int            boxH                  =   40;
static  int            boxVPerSecond         =  120;
static  int            boxDir                =    1;

static  Vector         lineTip               =  { MAXIGIN_GAME_NATIVE_W / 2,
                                                  MAXIGIN_GAME_NATIVE_H / 2 };

static MaxiginRand     rand;


static int  burstVX[8] = { 0, 2, 3,  2,  0, -2, -3, -2 };
static int  burstVY[8] = { 3, 2, 0, -2, -3, -2,  0,  2 };

    
    

#define  NUM_BULK_FILES  8

static const char  *fileNames[ NUM_BULK_FILES ] = { "bullet.tga",
                                                    "bulletGlow.tga",
                                                    "bigPointer.tga",
                                                    "bigPointer2.tga",
                                                    "vertLine.tga",
                                                    "horLine.tga",
                                                    "ship.tga",
                                                    "bombParticle.tga" };

static int          spriteHandles[ NUM_BULK_FILES ];

static int          spriteStrip;
static int          buttonHintStrip;

static int          stripIndex         =   0;
static int          stripC             =   0;

static MaxiginGUI   gameGUI;
static int          sliderValue        =   7;
static int          sliderValueB       =   7;
static int          sliderValueC       =   7;

static int          plunkSound         =  -1;
static int          thunkSound         =  -1;
static int          examinePieceSound  =  -1;
static int          pickFailedSound    =  -1;

static int          checkmateGood      =  -1;
static int          checkmateBad       =  -1;



static int          spinFrameSprite            = -1;
static int          spinUnpressedSprite        = -1;
static int          spinPressedSprite          = -1;
static int          spinPressedTextSprite      = -1;
static int          spinPressedTextGlowSprite  = -1;

static int          drawButtonSprite           = -1;
static int          drawButtonFrameSprite      = -1;
static int          drawButtonBackSprite       = -1;
static char         drawButtonPressed          =  0;
static int          drawPrice                  =  1;
static int          drawButtonPosY             =  MAXIGIN_GAME_NATIVE_H - 10;
static int          drawButtonPosX             =  MAXIGIN_GAME_NATIVE_W / 2 + 25;

static char         redrawRemoveRunning        =  0;
static char         redrawAddRunning           =  0;
static int          redrawProgressMax          =  100;


static int          logoSprite                 = -1;
static int          logoSubSprite              = -1;
static int          microdoseSprite            = -1;
static int          microdoseSpriteA           = -1;
static int          microdoseSpriteB           = -1;
static int          microdoseSpriteC           = -1;
static int          jasonBylineSprite          = -1;
static int          tomBylineSprite            = -1;
static int          smallCapsFont              = -1;

static int          lang_settings;
static int          lang_newGame;
static int          lang_quit;
static int          lang_musicVolume;
static int          lang_effectsVolume;
static int          lang_fullscreen;

static int          lang_action;
static int          lang_draw;

static int          lang_bomb;

static int          lang_drawInstruct;


static BoardState     boardState;
static Captured       postMoveCaptured;
static BoardState     postMoveState;
static Move           boardMove;
static AnimProgress   moveProgress;

static Deck           playerDeck;

static char           boardMarkers[ BH ][ BW ];
static int            boardMarkersDownCount      =  0;
static char           boardMarkersHidden         =  0;

static DrawBoardLift  redrawLift;
static DrawBoardLift  redrawSmoothLift;

static char         moveMade           =  0;
static char         chessGameOver      =  0;
static char         checkmate          =  0;
static char         stalemate          =  0;
static char         drawGame           =  0;
static int          gameLoserColor     =  CHESS_WHITE;

static int          noScoreMoveCount   =  0;
static int          statesTested       =  0;
static int          movePickTime       =  0;


static int            endMessageSprites[ 3 ]  = { -1,
                                                  -1,
                                                  -1 };

static int            endMessageColor              =  CHESS_WHITE;
static int            endMessageIndex              =  -1;
static int            explodingEndMessageProgress  =  -1;
static int            explodingEndMessageMax       =  512;
static unsigned char  endMessageFade               =  0;
static int            endMessagePreFadeSteps       =  0;


static char           spinning                    =  0;

static char           showingMoveLog              =  0;
static char           moveLogButtonDown           =  1;
static int            moveLogProgress             =  0;
static char           moveLogAdvDown              =  0;
static char           moveLogBackDown             =  0;

static long           stepSec                     =  0;
static long           stepMSec                    =  0;


static int            boardCenterX;
static int            boardCenterY;
static ChessPiece     infoPanelPiece              =  noPiece;
static ChessPiece     infoPanelLastPiece          =  noPiece;
static unsigned char  infoPanelFade               =  0;

#define  INFO_HIGHLIGHT_BUFFER_SIZE  5
static int            infoRow [ INFO_HIGHLIGHT_BUFFER_SIZE ];
static int            infoCol [ INFO_HIGHLIGHT_BUFFER_SIZE ];
static unsigned char  infoFade[ INFO_HIGHLIGHT_BUFFER_SIZE ];
static int            curInfoIndex                =  0;



static void clearDrawMarkers( void ) {
    int  y;
    int  x;

    for( y = 0;
         y < BH;
         y ++ ) {
        
        for( x = 0;
             x < BW;
             x ++ ) {

            boardMarkers[y][x] = 0;
            }
        }
    boardMarkersDownCount = 0;
    }



void maxiginGame_getNativePixels( unsigned char *inRGBBuffer ) {
    
    int  boxStartX  =  boxPosX - boxW / 2;
    int  boxStartY  =  boxPosY - boxH / 2;
    int  numPixels  =  MAXIGIN_GAME_NATIVE_W * MAXIGIN_GAME_NATIVE_H;
    int  p;
    int  x;
    int  y;
    int  i;
    int  spinButtonY;
    

    maxigin_drawSetAlpha( 255 );
    
    /* black background */
    for( p = 0; p<numPixels; p++ ) {
        int pix = p * 3;

        inRGBBuffer[pix]     = 0;
        inRGBBuffer[pix + 1] = 0;
        inRGBBuffer[pix + 2] = 0;
        }

    /* now draw black box */
    if( 0 )
    for( y = boxStartY;
         y < boxStartY + boxH;
         y ++ ) {
        
        if( y >= 0
            &&
            y < MAXIGIN_GAME_NATIVE_H ) {
            
            int rowStart = y * MAXIGIN_GAME_NATIVE_W * 3;
        
            for( x = boxStartX;
                 x < boxStartX + boxW;
                 x ++ ) {
                
                if( x >=0
                    &&
                    x < MAXIGIN_GAME_NATIVE_W ) {
                    
                    int pix = rowStart + x * 3;
                    inRGBBuffer[pix] =
                        (unsigned char)( 255 * (y-boxStartY) / boxH );
                    inRGBBuffer[pix + 1] = 0;
                    inRGBBuffer[pix + 2] = 0;
                    }
                }
            }
        }

    /* green dot at center of box for sanity check */
    if( 0 ) {
        if( boxPosY >= 0
            &&
            boxPosY < MAXIGIN_GAME_NATIVE_H
            &&
            boxPosX >= 0
            &&
            boxPosX < MAXIGIN_GAME_NATIVE_W ) {
            
            int pix = boxPosY * MAXIGIN_GAME_NATIVE_W * 3 + boxPosX * 3;

            inRGBBuffer[ pix++ ] = 0;
            inRGBBuffer[ pix++ ] = 255;
            inRGBBuffer[ pix++ ] = 0;
            }
        }
    

    /* hint test */

    /*
    maxigin_drawButtonHintSprite( SHOOT,
                                  20,
                                  20 );


    maxigin_drawSetColor( 255, 255, 255, 255 );
    maxigin_drawLangText( lang_shoot,
                          30,
                          20,
                          MAXIGIN_LEFT );

    maxigin_drawButtonHintSprite( BOMB,
                                  MAXIGIN_GAME_NATIVE_W - 20,
                                  20 );

    maxigin_drawLangText( lang_bomb,
                          MAXIGIN_GAME_NATIVE_W - 30,
                          20,
                          MAXIGIN_RIGHT );
    */

    
        
    if( 0 )
    for( i = 0;
         i < 9;
         i ++ ) {

        /* vert line */
        maxigin_drawSprite( spriteHandles[4],
                            i * 20 + 79,
                            MAXIGIN_GAME_NATIVE_H /  2);
        /* hor line */
        maxigin_drawSprite( spriteHandles[5],
                            MAXIGIN_GAME_NATIVE_W /  2,
                            i * 20 + 39 );
        }

    maxigin_drawToggleAdditive( 1 );

    if( 0 )
    for( i = 0;
         i < MAX_NUM_BULLETS;
         i ++ ) {
        
        if( bulletOn[ i ] ) {
            
            maxigin_drawSetAlpha( bulletFade[ i ] );

            maxigin_drawSprite( spriteHandles[ 0 ],
                                bulletPos[ i ].x,
                                bulletPos[ i ].y );
            }
        }

    if( 0 )
    for( i = 0;
         i < MAX_NUM_BULLETS;
         i ++ ) {
        
        if( bombOn[ i ] ) {
            
            maxigin_drawSetAlpha( 255 );

            if( bombProgress[i] > 0 ) {
                
                maxigin_drawSprite(
                    maxigin_getSpriteFromStrip( spriteStrip,
                                                bombStripIndex[ i ] ),
                    bombPos[ i ].x,
                    bombPos[ i ].y );
                }
            else {
                int  b;

                for( b = 0;
                     b < 8;
                     b ++ ) {

                    maxigin_drawSetAlpha( bombBurstFade[i] );
                    
                    maxigin_drawSprite(
                        spriteHandles[7],
                        bombPos[ i ].x + burstVX[b] * bombBurstProgress[i],
                        bombPos[ i ].y + burstVY[b] * bombBurstProgress[i] );
                    }
                }
            }
        }

    if( 0 ) {
        
        maxigin_drawToggleAdditive( 1 );

        maxigin_drawSetColor( 255, 255, 255, 64 );

        maxigin_drawLine( MAXIGIN_GAME_NATIVE_W / 2,
                          MAXIGIN_GAME_NATIVE_H / 2,
                          lineTip.x,
                          lineTip.y );

        maxigin_drawToggleAdditive( 0 );
        maxigin_drawSetColor( 255, 255, 255, 255 );
        maxigin_drawSprite( spriteHandles[6],
                            lineTip.x,
                            lineTip.y );
        }
    

    if( 0)
    maxigin_drawFillRect( MAXIGIN_GAME_NATIVE_W / 2,
                          MAXIGIN_GAME_NATIVE_H / 2,
                          lineTip.x,
                          lineTip.y );

    maxigin_drawToggleAdditive( 0 );
    
    maxigin_drawSetColor( 255, 0, 0, 255 );

    if( 0 )
    maxigin_drawRect( MAXIGIN_GAME_NATIVE_W / 2,
                      MAXIGIN_GAME_NATIVE_H / 2,
                      lineTip.x,
                      lineTip.y );

    maxigin_drawResetColor();


    maxigin_drawResetColor();

    spinButtonY = MAXIGIN_GAME_NATIVE_H - 25;
    
    maxigin_drawSprite( spinFrameSprite,
                        MAXIGIN_GAME_NATIVE_W - 35,
                        spinButtonY );

    if( ! spinning ) {
        maxigin_drawSprite( spinUnpressedSprite,
                            MAXIGIN_GAME_NATIVE_W - 35,
                            spinButtonY );
        }
    else {
        maxigin_drawSprite( spinPressedSprite,
                            MAXIGIN_GAME_NATIVE_W - 35,
                            spinButtonY );
    
        maxigin_drawSprite( spinPressedTextSprite,
                            MAXIGIN_GAME_NATIVE_W - 35,
                            spinButtonY );

        maxigin_drawSpriteGlowOnly( spinPressedTextGlowSprite,
                                    MAXIGIN_GAME_NATIVE_W - 35,
                                    spinButtonY );
        }

    if( moveMade ) {
        drawMoveAnimation( boardCenterX,
                           boardCenterY,
                           &boardState,
                           &boardMove,
                           &postMoveCaptured,
                           &postMoveState,
                           &moveProgress );
        }
    else if( showingMoveLog ) {
        
        BoardState  testedState;
        int         testedBestScore;
        char        logGood;
        
        /* draw one of the logged moves */
        boardDraw( boardCenterX,
                   boardCenterY);

        logGood = getLoggedState( moveLogProgress,
                                  &testedState,
                                  &testedBestScore );

        if( logGood ) {

            drawSetPieceColor( testedState.nextToMove );
            maxigin_drawFillRect( 0,
                                  0,
                                  20,
                                  20 );

            maxigin_drawResetColor();

            numberDraw( testedState.moveCount,
                        40,
                        10,
                        1 );

            numberDraw( testedBestScore,
                        40,
                        30,
                        1 );
            
            numberDraw( getScore( &testedState ),
                        40,
                        40,
                        1 );
            
            drawBoardState( &testedState,
                            checkmate,
                            stalemate,
                            drawGame,
                            gameLoserColor,
                            &boardMove,
                            0,
                            0,
                            boardCenterX,
                            boardCenterY,
                            0,
                            0 );
            }
        }
    else {
        /* draw regular board (no move log) */

        boardDraw( boardCenterX,
                   boardCenterY);

        if( ! spinning
            &&
            ! chessGameOver
            &&
            ! boardMarkersHidden ) {

            boardDrawMarkers( boardCenterX,
                              boardCenterY,
                              boardMarkers );
            }

        drawBoardState( &boardState,
                        checkmate,
                        stalemate,
                        drawGame,
                        gameLoserColor,
                        &boardMove,
                        0,
                        0,
                        boardCenterX,
                        boardCenterY,
                        0,
                        &redrawSmoothLift );
        }
    

    
    if( explodingEndMessageProgress != -1 ) {

        unsigned char  a;

        drawSetPieceColor( endMessageColor );


        a = (unsigned char)( ( (long)( explodingEndMessageMax -
                                       explodingEndMessageProgress ) * 255 )
                             / explodingEndMessageMax );
        
        maxigin_drawSprite( endMessageSprites[ endMessageIndex ],
                            boardCenterX,
                            boardCenterY );

        
        maxigin_drawToggleAdditive( 1 );


        if( endMessageColor == CHESS_WHITE ) {
            /* desaturate gold color to make it clip white */
            maxigin_drawSetColorSaturation( 8500 );
            }

        maxigin_drawExplodingSprite( endMessageSprites[ endMessageIndex ],
                                     getParticleSprite(),
                                     boardCenterX,
                                     boardCenterY,
                                     boardCenterX,
                                     boardCenterY,
                                     BOARD_SQUARE_SIZE / 4,
                                     explodingEndMessageProgress,
                                     explodingEndMessageMax,
                                     a );
    
        maxigin_drawToggleAdditive( 0 );
        

        }
    else if( chessGameOver
             &&
             endMessageFade > 0 ) {

        drawSetPieceColor( endMessageColor );

        maxigin_drawSetAlpha( endMessageFade );

        maxigin_drawSprite( endMessageSprites[ endMessageIndex ],
                            boardCenterX,
                            boardCenterY );

        }

    if( ! spinning
        &&
        ! chessGameOver ) {

        const char  *drawString;
        
        int           buttonSink  =  0;
        
        maxigin_drawResetColor();


        drawString = maxigin_getLangText( lang_drawInstruct );

        maxigin_setLanguageFontIndex( 1 );
    
        maxigin_drawLangTextString(
                drawString,
                MAXIGIN_GAME_NATIVE_W / 2,
                drawButtonPosY,
                MAXIGIN_RIGHT );
        maxigin_setLanguageFontIndex( 0 );


        if( drawButtonPressed ) {
            buttonSink = 3;
            }

        if( boardMarkersDownCount > 0 ) {
            
            maxigin_drawSprite( drawButtonBackSprite,
                                drawButtonPosX,
                                drawButtonPosY - 9 );

            maxigin_drawSprite( drawButtonSprite,
                                drawButtonPosX,
                                drawButtonPosY - 2 + buttonSink );
        
            maxigin_drawSprite( drawButtonFrameSprite,
                                drawButtonPosX,
                                drawButtonPosY );

            maxigin_drawButtonHintSprite( DRAW,
                                          drawButtonPosX + 30,
                                          drawButtonPosY );
            
            numberDraw( drawPrice,
                        drawButtonPosX + 50,
                        drawButtonPosY,
                        0 );
            
            }
        }
    

    moneyDraw( MAXIGIN_GAME_NATIVE_W - 20,
               30 );

    checkDisplayDraw( boardCenterX,
                      boardCenterY );

    if( 0 ) {

        /* debugging numbers */

        int  xPos = MAXIGIN_GAME_NATIVE_W - 30;
        
        maxigin_drawResetColor();
        
        numberDraw( (int)stepSec,
                    xPos,
                    40,
                    1 );

        numberDraw( (int)stepMSec,
                    xPos,
                    50,
                    1 );

        numberDraw( statesTested,
                    xPos,
                    60,
                    1 );

        numberDraw( movePickTime,
                    xPos,
                    70,
                    1 );

        if( showingMoveLog ) {
            numberDraw( moveLogProgress,
                        xPos,
                        80,
                        1 );
            }
        numberDraw( boardState.moveCount,
                    xPos,
                    90,
                    1 );
        }
    
    
    
    if( 0 ) {

        int  cX = MAXIGIN_GAME_NATIVE_W / 2;
        int  cY = MAXIGIN_GAME_NATIVE_H / 2;
        
        maxigin_drawResetColor();

        maxigin_drawSetColor( 0,
                              0,
                              0,
                              255 );
        maxigin_drawFillRect( 0,
                              0,
                              MAXIGIN_GAME_NATIVE_W,
                              MAXIGIN_GAME_NATIVE_H );

        maxigin_drawResetColor();

        if( 1 ) {
            /*const  char  *phrase  =  "CHECKMATE IN ONE... ROCKET?";*/
            const  char  *phrase  =  "TRADING ROCKETS";
            
            
            maxigin_drawText( smallCapsFont,
                              phrase,
                              cX,
                              cY,
                              MAXIGIN_CENTER );
            }
        else if( 0 ) {
            
            maxigin_drawSprite( logoSprite,
                                cX,
                                cY );

            maxigin_drawSetColor( 255,
                                  0,
                                  0,
                                  255 );
            maxigin_drawSprite( logoSubSprite,
                                cX,
                                cY + 11 );

            if( 1 ) {
                maxigin_drawResetColor();
                
                maxigin_drawSprite( jasonBylineSprite,
                                cX,
                                cY - 20 );
                maxigin_drawSprite( tomBylineSprite,
                                cX,
                                cY + 48 );
                }
            }
        else {

            
            
            maxigin_drawSprite( microdoseSprite,
                                cX,
                                cY );

            if( 0 ) {
                int  border  =  10;
                
                maxigin_drawResetColor();

                maxigin_drawRect( cX - 90 - border,
                                  cY - 13 - border,
                                  cX + 90 + border,
                                  cY + 13 + border );
                }
            
            }
        
        }
    

    /*
    maxigin_drawSetColor( 255,
                          64,
                          64,
                          128 );
    maxigin_drawSpriteGlowOnly( logoSubSprite,
                        boardCenterX,
                        boardCenterY + 12 );
    */

    if( infoPanelPiece != noPiece ) {
        drawPieceInfoPanel( infoPanelPiece,
                            MAXIGIN_GAME_NATIVE_W - 42,
                            boardCenterY,
                            infoPanelFade );
        
        }
    else if( infoPanelLastPiece != noPiece
             &&
             infoPanelFade > 0 ) {
        drawPieceInfoPanel( infoPanelLastPiece,
                            MAXIGIN_GAME_NATIVE_W - 42,
                            boardCenterY,
                            infoPanelFade );
        }

    for( i = 0;
         i < INFO_HIGHLIGHT_BUFFER_SIZE;
         i ++ ) {

        if( infoFade[ i ] >   0
            &&
            infoRow [ i ] != -1
            &&
            infoCol [ i ] != -1 ) {
            
            drawPieceHighlight( &boardState,
                                boardCenterX,
                                boardCenterY,
                                infoRow[ i ],
                                infoCol[ i  ],
                                infoFade[ i ] );
            }
        }
    
        
        
                
    
    maxigin_drawGUI( &gameGUI );

    pinchApply( inRGBBuffer );
    }




static void fireBomb( int  inX,
                      int  inY ) {

    int  i;

    for( i = 0;
         i < MAX_NUM_BULLETS;
         i ++ ) {

        if( ! bombOn[ i ] ) {
            /* found empty bullet slot */

            bombOn[ i ] = 1;

            bombPos[ i ].x = inX;
            bombPos[ i ].y = inY;

            bombSpeed[ i ].x = 0;
            bombSpeed[ i ].y = -2;

            bombStripIndex[ i ] = 0;

            bombProgress[ i ] = maxigin_randRange( &rand,
                                                   4,
                                                   8 );

            bombBurstProgress[ i ] = 0;

            bombBurstFade[ i ] = 255;
            break;
            }
        }
    
    
    }



static char  remappingJump = 0;

static char  randColorsDown = 0;

static char  printColorsDown = 0;



void maxiginGame_step( void ) {
    
    int   r;
    int   i;
    int   boxVPerStep;
    
    char  pointerLive;
    int   pointerX;
    int   pointerY;
    /*
    int   panel;
    int   subPanel;
    */
    
    char  stickLive;

    /* leaving these uninitialized generates a warning in some compilers */
    int   stickPos         =  0;
    int   stickLowerRange  =  0;
    int   stickUpperRange  =  0;

    int   deltaFade;
    
    
    if( 0 ) {
        int  mouseX;
        int  mouseY;

        char avail = maxigin_getPointerLocation( &mouseX,
                                                 &mouseY );

        if( avail ) {
            pinchSet( mouseX,
                      mouseY,
                      mouseY * 1024 / MAXIGIN_GAME_NATIVE_W,
                      mouseX );
            }

        }
    
    mingin_getRunningTime( &stepSec,
                           &stepMSec );
    
    stripC ++;

    if( stripC >= 10 ) {
        stripC = 0;
        
        stripIndex ++;

        if( stripIndex >= maxigin_getNumSpritesInStrip( spriteStrip ) ) {
            stripIndex = 0;
            }

        for( i = 0;
             i < MAX_NUM_BULLETS;
             i ++ ) {
        
            if( bombOn[ i ] ) {

                bombStripIndex[ i ] ++;
                
                if( bombStripIndex[i] >=
                    maxigin_getNumSpritesInStrip( spriteStrip ) ) {
                    bombStripIndex[i] = 0;
                    }

                if( bombProgress[ i ] > 0 ) {
                    bombProgress[ i ] --;

                    if( bombProgress[ i ] == 0 ) {
                        maxigin_playSoundEffect( thunkSound,
                                                 512 );
                        }
                    }
                }
            }          
        }

    for( i = 0;
         i < MAX_NUM_BULLETS;
         i ++ ) {
        
        if( bombOn[ i ]
            &&
            bombProgress[ i ] == 0 ) {

            bombBurstProgress[i] ++;

            if( bombBurstFade[i] > 4 ) {
                bombBurstFade[i] -= 4;
                }
            else {
                bombOn[i] = 0;
                }
            }
        }
    
        
        

    for( i = 0;
         i < NUM_BULK_FILES;
         i ++ ) {
    
        if( mingin_getBulkDataChanged( fileNames[ i ] ) ) {
            int  bulkHandle;
            int  bulkSize;

            mingin_log( "Bulk data changed: " );
            mingin_log( fileNames[ i ] );
            mingin_log( "\n" );

            /* open again to reset changed status */

            bulkHandle = mingin_startReadBulkData( fileNames[ i ],
                                                   & bulkSize );

            if( bulkHandle != -1 ) {
                mingin_endReadBulkData( bulkHandle );
                }
            }
        }
        
    if( remappingJump ) {
        MinginButton last = mingin_getLastButtonPressed();

        if( last != MGN_BUTTON_NONE ) {
            MinginButton newJumpMapping[2] = { MGN_MAP_END, MGN_MAP_END };
            newJumpMapping[0] = last;

            /* overwrite mapping for jump */
            maxigin_registerButtonMapping( JUMP, newJumpMapping );
            remappingJump = 0;
            }
        }

    r = mingin_getStepsPerSecond();

    stepsSinceLastBullet ++;

    if( ! spinning
        &&
        ! chessGameOver
        &&
        ( maxigin_isButtonDown( JUMP ) ) ) {

        spinning = 1;

        maxigin_playSoundEffect( thunkSound,
                                 512 );
        }

    if( ! spinning ) {
        if( ! moveLogButtonDown ) {

            if( maxigin_isButtonDown( TOGGLE_MOVE_LOG ) ) {
                moveLogButtonDown = 1;
                
                showingMoveLog  =  ! showingMoveLog;
                moveLogProgress =  0;
                }
            }
        }

    if( maxigin_isButtonDown( RAND_COLORS ) ) {

        if( ! randColorsDown ) {
            randColorsDown = 1;
            colorsSetRandom();
            }
        }
    else {
        randColorsDown = 0;
        }

    if( maxigin_isButtonDown( ROT_COLORS_0 ) ) {
        colorsRotate( 0 );
        }
    if( maxigin_isButtonDown( ROT_COLORS_1 ) ) {
        colorsRotate( 1 );
        }
    if( maxigin_isButtonDown( ROT_COLORS_2 ) ) {
        colorsRotate( 2 );
        }
    if( maxigin_isButtonDown( ROT_COLORS_ALL ) ) {
        colorsRotate( -1 );
        }

    if( maxigin_isButtonDown( PRINT_COLORS ) ) {

        if( ! printColorsDown ) {
            colorsPrint();
            printColorsDown = 1;
            }
        }
    else {
        printColorsDown = 0;
        }

    moveLogButtonDown = maxigin_isButtonDown( TOGGLE_MOVE_LOG );

    if( showingMoveLog ) {
        int  amount = 1;

        if( maxigin_isButtonDown( CTRL ) ) {
            amount = 10;
            }
        
        if( ! moveLogAdvDown
            &&
            maxigin_isButtonDown( ADVANCE_MOVE_LOG ) ) {
            moveLogAdvDown = 1;
            moveLogProgress += amount;
            }
        if( ! moveLogBackDown
            &&
            maxigin_isButtonDown( BACK_MOVE_LOG ) ) {
            moveLogBackDown = 1;
            moveLogProgress -= amount;
            if( moveLogProgress < 0 ) {
                moveLogProgress = 0;
                }
            }
        }
    moveLogAdvDown = maxigin_isButtonDown( ADVANCE_MOVE_LOG );
    moveLogBackDown = maxigin_isButtonDown( BACK_MOVE_LOG );

    if( maxigin_isButtonDown( SHIFT ) ) {
        int  amount = 1;

        if( maxigin_isButtonDown( CTRL ) ) {
            amount = 10;
            }
        
        
        if( moveLogAdvDown ) {
            moveLogProgress += amount;
            }
        else if( moveLogBackDown ) {
            moveLogProgress -= amount;
            if( moveLogProgress < 0 ) {
                moveLogProgress = 0;
                }
            }
        }
    
    
    if( spinning ) {

        if( ! moveMade
            &&
            ! chessGameOver ) {

            MaxiginTimer  timer  =  maxigin_startTimer();
            
            /* make a chess move */
            if( getChessMove( &boardState,
                              &boardMove,
                              &postMoveCaptured,
                              &postMoveState ) ) {

                initMoveAnimation( &boardState,
                                   &boardMove,
                                   &postMoveCaptured,
                                   &postMoveState,
                                   &moveProgress );
                moveMade     = 1;
                }
            else {
                /* failed to make a move and not checkmated */

                /* trapped condition */

                stalemate     = 1;
                chessGameOver = 1;

                if( boardState.nextToMove == CHESS_BLACK ) {
                    maxigin_playSoundEffect( checkmateGood,
                                             512 );
                    endMessageColor = CHESS_WHITE;
                    gameLoserColor  = CHESS_BLACK;
                    }
                else {
                    maxigin_playSoundEffect( checkmateBad,
                                             512 );
                    endMessageColor = CHESS_BLACK;
                    gameLoserColor  = CHESS_WHITE;
                    }

                /* start TRAPPED explosion */
                endMessageIndex = 1;
                explodingEndMessageProgress = 0;
                endMessageFade = 255;
                endMessagePreFadeSteps = 0;
                }

            statesTested = getStateCountLastMove();
            movePickTime = maxigin_getElapsedMilliseconds( timer );
            }
        }
    else if( showingMoveLog ) {
        }
    

    if( moveMade ) {

        char moveDone = stepMoveAnimation( &boardState,
                                           &boardMove,
                                           &postMoveCaptured,
                                           &postMoveState,
                                           &moveProgress );

        if( moveDone ) {

            if( isCheckmate( &postMoveState,
                             &gameLoserColor ) ) {

                /* move led to checkmate */
                if( gameLoserColor == CHESS_BLACK ) {
                    maxigin_playSoundEffect( checkmateGood,
                                             512 );
                    endMessageColor = CHESS_WHITE;
                    }
                else {
                    maxigin_playSoundEffect( checkmateBad,
                                             512 );
                    endMessageColor = CHESS_BLACK;
                    }

                checkmate = 1;
                chessGameOver = 1;

                /* start checkmate explosion */
                
                endMessageIndex = 0;
                explodingEndMessageProgress = 0;
                endMessageFade = 255;
                endMessagePreFadeSteps = 0;
                }
            else if( postMoveCaptured.num == 0 ) {
                
                noScoreMoveCount ++;

                if( noScoreMoveCount > 50 ) {
                    /* overrun condition */
                    
                    int  newScore  =  getScore( &postMoveState );

                    drawGame      = 1;
                    chessGameOver = 1;

                    if( newScore >= 0 ) {
                        /* for now, count tie overrun as win for white */
                        maxigin_playSoundEffect( checkmateGood,
                                                 512 );
                        endMessageColor = CHESS_WHITE;
                        gameLoserColor  = CHESS_BLACK;
                        }
                    else {
                        maxigin_playSoundEffect( checkmateBad,
                                                 512 );
                        endMessageColor = CHESS_BLACK;
                        gameLoserColor  = CHESS_WHITE;
                        }

                    /* start OVERRUN explosion */
                    endMessageIndex = 2;
                    explodingEndMessageProgress = 0;
                    endMessageFade = 255;
                    endMessagePreFadeSteps = 0;
                    }
                }
            else {
                noScoreMoveCount = 0;
                }
            
            
            applyMove( &boardState,
                       &boardMove,
                       &postMoveState );
            
            moveMade = 0;

            if( chessGameOver ) {
                spinning = 0;
                }
            }
        }

    if( ! spinning ) {

        int         mouseX;
        int         mouseY;
        ChessPiece  oldPiece  =  infoPanelPiece;
        int         panRow    =  infoRow[ curInfoIndex ];
        int         panCol    =  infoCol[ curInfoIndex ];
        
        if( ! maxigin_getPointerLocation( &mouseX,
                                          &mouseY ) ) {

            /* pointer not present on this platform */
            
            int  curRow = -1;
            int  curCol = -1;
            int  dirX;
            int  dirY;

            /* prioritize x dir, no diagonals since they are
               ambiguous in sparse grid */
            navGetDir( 0,
                       &dirX,
                       &dirY );

            if( dirX != 0
                ||
                dirY != 0 ) {
            
                if( infoPanelPiece == noPiece ) {

                    /* allow enter based on dir */


                    if( dirX == -1 ) {
                        curCol = BW;
                        curRow = 0;
                        }
                    else if( dirX == 1 ) {
                        curCol = -1;
                        curRow =  0;
                        }
                    else if( dirY == -1 ) {
                        curRow = BH;
                        curCol = 0;
                        }
                    else if( dirY == 1 ) {
                        curRow = -1;
                        curCol =  0;
                        }
                    }
                else {
                    /* take current position */
                    curRow = infoRow[ curInfoIndex ];
                    curCol = infoCol[ curInfoIndex ];
                    }

                curRow += dirY;
                curCol += dirX;

                if( dirX > 0
                    &&
                    curCol >= BW ) {
                    curCol = 0;
                    curRow ++;
                    if( curRow >= BH ) {
                        curRow = 0;
                        }
                    }
                if( dirX < 0
                    &&
                    curCol < 0 ) {
                    curCol =  BW - 1;
                    curRow --;
                    if( curRow < 0 ) {
                        curRow = BH - 1;
                        }
                    }

                if( dirY > 0
                    &&
                    curRow >= BW ) {
                    curRow = 0;
                    curCol ++;
                    if( curCol >= BW ) {
                        curCol = 0;
                        }
                    }
                if( dirY < 0
                    &&
                    curRow < 0 ) {
                    curRow =  BH - 1;
                    curCol --;
                    if( curCol < 0 ) {
                        curCol = BW - 1;
                        }
                    }

                while( boardState.grid[ curRow ][ curCol ] == noPiece ) {

                    if( dirX != 0 ) {
                        curCol += dirX;

                        if( dirX > 0
                            &&
                            curCol >= BW ) {
                            curCol = 0;
                            curRow ++;
                            if( curRow >= BH ) {
                                curRow = 0;
                                }
                            }
                        if( dirX < 0
                            &&
                            curCol < 0 ) {
                            curCol =  BW - 1;
                            curRow --;
                            if( curRow < 0 ) {
                                curRow = BH - 1;
                                }
                            }
                        }
                    else {
                        /* move in y dir only */
                        curRow += dirY;

                        if( dirY > 0
                            &&
                            curRow >= BH ) {
                            curRow = 0;
                            curCol ++;
                            if( curCol >= BW ) {
                                curCol = 0;
                                }
                            }
                        if( dirY < 0
                            &&
                            curRow < 0 ) {
                            curRow =  BH - 1;
                            curCol --;
                            if( curCol < 0 ) {
                                curCol = BW - 1;
                                }
                            }
                        }
                    }

                infoPanelPiece = boardState.grid[ curRow ][ curCol ];

                panRow = curRow;
                panCol = curCol;

                maxigin_playSoundEffect( examinePieceSound,
                                         256 );
                }
            }
        else {
            /* mouse available */
            
            infoPanelPiece  =  getPointerOverPiece( &boardState,
                                                    boardCenterX,
                                                    boardCenterY,
                                                    &panRow,
                                                    &panCol );

            if( oldPiece != infoPanelPiece
                &&
                infoPanelPiece != noPiece ) {
                maxigin_playSoundEffect( examinePieceSound,
                                         256 );
                }

            
                
            }
        

        if( ! chessGameOver
            &&
            infoPanelPiece != noPiece
            &&
            maxigin_isButtonDown( ACTION )
            &&
            ! actionHeldDown ) {

            if( ( infoPanelPiece & CHESS_COLOR_MASK ) == CHESS_WHITE
                &&
                ( infoPanelPiece & CHESS_TYPE_MASK  ) != king ) {

                if( ! boardMarkers[ panRow ][ panCol ] ) {
                    playBeepDownSound();
                    boardMarkersDownCount ++;
                    }
                else {
                    playBeepUpSound();
                    boardMarkersDownCount --;
                    }
                
                boardMarkers[ panRow ][ panCol ] =
                    ! boardMarkers[ panRow ][ panCol ];
                }
            else {
                /* tried to click on an unpickable piece */
                maxigin_playSoundEffect( pickFailedSound,
                                         256 );
                }
            actionHeldDown = 1;
            }
            

        if( infoPanelPiece != noPiece ) {
            infoPanelFade = 255;

            if( panRow != infoRow[ curInfoIndex ]
                ||
                panCol != infoCol[ curInfoIndex ] ) {

                curInfoIndex ++;

                if( curInfoIndex >= INFO_HIGHLIGHT_BUFFER_SIZE ) {
                    curInfoIndex = 0;
                    }
                infoRow [ curInfoIndex ] = panRow;
                infoCol [ curInfoIndex ] = panCol;
                }
            
            infoFade[ curInfoIndex ] = 255;
            }

        if( oldPiece != noPiece
            &&
            oldPiece != infoPanelPiece ) {
            
            infoPanelLastPiece = oldPiece;
            }                       
        }
    else {
        infoPanelPiece = noPiece;
        }

    if( ! maxigin_isButtonDown( ACTION ) ) {
        actionHeldDown = 0;
        }

    deltaFade = ( 20 * 60 ) / r;

    if( infoPanelPiece == noPiece ) {
        
            
        if( infoPanelFade > 0 ) {
        
            if( infoPanelFade > deltaFade ) {
            
                infoPanelFade = (unsigned char)( infoPanelFade - deltaFade );
                }
            else {
                infoPanelFade = 0;
                }
            }
        if( infoFade[ curInfoIndex ] > 0 ) {
            
            if( infoFade[ curInfoIndex ] > deltaFade ) {
            
                infoFade[ curInfoIndex ] =
                    (unsigned char)( infoFade[ curInfoIndex ] - deltaFade );
                }
            else {
                infoFade[ curInfoIndex ] = 0;
                }
            }
        }
    

    for( i = 0;
         i < INFO_HIGHLIGHT_BUFFER_SIZE;
         i ++ ) {

        if( i != curInfoIndex
            &&
            infoFade[ i ] > 0 ) {

            if( infoFade[ i ] > deltaFade ) {
            
                infoFade[ i ] = (unsigned char)( infoFade[ i ] - deltaFade );
                }
            else {
                infoFade[ i ] = 0;
                }
            }
        }
    

    if( boardMarkersDownCount > 0 ) {

        
        if( ! drawButtonPressed ) {

            

            if( maxigin_isButtonDown( DRAW ) ) {
                
                                         
                drawButtonPressed = 1;
                }
            else if( maxigin_isButtonDown( ACTION ) ) {

                if( maxigin_getPointerLocation( &pointerX,
                                                &pointerY ) ) {

                    int  buttonW;
                    int  buttonH;
                    
                    maxigin_getSpriteDimensions( drawButtonSprite,
                                                 &buttonW,
                                                 &buttonH );
                    if( pointerX >= drawButtonPosX - buttonW / 2
                        &&
                        pointerX <= drawButtonPosX + buttonW / 2 - 1
                        &&
                        pointerY >= drawButtonPosY - buttonH / 2 - 2
                        &&
                        pointerY <= drawButtonPosY + buttonH / 2 - 2 ) {

                        drawButtonPressed = 1;
                        }
                    }
                }

            if( drawButtonPressed ) {

                /* press attempt */
                if( moneyGetTotal() < drawPrice ) {
                    /* fail */
                    maxigin_playSoundEffect( pickFailedSound,
                                             256 );
                    }
                else {
                    maxigin_playSoundEffect( examinePieceSound,
                                             256 );
                    redrawRemoveRunning = 1;
                    redrawAddRunning    = 0;

                    moneyAdd( - drawPrice );
                    }
                }
            }
        }
    

    if( drawButtonPressed ) {

        
        int  scaleFactor  =  ( redrawProgressMax * redrawProgressMax )
            / MAXIGIN_GAME_NATIVE_H;
        
        int   minLiftForNextStart  =  30;
        int   stepSize             =  ( 4 * 60 ) / r;

        
        if( redrawRemoveRunning
            &&
            moneyIsSettled() ) {

            int   y;
            int   x;

            char  foundStarting        =  0;
            char  allAtEnd             =  1;
            
            boardMarkersHidden  = 1;
            
            for( y = 0;
                 y < BH;
                 y ++ ) {
                
                for( x = 0;
                     x < BW;
                     x ++ ) {

                    if( ! boardMarkers[y][x] ) {
                        continue;
                        }

                    if( redrawLift.grid[y][x] < redrawProgressMax ) {
                        
                        int  val;

                        allAtEnd = 0;

                        if( redrawLift.grid[y][x] == 0 ) {
                            maxigin_playSoundEffect( examinePieceSound,
                                                     256 );
                            }
                        
                        redrawLift.grid[y][x] += stepSize;
                        
                        if( redrawLift.grid[y][x] > redrawProgressMax ) {
                            redrawLift.grid[y][x] = redrawProgressMax;
                            }

                        val = redrawLift.grid[y][x];
                        
                        redrawSmoothLift.grid[y][x] =
                            ( val * val ) / scaleFactor;
                        
                        if( redrawLift.grid[y][x] < minLiftForNextStart ) {
                            foundStarting = 1;
                            break;
                            }
                        }
                    }
                if( foundStarting ) {
                    break;
                    }
                }
            
            if( allAtEnd ) {

                /* swap pieces while off screen */

                for( y = 0;
                     y < BH;
                     y ++ ) {
                    for( x = 0;
                         x < BW;
                         x ++ ) {

                        if( boardMarkers[y][x] ) {
                            
                            boardState.grid[y][x] = deckDraw( &playerDeck );
                            }
                        }
                    }
                
                redrawRemoveRunning = 0;
                redrawAddRunning    = 1;
                }
            }
            
        if( redrawAddRunning ) {
            
            int   y;
            int   x;

            char  foundStarting        =  0;
            char  allAtEnd             =  1;
            
            for( y = 0;
                 y < BH;
                 y ++ ) {
                
                for( x = 0;
                     x < BW;
                     x ++ ) {

                    if( ! boardMarkers[y][x] ) {
                        continue;
                        }

                    if( redrawLift.grid[y][x] > 0 ) {

                        int  val;

                        allAtEnd = 0;
                        
                        redrawLift.grid[y][x] -= stepSize;
                        
                        if( redrawLift.grid[y][x] < 0 ) {
                            redrawLift.grid[y][x] = 0;
                            }

                        val = redrawLift.grid[y][x];
                        
                        redrawSmoothLift.grid[y][x] =
                            ( val * val ) / scaleFactor;

                        if( val == 0 ) {
                            maxigin_playSoundEffect( examinePieceSound,
                                                     256 );
                            }
                        
                        if( redrawLift.grid[y][x] >
                            redrawProgressMax - minLiftForNextStart ) {
                            
                            foundStarting = 1;
                            break;
                            }
                        }
                    }
                if( foundStarting ) {
                    break;
                    }
                }

            if( allAtEnd ) {
                redrawAddRunning = 0;

                drawButtonPressed = 0;

                /* price goes up for future redraws */
                drawPrice += 1;

                clearDrawMarkers();
                boardMarkersHidden = 0;
                clearDrawLift( &redrawLift );
                clearDrawLift( &redrawSmoothLift );
                }
            }
        }    
    
            

    
    if( explodingEndMessageProgress != -1 ) {
        explodingEndMessageProgress += ( 10 * 60 ) / r;

        if( explodingEndMessageProgress >= explodingEndMessageMax ) {
            explodingEndMessageProgress = -1;

            /* when end-game message done exploding, release any pent-up
               money */
            moneyReleaseDelayed();
            }
        }
    else if( chessGameOver
             &&
             endMessageFade > 0 ) {

        if( endMessagePreFadeSteps < ( 30 * 60 ) / r ) {
            endMessagePreFadeSteps ++;
            }
        else {

            int  newFade = endMessageFade - ( 5 * 60 ) / r;

            if( newFade < 0 ) {
                endMessageFade = 0;
                }
            else {
                endMessageFade = (unsigned char)newFade;
                }
            }
        }


    moneyStep();
    checkDisplayStep();

    navStep();

        
    if( ! remappingJump && maxigin_isButtonDown( REMAP ) ) {
        /* clear last button memory */
        mingin_getLastButtonPressed();
        remappingJump = 1;
        }


    if( maxigin_isButtonDown( BOMB ) ) {
        int  msSinceLastBullet  =  (stepsSinceLastBullet * 1000 ) / r;

        if( msSinceLastBullet > msBetweenBullets ) {
            stepsSinceLastBullet = 0;

            fireBomb( boxPosX, boxPosY );

            maxigin_playSoundEffect( thunkSound,
                                     64 );
            }
        }
    
    if( maxigin_isButtonDown( CRASH ) ) {

        int *baddAddress = (int*)0;

        baddAddress[0] = 5;
        }
    
    
    

    /* move box */

    
    pointerLive = maxigin_getPointerLocation( & pointerX,
                                              & pointerY );
    if( pointerLive ) {
        boxPosX = pointerX;
        boxPosY = pointerY;

        lineTip.x = pointerX;
        lineTip.y = pointerY;
        }
    else {
        /* auto-move box, no on-screen pointer */
        
        boxVPerStep = boxVPerSecond / r;
    
        if( boxVPerStep < 1 ) {
            boxVPerStep = 1;
            }
        boxPosX += boxDir * boxVPerStep;

        if( boxDir > 0 && boxPosX >= MAXIGIN_GAME_NATIVE_W ) {
            boxDir = -1;
            }
        else if( boxDir < 0 && boxPosX <= 0 ) {
            boxDir = 1;
            }

        lineTip.x = boxPosX;
        lineTip.y = boxPosY;
        }

    stickLive = mingin_getStickPosition( BOX_THICK,
                                         & stickPos,
                                         & stickLowerRange,
                                         & stickUpperRange );

    if( stickLive ) {
        
        int  mid       =
            ( stickUpperRange - stickLowerRange ) / 2
            + stickLowerRange;
        
        int  halfSpan  =  ( stickUpperRange - mid );
        
        boxW = 20 + ( ( stickPos - mid ) * 20 ) / halfSpan;
        boxW *= 2;
        }
    else {
        /* no stick, default */
        boxW = 10;
        }


    /* move bullets */
    for( i = 0;
         i < MAX_NUM_BULLETS;
         i ++ ) {

        if( bulletOn[ i ] ) {

            int  newBulletFade;
            
            bulletPos[ i ].x += bulletSpeed[ i ].x;
            bulletPos[ i ].y += bulletSpeed[ i ].y;

            /* check for out of bounds */
            
            if( bulletPos[ i ].x >= MAXIGIN_GAME_NATIVE_W ||
                bulletPos[ i ].x <= 0 ) {
                bulletOn[ i ] = 0;
                }

            if( bulletPos[ i ].y >= MAXIGIN_GAME_NATIVE_H ||
                bulletPos[ i ].y <= 0 ) {
                bulletOn[ i ] = 0;
                }

            newBulletFade = bulletFade[ i ] - 3;

            if( newBulletFade < 0 ) {
                newBulletFade = 0;
                }
            bulletFade[ i ] = (unsigned char)newBulletFade;
            
            if( bulletFade[ i ] == 0 ) {
                bulletOn[ i ] = 0;
                }
            }
        }


    /* move bombs */
    for( i = 0;
         i < MAX_NUM_BULLETS;
         i ++ ) {

        if( bombOn[ i ] ) {
            
            bombPos[ i ].x += bombSpeed[ i ].x;
            bombPos[ i ].y += bombSpeed[ i ].y;

            /* ignore out of bounds, fade will handle destruction */
            }
        }

    /*
    maxigin_startGUI( &gameGUI );

    panel = maxigin_guiStartPanel( &gameGUI,
                                   sliderValueC,
                                   40,
                                   120,
                                   104,
                                   255 );
    
    maxigin_guiSlider( &gameGUI,
                       &sliderValue,
                       0,
                       10,
                       -50,
                       50,
                       25,
                       10,
                       20,
                       10 );

    subPanel = maxigin_guiStartPanel( &gameGUI,
                                      0,
                                      -30 + sliderValueC / 10,
                                      104,
                                      32,
                                      255 );

    maxigin_guiSlider( &gameGUI,
                       &sliderValueB,
                       0,
                       200,
                       -50,
                       50,
                       0,
                       10,
                       20,
                       10 );
    
    maxigin_guiEndPanel( &gameGUI,
                         subPanel );

    maxigin_guiEndPanel( &gameGUI,
                         panel );
                         

    panel = maxigin_guiStartPanel( &gameGUI,
                                   -50,
                                   -50,
                                   120,
                                   104,
                                   255 );
    maxigin_guiSlider( &gameGUI,
                       &sliderValueC,
                       0,
                       200,
                       -50,
                       50,
                       0,
                       10,
                       20,
                       10 );
    
    maxigin_guiEndPanel( &gameGUI,
                         panel );
    
    maxigin_endGUI( &gameGUI );
    */
    }



static MinginButton jumpMapping[]    =  { MGN_KEY_SPACE,     MGN_MAP_END };

static MinginButton moveLogMapping[] =  { MGN_KEY_Y,     MGN_MAP_END };
static MinginButton moveLogAdvMapping[] =  { MGN_KEY_I,     MGN_MAP_END };
static MinginButton moveLogBackMapping[] =  { MGN_KEY_U,     MGN_MAP_END };
static MinginButton shiftMapping[] =  { MGN_KEY_SHIFT_L,     MGN_MAP_END };
static MinginButton ctrlMapping[] =  { MGN_KEY_CONTROL_L,     MGN_MAP_END };

static MinginButton randColorsMapping[] = { MGN_KEY_R,  MGN_MAP_END };
static MinginButton rotColors0Mapping[] = { MGN_KEY_1,  MGN_MAP_END };
static MinginButton rotColors1Mapping[] = { MGN_KEY_2,  MGN_MAP_END };
static MinginButton rotColors2Mapping[] = { MGN_KEY_3,  MGN_MAP_END };
static MinginButton rotColorsAllMapping[] = { MGN_KEY_4,  MGN_MAP_END };
static MinginButton printColorsMapping[] = { MGN_KEY_5,  MGN_MAP_END };


static MinginButton actionMapping[]  =  { MGN_BUTTON_MOUSE_LEFT,
                                          MGN_BUTTON_PS_X,
                                          MGN_BUTTON_XBOX_A,
                                          MGN_MAP_END };

static MinginButton drawMapping[]  =  { MGN_BUTTON_PS_TRIANGLE,
                                        MGN_BUTTON_XBOX_Y,
                                        MGN_MAP_END };

static MinginButton bombMapping[]  =  { MGN_KEY_B,
                                        MGN_MAP_END };

static MinginButton hintMapping[]   =  { MGN_BUTTON_MOUSE_LEFT,
                                         MGN_BUTTON_MOUSE_RIGHT,
                                         MGN_BUTTON_PS_X,
                                         MGN_BUTTON_PS_TRIANGLE,
                                         MGN_BUTTON_PS_CIRCLE,
                                         MGN_BUTTON_PS_SQUARE,
                                         MGN_BUTTON_XBOX_A,
                                         MGN_BUTTON_XBOX_B,
                                         MGN_BUTTON_XBOX_X,
                                         MGN_BUTTON_XBOX_Y,
                                         MGN_BUTTON_XBOX_GUIDE,
                                         MGN_BUTTON_XBOX_START,
                                         MGN_BUTTON_XBOX_BACK,
                                         MGN_BUTTON_PS_PS,
                                         MGN_BUTTON_PS_OPTIONS,
                                         MGN_BUTTON_PS_SHARE,
                                         MGN_BUTTON_R1,
                                         MGN_BUTTON_L1,
                                         MGN_BUTTON_R2,
                                         MGN_BUTTON_L2,
                                         MGN_BUTTON_DPAD_UP,
                                         MGN_BUTTON_DPAD_DOWN,
                                         MGN_BUTTON_DPAD_LEFT,
                                         MGN_BUTTON_DPAD_RIGHT,
                                         MGN_BUTTON_STICK_LEFT_PRESS,
                                         MGN_BUTTON_STICK_RIGHT_PRESS,
                                         MGN_KEY_ESCAPE,
                                         MGN_KEY_TAB,
                                         MGN_KEY_RETURN,
                                         MGN_KEY_BACKSPACE,
                                         MGN_KEY_SPACE,
                                         MGN_KEY_CONTROL_L,
                                         MGN_KEY_ALT_L,
                                         MGN_KEY_CONTROL_R,
                                         MGN_KEY_ALT_R,
                                         MGN_KEY_SHIFT_L,
                                         MGN_KEY_SHIFT_R,
                                         MGN_KEY_DELETE,
                                         MGN_KEY_SUPER_L,
                                         MGN_KEY_SUPER_R,
                                         MGN_KEY_CAPS_LOCK,
                                         MGN_KEY_PAGE_UP,
                                         MGN_KEY_PAGE_DOWN,
                                         MGN_KEY_HOME,
                                         MGN_KEY_END,
                                         MGN_KEY_NUM_LOCK,
                                         MGN_KEY_META_L,
                                         MGN_KEY_META_R,
                                         MGN_KEY_LEFT,
                                         MGN_KEY_RIGHT,
                                         MGN_KEY_UP,
                                         MGN_KEY_DOWN,
                                         MGN_KEY_F1,
                                         MGN_KEY_F2,
                                         MGN_KEY_F3,
                                         MGN_KEY_F4,
                                         MGN_KEY_F5,
                                         MGN_KEY_F6,
                                         MGN_KEY_F7,
                                         MGN_KEY_F8,
                                         MGN_KEY_F9,
                                         MGN_KEY_F10,
                                         MGN_KEY_F11,
                                         MGN_KEY_F12,
                                         MGN_ANY_KEY,
                                         MGN_MAP_END };

static MinginButton remapMapping[]  =  { MGN_KEY_P,         MGN_MAP_END };

static MinginButton crashMapping[]  =  { MGN_KEY_M,         MGN_MAP_END };


static MinginStick  thickMapping[]  =  { MGN_STICK_LEFT_Y,  MGN_MAP_END };


static const char  *sliderBarSprites[]  =  { "sliderFullBar100.tga",
                                             "sliderFullBar200.tga", };

static const char  *panelFullSprites[]  =  { "panelFull200_100.tga",
                                             "panelFull300_220.tga" };


#include "memoryRegister.h"


static void initEndMessageSprite( int  inIndex,
                                  const char  *inDataName ) {

    int  i  =  inIndex;
    
    endMessageSprites[i] = maxigin_initSprite( inDataName );

    maxigin_initMakeGlowSprite( endMessageSprites[i],
                                4,
                                2 );

    /* hazy drop shadow top to bottom */
    maxigin_initMakeDropShadowSprite( endMessageSprites[i],
                                      5,
                                      2,
                                      255,
                                      255,
                                      100,
                                      0,
                                      100,
                                      0 );
    }


void maxiginGame_init( void ) {

    int  i;
    int  fontStrip;
    
    for( i = 0;
         i < MAX_NUM_BULLETS;
         i ++ ) {

        bulletOn[ i ] = 0;

        bombOn[ i ] = 0;
        }

    for( i = 0;
         i < INFO_HIGHLIGHT_BUFFER_SIZE;
         i ++ ) {
        infoRow [i] = -1;
        infoCol [i] = -1;
        infoFade[i] = 0;
        }
        

    maxigin_randSeed( &rand,
                      1234859 );

    
    boardCenterX = MAXIGIN_GAME_NATIVE_W / 2 - 23;
    boardCenterY = MAXIGIN_GAME_NATIVE_H / 2;

    
    maxigin_initEnableCRTOverlay();
    
    spriteStrip = maxigin_initSpriteStrip( "stripTest.tga",
                                           16 );

    maxigin_initMakeGlowSpriteStrip( spriteStrip,
                                     2,
                                     2 );

    buttonHintStrip = maxigin_initSpriteStrip( "buttonHintStrip.tga",
                                               16 );

    maxigin_initMakeGlowSpriteStrip( buttonHintStrip,
                                     2,
                                     2 );
    
    maxigin_initKeyAndButtonHintSprites( buttonHintStrip,
                                         hintMapping );

    for( i = 0;
         i < NUM_BULK_FILES;
         i ++ ) {

        spriteHandles[ i ] = maxigin_initSprite( fileNames[ i ] );

        maxigin_initMakeGlowSprite( spriteHandles[ i ],
                                    4,
                                    2 );

        

        if( spriteHandles[ i]  == -1 ) {
            mingin_log( "Failed to load sprite: " );
            mingin_log( fileNames[ i ] );
            mingin_log( "\n" );
            }
        }


    initEndMessageSprite( 0,
                          "checkmate.tga" );
    initEndMessageSprite( 1,
                          "trapped.tga" );
    initEndMessageSprite( 2,
                          "overrun.tga" );
    


    spinFrameSprite = maxigin_initSprite( "spinButtonFrame.tga" );
    spinUnpressedSprite = maxigin_initSprite( "spinButtonUnpressed.tga" );
    spinPressedSprite = maxigin_initSprite( "spinButtonPressed.tga" );
    spinPressedTextSprite = maxigin_initSprite( "spinButtonPressedText.tga" );
    spinPressedTextGlowSprite =
        maxigin_initSprite( "spinButtonPressedTextGlow.tga" );
    
    maxigin_initMakeGlowSprite( spinFrameSprite,
                                4,
                                2 );

    maxigin_initMakeGlowSprite( spinPressedSprite,
                                8,
                                2 );

    maxigin_initMakeGlowSprite( spinPressedTextGlowSprite,
                                5,
                                3 );

    drawButtonSprite = maxigin_initSprite( "drawButton.tga" );
    drawButtonFrameSprite = maxigin_initSprite( "drawButtonFrame.tga" );
    drawButtonBackSprite = maxigin_initSprite( "drawButtonFrameBack.tga" );
    

    logoSprite = maxigin_initSprite( "logo.tga" );
    logoSubSprite = maxigin_initSprite( "logoSub.tga" );

    maxigin_initMakeGlowSprite( logoSprite,
                                4,
                                2 );

    maxigin_initMakeDropShadowSprite( logoSprite,
                                      4,
                                      2,
                                      255,
                                      255,
                                      100,
                                      0,
                                      200,
                                      0 );
    
    maxigin_initMakeGlowSprite( logoSubSprite,
                                2,
                                1 );

    maxigin_initMakeDropShadowSprite( logoSubSprite,
                                      4,
                                      2,
                                      255,
                                      255,
                                      100,
                                      0,
                                      300,
                                      0 );

    microdoseSprite  = maxigin_initSprite( "microdoseLogo.tga" );
    microdoseSpriteA = maxigin_initSprite( "microdoseLogoSolo.tga" );
    microdoseSpriteB = maxigin_initSprite( "microdoseLogoTiny.tga" ); 
    microdoseSpriteC = maxigin_initSprite( "microdoseLogoTinyCheap.tga" );

    maxigin_initMakeGlowSprite( microdoseSprite,
                                4,
                                2 );
    maxigin_initMakeGlowSprite( microdoseSpriteA,
                                4,
                                2 );
    maxigin_initMakeGlowSprite( microdoseSpriteB,
                                4,
                                2 );
    maxigin_initMakeGlowSprite( microdoseSpriteC,
                                4,
                                2 );
    

    jasonBylineSprite = maxigin_initSprite( "jasonByline.tga" );

    maxigin_initMakeGlowSprite( jasonBylineSprite,
                                4,
                                2 );

    
    tomBylineSprite = maxigin_initSprite( "tomByline.tga" );

    maxigin_initMakeGlowSprite( tomBylineSprite,
                                4,
                                2 );

    fontStrip = maxigin_initSpriteStrip( "5x9CapsLatinFont.tga",
                                         9 );

    if( fontStrip != -1 ) {

        maxigin_initMakeGlowSpriteStrip( fontStrip,
                                         2,
                                         2 );

        smallCapsFont = maxigin_initFont( fontStrip,
                                          "latinFont.txt",
                                          2,
                                          6,
                                          0,
                                          6 );
        }
    
    lang_settings      = maxigin_initTranslationKey( "settings" );
    lang_newGame       = maxigin_initTranslationKey( "newGame"  );
    lang_quit          = maxigin_initTranslationKey( "quit" );
    lang_musicVolume   = maxigin_initTranslationKey( "musicVolume" );
    lang_effectsVolume = maxigin_initTranslationKey( "effectsVolume" );
    lang_fullscreen    = maxigin_initTranslationKey( "fullscreen" );

    lang_action        = maxigin_initTranslationKey( "actionDesc" );
    lang_draw          = maxigin_initTranslationKey( "drawDesc" );
    lang_bomb          = maxigin_initTranslationKey( "bombDesc" );
    lang_drawInstruct  = maxigin_initTranslationKey( "drawInstruct" );
    
    
    maxigin_registerButtonMapping( JUMP,   jumpMapping );

    maxigin_registerButtonMapping( TOGGLE_MOVE_LOG,
                                   moveLogMapping );
    maxigin_registerButtonMapping( ADVANCE_MOVE_LOG,
                                   moveLogAdvMapping );
    maxigin_registerButtonMapping( BACK_MOVE_LOG,
                                   moveLogBackMapping );
    maxigin_registerButtonMapping( SHIFT,
                                   shiftMapping );
    maxigin_registerButtonMapping( CTRL,
                                   ctrlMapping );

    maxigin_registerButtonMapping( RAND_COLORS,
                                   randColorsMapping );
    maxigin_registerButtonMapping( ROT_COLORS_0,
                                   rotColors0Mapping );
    maxigin_registerButtonMapping( ROT_COLORS_1,
                                   rotColors1Mapping );
    maxigin_registerButtonMapping( ROT_COLORS_2,
                                   rotColors2Mapping );
    maxigin_registerButtonMapping( ROT_COLORS_ALL,
                                   rotColorsAllMapping );
    maxigin_registerButtonMapping( PRINT_COLORS,
                                   printColorsMapping );
    
    maxigin_registerDynamicButtonMapping(
        ACTION,
        actionMapping,
        lang_action  );

    maxigin_registerDynamicButtonMapping(
        DRAW,
        drawMapping,
        lang_draw  );

    maxigin_registerDynamicButtonMapping(
        BOMB,
        bombMapping,
        lang_bomb );
    
    maxigin_registerButtonMapping( REMAP,  remapMapping );
    maxigin_registerButtonMapping( CRASH,  crashMapping );

    maxigin_logInt( "Primary button for ACTION is: ",
                    maxigin_getPlatformPrimaryButton( ACTION ) );

    mingin_registerStickAxis( BOX_THICK,  thickMapping );


    if( 0 ) {
        
        maxigin_initSliderSprites( "sliderLeftEndEmpty.tga",
                                   "sliderLeftEndFull.tga",
                                   "sliderRightEndEmpty.tga",
                                   "sliderRightEndFull.tga",
                                   "sliderBarEmpty.tga",
                                   "sliderBarFull.tga",
                                   "sliderSliverEmpty.tga",
                                   "sliderSliverFull.tga",
                                   "sliderThumbPassive.tga",
                                   "sliderThumbHot.tga",
                                   "sliderThumbActive.tga",
                                   "loopStart.tga",
                                   "loopEnd.tga" );
        }
    else {
        maxigin_initSliderSpritesStatic( 2,
                                         sliderBarSprites,
                                         "sliderThumbPassive.tga",
                                         "sliderThumbHot.tga",
                                         "sliderThumbActive.tga",
                                         "loopStart.tga",
                                         "loopEnd.tga" );
        }
    

    if( 0 ) {
        maxigin_initPanelSprites( "panelTopLeft.tga",
                                  "panelTopRight.tga",
                                  "panelBottomLeft.tga",
                                  "panelBottomRight.tga",
                                  "panelLeftEdge.tga",
                                  "panelRightEdge.tga",
                                  "panelTopEdge.tga",
                                  "panelBottomEdge.tga",
                                  "panelFill.tga" );
        }
    else {
        maxigin_initPanelSpritesStatic( 2,
                                        panelFullSprites );
        }

    maxigin_initCheckboxSprites( "checkboxCoolUnchecked.tga",
                                 "checkboxCoolChecked.tga",
                                 "checkboxHotUnchecked.tga",
                                 "checkboxHotChecked.tga" );

    maxigin_initButtonSprites( "buttonCool.tga",
                               "buttonHot.tga",
                               "buttonPressed.tga" );
    
    maxigin_initMakeGuiGlow( 2,
                             2 );

    maxigin_initGUI( &gameGUI );

    
    maxigin_initMusicLoop( "musicLoop_stereo_16_44100.wav" );

    maxigin_initSoundEffect( "hey1.wav" );
    maxigin_initSoundEffect( "hey2.wav" );
    plunkSound = maxigin_initSoundEffect( "plunk1.wav" );
    thunkSound = maxigin_initSoundEffect( "thunk1.wav" );

    examinePieceSound = maxigin_initSoundEffect( "examinePiece_misc_10.wav" );

    pickFailedSound = maxigin_initSoundEffect( "pickFailed_sd_3.wav" );

    checkmateGood = maxigin_initSoundEffect( "checkmateGood.wav" );
    checkmateBad = maxigin_initSoundEffect( "checkmateBad.wav" );

    maxigin_initSoundEffect( "test_long.wav" );

    maxigin_initSetMenuSounds( plunkSound,
                               256,
                               thunkSound,
                               512,
                               plunkSound );

    

    maxigin_initSetLanguageFontGLow( 2,
                                     2 );
    chessInit();
    boardInit();
    pieceSpritesInit();
    particleSpriteInit();
    moveAnimInit();
    moneyInit( 10,
               plunkSound );
    numbersInit();
    checkDisplayInit();
    colorsInit();

    pinchInit();
    particleSystemInit();
    pieceDescriptionsInit();

    navInit();
    
    levelsInit();

    deckInit();


    clearDrawMarkers();

    clearDrawLift( &redrawLift );
    clearDrawLift( &redrawSmoothLift );

    if(0)runChessTest();
    
    
    /* init position in image center */
    boxPosX = MAXIGIN_GAME_NATIVE_W / 2;
    boxPosY = MAXIGIN_GAME_NATIVE_H / 2;

    boxH = ( MAXIGIN_GAME_NATIVE_H * 3 ) / 12;

    if(1) getStartBoard( &boardState );
    if(0) getTestBoard( &boardState );
    
    getPlayerStartDeck( &playerDeck );
    
    if(0) {
        
        getLevel( 0,
                  &boardState,
                  &playerDeck );
        }
    
    

    REGISTER_VAL_MEM( boxPosX );
    REGISTER_VAL_MEM( boxPosY );
    REGISTER_VAL_MEM( boxW );
    REGISTER_VAL_MEM( boxH );
    REGISTER_VAL_MEM( boxVPerSecond );
    REGISTER_VAL_MEM( boxDir );

    REGISTER_VAL_MEM( sliderValue );
    REGISTER_VAL_MEM( sliderValueB );
    REGISTER_VAL_MEM( sliderValueC );

    REGISTER_VAL_MEM( gameGUI );

    REGISTER_VAL_MEM( boardState );
    REGISTER_VAL_MEM( postMoveCaptured );
    REGISTER_VAL_MEM( postMoveState );
    REGISTER_VAL_MEM( boardMove );
    
    REGISTER_VAL_MEM( moveMade );
    REGISTER_VAL_MEM( moveProgress );

    REGISTER_VAL_MEM( checkmate );
    REGISTER_VAL_MEM( stalemate );
    REGISTER_VAL_MEM( drawGame );
    REGISTER_VAL_MEM( gameLoserColor );

    REGISTER_VAL_MEM( chessGameOver );
    REGISTER_VAL_MEM( endMessageColor );
    REGISTER_VAL_MEM( endMessageIndex );
    REGISTER_VAL_MEM( explodingEndMessageProgress );
    REGISTER_VAL_MEM( endMessageFade );
    REGISTER_VAL_MEM( endMessagePreFadeSteps );

    REGISTER_VAL_MEM( spinning );

    REGISTER_VAL_MEM( statesTested );
    
    
    REGISTER_ARRAY_MEM( bulletOn );
    REGISTER_ARRAY_MEM( bulletPos );
    REGISTER_ARRAY_MEM( bulletSpeed );
    REGISTER_ARRAY_MEM( bulletFade );

    REGISTER_ARRAY_MEM( bombOn );
    REGISTER_ARRAY_MEM( bombPos );
    REGISTER_ARRAY_MEM( bombSpeed );
    REGISTER_ARRAY_MEM( bombStripIndex );
    REGISTER_ARRAY_MEM( bombProgress );
    REGISTER_ARRAY_MEM( bombBurstProgress );
    REGISTER_ARRAY_MEM( bombBurstFade ); 
    
    REGISTER_VAL_MEM( stepsSinceLastBullet );

    REGISTER_VAL_MEM( lineTip );

    REGISTER_VAL_MEM( stripIndex );

    REGISTER_VAL_MEM( stepSec );
    REGISTER_VAL_MEM( stepMSec );

    REGISTER_VAL_MEM( infoPanelPiece );
    REGISTER_VAL_MEM( infoPanelLastPiece );
    REGISTER_VAL_MEM( infoPanelFade );

    REGISTER_ARRAY_MEM( infoRow );
    REGISTER_ARRAY_MEM( infoCol );
    REGISTER_ARRAY_MEM( infoFade );
    
    REGISTER_VAL_MEM( curInfoIndex );

    REGISTER_VAL_MEM( drawButtonPressed );

    REGISTER_VAL_MEM( boardMarkersDownCount );
    REGISTER_ARRAY_MEM( boardMarkers );

    REGISTER_VAL_MEM( playerDeck );

    REGISTER_VAL_MEM( drawPrice );

    /* need to register both, in case we stop playback during a lift */
    REGISTER_VAL_MEM( redrawLift );
    REGISTER_VAL_MEM( redrawSmoothLift );

    REGISTER_VAL_MEM( redrawRemoveRunning );
    REGISTER_VAL_MEM( redrawAddRunning );
    REGISTER_VAL_MEM( boardMarkersHidden );

    maxigin_initRestoreStaticMemoryFromLastRun();
    }
