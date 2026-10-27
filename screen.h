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
   starts trimming subsequent draw commands to fit on screen,

   inForceClip controls whether clipping is forced or not on top edge
   (enable when pieces are dropping in, or board is sliding up) */
void screenStartDraw( char  inForceClip );


/* ends any clipping started by screenStartDraw
   Obeys orginal setting of inForceClip when toggling back on
   (only toggles on if inForceClip was 1 ) */
void screenToggleClip( char  inClipOn );


/* draws border around edge of screen, ends draw trimming */
void screenEndDraw( void );

#endif



#ifdef  SCREEN_IMPLEMENTATION

#ifndef SCREEN_IMPLEMENTATION_INCLUDED
#define SCREEN_IMPLEMENTATION_INCLUDED


static  int   screenTopEdgeSprite     =  -1;
static  int   screenBottomEdgeSprite  =  -1;
static  int   screenLeftEdgeSprite    =  -1;
static  int   screenRightEdgeSprite   =  -1;

static  int   screenCenterX;
static  int   screenCenterY;
static  int   screenClipY;

static  char  screenClipForced        =   0;
static  char  screenClipToggle        =   1;


void screenInit( int  inScreenCenterX,
                 int  inScreenCenterY ) {
    
    screenTopEdgeSprite    = maxigin_initSprite( "screenEdgeTop.tga" );
    screenBottomEdgeSprite = maxigin_initSprite( "screenEdgeBottom.tga" );
    screenLeftEdgeSprite   = maxigin_initSprite( "screenEdgeLeft.tga" );
    screenRightEdgeSprite  = maxigin_initSprite( "screenEdgeRight.tga" );

    screenCenterX = inScreenCenterX;
    screenCenterY = inScreenCenterY;

    screenClipY   = screenCenterY - 109;
    }



void screenStartDraw( char  inForceClip ) {

    maxigin_drawSetColor( 34,
                          42,
                          92,
                          255 );
    
    maxigin_drawFillRect( screenCenterX - 101,
                          screenCenterY - 109,
                          screenCenterX + 100,
                          screenCenterY + 113 );

    /* draw bottom, left, and right edges now so taht
       glows and particles can go over top of those frame pieces */
    maxigin_drawResetColor();
    maxigin_drawSprite( screenBottomEdgeSprite,
                        screenCenterX,
                        screenCenterY + 116 );
    
    maxigin_drawSprite( screenLeftEdgeSprite,
                        screenCenterX - 104,
                        screenCenterY + 2 );
    
    maxigin_drawSprite( screenRightEdgeSprite,
                        screenCenterX + 104,
                        screenCenterY + 2 );

    screenClipForced = inForceClip;

    if( inForceClip ) {
        /* note that clip rectangle does NOT affect left/right/bottom
           edges, since we want glows and particle effects to bleed out */
        maxigin_drawPushClipRectangle( -1,
                                       -1,
                                       screenClipY,
                                       -1 );
        }
    else {
        /* no clipping, draw top edge now */
        maxigin_drawSprite( screenTopEdgeSprite,
                            screenCenterX,
                            screenCenterY - 112 );
        }
    
    screenClipToggle = 1;
    }



void screenToggleClip( char  inClipOn ) {

    if( ! screenClipForced ) {
        /* ignore toggle */
        return;
        }

    if( inClipOn
        &&
        ! screenClipToggle ) {

        screenClipToggle = 1;

        maxigin_drawPushClipRectangle( -1,
                                       -1,
                                       screenClipY,
                                       -1 );
        }
    else if( ! inClipOn
             &&
             screenClipToggle ) {

        screenClipToggle = 0;

        maxigin_drawPopClipRectangle();
        }  
    }



/* draws border around edge of screen, ends draw trimming */
void screenEndDraw( void ) {

    if( screenClipForced ) {

        if( screenClipToggle ) {
            maxigin_drawPopClipRectangle();
            }

        screenClipToggle = 1;
        
        screenClipForced = 0;

        maxigin_drawResetColor();

        /* draw top edge over top of everything, so that when pieces lift
           off board, they disappear behind top frame edge. */
        maxigin_drawSprite( screenTopEdgeSprite,
                            screenCenterX,
                            screenCenterY - 112 );
        }
    }



#endif

#endif
