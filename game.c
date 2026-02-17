
#define MINGIN_IMPLEMENTATION

#define MAXIGIN_GAME_NATIVE_W  320
#define MAXIGIN_GAME_NATIVE_H  240

#define MAXIGIN_IMPLEMENTATION

#include "maxigin.h"



enum GameUserAction {
    JUMP,
    SHOOT,
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

    

#define  NUM_BULK_FILES  6

static const char  *fileNames[ NUM_BULK_FILES ] = { "bullet.tga",
                                                    "bulletGlow.tga",
                                                    "bigPointer.tga",
                                                    "bigPointer2.tga",
                                                    "vertLine.tga",
                                                    "horLine.tga" };

static int          spriteHandles[ NUM_BULK_FILES ];

static int          spriteStrip;
static int          buttonHintStrip;

static int          stripIndex    =   0;
static int          stripC        =   0;

static MaxiginGUI   gameGUI;
static int          sliderValue   =   7;
static int          sliderValueB  =   7;

static int          plunkSound    =  -1;



void maxiginGame_getNativePixels( unsigned char *inRGBBuffer ) {
    
    int  boxStartX  =  boxPosX - boxW / 2;
    int  boxStartY  =  boxPosY - boxH / 2;
    int  numPixels  =  MAXIGIN_GAME_NATIVE_W * MAXIGIN_GAME_NATIVE_H;
    int  p;
    int  x;
    int  y;
    int  i;

    maxigin_drawSetAlpha( 255 );
    
    /* black background */
    for( p = 0; p<numPixels; p++ ) {
        int pix = p * 3;

        inRGBBuffer[pix] = 0;
        inRGBBuffer[pix + 1] = 0;
        inRGBBuffer[pix + 2] = 0;
        }

    /* now draw black box */
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
    if( 1 ) {
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
    
    
    /* put yellow square in top left corner as sanity check */
    
    for( y = 0; y < 10; y++ ) {
        int rowStart = y * MAXIGIN_GAME_NATIVE_W * 3;
        
        for( x = 0; x < 10; x ++ ) {
            int pix = rowStart + x * 3;
            inRGBBuffer[pix] = 255;
            inRGBBuffer[pix + 1] = 255;
            inRGBBuffer[pix + 2] = 0;
            }
        }

    /* hint test */

    maxigin_drawButtonHintSprite( SHOOT,
                                  20,
                                  20 );


    maxigin_drawSetColor( 255, 255, 0, 255 );
    maxigin_drawLangText( 0,
                          20,
                          120,
                          MAXIGIN_LEFT );

    maxigin_drawSetColor( 0, 255, 255, 255 );
    maxigin_drawLangText( 1,
                          20,
                          140,
                          MAXIGIN_LEFT );
        
    maxigin_drawResetColor();
    maxigin_drawLangText( 2,
                          20,
                          160,
                          MAXIGIN_LEFT);

    maxigin_drawLangText( 3,
                          20,
                          180,
                          MAXIGIN_LEFT );
        
    maxigin_drawLangText( 4,
                          20,
                          200,
                          MAXIGIN_LEFT );
                
    maxigin_drawLangText( 5,
                          20,
                          220,
                          MAXIGIN_LEFT );
                       


    return;
    
    
    for( i = 0;
         i < 9;
         i ++ ) {

        /* vert line */
        maxigin_drawSprite( spriteHandles[4],
                            i * 20 + 19,
                            MAXIGIN_GAME_NATIVE_H /  2);
        /* hor line */
        maxigin_drawSprite( spriteHandles[5],
                            MAXIGIN_GAME_NATIVE_W /  2,
                            i * 20 + 44 );
        }

    maxigin_drawToggleAdditive( 1 );
    
    for( i = 0;
         i < MAX_NUM_BULLETS;
         i ++ ) {
        
        if( bulletOn[ i ] ) {
            
            int  startX  =  bulletPos[ i ].x - 2;
            int  startY  =  bulletPos[ i ].y - 2;
            int  endX    =  bulletPos[ i ].x + 2;
            int  endY    =  bulletPos[ i ].y + 2;

            maxigin_drawSetAlpha( bulletFade[ i ] );
            
            if( i % 4 == 0 ) {
                
                maxigin_drawSprite( spriteHandles[ 0 ],
                                    bulletPos[ i ].x,
                                    bulletPos[ i ].y );
                /* double glow */
                maxigin_drawSprite( spriteHandles[ 1 ],
                                    bulletPos[ i ].x,
                                    bulletPos[ i ].y );
                maxigin_drawSprite( spriteHandles[ 1 ],
                                    bulletPos[ i ].x,
                                    bulletPos[ i ].y );
                }
            else if( i % 4 == 1 ) {
                maxigin_drawSprite( spriteHandles[2],
                                    bulletPos[ i ].x,
                                    bulletPos[ i ].y );
                }
            else if( i % 4 == 2 ) {
                maxigin_drawSprite( spriteHandles[3],
                                    bulletPos[ i ].x,
                                    bulletPos[ i ].y );
                }
            else if( 1 || i % 4 == 3) {
                maxigin_drawSprite( maxigin_getSpriteFromStrip( spriteStrip,
                                                                stripIndex ),
                                    bulletPos[ i ].x,
                                    bulletPos[ i ].y );
                }
            
            if( 0 )
            for( y = startY;
                 y < endY;
                 y ++ ) {

                if( y >= 0
                    &&
                    y < MAXIGIN_GAME_NATIVE_H ) {
                    
                    int rowStart = y * MAXIGIN_GAME_NATIVE_W * 3;
        
                    for( x = startX;
                         x < endX;
                         x ++ ) {

                        if( x >= 0
                            &&
                            x < MAXIGIN_GAME_NATIVE_W ) {
                    
                            int pix = rowStart + x * 3;
                            inRGBBuffer[pix] = 0;
                            inRGBBuffer[pix + 1] = 0;
                            inRGBBuffer[pix + 2] = 255;
                            }
                        }
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

    
    maxigin_drawFillRect( MAXIGIN_GAME_NATIVE_W / 2,
                          MAXIGIN_GAME_NATIVE_H / 2,
                          lineTip.x,
                          lineTip.y );

    maxigin_drawToggleAdditive( 0 );
    
    maxigin_drawSetColor( 255, 0, 0, 255 );

    
    maxigin_drawRect( MAXIGIN_GAME_NATIVE_W / 2,
                      MAXIGIN_GAME_NATIVE_H / 2,
                      lineTip.x,
                      lineTip.y );

    maxigin_drawResetColor();

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



static char remappingJump = 0;


void maxiginGame_step( void ) {
    
    int   r;
    int   i;
    int   boxVPerStep;
    
    char  pointerLive;
    int   pointerX;
    int   pointerY;
    
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

            maxigin_playSoundEffect( plunkSound );
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


    maxigin_startGUI( &gameGUI );

    
    maxigin_guiSlider( &gameGUI,
                       &sliderValue,
                       0,
                       10,
                       30,
                       MAXIGIN_GAME_NATIVE_W - 30,
                       30,
                       10,
                       20,
                       10,
                       0 );


    maxigin_guiSlider( &gameGUI,
                       &sliderValueB,
                       0,
                       200,
                       30,
                       MAXIGIN_GAME_NATIVE_W - 30,
                       50,
                       10,
                       20,
                       10,
                       0 );

    maxigin_endGUI( &gameGUI );
    
    }



static MinginButton jumpMapping[]   =  { MGN_KEY_SPACE,     MGN_MAP_END };

static MinginButton shootMapping[]  =  { MGN_BUTTON_MOUSE_LEFT,
                                         MGN_BUTTON_PS_X,
                                         MGN_BUTTON_XBOX_A,
                                         MGN_MAP_END };

static MinginButton remapMapping[]  =  { MGN_KEY_P,         MGN_MAP_END };

static MinginButton crashMapping[]  =  { MGN_KEY_M,         MGN_MAP_END };


static MinginStick  thickMapping[]  =  { MGN_STICK_LEFT_Y,  MGN_MAP_END };


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
        bulletPos[ i ].x = 0;
        bulletPos[ i ].y = 0;
        bulletSpeed[ i ].x = 0;
        bulletSpeed[ i ].y = 0;
        }

    
    spriteStrip = maxigin_initSpriteStrip( "stripTest.tga",
                                           16 );

    maxigin_initMakeGlowSpriteStrip( spriteStrip,
                                     2,
                                     2 );

    buttonHintStrip = maxigin_initSpriteStrip( "buttonHintStrip.tga",
                                               16 );

    maxigin_initKeyAndButtonHintSprites( buttonHintStrip,
                                         shootMapping );

    for( i = 0;
         i < NUM_BULK_FILES;
         i ++ ) {

        if( i == 3 || i == 4 || i == 5 ) {
            spriteHandles[ i ] = maxigin_initSprite( fileNames[ i ] );

            maxigin_initMakeGlowSprite( spriteHandles[ i ],
                                        4,
                                        2 );
            }
        else {
            spriteHandles[ i ] = maxigin_initSprite( fileNames[ i ] );
            }
        

        if( spriteHandles[ i]  == -1 ) {
            mingin_log( "Failed to load sprite: " );
            mingin_log( fileNames[ i ] );
            mingin_log( "\n" );
            }
        }

    
    maxigin_registerButtonMapping( JUMP,   jumpMapping );
    maxigin_registerButtonMapping( SHOOT,  shootMapping );
    maxigin_registerButtonMapping( REMAP,  remapMapping );
    maxigin_registerButtonMapping( CRASH,  crashMapping );

    maxigin_logInt( "Primary button for SHOOT is: ",
                    maxigin_getPlatformPrimaryButton( SHOOT ) );

    mingin_registerStickAxis( BOX_THICK,  thickMapping );


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
    

    maxigin_initGUI( &gameGUI );

    
    maxigin_initMusicLoop( "musicLoop_stereo_16_44100.wav" );

    maxigin_initSoundEffect( "hey1.wav" );
    maxigin_initSoundEffect( "hey2.wav" );
    plunkSound = maxigin_initSoundEffect( "plunk1.wav" );

    maxigin_initSoundEffect( "test_long.wav" );


    maxigin_initTranslationKey( 0,
                                "settings" );
    maxigin_initTranslationKey( 1,
                                "newGame" );
    maxigin_initTranslationKey( 2,
                                "quit" );
    maxigin_initTranslationKey( 3,
                                "musicVolume" );
    maxigin_initTranslationKey( 4,
                                "effectsVolume" );
    maxigin_initTranslationKey( 5,
                                "fullscreen" );

    
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

    REGISTER_VAL_MEM( gameGUI );
    

    REGISTER_ARRAY_MEM( bulletOn );
    REGISTER_ARRAY_MEM( bulletPos );
    REGISTER_ARRAY_MEM( bulletSpeed );
    REGISTER_ARRAY_MEM( bulletFade );
    
    REGISTER_VAL_MEM( stepsSinceLastBullet );

    REGISTER_VAL_MEM( lineTip );

    REGISTER_VAL_MEM( stripIndex );

    maxigin_initRestoreStaticMemoryFromLastRun();
    }
