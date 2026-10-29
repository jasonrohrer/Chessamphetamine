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




void moveDiagramWindowInit( int  inCenterX,
                            int  inCenterY );


void moveDiagramWindowSet( ChessPiece  inPiece );


void moveDiagramWindowStep( void );


void moveDiagramWindowDraw( void );


#endif


#ifdef  MOVE_DIAGRAM_WINDOW_IMPLEMENTATION

#ifndef MOVE_DIAGRAM_WINDOW_IMPLEMENTATION_INCLUDED
#define MOVE_DIAGRAM_WINDOW_IMPLEMENTATION_INCLUDED


#define                MOVE_DIAGRAM_WINDOW_PROGRESS_MAX     100
#define                MOVE_DIAGRAM_DIM_MIN                 32
static  int            mdCenterX;
static  int            mdCenterY;
static  int            mdWindowSprite              =  -1;
static  int            mdShadowSprite              =  -1;
static  int            mdGlintSprite               =  -1;
static  int            mdPaddleSprite              =  -1;

static  int            diagramBoardStrip           =  -1;
static  int            diagramPieceStrip           =  -1;


#define                MD_NUM_PADDLES                   4
static  int            mdLivePaddleIndex           =   -1;
static  int            mdNextPaddleIndex           =    0;

static  ChessPiece     mdPaddlePieces  [ MD_NUM_PADDLES ];
static  int            mdPaddleProgress[ MD_NUM_PADDLES ];




void moveDiagramWindowInit( int  inCenterX,
                       int  inCenterY ) {

    int  i;

    
    mdCenterX = inCenterX;
    mdCenterY = inCenterY;

    mdWindowSprite     = maxigin_initSprite( "moveDiagramWindow.tga" );
    mdShadowSprite     = maxigin_initSprite( "moveDiagramWindowShadow.tga" );
    mdGlintSprite      = maxigin_initSprite( "moveDiagramWindowGlint.tga" );
    mdPaddleSprite     = maxigin_initSprite( "moveDiagramWindowPaddle.tga" );

    diagramBoardStrip = maxigin_initSpriteStrip( "moveDiagrams_boards.tga",
                                                 27 );

    
    diagramPieceStrip = maxigin_initSpriteStrip( "moveDiagrams_pieces.tga",
                                                 27 );


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

    for( i = 0;
         i < MD_NUM_PADDLES;
         i   ++ ) {

        mdPaddleProgress[ i ] = 0;
        mdPaddlePieces  [ i ] = noPiece;
        }
    
    REGISTER_ARRAY_MEM( mdPaddleProgress );
    REGISTER_ARRAY_MEM( mdPaddlePieces );

    REGISTER_VAL_MEM( mdLivePaddleIndex );
    REGISTER_VAL_MEM( mdNextPaddleIndex );
    }



void moveDiagramWindowSet( ChessPiece  inNewPiece ) {

    ChessPiece  t  =  inNewPiece & CHESS_TYPE_MASK;

    if( t == noPiece ) {

        mdLivePaddleIndex = -1;
        
        return;
        }


    if( t == mdPaddlePieces[ mdLivePaddleIndex ] ) {
        return;
        }
        

    mdLivePaddleIndex = mdNextPaddleIndex;
    mdPaddlePieces[ mdNextPaddleIndex ] = t;
    mdNextPaddleIndex ++;

    if( mdNextPaddleIndex >= MD_NUM_PADDLES ) {
        mdNextPaddleIndex = 0;
        }
    }



void moveDiagramWindowStep( void ) {

    int  r         =  mingin_getStepsPerSecond();
    int  i;
    int  progRate  =  r * 20 / 60;

    for( i = 0;
         i < MD_NUM_PADDLES;
         i   ++ ) {

        if( i == mdLivePaddleIndex
            &&
            mdPaddleProgress[ i ] < MOVE_DIAGRAM_WINDOW_PROGRESS_MAX ) {

            mdPaddleProgress[ i ] += progRate;

            if( mdPaddleProgress[ i ] >= MOVE_DIAGRAM_WINDOW_PROGRESS_MAX ) {
                mdPaddleProgress[ i ] = MOVE_DIAGRAM_WINDOW_PROGRESS_MAX;
                }
            }
        else if( mdPaddleProgress[ i ] > 0 ) {
            mdPaddleProgress[ i ] -= progRate;

            if( mdPaddleProgress[ i ] <= 0 ) {
                mdPaddleProgress[ i ] = 0;
                }
            }
        
        }
    
    }



void moveDiagramWindowDraw( void ) {

    
    int  i;
    
    maxigin_drawResetColor();
    
    maxigin_drawSprite( mdWindowSprite,
                        mdCenterX,
                        mdCenterY );

    maxigin_drawPushClipRectangle( mdCenterX - 33,
                                   mdCenterX + 35,
                                   mdCenterY - 16,
                                   mdCenterY + 11 );

    for( i = 0;
         i < MD_NUM_PADDLES;
         i   ++ ) {

        if( mdPaddleProgress[ i ] > 0 ) {
            
            int  yRollOffset  =
                mdPaddleProgress[ i ] * 16 / MOVE_DIAGRAM_WINDOW_PROGRESS_MAX;

            maxigin_drawResetColor();

            maxigin_drawSprite( mdPaddleSprite,
                                mdCenterX + 1,
                                mdCenterY - yRollOffset - 2 );


            if( maxigin_getNumSpritesInStrip( diagramBoardStrip ) >
                mdPaddlePieces[ i ] ) {
        
                colorsApplyBoardColor();

                maxigin_drawSprite( maxigin_getSpriteFromStrip(
                                        diagramBoardStrip,
                                        mdPaddlePieces[ i ] ),
                                    mdCenterX,
                                    mdCenterY - yRollOffset - 2 );
                }

            if( maxigin_getNumSpritesInStrip( diagramPieceStrip ) >
                mdPaddlePieces[ i ] ) {

                maxigin_drawSetColor( 255,
                                      255, 
                                      255,
                                      255 );

                maxigin_drawSprite( maxigin_getSpriteFromStrip(
                                        diagramPieceStrip,
                                        mdPaddlePieces[ i ] ),
                                    mdCenterX,
                                    mdCenterY - yRollOffset - 2 );
                }
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
