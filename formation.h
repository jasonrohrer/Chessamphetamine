/*
  Include in your C code wherever like so:

      #include "formation.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define FORMATION_IMPLEMENTATION
      #include "formation.h"

*/

#ifndef FORMATION_H_INCLUDED
#define FORMATION_H_INCLUDED


void formationInit( void );


void formationPlayerReroll( void );


/* king is always in spot 0 */
char formationSpotGet(  int   inSpotIndex,
                        int  *outX,
                        int  *outY );



void formationDraw( int            inBoardCenterX,
                    int            inBoardCenterY,
                    int            inSpotIndexToShow,
                    unsigned char  inSpotFade );


#endif



#ifdef  FORMATION_IMPLEMENTATION

#ifndef FORMATION_IMPLEMENTATION_INCLUDED
#define FORMATION_IMPLEMENTATION_INCLUDED


#include "chess.h"
#include "moveAnim.h"

#include "memoryRegister.h"

#include "button.h"

#include "unlocks.h"

#include "cost.h"

#include "sideBoard.h"



#define              MAX_FORMATION_SLOTS  BW * 3

static  int          formationX[ MAX_FORMATION_SLOTS ];
static  int          formationY[ MAX_FORMATION_SLOTS ];

static  int          fmSpotSprite  =  -1;

static  MaxiginRand  fmRand;



void formationInit( void ) {

    fmSpotSprite           = maxigin_initSprite( "formationSpot.tga" );

    maxigin_initMakeGlowSprite( fmSpotSprite,
                                4,
                                2 );

    maxigin_randSeed( &fmRand,
                      mingin_getEntropySeed() );
    
    REGISTER_VAL_MEM( fmRand );

    REGISTER_ARRAY_MEM( formationX );
    REGISTER_ARRAY_MEM( formationY );
    }




void formationDraw( int            inBoardCenterX,
                    int            inBoardCenterY,
                    int            inSpotIndexToShow,
                    unsigned char  inSpotFade ) {

    unsigned char  origFade       =  inSpotFade;
    int            cX;
    int            cY;
    int            moneyXOffset   =  13;
    int            offsetVal      =  13;
    int            secondX;
    int            firstNumX;
    int            secondNumX;

    if( inSpotIndexToShow >= MAX_FORMATION_SLOTS ) {
        return;
        }

    if( formationX[ inSpotIndexToShow ] >= BW - 2 ) {
        moneyXOffset = - offsetVal;
        }
                
    boardGetSquareCenter( inBoardCenterX,
                          inBoardCenterY,
                          formationY[ inSpotIndexToShow ],
                          formationX[ inSpotIndexToShow ],
                          &cX,
                          &cY );
    
    maxigin_drawResetColor();

    maxigin_drawSetAlpha( inSpotFade );
                
    maxigin_drawSprite( fmSpotSprite,
                        cX,
                        cY );

    firstNumX = cX + moneyXOffset;
    
    moneyDrawCoin( firstNumX,
                   cY - 13,
                   inSpotFade / 2 );

    
    colorsApplyMoneyColor();      

    maxigin_drawSetAlpha( inSpotFade );

    numberDrawCenter( sideBoardGetPlacementCost(),
                      firstNumX,
                      cY - 13 + 8,
                      1 );
    

    /* draw fading view of future */
    inSpotFade /= 4;
    inSpotIndexToShow ++;

    if( inSpotFade > 0
        &&
        inSpotIndexToShow < MAX_FORMATION_SLOTS ) {

        boardGetSquareCenter( inBoardCenterX,
                              inBoardCenterY,
                              formationY[ inSpotIndexToShow ],
                              formationX[ inSpotIndexToShow ],
                              &cX,
                              &cY );

        maxigin_drawSetAlpha( inSpotFade );
                
        maxigin_drawSprite( fmSpotSprite,
                            cX,
                            cY );

        secondX    = formationX[ inSpotIndexToShow ];

        secondNumX = cX + moneyXOffset;

        if( secondNumX == firstNumX ) {
            moneyXOffset *= -1;
            
            secondNumX = cX + moneyXOffset;
            }
        else if( secondX == BW - 1  ) {
            moneyXOffset = - offsetVal;
            secondNumX = cX + moneyXOffset;
            }
        else if( secondX != 0
                 &&
                 secondX != BW - 1 ) {

            /* try flipping and pushing farther away from firstNumX */
            int  try2 = cX - moneyXOffset;
            int  diff1 = firstNumX - secondNumX;
            int  diff2 = firstNumX - try2;

            diff1 *= diff1;
            diff2 *= diff2;

            if( diff2 > diff1 ) {
                secondNumX = try2;
                }
            }
        

        moneyDrawCoin( secondNumX,
                       cY - 13,
                       inSpotFade / 2 );

    
        colorsApplyMoneyColor();

        maxigin_drawSetAlpha( origFade );

        numberDrawCenter( sideBoardGetNextPlacementCost(),
                          secondNumX,
                          cY - 13 + 8,
                          1 );
        }
    }




typedef struct FormSpot{
        int  x;
        int  y;
    } FormSpot;
        


void formationPlayerReroll( void ) {

    

    static  char      form      [ BH ][ BW ];
    static  FormSpot  extraSpots[ MAX_FORMATION_SLOTS ];
    static  int       extraIndex[ MAX_FORMATION_SLOTS ];
        
    int  y;
    int  x;

    int  i;
    char found;
    int  numExtra  =  0;

    /* use level randomizer to get king and first two piece spots */
    levelGetRandomFormation( form,
                             2,
                             CHESS_WHITE );

    /* find king */
    found = 0;
    
    for( y = BH - 3;
         y < BH;
         y ++ ) {
        for( x = 0;
             x < BW;
             x ++ ) {

            char  f  =  form[ y ][ x ];

            if( f == 2 ) {
                formationX[ 0 ] = x;
                formationY[ 0 ] = y;
                found = 1;
                break;
                }
            }
        if( found ) {
            break;
            }
        }

    
    /* find piece directly in front of king */
    for( y = BH - 3;
         y < formationY[ 0 ];
         y ++ ) {

        char  f  =  form[ y ][ formationX[ 0 ] ];

        if( f == 1 ) {
            formationX[ 1 ] = x;
            formationY[ 1 ] = y;
            break;
            }
        }

    /* find other piece in row in front of king */
    found = 0;
    
    for( y = BH - 3;
         y < formationY[ 0 ];
         y ++ ) {
        for( x = 0;
             x < BW;
             x ++ ) {

            char  f  =  form[ y ][ x ];

            if( x == formationX[ 0 ] ) {
                continue;
                }

            if( f == 1 ) {
                formationX[ 2 ] = x;
                formationY[ 2 ] = y;
                found = 1;
                break;
                }
            }
        if( found ) {
            break;
            }
        }

    /* now find empty spots and shuffle them */
    for( y = BH - 3;
         y < BH;
         y ++ ) {
        for( x = 0;
             x < BW;
             x ++ ) {

            char  alreadyIn  =  0;

            for( i = 0;
                 i < 3;
                 i   ++ ) {

                if( x == formationX[ i ]
                    &&
                    y == formationY[ i ] ) {

                    alreadyIn = 1;
                    break;
                    }
                }
            if( ! alreadyIn ) {

                extraSpots[ numExtra ].x = x;
                extraSpots[ numExtra ].y = y;

                /* build index to shuffle it */
                extraIndex[ numExtra ] = numExtra;
                
                numExtra ++;
                }
            }
        }

    maxigin_shuffle( &fmRand,
                     numExtra,
                     extraIndex );

    for( i = 0;
         i < numExtra;
         i   ++ ) {


        formationX[ i + 3 ] = extraSpots[ extraIndex[ i ] ].x;
        formationY[ i + 3 ] = extraSpots[ extraIndex[ i ] ].y;
        }        
    }



char formationSpotGet(  int   inSpotIndex,
                        int  *outX,
                        int  *outY ) {
    
    if( inSpotIndex >= MAX_FORMATION_SLOTS ) {
        return 0;
        }

    *outX = formationX[ inSpotIndex ];
    *outY = formationY[ inSpotIndex ];

    return 1;
    }



#endif

#endif
