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


void rarityWindowStep( void );


void rarityWindowDraw( void );



#ifdef  RARITY_WINDOW_IMPLEMENTATION

#ifndef RARITY_WINDOW_IMPLEMENTATION_INCLUDED
#define RARITY_WINDOW_IMPLEMENTATION_INCLUDED


static  int  rwCenterX;
static  int  rwCenterY;

static  int  rwWindowSprite      =  -1;
static  int  rwFilmSprite        =  -1;
static  int  rwFilmShadowSprite  =  -1;


void rarityWindowInit( int  inCenterX,
                       int  inCenterY ) {
    rwCenterX = inCenterX;
    rwCenterY = inCenterY;

    rwWindowSprite     = maxigin_initSprite( "rarityWindow.tga" );
    rwFilmSprite       = maxigin_initSprite( "rarityWindowFilm.tga" );
    rwFilmShadowSprite = maxigin_initSprite( "rarityWindowShadow.tga" );
    }



void rarityWindowStep( void ) {
    }



void rarityWindowDraw( void ) {

    maxigin_drawResetColor();
    
    maxigin_drawSprite( rwWindowSprite,
                        rwCenterX,
                        rwCenterY );
    }



#endif

#endif
