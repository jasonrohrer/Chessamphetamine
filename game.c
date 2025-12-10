
#define MINGIN_IMPLEMENTATION

#define MAXIGIN_GAME_NATIVE_W  200
#define MAXIGIN_GAME_NATIVE_H  200

#define MAXIGIN_IMPLEMENTATION

#include "maxigin.h"



enum GameUserAction {
    JUMP,
    SHOOT,
    REMAP
    };


#define  MAX_NUM_BULLETS  20

typedef struct Vector{
        int  x;
        int  y;
    } Vector;


static  char    bulletOn[ MAX_NUM_BULLETS ];

static  Vector  bulletPos[ MAX_NUM_BULLETS ];

static  Vector  bulletSpeed[ MAX_NUM_BULLETS ];

static  int     stepsSinceLastBullet  =  0;
static  int     msBetweenBullets      =  100;


static int boxPosX = -1;
static int boxPosY = -1;
static int boxW = 10;
static int boxH = 40;
static int boxVPerSecond = 120;
static int boxDir = 1;



void maxiginGame_getNativePixels( unsigned char *inRGBBuffer ) {
    
    int  boxStartX  =  boxPosX - boxW / 2;
    int  boxStartY  =  boxPosY - boxH / 2;
    int  numPixels  =  MAXIGIN_GAME_NATIVE_W * MAXIGIN_GAME_NATIVE_H;
    int  p;
    int  x;
    int  y;
    int  i;
    
    /* white background */
    for( p = 0; p<numPixels; p++ ) {
        int pix = p * 3;

        inRGBBuffer[pix] = 255;
        inRGBBuffer[pix + 1] = 255;
        inRGBBuffer[pix + 2] = 255;
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

    
    for( i = 0;
         i < MAX_NUM_BULLETS;
         i ++ ) {
        
        if( bulletOn[ i ] ) {
            
            int  startX  =  bulletPos[ i ].x - 2;
            int  startY  =  bulletPos[ i ].y - 2;
            int  endX    =  bulletPos[ i ].x + 2;
            int  endY    =  bulletPos[ i ].y + 2;

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
            
            break;
            }
        }
    
    
    }



static char remappingJump = 0;


void maxiginGame_step( void ) {
    
    int  r;
    int  i;
    int  boxVPerStep;

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
            }
        }
    
        
    
    
    

    /* move box */
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


    /* move bullets */
    for( i = 0;
         i < MAX_NUM_BULLETS;
         i ++ ) {

        if( bulletOn[ i ] ) {

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
            
            }
        }
    }



static MinginButton jumpMapping[]  =   { MGN_KEY_SPACE,  MGN_MAP_END };

static MinginButton shootMapping[]  =  { MGN_KEY_X,      MGN_MAP_END };

static MinginButton remapMapping[]  =  { MGN_KEY_P,      MGN_MAP_END };


#define REGISTER_INT_MEM( x )  \
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

    
    maxigin_registerButtonMapping( JUMP,   jumpMapping );
    maxigin_registerButtonMapping( SHOOT,  shootMapping );
    maxigin_registerButtonMapping( REMAP,  remapMapping );
    
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

    REGISTER_ARRAY_MEM( bulletOn );
    REGISTER_ARRAY_MEM( bulletPos );
    REGISTER_ARRAY_MEM( bulletSpeed );

    REGISTER_INT_MEM( stepsSinceLastBullet );

    maxigin_initRestoreStaticMemoryFromLastRun();
    }
