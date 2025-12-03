
#define MINGIN_IMPLEMENTATION

#define MAXIGIN_GAME_NATIVE_W  200
#define MAXIGIN_GAME_NATIVE_H  200

#define MAXIGIN_IMPLEMENTATION

#include "maxigin.h"



enum GameUserAction {
    JUMP,
    REMAP
    };



static int boxPosX = -1;
static int boxPosY = -1;
static int boxW = 10;
static int boxH = 40;
static int boxVPerSecond = 120;
static int boxDir = 1;



void maxiginGame_getNativePixels( unsigned char *inRGBBuffer ) {
    int boxStartX = boxPosX - boxW / 2;
    int boxStartY = boxPosY - boxH / 2;
    int numPixels = MAXIGIN_GAME_NATIVE_W * MAXIGIN_GAME_NATIVE_H;
    int p, x, y;
    
    /* white background */
    for( p = 0; p<numPixels; p++ ) {
        int pix = p * 3;

        inRGBBuffer[pix] = 255;
        inRGBBuffer[pix + 1] = 255;
        inRGBBuffer[pix + 2] = 255;
        }

    /* now draw black box */
    for( y = boxStartY; y < boxStartY + boxH; y++ ) {
        int rowStart = y * MAXIGIN_GAME_NATIVE_W * 3;
        
        for( x = boxStartX; x < boxStartX + boxW; x ++ ) {
            int pix = rowStart + x * 3;
            inRGBBuffer[pix] = (unsigned char)( 255 * (y-boxStartY) / boxH );
            inRGBBuffer[pix + 1] = 0;
            inRGBBuffer[pix + 2] = 0;
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
    
    }



static char remappingJump = 0;


void maxiginGame_step( void ) {
    int r;
    int boxVPerStep;

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

    if( mingin_isButtonDown( JUMP ) ) {
        boxPosY -= 1;
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

    if( boxDir > 0 && boxPosX > MAXIGIN_GAME_NATIVE_W ) {
        boxDir = -1;
        }
    else if( boxDir < 0 && boxPosX < 0 ) {
        boxDir = 1;
        }
    }



static MinginButton jumpMapping[] = { MGN_KEY_SPACE, MGN_MAP_END };

static MinginButton remapMapping[] = { MGN_KEY_P, MGN_MAP_END };


#define REGISTER_INT_MEM( x )  \
    maxigin_initRegisterStaticMemory( &x, sizeof(x), #x )


void maxiginGame_init( void ) {
    
    mingin_registerButtonMapping( JUMP, jumpMapping );
    mingin_registerButtonMapping( REMAP, remapMapping );
    
    /* init position in image center */
    boxPosX = MAXIGIN_GAME_NATIVE_W / 2;
    boxPosY = MAXIGIN_GAME_NATIVE_H / 2;

    boxH = ( MAXIGIN_GAME_NATIVE_H * 3 ) / 12;


    REGISTER_INT_MEM( boxPosX );
    REGISTER_INT_MEM( boxPosY );
    REGISTER_INT_MEM( boxW );
    REGISTER_INT_MEM( boxH );
    REGISTER_INT_MEM( boxVPerSecond );
    REGISTER_INT_MEM( boxDir );

    maxigin_initRestoreStaticMemoryFromLastRun();
    }
