/*
  Include in your C code wherever like so:

      #include "colors.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define COLORS_IMPLEMENTATION
      #include "colors.h"

*/

#ifndef COLORS_H_INCLUDED
#define COLORS_H_INCLUDED


void colorsInit( void );

void colorsSetClassic( void );

void colorsSetRandom( void );


/* inColor pick is 0, 1, or 2 for white, black, and board colors
   or -1 for all colors */
void colorsRotate( int  inColorPick );

void colorsPrint( void );


/* these call maxigin_drawSetColor */
void colorsApplyWhitePieceColor( void );
void colorsApplyBlackPieceColor( void );
void colorsApplyBoardColor     ( void );






#ifdef COLORS_IMPLEMENTATION

#include "maxigin.h"
#include "memoryRegister.h"

#define  PALETTE_SIZE  75


static unsigned char colorsPalette[PALETTE_SIZE][3] = {
    { 6, 39, 94 },
    { 8, 30, 103 },
    { 11, 18, 113 },
    { 24, 14, 122 },
    { 47, 18, 132 },
    { 71, 23, 142 },
    { 98, 28, 151 },
    { 126, 35, 161 },
    { 157, 42, 172 },
    { 184, 52, 178 },
    { 202, 71, 176 },
    { 203, 71, 161 },
    { 203, 72, 147 },
    { 205, 74, 135 },
    { 208, 78, 123 },
    { 212, 84, 114 },
    { 218, 92, 107 },
    { 224, 102, 103 },
    { 231, 127, 115 },
    { 238, 154, 131 },
    { 244, 181, 150 },
    { 249, 206, 172 },
    { 253, 227, 196 },
    { 255, 244, 222 },
    { 255, 254, 250 },

    { 0, 7, 3 },
    { 1, 22, 12 },
    { 2, 36, 26 },
    { 4, 51, 45 },
    { 6, 64, 66 },
    { 9, 67, 80 },
    { 14, 68, 95 },
    { 19, 65, 111 },
    { 26, 61, 127 },
    { 36, 56, 145 },
    { 55, 58, 174 },
    { 66, 55, 174 },
    { 80, 56, 176 },
    { 96, 59, 178 },
    { 114, 63, 183 },
    { 134, 69, 190 },
    { 156, 77, 198 },
    { 180, 89, 208 },
    { 204, 103, 218 },
    { 228, 121, 229 },
    { 238, 142, 228 },
    { 246, 165, 229 },
    { 252, 192, 232 },
    { 254, 220, 239 },
    { 255, 250, 252 },

    { 3, 4, 0 },
    { 7, 12, 0 },
    { 9, 20, 0 },
    { 9, 28, 1 },
    { 6, 35, 1 },
    { 2, 43, 3 },
    { 3, 52, 12 },
    { 4, 60, 23 },
    { 5, 69, 37 },
    { 7, 79, 54 },
    { 11, 94, 79 },
    { 11, 94, 88 },
    { 11, 91, 95 },
    { 11, 83, 96 },
    { 12, 76, 99 },
    { 13, 68, 102 },
    { 15, 61, 107 },
    { 17, 54, 112 },
    { 20, 47, 118 },
    { 23, 40, 123 },
    { 28, 33, 129 },
    { 38, 32, 133 },
    { 54, 37, 136 },
    { 70, 43, 137 },
    { 84, 49, 138 }
        
    };


typedef struct ColorsColor {

        union {
                struct {
                
                        unsigned char  red;
                
                        unsigned char  green;
                
                        unsigned char  blue;
                
                    } comp;

                unsigned char val[ 3 ];
            } c;
        
        int  index;
        
    } ColorsColor;



static  ColorsColor  colorsWhitePlayer;
static  ColorsColor  colorsBlackPlayer;
static  ColorsColor  colorsBoard;

static  MaxiginRand  colorsRand;


void colorsInit( void )  {
    colorsSetClassic();

    maxigin_randSeed( &colorsRand,
                      13873879 );

    REGISTER_VAL_MEM( colorsWhitePlayer );
    REGISTER_VAL_MEM( colorsBlackPlayer );
    REGISTER_VAL_MEM( colorsBoard );
    REGISTER_VAL_MEM( colorsRand );
    }



static void colorsSet( ColorsColor   *inColor,
                       unsigned char  inR,
                       unsigned char  inG,
                       unsigned char  inB ) {

    inColor->c.comp.red   =  inR;
    inColor->c.comp.green =  inG;
    inColor->c.comp.blue  =  inB;
    inColor->index      =  -1;
    }


/* index into palette */
static void colorsSetFromIndex( ColorsColor   *inColor,
                                int            inIndex ) {

    int  i;

    for( i = 0;
         i < 3;
         i ++ ) {
        
        inColor->c.val[i] = colorsPalette[inIndex][i];
        }
    inColor->index = inIndex;
    }



static void colorsSetFromRandomIndex( ColorsColor   *inColor ) {

    colorsSetFromIndex( inColor,
                        maxigin_randRange( &colorsRand,
                                           0,
                                           PALETTE_SIZE - 1 ) );
    }



static int findIndex( ColorsColor  *inColor ) {

    int  i;
    int  j;

    if( inColor->index != -1 ) {
        return inColor->index;
        }
    
    /* if no index marked, search to find match */
    
    for( i = 0;
         i < PALETTE_SIZE;
         i ++ ) {

        char  equal = 1;

        for( j = 0;
             j < 3;
             j ++ ) {
            if( inColor->c.val[j] != colorsPalette[i][j] ) {
                equal = 0;
                break;
                }
            }

        if( equal ) {
            return i;
            }
        }
    
    return -1;
    }



static void logColor( ColorsColor  *inColor ) {

    int  i  =  findIndex( inColor );
    
    mingin_log( "{ " );
    mingin_log( maxigin_intToString( inColor->c.comp.red ) );
    mingin_log( ", " );
    mingin_log( maxigin_intToString( inColor->c.comp.green ) );
    mingin_log( ", " );
    mingin_log( maxigin_intToString( inColor->c.comp.blue ) ); 
    mingin_log( " }" );

    if( i != -1 ) {
        mingin_log( " (index = " );
        mingin_log( maxigin_intToString( i ) );
        mingin_log( ")" );
        }
    }


static void logColors( void ) {

    mingin_log( "Black Player: " );
    logColor( &colorsBlackPlayer );
    mingin_log( "\n" );
    mingin_log( "White Player: " );
    logColor( &colorsWhitePlayer );
    mingin_log( "\n" );
    mingin_log( "Board: " );
    logColor( &colorsBoard );
    mingin_log( "\n\n" );
    }

    

    

void colorsSetClassic( void ) {

    colorsSet( &colorsBlackPlayer,
               128,
               64,
               128 );

    colorsSet( &colorsWhitePlayer,
               192,
               128,
               0 );

    colorsSet( &colorsBoard,
               0,
               255,
               255  );

    logColors();
    }



void colorsSetRandom( void ) {

    colorsSetFromRandomIndex( &colorsBlackPlayer );
    colorsSetFromRandomIndex( &colorsWhitePlayer );
    colorsSetFromRandomIndex( &colorsBoard       );

    logColors();
    }



static void colorsApplyColor( ColorsColor  *inColor ) {
    maxigin_drawSetColor( 255,
                          255,
                          255,
                          255 );
    (void)inColor;
    /*maxigin_drawSetColor( inColor->c.comp.red,
                          inColor->c.comp.green,
                          inColor->c.comp.blue,
                          255 );
    */
    }



void colorsApplyWhitePieceColor( void ) {
    colorsApplyColor( &colorsWhitePlayer );
    }


    
void colorsApplyBlackPieceColor( void ) {
    colorsApplyColor( &colorsBlackPlayer );
    }



void colorsApplyBoardColor( void ) {
    colorsApplyColor( &colorsBoard );
    }


static void colorsRotateColor( ColorsColor  *inColor ) {

    int  newIndex;
    
    if( inColor->index == -1 ) {
        return;
        }

    newIndex = inColor->index;

    newIndex ++;

    if( newIndex >= PALETTE_SIZE ) {
        newIndex = 0;
        }

    colorsSetFromIndex( inColor,
                        newIndex );
    }


void colorsRotate( int  inColorPick ) {

    if( inColorPick == -1 ) {
        colorsRotateColor( &colorsWhitePlayer );
        colorsRotateColor( &colorsBlackPlayer );
        colorsRotateColor( &colorsBoard       );
        }
    else {
        switch( inColorPick ) {
            case 0:
                colorsRotateColor( &colorsWhitePlayer );
                break;
            case 1:
                colorsRotateColor( &colorsBlackPlayer );
                break;
            case 2:
                colorsRotateColor( &colorsBoard       );
                break;
            }
        }
    

    if( 0 ) logColors();
    }



void colorsPrint( void ) {

    logColors();
    }



#endif

#endif
