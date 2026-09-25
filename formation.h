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


void formationInit( int  inPointerActionHandle,
                    int  inDynamicDoneButtonHandle );


void formationSetEnemyLocations( BoardState  *inState );


void formationPlayerReroll( void );


void formationDraw( int   inBoardCenterX,
                    int   inBoardCenterY,
                    char  inSlidingUp );


/* returns 1 if done adjusting formation */
char formationStep( int  inBoardCenterX,
                    int  inBoardCenterY,
                    int  inPieceLiftSound );


/* returns 0 for empty, 1 for regular piece, 2 for king */
char formationGet( int  inRow,
                   int  inCol );

void formationAddNewSpot( void );


int formationGetNumNonKingSpots( void );


/* back to starting formation */
void formationBackToStart( void );


char formationHasRoomForNewSpot( void );


int formationGetSpotSprite( void );

int formationGetNewSpotLangHandle( void );



#endif



#ifdef  FORMATION_IMPLEMENTATION

#ifndef FORMATION_IMPLEMENTATION_INCLUDED
#define FORMATION_IMPLEMENTATION_INCLUDED


#include "chess.h"
#include "moveAnim.h"

#include "memoryRegister.h"

#include "button.h"

#include "unlocks.h"



/* 0 empty, 1 regular piece, 2 king, -1 enemy regular piece, -2 enemy king */
static  char           formation             [BH][BW];
static  unsigned char  formationHighlightFade[BH][BW];

/* at most 16 pieces in back 3 rows (24 spots total) */
static  int            fmMaxFilledPercent      =  67;


static  int            fmSpotSprite            =  -1;
static  int            fmSpotPickedSprite      =  -1;
static  int            fmSpotKingSprite        =  -1;
static  int            fmSpotKingPickedSprite  =  -1;

static  int            fmDoneButton            =  -1;

static  char           fmNewSpotMessageShowing =   0;
static  int            formationSize           =   0;

static  int            lang_newSpot;

static  int            lang_kingSpot;
static  int            lang_otherSpot;
static  int            lang_kingSpotDesc;
static  int            lang_otherSpotDesc;

static  int            lang_kingSpotEnemy;
static  int            lang_otherSpotEnemy;
static  int            lang_kingSpotEnemyDesc;
static  int            lang_otherSpotEnemyDesc;

static  int            fmOverSlotX             =  -1;
static  int            fmOverSlotY             =  -1;


void formationInit( int  inPointerActionHandle,
                    int  inDynamicDoneButtonHandle ) {

    fmSpotSprite           = maxigin_initSprite( "formationSpot.tga"           );
    fmSpotPickedSprite     = maxigin_initSprite( "formationSpotPicked.tga"     );
    fmSpotKingSprite       = maxigin_initSprite( "formationKingSpot.tga"       );
    fmSpotKingPickedSprite = maxigin_initSprite( "formationKingSpotPicked.tga" );

    maxigin_initMakeGlowSprite( fmSpotSprite,
                                4,
                                2 );
    maxigin_initMakeGlowSprite( fmSpotPickedSprite,
                                4,
                                2 );
    maxigin_initMakeGlowSprite( fmSpotKingSprite,
                                4,
                                2 );
    maxigin_initMakeGlowSprite( fmSpotKingPickedSprite,
                                4,
                                2 );

    formationBackToStart();


    fmDoneButton = buttonInit( maxigin_initSprite( "doneButton.tga" ),
                               -1,
                               maxigin_initSprite( "doneButtonPressed.tga" ),
                               MAXIGIN_GAME_NATIVE_W - 100,
                               MAXIGIN_GAME_NATIVE_H - 10,
                               0,
                               inPointerActionHandle,
                               inDynamicDoneButtonHandle,
                               1 );

    lang_newSpot   = maxigin_initTranslationKey( "newFormationSpot" );
    
    lang_kingSpot  = maxigin_initTranslationKey( "formationKing" );
    lang_otherSpot = maxigin_initTranslationKey( "formationOther" );
    
    lang_kingSpotDesc  = maxigin_initTranslationKey( "formationKingDesc" );
    lang_otherSpotDesc = maxigin_initTranslationKey( "formationOtherDesc" );

    
    lang_kingSpotEnemy  = maxigin_initTranslationKey( "formationKingEnemy" );
    lang_otherSpotEnemy = maxigin_initTranslationKey( "formationOtherEnemy" );
    
    lang_kingSpotEnemyDesc  =
        maxigin_initTranslationKey( "formationKingEnemyDesc" );
    lang_otherSpotEnemyDesc =
        maxigin_initTranslationKey( "formationOtherEnemyDesc" );
    

    REGISTER_ARRAY_MEM( formation              );
    REGISTER_ARRAY_MEM( formationHighlightFade );
    
    REGISTER_VAL_MEM  ( fmOverSlotX            );
    REGISTER_VAL_MEM  ( fmOverSlotY            );

    REGISTER_VAL_MEM  ( formationSize          );

    REGISTER_VAL_MEM  ( fmNewSpotMessageShowing );
    }



void formationDraw( int   inBoardCenterX,
                    int   inBoardCenterY,
                    char  inSlidingUp ) {

    int            y;
    int            x;
    int            descX;
    int            descY;
    unsigned char  descFade;
    

    /* black box behind to cover deck when board slides up */
    maxigin_drawSetColor( 0,
                          0,
                          0,
                          255 );
    maxigin_drawFillRect( inBoardCenterX - ( BW * BOARD_SQUARE_SIZE ) / 2,
                          inBoardCenterY - ( BH * BOARD_SQUARE_SIZE ) / 2,
                          inBoardCenterX + ( BW * BOARD_SQUARE_SIZE ) / 2,
                          inBoardCenterY + ( BH * BOARD_SQUARE_SIZE ) / 2 );
                          
    /* last 3 rows on white's side */
    maxigin_drawResetColor();
    

    /* darker portion under enemy formation */
    boardDrawPortion( inBoardCenterX,
                      inBoardCenterY,
                      0,
                      4,
                      128,
                      0);


    /* full brightness portion under player formation, with border */
    boardDrawPortion( inBoardCenterX,
                      inBoardCenterY,
                      5,
                      BH - 1,
                      255,
                      1 );

    boardDrawHorizontalLine( inBoardCenterX,
                             inBoardCenterY,
                             4 );
    

    for( y = 0;
         y < BH;
         y ++ ) {
        for( x = 0;
             x < BW;
             x ++ ) {

            int  f  =  formation[ y ][ x ];
            

            if( f != 0 ) {

                int  cX;
                int  cY;
                int  s    =  fmSpotSprite;
                
                boardGetSquareCenter( inBoardCenterX,
                                      inBoardCenterY,
                                      y,
                                      x,
                                      &cX,
                                      &cY );

                if( f == 1
                    ||
                    f == -1 ) {
                    s = fmSpotSprite;
                    }
                if( f == 2
                    ||
                    f == -2 ) {
                    s = fmSpotKingSprite;
                    }

                maxigin_drawResetColor();

                if( f < 0 ) {
                    drawSetPieceColor( CHESS_BLACK );
                    }
                
                maxigin_drawSprite( s,
                                    cX,
                                    cY );

                if( formationHighlightFade[ y ][ x ] > 0 ) {


                    maxigin_drawSetAlpha( formationHighlightFade[ y ][ x ] );
                    
                    maxigin_drawSpriteGlowOnly( s,
                                                cX,
                                                cY );

                    if( f < 0 ) {
                        /* draw a second time to make highlight brighter */
                        maxigin_drawSprite( s,
                                    cX,
                                    cY-1 );
                        if(0)maxigin_drawSpriteGlowOnly( s,
                                                    cX,
                                                    cY );
                        }
                    }
                }
            }
        }
    


    descX    = fmOverSlotX;
    descY    = fmOverSlotY;
    descFade = formationHighlightFade[ descY ][ descX ];
        
    

    if( descX != -1
        &&
        descY != -1
        &&
        ! unlocksIsViewerActive()
        &&
        formation[ descY ][ descX ] != 0 ) {

        int            f         =  formation[ descY ][ descX ];
        int            titleKey  =  lang_otherSpot;
        int            descKey   =  lang_otherSpotDesc;
        int            centX     =  MAXIGIN_GAME_NATIVE_W - 41;
        int            centY     =  MAXIGIN_GAME_NATIVE_H / 2;
        int            rarity    =  common;

        if( f == 2 ) {
            titleKey = lang_kingSpot;
            descKey  = lang_kingSpotDesc;
            rarity   = contraband;
            }
        else if( f == -2 ) {
            titleKey = lang_kingSpotEnemy;
            descKey  = lang_kingSpotEnemyDesc;
            rarity   = contraband;
            }
        else if( f == -1 ) {
            titleKey = lang_otherSpotEnemy;
            descKey  = lang_otherSpotEnemyDesc;
            rarity   = common;
            }
            
        

        drawDescriptionText( titleKey,
                             descKey,
                             centX,
                             centY,
                             descFade );

        raritySetDrawColorFromRarity( rarity );

        maxigin_drawSetAlpha( descFade  );
            
        drawDescriptionFrame( centX,
                              centY );
        }



    if( fmNewSpotMessageShowing ) {
        maxigin_drawResetColor();

        maxigin_setLanguageFontIndex( 1 );
        
        maxigin_drawLangText(
                lang_newSpot,
                inBoardCenterX + BOARD_SQUARE_SIZE / 2,
                inBoardCenterY - 10,
                MAXIGIN_CENTER );
        
        maxigin_setLanguageFontIndex( 0 );
        }
    
    
    if( ! inSlidingUp ) {
        /* hide done button if sliding up  */
        
        buttonDraw( fmDoneButton );
        }
    }



char formationStep( int  inBoardCenterX,
                    int  inBoardCenterY,
                    int  inPieceLiftSound ) {
    

    int  pointerX;
    int  pointerY;
    int  y;
    int  x;
    int  r              =  mingin_getStepsPerSecond();
    int  deltaFade      =  ( 20 * 60 ) / r;


    int  squareR        =  BOARD_SQUARE_SIZE / 2;
    
    if( maxigin_getActivePointerLocation( &pointerX,
                                          &pointerY ) ) {
        
        fmOverSlotX = -1;
        fmOverSlotY = -1;

        for( y = 0;
             y < BH;
             y ++ ) {
            for( x = 0;
                 x < BW;
                 x ++ ) {

                int  cX;
                int  cY;
                
                boardGetSquareCenter( inBoardCenterX,
                                      inBoardCenterY,
                                      y,
                                      x,
                                      &cX,
                                      &cY );

                if( pointerX > cX - squareR
                    &&
                    pointerX < cX + squareR
                    &&
                    pointerY > cY - squareR
                    &&
                    pointerY < cY + squareR ) {

                    if( formation[ y ][ x ] != 0
                        &&
                        formationHighlightFade[ y ][ x ] < 255 ) {
                        
                        maxigin_playSoundEffect( inPieceLiftSound,
                                                 256 );
                        }
                    formationHighlightFade[ y ][ x ] = 255;

                    fmOverSlotX = x;
                    fmOverSlotY = y;
                    }
                }
            }
        }
    else {
        /* pointer not available, consider controller */
        
        static  char  presentMap[ BH * BW ];

        int  oldX  =  fmOverSlotX;
        int  oldY  =  fmOverSlotY;

        /* end any pointer drag, in case pointer left screen */
    
        for( y = 0;
             y < BH;
             y ++ ) {
            for( x = 0;
                 x < BW;
                 x ++ ) {

                if( formation[ y ][ x ] == 0 ) {
                    presentMap[ y * BW + x ] = 0;
                    }
                else {
                    presentMap[ y * BW + x ] = 1;
                    }
                }
            }
        sparseGridNav( presentMap,
                       BW,
                       BH,
                       &fmOverSlotX,
                       &fmOverSlotY);

        if( fmOverSlotX != -1
            &&
            fmOverSlotY != -1
            &&
            ( fmOverSlotX != oldX
              ||
              fmOverSlotY != oldY ) ) {

            maxigin_playSoundEffect( inPieceLiftSound,
                                     256 );
            formationHighlightFade[ fmOverSlotY ][ fmOverSlotX ] = 255;

            unlocksCancelViewer();
            }
      
        }
    
    
    for( y = 0;
         y < BH;
         y ++ ) {
        for( x = 0;
             x < BW;
             x ++ ) {


            if( ( y != fmOverSlotY
                  ||
                  x != fmOverSlotX )
                &&
                formationHighlightFade[ y ][ x ] > 0 ) {

                int  newHighlight =
                    formationHighlightFade[ y ][ x ] - deltaFade;

                if( newHighlight > 0 ) {
                    formationHighlightFade[ y ][ x ] =
                        (unsigned char)newHighlight;
                    }
                else {
                    formationHighlightFade[ y ][ x ] = 0;
                    }
                }
            }

        }


    /* no done button pressable if they are still picking a slot
       and haven't set it down yet */
    if( buttonIsNewPressed( fmDoneButton ) ) {

        fmOverSlotX = -1;
        fmOverSlotY = -1;

        unlocksCancelViewer();

        fmNewSpotMessageShowing = 0;
        return 1;
        }
    
    return 0;
    }



char formationGet( int  inRow,
                   int  inCol ) {
    return formation[ inRow ][ inCol ];
    }


void formationBackToStart( void ) {
    
    int  y;
    int  x;
    
    for( y = 0;
         y < BH;
         y ++ ) {
        for( x = 0;
             x < BW;
             x ++ ) {

            formation             [ y ][ x ] = 0;
            formationHighlightFade[ y ][ x ] = 0;
            }
        }

    fmOverSlotX = -1;
    fmOverSlotY = -1;

    /* default starting formation
       k in back, 2 pieces in front */
    
    /* king */
    formation[ 6 ][ 4 ] = 2;
    
    formation[ 6 ][ 1 ] = 1;
    formation[ 6 ][ 6 ] = 1;

    formationSize = 3;
    }


void formationAddNewSpot( void ) {

    fmNewSpotMessageShowing = 1;

    formationSize ++;
    }



void formationSetEnemyLocations( BoardState  *inState ) {
    int  y;
    int  x;
    
    /* clear old */
    for( y = 0;
         y < BH;
         y ++ ) {
        for( x = 0;
             x < BW;
             x ++ ) {

            if( formation[ y ][ x ] < 0 ) {
                
                formation[ y ][ x ] = 0;
                }
            }
        }

    /* set new */
    for( y = 0;
         y < BH;
         y ++ ) {
        for( x = 0;
             x < BW;
             x ++ ) {

            ChessPiece  p  =  inState->grid[ y ][ x ];
            

            if( p != noPiece
                &&
                ( p & CHESS_COLOR_MASK ) == CHESS_BLACK ) {

                int  t  =  p & CHESS_TYPE_MASK;

                if( t == king ) {
                    /* for now, keep showing enemy king spot */
                    formation[ y ][ x ] = -2;
                    }
                else {
                    formation[ y ][ x ] = -1;
                    }
                }
            }
        }
    }



void formationPlayerReroll( void ) {

    static  char  form[ BH ][ BW ];

    int  y;
    int  x;

    levelGetRandomFormation( form,
                             formationSize - 1,
                             CHESS_WHITE );

    for( y = BH - 4;
         y < BH;
         y ++ ) {
        for( x = 0;
             x < BW;
             x ++ ) {

            char  f  =  form[ y ][ x ];

            formation[ y ][ x ] = f;
            }
        }
    }



char formationHasRoomForNewSpot( void ) {

    /* can fill first 3 rows, and be specified percentage full */
    if( formationSize < ( BW * 3 * fmMaxFilledPercent ) / 100 ) {
        return 1;
        }
    else {
        return 0;
        }
    }



int formationGetSpotSprite( void ) {
    return fmSpotSprite;
    }


int formationGetNewSpotLangHandle( void ) {
    return lang_newSpot;
    }


int formationGetNumNonKingSpots( void ) {
    return formationSize - 1;
    }



#endif

#endif
