
/* NOTE:
   These MUST match defines in maxigin_imp.c */
#define MAXIGIN_GAME_NATIVE_W  320
#define MAXIGIN_GAME_NATIVE_H  240


#include "maxigin.h"


#define BOARD_IMPLEMENTATION
#include "board.h"


#define CHESS_IMPLEMENTATION
#include "chess.h"

#define PIECE_SPRITES_IMPLEMENTATION
#include "pieceSprites.h"



enum GameUserAction {
    JUMP,
    SHOOT,
    BOMB,
    REMAP,
    CRASH,
    BOX_THICK
    };


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

static int          stripIndex    =   0;
static int          stripC        =   0;

static MaxiginGUI   gameGUI;
static int          sliderValue   =   7;
static int          sliderValueB  =   7;
static int          sliderValueC  =   7;

static int          plunkSound    =  -1;
static int          thunkSound    =  -1;


static int          lang_settings;
static int          lang_newGame;
static int          lang_quit;
static int          lang_musicVolume;
static int          lang_effectsVolume;
static int          lang_fullscreen;

static int          lang_shoot;

static int          lang_bomb;




void maxiginGame_getNativePixels( unsigned char *inRGBBuffer ) {
    
    int  boxStartX  =  boxPosX - boxW / 2;
    int  boxStartY  =  boxPosY - boxH / 2;
    int  numPixels  =  MAXIGIN_GAME_NATIVE_W * MAXIGIN_GAME_NATIVE_H;
    int  p;
    int  x;
    int  y;
    int  i;

    BoardState  state;
    

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

    boardDraw( MAXIGIN_GAME_NATIVE_W / 2,
               MAXIGIN_GAME_NATIVE_H / 2 );


    getStartBoard( &state );

    drawBoardState( &state,
                    MAXIGIN_GAME_NATIVE_W / 2,
                    MAXIGIN_GAME_NATIVE_H / 2 );
    

    maxigin_drawGUI( &gameGUI );
    }



static void fireBullet( int  inX,
                        int  inY ) {

    int  i;

    for( i = 0;
         i < MAX_NUM_BULLETS;
         i ++ ) {

        if( ! bulletOn[ i ] ) {
            /* found empty bullet slot */

            bulletOn[ i ] = 1;

            bulletPos[ i ].x = inX;
            bulletPos[ i ].y = inY;

            bulletSpeed[ i ].x = 0;
            bulletSpeed[ i ].y = -2;
            
            bulletFade[ i ] = 255;
            
            break;
            }
        }
    
    
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



static char remappingJump = 0;

static int  loudnessToggle = 512;


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
    
    
    if( maxigin_isButtonDown( JUMP ) ) {
        boxPosY -= 1;
        }

    if( ! remappingJump && maxigin_isButtonDown( REMAP ) ) {
        /* clear last button memory */
        mingin_getLastButtonPressed();
        remappingJump = 1;
        }

    if( maxigin_isButtonDown( SHOOT ) ) {
        int  msSinceLastBullet  =  (stepsSinceLastBullet * 1000 ) / r;

        if( msSinceLastBullet > msBetweenBullets ) {
            stepsSinceLastBullet = 0;

            fireBullet( boxPosX, boxPosY );

            maxigin_playSoundEffect( plunkSound,
                                     loudnessToggle );

            if( loudnessToggle == 512 ) {
                loudnessToggle = 128;
                }
            else {
                loudnessToggle = 512;
                }
            }
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



static MinginButton jumpMapping[]   =  { MGN_KEY_SPACE,     MGN_MAP_END };

static MinginButton shootMapping[]  =  { MGN_KEY_V,
                                         MGN_BUTTON_MOUSE_LEFT,
                                         MGN_BUTTON_PS_X,
                                         MGN_BUTTON_XBOX_A,
                                         MGN_MAP_END };
static MinginButton bombMapping[]  =  { MGN_KEY_B,
                                        MGN_BUTTON_MOUSE_RIGHT,
                                        MGN_BUTTON_PS_TRIANGLE,
                                        MGN_BUTTON_XBOX_Y,
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


#define REGISTER_VAL_MEM( x )  \
    maxigin_initRegisterStaticMemory( &x, sizeof(x), #x )

#define REGISTER_ARRAY_MEM( a ) \
    maxigin_initRegisterStaticMemory( a, sizeof(a), #a )


void maxiginGame_init( void ) {

    int  i;
    
    for( i = 0;
         i < MAX_NUM_BULLETS;
         i ++ ) {

        bulletOn[ i ] = 0;

        bombOn[ i ] = 0;
        }

    maxigin_randSeed( &rand,
                      1234859 );

    
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

    lang_settings      = maxigin_initTranslationKey( "settings" );
    lang_newGame       = maxigin_initTranslationKey( "newGame"  );
    lang_quit          = maxigin_initTranslationKey( "quit" );
    lang_musicVolume   = maxigin_initTranslationKey( "musicVolume" );
    lang_effectsVolume = maxigin_initTranslationKey( "effectsVolume" );
    lang_fullscreen    = maxigin_initTranslationKey( "fullscreen" );

    lang_shoot         = maxigin_initTranslationKey( "shootDesc" );
    lang_bomb          = maxigin_initTranslationKey( "bombDesc" );
    
    
    maxigin_registerButtonMapping( JUMP,   jumpMapping );
    
    maxigin_registerDynamicButtonMapping(
        SHOOT,
        shootMapping,
        lang_shoot  );

    maxigin_registerDynamicButtonMapping(
        BOMB,
        bombMapping,
        lang_bomb );
    
    maxigin_registerButtonMapping( REMAP,  remapMapping );
    maxigin_registerButtonMapping( CRASH,  crashMapping );

    maxigin_logInt( "Primary button for SHOOT is: ",
                    maxigin_getPlatformPrimaryButton( SHOOT ) );

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
                                   "sliderThumbActive.tga" );
        }
    else {
        maxigin_initSliderSpritesStatic( 2,
                                         sliderBarSprites,
                                         "sliderThumbPassive.tga",
                                         "sliderThumbHot.tga",
                                         "sliderThumbActive.tga" );
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

    maxigin_initSoundEffect( "test_long.wav" );

    maxigin_initSetMenuSounds( plunkSound,
                               256,
                               thunkSound,
                               512,
                               plunkSound );

    

    maxigin_initSetLanguageFontGLow( 2,
                                     2 );

    boardInit();
    pieceSpritesInit();
    
    /* init position in image center */
    boxPosX = MAXIGIN_GAME_NATIVE_W / 2;
    boxPosY = MAXIGIN_GAME_NATIVE_H / 2;

    boxH = ( MAXIGIN_GAME_NATIVE_H * 3 ) / 12;


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

    maxigin_initRestoreStaticMemoryFromLastRun();
    }
