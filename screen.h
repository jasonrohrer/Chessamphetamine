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
    
    maxigin_drawFillRect( screenCenterX - 101,
                          screenCenterY - 109,
                          screenCenterX + 100,
                          screenCenterY + 113 );
    
    /* note that clip rectangle does NOT affect left/right/bottom
       edges, since we want glows and particle effects to bleed out

    fixme */
    maxigin_drawPushClipRectangle( screenCenterX - 101,
                                   screenCenterX + 100,
                                   screenCenterY - 109,
                                   screenCenterY + 113 );
    }



/* draws border around edge of screen, ends draw trimming */
void screenEndDraw( void ) {
    maxigin_drawPopClipRectangle();

    maxigin_drawResetColor();

    maxigin_drawSprite( screenTopEdgeSprite,
                        screenCenterX,
                        screenCenterY - 112 );
    
    maxigin_drawSprite( screenBottomEdgeSprite,
                        screenCenterX,
                        screenCenterY + 116 );
    
    maxigin_drawSprite( screenLeftEdgeSprite,
                        screenCenterX - 104,
                        screenCenterY + 2 );
    
    maxigin_drawSprite( screenRightEdgeSprite,
                        screenCenterX + 104,
                        screenCenterY + 2 );
    }



#endif

#endif
