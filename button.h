/*
  Include in your C code wherever like so:

      #include "button.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define BUTTON_IMPLEMENTATION
      #include "button.h"

*/

#ifndef BUTTON_H_INCLUDED
#define BUTTON_H_INCLUDED


/* base sprite required
   hover and pressed sprite can be -1

   both action handles are maxigin-registered handles
   
   inPointerActionHandle  is a registered action that should trigger the button
                          when the pointer is over it, or -1.
                          
   inActionHandle         is a registered action that should trigger the button,
                          or -1
*/
int buttonInit( int  inBaseSprite,
                int  inHoverSprite,
                int  inPressedSprite,
                int  inCenterX,
                int  inCenterY,
                char inSticky,
                int  inPointerActionHandle,
                int  inActionHandle );


void buttonDraw( int  inButtonHandle );


/* must be called every step, since the button has no
   internal stepping  to check for events */
char buttonIsNewPressed( int  inButtonHandle );

char buttonIsHover( int  inButtonHandle );


char buttonIsStuck( int  inButtonHandle );



/* only affects sticky buttons */
void buttonReset( int  inButtonHandle );

void buttonGetPos( int   inButtonHandle,
                   int  *outX,
                   int  *outY );



#endif



#ifdef  BUTTON_IMPLEMENTATION

#ifndef BUTTON_IMPLEMENTATION_INCLUDED
#define BUTTON_IMPLEMENTATION_INCLUDED




#include "memoryRegister.h"




typedef struct Button {
        int  baseSprite;
        int  hoverSprite;
        int  pressedSprite;
        int  centerX;
        int  centerY;
        char sticky;
        int  pointerActionHandle;
        int  actionHandle;

        char sawUnpressed;

        char pressed;
        char hover;
    } Button;

#define MAX_NUM_BUTTONS  20


static  Button  buttonRecords[ MAX_NUM_BUTTONS ];

static  int     numButtons  =  0;



int buttonInit( int  inBaseSprite,
                int  inHoverSprite,
                int  inPressedSprite,
                int  inCenterX,
                int  inCenterY,
                char inSticky,
                int  inPointerActionHandle,
                int  inActionHandle ) {

    Button  *b;
    int      newHandle;
    
    if( numButtons >= MAX_NUM_BUTTONS ) {
        mingin_log( "Too many buttons registered via buttons.h\n" );
        return -1;
        }

    newHandle = numButtons;
    numButtons ++;

    b  = &( buttonRecords[ newHandle ] );

    b->baseSprite          = inBaseSprite;
    b->hoverSprite         = inHoverSprite;
    b->pressedSprite       = inPressedSprite;
    b->centerX             = inCenterX;
    b->centerY             = inCenterY;
    b->sticky              = inSticky;
    b->pointerActionHandle = inPointerActionHandle;
    b->actionHandle        = inActionHandle;

    b->pressed = 0;
    b->hover = 0;
    b->sawUnpressed = 0;

    REGISTER_VAL_MEM( b->pressed );
    REGISTER_VAL_MEM( b->hover );
    REGISTER_VAL_MEM( b->sawUnpressed );

    return newHandle;
    }



void buttonGetPos( int   inButtonHandle,
                   int  *outX,
                   int  *outY ) {

    Button  *b;
    
    *outX = 0;
    *outY = 0;
    
    if( inButtonHandle == -1 ) {
        return;
        }

    b = &( buttonRecords[ inButtonHandle ] );

    *outX = b->centerX;
    *outY = b->centerY;
    }



void buttonDraw( int  inButtonHandle ) {

    Button  *b;
    int      s   =  -1;

    if( inButtonHandle == -1 ) {
        return;
        }

    b = &( buttonRecords[ inButtonHandle ] );


    if( b->pressed
        &&
        b->pressedSprite != -1 ) {
        
        s = b->pressedSprite;
        }
    else if( b->hover
        &&
        b->hoverSprite != -1 ) {

        s = b->hoverSprite;
        }
    else {
        s = b->baseSprite;
        }
    

    if( s != -1 ) {

        maxigin_drawResetColor();
        
        maxigin_drawSprite( s,
                            b->centerX,
                            b->centerY );

        if( b->actionHandle != -1 ) {
            
            int  spriteW;
            int  spriteH;

            int  dir       =  -1;

            if( b->centerX < MAXIGIN_GAME_NATIVE_W / 3 ) {
                dir = 1;
                }

            maxigin_getSpriteDimensions( s,
                                         &spriteW,
                                         &spriteH );
            
            maxigin_drawButtonHintSprite(
                b->actionHandle,
                b->centerX + dir * ( 4 + spriteW / 2 ),
                b->centerY + spriteH / 2 - 7 );
            }
        }
    }



static char pointerInButton( Button  *b ) {
    int  pointerX;
    int  pointerY;
        
    if( maxigin_getPointerLocation( &pointerX,
                                    &pointerY ) ) {

        int  buttonW  =  0;
        int  buttonH  =  0;

        if( b->baseSprite != -1 ) {
            maxigin_getSpriteDimensions( b->baseSprite,
                                         &buttonW,
                                         &buttonH );
            }
            
        if( pointerX >= b->centerX - buttonW / 2
            &&
            pointerX <= b->centerX + buttonW / 2 - 1
            &&
            pointerY >= b->centerY - buttonH / 2 - 2
            &&
            pointerY <= b->centerY + buttonH / 2 - 2 ) {

            return 1;
            }
        }
    return 0;
    }



char buttonIsHover( int  inButtonHandle ) {
    Button  *b;
    if( inButtonHandle == -1 ) {
        return 0;
        }

    b = &( buttonRecords[ inButtonHandle ] );

    return pointerInButton( b );
    }



char buttonIsNewPressed( int  inButtonHandle ) {
    
    Button  *b;
    char     pressedNow     =  0;
    char     pressedBefore;
    
    if( inButtonHandle == -1 ) {
        return 0;
        }

    b = &( buttonRecords[ inButtonHandle ] );

    pressedBefore = b->pressed;

    if( ! b->sawUnpressed
        &&
        b->actionHandle != -1
        &&
        ! maxigin_isButtonDown( b->actionHandle ) ) {
        
        b->sawUnpressed = 1;
        }
    
    
    if( b->actionHandle != -1
        &&
        b->sawUnpressed
        &&
        maxigin_isButtonDown( b->actionHandle ) ) {

        pressedNow = 1;
        }
    else if( b->pointerActionHandle != -1
             &&
             maxigin_isButtonDown( b->pointerActionHandle ) ) {

        if( pointerInButton( b ) ) {
            
            pressedNow = 1;
            }
        }
    else {
        if( pointerInButton( b ) ) {
            b->hover = 1;
            }
        else {
            b->hover = 0;
            }
        }

    if( pressedNow ) {
        b->pressed = 1;
        }
    else {
        if( ! b->sticky ) {
            b->pressed = 0;
            }
        }

    return b->pressed
           &&
           ! pressedBefore;
    }



void buttonReset( int  inButtonHandle ) {
    
    Button  *b;

    if( inButtonHandle == -1 ) {
        return;
        }

    b = &( buttonRecords[ inButtonHandle ] );

    b->pressed = 0;
    b->sawUnpressed = 0;
    }


char buttonIsStuck( int  inButtonHandle ) {

    Button  *b;

    if( inButtonHandle == -1 ) {
        return 0;
        }

    b = &( buttonRecords[ inButtonHandle ] );
    
    if( b->pressed
        &&
        b->sticky ) {
        return 1;
        }
    return 0;
    }



#endif



#endif
