/*
  Include in your C code wherever like so:

      #include "deckView.h"

  Include exactly once, in one .c file, like so, to compile in the
  implementation:

      #define DECK_VIEW_IMPLEMENTATION
      #include "deckView.h"

*/

#ifndef DECK_VIEW_H_INCLUDED
#define DECK_VIEW_H_INCLUDED

#include "deck.h"


void deckViewInit(  int  inCenterX,
                    int  inCenterY,
                    int  inPointerActionHandle,
                    int  inNextButtonActionHandle,
                    int  inPrevButtonActionHandle );


void deckViewSet( void );


void deckViewDraw( void );


/* returns current moused-over piece for sidebar info panel */
ChessPiece deckViewStep( int  inPageSound );



#endif



#ifdef  DECK_VIEW_IMPLEMENTATION

#ifndef DECK_VIEW_IMPLEMENTATION_INCLUDED
#define DECK_VIEW_IMPLEMENTATION_INCLUDED




#include "memoryRegister.h"
#include "unlocks.h"


static  int  deckViewCenterX;
static  int  deckViewCenterY;


typedef struct DeckViewSlot {
        
        ChessPiece  piece;

        char        present;

        char        played;
        
    } DeckViewSlot;



static  DeckViewSlot   deckViewSlots[ MAX_DECK_SIZE ];

static  int            deckViewNumFullSlots              =  0;

#define  DECK_VIEW_ROWS       4
#define  DECK_VIEW_COLS       5
#define  DECK_VIEW_VIS_SLOTS  ( DECK_VIEW_ROWS * DECK_VIEW_COLS )


static  int            deckViewSlotPosX     [ DECK_VIEW_VIS_SLOTS ];
static  int            deckViewSlotPosY     [ DECK_VIEW_VIS_SLOTS ];
static  unsigned char  deckViewHighlightFade[ DECK_VIEW_VIS_SLOTS ];

static  int            deckViewCellSizeX                =  BOARD_SQUARE_SIZE;
static  int            deckViewCellSizeY                =  2 * BOARD_SQUARE_SIZE;
static  int            deckViewPageNumber               =  0;

static  int            nextButton                       =  -1;
static  int            prevButton                       =  -1;

static  int            deckViewOverSlot                 =  -1;

static  int            deckViewPlayedCheckSprite        =  -1;

static  int            lang_exhausted;
static  int            lang_exhaustDesc;

static  char           deckExhaustDescShowing           =   0;
static  char           deckExhaustedShowing             =   0;
static  unsigned char  deckExhaustedFade                =   0;


static void deckViewClear( void ) {
    deckViewNumFullSlots = 0;
    deckViewPageNumber   = 0;
    }



void deckViewInit(  int  inCenterX,
                    int  inCenterY,
                    int  inPointerActionHandle,
                    int  inNextButtonActionHandle,
                    int  inPrevButtonActionHandle ) {

    int  x;
    int  y;
    int  i      =  0;
    int  yPos;
    
    deckViewCenterX = inCenterX;
    deckViewCenterY = inCenterY;

    deckViewClear();

    yPos  =
        deckViewCenterY
        - ( ( DECK_VIEW_ROWS -  1 ) * deckViewCellSizeY ) / 2;

    yPos += 2;
    

    for( y = 0;
         y < DECK_VIEW_ROWS;
         y ++ ) {

        /* restart xPos for each row */
        int  xPos  =
            deckViewCenterX
            - ( ( DECK_VIEW_COLS -  1 ) * deckViewCellSizeX ) / 2;

        for( x = 0;
             x < DECK_VIEW_COLS;
             x ++ ) {

            deckViewSlotPosX[i] = xPos;
            deckViewSlotPosY[i] = yPos;
            
            i ++;
            xPos += deckViewCellSizeX;
            }
        
        yPos += deckViewCellSizeY;
        }

    for( i = 0;
         i < DECK_VIEW_VIS_SLOTS;
         i ++ ) {
        deckViewHighlightFade[i] = 0;
        }

    nextButton = buttonInit( maxigin_initSprite( "nextButton.tga" ),
                             -1,
                             maxigin_initSprite( "nextButtonPressed.tga" ),
                             inCenterX
                             + ( DECK_VIEW_COLS - 1 ) * deckViewCellSizeX / 2
                             + 31,
                             inCenterY - 10,
                             0,
                             inPointerActionHandle,
                             inNextButtonActionHandle );
    
    prevButton = buttonInit( maxigin_initSprite( "prevButton.tga" ),
                             -1,
                             maxigin_initSprite( "prevButtonPressed.tga" ),
                             inCenterX
                             - ( DECK_VIEW_COLS - 1 ) * deckViewCellSizeX / 2
                             - 31,
                             inCenterY - 10,
                             0,
                             inPointerActionHandle,
                             inPrevButtonActionHandle );


    deckViewPlayedCheckSprite = maxigin_initSprite( "playedCheck.tga" );
    
    maxigin_initMakeGlowSprite( deckViewPlayedCheckSprite,
                                4,
                                2 );

    maxigin_initMakeDropShadowSprite( deckViewPlayedCheckSprite,
                                      5,
                                      2,
                                      255,
                                      255,
                                      0,
                                      100,
                                      100,
                                      0 );

    lang_exhausted   = maxigin_initTranslationKey( "exhausted"   );
    lang_exhaustDesc = maxigin_initTranslationKey( "exhaustDesc" );

    
    REGISTER_ARRAY_MEM( deckViewSlots         );
    REGISTER_ARRAY_MEM( deckViewHighlightFade );

    REGISTER_VAL_MEM( deckViewNumFullSlots    );
    REGISTER_VAL_MEM( deckViewPageNumber      );

    REGISTER_VAL_MEM( deckViewOverSlot        );
    
    REGISTER_VAL_MEM( deckExhaustDescShowing  );
    REGISTER_VAL_MEM( deckExhaustedShowing    );
    REGISTER_VAL_MEM( deckExhaustedFade       );
    }



/* show deck in order for debugging */
static void deckViewSetDebug( void ) {

    int    p;
    Deck  *deck  =  playerDeckGetDrawDeck();
    char  *map   =  playerDeckGetPiecePlayedMap();
    for( p = 0;
         p < deck->numPieces;
         p ++ ) {

        deckViewSlots[p].piece   = deck->pieces [p];
        deckViewSlots[p].present = deck->present[p];
        deckViewSlots[p].played  = map          [p];
        }

    deckViewNumFullSlots = deck->numPieces;
    }
    


void deckViewSet( void ) {

    int    i;
    int    n     =  0;
    Deck  *deck  =  playerDeckGetDrawDeck();
    
    if( 0 ) {
        deckViewSetDebug();
        return;
        }

    deckViewOverSlot     = -1;
    deckViewPageNumber   =  0;
    deckViewNumFullSlots =  deck->numPieces;
    

    /* show in order
       so we don't give away shuffled order */
    for( i = NUM_CHESS_PIECES - 1;
         i >= FIRST_CHESS_PIECE;
         i -- ) {
        
        signed char  present;

        /* show present pieces of this type first,
           followed by non-present ones,
           followed by played ones */
        for( present = 1;
             present >= -1;
             present -- ) {

            int    p;
            char  *map  =  playerDeckGetPiecePlayedMap();
            
            for( p = 0;
                 p < deck->numPieces;
                 p ++ ) {

                char  piecePresence  =  deck->present[p];
                
                if( piecePresence
                    &&
                    deck->drawPos < p ) {
                    /* pieces present in discard pile are still
                       marked as dark in deck view */
                    piecePresence = 0;
                    }

                if( piecePresence == present
                    &&
                    ! map[ p ] ) {
                    
                    ChessPiece  thisPiece  =  deck->pieces[p];

                    if( thisPiece == i ) {
                        deckViewSlots[n].piece   = thisPiece;
                        deckViewSlots[n].present = present;
                        deckViewSlots[n].played  = 0;
                        
                        n++;
                        }
                    }
                else if( present == -1
                         &&
                         map[ p ] ) {
                    
                    ChessPiece  thisPiece  =  deck->pieces[p];

                    if( thisPiece == i ) {
                        deckViewSlots[n].piece   = thisPiece;
                        deckViewSlots[n].present = present;
                        deckViewSlots[n].played  = 1;
                        
                        n++;
                        }
                    }
                }
            }
        }

    deckExhaustedFade      = 0;
    deckExhaustedShowing   = 0;
    deckExhaustDescShowing = 0;
    }



void deckViewDraw( void ) {

    int  skip  =  deckViewPageNumber * DECK_VIEW_VIS_SLOTS;

    int  i     =  skip;

    int  y;
    int  x;

    if( deckViewPageNumber > 0 ) {
        buttonDraw( prevButton );
        }
    if( skip + DECK_VIEW_VIS_SLOTS < deckViewNumFullSlots ) {
        buttonDraw( nextButton );
        }
    

    for( y = 0;
         y < DECK_VIEW_ROWS
         &&
         i < deckViewNumFullSlots;
         y ++ ) {

        for( x = 0;
             x < DECK_VIEW_COLS
             &&
             i < deckViewNumFullSlots;
             x ++ ) {

            int  xPos  =  deckViewSlotPosX[ i - skip ];
            int  yPos  =  deckViewSlotPosY[ i - skip ];
            
            drawPiece( deckViewSlots[i].piece,
                       xPos,
                       yPos );

            if( ! deckViewSlots[i].present ) {

                drawPieceShadowOnly( deckViewSlots[i].piece,
                                     xPos,
                                     yPos );

                /* this is place-holder anyway...
                   another one lower down */
                drawPieceShadowOnly( deckViewSlots[i].piece,
                                     xPos,
                                     yPos + BOARD_SQUARE_SIZE / 2 );
                }

            if( deckViewHighlightFade[ i - skip ] > 0 ) {
                drawPieceHighlight( deckViewSlots[i].piece,
                                     xPos,
                                     yPos,
                                     deckViewHighlightFade[ i -  skip ] );
                }

            if( deckViewSlots[ i ].played ) {
                maxigin_drawResetColor();
                
                maxigin_drawSprite( deckViewPlayedCheckSprite ,
                                    xPos,
                                    yPos - 5 );
                }
            
            i ++;
            }
        
        }

    maxigin_setLanguageFontIndex( 1 );
    if( deckExhaustedFade > 0 ) {
        maxigin_drawResetColor();
        maxigin_drawSetAlpha( deckExhaustedFade );
        

        maxigin_drawLangText( lang_exhausted,
                              MAXIGIN_GAME_NATIVE_W - 42,
                              MAXIGIN_GAME_NATIVE_H / 2 + 75,
                              MAXIGIN_CENTER );
        }

    if( deckExhaustDescShowing ) {
        maxigin_drawResetColor();

        maxigin_drawLangText( lang_exhaustDesc,
                              deckViewCenterX,
                              deckViewCenterY + 90,
                              MAXIGIN_CENTER );
        }

    maxigin_setLanguageFontIndex( 0 );
    
        

    }



ChessPiece deckViewStep( int  inPageSound ) {

    /* fixme:
       also handle case where controller is used */
    
    int   pointerX;
    int   pointerY;
    int   i;
    int   skip       =  deckViewPageNumber * DECK_VIEW_VIS_SLOTS;
    int   r          =  mingin_getStepsPerSecond();
    int   deltaFade  =  ( 20 * 60 ) / r;

    char  controllerMovedSlot  =  0;

    
    if( deckViewPageNumber > 0 ) {
        if( buttonIsNewPressed( prevButton ) ) {
            maxigin_playSoundEffect( inPageSound,
                                     256 );
            deckViewPageNumber --;
            deckViewOverSlot = -1;
            unlocksCancelViewer();
            }
        }
    if( skip + DECK_VIEW_VIS_SLOTS < deckViewNumFullSlots ) {
        if( buttonIsNewPressed( nextButton ) ) {
            maxigin_playSoundEffect( inPageSound,
                                     256 );
            deckViewPageNumber ++;
            deckViewOverSlot = -1;
            unlocksCancelViewer();
            }
        }

    if( unlocksIsViewerActive() ) {
        deckViewOverSlot = -1;
        }

    skip = deckViewPageNumber * DECK_VIEW_VIS_SLOTS;

    
    if( maxigin_getPointerLocation( &pointerX,
                                    &pointerY ) ) {
        
        deckViewOverSlot = -1;
        
        for( i = 0;
             i < DECK_VIEW_VIS_SLOTS;
             i ++ ) {

            ChessPiece  p;

            if( i + skip >= deckViewNumFullSlots ) {
                break;
                }
        
            p =  deckViewSlots[ i + skip ].piece;

            if( p != noPiece ) {

                if( getPixelOverPiece( p | CHESS_WHITE,
                                       deckViewSlotPosX[i],
                                       deckViewSlotPosY[i],
                                       pointerX,
                                       pointerY ) ) {

                    deckViewOverSlot = i;
                    deckViewHighlightFade[ i ] = 255;
                    break;
                    }
                }
            }
        }
    else {
        /* examine slots with controller */

        static  char  presentMap[  DECK_VIEW_ROWS * DECK_VIEW_COLS ];

        int  pickX  =  -1;
        int  pickY  =  -1;
        
        for( i = 0;
             i < DECK_VIEW_ROWS * DECK_VIEW_COLS;
             i ++ ) {
            
            if( deckViewSlots[ i + skip ].piece != noPiece ) {

                presentMap[ i ] = 1;
                }
            else {
                presentMap[ i ] = 0;
                }
            }

        if( deckViewOverSlot != -1 ) {
            pickY = deckViewOverSlot / DECK_VIEW_COLS;
            pickX = deckViewOverSlot % DECK_VIEW_COLS;
            }

        sparseGridNav( presentMap,
                       DECK_VIEW_COLS,
                       DECK_VIEW_ROWS,
                       &pickX,
                       &pickY );

        if( pickX != -1
            &&
            pickY != -1 ) {

            int  old  =  deckViewOverSlot;
            
            deckViewOverSlot = pickY * DECK_VIEW_COLS + pickX;

            deckViewHighlightFade[ deckViewOverSlot ] = 255;

            if( old != deckViewOverSlot ) {
                controllerMovedSlot = 1;
                unlocksCancelViewer();
                }
            }
        }
    

    for( i = 0;
         i < DECK_VIEW_VIS_SLOTS;
         i ++ ) {

        if( i + skip >= deckViewNumFullSlots ) {
            break;
            }

        if( i != deckViewOverSlot
            &&
            deckViewHighlightFade[i] > 0 ) {

            int  newHighlight = deckViewHighlightFade[i] - deltaFade;

            if( newHighlight > 0 ) {
                deckViewHighlightFade[i] = (unsigned char)newHighlight;
                }
            else {
                deckViewHighlightFade[i] = 0;
                }
            }
        }

    if( ! deckExhaustedShowing
        &&
        deckExhaustedFade > 0 ) {

        int  newFade  =  deckExhaustedFade -  deltaFade;

        if( newFade > 0 ) {
            deckExhaustedFade = (unsigned char)newFade;
            }
        else {
            deckExhaustedFade = 0;
            }
        }
    

    if( controllerMovedSlot ) {
        /* return noPiece for one step, to allow piece info panel
           to fade slightly, and so that game will play sound */
        deckExhaustedShowing = 0;
        return noPiece;
        }
    
    if( deckViewOverSlot != -1 ) {

        if( deckViewSlots[ deckViewOverSlot + skip ].played ) {
            deckExhaustDescShowing = 1;
            deckExhaustedShowing   = 1;
            deckExhaustedFade      = 255;
            }
        return deckViewSlots[ deckViewOverSlot + skip ].piece;
        }
    else {
        deckExhaustedShowing = 0;
        return noPiece;
        }
    }




#endif
#endif
