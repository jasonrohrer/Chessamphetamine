/*
  Include in your C code wherever like so:

      #include "checkDisplay.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define CHECK_DISPLAY_IMPLEMENTATION
      #include "checkDisplay.h"

*/

#ifndef CHECK_DISPLAY_H_INCLUDED
#define CHECK_DISPLAY_H_INCLUDED

#include "chess.h"


void checkDisplayInit( void );

void checkDisplayDraw( int  inBoardCenterX,
                       int  inBoardCenterY );


void checkDisplayStep( void );

void checkDisplayStartCheck( BoardState  *inState );

                             


/* returns 1 if check animations are settled and done */
char checkDisplayIsSettled( void );



#ifdef CHECK_DISPLAY_IMPLEMENTATION

#include "memoryRegister.h"
#include "util.h"


static  char        checkRunning      =  0;
static  int         checkProgress;
static  int         checkProgressMax  =  512;
static  int         checkKingX;
static  int         checkKingY;
static  Move        checkingMove;
static  ChessPiece  checkingPiece;


static  int   checkSprite       =  -1;
static  int   checkSound        =  -1;


void checkDisplayInit( void ) {

    REGISTER_VAL_MEM( checkRunning );
    REGISTER_VAL_MEM( checkProgress );
    REGISTER_VAL_MEM( checkKingX );
    REGISTER_VAL_MEM( checkKingY );
    REGISTER_VAL_MEM( checkingMove );
    REGISTER_VAL_MEM( checkingPiece );

    checkSprite = maxigin_initSprite( "check.tga" );

    if( checkSprite != -1 ) {
        maxigin_initMakeGlowSprite( checkSprite,
                                    4,
                                    2 );
        /* hazy, faded black shadow  top-to-bottom */
        maxigin_initMakeDropShadowSprite(
            checkSprite,
            4,
            2,
            192,
            192,
            60,
            30,
            50,
            0 );
        }

    checkSound = maxigin_initSoundEffect( "check_sd_19.wav" );
    }



static int getMoveLength( Move  *inMove ) {
    
    int  dX  =  inMove->endPos[1] - inMove->startPos[1];
    int  dY  =  inMove->endPos[0] - inMove->startPos[0];

    if( dX < 0 ) {
        dX *= -1;
        }
    if( dY < 0 ) {
        dY *= -1;
        }

    if( dX > dY ) {
        return dX;
        }
    else {
        return dY;
        }
    }
    


void checkDisplayDraw( int  inBoardCenterX,
                       int  inBoardCenterY ) {

    int   kingScreenX;
    int   kingScreenY;
    int   threatScreenX;
    int   threatScreenY;
    long  fade;
    long  jump;
    int   bounceProgress;
    int   bounceMax;
    
    if( ! checkRunning ) {
        return;
        }

    
        
    boardGetSquareCenter( inBoardCenterX,
                          inBoardCenterY,
                          checkKingY,
                          checkKingX,
                          &kingScreenX,
                          &kingScreenY );

    fade = ( (long)checkProgress * 255 ) / checkProgressMax;

    fade = 255 - fade;
    
    jump = ( (long)checkProgress * 30 ) / checkProgressMax;
    
    maxigin_drawSetColor( 255,
                          0,
                          0,
                          (unsigned char)fade );

    maxigin_drawSprite( checkSprite,
                        kingScreenX,
                        kingScreenY - (int)jump - 10);

    boardGetSquareCenter( inBoardCenterX,
                          inBoardCenterY,
                          checkingMove.startPos[0],
                          checkingMove.startPos[1],
                          &threatScreenX,
                          &threatScreenY );


    bounceMax      = checkProgressMax / 2;
    bounceProgress = checkProgress;
    if( bounceProgress > bounceMax ) {
        bounceProgress = bounceMax;
        }
    
    drawPieceBaseAndGlowOnlyNoColor( checkingPiece,
                                     threatScreenX,
                                     threatScreenY - parabola( bounceProgress,
                                                               bounceMax,
                                                               10  ) );

    /* do this always, for now. */
    if( 1
        ||
        checkKingX != checkingMove.endPos[1]
        ||
        checkKingY != checkingMove.endPos[0] ) {
        /* piece doesn't take king directly with basic move
           must take king with special effect,
           highlight piece destination to make this clear */
        int   destX;
        int   destY;
        long  drawX;
        long  drawY;

        boardGetSquareCenter( inBoardCenterX,
                              inBoardCenterY,
                              checkingMove.endPos[0],
                              checkingMove.endPos[1],
                              &destX,
                              &destY );

        if( getMoveLength( &checkingMove ) > 1 ) {
            /* piece is moving long distance, show slide effect */
        
            drawX =
                ( bounceProgress * destX
                  + ( bounceMax - bounceProgress ) * threatScreenX )
                / bounceMax;

            drawY =
                ( bounceProgress * destY
                  + ( bounceMax - bounceProgress ) * threatScreenY )
                / bounceMax;
        
            drawPieceBaseAndGlowOnlyNoColor( checkingPiece,
                                             (int)drawX,
                                             (int)drawY );
            }
        

        if( checkKingX != checkingMove.endPos[1]
            ||
            checkKingY != checkingMove.endPos[0] ) {

            /* if we don't land right on king, show our destination
               position too */
            
            drawPieceBaseAndGlowOnlyNoColor( checkingPiece,
                                             (int)destX,
                                             (int)destY );
            }
        
        }
    
        
        
    }



void checkDisplayStep( void ) {
    int  r  = mingin_getStepsPerSecond();

    checkProgress += ( 20 * 60 ) / r;

    if( checkProgress >= checkProgressMax ) {
        checkRunning = 0;
        } 
    }



char checkDisplayIsSettled( void ) {
    return ! checkRunning;
    }



void checkDisplayStartCheck( BoardState  *inState ) {

    if( isKingInCheckGetMove( inState,
                              inState->nextToMove,
                              &checkKingX,
                              &checkKingY,
                              &checkingMove ) ) {

        checkRunning  = 1;
        checkProgress = 0;

        checkingPiece = inState->grid[ checkingMove.startPos[0] ]
                                     [ checkingMove.startPos[1] ];

        maxigin_playSoundEffect( checkSound,
                                 512 );
        }
    }



#endif

#endif
