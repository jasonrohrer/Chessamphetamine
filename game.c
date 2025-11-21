
#define MINGIN_IMPLEMENTATION

#include "mingin.h"



enum UserAction {
    QUIT,
    JUMP,
    FULLSCREEN_TOGGLE,
    REMAP
    };



static int boxPosX = -1;
static int boxPosY = -1;
static int boxW = 10;
static int boxH = 200;
static int boxVPerSecond = 120;
static int boxDir = 1;

static char initDone = 0;

static char fullscreenTogglePressed = 0;

#define GAME_NATIVE_W 320
#define GAME_NATIVE_H 240

/* RGB pixels of game's native image size */
static unsigned char gameImageBuffer[ GAME_NATIVE_W * GAME_NATIVE_H * 3 ];



void minginGame_getMinimumViableScreenSize( int *outWide, int *outHigh ) {
    *outWide = GAME_NATIVE_W;
    *outHigh = GAME_NATIVE_H;
    }


static void redrawGameImage( void );

static void redrawGameImage( void ) {
    int boxStartX = boxPosX - boxW / 2;
    int boxStartY = boxPosY - boxH / 2;
    int numPixels = GAME_NATIVE_W * GAME_NATIVE_H;
    int p, x, y;
    /* white background */
    for( p = 0; p<numPixels; p++ ) {
        int pix = p * 3;


        gameImageBuffer[pix] = 255;
        gameImageBuffer[pix + 1] = 255;
        gameImageBuffer[pix + 2] = 255;
        }

    /* now draw black box */
    for( y = boxStartY; y < boxStartY + boxH; y++ ) {
        int rowStart = y * GAME_NATIVE_W * 3;
        
        for( x = boxStartX; x < boxStartX + boxW; x ++ ) {
            int pix = rowStart + x * 3;
            gameImageBuffer[pix] = (unsigned char)( 255 * (y-boxStartY) / boxH );
            gameImageBuffer[pix + 1] = 0;
            gameImageBuffer[pix + 2] = 0;
            }
        }
    /* put yellow square in top left corner as sanity check */
    
    for( y = 0; y < 10; y++ ) {
        int rowStart = y * GAME_NATIVE_W * 3;
        
        for( x = 0; x < 10; x ++ ) {
            int pix = rowStart + x * 3;
            gameImageBuffer[pix] = 255;
            gameImageBuffer[pix + 1] = 255;
            gameImageBuffer[pix + 2] = 0;
            }
        }
    
    }


void minginGame_getScreenPixels( int inWide, int inHigh,
                                 unsigned char *inRGBBuffer ) {
    int numPixels = inWide * inHigh;
    int numPixelBytes = numPixels * 3;
    int p;
    int x, y;
    
    int scaleFactor;
    int scaleW, scaleH;
    int scaledGameW, scaledGameH;
    int offsetX, offsetY;
    
    redrawGameImage();

    scaleW = inWide /  GAME_NATIVE_W;

    scaleH = inHigh / GAME_NATIVE_H;

    scaleFactor = scaleW;

    if( scaleH < scaleFactor ) {
        scaleFactor = scaleH;
        }

    if( scaleFactor < 1 ) {
        /* screen isn't big enough for our native size?
           let it just get cut off for now */
        scaleFactor = 1;
        }
    
    scaledGameW = scaleFactor * GAME_NATIVE_W;
    scaledGameH = scaleFactor * GAME_NATIVE_H;


    /* pick offsets to center in screen, if possible */
    offsetX = 0;
    offsetY = 0;

    if( scaledGameW < inWide ) {
        offsetX = ( inWide - scaledGameW ) / 2;
        }
    if( scaledGameH < inHigh ) {
        offsetY = ( inHigh - scaledGameH ) / 2;
        }
    

    if( offsetX > 0 || offsetY > 0 ) {
        /* black background beyond edges of our centered image */
        for( p = 0; p<numPixelBytes; p++ ) {
            inRGBBuffer[p] = 0;
            }
        }

    /* naive nearest neighbor scaling */
    for( y = offsetY; y < offsetY + scaledGameH; y++ ) {
        int rowStartDest = y * inWide * 3;
        int ySrcScaled = y - offsetY;
        int ySrcOrig = ySrcScaled /  scaleFactor;
        
        int rowStartSrcOrig = ySrcOrig * GAME_NATIVE_W * 3;
        
        for( x = offsetX; x < offsetX + scaledGameW; x++ ) {
            int xSrcScaled = x - offsetX;
            int xSrcOrig = xSrcScaled /  scaleFactor;
        
            int pixDest = rowStartDest + x * 3;
            int pixSrcOrig = rowStartSrcOrig + xSrcOrig * 3;

            inRGBBuffer[ pixDest ] = gameImageBuffer[ pixSrcOrig ];
            inRGBBuffer[ pixDest + 1 ] = gameImageBuffer[ pixSrcOrig + 1 ];
            inRGBBuffer[ pixDest + 2 ] = gameImageBuffer[ pixSrcOrig + 2 ];
            }
        }
    }


static void gameInit( void );


static char remappingJump = 0;


void minginGame_step( char inFinalStep ) {
    int r;
    int boxVPerStep;


    if( inFinalStep ) {
        /* no clean-up needed so far */
        return;
        }
    
    if( ! initDone ) {
        gameInit();
        initDone = 1;
        }

    if( remappingJump ) {
        MinginButton last = mingin_getLastButtonPressed();

        if( last != MGN_BUTTON_NONE ) {
            MinginButton newJumpMapping[2] = { MGN_MAP_END, MGN_MAP_END };
            newJumpMapping[0] = last;

            /* overwrite mapping for jump */
            mingin_registerButtonMapping( JUMP, newJumpMapping );
            remappingJump = 0;
            }
        }
    
    if( mingin_isButtonDown( QUIT ) ) {
        mingin_log( "Got quit key\n" );
        
        mingin_quit();
        return;
        }

    if( mingin_isButtonDown( JUMP ) ) {
        boxPosY -= 1;
        }
    
    if( mingin_isButtonDown( FULLSCREEN_TOGGLE ) ) {

        if( ! fullscreenTogglePressed ) {
            fullscreenTogglePressed = 1;

            mingin_toggleFullscreen( ! mingin_isFullscreen() );
            }
        }
    else {
        fullscreenTogglePressed = 0;
        }

    if( ! remappingJump && mingin_isButtonDown( REMAP ) ) {
        /* clear last button memory */
        mingin_getLastButtonPressed();
        remappingJump = 1;
        }
    
    
    r = mingin_getStepsPerSecond();

    /* move box */
    boxVPerStep = boxVPerSecond / r;
    
    if( boxVPerStep < 1 ) {
        boxVPerStep = 1;
        }
    boxPosX += boxDir * boxVPerStep;

    if( boxDir > 0 && boxPosX > GAME_NATIVE_W ) {
        boxDir = -1;
        }
    else if( boxDir < 0 && boxPosX < 0 ) {
        boxDir = 1;
        }
    }



static MinginButton quitMapping[] = { MGN_KEY_Q, MGN_KEY_ESCAPE, MGN_MAP_END };
static MinginButton jumpMapping[] = { MGN_KEY_SPACE, MGN_MAP_END };
static MinginButton fullscreenMapping[] = { MGN_KEY_F, MGN_MAP_END };

static MinginButton remapMapping[] = { MGN_KEY_P, MGN_MAP_END };

static void gameInit( void ) {
    
    mingin_registerButtonMapping( QUIT, quitMapping );
    mingin_registerButtonMapping( JUMP, jumpMapping );
    mingin_registerButtonMapping( FULLSCREEN_TOGGLE, fullscreenMapping );
    mingin_registerButtonMapping( REMAP, remapMapping );
    
    /* init position in image center */
    boxPosX = GAME_NATIVE_W / 2;
    boxPosY = GAME_NATIVE_H / 2;

    boxH = ( GAME_NATIVE_H * 3 ) / 4;
    }
