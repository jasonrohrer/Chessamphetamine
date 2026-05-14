/*
  Include in your C code wherever like so:

      #include "numbers.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define NUMBERS_IMPLEMENTATION
      #include "numbers.h"

*/

#ifndef NUMBERS_H_INCLUDED
#define NUMBERS_H_INCLUDED


void numbersInit( void );


/* draws a right-aligned number centered on inPosY and butted against inPosX */
void numberDraw( int   inValue,
                 int   inPosX,
                 int   inPosY,
                 char  inFixedWidth );

void numberDrawText( const char  *inText,
                     int          inPosX,
                     int          inPosY,
                     char         inFixedWidth );


#ifdef NUMBERS_IMPLEMENTATION


static  int  numbersFont  =  -1;


void numbersInit( void ) {
    
    int fontStrip = maxigin_initSpriteStrip( "modifierFont.tga",
                                             8 );

    if( fontStrip != -1 ) {

        maxigin_initMakeGlowSpriteStrip( fontStrip,
                                         2,
                                         2 );

        /* hazy, faded black shadow  top-to-bottom */
        maxigin_initMakeDropShadowSpriteStrip(
            fontStrip,
            4,
            2,
            192,
            192,
            60,
            30,
            50,
            0 );

        numbersFont = maxigin_initFont( fontStrip,
                                        "modifierFont.txt",
                                        0,
                                        4,
                                        0 );
        }
    }



void numberDraw( int   inValue,
                 int   inPosX,
                 int   inPosY,
                 char  inFixedWidth ) {

    numberDrawText( maxigin_intToString( inValue ),
                    inPosX,
                    inPosY,
                    inFixedWidth );
    }



void numberDrawText( const char  *inText,
                     int          inPosX,
                     int          inPosY,
                     char         inFixedWidth ) {
    if( inFixedWidth ) {
        maxigin_adjustFontSpacing( numbersFont,
                                   0,
                                   4,
                                   7 );
        }
    else {
        maxigin_adjustFontSpacing( numbersFont,
                                   0,
                                   4,
                                   0 );
        }
        
    maxigin_drawText( numbersFont,
                      inText,
                      inPosX,
                      inPosY,
                      MAXIGIN_RIGHT );
    }




#endif

#endif
