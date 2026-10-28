/*
  Include in your C code wherever like so:

      #include "moveDiagramWindow.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define MOVE_DIAGRAM_WINDOW_IMPLEMENTATION
      #include "moveDiagramWindow.h"

*/

#ifndef MOVE_DIAGRAM_WINDOW_H_INCLUDED
#define MOVE_DIAGRAM_WINDOW_H_INCLUDED


#endif


void moveDiagramWindowInit( int  inCenterX,
                       int  inCenterY );


void moveDiagramWindowSet( ChessPiece  inPiece );


void moveDiagramWindowStep( void );


void moveDiagramWindowDraw( void );



#ifdef  MOVE_DIAGRAM_WINDOW_IMPLEMENTATION

#ifndef MOVE_DIAGRAM_WINDOW_IMPLEMENTATION_INCLUDED
#define MOVE_DIAGRAM_WINDOW_IMPLEMENTATION_INCLUDED


#define                MOVE_DIAGRAM_WINDOW_PROGRESS_MAX     100
#define                MOVE_DIAGRAM_DIM_MIN                 32
static  int            mdCenterX;
static  int            mdCenterY;
static  int            mdWindowSprite              =  -1;
static  int            mdShadowSprite          =  -1;
static  int            mdGlintSprite               =  -1;
static  int            mdPaddleSprite              =  -1;

static  ChessPiece     mdCurrentPiece              =  noPiece;
static  ChessPiece     mdNextPiece                 =  noPiece;
static  char           mdDimCurrent                =  0;
static  char           mdDimNext                   =  0;
static  unsigned char  mdDimAlphaCurrent           =  255;
static  unsigned char  mdDimAlphaNext              =  255;

static  int            mdProgress                  =   0;



void moveDiagramWindowInit( int  inCenterX,
                       int  inCenterY ) {
    mdCenterX = inCenterX;
    mdCenterY = inCenterY;

    mdWindowSprite     = maxigin_initSprite( "moveDiagramWindow.tga" );
    mdShadowSprite     = maxigin_initSprite( "moveDiagramWindowShadow.tga" );
    mdGlintSprite      = maxigin_initSprite( "moveDiagramWindowGlint.tga" );
    mdPaddleSprite     = maxigin_initSprite( "moveDiagramWindowPaddle.tga" );


    /* hazy, faded black shadow  top-to-bottom */
    maxigin_initMakeDropShadowSprite(
        mdPaddleSprite,
        3,
        2,
        255,
        255,
        100,
        0,
        100,
        0 );
    
    REGISTER_VAL_MEM( mdCurrentPiece );
    REGISTER_VAL_MEM( mdNextPiece );
    REGISTER_VAL_MEM( mdProgress );

    REGISTER_VAL_MEM( mdDimAlphaCurrent );
    REGISTER_VAL_MEM( mdDimAlphaNext );
    }



void moveDiagramWindowSet( ChessPiece  inNewPiece ) {

    if( inNewPiece == mdCurrentPiece ) {
        mdDimCurrent = 0;
        return;
        }
    
    if( mdCurrentPiece == noPiece ) {
        mdNextPiece = inNewPiece;
        return;
        }

    if( inNewPiece == noPiece ) {
        mdDimCurrent = 1;
        mdDimNext    = 1;
        return;
        }
    }


static unsigned char mdStepDim( char           inDim,
                                unsigned char  inAlpha ) {

    int  r        =  mingin_getStepsPerSecond();
    int  dimRate  =  r * 40 / 60;

    if( inDim
        &&
        inAlpha > MOVE_DIAGRAM_DIM_MIN ) {

        int  newDim  =  inAlpha - dimRate;

        if( newDim < MOVE_DIAGRAM_DIM_MIN ) {
            newDim = MOVE_DIAGRAM_DIM_MIN;
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



void moveDiagramWindowStep( void ) {

    int  r        =  mingin_getStepsPerSecond();

    if( mdNextPiece != mdCurrentPiece ) {

        mdProgress += r * 20 / 60;

        if( mdProgress >= MOVE_DIAGRAM_WINDOW_PROGRESS_MAX ) {

            mdCurrentPiece = mdNextPiece;

            mdProgress = 0;

            if( mdDimNext == 0 ) {
                mdDimCurrent        = 0;
                mdDimAlphaCurrent   = 255;
                }
            }
        }

    mdDimAlphaCurrent = mdStepDim( mdDimCurrent,
                                   mdDimAlphaCurrent );
    
    mdDimAlphaNext    = mdStepDim( mdDimNext,
                                   mdDimAlphaNext );
    
    }



void moveDiagramWindowDraw( void ) {

    int  yRollOffset  =  mdProgress * 16 / MOVE_DIAGRAM_WINDOW_PROGRESS_MAX;

    maxigin_drawResetColor();
    
    maxigin_drawSprite( mdWindowSprite,
                        mdCenterX,
                        mdCenterY );

    maxigin_drawPushClipRectangle( mdCenterX - 33,
                                   mdCenterX + 35,
                                   mdCenterY - 16,
                                   mdCenterY + 11 );


    maxigin_drawSprite( mdPaddleSprite,
                        mdCenterX + 1,
                        mdCenterY - yRollOffset - 2 + 10 );

    if( mdCurrentPiece != noPiece ) {
        
        maxigin_drawResetColor();

        if( mdProgress != 0 ) {
            long  fade  =
                255 - 255 * mdProgress / MOVE_DIAGRAM_WINDOW_PROGRESS_MAX;

            if( mdDimCurrent ) {
                fade = fade * mdDimAlphaCurrent / 255;
                }
            
            maxigin_drawSetAlpha( (unsigned char)fade );
            }
        else {
            maxigin_drawSetAlpha( mdDimAlphaCurrent );
            }

        
        }
    if( mdNextPiece != noPiece ) {

        maxigin_drawResetColor();

        if( mdProgress != 0 ) {
            long  fade  =  255 * mdProgress / MOVE_DIAGRAM_WINDOW_PROGRESS_MAX;

            if( mdDimNext ) {
                fade = fade * mdDimAlphaNext / 255;
                }
            
            maxigin_drawSetAlpha( (unsigned char)fade );
            }
        }

    
    maxigin_drawSprite( mdShadowSprite,
                        mdCenterX + 1,
                        mdCenterY - 2 );

    maxigin_drawSprite( mdGlintSprite,
                        mdCenterX + 34,
                        mdCenterY - 14 );

    maxigin_drawPopClipRectangle();
    }



#endif

#endif
