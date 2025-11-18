
#define MINGIN_IMPLEMENTATION

#include "mingin.h"



enum UserAction {
    QUIT,
    JUMP,
    FULLSCREEN_TOGGLE
    };



static int boxPosX = -1;
static int boxPosY = -1;
static int boxW = 10;
static int boxH = 600;
static int boxVPerSecond = 120;
static int boxDir = 1;

static char initDone = 0;

static char fullscreenTogglePressed = 0;


void minginGame_getScreenPixels( int inWide, int inHigh,
                                 unsigned char *inRGBBuffer ) {
    int numPixels = inWide * inHigh;
    int p;
    int x, y;
    
    int boxStartX = boxPosX - boxW / 2;
    int boxStartY = boxPosY - boxH / 2;
    
    /* white background */
    for( p = 0; p<numPixels; p++ ) {
        int pix = p * 3;


        inRGBBuffer[pix] = 255;
        inRGBBuffer[pix + 1] = 255;
        inRGBBuffer[pix + 2] = 255;
        }

    /* now draw black box */
    for( y = boxStartY; y < boxStartY + boxH; y++ ) {
        int rowStart = y * inWide * 3;
        
        for( x = boxStartX; x < boxStartX + boxW; x ++ ) {
            int pix = rowStart + x * 3;
            inRGBBuffer[pix] = 255;
            inRGBBuffer[pix + 1] = 0;
            inRGBBuffer[pix + 2] = 0;
            }
        }
    
    }


static void gameInit( void );


void minginGame_step( void ) {
    int w, h;
    int r;
    int boxVPerStep;
    
    if( ! initDone ) {
        gameInit();
        initDone = 1;
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
    
    
    mingin_getScreenSize( &w, &h );
    r = mingin_getStepsPerSecond();

    /* move box */
    boxVPerStep = boxVPerSecond / r;
    
    if( boxVPerStep < 1 ) {
        boxVPerStep = 1;
        }
    boxPosX += boxDir * boxVPerStep;

    if( boxDir > 0 && boxPosX > w ) {
        boxDir = -1;
        }
    else if( boxDir < 0 && boxPosX < 0 ) {
        boxDir = 1;
        }
    }



static MinginButton quitMapping[] = { MGN_KEY_Q, MGN_KEY_ESCAPE, MGN_MAP_END };
static MinginButton jumpMapping[] = { MGN_KEY_SPACE, MGN_MAP_END };
static MinginButton fullscreenMapping[] = { MGN_KEY_F, MGN_MAP_END };

static void gameInit( void ) {
    int w, h;
    
    mingin_registerButtonMapping( QUIT, quitMapping );
    mingin_registerButtonMapping( JUMP, jumpMapping );
    mingin_registerButtonMapping( FULLSCREEN_TOGGLE, fullscreenMapping );

    /* init position in screen center */
    mingin_getScreenSize( &w, &h );
    boxPosX = w / 2;
    boxPosY = h / 2;

    boxH = ( h * 3 ) / 4;
    }
