/*
  Include in your C code wherever like so:

      #include "rarityWindow.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define RARITY_WINDOW_IMPLEMENTATION
      #include "rarityWindow.h"

*/

#ifndef RARITY_WINDOW_H_INCLUDED
#define RARITY_WINDOW_H_INCLUDED


#endif


void rarityWindowInit( int  inCenterX,
                       int  inCenterY );


void rarityWindowSet( ChessPiece  inPiece );


void rarityWindowStep( void );


void rarityWindowDraw( void );



#ifdef  RARITY_WINDOW_IMPLEMENTATION

#ifndef RARITY_WINDOW_IMPLEMENTATION_INCLUDED
#define RARITY_WINDOW_IMPLEMENTATION_INCLUDED


#define                RARITY_WINDOW_PROGRESS_MAX     100
#define                RARITY_DIM_MIN                 32
static  int            rwCenterX;
static  int            rwCenterY;
static  int            rwWindowSprite              =  -1;
static  int            rwFilmSprite                =  -1;
static  int            rwFilmShadowSprite          =  -1;

static  ChessPiece     rwCurrentPiece              =  noPiece;
static  ChessPiece     rwNextPiece                 =  noPiece;
static  char           rwDimCurrent                =  0;
static  char           rwDimNext                   =  0;
static  unsigned char  rwDimAlphaCurrent           =  255;
static  unsigned char  rwDimAlphaNext              =  255;

static  int            rwProgress                  =   0;


void rarityWindowInit( int  inCenterX,
                       int  inCenterY ) {
    rwCenterX = inCenterX;
    rwCenterY = inCenterY;

    rwWindowSprite     = maxigin_initSprite( "rarityWindow.tga" );
    rwFilmSprite       = maxigin_initSprite( "rarityWindowFilm.tga" );
    rwFilmShadowSprite = maxigin_initSprite( "rarityWindowShadow.tga" );

    REGISTER_VAL_MEM( rwCurrentPiece );
    REGISTER_VAL_MEM( rwNextPiece );
    REGISTER_VAL_MEM( rwProgress );

    REGISTER_VAL_MEM( rwDimAlphaCurrent );
    REGISTER_VAL_MEM( rwDimAlphaNext );
    }



void rarityWindowSet( ChessPiece  inNewPiece ) {

    if( inNewPiece == rwCurrentPiece ) {
        rwDimCurrent = 0;
        return;
        }
    
    if( rwCurrentPiece == noPiece ) {
        rwNextPiece = inNewPiece;
        return;
        }

    if( inNewPiece == noPiece ) {
        rwDimCurrent = 1;
        rwDimNext    = 1;
        return;
        }
    

    if( rarityGet( inNewPiece )
        !=
        rarityGet( rwCurrentPiece ) ) {
    
        rwNextPiece = inNewPiece;
        rwDimNext   = 0;
        }
    else {
        rwDimCurrent = 0;
        }
    }


static unsigned char rwStepDim( char           inDim,
                                unsigned char  inAlpha ) {

    int  r        =  mingin_getStepsPerSecond();
    int  dimRate  =  r * 40 / 60;

    if( inDim
        &&
        inAlpha > RARITY_DIM_MIN ) {

        int  newDim  =  inAlpha - dimRate;

        if( newDim < RARITY_DIM_MIN ) {
            newDim = RARITY_DIM_MIN;
            }
        return (unsigned char) newDim;
        }
    else if( ! inDim
             &&
             inAlpha < 255 ) {
        
        int  newDim  =  inAlpha + dimRate;

        if( newDim > 255 ) {
            newDim = 255;
            }
        return (unsigned char) newDim;
        }

    return inAlpha;
    }



void rarityWindowStep( void ) {

    int  r        =  mingin_getStepsPerSecond();

    if( rwNextPiece != rwCurrentPiece ) {

        rwProgress += r * 20 / 60;

        if( rwProgress >= RARITY_WINDOW_PROGRESS_MAX ) {

            rwCurrentPiece = rwNextPiece;

            rwProgress = 0;

            if( rwDimNext == 0 ) {
                rwDimCurrent        = 0;
                rwDimAlphaCurrent   = 255;
                }
            }
        }

    rwDimAlphaCurrent = rwStepDim( rwDimCurrent,
                                   rwDimAlphaCurrent );
    
    rwDimAlphaNext    = rwStepDim( rwDimNext,
                                   rwDimAlphaNext );
    
    }



void rarityWindowDraw( void ) {

    int  yRollOffset  =  rwProgress * 16 / RARITY_WINDOW_PROGRESS_MAX;

    maxigin_drawResetColor();
    
    maxigin_drawSprite( rwWindowSprite,
                        rwCenterX,
                        rwCenterY );

    maxigin_drawPushClipRectangle( -1,
                                   -1,
                                   rwCenterY - 6,
                                   rwCenterY + 6 );
    
    maxigin_drawSprite( rwFilmSprite,
                        rwCenterX + 1,
                        rwCenterY - yRollOffset );

    maxigin_drawSprite( rwFilmSprite,
                        rwCenterX + 1,
                        rwCenterY + 16 - yRollOffset );
    
    maxigin_drawSprite( rwFilmShadowSprite,
                        rwCenterX + 1,
                        rwCenterY );

    maxigin_setLanguageFontIndex( 1 );

    if( rwCurrentPiece != noPiece ) {

        int  rarityKey;
        int  upperH;
        int  lowerH;
        
        raritySetDrawColor( rwCurrentPiece );

        if( rwProgress != 0 ) {
            long  fade  =  255 - 255 * rwProgress / RARITY_WINDOW_PROGRESS_MAX;

            if( rwDimCurrent ) {
                fade = fade * rwDimAlphaCurrent / 255;
                }
            
            maxigin_drawSetAlpha( (unsigned char)fade );
            }
        else {
            maxigin_drawSetAlpha( rwDimAlphaCurrent );
            }


        rarityKey = rarityGetLangKey( rwCurrentPiece );

        maxigin_measureLangTextVertical( rarityKey,
                                         &upperH,
                                         &lowerH );
    
        maxigin_drawLangText( rarityGetLangKey( rwCurrentPiece ),
                              rwCenterX,
                              rwCenterY - yRollOffset,
                              MAXIGIN_CENTER );

        if( 0 ) {
            
        maxigin_drawToggleAdditive( 1 );
        maxigin_drawLangText( rarityGetLangKey( rwCurrentPiece ),
                              rwCenterX,
                              rwCenterY - yRollOffset,
                              MAXIGIN_CENTER );
        maxigin_drawToggleAdditive( 0 );
            }
        
        }
    if( rwNextPiece != noPiece ) {

        int  rarityKey;
        int  upperH;
        int  lowerH;
        
        raritySetDrawColor( rwNextPiece );

        if( rwProgress != 0 ) {
            long  fade  =  255 * rwProgress / RARITY_WINDOW_PROGRESS_MAX;

            if( rwDimNext ) {
                fade = fade * rwDimAlphaNext / 255;
                }
            
            maxigin_drawSetAlpha( (unsigned char)fade );
            }

        rarityKey = rarityGetLangKey( rwNextPiece );

        maxigin_measureLangTextVertical( rarityKey,
                                         &upperH,
                                         &lowerH );
    
        maxigin_drawLangText( rarityGetLangKey( rwNextPiece ),
                              rwCenterX,
                              rwCenterY + 16 - yRollOffset,
                              MAXIGIN_CENTER );
        }
    
    maxigin_setLanguageFontIndex( 0 );

    maxigin_drawPopClipRectangle();
    }



#endif

#endif
