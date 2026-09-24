/*
  Include in your C code wherever like so:

      #include "newRecruits.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define NEW_RECRUITS_IMPLEMENTATION
      #include "newRecruits.h"

*/

#ifndef NEW_RECRUITS_H_INCLUDED
#define NEW_RECRUITS_H_INCLUDED


void newRecruitsInit( int  inPointerActionHandle,
                      int  inActionHandle,
                      int  inCenterX,
                      int  inCenterY );



/* refresh the newRecruits */
void newRecruitsReroll( void );



void newRecruitsDraw( void );


/* returns current mouse-over or controller selected piece

*/
ChessPiece newRecruitsStep( int  inPurchaseSound );


char newRecruitsIsDone( void );




#endif



#ifdef  NEW_RECRUITS_IMPLEMENTATION

#ifndef NEW_RECRUITS_IMPLEMENTATION_INCLUDED
#define NEW_RECRUITS_IMPLEMENTATION_INCLUDED




#include "playerDeck.h"
#include "numbers.h"
#include "pieceSprites.h"

#include "memoryRegister.h"
#include "mingin.h"

#include "button.h"
#include "slotLift.h"
#include "cost.h"

#include "unlocks.h"



#define                NUM_NEW_RECRUITS_BASKETS           4

#define                NUM_NEW_RECRUITS_SLOTS_PER_BASKET  7

static  int            newRecruitsBaseVisibleBaskets   =  3;
static  int            newRecruitsNumVisibleBaskets    =  3;


static  ChessPiece     newRecruitsSlots[ NUM_NEW_RECRUITS_BASKETS          ]
                                       [ NUM_NEW_RECRUITS_SLOTS_PER_BASKET ];


static  int            newRecruitsSelectedBasket       =  -1;
static  int            newRecruitsSelectedSlot         =  -1;


static  unsigned char  newRecruitsSlotHighlightFade
                           [ NUM_NEW_RECRUITS_BASKETS          ]
                           [ NUM_NEW_RECRUITS_SLOTS_PER_BASKET ];

static  unsigned char  newRecruitsBasketHighlightFade
                           [ NUM_NEW_RECRUITS_BASKETS          ];


static  int            newRecruitsSlotPosX
                           [ NUM_NEW_RECRUITS_BASKETS          ]
                           [ NUM_NEW_RECRUITS_SLOTS_PER_BASKET ];
static  int            newRecruitsSlotPosY
                           [ NUM_NEW_RECRUITS_BASKETS          ]
                           [ NUM_NEW_RECRUITS_SLOTS_PER_BASKET ];

static  char           newRecruitsActionDown           =   0;


static  int            lang_newRecruitsInstruct        =  -1;


static  char           newRecruitsDone                 =   0;

static  int            newRecruitsPointerActionHandle  =  -1;
static  int            newRecruitsActionHandle         =  -1;

static  int            newRecruitsCenterX;
static  int            newRecruitsCenterY;

static  int            newRecruitsSlideUp              =   0;
static  int            newRecruitsSlideUpMax           = 100;

static  int            newRecruitsColOffsetX           =   0;

static  char           nrSlotPickedWithController      =   0;



static void newRecruitsResetHightlighFades( void ) {
    int  b;
    int  s;

    for( b = 0;
         b < NUM_NEW_RECRUITS_BASKETS;
         b   ++ ) {
        for( s = 0;
             s < NUM_NEW_RECRUITS_SLOTS_PER_BASKET;
             s   ++ ) {

            newRecruitsSlotHighlightFade[ b ][ s ] = 0;
            }
        newRecruitsBasketHighlightFade[ b ] = 0;
        }
    }



static void newRecruitsClear( void ) {
    int  b;
    int  s;

    for( b = 0;
         b < NUM_NEW_RECRUITS_BASKETS;
         b   ++ ) {
        for( s = 0;
             s < NUM_NEW_RECRUITS_SLOTS_PER_BASKET;
             s   ++ ) {

            newRecruitsSlots[ b ][ s ] = noPiece;
            }
        }
    }


void newRecruitsReroll( void ) {
    
    int  b;
    int  s;

    int  minX  =  newRecruitsSlotPosX[ 0 ][ 0 ];
    int  maxX  =  minX;
    int  aveX  =  minX;
    
    newRecruitsClear();

    newRecruitsNumVisibleBaskets = newRecruitsBaseVisibleBaskets +
                                   unlocksGetExtraNewRecruitsBaskets();

    newRecruitsSlideUp    = newRecruitsSlideUpMax;
    newRecruitsDone       = 0;
    newRecruitsActionDown = 0;
    
    newRecruitsSelectedSlot   = -1;
    newRecruitsSelectedBasket = -1;
    
    nrSlotPickedWithController = 0;
    
    for( b = 0;
         b < newRecruitsNumVisibleBaskets;
         b   ++ ) {
        
        maxX = newRecruitsSlotPosX[ b ][ 0 ];
        
        for( s = 0;
             s < NUM_NEW_RECRUITS_SLOTS_PER_BASKET;
             s   ++ ) {

            newRecruitsSlots[ b ][ s ] = rarityRollPiece();
            }
        }

    aveX = ( minX + maxX ) / 2;

    newRecruitsColOffsetX = - aveX;
    }



void newRecruitsInit( int  inPointerActionHandle,
                      int  inActionHandle,
                      int  inCenterX,
                      int  inCenterY ) {

        
    int  b;
    int  s;
    int  hopSizeX       =  50;
    int  numStartHopsX  =  NUM_NEW_RECRUITS_BASKETS / 2;
    int  hopSizeY       =  BOARD_SQUARE_SIZE;
    int  numStartHopsY  =  NUM_NEW_RECRUITS_SLOTS_PER_BASKET / 2;
    int  startHopX      =  hopSizeX * numStartHopsX;
    int  startHopY      =  hopSizeY * numStartHopsY;
    int  curPosX;
    int  curPosY;

    newRecruitsActionDown = 0;
    newRecruitsSlideUp    = newRecruitsSlideUpMax;
    newRecruitsDone       = 1;

    newRecruitsPointerActionHandle = inPointerActionHandle;
    newRecruitsActionHandle        = inActionHandle;

    newRecruitsCenterX = inCenterX;
    newRecruitsCenterY = inCenterY;

    newRecruitsClear();
    newRecruitsResetHightlighFades();

    lang_newRecruitsInstruct =
        maxigin_initTranslationKey( "newRecruitsInstruct"  );

    
    /* set up slot positions */
 
    
    if( ( NUM_NEW_RECRUITS_BASKETS % 2 ) == 0 ) {
        /* center between two middle slots */
        startHopX -= hopSizeX / 2;
        }

    if( ( NUM_NEW_RECRUITS_SLOTS_PER_BASKET % 2 ) == 0 ) {
        /* center between two middle slots */
        startHopY -= hopSizeY / 2;
        }

    /* shift down slightly */
    startHopY += 5;
    
    curPosX = - startHopX;

    for( b = 0;
         b < NUM_NEW_RECRUITS_BASKETS;
         b   ++ ) {

        curPosY = - startHopY;
        
        for( s = 0;
             s < NUM_NEW_RECRUITS_SLOTS_PER_BASKET;
             s   ++ ) {

            newRecruitsSlotPosX[ b ][ s ] = curPosX;
            newRecruitsSlotPosY[ b ][ s ] = curPosY;
            
            curPosY += hopSizeY;
            }
        curPosX += hopSizeX;
        }

    REGISTER_ARRAY_MEM( newRecruitsSlots );

    REGISTER_ARRAY_MEM( newRecruitsSlotHighlightFade );
    REGISTER_ARRAY_MEM( newRecruitsBasketHighlightFade );

    REGISTER_VAL_MEM( newRecruitsDone );

    REGISTER_VAL_MEM( newRecruitsSelectedBasket );
    REGISTER_VAL_MEM( newRecruitsSelectedSlot );

    REGISTER_VAL_MEM( newRecruitsSlideUp );

    REGISTER_VAL_MEM( newRecruitsColOffsetX );

    REGISTER_VAL_MEM( newRecruitsActionDown );

    REGISTER_VAL_MEM( nrSlotPickedWithController );
    }



static int nrGetLiveCenterY( void ) {
    
    int  liveCenterY =  newRecruitsCenterY;
    
    if( newRecruitsSlideUp > 0 ) {

        int  slideUpY    =  0;
        int  scaleFactor  =  ( newRecruitsSlideUpMax * newRecruitsSlideUpMax )
            / MAXIGIN_GAME_NATIVE_H;

        slideUpY =
            ( newRecruitsSlideUp * newRecruitsSlideUp ) / scaleFactor;

        liveCenterY -= slideUpY;
        }
    return liveCenterY;
    }



void newRecruitsDraw( void ) {

    int  bgW  =  BW * BOARD_SQUARE_SIZE;
    int  bgH  =  BH * BOARD_SQUARE_SIZE;
    int  b;
    int  s;
    int  liveCenterY =  nrGetLiveCenterY();
    int  markerS     =  boardGetMoveMarkerSprite();
    
    maxigin_drawSetColor( 0,
                          0,
                          0,
                          255 );

    maxigin_drawFillRect( newRecruitsCenterX - bgW / 2,
                          liveCenterY        - bgH / 2,
                          newRecruitsCenterX + bgW / 2  - 1,
                          liveCenterY        + bgH / 2  - 1);

    boardDrawBorder( newRecruitsCenterX,
                     liveCenterY );

    maxigin_drawResetColor();

    maxigin_setLanguageFontIndex( 1 );

    maxigin_drawLangText( lang_newRecruitsInstruct,
                          newRecruitsCenterX,
                          liveCenterY + bgH / 2 - 10,
                          MAXIGIN_CENTER );

    maxigin_setLanguageFontIndex( 0 );

   
    for( b = 0;
         b < newRecruitsNumVisibleBaskets;
         b   ++ ) {
        for( s = 0;
             s < NUM_NEW_RECRUITS_SLOTS_PER_BASKET;
             s   ++ ) {

            ChessPiece  p  =  newRecruitsSlots[ b ][ s ];

            if( p != noPiece ) {

                int  x  =  newRecruitsCenterX +
                           newRecruitsColOffsetX +
                           newRecruitsSlotPosX[ b ][ s ];
                
                int  y  =  liveCenterY        + newRecruitsSlotPosY[ b ][ s ];


                if( newRecruitsBasketHighlightFade[ b ] > 0 ) {
                    raritySetDrawColor( p );

                    maxigin_drawSetAlpha( newRecruitsBasketHighlightFade[ b ] );

                    maxigin_drawSprite( markerS,
                                        x,
                                        y );
                    }
                
                drawPiece( p | CHESS_WHITE,
                           x,
                           y );

                if( newRecruitsSlotHighlightFade[ b ][ s ] > 0 ) {
                    drawPieceHighlight( p | CHESS_WHITE,
                                        x,
                                        y,
                                        newRecruitsSlotHighlightFade[ b ][ s ] );
                    }

                if( nrSlotPickedWithController
                    &&
                    b == newRecruitsSelectedBasket
                    &&
                    s == NUM_NEW_RECRUITS_SLOTS_PER_BASKET - 1 ) {

                    maxigin_drawButtonHintSprite(
                        newRecruitsActionHandle,
                        x,
                        y + 20 );
                    }
                }
            }
        }
    }



ChessPiece newRecruitsStep( int  inPurchaseSound ) {

    int         r            =  mingin_getStepsPerSecond();
    int         b;
    int         s;
    int         liveCenterY  =  nrGetLiveCenterY();
    int         pointerX;
    int         pointerY;
    ChessPiece  overPiece    =  noPiece;
    char        spotChange   =  0;
    int         deltaFade    =  ( 20 * 60 ) / r;
    int  markerS             =  boardGetMoveMarkerSprite();
    int  markerW;
    int  markerH;

    maxigin_getSpriteDimensions( markerS,
                                 &markerW,
                                 &markerH );

    if( maxigin_getActivePointerLocation( &pointerX,
                                          &pointerY ) ) {
        
        nrSlotPickedWithController = 0;
        
        for( b = 0;
             b < newRecruitsNumVisibleBaskets;
             b   ++ ) {
            for( s =  NUM_NEW_RECRUITS_SLOTS_PER_BASKET - 1;
                 s >= 0;
                 s    -- ) {

                ChessPiece  p  =  newRecruitsSlots[ b ][ s ];

                if( p != noPiece ) {

                    int  x  =  newRecruitsCenterX +
                        newRecruitsColOffsetX +
                        newRecruitsSlotPosX[ b ][ s ];
                
                    int  y  =  liveCenterY + newRecruitsSlotPosY[ b ][ s ];

                    char inSquare  =  0;

                    if( ( pointerX - x ) < markerW / 2
                        &&
                        ( pointerX - x ) > - markerW / 2
                        &&
                        ( pointerY - y ) < markerH / 2
                        &&
                        ( pointerY - y ) > - markerH / 2 ) {

                        inSquare = 1;
                        }
                    

                    if( inSquare
                        ||
                        getPixelOverPiece( p | CHESS_WHITE,
                                           x,
                                           y,
                                           pointerX,
                                           pointerY ) ) {

                        newRecruitsSelectedBasket = b;
                        newRecruitsSelectedSlot   = s;

                        if( newRecruitsSlotHighlightFade[ b ][ s ] != 255 ) {
                            spotChange = 1;
                            }
                        newRecruitsSlotHighlightFade  [ b ][ s ] = 255;
                        newRecruitsBasketHighlightFade[ b ]      = 255;
                        
                        overPiece = p;
                        break;
                        }
                    }
                }
            if( overPiece != noPiece ) {
                break;
                }
            }
        
        if( overPiece == noPiece ) {
            newRecruitsSelectedBasket = -1;
            newRecruitsSelectedSlot   = -1;
            }
        }
    else {
        int  dirX;
        int  dirY;

        navGetDir( 0,
                   &dirX,
                   &dirY );

        
        if( dirX != 0
            ||
            dirY != 0 ) {
            
            nrSlotPickedWithController = 1;

            if( newRecruitsSelectedSlot != -1
                &&
                newRecruitsSelectedBasket != -1 ) {

                if( dirX != 0 ) {
                    newRecruitsSelectedBasket += dirX;

                    if( newRecruitsSelectedBasket
                        >=
                        newRecruitsNumVisibleBaskets ) {
                        newRecruitsSelectedBasket = 0;
                        }
                    else if( newRecruitsSelectedBasket < 0 ) {
                        newRecruitsSelectedBasket =
                            newRecruitsNumVisibleBaskets - 1;
                        }
                    }
                if( dirY != 0 ) {
                    newRecruitsSelectedSlot += dirY;

                    if( newRecruitsSelectedSlot
                        >=
                        NUM_NEW_RECRUITS_SLOTS_PER_BASKET ) {
                        newRecruitsSelectedSlot = 0;
                        }
                    else if( newRecruitsSelectedSlot < 0 ) {
                        newRecruitsSelectedSlot =
                            NUM_NEW_RECRUITS_SLOTS_PER_BASKET - 1;
                        }
                    }

                }
            else {
                /* nothing picked yet */

                newRecruitsSelectedSlot = 0;
                
                if( dirX == -1 ) {
                    /* enter from right */
                    newRecruitsSelectedBasket = newRecruitsNumVisibleBaskets - 1;
                    }
                else {
                    /* left */
                    newRecruitsSelectedBasket = 0;
                    }
                }
            }

        if( newRecruitsSelectedSlot != -1
            &&
            newRecruitsSelectedBasket != -1 ) {
            
            overPiece = newRecruitsSlots  [ newRecruitsSelectedBasket ]
                                          [ newRecruitsSelectedSlot ];

            if( newRecruitsSlotHighlightFade[ newRecruitsSelectedBasket]
                                            [ newRecruitsSelectedSlot  ]
                                            != 255 ) {
                spotChange = 1;
                }
            
            newRecruitsSlotHighlightFade  [ newRecruitsSelectedBasket ]
                                          [ newRecruitsSelectedSlot   ] = 255;
            
            newRecruitsBasketHighlightFade[ newRecruitsSelectedBasket ] = 255;

            }
        }

            


    for( b = 0;
         b < newRecruitsNumVisibleBaskets;
         b   ++ ) {

        if( b != newRecruitsSelectedBasket
            &&
            newRecruitsBasketHighlightFade[ b ] > 0 ) {
            
            int  newHighlight =
                newRecruitsBasketHighlightFade[ b ] - deltaFade;

            if( newHighlight > 0 ) {
                newRecruitsBasketHighlightFade[ b ]
                    = (unsigned char)newHighlight;
                }
            else {
                newRecruitsBasketHighlightFade[ b ] = 0;
                }
            }
        
        for( s = 0;
             s < NUM_NEW_RECRUITS_SLOTS_PER_BASKET;
             s   ++ ) {

            if( ( b != newRecruitsSelectedBasket
                  ||
                  s != newRecruitsSelectedSlot )
                &&
                newRecruitsSlotHighlightFade[ b ][ s ] > 0 ) {

                int  newHighlight =
                    newRecruitsSlotHighlightFade[ b ][ s ] - deltaFade;

                if( newHighlight > 0 ) {
                    newRecruitsSlotHighlightFade[ b ][ s ]
                        = (unsigned char)newHighlight;
                    }
                else {
                    newRecruitsSlotHighlightFade[ b ][ s ] = 0;
                    }
                }
            }
        }

    if( overPiece != noPiece
        &&
        newRecruitsSelectedBasket != -1
        &&
        ! newRecruitsActionDown
        &&
        ( maxigin_isButtonDown( newRecruitsPointerActionHandle )
          ||
          maxigin_isButtonDown( newRecruitsActionHandle ) ) ) {


        b = newRecruitsSelectedBasket;
        
        for( s = 0;
             s < NUM_NEW_RECRUITS_SLOTS_PER_BASKET;
             s   ++ ) {
        
                playerDeckAddPiece( newRecruitsSlots[ b ][ s ] );
                newRecruitsSlots[ b ][ s ] = noPiece;
            }

        maxigin_playSoundEffect( inPurchaseSound,
                                 256 );
        playerDeckReshuffle();

        newRecruitsActionDown = 1;

        newRecruitsDone = 1;
        }
    

    if( ! newRecruitsDone
        &&
        newRecruitsSlideUp > 0 ) {
        
        newRecruitsSlideUp -= ( 4 * 60 ) / r;

        if( newRecruitsSlideUp <= 0 ) {
            newRecruitsSlideUp = 0;
            }
        }

    if( newRecruitsDone
        &&
        newRecruitsSlideUp < newRecruitsSlideUpMax ) {
        
        newRecruitsSlideUp += ( 4 * 60 ) / r;

        if( newRecruitsSlideUp >= newRecruitsSlideUpMax ) {
            newRecruitsSlideUp = newRecruitsSlideUpMax;
            }
        }
    

    if( spotChange ) {
        /* tempoararily return noPiece so change sound plays */
        return noPiece;
        }
    else {
        return overPiece;
        }
    }



char newRecruitsIsDone( void ) {

    if( ! newRecruitsDone ) {
        return 0;
        }
    else if( newRecruitsSlideUp < newRecruitsSlideUpMax ) {
        return 0;
        }
    else {
        return 1;
        }
    }



#endif

#endif
