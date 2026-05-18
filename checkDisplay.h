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


void checkDisplayInit( void );

void checkDisplayDraw( int  inBoardCenterX,
                       int  inBoardCenterY );


void checkDisplayStep( void );

void checkDisplayStartCheck( int    inKingX,
                             int    inKingY,
                             Move  *inCheckingMove );

                             


/* returns 1 if check animations are settled and done */
char checkDisplayIsSettled( void );



#ifdef CHECK_DISPLAY_IMPLEMENTATION

#include "memoryRegister.h"


static  char  checkRunning      =  0;
static  int   checkProgress;
static  int   checkProgressMax  =  512;
static  int   checkKingX;
static  int   checkKingY;
static  Move  checkingMove;



void checkDisplayInit( void ) {

    REGISTER_VAL_MEM( checkRunning );
    REGISTER_VAL_MEM( checkProgress );
    REGISTER_VAL_MEM( checkKingX );
    REGISTER_VAL_MEM( checkKingY );
    REGISTER_VAL_MEM( checkingMove );
    }



void checkDisplayDraw( int  inBoardCenterX,
                       int  inBoardCenterY ) {

    int   kingScreenX;
    int   kingScreenY;
    long  fade;
    
    
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
    
    maxigin_drawSetColor( 255,
                          0,
                          0,
                          (unsigned char)fade );

    maxigin_drawFillRect( kingScreenX - 10,
                          kingScreenY - 10,
                          kingScreenX + 10,
                          kingScreenY + 10 );                  
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



void checkDisplayStartCheck( int    inKingX,
                             int    inKingY,
                             Move  *inCheckingMove ) {
    checkRunning  = 1;
    checkProgress = 0;
    checkKingX         = inKingX;
    checkKingY         = inKingY;

    checkingMove = *inCheckingMove;
    }



#endif

#endif
