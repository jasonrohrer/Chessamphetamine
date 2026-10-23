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


static  int         rwCenterX;
static  int         rwCenterY;

static  int         rwWindowSprite      =  -1;
static  int         rwFilmSprite        =  -1;
static  int         rwFilmShadowSprite  =  -1;

static  ChessPiece  rwCurrentPiece      =   noPiece;
static  ChessPiece  rwNextPiece         =   noPiece;

static  int         rwProgress          =   0;


void rarityWindowInit( int  inCenterX,
                       int  inCenterY ) {
    rwCenterX = inCenterX;
    rwCenterY = inCenterY;

    rwWindowSprite     = maxigin_initSprite( "rarityWindow.tga" );
    rwFilmSprite       = maxigin_initSprite( "rarityWindowFilm.tga" );
    rwFilmShadowSprite = maxigin_initSprite( "rarityWindowShadow.tga" );
    }



void rarityWindowSet( ChessPiece  inNewPiece ) {

    if( inNewPiece == rwCurrentPiece ) {
        return;
        }
    
    if( inNewPiece == noPiece
        ||
        rwCurrentPiece == noPiece ) {
        rwNextPiece = inNewPiece;
        return;
        }

    if( rarityGet( inNewPiece )
        !=
        rarityGet( rwCurrentPiece ) ) {
    
        rwNextPiece = inNewPiece;
        }
    }



void rarityWindowStep( void ) {

    int  r  =  mingin_getStepsPerSecond();

    if( rwNextPiece != rwCurrentPiece ) {

        rwProgress += r * 20 / 60;

        if( rwProgress >= 100 ) {

            rwCurrentPiece = rwNextPiece;

            rwProgress = 0;
            }
        }
    }



void rarityWindowDraw( void ) {

    int  yRollOffset  =  rwProgress * 16 / 100;

    maxigin_drawResetColor();
    
    maxigin_drawSprite( rwWindowSprite,
                        rwCenterX,
                        rwCenterY );

    maxigin_drawPushClipRectangle( -1,
                                   -1,
                                   rwCenterY - 6,
                                   rwCenterY + 6 );
    
    maxigin_drawSprite( rwFilmSprite,
                        rwCenterX,
                        rwCenterY - yRollOffset );

    maxigin_drawSprite( rwFilmSprite,
                        rwCenterX,
                        rwCenterY + 16 - yRollOffset );
    
    maxigin_drawSprite( rwFilmShadowSprite,
                        rwCenterX,
                        rwCenterY );

    maxigin_setLanguageFontIndex( 1 );

    if( rwCurrentPiece != noPiece ) {

        int  rarityKey;
        int  upperH;
        int  lowerH;
        
        raritySetDrawColor( rwCurrentPiece );


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
