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


static  char        checkRunning      =  0;
static  int         checkProgress;
static  int         checkProgressMax  =  512;
static  int         checkKingX;
static  int         checkKingY;
static  Move        checkingMove;
static  ChessPiece  checkingPiece;


static  int   checkSprite       =  -1;


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
    }



void checkDisplayDraw( int  inBoardCenterX,
                       int  inBoardCenterY ) {

    int   kingScreenX;
    int   kingScreenY;
    int   threatScreenX;
    int   threatScreenY;
    long  fade;
    long  jump;
    
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
    

    drawPieceBaseAndGlowOnlyNoColor( checkingPiece,
                                     threatScreenX,
                                     threatScreenY );
    }



void checkDisplayStep( void ) {
    int  r  = mingin_getStepsPerSecond();

    checkProgress += ( 10 * 60 ) / r;

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
        }
    }



#endif

#endif
