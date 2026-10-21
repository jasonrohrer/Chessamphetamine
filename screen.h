/*
  Include in your C code wherever like so:

      #include "screen.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define SCREEN_IMPLEMENTATION
      #include "screen.h"

*/

/* A simulated computer monitor sub-screen */

#ifndef SCREEN_H_INCLUDED
#define SCREEN_H_INCLUDED


void screenInit( int  inScreenCenterX,
                 int  inScreenCenterY );


/* draws screen bg color,
   starts trimming subsequent draw commands to fit on screen */
void screenStartDraw( void );


/* draws border around edge of screen, ends draw trimming */
void screenEndDraw( void );

#endif



#ifdef  SCREEN_IMPLEMENTATION

#ifndef SCREEN_IMPLEMENTATION_INCLUDED
#define SCREEN_IMPLEMENTATION_INCLUDED


static  int  screenTopEdgeSprite     =  -1;
static  int  screenBottomEdgeSprite  =  -1;
static  int  screenLeftEdgeSprite    =  -1;
static  int  screenRightEdgeSprite   =  -1;

static  int  screenCenterX;
static  int  screenCenterY;


void screenInit( int  inScreenCenterX,
                 int  inScreenCenterY ) {
    
    screenTopEdgeSprite    = maxigin_initSprite( "screenEdgeTop.tga" );
    screenBottomEdgeSprite = maxigin_initSprite( "screenEdgeBottom.tga" );
    screenLeftEdgeSprite   = maxigin_initSprite( "screenEdgeLeft.tga" );
    screenRightEdgeSprite  = maxigin_initSprite( "screenEdgeRight.tga" );

    screenCenterX = inScreenCenterX;
    screenCenterY = inScreenCenterY;
    }



void screenStartDraw( void ) {

    maxigin_drawSetColor( 34,
                          42,
                          92,
                          255 );
    
    maxigin_drawFillRect( screenCenterX - 98,
                          screenCenterY - 106,
                          screenCenterX + 97,
                          screenCenterY + 105 );
    
    maxigin_drawPushClipRectangle( screenCenterX - 98,
                                   screenCenterX + 97,
                                   screenCenterY - 106,
                                   screenCenterY + 105 );
    }



/* draws border around edge of screen, ends draw trimming */
void screenEndDraw( void ) {
    maxigin_drawPopClipRectangle();

    maxigin_drawResetColor();

    maxigin_drawSprite( screenTopEdgeSprite,
                        screenCenterX,
                        screenCenterY - 109 );
    
    maxigin_drawSprite( screenBottomEdgeSprite,
                        screenCenterX,
                        screenCenterY + 108 );
    
    maxigin_drawSprite( screenLeftEdgeSprite,
                        screenCenterX - 101,
                        screenCenterY );
    
    maxigin_drawSprite( screenRightEdgeSprite,
                        screenCenterX + 101,
                        screenCenterY );
    }



#endif

#endif
