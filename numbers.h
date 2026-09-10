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

/* on by default */
void numbersToggleBorder( char  inBorderOn );



/* draws a right-aligned number centered on inPosY and butted against inPosX */
void numberDraw( int   inValue,
                 int   inPosX,
                 int   inPosY,
                 char  inFixedWidth );


void numberDrawCenter( int   inValue,
                       int   inPosX,
                       int   inPosY,
                       char  inFixedWidth );

void numberDrawLeft( int   inValue,
                     int   inPosX,
                     int   inPosY,
                     char  inFixedWidth );


void numberDrawText( const char   *inText,
                     int           inPosX,
                     int           inPosY,
                     char          inFixedWidth,
                     MaxiginAlign  inAlign );


#endif



#ifdef  NUMBERS_IMPLEMENTATION

#ifndef NUMBERS_IMPLEMENTATION_INCLUDED
#define NUMBERS_IMPLEMENTATION_INCLUDED




static  int  numbersFont          =  -1;
static  int  numbersFontNoBorder  =  -1;

static  int  numbersCurrentFont   =  -1;


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
                                        0,
                                        8 );
        }

    
    /* repeat for non-border version with no drop shadow */
    fontStrip = maxigin_initSpriteStrip( "modifierFontNoBorder.tga",
                                         8 );

    if( fontStrip != -1 ) {

        maxigin_initMakeGlowSpriteStrip( fontStrip,
                                         2,
                                         2 );

        numbersFontNoBorder = maxigin_initFont( fontStrip,
                                                "modifierFont.txt",
                                                0,
                                                4,
                                                0,
                                                8 );
        }

    numbersCurrentFont = numbersFont;
    }



void numbersToggleBorder( char  inBorderOn ) {
    if( inBorderOn ) {
        numbersCurrentFont = numbersFont;
        }
    else {
        numbersCurrentFont = numbersFontNoBorder;
        }
    }
    


void numberDraw( int   inValue,
                 int   inPosX,
                 int   inPosY,
                 char  inFixedWidth ) {

    numberDrawText( maxigin_intToString( inValue ),
                    inPosX,
                    inPosY,
                    inFixedWidth,
                    MAXIGIN_RIGHT );
    }



void numberDrawCenter( int   inValue,
                        int   inPosX,
                        int   inPosY,
                        char  inFixedWidth ) {

    numberDrawText( maxigin_intToString( inValue ),
                    inPosX,
                    inPosY,
                    inFixedWidth,
                    MAXIGIN_CENTER );
    }


void numberDrawLeft( int   inValue,
                     int   inPosX,
                     int   inPosY,
                     char  inFixedWidth ) {

    numberDrawText( maxigin_intToString( inValue ),
                    inPosX,
                    inPosY,
                    inFixedWidth,
                    MAXIGIN_LEFT );
    }



void numberDrawText( const char   *inText,
                     int           inPosX,
                     int           inPosY,
                     char          inFixedWidth,
                     MaxiginAlign  inAlign ) {
    if( inFixedWidth ) {
        maxigin_adjustFontSpacing( numbersCurrentFont,
                                   0,
                                   4,
                                   7,
                                   8 );
        }
    else {
        maxigin_adjustFontSpacing( numbersCurrentFont,
                                   0,
                                   4,
                                   0,
                                   8 );
        }
        
    maxigin_drawText( numbersCurrentFont,
                      inText,
                      inPosX,
                      inPosY,
                      inAlign );
    }




#endif

#endif
