/*
  Include in your C code wherever like so:

      #include "vfdDigit.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define VFD_DIGIT_IMPLEMENTATION
      #include "vfdDigit.h"

*/

#ifndef VFD_DIGIT_H_INCLUDED
#define VFD_DIGIT_H_INCLUDED


void vfdDigitInit( void );


void vfdDigitDraw( int  inDigit,
                   int  inCenterX,
                   int  inCenterY );


void vfdDigitDrawLetter( char  inLetter,
                         int   inCenterX,
                         int   inCenterY );




#endif



#ifdef  VFD_DIGIT_IMPLEMENTATION

#ifndef VFD_DIGIT_IMPLEMENTATION_INCLUDED
#define VFD_DIGIT_IMPLEMENTATION_INCLUDED


#define  NUM_SEGMENTS  7

static  int  vfdSegmentSprites[ NUM_SEGMENTS ];
static  int  vfdDimSprite;

/* x, then y, from center */
static  int  vfdSegmentOffsets[ NUM_SEGMENTS ][ 2 ] = { {  2, -7 },
                                                        {  6, -3 },
                                                        {  0,  0 },
                                                        { -3, -4 },
                                                        {  4,  4 },
                                                        { -1,  7 },
                                                        { -5,  4 } };


/* maps digits from 0..9 to which segments are on */
static  char  vfdDigitMapping [ 10  ][ NUM_SEGMENTS ] =
                 { { 1, 1, 0, 1, 1, 1, 1 },
                   { 0, 1, 0, 0, 1, 0, 0 },
                   { 1, 1, 1, 0, 0, 1, 1 },
                   { 1, 1, 1, 0, 1, 1, 0 },
                   { 0, 1, 1, 1, 1, 0, 0 },
                   { 1, 0, 1, 1, 1, 1, 0 },
                   { 1, 0, 1, 1, 1, 1, 1 },
                   { 1, 1, 0, 0, 1, 0, 0 },
                   { 1, 1, 1, 1, 1, 1, 1 },
                   { 1, 1, 1, 1, 1, 0, 0 } };

  

/* maps non-extended ASCII characters to which segments are on
   not all characters in ASCII table supported (some may be blank) */
static  char  vfdLetterMapping[ 127 ][ NUM_SEGMENTS ];

#define MAKE_VFD_LETTER_MAPPING( inChar, \
                                 inOn0, inOn1, inOn2, inOn3, \
                                 inOn4, inOn5, inOn6 ) \
           vfdLetterMapping[ inChar ][ 0 ] = inOn0; \
           vfdLetterMapping[ inChar ][ 1 ] = inOn1; \
           vfdLetterMapping[ inChar ][ 2 ] = inOn2; \
           vfdLetterMapping[ inChar ][ 3 ] = inOn3; \
           vfdLetterMapping[ inChar ][ 4 ] = inOn4; \
           vfdLetterMapping[ inChar ][ 5 ] = inOn5; \
           vfdLetterMapping[ inChar ][ 6 ] = inOn6;



void vfdDigitInit( void ) {

    int  i;
    int  c;
    

    vfdDimSprite = maxigin_initSprite( "vfdDim.tga" );
    

    for( i = 0;
         i < NUM_SEGMENTS;
         i   ++ ) {

        const char  *fileName  =
                         maxigin_stringConcat3( "vfdSegment",
                                                maxigin_intToString( i + 1 ),
                                                ".tga" );

        vfdSegmentSprites[ i ] = maxigin_initSprite( fileName );

        maxigin_initMakeGlowSprite( vfdSegmentSprites[ i ],
                                    4,
                                    2 );
        }

    for( c = 0;
         c < 127;
         c   ++ ) {
        
        for( i = 0;
             i < NUM_SEGMENTS;
             i   ++ ) {

            vfdLetterMapping[ c ][ i ] = 0;
            }
        }
    MAKE_VFD_LETTER_MAPPING( ' ', 0, 0, 0, 0, 0, 0, 0 );
    MAKE_VFD_LETTER_MAPPING( 'A', 1, 1, 1, 1, 1, 0, 1 );
    MAKE_VFD_LETTER_MAPPING( 'B', 0, 0, 1, 1, 1, 1, 1 );
    MAKE_VFD_LETTER_MAPPING( 'C', 1, 0, 0, 1, 0, 1, 1 );
    MAKE_VFD_LETTER_MAPPING( 'D', 0, 1, 1, 0, 1, 1, 1 );
    MAKE_VFD_LETTER_MAPPING( 'E', 1, 0, 1, 1, 0, 1, 1 );
    MAKE_VFD_LETTER_MAPPING( 'F', 1, 0, 1, 1, 0, 0, 1 );
    MAKE_VFD_LETTER_MAPPING( 'G', 1, 1, 1, 1, 1, 1, 0 );
    MAKE_VFD_LETTER_MAPPING( 'H', 0, 1, 1, 1, 1, 0, 1 );
    MAKE_VFD_LETTER_MAPPING( 'I', 0, 1, 0, 0, 1, 0, 0 );
    MAKE_VFD_LETTER_MAPPING( 'J', 0, 1, 0, 0, 1, 1, 1 );
    MAKE_VFD_LETTER_MAPPING( 'L', 0, 0, 0, 1, 0, 1, 1 );
    MAKE_VFD_LETTER_MAPPING( 'N', 1, 1, 0, 1, 1, 0, 1 );
    MAKE_VFD_LETTER_MAPPING( 'O', 1, 1, 0, 1, 1, 1, 1 );
    MAKE_VFD_LETTER_MAPPING( 'P', 1, 1, 1, 1, 0, 0, 1 );
    MAKE_VFD_LETTER_MAPPING( 'Q', 1, 1, 1, 1, 1, 0, 0 );
    MAKE_VFD_LETTER_MAPPING( 'R', 0, 0, 1, 0, 0, 0, 1 );
    MAKE_VFD_LETTER_MAPPING( 'S', 1, 0, 1, 1, 1, 1, 0 );
    MAKE_VFD_LETTER_MAPPING( 'U', 0, 1, 0, 1, 1, 1, 1 );
    MAKE_VFD_LETTER_MAPPING( 'Y', 0, 1, 1, 1, 1, 1, 0 );
    MAKE_VFD_LETTER_MAPPING( 'Z', 1, 1, 1, 0, 0, 1, 1 );

    /* lower case letters just copy upper case letters */
    for( c =  'A';
         c <= 'Z';
         c    ++ ) {

        for( i = 0;
             i < NUM_SEGMENTS;
             i   ++ ) {

            vfdLetterMapping[ c + 32 ][ i ] = vfdLetterMapping[ c ][ i ];
            }
        }
        
    }


static void vfdSegmentDraw( int  inSegmentNumber,
                            int  inCenterX,
                            int  inCenterY ) {
    
    maxigin_drawSprite( vfdSegmentSprites            [ inSegmentNumber ],
                        inCenterX + vfdSegmentOffsets[ inSegmentNumber ][ 0 ],
                        inCenterY + vfdSegmentOffsets[ inSegmentNumber ][ 1 ] );
    }



static void vfdDrawFromMapping( int   inCenterX,
                                int   inCenterY,
                                char  inMapping[][ NUM_SEGMENTS ],
                                int   inMapIndex ) {

    int  i;

    maxigin_drawResetColor();

    maxigin_drawSetAlpha( 92 );
    
    maxigin_drawSprite( vfdDimSprite,
                        inCenterX,
                        inCenterY );

    maxigin_drawSetAlpha( 255 );
    
    for( i = 0;
         i < NUM_SEGMENTS;
         i   ++ ) {

        if( inMapping[ inMapIndex ][ i ] ) {

            vfdSegmentDraw( i,
                            inCenterX,
                            inCenterY );
            }
        }
    }



void vfdDigitDraw( int  inDigit,
                   int  inCenterX,
                   int  inCenterY ) {

    vfdDrawFromMapping( inCenterX,
                        inCenterY,
                        vfdDigitMapping,
                        inDigit );
    }



void vfdDigitDrawLetter( char  inLetter,
                         int   inCenterX,
                         int   inCenterY ) {


    vfdDrawFromMapping( inCenterX,
                        inCenterY,
                        vfdLetterMapping,
                        inLetter );
    }





#endif



#endif
